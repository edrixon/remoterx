#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <edgpio.h>

#include "remoterx.h"

#include "millis.h"
#include "net.h"
#include "commands.h"

int resetCount;
int spareCount;

unsigned long int netTicks;
unsigned long int powerLedMillis;
unsigned long int actLedMillis;
int powerLed;

int currentAntenna;
unsigned long antChangeMSecs;

void ledTest()
{
    int led;

    printf("  LED test - ");
    for(led = 1; led <= 6; led++)
    {
        printf("  %d", led);
        fflush(stdout);

        ioWritePin(led, ON);
        msleep(100);
        ioWritePin(led, OFF);
        msleep(100);
    }
    printf("\n");
}

void hwInit()
{
    uint8_t intMask;
    int led;

    // Setup button pins
    ioSetPinDirection(RESET_SW, INPUT);
    ioSetPinPullup(RESET_SW, ENABLED);
    ioSetPinDirection(ANT_SW, INPUT);
    ioSetPinPullup(ANT_SW, ENABLED);
    ioSetPinDirection(SPARE_SW, INPUT);
    ioSetPinPullup(SPARE_SW, ENABLED);

    // Set interrupt type for buttons to be "state change"
    ioSetInterruptType(IO_PORTB, 0x00);

    // Enable interrupts
    ioSetInterruptOnPin(RESET_SW, ENABLED);
    ioSetInterruptOnPin(SPARE_SW, ENABLED);
    ioSetInterruptOnPin(ANT_SW, ENABLED);

    // Setup LED pins
    for(led = 1; led <= 6; led++)
    {
        ioSetPinDirection(led, OUTPUT);
        ioWritePin(led, OFF);
    }

    // Setup relay pins
    ioSetPinDirection(ANTA_RLY, OUTPUT);
    ioSetPinDirection(ANTB_RLY, OUTPUT);
}

void flashPowerLed()
{
    if(millis() - powerLedMillis > 50)
    {
        powerLedMillis = millis();

        if(powerLed == ON)
        {
           powerLed = OFF;
        }
        else
        {
           powerLed = ON;
        }

        ioWritePin(POWER_LED, powerLed);
    }
}

void powerLedInit()
{
    powerLedMillis = millis();
    powerLed = ON;
    ioWritePin(POWER_LED, powerLed);
}

void actLedOn()
{
    actLedMillis= millis();
    ioWritePin(ACT_LED, ON);
}

void actLedOff()
{
    if(millis() - actLedMillis > 100)
    {
        ioWritePin(ACT_LED, OFF);
    }
}

void changeAntenna()
{
    unsigned long int nowMSecs;

    nowMSecs = millis();
    if(nowMSecs - antChangeMSecs > 250)
    {
        antChangeMSecs = nowMSecs;
        if(currentAntenna == ANTA)
        {
            selectAntenna(ANTB);
        }
        else
        {
            selectAntenna(ANTA);
        }
    }
}

int main(int argc, char *argv[])
{
    uint8_t intStatus;
    uint8_t portData;
    int sockFd;
    int newSock;
    unsigned char *netRxBuff;
    unsigned char *netTxBuff;

    printf("GD6XHG remote controlled receiver\n");

    printf("  I2C - ");
    // Use bus 1 with 128 byte buffers
    i2cInit("/dev/i2c-1", 128, 128, 20);
    printf("OK\n");

    // Initialise MCP23017
    printf("  MCP23017 - ");
    ioInit(1, 0x20);
    printf("OK\n");

    // Configure IO pins
    printf("  IO pins - ");
    hwInit();
    printf("OK\n");

    printf("  IPC - ");
    sockFd = netInit(SRVPORT, NETBUFFSIZE, &netRxBuff,
                                                  NETBUFFSIZE, &netTxBuff);
    if(sockFd == -1)
    {
        printf("FAIL\n");
        exit(-1);
    }

    printf("OK\n");

    // Select antenna "A"
    currentAntenna = -1;
    selectAntenna(ANTA);

    // Get ready LED ready..
    powerLedInit();

    printf("Ready\n");

    resetCount = 0;
    spareCount = 0;

    while(1)
    {
        flashPowerLed();

        // Check interrupt status port B
        // If antenna change switch pressed
        // then swap antenna
        intStatus = ioReadInterruptStatus(IO_PORTB);

        if(intStatus != 0)
        {
            // Clear interrupts but save pin data
            portData = ioReadPort(IO_PORTB);
        }

        // Check for antenna changeover
        // Interrupt caused by antenna changeover switch
        // and button still pressed
        if((intStatus & TST_ANT_SW) == TST_ANT_SW)
        {
            if((portData & TST_ANT_SW) != TST_ANT_SW)
            {
                changeAntenna();
            }
        }

        // Check for "spare" switch"
        if((intStatus & TST_SPARE_SW) == TST_SPARE_SW)
        {
            if((portData & TST_SPARE_SW) != TST_SPARE_SW)
            {
                spareCount++;
                printf("*** SPARE SWITCH PRESSED - %d\n", spareCount);
            }
        }

        // Check for system reset
        if((intStatus & TST_RESET_SW) == TST_RESET_SW)
        {
            if((portData & TST_RESET_SW) != TST_RESET_SW)
            {
                resetCount++;
                printf("*** RESET SWITCH PRESSED - %d\n", resetCount);
            }
        }

        if(netGetMsg(sockFd, &newSock, NETBUFFSIZE, netRxBuff) != 0)
        {
            actLedOn();
            doCommand(netRxBuff, netTxBuff);

            netSendMsg(newSock, netTxBuff[0], &netTxBuff[1]);

            close(newSock);
        }

        actLedOff();

        msleep(20);
    }

    return 0;
}
