#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>

#include "net.h"

int netInit(int portNumber, int rBuffSize, unsigned char **rBuff,
                                          int tBuffSize, unsigned char **tBuff)
{
    int sock;
    struct sockaddr_in addr;

    *rBuff = malloc(rBuffSize);
    if(rBuff == NULL)
    {
        perror("malloc rBuff");
        return -1;
    }

    *tBuff = malloc(tBuffSize);
    if(tBuff == NULL)
    {
        perror("malloc tBuff");
        return -1;
    }

    sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(sock == -1)
    {
        perror("socket");
        return -1;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
    if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
        perror("bind");
        return -1;
    }

    if(listen(sock, 5) == -1)
    {
        perror("listen");
        return -1;
    }

    return sock;
}

int netGetMsg(int sock, int *newSock, int length, unsigned char *rxBuff)
{
    int rxLength;

    *newSock = accept(sock, NULL, NULL);
    if(*newSock == -1)
    {
        if(errno == EWOULDBLOCK)
        {
            return 0;
        }

        perror("accept");
        return -1;
    }

    rxLength = recv(*newSock, rxBuff, length, 0);
    if(rxLength == -1)
    {
        perror("recv");
        return -1;
    }

    return rxLength;
}

int netSendMsg(int sock, int length, unsigned char *txBuff)
{
    if(send(sock, txBuff, length, 0) == -1)
    {
        perror("send");
        return -1;
    }

    return 0;
}
