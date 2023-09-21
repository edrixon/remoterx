#include <stdio.h>
#include <edgpio.h>

#include "remoterx.h"
#include "commands.h"
#include "cmd.h"

extern int currentAntenna;

void selectAntenna(int ant)
{
    if(ant == ANTA)
    {
        currentAntenna = ANTA;

        printf("  Antenna A selected\n");
        ioWritePin(ANTA_RLY, ON);
        ioWritePin(ANTB_RLY, OFF);
        ioWritePin(ANTA_LED, ON);
        ioWritePin(ANTB_LED, OFF);
    }
    else
    {
        currentAntenna = ANTB;

        printf("  Antenna B selected\n");
        ioWritePin(ANTA_RLY, OFF);
        ioWritePin(ANTB_RLY, ON);
        ioWritePin(ANTA_LED, OFF);
        ioWritePin(ANTB_LED, ON);
    }
}

void cmdSelectAntenna(unsigned char *cmd, unsigned char *resp)
{
    switch(cmd[1])
    {
        case 'A':
            selectAntenna(ANTA);
            break;

        case 'B':
            selectAntenna(ANTB);
            break;

        default:
    }

    resp[0] = 2;
    resp[1] = RESP_OK;
    if(currentAntenna == ANTA)
    {
        resp[2] = 'A';
    }
    else
    {
        resp[2] = 'B';
    }
}

void doCommand(unsigned char *rxBuff, unsigned char *txBuff)
{
    switch(rxBuff[0])
    {
        case CMD_SELANT:
            cmdSelectAntenna(rxBuff, txBuff);
            break;

        default:
            printf("  Unknown command '%c'\n", *rxBuff);
            txBuff[0] = 1;
            txBuff[1] = RESP_ERR;
    }
}


