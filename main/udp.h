/*
 * udp.h
 *
 *  Created on: May 25, 2018
 *      Author: yuan
 */

#ifndef UDP_H_
#define UDP_H_

typedef struct send_udp{
   int  sockfd;     //socket
   char *info;          //发送的数据
   char *addr;         //发送的地址
   int port;             //发送的端口
}thread_send;

typedef struct recv_udp{
   int  sockfd;     //socket
   char *info;          //接受的数据
}thread_recdata;

unsigned int CRC16(char *updata,unsigned int len);
int CreateSocket();
void udp_hreatdatasend(int socketfd);
void udp_loginsend(int socketfd);
void recv_handle(char* recvbuf, int len);
void rfid_rec(int socketfd);

#endif /* UDP_H_ */
