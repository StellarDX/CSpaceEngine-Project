#include "CSE/Physics/Orbit.h"
#include <cstring>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

using namespace std;

_CSE_BEGIN
_ORBIT_BEGIN

TLE::TLE(const char* Name, const char* L1, const char* L2)
{
    if (!L1 || !L2)
    {
        throw std::logic_error("Invalid TLE string, at least 2 lines required.");
    }
    errno_t ErrorCode;
    if (Name) {ErrorCode = ::memcpy_s(Title, TitleLength, Name, TitleLength);}
    if (ErrorCode) {throw std::logic_error(format("Error creating TLE datablock({})", ErrorCode));}
    ErrorCode = ::memcpy_s(Line1, DataLength, L1, DataLength);
    if (ErrorCode) {throw std::logic_error(format("Error creating TLE datablock({})", ErrorCode));}
    ErrorCode = ::memcpy_s(Line2, DataLength, L2, DataLength);
    if (ErrorCode) {throw std::logic_error(format("Error creating TLE datablock({})", ErrorCode));}
}

string TLE::ToString(char Delim) const
{
    return format("{}{}{}{}{}", Title, Delim, Line1, Delim, Line2);
}

TLE TLE::FromString(const char* Data, char Delim)
{
    // Use C-like pattern
    const char* BreakPoints[3] = {nullptr, nullptr, nullptr};
    const char* CurrentPosition = Data;
    int i = 0;
    while(i < 3)
    {
        CurrentPosition = ::strchr(CurrentPosition, Delim);
        ++i;
        if (CurrentPosition == nullptr) {break;}
    }

    if (i < 2)
    {
        throw std::logic_error("Invalid TLE string, at least 2 lines required.");
    }
    else if (i == 2)
    {
        uint64 CopySize1 = max(DataLength, uint64(BreakPoints[0] - Data));
        uint64 CopySize2 = max(DataLength, uint64(BreakPoints[1] - BreakPoints[0]));
        TLE Result;
        ::memcpy_s(Result.Line1, DataLength, Data, CopySize1); // C11
        ::memcpy_s(Result.Line2, DataLength, Data, CopySize2);
        return Result;
    }
    else
    {
        uint64 CopySize1 = max(TitleLength, uint64(BreakPoints[0] - Data));
        uint64 CopySize2 = max(DataLength, uint64(BreakPoints[1] - BreakPoints[0]));
        uint64 CopySize3 = max(DataLength, uint64(BreakPoints[2] - BreakPoints[1]));
        TLE Result;
        ::memcpy_s(Result.Title, TitleLength, Data, CopySize1);
        ::memcpy_s(Result.Line1, DataLength, Data, CopySize2);
        ::memcpy_s(Result.Line2, DataLength, Data, CopySize3);
        return Result;
    }
}

_ORBIT_END
_CSE_END
