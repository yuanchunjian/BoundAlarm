#ifndef _ReadINI_H
#define _ReadINI_H

#define CONF_FILE_PATH  "FtpServer.ini"
#define  MAX_PATH 260
/*
int GetCurrentPath(char buf[],char *pFileName);

char *GetIniKeyString(char *title,char *key,char *filename);

int GetIniKeyInt(char *title,char *key,char *filename);*/

#ifdef __cplusplus

extern "C" int GetCurrentPath(char buf[],char *pFileName);

extern "C" char *GetIniKeyString(char *title,char *key,char *filename);

extern "C" int GetIniKeyInt(char *title,char *key,char *filename);
#else
extern int GetCurrentPath(char buf[],char *pFileName);

extern char *GetIniKeyString(char *title,char *key,char *filename);

extern int GetIniKeyInt(char *title,char *key,char *filename);
#endif

#endif
