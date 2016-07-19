/* mbed
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "mbed-hal/lp_ticker_api.h"

#include <sys/time.h>


static time_t unixTimeOffset __attribute__ ((section (".noinit")));//  = 1420070400; // default boot time is January 1st 2015 0:0:0 UTC


extern "C"
{
static void getTimeval(struct timeval* tvp);

// time_t time(time_t* timer)
// {
//     struct timeval globalTVP;

//     gettimeofday(&globalTVP, NULL);

//     if (timer)
//     {
//         *timer = globalTVP.tv_sec;
//     }

//     return globalTVP.tv_sec;
// }

int gettimeofday(struct timeval* globalTVP, void* tzp __attribute__((unused)))
{
    getTimeval(globalTVP);

    globalTVP->tv_sec += unixTimeOffset;

    return 0;
}

int settimeofday(const struct timeval* globalTVP, const struct timezone* tzp __attribute__((unused)))
{
    struct timeval localTVS;

    getTimeval(&localTVS);

    unixTimeOffset = globalTVP->tv_sec - localTVS.tv_sec;

    return 0;
}

static void getTimeval(struct timeval* tvp)
{
    if (tvp)
    {
        //[> overflow happens very infrequent, so we cache overflow calculations. <]
        static uint32_t overflow = 0;
        static uint64_t highTicks = 0;

        /*  read overflow counter twice to prevent inconsistency between
            tick counter and overflow counter.
        */
        uint32_t preOverflow = lp_ticker_get_overflows_counter();
        uint32_t lowTicks = lp_ticker_read();
        uint32_t postOverflow = lp_ticker_get_overflows_counter();

        /*  Overflow occured. Read tick counter again for consistency and redo
            the highTick calculation.
        */
        if (preOverflow != postOverflow)
        {
            lowTicks = lp_ticker_read();
            overflow = postOverflow;
            highTicks = ((uint64_t) overflow) << 32;
        }
        /*  The cached highTick calculation is out of date. Redo calculation. */
        else if (overflow != postOverflow)
        {
            overflow = postOverflow;
            highTicks = ((uint64_t) overflow) << 32;
        }

        uint64_t seconds = (highTicks + ((uint64_t) lowTicks)) / MINAR_PLATFORM_TIME_BASE;

        tvp->tv_sec = seconds;
        tvp->tv_usec = (uint64_t)(lowTicks % MINAR_PLATFORM_TIME_BASE) * 1000000 / MINAR_PLATFORM_TIME_BASE;
    }
}

}

namespace mbed
{
    namespace time
    {
        void saveOffset()
        {
            struct timeval localTVS;

            getTimeval(&localTVS);

            unixTimeOffset += localTVS.tv_sec;
        }
    }
}
