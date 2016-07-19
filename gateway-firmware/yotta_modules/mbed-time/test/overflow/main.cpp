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

#include "mbed-drivers/mbed.h"
#include "mbed-drivers/test_env.h"

#include "mbed-time/Calendar.h"

#include <sys/time.h>

namespace {
static time_t unixtime;
}

void endTime()
{
    MBED_HOSTTEST_RESULT(true);
}

void printTime()
{
    /* verify that exactly 1 second has elapsed */
    time_t now = time(NULL);
    MBED_HOSTTEST_ASSERT(now == unixtime + 1);
    unixtime = now;
}

void app_start(int, char *[])
{
    MBED_HOSTTEST_TIMEOUT(60 * 61);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(Overflow test);
    MBED_HOSTTEST_START("Overflow");

    Calendar::setYear(2015);
    Calendar::setMonth(12);
    Calendar::setDate(31);

    Calendar::setHour(23);
    Calendar::setMinute(59);
    Calendar::setSecond(50);

    /* test unix time */
    unixtime = time(NULL);
    MBED_HOSTTEST_ASSERT(unixtime == 1451606390);

    /* schedule tasks */
    minar::Scheduler::postCallback(printTime)
        .period(minar::milliseconds(1000));

    minar::Scheduler::postCallback(endTime)
        .period(minar::milliseconds(60 * 60 * 1000));
}
