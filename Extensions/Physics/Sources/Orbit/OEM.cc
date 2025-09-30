#include "CSE/Physics/Orbit.h"
#include <ranges>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

_CSE_BEGIN
_ORBIT_BEGIN

void OEM::Parse(std::istream& fin, ValueSet* out)
{
    using namespace std;
    /*
        State machine:
                      +---------- EndOfData <---------+
                      V                               |
        Header --> Metadata --> Ephemeris --> CovarianceMatrix
                      ^             |
                      +-------------+
    */
    enum StateType
    {
        Header, Metadata, Ephemeris, CovarianceMatrix, EndOfData
    }State = Header;

    enum CovarianceMatrixStateType
    {
        Keywords, DataBlock
    }CMState = Keywords;
    uint64 CMDataCount = 0;

    map<string, string> MetadataBuf;
    vector<ValueType::EphemerisType> EphemerisBuf;
    vector<ValueType::CovarianceMatrixType> CovarianceMatrixBuf;
    ValueType::CovarianceMatrixType CovarianceMatrixC;

    uint64 LineNumber = 0;
    string Line;
    while(getline(fin, Line, '\n'))
    {
        ++LineNumber;
        // Skip comments and empty lines
        if (Line.empty()) {continue;}
        if (ParseComment(Line)) {continue;}

        RemoveWhiteSpace(Line);

        switch (State)
        {
        case Header:
            if (Line == "META_START")
            {
                TransferHeader(MetadataBuf, out);
                MetadataBuf.clear();
                State = Metadata;
            }
            else
            {
                auto KeyValue = ParseKeyValue(Line);
                if (MetadataBuf.contains(KeyValue.first))
                {
                    throw logic_error("Duplicate key: " +
                        KeyValue.first +
                        format(" at line {}", LineNumber));
                }
                MetadataBuf.insert(KeyValue);
            }
            break;
        case Metadata:
            if (Line == "META_STOP")
            {
                TransferMetaData(MetadataBuf, out);
                MetadataBuf.clear();
                State = Ephemeris;
            }
            else
            {
                auto KeyValue = ParseKeyValue(Line);
                if (MetadataBuf.contains(KeyValue.first))
                {
                    throw logic_error("Duplicate key: "
                        + KeyValue.first +
                        format(" at line {}", LineNumber));
                }
                MetadataBuf.insert(KeyValue);
            }
            break;
        case Ephemeris:
            if (Line == "META_START")
            {
                TransferEphemeris(EphemerisBuf, out);
                EphemerisBuf.clear();
                State = Metadata;
            }
            else if (Line == "COVARIANCE_START")
            {
                TransferEphemeris(EphemerisBuf, out);
                EphemerisBuf.clear();
                State = CovarianceMatrix;
            }
            else
            {
                EphemerisBuf.push_back(ParseEphemeris(Line));
            }
            break;
        case CovarianceMatrix:
            if (Line == "COVARIANCE_STOP")
            {
                TransferCovarianceMatrices(CovarianceMatrixBuf, out);
                CovarianceMatrixBuf.clear();
                State = EndOfData;
            }
            else
            {
                switch (CMState)
                {
                case Keywords:
                    if (::isdigit(Line[0]))
                    {
                        string Epoch = MetadataBuf.at("EPOCH");
                        int Y, M, D, h, m, off;
                        double s;
                        GetDateTimeFromISO8601String(Epoch, &Y, &M, &D, &h, &m, &s, &off);
                        CovarianceMatrixC.Epoch = {{Y, M, D}, {h, m, s}, float64(off)};
                        CovarianceMatrixC.RefFrame = MetadataBuf.at("COV_REF_FRAME");
                        CovarianceMatrixC.Data.fill(0);
                        MetadataBuf.clear();
                        CMState = DataBlock;
                        CMDataCount = 1;
                    }
                    else if (Line.substr(0, 5) == "EPOCH" ||
                        Line.substr(0, 13) == "COV_REF_FRAME")
                    {
                        auto KeyValue = ParseKeyValue(Line);
                        if (MetadataBuf.contains(KeyValue.first))
                        {
                            throw logic_error("Duplicate key: "
                                + KeyValue.first +
                                format(" at line {}", LineNumber));
                        }
                        MetadataBuf.insert(KeyValue);
                    }
                    else {throw logic_error("Unexpected key.");}
                case DataBlock:
                    if (::isupper(Line[0]))
                    {
                        CovarianceMatrixBuf.push_back(CovarianceMatrixC);
                        CMState = Keywords;
                        CMDataCount = 0;
                    }
                    else
                    {
                        if (CMDataCount > 6)
                        {
                            throw logic_error("Covariance Matrix data out of range");
                        }
                        auto Parts = ParseRawData(Line);
                        if (Parts.size() != CMDataCount)
                        {
                            throw logic_error(
                                format("Number of data in line {} must be {}",
                                LineNumber, CMDataCount));
                        }
                        for (int i = 0; i < Parts.size(); ++i)
                        {
                            CovarianceMatrixC.Data[i][CMDataCount - 1]
                                = stod(Parts[i]);
                        }
                    }
                }
            }
            break;
        case EndOfData:
            if (Line == "META_START") {State = Metadata;}
            else {throw logic_error("Unexpected line.");}
            break;
        }
    }

    // Post-processing
    switch (State)
    {
    case Header:
        throw logic_error("Metadata required.");
    case Metadata:
        throw logic_error("Ephemeris data required.");
    case Ephemeris:
        TransferEphemeris(EphemerisBuf, out);
        break;
    case CovarianceMatrix:
        throw logic_error("COVARIANCE_STOP required.");
    case EndOfData:
        break;
    }
}

bool OEM::ParseComment(std::string Line)
{
    RemoveWhiteSpace(Line);
    if (Line.substr(0, 7) == "COMMENT") {return 1;}
    return 0;
}

void OEM::RemoveWhiteSpace(std::string& Line)
{
    Line.erase(0, Line.find_first_not_of(" \t\r\n"));
    Line.erase(Line.find_last_not_of(" \t\r\n") + 1);
}

std::pair<std::string, std::string> OEM::ParseKeyValue(std::string Line)
{
    // ranges提供了一种新的写程序的范式，让程序更加的简洁，但是更抽象了。
    auto SplitView = Line
        | std::ranges::views::split('=')
        | std::ranges::views::transform([](auto&& Subrange)
        {
            return std::string(Subrange.begin(), Subrange.end());
        });
    std::vector<std::string> Result(SplitView.begin(), SplitView.end());
    if (Result.size() != 2)
    {
        throw std::logic_error("Each lines can only has 1 pair of key and value.");
    }
    RemoveWhiteSpace(Result[0]);
    RemoveWhiteSpace(Result[1]);
    return {Result[0], Result[1]};
}

std::vector<std::string> OEM::ParseRawData(std::string Line)
{
    std::replace(Line.begin(), Line.end(), '\t', ' ');
    auto Tokens = Line
        | std::ranges::views::split(' ')
        | std::ranges::views::transform([](auto&& Subrange)
        {
            return std::string(Subrange.begin(), Subrange.end());
        })
        | std::ranges::views::filter([](const std::string& s)
        {
            return !s.empty();
        });

    std::vector<std::string> Parts(Tokens.begin(), Tokens.end());
    return Parts;
}

OEM::ValueType::EphemerisType OEM::ParseEphemeris(std::string Line)
{
    auto Parts = ParseRawData(Line);
    if (Parts.size() != 7 && Parts.size() != 10)
    {
        throw std::logic_error(
            "The order in which data items are given shall be fixed: "
            "Epoch, X, Y, Z, Vx, Vy, Vz, [Ax, Ay, Az].");
    }

    int Y, M, D, h, m, off;
    double s;
    GetDateTimeFromISO8601String(Parts[0], &Y, &M, &D, &h, &m, &s, &off);

    if (Parts.size() == 7)
    {
        return
        {
            .Epoch = {{Y, M, D}, {h, m, s}, float64(off)},
            .Position = vec3(std::stod(Parts[1]),
                std::stod(Parts[2]), std::stod(Parts[3])),
            .Velocity =  vec3(std::stod(Parts[4]),
                std::stod(Parts[5]), std::stod(Parts[6]))
        };
    }

    if (Parts.size() == 10)
    {
        return
        {
            .Epoch = {{Y, M, D}, {h, m, s}, float64(off)},
            .Position = vec3(std::stod(Parts[1]),
                std::stod(Parts[2]), std::stod(Parts[3])),
            .Velocity =  vec3(std::stod(Parts[4]),
                std::stod(Parts[5]), std::stod(Parts[6])),
            .Acceleration =  vec3(std::stod(Parts[7]),
                std::stod(Parts[8]), std::stod(Parts[9]))
        };
    }
}

_ORBIT_END
_CSE_END
