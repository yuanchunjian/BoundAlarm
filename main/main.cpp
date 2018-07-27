/*
 * step4_tcpclient.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: yuan
 */
/*
 * step4_tcpclient.cpp
 *
 *  Created on: 2010-3-29
 *      Author: zhl
 */
#include "CMainFrame.h"
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>
#include <linux/fs.h>
#include "../src/PublicHead.h"
#include<sys/select.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <pthread.h>
#include "ReadINI.h"
#include "uart.h"

#define VERSION "1.0.6"
#define BINNAME "BoundAlarm"

int fd_com3 = -1;
pthread_t   com3_recv_threadid = 0;
int RF_flag = 0;
void print_help(char **argv);
void print_version(void);
int Com3_Init(void);

void* com3_rec(void *arg)
{
	char rec_buff[20];
	int len = 0;
	while(1)
	{
		if(fd_com3 > 0)
		{
			len = read(fd_com3,rec_buff,sizeof(rec_buff));
			if(len > 0)
			{
				RF_flag = 1;
				printf("it's worker here\n");
			}
			else
			{
				RF_flag = 0;
				//printf("recv nothing\n");
			}
		}
		else
		{
			printf("fd_com3 < 0,break\n");
			break;
		}
	}
	pthread_exit(0);
	return NULL;
}

int main(int argc, char* argv[])
{
	int curopt;
	int ret;
	while (1) { /* Option parsing routine */
		int longopt_index = 0; /* Used only to satisfy getopt_long() */
		struct option long_options[] = {
				{ "help", 0, 0, 'h' },
				{ "version", 0, 0, 'v' },
				{ 0, 0, 0, 0 } };
		curopt = getopt_long(argc, argv, "hvk", long_options, &longopt_index);
		if (curopt < 0)
			break;
		switch (curopt) {
		case 'h':
			print_help(argv);
			exit(EXIT_SUCCESS);
		case 'v':
			print_version();
			exit(EXIT_SUCCESS);
		case '?':
			print_help(argv);
			exit(EXIT_SUCCESS);
		default:
			break;
		}
	}

	ret = Com3_Init();
	if(ret > 0)
	{
		ret = pthread_create(&com3_recv_threadid,NULL,com3_rec,NULL);
		if(ret != 0)
		{
			printf("Com3 recv pthread create failed\n");
		}
		else
		{
			printf("Com3 recv pthread create success\n");
		}
	}

	//print_version();
/**      ***/
	CMainFrame obMainFrame;
	obMainFrame.SetExePath(argv[0]);

	bool bRet = obMainFrame.InitAllPar();
	if (!bRet)
	{
		printf("初始化参数失败，停止运行.");
		exit(ABNORMAL_EXIT);
	}

	bRet= obMainFrame.CreateTcpClient();
	if (!bRet)
	{
		printf("创建通讯失败，停止运行.");
		exit(ABNORMAL_EXIT);
	}

	obMainFrame.Runing(); //TCP lms runing

	pthread_join(com3_recv_threadid,NULL);
}

/*
 * Help and version screens
 */
void print_help(char **argv) {
  printf("\
Usage: %s [-hv]\n\
  -h, --help    : this help screen\n\
  -v, --version : version information\n", argv[0]);
}

void print_version(void)
{
  	printf("%s version %s(C) SiChuan GuoRuan 2016\n", BINNAME, VERSION);
}


int Com3_Init(void)
{
	fd_com3 = open("/dev/tty03",O_RDWR);

	if(fd_com3 < 0)
	{
		printf("open /dev/ttyO3 failed\n");
	}
	else
	{
		printf("open /dev/ttyO3 success\n");
	}

	set_speed(fd_com3,9600);

	set_Parity(fd_com3,8,1,'N');

	return fd_com3;
}

