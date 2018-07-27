#ifndef uart_H
#define uart_H

#ifdef __cplusplus
extern "C" void set_speed(int fd, int speed);

extern "C" int set_Parity(int fd,int databits,int stopbits,int parity);

extern "C" void receivethread(void);
#else
extern void set_speed(int fd, int speed);

extern int set_Parity(int fd,int databits,int stopbits,int parity);

extern void receivethread(void);
#endif

#endif
