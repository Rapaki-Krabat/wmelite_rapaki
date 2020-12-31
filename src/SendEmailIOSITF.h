
#ifndef SENDEMAILIOSITF_H
#define SENDEMAILIOSITF_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

void IOS_SetWindowForMailDialogue(SDL_Window* wind);

void IOS_SendMailWithAttachments(const char** logfiles, const int nrLogFiles, const char** savegames, const int nrSaveGames);

#ifdef __cplusplus
}
#endif

#endif // SENDEMAILIOSITF_H
