#include "CSE/Physics/Orbit.h"
#include <ranges>
#include <fstream>
#include <sstream>
#include <filesystem>

// Text-formating header
#if USE_FMTLIB
#include <fmt/format.h>
using namespace fmt;
#else
#include <format>
#endif

_CSE_BEGIN
_ORBIT_BEGIN

void OEM::Import(std::istream& fin, OEM* out)
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
                CovarianceMatrixBuf.push_back(CovarianceMatrixC);
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
                    break;
                case DataBlock:
                    if (::isupper(Line[0]))
                    {
                        CovarianceMatrixBuf.push_back(CovarianceMatrixC);
                        CMState = Keywords;
                        CMDataCount = 0;
                    }
                }

                switch (CMState)
                {
                case Keywords:
                    if (Line.substr(0, 5) == "EPOCH" ||
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
                    break;
                case DataBlock:
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
                    ++CMDataCount;
                    break;
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
                std::stod(Parts[2]), std::stod(Parts[3])) * 1000.,
            .Velocity =  vec3(std::stod(Parts[4]),
                std::stod(Parts[5]), std::stod(Parts[6])) * 1000.
        };
    }

    else // if (Parts.size() == 10)
    {
        return
        {
            .Epoch = {{Y, M, D}, {h, m, s}, float64(off)},
            .Position = vec3(std::stod(Parts[1]),
                std::stod(Parts[2]), std::stod(Parts[3])) * 1000.,
            .Velocity =  vec3(std::stod(Parts[4]),
                std::stod(Parts[5]), std::stod(Parts[6])) * 1000.,
            .Acceleration =  vec3(std::stod(Parts[7]),
                std::stod(Parts[8]), std::stod(Parts[9])) * 1000.
        };
    }
}

void OEM::TransferHeader(std::map<std::string, std::string> Buf, OEM *out)
{
    out->OEMVersion = Buf.at("CCSDS_OEM_VERS");
    if (Buf.contains("CLASSIFICATION")) {out->Classification = Buf.at("CLASSIFICATION");}
    int Y, M, D, h, m, off;
    double s;
    GetDateTimeFromISO8601String(Buf.at("CREATION_DATE"), &Y, &M, &D, &h, &m, &s, &off);
    out->CreationDate = {{Y, M, D}, {h, m, s}, float64(off)};
    out->Originator = Buf.at("ORIGINATOR");
    if (Buf.contains("MESSAGE_ID")) {out->MessageID = Buf.at("MESSAGE_ID");}
}

void OEM::TransferMetaData(std::map<std::string, std::string> Buf, OEM *out)
{
    OEM::ValueType NewSection;
    NewSection.MetaData.ObjectName = Buf.at("OBJECT_NAME");
    NewSection.MetaData.ObjectID = Buf.at("OBJECT_ID");
    NewSection.MetaData.CenterName = Buf.at("CENTER_NAME");
    NewSection.MetaData.RefFrame = Buf.at("REF_FRAME");
    int Y, M, D, h, m, off;
    double s;
    if (Buf.contains("REF_FRAME_EPOCH"))
    {
        GetDateTimeFromISO8601String(Buf.at("REF_FRAME_EPOCH"), &Y, &M, &D, &h, &m, &s, &off);
        NewSection.MetaData.RefFrameEpoch = {{Y, M, D}, {h, m, s}, float64(off)};
    }
    NewSection.MetaData.TimeSystem = Buf.at("TIME_SYSTEM");
    GetDateTimeFromISO8601String(Buf.at("START_TIME"), &Y, &M, &D, &h, &m, &s, &off);
    NewSection.MetaData.StartTime = {{Y, M, D}, {h, m, s}, float64(off)};
    if (Buf.contains("USEABLE_START_TIME"))
    {
        GetDateTimeFromISO8601String(Buf.at("USEABLE_START_TIME"), &Y, &M, &D, &h, &m, &s, &off);
        NewSection.MetaData.UseableStartTime = {{Y, M, D}, {h, m, s}, float64(off)};
    }
    if (Buf.contains("USEABLE_STOP_TIME"))
    {
        GetDateTimeFromISO8601String(Buf.at("USEABLE_STOP_TIME"), &Y, &M, &D, &h, &m, &s, &off);
        NewSection.MetaData.UseableStopTime = {{Y, M, D}, {h, m, s}, float64(off)};
    }
    GetDateTimeFromISO8601String(Buf.at("STOP_TIME"), &Y, &M, &D, &h, &m, &s, &off);
    NewSection.MetaData.StopTime = {{Y, M, D}, {h, m, s}, float64(off)};
    if (Buf.contains("INTERPOLATION"))
    {
        NewSection.MetaData.Interpolation = Buf.at("INTERPOLATION");
        NewSection.MetaData.InterpolaDegrees = stoull(Buf.at("INTERPOLATION_DEGREE"));
    }
    out->Data.push_back(NewSection);
}

void OEM::TransferEphemeris(std::vector<ValueType::EphemerisType> Buf, OEM *out)
{
    out->Data.back().Ephemeris = Buf;
}

void OEM::TransferCovarianceMatrices(std::vector<ValueType::CovarianceMatrixType> Buf, OEM *out)
{
    out->Data.back().CovarianceMatrices = Buf;
}

void OEM::ExportKeyValue(std::ostream& fout, std::string Key, std::string Value, bool Optional, cstring Fmt)
{
    using namespace std;
    if ((!Optional) || (Optional && !Value.empty()))
    {
        fout << vformat(Fmt, make_format_args(Key, Value));
        fout << '\n';
    }
}

void OEM::ExportEphemeris(std::ostream& fout, std::vector<ValueType::EphemerisType> Eph, cstring Fmt)
{
    using namespace std;
    for (const auto& i : Eph)
    {
        fout << vformat(Fmt, make_format_args(
            i.Epoch.ToString(SimplifiedISO8601String).ToStdString(),
            i.Position.x / 1000., i.Position.y / 1000., i.Position.z / 1000.,
            i.Velocity.x / 1000., i.Velocity.y / 1000., i.Velocity.z / 1000.,
            i.Acceleration.x / 1000., i.Acceleration.y / 1000., i.Acceleration.z / 1000.));
        fout << '\n';
    }
}

void OEM::ExportCovarianceMatrix(std::ostream& fout, std::vector<ValueType::CovarianceMatrixType> Mat, cstring KVFmt, cstring MatFmt)
{
    using namespace std;
    fout << "COVARIANCE_START\n";

    for (const auto& i : Mat)
    {
        ExportKeyValue(fout, "EPOCH", i.Epoch.ToString(SimplifiedISO8601String), 0, KVFmt);
        ExportKeyValue(fout, "COV_REF_FRAME", i.RefFrame, 0, KVFmt);

        for (int j = 0; j < 6; ++j)
        {
            for (int k = 0; k <= j; ++k)
            {
                if (k) {fout << ' ';}
                fout << vformat(MatFmt, make_format_args(i.Data[k][j]));
            }
            fout << '\n';
        }

        fout << '\n';
    }

    fout << "COVARIANCE_STOP\n";
}

OEM OEM::FromString(std::string Src)
{
    OEM Result;
    std::istringstream fin(Src);
    Import(fin, &Result);
    return Result;
}

OEM OEM::FromFile(std::filesystem::path Path)
{
    OEM Result;
    std::ifstream fin(Path);
    Import(fin, &Result);
    fin.close();
    return Result;
}

void OEM::Export(std::ostream& fout, cstring KVFmt, cstring EphFmt, cstring CMFmt)const
{
    // Export header
    ExportKeyValue(fout, "CCSDS_OEM_VERS", OEMVersion, 0, KVFmt);
    ExportKeyValue(fout, "CLASSIFICATION", Classification, 1, KVFmt);
    ExportKeyValue(fout, "CREATION_DATE", CreationDate.ToString(SimplifiedISO8601String), 0, KVFmt);
    ExportKeyValue(fout, "ORIGINATOR", Originator, 0, KVFmt);
    ExportKeyValue(fout, "MESSAGE_ID", MessageID, 1, KVFmt);

    fout << '\n';

    for (const auto& i : Data)
    {
        // Export Metadata
        fout << "META_START\n";
        ExportKeyValue(fout, "OBJECT_NAME", i.MetaData.ObjectName, 0, KVFmt);
        ExportKeyValue(fout, "OBJECT_ID", i.MetaData.ObjectID, 0, KVFmt);
        ExportKeyValue(fout, "CENTER_NAME", i.MetaData.CenterName, 0, KVFmt);
        ExportKeyValue(fout, "REF_FRAME", i.MetaData.RefFrame, 0, KVFmt);
        if (i.MetaData.RefFrameEpoch.IsValid())
        {
            ExportKeyValue(fout, "REF_FRAME_EPOCH", i.MetaData.RefFrameEpoch.ToString(SimplifiedISO8601String), 1, KVFmt);
        }
        ExportKeyValue(fout, "TIME_SYSTEM", i.MetaData.TimeSystem, 0, KVFmt);
        ExportKeyValue(fout, "START_TIME", i.MetaData.StartTime.ToString(SimplifiedISO8601String), 0, KVFmt);
        if (i.MetaData.UseableStartTime.IsValid())
        {
            ExportKeyValue(fout, "USEABLE_START_TIME", i.MetaData.UseableStartTime.ToString(SimplifiedISO8601String), 0, KVFmt);
        }
        if (i.MetaData.UseableStopTime.IsValid())
        {
            ExportKeyValue(fout, "USEABLE_STOP_TIME", i.MetaData.UseableStopTime.ToString(SimplifiedISO8601String), 0, KVFmt);
        }
        ExportKeyValue(fout, "STOP_TIME", i.MetaData.StopTime.ToString(SimplifiedISO8601String), 0, KVFmt);
        ExportKeyValue(fout, "INTERPOLATION", i.MetaData.Interpolation, 1, KVFmt);
        if (i.MetaData.InterpolaDegrees)
        {
            ExportKeyValue(fout, "INTERPOLATION_DEGREE", std::to_string(i.MetaData.InterpolaDegrees), 0, KVFmt);
        }
        fout << "META_STOP\n";

        fout << '\n';

        ExportEphemeris(fout, i.Ephemeris, EphFmt);

        fout << '\n';

        if (!i.CovarianceMatrices.empty())
        {
            ExportCovarianceMatrix(fout, i.CovarianceMatrices, KVFmt, CMFmt);
            fout << '\n';
        }
    }
}

std::string OEM::ToString() const
{
    std::ostringstream fout;
    Export(fout);
    return fout.str();
}

void OEM::ToFile(std::filesystem::path Path) const
{
    std::ofstream fout(Path);
    Export(fout);
    fout.close();
}

_ORBIT_END
_CSE_END
