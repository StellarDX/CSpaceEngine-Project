/////////////////////////////////////////////////////////////
//                                                         //
//         Time and Epoch module for CSpaceEngine          //
//                                                         //
// ------------------------------------------------------- //
// This file includes some general calendar functions      //
// Some of them taken from Stellarium 0.21.3(GPLv2 Lic.)   //
//                                                         //
// Time classes is similar to Qt's time class.             //
// But these are more simple to use.                       //
//                                                         //
/////////////////////////////////////////////////////////////

#pragma once

#ifndef _CSE_DATETIME_
#define _CSE_DATETIME_

#include <CSE/CSEBase/CSEBase.h>
#include <ctime>

// Namespace definations
#define _TIME_BEGIN namespace epoch{
#define _TIME epoch::
#define _TIME_END }

_CSE_BEGIN

_TIME_BEGIN

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
    std::string ToString(std::string _Fmt = "{}.{:02}.{:02}")const;

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

    bool SetHMS(int h, int m, int s, int ms = 0);
    std::string ToString(std::string _Fmt = "{:02}:{:02}:{:02}.{:03}");

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
    void SetTimeZone(const long& ToZone);

    CSEDateTime ToUTC() const;

    static CSEDateTime CurrentDateTime(_TIME CSETimezone* ExportTimezone = nullptr);
    static CSEDateTime CurrentDateTimeUTC();

    // Convert to string, default using ISO8601 string format.
    std::string ToString(std::string _Fmt = "{}-{:02}-{:02}T{:02}:{:02}:{:02}.{:03}{:+03}:{:02}")const;
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

// Functions

/**
 * @brief Convert a Julian Day to CSE DateTime class
 * @param jd - Julian Date
 * @return CSE DateTime class
 */
CSEDateTime JDToDateTime(const double& JD);

/**
 * @brief Get the current Julian Date from system time.
 * @return the current Julian Date
 */
double GetJDFromSystem();

/**
 * @brief Get the Julian Day Number (JD) from Besselian epoch. (Taken from Stellarium)
 * @param Epoch - Besselian epoch, expressed as year
 * @return Julian Day number (JD) for B(Year)
 */
double GetJDFromBesEpoch(const double Epoch);

/**
 * @brief Convert a time of day to the fraction of a Julian Day.
 *          Note that a Julian Day starts at 12:00, not 0:00, and
 *          so 12:00 == 0.0 and 0:00 == 0.5.
 *          (Taken from Stellarium)
 */
double TimeToJDFract(const _TIME CSETime& Time);

/**
 * @brief Inverse function of TimeToJDFract(CSETime&)
 *          (Taken from Stellarium)
 */
_TIME CSETime JDFractToTime(const double JD);

/**
 * @brief Returns the count of days in a specified month in a specified year (Taken from Stellarium)
 * @param month - Specify month
 * @param year - Specify year
 * @return Day count
 */
int NumOfDaysInMonthInYear(const int month, const int year);

/**
 * @brief A safer function from Stellarium using to convert Date and Time to Julian day.
 * @param Date - Input Date time
 * @return Julian Day
 */
float64 GetJDFromDate(CSEDateTime Date);

_CSE_END

#endif
