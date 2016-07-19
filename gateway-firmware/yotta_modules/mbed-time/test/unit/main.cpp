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


void app_start(int, char *[])
{
    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(Unit test);
    MBED_HOSTTEST_START("Unit");

    /* test year */
    Calendar::setYear(2015);
    uint32_t year  = Calendar::getYear();
    MBED_HOSTTEST_ASSERT(year == 2015);

    /* test month */
    Calendar::setMonth(12);
    uint8_t month  = Calendar::getMonth();
    MBED_HOSTTEST_ASSERT(month == 12);

    /* test date */
    Calendar::setDate(31);
    uint8_t date   = Calendar::getDate();
    MBED_HOSTTEST_ASSERT(date == 31);

    /* test hour */
    Calendar::setHour(23);
    uint8_t hour   = Calendar::getHour();
    MBED_HOSTTEST_ASSERT(hour == 23);

    /* test minute */
    Calendar::setMinute(59);
    uint8_t minute = Calendar::getMinute();
    MBED_HOSTTEST_ASSERT(minute == 59);

    /* test second */
    Calendar::setSecond(50);
    uint8_t second = Calendar::getSecond();
    MBED_HOSTTEST_ASSERT(second == 50);

    /* test unix time */
    time_t unixtime = time(NULL);
    MBED_HOSTTEST_ASSERT(unixtime == 1451606390);

    /* set/get unix time */
    struct timeval setTvs;
    setTvs.tv_sec = 1234567890;
    settimeofday(&setTvs, NULL);

    struct timeval getTvs;
    gettimeofday(&getTvs, NULL);

    MBED_HOSTTEST_ASSERT(setTvs.tv_sec == getTvs.tv_sec);

    /* all asserts passed. set result to true. */
    MBED_HOSTTEST_RESULT(true);
}
