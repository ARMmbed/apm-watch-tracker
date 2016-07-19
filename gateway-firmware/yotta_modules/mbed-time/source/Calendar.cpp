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


#include "mbed-time/Calendar.h"

#include <time.h>
#include <sys/time.h>


typedef enum {
  JANUARY = 0,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
} months_t;


static uint8_t clock_isleapyear(time_t year)
{
    uint8_t retval = 0;

    // year divisible by 4
    if ((year % 4) == 0)
    {
        // and year not divisible 100
        if ((year % 100) != 0)
        {
            retval = 1;
        }
        else
        {
            // unless year also divisible by 400
            if ((year % 400) == 0)
            {
                retval = 1;
            }
        }
    }

    return retval;
}

static uint8_t daysInMonth(uint8_t month, time_t year)
{
  uint8_t days;

  switch(month)
  {
    case JANUARY:
    case MARCH:
    case MAY:
    case JULY:
    case AUGUST:
    case OCTOBER:
    case DECEMBER:
                  days = 31;
                  break;
    case FEBRUARY:
                  days = 28 + clock_isleapyear(year + 1900); // function takes calendar years
                  break;
    case APRIL:
    case JUNE:
    case SEPTEMBER:
    case NOVEMBER:
    default:
                  days = 30;
                  break;
  }

  return days;
}


uint8_t Calendar::getHour()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    return timeinfo->tm_hour;
}

uint8_t Calendar::getMinute()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    return timeinfo->tm_min;
}

uint8_t Calendar::getSecond()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    return timeinfo->tm_sec;
}

void Calendar::getHour(uint8_t* hour, uint8_t* minute, uint8_t* second)
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    if (hour)
    {
      *hour = timeinfo->tm_hour;
    }

    if (minute)
    {
      *minute = timeinfo->tm_min;
    }

    if (second)
    {
      *second = timeinfo->tm_sec;
    }
}

uint8_t Calendar::getDay()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    return timeinfo->tm_wday;
}

uint8_t Calendar::getDate()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    return timeinfo->tm_mday;
}

uint8_t Calendar::getMonth()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    // months are 0-indexed
    return timeinfo->tm_mon + 1;
}

uint32_t Calendar::getYear()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    // tm_year is using 1900 as zero
    return timeinfo->tm_year + 1900;
}

void Calendar::setHour(uint8_t hour)
{
    if (hour < 24)
    {
        time_t unixtime = time(NULL);
        struct tm * timeinfo = localtime(&unixtime);

        timeinfo->tm_hour = hour;

        struct timeval tvp;
        tvp.tv_sec = mktime(timeinfo);
        settimeofday(&tvp, NULL);
    }
}

void Calendar::setMinute(uint8_t minute)
{
    if (minute < 60)
    {
        time_t unixtime = time(NULL);
        struct tm * timeinfo = localtime(&unixtime);

        timeinfo->tm_min = minute;

        struct timeval tvp;
        tvp.tv_sec = mktime(timeinfo);
        settimeofday(&tvp, NULL);
    }
}

void Calendar::setSecond(uint8_t second)
{
    if (second < 60)
    {
        time_t unixtime = time(NULL);
        struct tm * timeinfo = localtime(&unixtime);

        timeinfo->tm_sec = second;

        struct timeval tvp;
        tvp.tv_sec = mktime(timeinfo);
        settimeofday(&tvp, NULL);
    }
}

void Calendar::setDate(uint8_t date)
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    // check date integrity
    int days = daysInMonth(timeinfo->tm_mon, timeinfo->tm_year);

    if ((date >= 1) && (date <= days))
    {
        timeinfo->tm_mday = date;

        struct timeval tvp;
        tvp.tv_sec = mktime(timeinfo);
        settimeofday(&tvp, NULL);
    }
}

void Calendar::setMonth(uint8_t month)
{
    if ((month >= 1) && (month <= 12))
    {
        time_t unixtime = time(NULL);
        struct tm * timeinfo = localtime(&unixtime);

        // months are 0-indexed
        timeinfo->tm_mon = month - 1;

        // check date integrity
        int days = daysInMonth(timeinfo->tm_mon, timeinfo->tm_year);

        if (timeinfo->tm_mday > days)
        {
            timeinfo->tm_mday = days;
        }

        struct timeval tvp;
        tvp.tv_sec = mktime(timeinfo);
        settimeofday(&tvp, NULL);
    }
}

void Calendar::setYear(uint32_t year)
{
    if (year >= 1900)
    {
        time_t unixtime = time(NULL);
        struct tm * timeinfo = localtime(&unixtime);

        // tm_year is using 1900 as zero
        timeinfo->tm_year = year - 1900;

        // check date integrity
        int days = daysInMonth(timeinfo->tm_mon, timeinfo->tm_year);
        if (timeinfo->tm_mday > days)
        {
            timeinfo->tm_mday = days;
        }

        struct timeval tvp;
        tvp.tv_sec = mktime(timeinfo);
        settimeofday(&tvp, NULL);
    }
}

uint8_t Calendar::getDaysInMonth()
{
    time_t unixtime = time(NULL);
    struct tm * timeinfo = localtime(&unixtime);

    return daysInMonth(timeinfo->tm_mon, timeinfo->tm_year);
}

