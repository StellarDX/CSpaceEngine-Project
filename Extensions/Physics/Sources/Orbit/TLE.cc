#include "CSE/Base/ConstLists.h"
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

TLE::TLE()
{
    std::fill(std::begin(Title), std::end(Title), 0);
    std::fill(std::begin(Line1), std::end(Line1), 0);
    std::fill(std::begin(Line2), std::end(Line2), 0);
}

TLE::TLE(const char* Name, const char* L1, const char* L2) : TLE()
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

int TLE::VerifyLine(const char* Line, int Size, int Checksum)
{
    int Sum = 0;
    for (int i = 0; i < Size; ++i)
    {
        if (::isdigit(Line[i])) { Sum += Line[i] - '0';}
        if (Line[i] == '-') {++Sum;}
    }
    return Sum % 10;
}

bool TLE::IsValid() const
{
    int Checksum1 = -1, Checksum2 = -2;
    errno_t ErrorCode = 0;
    if (::isdigit(Line1[L1Checksum]) && ::isdigit(Line2[L2Checksum]))
    {
        Checksum1 = Line1[L1Checksum] - '0';
        Checksum2 = Line2[L2Checksum] - '0';
    }
    else {return 0;}

    // Check line1
    int Line1Verified = VerifyLine(Line1, DataLength - 1, Checksum1);
    int Line2Verified = VerifyLine(Line2, DataLength - 1, Checksum2);

    return Line1Verified == Checksum1 && Line2Verified == Checksum2;
}

void TLE::Get(void* Title, void* L1, void* L2) const
{
    ::memcpy(Title, this->Title, ::strlen(this->Title));
    ::memcpy(L1, this->Line1, DataLength);
    ::memcpy(L2, this->Line2, DataLength);
}

ustring TLE::SatelliteName() const
{
    std::string Result(this->Title);
    return ustring(Result);
}

SpacecraftBasicData TLE::BasicData() const
{
    SpacecraftBasicData Data;
    char* CurrentPos = nullptr;

    Data.CatalogNumber = ::strtol(Line1 + L1CatalogNumber, &CurrentPos, 10);
    Data.Classification = Line1[L1Classification];

    int COSPARIDYD = ::strtol(Line1 + L1COSPARIDYD, &CurrentPos, 10);
    Data.IntDesignator.LaunchYear = COSPARIDYD / 1000U;
    if (Data.IntDesignator.LaunchYear >= 57)
    {
        Data.IntDesignator.LaunchYear += 1900;
    }
    else {Data.IntDesignator.LaunchYear += 2000;}
    Data.IntDesignator.LaunchNumber = COSPARIDYD % 1000U;
    for (int i = 0; i < 3; ++i)
    {
        Data.IntDesignator.LaunchPiece[i] =
            ::isspace(Line1[L1COSPARIDP + i]) ? 0 : Line1[L1COSPARIDP + i];
    }

    Data.D1MeanMotion = ::strtod(Line1 + L1D1MeanMotion, &CurrentPos) * 2;
    Data.D1MeanMotion *= 360;
    Data.D1MeanMotion /= SynodicDay;

    int D2MeanMotionM = ::strtol(Line1 + L1D2MeanMotionM, &CurrentPos, 10);
    int D2MeanMotionE = ::strtol(Line1 + L1D2MeanMotionE, &CurrentPos, 10);
    Data.D2MeanMotion = float64(D2MeanMotionM) * pow(10, D2MeanMotionE) * 6;
    Data.D2MeanMotion *= 360;
    Data.D2MeanMotion /= uint64(SynodicDay) * SynodicDay * SynodicDay;

    int BSTARM = ::strtol(Line1 + L1BSTARM, &CurrentPos, 10);
    int BSTARE = ::strtol(Line1 + L1BSTARE, &CurrentPos, 10);
    Data.BSTAR = float64(BSTARM) * pow(10, BSTARE);
    Data.BSTAR /= EarthRadius;

    Data.EphemerisType = ::strtol(Line1 + L1EphemerisType, &CurrentPos, 10);

    int ElementSetNumber = ::strtol(Line1 + L1ElementSet, &CurrentPos, 10);
    Data.ElementSet = ElementSetNumber / 10U;

    int Revolutions = ::strtol(Line1 + L2Revolutions, &CurrentPos, 10);
    Data.RevolutionNum = Revolutions / 10U;

    return Data;
}

KeplerianOrbitElems TLE::OrbitElems() const
{
    KeplerianOrbitElems Data;
    char* CurrentPos = nullptr;

    Data.RefPlane = L"Equator";
    Data.GravParam = GravConstant * EarthMass;

    uint64 EpochI, EpochF;
    EpochI = ::strtoull(Line1 + L1EpochI, &CurrentPos, 10);
    EpochF = ::strtoull(Line1 + L1EpochF, &CurrentPos, 10);
    uint64 EpochYears = EpochI / 1000ULL;
    if (EpochYears > 56) {EpochYears += 1900;}
    else {EpochYears += 2000;}
    float64 EpochJD;
    GetJDFromDate(&EpochJD, EpochYears, 1, 1, 0, 0, 0);
    EpochJD += (EpochI % 1000ULL) + (float64(EpochF) / 100000000.);
    Data.Epoch = EpochJD;

    Data.Inclination = ::strtod(Line2 + L2Inclination, &CurrentPos);
    Data.AscendingNode = ::strtod(Line2 + L2AscendingNode, &CurrentPos);
    Data.Eccentricity = ::strtod(Line2 + L2Eccentricity, &CurrentPos) / 10000000.;
    Data.ArgOfPericenter = ::strtod(Line2 + L2ArgOfPericen, &CurrentPos);
    Data.MeanAnomaly = ::strtod(Line2 + L2MeanAnomaly, &CurrentPos);

    uint64 MeanMotionI, MeanMotionF;
    MeanMotionI = ::strtoull(Line2 + L2MeanMotionI, &CurrentPos, 10);
    MeanMotionF = ::strtoull(Line2 + L2MeanMotionF, &CurrentPos, 10);
    if (MeanMotionF >= 100000000ULL) {MeanMotionF /= 1000000ULL;}
    float64 MeanMotion = MeanMotionI + float64(MeanMotionF) / 100000000.;
    Data.Period = SynodicDay / MeanMotion;
    KeplerCompute(Data);

    return Data;
}

string TLE::ToString(char Delim) const
{
    return format("{}{}{}{}{}", Title, Delim, Line1, Delim, Line2);
}

TLE TLE::FromString(const char* Data, char Delim)
{
    // Use C-like pattern
    const char* BreakPoints[3] = {nullptr, nullptr, nullptr};
    const char* EndPoint = Data + strlen(Data);
    const char* CurrentPosition = Data;
    int i = 0;
    while(i < 3)
    {
        CurrentPosition = ::strchr(CurrentPosition, Delim);
        if (CurrentPosition == nullptr)
        {
            ++i;
            BreakPoints[i] = EndPoint;
            break;
        }
        BreakPoints[i] = CurrentPosition;
        ++CurrentPosition;
        ++i;
    }

    if (i < 2)
    {
        throw std::logic_error("Invalid TLE string, at least 2 lines required.");
    }
    else if (i == 2)
    {
        uint64 CopySize1 = min(DataLength, uint64(BreakPoints[0] - Data));
        uint64 CopySize2 = min(DataLength, uint64(BreakPoints[1] - BreakPoints[0]));
        TLE Result;
        ::memcpy_s(Result.Line1, DataLength, Data, CopySize1); // C11
        ::memcpy_s(Result.Line2, DataLength, BreakPoints[0] + 1, CopySize2);
        return Result;
    }
    else
    {
        uint64 CopySize1 = min(TitleLength, uint64(BreakPoints[0] - Data));
        uint64 CopySize2 = min(DataLength, uint64(BreakPoints[1] - BreakPoints[0]));
        uint64 CopySize3 = min(DataLength, uint64(BreakPoints[2] - BreakPoints[1]));
        TLE Result;
        ::memcpy_s(Result.Title, TitleLength, Data, CopySize1);
        ::memcpy_s(Result.Line1, DataLength, BreakPoints[0] + 1, CopySize2);
        ::memcpy_s(Result.Line2, DataLength, BreakPoints[1] + 1, CopySize3);
        return Result;
    }
}

_ORBIT_END
_CSE_END
