cmake_minimum_required(VERSION 3.11)

# TBD should be compatible to old MSVC CMake versions, but maybe a higher policy is still required
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(wmelite VERSION 1.0
        DESCRIPTION "WMELite game engine"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib/echo)
add_subdirectory(lib/effects)
add_subdirectory(lib/zlib)
add_subdirectory(lib/freeimage)
add_subdirectory(lib/freetype)
add_subdirectory(lib/SDL2)
add_subdirectory(lib/SDL_mixer)

add_executable(wmelite
src/AdActor.cpp 
src/AdActorDir.cpp 
src/AdEntity.cpp 
src/AdGame.cpp 
src/AdInventoryBox.cpp 
src/AdInventory.cpp 
src/AdItem.cpp 
src/AdLayer.cpp 
src/AdNodeState.cpp 
src/AdObject.cpp 
src/AdPath.cpp 
src/AdPathPoint.cpp 
src/AdRegion.cpp 
src/AdResponseBox.cpp 
src/AdResponseContext.cpp 
src/AdResponse.cpp 
src/AdRotLevel.cpp 
src/AdScaleLevel.cpp 
src/AdScene.cpp 
src/AdSceneNode.cpp 
src/AdSceneState.cpp 
src/AdSentence.cpp 
src/AdSpriteSet.cpp 
src/AdTalkDef.cpp 
src/AdTalkHolder.cpp 
src/AdTalkNode.cpp 
src/AdWaypointGroup.cpp 
src/BActiveRect.cpp 
src/BBase.cpp 
src/BDebugger.cpp 
src/BDiskFile.cpp 
src/BDynBuffer.cpp 
src/BEvent.cpp 
src/BFader.cpp 
src/BFile.cpp 
src/BFileEntry.cpp 
src/BFileManager.cpp 
src/BFontBitmap.cpp 
src/BFont.cpp 
src/BFontStorage.cpp 
src/BFontTT.cpp 
src/BFrame.cpp 
src/BGame.cpp 
src/BImage.cpp 
src/BKeyboardState.cpp 
src/BNamedObject.cpp 
src/BObject.cpp 
src/BPackage.cpp 
src/BParser.cpp 
src/BPersistMgr.cpp 
src/BPkgFile.cpp 
src/BPoint.cpp 
src/BQuickMsg.cpp 
src/BRegion.cpp 
src/BRegistry.cpp 
src/BRenderer.cpp 
src/BRenderSDL.cpp 
src/BResourceFile.cpp 
src/BResources.cpp 
src/BSaveThumbFile.cpp 
src/BSaveThumbHelper.cpp 
src/BScriptable.cpp 
src/BScriptHolder.cpp 
src/BSound.cpp 
src/BSprite.cpp 
src/BStringTable.cpp 
src/BSubFrame.cpp 
src/BSurface.cpp 
src/BSurfaceSDL.cpp 
src/BSurfaceStorage.cpp 
src/BTransitionMgr.cpp 
src/BViewport.cpp 
src/crc.cpp 
src/FontGlyphCache.cpp 
src/InAppSimulator.cpp 
src/main.cpp 
src/MathUtil.cpp 
src/Matrix4.cpp 
src/PartEmitter.cpp 
src/PartForce.cpp 
src/PartParticle.cpp 
src/PathUtil.cpp 
src/PlatformSDL.cpp 
src/ScEngine.cpp 
src/ScScript.cpp 
src/ScStack.cpp 
src/ScValue.cpp 
src/SdlUtil.cpp 
src/StringUtil.cpp 
src/SXArray.cpp 
src/SXDate.cpp 
src/SXFile.cpp 
src/SXMath.cpp 
src/SXMemBuffer.cpp 
src/SxObject.cpp 
src/SXStore.cpp 
src/SXString.cpp 
src/SysClass.cpp 
src/SysClassRegistry.cpp 
src/SysInstance.cpp 
src/tinyxml.cpp 
src/tinyxmlerror.cpp 
src/tinyxmlparser.cpp 
src/UIButton.cpp 
src/UIEdit.cpp 
src/UIEntity.cpp 
src/UIObject.cpp 
src/UIText.cpp 
src/UITiledImage.cpp 
src/UIWindow.cpp 
src/utils.cpp 
src/Vector2.cpp 
src/snd_SDL_mixer/BSoundBuffer.cpp
src/snd_SDL_mixer/BSoundMgr.cpp
src/LicenseCheckDummy.c
src/FileOperations.c
src/DirectoryOperations.c
src/ConvertUTF.c
)

target_link_libraries(wmelite wmelite_echo wmelite_effects wmelite_freeimage freetype SDL2 SDL_mixer zlib)

target_compile_definitions(wmelite PRIVATE -DUSE_SDL_MIXER)
target_include_directories(wmelite PRIVATE ./)
