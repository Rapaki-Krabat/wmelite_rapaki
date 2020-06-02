/*
This file is part of WME Lite.
http://dead-code.org/redir.php?target=wmelite

Copyright (c) 2011 Jan Nedoma

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "dcgf.h"
#include "BDiskFile.h"
#include "PathUtil.h"

#include <memory>

//////////////////////////////////////////////////////////////////////////
CBDiskFile::CBDiskFile(CBGame* inGame):CBFile(inGame)
{
	m_File = NULL;
	m_Compressed = false;
	m_PrefixSize = 0;
}


//////////////////////////////////////////////////////////////////////////
CBDiskFile::~CBDiskFile()
{
	Close();	
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Open(const char* Filename)
{
	Close();

	char FullPath[MAX_PATH];

	for(int i=0; i<Game->m_FileManager->m_SinglePaths.GetSize(); i++)
	{
		sprintf(FullPath, "%s%s", Game->m_FileManager->m_SinglePaths[i], Filename);
		CorrectSlashes(FullPath);

		// re-check the access method for every path
		ops = PathUtil::GetFileAccessMethod(FullPath);

		m_File = ops->file_open(FullPath, "rb");
		if(m_File!=NULL) break;
	}

	// if we didn't find it in search paths, try to open directly
	if(!m_File)
	{
		strcpy(FullPath, Filename);
		CorrectSlashes(FullPath);
		ops = PathUtil::GetFileAccessMethod(FullPath);
		m_File = ops->file_open(FullPath, "rb");
	}

	if(m_File)
	{
		DWORD magic1, magic2;
		ops->file_read((char *) (&magic1), sizeof(DWORD), m_File);
		ops->file_read((char *) (&magic2), sizeof(DWORD), m_File);

		if(magic1==DCGF_MAGIC && magic2==COMPRESSED_FILE_MAGIC) m_Compressed = true;

		if(m_Compressed)
		{
			DWORD DataOffset, CompSize, UncompSize;
			int status;
			long readSize;

			ops->file_read((char *) (&DataOffset), sizeof(DWORD), m_File);
			ops->file_read((char *) (&CompSize), sizeof(DWORD), m_File);
			ops->file_read((char *) (&UncompSize), sizeof(DWORD), m_File);

			// Game->LOG(0, "File %s is compressed, size=%d!", Filename, CompSize);

			std::unique_ptr<BYTE> CompBuffer{ new BYTE[CompSize] };
			if(!CompBuffer)
			{
				Game->LOG(0, "Error allocating memory for compressed file '%s'", Filename);
				Close();
				return E_FAIL;
			}

			m_Data = std::make_unique<BYTE[]>(UncompSize);
			if(!m_Data)
			{
				Game->LOG(0, "Error allocating buffer for file '%s'", Filename);
				Close();
				return E_FAIL;
			}

			status = ops->file_seek(m_File, DataOffset + m_PrefixSize, SEEK_SET);
			if (status != 0)
			{
				Game->LOG(0, "Seeking in file %s failed!", Filename);
			}
			readSize = ops->file_read((char *) CompBuffer.get(), CompSize, m_File);
			if (readSize != CompSize)
			{
				Game->LOG(0, "Reading file %s failed! Expected %d but read %d", Filename, CompSize, readSize);
			}

			int uncompStatus;

			// 64-bit safety
			uLongf SafeCompSize;
			uLongf SafeUncompSize;

			SafeCompSize   = CompSize;
			SafeUncompSize = UncompSize;

			uncompStatus = uncompress(m_Data.get(), &SafeUncompSize, (Bytef*) CompBuffer.get(), SafeCompSize);

			// Game->LOG(0, "Uncompressed size from zlib: %d", SafeUncompSize);

			if(uncompStatus != Z_OK)
			{
				Game->LOG(0, "Error uncompressing file '%s' status=%d", Filename, uncompStatus);
				Close();
				return E_FAIL;
			}

			m_Size = UncompSize;
			m_Pos = 0;
			ops->file_close(m_File);
			m_File = NULL;

			// Game->LOG(0, "File uncompress ok, uncompressed size=%d", UncompSize);
		}
		else
		{
			// Game->LOG(0, "File %s is NOT compressed!", Filename);

			m_Pos = 0;
			ops->file_seek(m_File, 0, SEEK_END);
			m_Size = ((DWORD) ops->file_tell(m_File)) - m_PrefixSize;
			ops->file_seek(m_File, m_PrefixSize, SEEK_SET);
		}

		return S_OK;
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Close()
{
	if(m_File) ops->file_close(m_File);
	m_File = NULL;
	m_Pos = 0;
	m_Size = 0;

	m_Compressed = false;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Read(void *Buffer, DWORD Size)
{
	if(m_Compressed)
	{
		memcpy(Buffer, m_Data.get()+m_Pos, Size);
		m_Pos+=Size;
		return S_OK;
	}
	else
	{
		if(m_File)			
		{
			size_t count = ops->file_read((char *) Buffer, Size, m_File);
			if (count == 0) 
				return E_FAIL;
			m_Pos += count;
			return S_OK;
		}
		else return E_FAIL;
	}
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Seek(LONG Pos, TSeek Origin)
{
	if(m_Compressed)
	{
		DWORD NewPos=0;

		switch(Origin)
		{
			case SEEK_TO_BEGIN:   NewPos = Pos;          break;
			case SEEK_TO_END:     NewPos = m_Size + Pos; break;
			case SEEK_TO_CURRENT: NewPos = m_Pos + Pos;  break;
		}

		if(NewPos > m_Size) return E_FAIL;
		else m_Pos = NewPos;
		return S_OK;
	}
	else
	{
		if(!m_File) return E_FAIL;

		int ret=1;

		switch(Origin)
		{
			case SEEK_TO_BEGIN:   ret = ops->file_seek(m_File, m_PrefixSize + Pos, SEEK_SET); break;
			case SEEK_TO_END:     ret = ops->file_seek(m_File, Pos, SEEK_END); break;
			case SEEK_TO_CURRENT: ret = ops->file_seek(m_File, Pos, SEEK_CUR); break;
		}
		if(ret==0)
		{
			m_Pos = ((DWORD) ops->file_tell(m_File)) - m_PrefixSize;
			return S_OK;
		}
		else return E_FAIL;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBDiskFile::CorrectSlashes(char* fileName)
{
	for (size_t i = 0; i < strlen(fileName); i++)
	{
		if (fileName[i] == '\\') fileName[i] = '/';
	}
}
