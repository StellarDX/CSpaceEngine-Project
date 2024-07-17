/*
    Member functions of TLE Decoder
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

#include "CSE/Physics/Orbit.h"
#include "CSE/Base/ConstLists.h"
#include <cstring>
#include <ranges>

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

/**
 * @brief Validator for TLE
 * The checksums for each line are calculated by adding all numerical
 * digits on that line, including the line number. One is added to
 * the checksum for each negative sign (-) on that line. All other
 * non-digit characters are ignored.
 * @return
 */
bool TLE::IsValid()const
{
    uint32_t Checksum1 = _M_Data[1][68] - '0';
    uint32_t Checksum2 = _M_Data[2][68] - '0';
    uint64 Sum1 = 0, Sum2 = 0;
    for (size_t i = 0; i < 68; i++)
    {
        if (isdigit(_M_Data[1][i])) { Sum1 += _M_Data[1][i] - '0'; }
        if (isdigit(_M_Data[2][i])) { Sum2 += _M_Data[2][i] - '0'; }
        if ('-' == _M_Data[1][i]) { Sum1 += 1; }
        if ('-' == _M_Data[2][i]) { Sum2 += 1; }
    }
    return Sum1 % 10 == Checksum1 && Sum2 % 10 == Checksum2;
}

string TLE::Title()const
{
    return string(_M_Data[0]);
}

TLE::SpacecraftBasicData TLE::BasicData()const
{
    using namespace std;
    string Buffer1;
    string Buffer2;

    Buffer1.resize(69);
    Buffer2.resize(69);
    copy(_M_Data[1], _M_Data[1] + 69, Buffer1.begin());
    copy(_M_Data[2], _M_Data[2] + 69, Buffer2.begin());

    auto FullYear = [](int32_t Yr)
    {
        if (Yr > 57 && Yr < 99) { return 1900 + Yr; }
        else if (Yr > 00 && Yr < 56) { return 2000 + Yr; }
        else { throw OrbitCalculateException("Invalid year number?"); }
    };

    auto Class = [](char C)->string
    {
        switch (C)
        {
        case 'U':
            return "Unclassified";
        case 'C':
            return "Classified";
        case 'S':
            return "Secret";
        default:
            throw OrbitCalculateException("Invalid classification?");
        }
    };

    return
    {
        .CatalogNumber    = stoul(Buffer1.substr( 2,  5)),
        .Classification   = Class(Buffer1[7]),
        .IntDesignator    =
        {
            .LaunchYear   = FullYear(stoi(Buffer1.substr(9, 2))),
            .LaunchNumber = stoul(Buffer1.substr(11,  3)),
            .LaunchPiece  = Buffer1.substr(14, 3)
        },
        .D1MeanMotion     = stod (Buffer1.substr(33, 10)) * 2.,
        .D2MeanMotion     = stod (Buffer1.substr(44,  6)) * 1e-5 * pow(10, stod(Buffer1.substr(50, 2))) * 6.,
        .BSTAR            = stod (Buffer1.substr(53,  6)) * 1e-5 * pow(10, stod(Buffer1.substr(59, 2))),
        .EphemerisType    = stoul(Buffer1.substr(62,  1)),
        .ElementSet       = stoul(Buffer1.substr(64,  4)),
        .RevolutionNum    = stoul(Buffer2.substr(63,  5))
    };
}

string TLE::SpacecraftBasicData::COSPAR()const
{
    return vformat("{}-{:03}{}", make_format_args
        (IntDesignator.LaunchYear,
        IntDesignator.LaunchNumber,
        IntDesignator.LaunchPiece));
}

KeplerianOrbitElems TLE::Orbit()const
{
    using namespace std;
    string Buffer1;
    string Buffer2;

    Buffer1.resize(69);
    Buffer2.resize(69);
    copy(_M_Data[1], _M_Data[1] + 69, Buffer1.begin());
    copy(_M_Data[2], _M_Data[2] + 69, Buffer2.begin());

    auto FullYear = [](int32_t Yr)
    {
        if (Yr > 57 && Yr < 99) { return 1900 + Yr; }
        else if (Yr > 00 && Yr < 56) { return 2000 + Yr; }
        else { throw OrbitCalculateException("Invalid year number?"); }
    };

    return
    {
        .RefPlane = "Equator",
        .Epoch = _TIME CSEDate(FullYear(stoi(Buffer1.substr(18, 2))), 1, 1).ToJulianDay() - 0.5 + stod(Buffer1.substr(20, 12)) - 1,
        .Period = SynodicDay / stod(Buffer2.substr(52, 11)),
        .Eccentricity = stod(Buffer2.substr(26, 7)) * 1E-7,
        .Inclination = stod(Buffer2.substr(8, 8)),
        .AscendingNode = stod(Buffer2.substr(17, 8)),
        .ArgOfPericenter = stod(Buffer2.substr(34, 8)),
        .MeanAnomaly = stod(Buffer2.substr(43, 8)),
    };
}

TLE TLE::FromString(string _Input)
{
    vector<string> Lines;
    for (auto Line : _Input | views::split('\n'))
    {
        Lines.emplace_back(Line.begin(), Line.end());
    }
    // Check length
    if (Lines.size() != 3 &&
        (Lines[1].size() != 69 || Lines[2].size() != 69))
    {
        throw OrbitCalculateException("Invalid TLE Data");
    }

    TLE Data;
    memset(Data._M_Data[0], 0, 69);
    copy(Lines[0].begin(), Lines[0].end(), Data._M_Data[0]);
    copy(Lines[1].begin(), Lines[1].end(), Data._M_Data[1]);
    copy(Lines[2].begin(), Lines[2].end(), Data._M_Data[2]);
    return Data;
}

string TLE::SpacecraftBasicData::ToString() const
{
    ostringstream l1str;
    l1str << "Catalog Number:                   " << CatalogNumber << '\n';
    l1str << "Classification:                   " << Classification << '\n';
    l1str << "COSPAR ID:                        " << COSPAR() << '\n';
    l1str << "Ballistic Coefficient:            " << D1MeanMotion << '\n';
    l1str << "Second Derivative of Mean Motion: " << D2MeanMotion << '\n';
    l1str << "Radiation Rressure Coefficient:   " << BSTAR << '\n';
    l1str << "Ephemeris Type:                   " << EphemerisType << '\n';
    l1str << "Element Set Number:               " << ElementSet << '\n';
    l1str << "Revolution number at epoch:       " << RevolutionNum << '\n';
    return l1str.str();
}

_ORBIT_END
_CSE_END
