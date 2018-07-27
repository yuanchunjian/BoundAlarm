#ifndef uart_H
#define uart_H



#ifdef __cplusplus
void set_speed(int fd, int speed);

int set_Parity(int fd,int databits,int stopbits,int parity);

void receivethread(void);

#endif

#endif
