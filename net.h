#ifndef __GOT_NET
#define __GOT_NET

int netInit(int portNumber, int rBuffSize, unsigned char **rBuff,
                                         int tBuffSize, unsigned char **tBuff);
int netGetMsg(int sock, int *newSock, int length, unsigned char *rxBuff);
int netSendMsg(int sock, int length, unsigned char *txBuff);

#define SRVPORT 4500

#endif
