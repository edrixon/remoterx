#ifndef __GOT_REMOTERX
#define __GOT_REMOTERX

#define POWER_LED    1
#define ACT_LED      2
#define SPARE_LED1   3
#define SPARE_LED2   4
#define ANTA_LED     5
#define ANTB_LED     6

#define ANTA_RLY    11
#define ANTB_RLY    12
#define SPARE_RLY   13

#define ANTA_MASK   0x04
#define ANTB_MASK   0x08

#define RESET_SW    14
#define ANT_SW      15
#define SPARE_SW    16

#define TST_RESET_SW 0x20
#define TST_ANT_SW   0x40
#define TST_SPARE_SW 0x80

#define PULSE_MS    50

#define ANTA        0
#define ANTB        1

#define NETBUFFSIZE 64

#endif
