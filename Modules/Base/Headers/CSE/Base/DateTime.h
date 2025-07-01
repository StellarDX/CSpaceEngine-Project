/////////////////////////////////////////////////////////////
//                                                         //
//         Time and Epoch module for CSpaceEngine          //
//                                                         //
// ------------------------------------------------------- //
// This file includes some general calendar functions      //
// Some of them taken from Stellarium (GPLv2 Lic.)         //
//                                                         //
// Time classes is similar to Qt's time class.             //
// But these are more simple to use.                       //
//                                                         //
/////////////////////////////////////////////////////////////

/*
    CSpaceEngine Astronomy Library
    Copyright (C) StellarDX Astronomy.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef _CSE_DATETIME_
#define _CSE_DATETIME_

#include <CSE/Base/CSEBase.h>
#include <CSE/Base/GLTypes.h>
#include <ctime>

#if _USE_BOOST_REGEX
#include <boost/regex.hpp>
#else
#include <regex>
#endif

// Namespace definations
#define _TIME_BEGIN namespace Epoch{
#define _TIME Epoch::
#define _TIME_END }

_CSE_BEGIN

_TIME_BEGIN

extern const _REGEX_NS regex ISO8601TimeStringRegex;
extern const _REGEX_NS regex SEDateTimeStringRegex;
extern const _REGEX_NS regex SEDateStringRegex;
extern const _REGEX_NS regex SETimeStringRegex;

class CSEDate
{
    int years = -1;         /*!< Years. All values are valid */
    int months = -1;        /*!< Months. Valid values : 1 (January) - 12 (December) */
    int days = -1;          /*!< Days. Valid values 1 - 28,29,30,31 Depends on month.*/

public:
    CSEDate(int y, int m, int d);
    CSEDate() {}

    bool IsNull() const;
    bool IsValid() const;

    int year() const;
    int month() const;
    int day() const;

    CSEDate AddDays(int ndays) const; // Returns a Date object containing a date ndays later than the date of this object (or earlier if ndays is negative).
    CSEDate AddMonths(int nmonths) const;
    CSEDate AddYears(int nyears) const;

    void GetDate(int* year, int* month, int* day) const; // Extracts the date's year, month, and day, and assigns them to *year, *month, and *day. The pointers may be null.
    bool SetDate(int year, int month, int day);
    ustring ToString(cstring _Fmt = "{}.{:02}.{:02}")const;

    /**
     * @brief - Return a day number of week for date
     * @return - number of day: 0 - sunday, 1 - monday,..
     */
    int DayOfWeek() const;

    /**
     * @brief Compute Julian day number from calendar date.
     * Dates before 1582-10-15 are in the Julian Calendar.
     * @deprecated Consider using "GetJDFromDate" instead.
     */
    int64 ToJulianDay() const;

    static CSEDate CurrentDate();

    /**
     * @brief
     * Make from julianDay a year, month, day for the Julian Date julianDay represents.
     * (This function is taken from Stellarium.)
     * @param JD - Input julian date
     * @return Date with YMD
     */
    static CSEDate FromJulianDay(const double JD);

    /**
     * @brief return true if a year is leap year.
     */
    bool IsLeap();
    static bool IsLeap(int year);

    static bool IsValid(int year, int month, int day);
};

// Remakes of gmtime and localtime
tm gmtime_remake(time_t timer);
tm localtime_remake(time_t timer);

class CSETime
{
    int hours = -1;         /*!< Hours. Valid values 0 - 23. */
    int minutes = -1;       /*!< Minutes. Valid values 0 - 59. */
    int seconds = -1;       /*!< Seconds. Valid values 0 - 59 */
    int msecs = -1;         /*!< MilliSeconds. Valid values 0 - 999 */

public:
    CSETime(int h, int m, int s = 0, int ms = 0);
    CSETime(int h, int m, double s = 0);
    CSETime() {}

    bool IsNull() const;
    bool IsValid() const;

    int hour() const;
    int minute() const;
    int second() const;
    int msec() const;

    CSETime AddMSecs(int ms, int* AddDays = nullptr) const;
    CSETime AddSecs(int s, int* AddDays = nullptr) const;
    int MSecsTo(CSETime right);

    bool SetHMS(int h, int m, int s, int ms = 0);
    ustring ToString(cstring _Fmt = "{:02}:{:02}:{:02}.{:03}");

    static bool IsValid(int h, int m, int s, int ms = 0);
    static CSETime CurrentTime(); // Returns the current time as reported by the system clock.
};

struct CSETimezone;

_TIME_END

class CSEDateTime
{
    double OffsetSecs = 0;
    _TIME CSEDate _Date;
    _TIME CSETime _Time;

public:
    CSEDateTime(const _TIME CSEDate& Date, const _TIME CSETime& Time, const _TIME CSETimezone& TimeZone); // Constructs a datetime with the given date and time, using the Time Zone
    CSEDateTime(const _TIME CSEDate& Date, const _TIME CSETime& Time, const double& OffsetSec = 0.0);
    CSEDateTime(const _TIME CSEDate& Date);
    CSEDateTime() {} // Constructs a null datetime

    _TIME CSEDate date() const { return _Date; }
    _TIME CSETime time() const { return _Time; }
    long TimeZone();

    CSEDateTime	AddDays(int ndays) const;
    CSEDateTime	AddMSecs(int msecs) const;
    CSEDateTime	AddMonths(int nmonths) const;
    CSEDateTime	AddSecs(int s) const;
    CSEDateTime	AddYears(int nyears) const;

    bool IsNull() const;
    bool IsValid() const;

    float64 OffsetFromUTC() const; // Returns this date-time's Offset From UTC in seconds.
    void SetDate(const _TIME CSEDate& date);
    void SetTime(const _TIME CSETime& time);
    void SetOffsetFromUTC(int OffsetSeconds);
    void SetTimeZone(const _TIME CSETimezone& ToZone);

    CSEDateTime ToUTC() const;

    static CSEDateTime CurrentDateTime(_TIME CSETimezone* ExportTimezone = nullptr);
    static CSEDateTime CurrentDateTimeUTC();

    // Convert to string, default using ISO8601 string format.
    ustring ToString(cstring _Fmt = "{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}{:+03}:{:02}")const;
};

struct _TIME CSETimezone
{
    long         Bias;
    std::wstring StandardName;
    CSEDateTime  StandardDate;
    long         StandardBias;
    std::wstring DaylightName;
    CSEDateTime  DaylightDate;
    long         DaylightBias;
};

// Stellarium Functions for advanced features of astronomical calendar.

// Reference:
// Chéreau, F., Wolf, A., Zotti, G., Hoffmann,
// S., Kabatsayev, R., Boonplod, W., Chéreau,
// G., & Bernardi, M. (2023). Stellarium
// (Version 23.4) [Computer software].
// https://github.com/Stellarium/stellarium

// Stellarium is available under the GPL-2.0 license below:
/*
 * Stellarium
 * Copyright (C) 2002 Fabien Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

// Original functions are here:
// https://github.com/Stellarium/stellarium/blob/master/src/core/StelUtils.cpp

/**
 * @brief Convert a Julian Day number to a CSEDateTime.
 * @param JD Julian Day number (with fractions) to convert
 * @result the matching CSEDateTime
 * @note StellarDX: A year "zero" (0 A.D.) is not exist in
 * actual calendar (1 B.C. is directly followed by 1 A.D.).
 * However, ISO 8601 and some astronomical calendar defines
 * the 1 B.C. as 0 year and -1 year is 2 B.C. hense. This
 * difference maybe cause problems in computer programming
 * such as QDateTime in Qt has no year 0 may limit the
 * applicability of program parts which use this method to
 * positive years.
 * CSEDateTime has no consideration of this problem previously
 * and used ISO 8601 standard when designing. But now, we
 * have to think about this problem.
 * @link https://en.wikipedia.org/wiki/Year_zero
 */
CSEDateTime JDToDateTime(const double& JD);

/**
 * @brief Extract from julianDay a year, month, day for the Julian Date julianDay represents.
 */
void GetDateFromJulianDay(const double jd, int *yy, int *mm, int *dd);

/**
 * @brief Extract from julianDay an hour, minute, second.
 */
void GetTimeFromJulianDay(const double julianDay, int *hour, int *minute, int *second, int *millis, bool *wrapDay = nullptr);

/**
 * @brief Extract from julianDay a year, month, day, hour, minute, second
 * and (optional) millisecond for the Julian Day julianDay represents.
 * This is the preferred method of complete decoding of a Julian day number.
 */
void GetDateTimeFromJulianDay(const double julianDay, int *year, int *month, int *day, int *hour, int *minute, int *second, int *millis);

/**
 * @brief Make hours (decimal format) from julianDay
 */
double GetHoursFromJulianDay(const double julianDay);

/**
 * @brief Format the given Julian Day in (UTC) ISO8601 date string.
 * Also handles negative and distant years.
 */
ustring JulianDayToISO8601String(const double jd, bool addMS = 1);

/**
 * @brief Return a day number of week for date
 * @return number of day: 0 - sunday, 1 - monday,..
 */
int GetDayOfWeek(double JD);
int GetDayOfWeek(int year, int month, int day);

/**
 * @brief Get the current Julian Date from system time.
 * @return the current Julian Date
 */
double GetJDFromSystem();

/**
 * @brief Get the Julian Day Number (JD) from Besselian epoch.
 * @param epoch Besselian epoch, expressed as year
 * @return Julian Day number (JD) for B<Year>
 */
double GetJDFromBesEpoch(const double Epoch);

/**
 * @brief Convert a time of day to the fraction of a Julian Day.
 * Note that a Julian Day starts at 12:00, not 0:00, and
 * so 12:00 == 0.0 and 0:00 == 0.5
 */
double TimeToJDFract(const _TIME CSETime& Time);

/**
 * @brief Convert a fraction of a Julian Day to a CSETime
 */
_TIME CSETime JDFractToTime(const double jd);

/**
 * @brief Compute Julian day number from calendar date.
 * Uses QDate functionality if possible, but also works for negative JD.
 * Dates before 1582-10-15 are in the Julian Calendar.
 * @param newjd pointer to JD
 * @param y Calendar year.
 * @param m month, 1=January ... 12=December
 * @param d day
 * @param h hour
 * @param min minute
 * @param s second
 * @result true in all conceivable cases.
 */
bool GetJDFromDate(double* newjd, const int y, const int m, const int d, const int h, const int min, const double s);

int NumOfDaysInMonthInYear(const int month, const int year);

/**
 * @result true if year is a leap year. Observes 1582 switch from Julian to Gregorian Calendar.
 */
bool IsLeapYear(const int year);

/**
 * Find day number for date in year.
 * Meeus, Astronomical Algorithms 2nd ed., 1998, ch.7, p.65
 */
int DayInYear(const int year, const int month, const int day);

/**
 * @brief Return a fractional year like YYYY.ddddd. For negative years, the year number is decreased. E.g. -500.5 occurs in -501.
 */
double YearFraction(const int year, const int month, const double day);

/**
 * @brief
 */
ivec3 DateFromDayYear(const int day, const int year);

/**
 * @brief Return the Julian Date matching the ISO8601 date string.
 * Also handles negative and distant years.
 */
int GetJulianDayFromISO8601String(const std::string& iso8601Date, double* jd);

/**
 * @brief Parse an ISO8601 date string.
 * Also handles negative and distant years.
 */
int GetDateTimeFromISO8601String(const std::string& iso8601Date, int* y, int* m, int* d, int* h, int* min, double* s, int* offsec);

_CSE_END

#endif
