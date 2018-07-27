
#include <string.h>
#include "ReadINI.h"
 
#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else
  
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#endif
  
//获取当前程序目录
int GetCurrentPath(char buf[],char *pFileName)
{
#ifdef WIN32
    GetModuleFileName(NULL,buf,MAX_PATH); 
#else
    char pidfile[64];
    int bytes;
    int fd;
 
    sprintf(pidfile, "/proc/%d/cmdline", getpid());
 
    fd = open(pidfile, O_RDONLY, 0);
    bytes = read(fd, buf, 256);
    close(fd);
    buf[MAX_PATH] = '\0';
 
#endif
    char * p = &buf[strlen(buf)];
    do
    {
        *p = '\0';
        p--;
#ifdef WIN32
    } while( '\\' != *p );
#else
    } while( '/' != *p );
#endif
 
    p++;
 
    //配置文件目录
    memcpy(p,pFileName,strlen(pFileName));
    return 0;
}
 
//从INI文件读取字符串类型数据
char *GetIniKeyString(char *title,char *key,char *filename) 
{ 
    FILE *fp; 
    char szLine[1024];
    static char tmpstr[1024];
    int rtnval;
    int i = 0; 
    int flag = 0; 
    char *tmp;
 
    if((fp = fopen(filename, "r")) == NULL) 
    { 
        printf("have   no   such   file \n");
        return ""; 
    }
    while(!feof(fp)) 
    { 
        rtnval = fgetc(fp); 
        if(rtnval == EOF) 
        { 
            break; 
        } 
        else
        { 
            szLine[i++] = rtnval; 
        } 
        if(rtnval == '\n') 
        { 
#ifndef WIN32
            i--;
#endif  
            if (szLine[i-1] == '\r')
              szLine[--i] = '\0';
            else
              szLine[i] = '\0';
            i = 0; 
            tmp = strchr(szLine, '='); 
 
            if(( tmp != NULL )&&(flag == 1)) 
            { 
                if(strstr(szLine,key)!=NULL) 
                { 
                    //注释行
                    if ('#' == szLine[0])
                    {
                    }
                    else if ( '\/' == szLine[0] && '\/' == szLine[1] )
                    {
                         
                    }
                    else
                    {
                        //找打key对应变量
                        strcpy(tmpstr,tmp+1); 
                        fclose(fp);
                        return tmpstr; 
                    }
                } 
            }
            else
            { 
                strcpy(tmpstr,"["); 
                strcat(tmpstr,title); 
                strcat(tmpstr,"]");
                if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 ) 
                {
                    //找到title
                    flag = 1; 
                }
            }
        }
    }
    fclose(fp); 
    return ""; 
}
 
//从INI文件读取整类型数据
int GetIniKeyInt(char *title,char *key,char *filename)
{
    return atoi(GetIniKeyString(title,key,filename));
}
