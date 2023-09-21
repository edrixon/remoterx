#include <unistd.h>
#include <time.h>

#include "millis.h"

unsigned long int millis()
{
    struct timespec tSpec;
    unsigned long int ms;

    clock_gettime(CLOCK_REALTIME, &tSpec);

    ms = 1000 * tSpec.tv_sec;
    ms = ms + (tSpec.tv_nsec / 1000000);

    return ms;
}

void msleep(int ms)
{
    usleep(ms * 1000);
}
