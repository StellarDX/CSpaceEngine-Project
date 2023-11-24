/************************************************************
  CSpaceEngine DateTime Module.
  Copyright (C) StellarDX Astronomy
  SPDX-License-Identifier: GPL-2.0
***********************************************************/

#include <CSE/CSEBase/DateTime.h>
#include <ctime>
#include <chrono>
#include <cassert>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

// Some of these functions has different behaviors at different platforms.
#include <CSE/CSEBase/sysdeps/SysDetector.h>

// Windows API connector
#ifdef CSE_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

using namespace std;

_CSE_BEGIN

/*****************************************************************************
  CSEDateTime member functions
 *****************************************************************************/

CSEDateTime::CSEDateTime(const _TIME CSEDate& Date, const _TIME CSETime& Time, const _TIME CSETimezone& TimeZone)
{
    OffsetSecs = -TimeZone.Bias * 60;
    _Date = Date;
    _Time = Time;
}

CSEDateTime::CSEDateTime(const _TIME CSEDate& Date, const _TIME CSETime& Time, const double& OffsetSec)
{
    OffsetSecs = OffsetSec;
    _Date = Date;
    _Time = Time;
}

CSEDateTime::CSEDateTime(const _TIME CSEDate& Date)
{
    _Date = Date;
    _Time = _TIME CSETime(0, 0, 0, 0); // Start of the day.
}

CSEDateTime CSEDateTime::AddDays(int ndays) const
{
    _TIME CSEDate DT = _Date;
    DT = DT.AddDays(ndays);
    return CSEDateTime(DT, _Time, OffsetSecs);
}

CSEDateTime CSEDateTime::AddMSecs(int msecs) const
{
    _TIME CSEDate DT = _Date;
    _TIME CSETime TM = _Time;
    int addDays = 0;
    TM = TM.AddMSecs(msecs, &addDays);
    DT = DT.AddDays(addDays);
    return CSEDateTime(DT, TM, OffsetSecs);
}

CSEDateTime CSEDateTime::AddMonths(int nmonths) const
{
    _TIME CSEDate DT = _Date;
    DT = DT.AddMonths(nmonths);
    return CSEDateTime(DT, _Time, OffsetSecs);
}

CSEDateTime CSEDateTime::AddSecs(int s) const
{
    _TIME CSEDate DT = _Date;
    _TIME CSETime TM = _Time;
    int addDays = 0;
    TM = TM.AddSecs(s, &addDays);
    DT = DT.AddDays(addDays);
    return CSEDateTime(DT, TM, OffsetSecs);
}

CSEDateTime CSEDateTime::AddYears(int nyears) const
{
    _TIME CSEDate DT = _Date;
    DT = DT.AddYears(nyears);
    return CSEDateTime(DT, _Time, OffsetSecs);
}

bool CSEDateTime::IsNull() const
{
    return _Date.IsNull() && _Time.IsNull();
}

bool CSEDateTime::IsValid() const
{
    return !IsNull() && _Date.IsValid() && _Time.IsValid();
}

float64 CSEDateTime::OffsetFromUTC() const
{
    return OffsetSecs;
}

void CSEDateTime::SetDate(const _TIME CSEDate& date)
{
    _Date = date;
}

void CSEDateTime::SetTime(const _TIME CSETime& time)
{
    _Time = time;
}

void CSEDateTime::SetOffsetFromUTC(int OffsetSeconds)
{
    OffsetSecs = OffsetSeconds;
}

void CSEDateTime::SetTimeZone(const long& ToZone)
{
    OffsetSecs = ToZone * 3600;
}

CSEDateTime CSEDateTime::ToUTC() const
{
    _TIME CSETime TM = _Time.AddMSecs((int)(-OffsetSecs * 1000.0));
    _TIME CSEDate DT = _Date.AddDays((int)ceil(-OffsetSecs * 1000 / 86400000.0));
    return CSEDateTime(DT, TM, 0);
}

// Implementions of getting current system time with time zone information
// This function is using system API before a better way has been found.
CSEDateTime CSEDateTime::CurrentDateTime(_TIME CSETimezone *ExportTimezone)
{
    #if defined(CSE_OS_UNIX) // Unix platform
    // TODO
    return CSEDateTime();

    #elif defined(CSE_OS_WIN) // Windows platform
    // We can get local time or "system" time (which is UTC);
    // otherwise, we must convert, which is most efficiently done from UTC.
    _SYSTEMTIME Time;
    _TIME_ZONE_INFORMATION TimezoneInfo;
    // GetSystemTime()'s page links to its partner page for GetLocalTime().
    // https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemtime
    GetLocalTime(&Time);
    GetSystemTime(&TimezoneInfo.StandardDate);
    GetTimeZoneInformation(&TimezoneInfo);
    _TIME CSETimezone TimeZone =
    {
        .Bias         = TimezoneInfo.Bias,
        .StandardName = TimezoneInfo.StandardName,
        .StandardDate = CSEDateTime
        (
            _TIME CSEDate(TimezoneInfo.StandardDate.wYear, TimezoneInfo.StandardDate.wMonth, TimezoneInfo.StandardDate.wDay),
            _TIME CSETime(TimezoneInfo.StandardDate.wHour, TimezoneInfo.StandardDate.wMinute, TimezoneInfo.StandardDate.wSecond, TimezoneInfo.StandardDate.wMilliseconds)
        ),
        .StandardBias = TimezoneInfo.StandardBias,
        .DaylightName = TimezoneInfo.DaylightName,
        .DaylightDate = CSEDateTime
        (
            _TIME CSEDate(TimezoneInfo.DaylightDate.wYear, TimezoneInfo.DaylightDate.wMonth, TimezoneInfo.DaylightDate.wDay),
            _TIME CSETime(TimezoneInfo.DaylightDate.wHour, TimezoneInfo.DaylightDate.wMinute, TimezoneInfo.DaylightDate.wSecond, TimezoneInfo.StandardDate.wMilliseconds)
        ),
        .DaylightBias = TimezoneInfo.DaylightBias,
    };

    if (ExportTimezone){*ExportTimezone = TimeZone;}
    return CSEDateTime
    (
        _TIME CSEDate(Time.wYear, Time.wMonth, Time.wDay),
        _TIME CSETime(Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds),
        TimeZone
    );

    #else
    // TODO
    return CSEDateTime();
    #endif
}

CSEDateTime CSEDateTime::CurrentDateTimeUTC()
{
    using namespace std::chrono;
    time_point NowPoint = system_clock::now();
    time_t CurrentTime = system_clock::to_time_t(NowPoint);
    tm GMTime = _TIME gmtime_remake(CurrentTime);
    return CSEDateTime
    (
        _TIME CSEDate(GMTime.tm_year + 1900, GMTime.tm_mon + 1, GMTime.tm_mday),
        _TIME CSETime(GMTime.tm_hour, GMTime.tm_min, GMTime.tm_sec,
        (duration_cast<milliseconds>(NowPoint.time_since_epoch()) % 1000).count()),
        0.0
    );
}

string CSEDateTime::ToString(string _Fmt)const
{
    auto Y = _Date.year();
    auto M = _Date.month();
    auto D = _Date.day();
    auto h = _Time.hour();
    auto m = _Time.minute();
    auto s = _Time.second();
    auto ms = _Time.msec();
    auto tzh = (int)OffsetSecs / 3600;
    auto tzm = (((int)OffsetSecs % 3600) / 60) + ((OffsetSecs / 60) - int(OffsetSecs / 60));
    return vformat(_Fmt, make_format_args
    (
        Y, M, D, h, m, s, ms, tzh, tzm
    ));
}

_TIME_BEGIN

/*****************************************************************************
  CSEDate member functions
 *****************************************************************************/

CSEDate::CSEDate(int y, int m, int d)
{
    years = y;
    months = m;
    days = d;
}

bool CSEDate::IsNull() const
{
    return years == -1 && months == -1 && days == -1;
}

bool CSEDate::IsValid() const
{
    if (IsNull()) { return false; }
    return IsValid(years, months, days);
}

int CSEDate::year() const
{
    if (IsValid()){return years;}
    return -1;
}

int CSEDate::month() const
{
    if (IsValid()){return months;}
    return -1;
}

int CSEDate::day() const
{
    if (IsValid()){return days;}
    return -1;
}

CSEDate CSEDate::AddDays(int ndays)const
{
    if (IsNull()){ return CSEDate(); }
    int64 JD = ToJulianDay(); // A "abnormal" method from Qt
    JD += ndays; // maybe a prevention of overflows is needed at this
    return FromJulianDay(JD);

    // Old method, maybe very slow when amount of days is very large.
    #if 0
    int Y = years;
    int M = months;
    int D = days;

    if (ndays >= 0)
    {
        while (ndays > 0)
        {
            ++D;
            if (D > NumOfDaysInMonthInYear(M, Y))
            {
                D -= NumOfDaysInMonthInYear(M, Y);
                ++M;
            }

            if (M > 12)
            {
                M -= 12;
                ++Y;
            }

            if (Y == 1582 && M == 10 && D == 5)
            {
                D = 15;
            }

            --ndays;
        }
    }

    else if (ndays < 0)
    {
        while (ndays < 0)
        {
            --D;
            if (D <= 0)
            {
                --M;
                D += NumOfDaysInMonthInYear(M, Y);
            }

            if (M <= 0)
            {
                M += 12;
                --Y;
            }

            if (Y == 1582 && M == 10 && D == 14)
            {
                D = 4;
            }

            ++ndays;
        }
    }

    return CSEDate(Y, M, D);
    #endif
}

CSEDate CSEDate::AddMonths(int nmonths)const
{
    if (!IsValid()){ return CSEDate(); }
    if (nmonths == 0){ return *this; }

    int Y = years;
    int M = months;
    int D = days;

    M += nmonths;
    while (M <= 0)
    {
        if (--Y) {M += 12;} // skip over year 0
    }
    while (M > 12)
    {
        M -= 12;
        if (!++Y) {++Y;} // skip over year 0
    }

    if (D > NumOfDaysInMonthInYear(M, Y))
    {
        D = NumOfDaysInMonthInYear(M, Y);
    }

    if (Y == 1582 && M == 10 && D > 4 && D < 15)
    {
        D = 15;
    }

    return CSEDate(Y, M, D);
}

CSEDate CSEDate::AddYears(int nyears)const
{
    if (!IsValid()){ return CSEDate(); }

    int Y = years;
    int M = months;
    int D = days;

    Y += nyears;

    if (D > NumOfDaysInMonthInYear(M, Y))
    {
        D = NumOfDaysInMonthInYear(M, Y);
    }

    if (Y == 1582 && M == 10 && D > 4 && D < 15)
    {
        D = 15;
    }

    return CSEDate(Y, M, D);
}

void CSEDate::GetDate(int* year, int* month, int* day)const
{
    const bool OK = IsValid();
    if (year) {*year = OK ? years : 0;}
    if (month) {*month = OK ? months : 0;}
    if (day) {*day = OK ? days : 0;}
}

bool CSEDate::SetDate(int year, int month, int day)
{
    *this = CSEDate(year, month, day);
    return IsValid();
}

string CSEDate::ToString(string _Fmt) const
{
    return vformat(_Fmt, make_format_args(years, months, days));
}

int CSEDate::DayOfWeek() const
{
    double JD = (double)ToJulianDay();
    double d = fmod(JD + 1.5, 7);
    if (d < 0) { d += 7.0; }
    return lround(floor(d));
}

int64 CSEDate::ToJulianDay() const // Convert a Date class to julian day(without fraction).
{
    /*
     * This algorithm is taken from
     * "Numerical Recipes in C, 2nd Ed." (1992), pp. 11-12
    */
    static const long IGREG = 588829; // (15 + 31L * (10 + 12L * 1582))

    long ja, jy = years, jm;

    if (jy == 0){throw logic_error("julday: there is no year zero.");}

    if (jy < 0) { ++jy; }
    if (months > 2)
    {
        jm = months + 1;
    }
    else
    {
        --jy;
        jm = months + 13;
    }

    long Jul = (long)(floor(365.25 * jy) + floor(30.6001 * jm) + days + 1720995);

    if (days + 31L * (months + 12L * years) >= IGREG)
    {
        ja = jy / 100;
        Jul += 2 - ja + (ja / 4);
    }

    return Jul;
}

CSEDate CSEDate::CurrentDate()
{
    time_t NowSecFrom1970;
    time(&NowSecFrom1970);
    tm NowDateTime = gmtime_remake(NowSecFrom1970); // Only get date in this scope.
    return CSEDate(NowDateTime.tm_year + 1900, NowDateTime.tm_mon + 1, NowDateTime.tm_mday);
}

CSEDate CSEDate::FromJulianDay(const double JD) // From Stellarium
{
    /*
     * This algorithm is taken from
     * "Numerical Recipes in C, 2nd Ed." (1992), pp. 14-15
     * and converted to integer math.
    */

    static const long JD_GREG_CAL = 2299161;
    static const int JB_MAX_WITHOUT_OVERFLOW = 107374182;
    const long julian = static_cast<long>(floor(JD + 0.5));

    long ta, jalpha, tb, tc, td, te;

    if (julian >= JD_GREG_CAL)
    {
        jalpha = (4 * (julian - 1867216) - 1) / 146097;
        ta = julian + 1 + jalpha - jalpha / 4;
    }
    else if (julian < 0)
    {
        ta = julian + 36525 * (1 - julian / 36525);
    }
    else
    {
        ta = julian;
    }

    tb = ta + 1524;
    if (tb <= JB_MAX_WITHOUT_OVERFLOW)
    {
        tc = (tb * 20 - 2442) / 7305;
    }
    else
    {
        tc = static_cast<long>((static_cast<unsigned long long>(tb) * 20 - 2442) / 7305);
    }
    td = 365 * tc + tc / 4;
    te = ((tb - td) * 10000) / 306001;

    int dd = tb - td - (306001 * te) / 10000;

    int mm = te - 1;
    if (mm > 12)
    {
        mm -= 12;
    }
    int yy = tc - 4715;
    if (mm > 2)
    {
        --(yy);
    }
    if (julian < 0)
    {
        yy -= 100 * (1 - julian / 36525);
    }

    return CSEDate(yy, mm, dd);
}

bool CSEDate::IsLeap()
{
    return IsLeap(years);
}

bool CSEDate::IsLeap(int year)
{
    if (year > 1582)
    {
        if (year % 100 == 0) { return (year % 400 == 0); }
        else { return (year % 4 == 0); }
    }
    else
    {
        return (year % 4 == 0); // astronomical year counting: strictly every 4th year.
    }
}

bool CSEDate::IsValid(int year, int month, int day)
{
    if (year == 1582 && month == 10 && day >= 5 && day <= 14) { return false; } // When Julian calendar had been replaced by Gregorian calendar...
    if ((month > 0 && month <= 12) && (day > 0 && day <= NumOfDaysInMonthInYear(month, year))) { return true; }
    else { return false; }
}

/*****************************************************************************
  CSETime member functions
 *****************************************************************************/

// Rewrite localtime function and gmtime function because they are unsafe
// Reference: https://qa.1r1g.com/sf/ask/2662382341/
tm localtime_remake(time_t timer)
{
    tm bt{};
    #if defined(CSE_OS_UNIX) // Unix platform
    localtime_r(&timer, &bt);
    #elif defined(CSE_OS_WIN) // Windows platform
    localtime_s(&bt, &timer);
    #else
    static mutex mtx;
    lock_guard<mutex> lock(mtx);
    bt = *localtime(&timer);
    #endif
    return bt;
}

// StellarDX: But these functions end with "_s" maybe also have traps... Reasons below:
// 1. Parameter sequence is different from C++ standard
// 2. Return type is also different from standard
// 3. Behavior of these functions maybe differs from different compiler and sdk.
tm gmtime_remake(time_t timer)
{
    tm bt{};
    #if defined(CSE_OS_UNIX) // Unix platform
    gmtime_r(&timer, &bt);
    #elif defined(CSE_OS_WIN) // Windows platform
    gmtime_s(&bt, &timer);
    #else
    static mutex mtx;
    lock_guard<mutex> lock(mtx);
    bt = *gmtime(&timer);
    #endif
    return bt;
}

// So, Don't put too much faith in standard library.
// There also have "maverick" exceptions in it.

CSETime::CSETime(int h, int m, int s, int ms)
{
    hours = h;
    minutes = m;
    seconds = s;
    msecs = ms;
}

CSETime::CSETime(int h, int m, double s)
{
    hours = h;
    minutes = m;
    double sec;
    msecs = int(modf(s, &sec) * 1000);
    seconds = int(sec);
}

bool CSETime::IsNull() const
{
    return hours == -1 && minutes == -1 && seconds == -1 && msecs == -1;
}

bool CSETime::IsValid() const
{
    return !IsNull() && IsValid(hours, minutes, seconds, msecs);
}

int CSETime::hour() const
{
    if (IsValid()){return hours;}
    return -1;
}

int CSETime::minute() const
{
    if (IsValid()){return minutes;}
    return -1;
}

int CSETime::second() const
{
    if (IsValid()){return seconds;}
    return -1;
}

int CSETime::msec() const
{
    if (IsValid()){return msecs;}
    return -1;
}

CSETime CSETime::AddMSecs(int ms, int* AddDays) const
{
    // This method is inspired by the Qt source code
    static const long MAX_MSEC_IN_DAYS = 86400000; // 24 * 60 * 60 * 1000
    long CurrentMsecs = hours * 3600000 + minutes * 60000 + seconds * 1000 + msecs;
    CurrentMsecs += ms;
    if (CurrentMsecs > MAX_MSEC_IN_DAYS)
    {
        if (AddDays){*AddDays = long(CurrentMsecs / MAX_MSEC_IN_DAYS);}
        CurrentMsecs %= MAX_MSEC_IN_DAYS;
    }
    // Convertion provided by New Bing
    return CSETime
    (
        CurrentMsecs / 3600000,
        (CurrentMsecs % 3600000) / 60000,
        (CurrentMsecs % 60000) / 1000,
        CurrentMsecs % 1000
    );

    // Old method, maybe very slow when amount of days is very large.
    #if 0
    int H = hours;
    int M = minutes;
    int S = seconds;
    int mS = msecs;

    mS += ms;

    if (ms > 0)
    {
        while (mS >= 1000)
        {
            mS -= 1000;
            ++S;

            if (S >= 60)
            {
                S -= 60;
                ++M;
            }

            if (M >= 60)
            {
                M -= 60;
                ++H;
            }

            if (H >= 24)
            {
                H -= 24;
                if (nullptr != AddDays)
                {
                    ++(*AddDays);
                }
            }
        }
    }

    if (ms < 0)
    {
        while (mS < 0)
        {
            mS += 1000;
            --S;

            if (S < 0)
            {
                S += 60;
                --M;
            }

            if (M < 0)
            {
                M += 60;
                --H;
            }

            if (H < 0)
            {
                H += 24;
                if (nullptr != AddDays)
                {
                    --(*AddDays);
                }
            }
        }
    }

    return CSETime(H, M, S, mS);
    #endif
}

CSETime CSETime::AddSecs(int s, int* AddDays) const
{
    return AddMSecs(s * 1000, AddDays);
    #if 0
    int H = hours;
    int M = minutes;
    int S = seconds;
    int mS = msecs;

    S += s;

    if (s >= 0)
    {
        while (S >= 60)
        {
            S -= 60;
            ++M;

            if (M >= 60)
            {
                M -= 60;
                ++H;
            }

            if (H >= 24)
            {
                H -= 24;
                if (nullptr != AddDays)
                {
                    ++(*AddDays);
                }
            }
        }
    }

    if (s < 0)
    {
        while (S < 0)
        {
            S += 60;
            --M;

            if (M < 0)
            {
                M += 60;
                --H;
            }

            if (H < 0)
            {
                H += 24;
                if (nullptr != AddDays)
                {
                    --(*AddDays);
                }
            }
        }
    }

    return CSETime(H, M, S, mS);
    #endif
}

bool CSETime::SetHMS(int h, int m, int s, int ms)
{
    if (!IsValid(h, m, s, ms))
    {
        hours = -1;
        minutes = -1;
        seconds = -1;
        msecs = -1; // make this invalid
        return false;
    }

    hours = h;
    minutes = m;
    seconds = s;
    msecs = ms;
    return true;
}

string CSETime::ToString(string _Fmt)
{
    return vformat(_Fmt, make_format_args(hours, minutes, seconds, msecs));
}

bool CSETime::IsValid(int h, int m, int s, int ms)
{
    return (h < 24 && m < 60 && s < 60 && ms < 1000) && (h >= 0 && m >= 0 && s >= 0 && ms > 0);
}

CSETime CSETime::CurrentTime()
{
    // Method only using standard library, and precision can be extremely (ns)
    // Reference: https://blog.csdn.net/wangmeng_92/article/details/131583012
    using namespace std::chrono;
    time_point NowPoint = system_clock::now();
    time_t CurrentTime = system_clock::to_time_t(NowPoint);
    tm LocalTime = localtime_remake(CurrentTime);
    return CSETime(LocalTime.tm_hour, LocalTime.tm_min, LocalTime.tm_sec,
    (duration_cast<milliseconds>(NowPoint.time_since_epoch()) % 1000).count());
}

_TIME_END

/*****************************************************************************
  Other functions
 *****************************************************************************/

CSEDateTime JDToDateTime(const double& JD)
{
    CSEDateTime result = CSEDateTime(_TIME CSEDate().FromJulianDay(JD), JDFractToTime(JD), 0.0);
    if (!result.IsValid())
    {
        throw logic_error("Invalid DateTime.");
    }
    return result;
}

double GetJDFromSystem()
{
    return _TIME CSEDate::CurrentDate().ToJulianDay() + TimeToJDFract(_TIME CSETime::CurrentTime());
}

double GetJDFromBesEpoch(const double Epoch)
{
    return 2400000.5 + (15019.81352 + (Epoch - 1900.0) * 365.242198781);
}

double TimeToJDFract(const _TIME CSETime& Time)
{
    return double((Time.hour() - 12.0) / 24.0 + Time.minute() / 1440.0 + (Time.second() + Time.msec() / 1000.0) / 86400.0);
}

_TIME CSETime JDFractToTime(const double JD) // Stellarium function
{
    double decHours = std::fmod(JD + 0.5, 1.0) * 24.;
    int hours = int(std::floor(decHours));
    double decMins = (decHours - hours) * 60.;
    int mins = int(std::floor(decMins));
    double decSec = (decMins - mins) * 60.;
    int sec = int(std::floor(decSec));
    double decMsec = (decSec - sec) * 1000.;
    int ms = int(std::round(decMsec));

    if (ms >= 1000)
    {
        ms -= 1000;
        sec += 1;
    }
    if (sec >= 60)
    {
        sec -= 60;
        mins += 1;
    }
    if (mins >= 60)
    {
        mins -= 60;
        hours += 1;
    }
    if (hours >= 24)
    {
        CSESysDebug("DateTime", CSEDebugger::WARNING, "hours exceed a full day!\n");
    }
    hours %= 24;

    _TIME CSETime tm = _TIME CSETime(hours, mins, sec, ms);
    if (!tm.IsValid())
    {
        throw logic_error("Invalid Time:" + std::to_string(hours) + "/" + std::to_string(mins) + "/" + std::to_string(sec) + "/" + std::to_string(ms));
    }
    return tm;
}

int NumOfDaysInMonthInYear(const int month, const int year) // From Stellarium
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        if (year > 1582)
        {
            if (year % 4 == 0)
            {
                if (year % 100 == 0)
                {
                    return (year % 400 == 0 ? 29 : 28);
                }
                else
                {
                    return 29;
                }
            }
            else
            {
                return 28;
            }
        }
        else
        {
            return (year % 4 == 0 ? 29 : 28);
        }
    case 0:
        return NumOfDaysInMonthInYear(12, year - 1);
    case 13:
        return NumOfDaysInMonthInYear(1, year + 1);
    default:
        return 0;
    }
}

float64 GetJDFromDate(CSEDateTime Date)
{
    Date = Date.ToUTC();
    const int y = Date.date().year();
    const int m = Date.date().month();
    const int d = Date.date().day();
    const int h = Date.time().hour();
    const int min = Date.time().minute();
    const float64 s = (float64)Date.time().second() + Date.time().msec() / 1000.;

    #ifndef _USE_ALTER_ALGORITHM_FOR_JULIAN

    static const long IGREG2 = 15 + 31L * (10 + 12L * 1582);
    float64 deltaTime = (h / 24.0) + (min / (1440.0)) + (s / (86400.0)) - 0.5;

    #ifndef _FORCE_USE_SAFER_JULIAN_ALG
    _TIME CSEDate test((y <= 0 ? y - 1 : y), m, d);
    if (test.IsValid() && y > 1582)
    {
        float64 jd = (float64)test.ToJulianDay();
        jd += deltaTime;
        return jd;
    }
    else
    {
    #endif

        /*
         * Algorithm taken from "Numerical Recipes in C, 2nd Ed." (1992), pp. 11-12
         * Stellarium converted it to integer math.
         */
        long ljul;
        long jy, jm;
        long laa, lbb, lcc, lee;

        jy = y;
        if (m > 2)
        {
            jm = m + 1;
        }
        else
        {
            --jy;
            jm = m + 13;
        }

        laa = 1461 * jy / 4;
        if (jy < 0 && jy % 4)
        {
            --laa;
        }
        lbb = 306001 * jm / 10000;
        ljul = laa + lbb + d + 1720995L;

        if (d + 31L * (m + 12L * y) >= IGREG2)
        {
            lcc = jy / 100;
            if (jy < 0 && jy % 100)
            {
                --lcc;
            }
            lee = lcc / 4;
            if (lcc < 0 && lcc % 4)
            {
                --lee;
            }
            ljul += 2 - lcc + lee;
        }
        float64 jd = (float64)(ljul);
        jd += deltaTime;
        return jd;

    #ifndef _FORCE_USE_SAFER_JULIAN_ALG
    }
    #endif

    #else

    double extra = (100.0 * y) + m - 190002.5;
    double rjd = 367.0 * y;
    rjd -= floor(7.0 * (y + floor((m + 9.0) / 12.0)) / 4.0);
    rjd += floor(275.0 * m / 9.0);
    rjd += d;
    rjd += (h + (min + s / 60.0) / 60.) / 24.0;
    rjd += 1721013.5;
    rjd -= 0.5 * extra / std::fabs(extra);
    rjd += 0.5;
    return rjd;

    #endif
}

_CSE_END
