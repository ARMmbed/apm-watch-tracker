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


#ifndef __CALENDAR_H__
#define __CALENDAR_H__

#include <stdint.h>

class Calendar
{
public:
    static uint8_t getHour();
    static uint8_t getMinute();
    static uint8_t getSecond();
    static void getHour(uint8_t* hour, uint8_t* minute, uint8_t* second);

    static uint8_t getDay();
    static uint8_t getDate();
    static uint8_t getMonth();
    static uint32_t getYear();

    static void setHour(uint8_t hour);
    static void setMinute(uint8_t minute);
    static void setSecond(uint8_t second);

    static void setDate(uint8_t date);
    static void setMonth(uint8_t month);
    static void setYear(uint32_t year);

    static uint8_t getDaysInMonth();
};

#endif // __CALENDAR_H__
