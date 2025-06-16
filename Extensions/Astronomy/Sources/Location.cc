#include "CSE/Base/ConstLists.h"
#include "CSE/Astronomy/Location.h"

_CSE_BEGIN

SystemBuilder::IndexTreeType SystemBuilder::__Sort_Indices(std::vector<Object> List, uint64* Barycenter)
{
    IndexTreeType Indices;
    std::map<ustring, uint64> NameBuffer;
    bool FoundBarycenter = 0;

    for (size_t i = 0; i < List.size(); ++i)
    {
        for (int j = 0; j < List[i].Name.size(); ++j)
        {
            NameBuffer.insert({List[i].Name[j], i});
        }
    }

    for (size_t i = 0; i < List.size(); ++i)
    {
        auto ParentBodyIt = NameBuffer.find(List[i].ParentBody);
        if (ParentBodyIt != NameBuffer.end())
        {
            if (ParentBodyIt->second == i)
            {
                if (FoundBarycenter)
                {
                    throw std::logic_error("System can only have 1 main barycenter.");
                }
                *Barycenter = i;
                FoundBarycenter = 1;
                continue;
            }
            Indices.insert({ParentBodyIt->second, i});
        }
    }

    if (!FoundBarycenter) {throw std::logic_error("Barycenter is not found.");}
    return Indices;
}

std::shared_ptr<StellarSystem>
    SystemBuilder::__BFS_BuildSystem(std::vector<Object> List, IndexTreeType Indices, uint64 Barycenter)
{
    std::shared_ptr<StellarSystem> TotalSystem
        = std::make_shared<StellarSystem>(StellarSystem());
    std::queue<uint64> IndexQueue;
    std::queue<std::shared_ptr<StellarSystem>> SystemQueue;

    TotalSystem->PObject = std::make_shared<Object>(List[Barycenter]);
    IndexQueue.push(Barycenter);
    SystemQueue.push(TotalSystem);

    while (!IndexQueue.empty())
    {
        uint64 CurrentIndex = IndexQueue.front();
        std::shared_ptr<StellarSystem> CurrentPointer = SystemQueue.front();
        IndexQueue.pop();
        SystemQueue.pop();

        auto it = Indices.lower_bound(CurrentIndex);
        auto end = Indices.upper_bound(CurrentIndex);
        for (; it != end; ++it)
        {
            std::shared_ptr<StellarSystem> SubSystem
                = std::make_shared<StellarSystem>(StellarSystem());
            SubSystem->PObject = std::make_shared<Object>(List[it->second]);
            CurrentPointer->PSubSystem.push_back(SubSystem);
            IndexQueue.push(it->second);
            SystemQueue.push(CurrentPointer->PSubSystem.back());
        }
    }

    return TotalSystem;
}

std::shared_ptr<StellarSystem> MakeSystem(std::vector<Object> List)
{
    uint64 BarycenterIndex;
    auto IndexTree = SystemBuilder::__Sort_Indices(List, &BarycenterIndex);
    return SystemBuilder::__BFS_BuildSystem(List, IndexTree, BarycenterIndex);
}

// ----------------------------------------------------------------------------------------------------

Sexagesimal::Sexagesimal(Angle Decimal)
{
    float64 DecimalDeg = Decimal.ToDegrees();

    Negative = DecimalDeg < 0;
    DecimalDeg = abs(DecimalDeg);

    Degrees = uint16_t(DecimalDeg);
    float64 Remaining = (DecimalDeg - Degrees) * 60;

    Minutes = uint16_t(Remaining);
    Seconds = (Remaining - Minutes) * 60;
}

Sexagesimal::operator float64()
{
    return operator Angle().Data;
}

Sexagesimal::operator Angle()
{
    return Angle::FromDegrees((Negative ? -1 : 1) *
        (float64(Degrees) + float64(Minutes) / 60.0 + Seconds / 3600.0));
}

std::ostream& operator<<(std::ostream& os, const Sexagesimal& Ang)
{
    os << (Ang.Negative ? -1 : 1) * Ang.Degrees << ' '
       << Ang.Minutes << ' ' << Ang.Seconds;
    return os;
}

std::wostream& operator<<(std::wostream& os, const Sexagesimal& Ang)
{
    os << (Ang.Negative ? -1 : 1) * Ang.Degrees << ' '
       << Ang.Minutes << ' ' << Ang.Seconds;
    return os;
}

Sexagesimal __Create_Sexagesimal_From_Seconds(float64 TS)
{
    uint64 D = uint16_t(TS / 3600);
    uint64 RS = int(TS) % 3600;
    uint64 M = RS / 60;
    float64 S = RS % 60 + (TS - int(TS));
    return Sexagesimal(0, D, M, S);
}

Sexagesimal __Convert_24_to_360(bool n, float64 d, float64 m, float64 s)
{
    float64 TS = (n ? -1 : 1) * (d * 3600 + m * 60.0 + s);
    TS *= 15;
    if (TS < 0) {TS += 360;}
    return __Create_Sexagesimal_From_Seconds(TS);
}

Sexagesimal __Convert_360_to_24(Sexagesimal Ang)
{
    float64 TS = (Ang.Negative ? -1 : 1) *
                 (Ang.Degrees * 3600 + Ang.Minutes * 60.0 + Ang.Seconds);
    TS /= 15;
    if (TS < 0) {TS += 360;}
    return __Create_Sexagesimal_From_Seconds(TS);
}

// ----------------------------------------------------------------------------------------------------

const struct __Flux_Type __Photometric_Wavelengths_Table[9]
{
    {'U', 365,   66}, {'B',  445,  94}, {'V',  551,  88},
    {'R', 658,  138}, {'G',  464, 128}, {'I',  806, 149},
    {'J', 1220, 213}, {'H', 1630, 307}, {'K', 2190, 390},
};

// ----------------------------------------------------------------------------------------------------

#if __has_include(<CSE/Parser.h>)

using namespace __scstream_table_helpers;

Location GetLocationFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    Location Loc;

    Loc.Type = KeyValue.Key;
    ustring NameStr;
    KeyValue.Value.front().GetQualified(&NameStr);
    Loc.Name = __Str_Split(NameStr);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;

        auto RAIt = __Find_Table_From_List(CurrentTable, L"RA");
        if (RAIt != CurrentTable->Get().end())
        {
            if (RAIt->Value.size() == 1)
            {
                float64 RA;
                RAIt->Value[0].GetQualified(&RA);
                Loc.RA = Angle::FromDegrees(RA * 15);
            }
            else
            {
                bool n = 0;
                float64 d = 0, m = 0, s = 0;
                if (RAIt->Value[0].Value.front()[0] == '-') {n = 1;}
                RAIt->Value[0].GetQualified(&d);
                d = abs(d);
                RAIt->Value[1].GetQualified(&m);
                if (RAIt->Value.size() == 3) {RAIt->Value[2].GetQualified(&s);}
                Loc.RA = __Convert_24_to_360(n, d, m, s);
            }
        }

        auto DecIt = __Find_Table_From_List(CurrentTable, L"Dec");
        if (DecIt != CurrentTable->Get().end())
        {
            if (DecIt->Value.size() == 1)
            {
                float64 Dec;
                DecIt->Value[0].GetQualified(&Dec);
                Loc.Dec = Angle::FromDegrees(Dec);
            }
            else
            {
                bool n = 0;
                float64 d = 0, m = 0, s = 0;
                if (DecIt->Value[0].Value.front()[0] == '-') {n = 1;}
                DecIt->Value[0].GetQualified(&d);
                d = abs(d);
                DecIt->Value[1].GetQualified(&m);
                if (RAIt->Value.size() == 3) {DecIt->Value[2].GetQualified(&s);}
                Loc.Dec = {n, uint16_t(d), uint16_t(m), s};
            }
        }

        float64 Parallax = _NoDataDbl;
        __Get_Value_From_Table(&Parallax, CurrentTable, L"Parallax", _NoDataDbl);
        Loc.Dist = 1. / (Parallax / 1000.);

        __Get_Value_With_Unit(&Loc.Dist, CurrentTable, L"Dist", _NoDataDbl, 1,
        {
            {L"Ly", LightYear / Parsec}
        });

        float64 AppMagn = _NoDataDbl;
        __Get_Value_From_Table(&AppMagn, CurrentTable, L"AppMagn", _NoDataDbl);
        if (!IS_NO_DATA_DBL(AppMagn))
        {
            Loc.AppMagn.insert({'V', AppMagn});
        }
        auto it = std::begin(__Photometric_Wavelengths_Table);
        auto end = std::end(__Photometric_Wavelengths_Table);
        for (; it != end; ++it)
        {
            ustring QueryStr = ustring(L"AppMagn") + it->PhotometricLetter;
            __Get_Value_From_Table(&AppMagn, CurrentTable, QueryStr, _NoDataDbl);
            if (!IS_NO_DATA_DBL(AppMagn))
            {
                Loc.AppMagn.insert({it->PhotometricLetter, AppMagn});
            }
        }
    }

    return Loc;
}

template<> Location GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().String) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetLocationFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(Location Loc, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable, ContentTable;
    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);
    __Add_Key_Value(&MainTable, Loc.Type, __Str_List_To_String(Loc.Name), Fl, Prec);

    auto __Add_Coordinates = [Prec](_SC SCSTable* Table, Sexagesimal RA, Sexagesimal Dec)
    {
        auto __Add_Value = [Table, Prec](auto Value)
        {
            std::wostringstream ValueStr;
            ValueStr << std::setprecision(Prec) << Value;
            Table->Get().back().Value.push_back(
            {
                .Type = _SC ValueType::ToTypeID<float64>(),
                .Value = {ustring(ValueStr.str())}
            });
        };

        __Add_Empty_Tag(Table);
        Table->Get().back().Key = "RA";
        __Add_Value((RA.Negative ? __Float64::FromBytes(__Float64(RA.Degrees).Bytes | uint64(0x8000000000000000)).x : RA.Degrees));
        __Add_Value(int(RA.Minutes));
        __Add_Value(RA.Seconds);

        __Add_Empty_Tag(Table);
        Table->Get().back().Key = "Dec";
        __Add_Value((Dec.Negative ? __Float64::FromBytes(__Float64(Dec.Degrees).Bytes | uint64(0x8000000000000000)).x : Dec.Degrees));
        __Add_Value(int(Dec.Minutes));
        __Add_Value(Dec.Seconds);
    };

    __Add_Coordinates(&ContentTable, __Convert_360_to_24(Loc.RA), Loc.Dec);
    __Add_Key_Value(&ContentTable, L"Dist", Loc.Dist, FixedOutput, Prec);

    for (auto i : Loc.AppMagn)
    {
        __Add_Key_Value(&ContentTable,
            (i.first == 'V' ? ustring(L"AppMagn") : ustring(L"AppMagn") + i.first),
            i.second, FixedOutput, Prec);
    }

    MainTable.Get().front().SubTable = std::make_shared<decltype(ContentTable)>(ContentTable);
    return MainTable;
}

// ----------------------------------------------------------------------------------------------------

StarBarycenter GetStarBarycenterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    StarBarycenter Bar;
    (Location&)Bar = GetLocationFromKeyValue(KeyValue);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Bar.DateUpdated, CurrentTable, L"DateUpdated", Bar.DateUpdated);
        __Get_Value_From_Table(&Bar.CenterOf, CurrentTable, L"CenterOf", Bar.CenterOf);
        __Get_Value_From_Table(&Bar.ForceAppMagn, CurrentTable, L"ForceAppMagn", Bar.ForceAppMagn);

        ObjectPtr_t Obj = std::make_shared<Object>(Object());

        __Get_Value_From_Table(&Obj->AbsMagn, CurrentTable, L"AbsMagn", Obj->AbsMagn);
        __Get_Value_With_Unit(&Obj->Mass, CurrentTable, L"Mass", _NoDataDbl, EarthMass,
        {
            {L"Kg", 1},
            {L"Sol", SolarMass}
        });
        vec3 Dimensions;
        __Get_Value_With_Unit(&Dimensions, CurrentTable, L"Dimensions",
            {_NoDataDbl, _NoDataDbl, _NoDataDbl}, 1000, {{L"Sol", SolarRadius}});
        if (IS_NO_DATA_DBL(Dimensions[0]))
        {
            float64 Radius;
            vec3 Flattening(0, 0, 0);
            __Get_Value_With_Unit(&Radius, CurrentTable, L"Radius", _NoDataDbl, 1000,
            {
                {L"Sol", SolarRadius}
            });
            if (IS_NO_DATA_DBL(Radius))
            {
                __Get_Value_From_Table(&Radius, CurrentTable, L"RadSol", _NoDataDbl);
                Radius *= SolarRadius;
            }
            auto OblateTable = __Find_Table_From_List(CurrentTable, L"Oblateness");
            if (OblateTable != CurrentTableEnd)
            {
                if (OblateTable->Value.front().Type & _SC ValueType::Array)
                {
                    OblateTable->Value.front().GetAsArray(&Flattening);
                }
                else {OblateTable->Value.front().GetQualified(&Flattening[1]);}
            }
            else {Flattening = {0, 0, 0};}
            Obj->Dimensions = 2. * (Radius - Radius * Flattening);
        }
        else {Obj->Dimensions = vec3(Dimensions);}

        __Get_Value_From_Table(&Obj->Temperature, CurrentTable, L"Teff", Obj->Temperature);
        if (IS_NO_DATA_DBL(Obj->Temperature))
        {
            __Get_Value_From_Table(&Obj->Temperature, CurrentTable, L"Temperature", Obj->Temperature);
        }
        __Get_Value_From_Table(&Obj->Luminosity, CurrentTable, L"Luminosity", Obj->Luminosity);
        if (IS_NO_DATA_DBL(Obj->Luminosity))
        {
            __Get_Value_From_Table(&Obj->Luminosity, CurrentTable, L"Lum", Obj->Luminosity);
        }
        __Get_Value_From_Table(&Obj->LumBol, CurrentTable, L"LumBol", Obj->LumBol);
        if (IS_NO_DATA_DBL(Obj->LumBol))
        {
            __Get_Value_From_Table(&Obj->LumBol, CurrentTable, L"LuminosityBol", Obj->LumBol);
        }
        Obj->Luminosity *= SolarLum;
        Obj->LumBol *= SolarLumBol;
        __Get_Value_From_Table(&Obj->FeH, CurrentTable, L"FeH", Obj->FeH);
        __Get_Value_From_Table(&Obj->CtoO, CurrentTable, L"CtoO", Obj->CtoO);
        __Get_Value_With_Unit(&Obj->Age, CurrentTable, L"Age", Obj->Age, 1E12, {});

        __Get_Value_From_Table(&Obj->Rotation.RotationEpoch, CurrentTable, L"RotationEpoch", Obj->Rotation.RotationEpoch);
        __Get_Value_From_Table(&Obj->Rotation.Obliquity, CurrentTable, L"Obliquity", Obj->Rotation.Obliquity);
        __Get_Value_From_Table(&Obj->Rotation.EqAscendNode, CurrentTable, L"EqAscendNode", Obj->Rotation.EqAscendNode);
        __Get_Value_From_Table(&Obj->Rotation.RotationOffset, CurrentTable, L"RotationOffset", Obj->Rotation.RotationOffset);
        __Get_Value_With_Unit(&Obj->Rotation.RotationPeriod, CurrentTable, L"RotationPeriod", Obj->Rotation.RotationPeriod, 3600, {});
        __Get_Value_With_Unit(&Obj->Rotation.Precession, CurrentTable, L"Precession", Obj->Rotation.Precession, JulianYear, {});
        if (Quadrant(Obj->Rotation.Obliquity) >= 2 &&
            Quadrant(Obj->Rotation.Obliquity) < 6 &&
            Obj->Rotation.RotationPeriod > 0)
        {
            Obj->Rotation.RotationPeriod = -Obj->Rotation.RotationPeriod;
        }

        Bar.System = std::make_shared<StellarSystem>(StellarSystem{.PObject = Obj});
    }

    return Bar;
}

template<> StarBarycenter GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().String) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetStarBarycenterFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(StarBarycenter Bar, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable = MakeTable<Location>(Bar, Fl, Prec);
    _SC SCSTable* ContentTable = MainTable.Get().front().SubTable.get();

    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);

    __Add_Key_Value(ContentTable, L"DateUpdated", Bar.DateUpdated, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"CenterOf", Bar.CenterOf, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"ForceAppMagn", Bar.ForceAppMagn, 0, Prec);

    if (Bar.Type == "Star")
    {
        auto& Obj = *Bar.System->PObject;
        __Add_Key_Value(ContentTable, L"AbsMagn", Obj.AbsMagn, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"MassSol", Obj.Mass / SolarMass, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Radius", _ASOBJ EquatorialRadius(Obj) / 1000., FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"LumBol", Obj.LumBol / SolarLumBol, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Luminosity", Obj.Luminosity / SolarLum, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Teff", Obj.Temperature, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"FeH", Obj.FeH, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"CtoO", Obj.CtoO, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Age", Obj.Age / 1E+12, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"RotationEpoch", Obj.Rotation.RotationEpoch, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Obliquity", Obj.Rotation.Obliquity, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"EqAscendNode", Obj.Rotation.EqAscendNode, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"RotationOffset", Obj.Rotation.RotationOffset, FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"RotationPeriod", Obj.Rotation.RotationPeriod / 3600., FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Oblateness", _ASOBJ Flattening(Obj), FixedOutput, Prec);
        __Add_Key_Value(ContentTable, L"Precession", Obj.Rotation.Precession / JulianYear, FixedOutput, Prec);
    }

    return MainTable;
}

// ----------------------------------------------------------------------------------------------------

DSO GetDSOFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    DSO Obj;
    (Location&)Obj = GetLocationFromKeyValue(KeyValue);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Obj.Type, CurrentTable, L"Type", Obj.Type);
        __Get_Value_From_Table(&Obj.EulerAngles.Yaw, CurrentTable, L"Yaw", Obj.EulerAngles.Yaw);
        __Get_Value_From_Table(&Obj.EulerAngles.Pitch, CurrentTable, L"Pitch", Obj.EulerAngles.Pitch);
        __Get_Value_From_Table(&Obj.EulerAngles.Roll, CurrentTable, L"Roll", Obj.EulerAngles.Roll);
        __Get_Value_From_Table(&Obj.Axis, CurrentTable, L"Axis", Obj.Axis);
        __Get_Value_From_Table(&Obj.Quat, CurrentTable, L"Quat", Obj.Quat);
        __Get_Value_From_Table(&Obj.AbsMagn, CurrentTable, L"AbsMagn", Obj.AbsMagn);
        __Get_Value_With_Unit(&Obj.Radius, CurrentTable, L"Radius", _NoDataDbl, 1,
        {
            {L"Ly", LightYear / Parsec},
        });
        __Get_Value_From_Table(&Obj.Luminosity, CurrentTable, L"Luminosity", Obj.Luminosity);
        if (IS_NO_DATA_DBL(Obj.Luminosity))
        {
            __Get_Value_From_Table(&Obj.Luminosity, CurrentTable, L"Lum", Obj.Luminosity);
        }
        __Get_Value_With_Unit(&Obj.Age, CurrentTable, L"Age", Obj.Age, 1E12, {});
    }

    return Obj;
}

template<> DSO GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().String) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetDSOFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(DSO Obj, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable = MakeTable<Location>(Obj, Fl, Prec);
    _SC SCSTable* ContentTable = MainTable.Get().front().SubTable.get();

    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);

    __Add_Key_Value(ContentTable, L"Type", Obj.Type, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Yaw", Obj.EulerAngles.Yaw, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Pitch", Obj.EulerAngles.Pitch, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Roll", Obj.EulerAngles.Roll, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Axis", Obj.Axis, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Quat", Obj.Quat, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"AbsMagn", Obj.AbsMagn, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Radius", Obj.Radius, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Luminosity", Obj.Luminosity, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Age", Obj.Age / 1E12, FixedOutput, Prec);

    return MainTable;
}

// ----------------------------------------------------------------------------------------------------

Galaxy GetGalaxyFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    Galaxy Obj;
    (DSO&)Obj = GetDSOFromKeyValue(KeyValue);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Obj.ModelBright, CurrentTable, L"ModelBright", Obj.ModelBright);
        __Get_Value_From_Table(&Obj.SolFade, CurrentTable, L"SolFade", Obj.SolFade);
    }

    return Obj;
}

template<> Galaxy GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().String) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetGalaxyFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(Galaxy Obj, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable = MakeTable<DSO>(Obj, Fl, Prec);
    _SC SCSTable* ContentTable = MainTable.Get().front().SubTable.get();

    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);

    __Add_Key_Value(ContentTable, L"Type", Obj.ModelBright, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"SolFade", Obj.SolFade, 0, Prec);

    return MainTable;
}

// ----------------------------------------------------------------------------------------------------

Cluster GetClusterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    Cluster Obj;
    (DSO&)Obj = GetDSOFromKeyValue(KeyValue);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Obj.Galaxy, CurrentTable, L"Galaxy", Obj.Galaxy);
        __Get_Value_From_Table(&Obj.CenPow, CurrentTable, L"SolFade", Obj.CenPow);
        __Get_Value_From_Table(&Obj.NStars, CurrentTable, L"NStars", Obj.NStars);
        __Get_Value_From_Table(&Obj.MaxStarAppMagn, CurrentTable, L"MaxStarAppMagn", Obj.MaxStarAppMagn);
        __Get_Value_From_Table(&Obj.FeH, CurrentTable, L"FeH", Obj.FeH);
        __Get_Value_From_Table(&Obj.Color, CurrentTable, L"Color", Obj.Color);
    }

    return Obj;
}

template<> Cluster GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().String) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetClusterFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(Cluster Obj, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable = MakeTable<DSO>(Obj, Fl, Prec);
    _SC SCSTable* ContentTable = MainTable.Get().front().SubTable.get();

    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);

    __Add_Key_Value(ContentTable, L"Galaxy", Obj.Galaxy, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"CenPow", Obj.CenPow, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"NStars", Obj.NStars, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"MaxStarAppMagn", Obj.MaxStarAppMagn, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"FeH", Obj.FeH, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Color", Obj.Color, FixedOutput, Prec);

    return MainTable;
}

// ----------------------------------------------------------------------------------------------------

Nebula GetNebulaFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    Nebula Obj;
    (DSO&)Obj = GetDSOFromKeyValue(KeyValue);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Obj.Galaxy, CurrentTable, L"Galaxy", Obj.Galaxy);
        __Get_Value_From_Table(&Obj.Randomize, CurrentTable, L"Randomize", Obj.Randomize);
        __Get_Value_From_Table(&Obj.GenerateStars, CurrentTable, L"GenerateStars", Obj.GenerateStars);
    }

    return Obj;
}

template<> Nebula GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().String) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetNebulaFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(Nebula Obj, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable = MakeTable<DSO>(Obj, Fl, Prec);
    _SC SCSTable* ContentTable = MainTable.Get().front().SubTable.get();

    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);

    __Add_Key_Value(ContentTable, L"Galaxy", Obj.Galaxy, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"Randomize", Obj.Randomize, FixedOutput, Prec);
    __Add_Key_Value(ContentTable, L"GenerateStars", Obj.GenerateStars, 1, Prec);

    return MainTable;
}

#endif

_CSE_END
