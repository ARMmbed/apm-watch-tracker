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

#include <sys/time.h>


uint8_t counter = 0;

void printTime()
{
    printf("*");
    fflush(stdout);
}

void app_start(int, char *[])
{
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(wait_us_auto);
    MBED_HOSTTEST_DESCRIPTION(Accuracy test);
    MBED_HOSTTEST_START("Accuracy");

    /* schedule tasks */
    minar::Scheduler::postCallback(printTime)
        .period(minar::milliseconds(1000));
}
