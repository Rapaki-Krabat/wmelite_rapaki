
#include "SendEMail.h"

#include "SendEmailIOSITF.h"

#include "PathUtil.h"

#include "SDL.h"

void SendEmail::SendMailWithAttachments(const AnsiString& logfileDir, const AnsiString& savegameDir)
{
    AnsiStringList logFileList;
    PathUtil::GetFilesInDirectory(logfileDir, AnsiString("*") + AnsiString(".log"), logFileList);

    AnsiStringList saveGameList;
    PathUtil::GetFilesInDirectory(savegameDir, AnsiString("*") + AnsiString(".dsv"), saveGameList);

    ssize_t logFileListLength = logFileList.size();
    char** logFiles = new char*[logFileListLength];
    auto logFileIter = logFileList.begin();
    for (ssize_t i = 0; i < logFileListLength; i++)
    {
        AnsiString fullFileName = logfileDir + "/" + *logFileIter;
        
        SDL_Log("Prepare log file name '%s' at position %zd.\n",
                fullFileName.c_str(), i);
        
        size_t logFileNameLength = strlen(fullFileName.c_str());
        logFiles[i] = new char[logFileNameLength + 1]();
        strcpy(logFiles[i], fullFileName.c_str());
        
        logFileIter++;
    }
    
    ssize_t savegameFileListLength = saveGameList.size();
    char** savegameFiles = new char*[savegameFileListLength];
    auto savegameFileIter = saveGameList.begin();
    for (ssize_t i = 0; i < savegameFileListLength; i++)
    {
        AnsiString fullFileName = savegameDir + "/" + *savegameFileIter;

        SDL_Log("Prepare savegame file name '%s' at position %zd.\n",
                fullFileName.c_str(), i);
        
        size_t savegameFileNameLength = strlen(fullFileName.c_str());
        savegameFiles[i] = new char[savegameFileNameLength + 1]();
        strcpy(savegameFiles[i], fullFileName.c_str());
        
        savegameFileIter++;
    }
    
    IOS_SendMailWithAttachments(logFiles, (int) logFileListLength, savegameFiles, (int) savegameFileListLength);
    
    // TODO don't forget to deallocate!
}
