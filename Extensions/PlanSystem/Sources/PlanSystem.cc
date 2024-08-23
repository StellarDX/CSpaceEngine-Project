/*
    Planetary System data structure
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

#include "CSE/PlanSystem.h"
#include "CSE/Base/ConstLists.h"

using namespace std;

_CSE_BEGIN

SystemBuilder::IndexTreeType SystemBuilder::__Sort_Indices(vector<Object> List, uint64* Barycenter)
{
    IndexTreeType Indices;
    map<ustring, uint64> NameBuffer;
    bool FoundBarycenter = 0;

    // Insert name into name buffer
    for (size_t i = 0; i < List.size(); ++i)
    {
        for (int j = 0; j < List[i].Name.size(); ++j)
        {
            NameBuffer.insert({List[i].Name[j], i});
        }
    }

    for (size_t i = 0; i < List.size(); ++i)
    {
        // if its parent body is also contained in buffeer, insert their indices.
        auto ParentBodyIt = NameBuffer.find(List[i].ParentBody);
        if (ParentBodyIt != NameBuffer.end())
        {
            if (ParentBodyIt->second == i)
            {
                if (FoundBarycenter) {throw logic_error("System can only have 1 main barycenter.");}
                *Barycenter = i;
                FoundBarycenter = 1;
                continue;
            }
            Indices.insert({ParentBodyIt->second, i});
        }
    }

    if (!FoundBarycenter) {throw logic_error("Barycenter is not found.");}
    return Indices;
}

PlanetarySystemPointer SystemBuilder::__BFS_BuildSystem(vector<Object> List, IndexTreeType Indices, uint64 Barycenter)
{
    PlanetarySystemPointer TotalSystem = make_shared<PlanetarySystem>(PlanetarySystem());
    queue<uint64> IndexQueue;
    queue<PlanetarySystemPointer> SystemQueue;

    TotalSystem->PObject = make_shared<Object>(List[Barycenter]);
    IndexQueue.push(Barycenter);
    SystemQueue.push(TotalSystem);

    while (!IndexQueue.empty())
    {
        uint64 CurrentIndex = IndexQueue.front();
        PlanetarySystemPointer CurrentPointer = SystemQueue.front();
        IndexQueue.pop();
        SystemQueue.pop();

        auto it = Indices.lower_bound(CurrentIndex);
        auto end = Indices.upper_bound(CurrentIndex);
        for (; it != end; ++it)
        {
            PlanetarySystemPointer SubSystem = make_shared<PlanetarySystem>(PlanetarySystem());
            SubSystem->PObject = make_shared<Object>(List[it->second]);
            CurrentPointer->PSubSystem.push_back(SubSystem);
            IndexQueue.push(it->second);
            SystemQueue.push(CurrentPointer->PSubSystem.back());
        }
    }

    return TotalSystem;
}

#ifdef _CSE_SCSTREAM

using namespace __scstream_table_helpers;

StarBarycenter GetStarBarycenterFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    StarBarycenter Bar;

    Bar.Type = KeyValue.Key;
    ustring NameStr;
    KeyValue.Value.front().GetQualified(&NameStr);
    Bar.Name = __Str_Split(NameStr);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Bar.DateUpdated, CurrentTable, L"DateUpdated", Bar.DateUpdated);
        __Get_Value_From_Table(&Bar.CenterOf, CurrentTable, L"CenterOf", Bar.CenterOf);

        auto RAIt = __Find_Table_From_List(CurrentTable, L"RA");
        if (RAIt != CurrentTable->Get().end())
        {
            bool n = 0;
            float64 d, m, s;
            if (RAIt->Value[0].Value.front()[0] == '-') {n = 1;}
            RAIt->Value[0].GetQualified(&d);
            d = abs(d);
            RAIt->Value[1].GetQualified(&m);
            RAIt->Value[2].GetQualified(&s);
            Bar.RA = ConstructSexagesimal(n, d, m, s);
        }

        auto DecIt = __Find_Table_From_List(CurrentTable, L"Dec");
        if (DecIt != CurrentTable->Get().end())
        {
            bool n = 0;
            float64 d, m, s;
            if (DecIt->Value[0].Value.front()[0] == '-') {n = 1;}
            DecIt->Value[0].GetQualified(&d);
            d = abs(d);
            DecIt->Value[1].GetQualified(&m);
            DecIt->Value[2].GetQualified(&s);
            Bar.Dec = ConstructSexagesimal(n, d, m, s);
        }

        float64 Dist = _NoDataDbl;
        __Get_Value_With_Unit(&Dist, CurrentTable, L"Dist", _NoDataDbl, Parsec,
        {
            {L"Ly", LightYear}
        });
        Bar.Parallax = 1000. / (Dist / Parsec);

        __Get_Value_From_Table(&Bar.Parallax, CurrentTable, L"Parallax", Bar.Parallax);
        __Get_Value_From_Table(&Bar.AppMagn, CurrentTable, L"AppMagn", Bar.AppMagn);

        ObjectPointer Obj = std::make_shared<Object>(Object());

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

        __Get_Value_From_Table(&Obj->Rotation.RotationEpoch, CurrentTable, L"RotationEpoch", float64(J2000));
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

        Bar.System = std::make_shared<PlanetarySystem>(PlanetarySystem{.PObject = Obj});
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
    _SC SCSTable MainTable, ContentTable;
    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);
    bool NoBooleans = !(Fl & __Object_Manipulator::Booleans);
    __Add_Key_Value(&MainTable, Bar.Type, __Str_List_To_String(Bar.Name), Fl, Prec);

    __Add_Key_Value(&ContentTable, L"DateUpdated", Bar.DateUpdated, FixedOutput, Prec);
    __Add_Key_Value(&ContentTable, L"CenterOf", Bar.CenterOf, FixedOutput, Prec);

    auto __Add_Coordinates = [Prec](_SC SCSTable* Table, Sexagesimal RA, Sexagesimal Dec)
    {
        auto __Add_Value = [Table, Prec](auto Value)
        {
            std::wostringstream ValueStr;
            ValueStr << setprecision(Prec) << Value;
            Table->Get().back().Value.push_back(
            {
                .Type = _SC ValueType::ToTypeID<float64>(),
                .Value = {ustring(ValueStr.str())}
            });
        };

        __Add_Empty_Tag(Table);
        Table->Get().back().Key = "RA";
        __Add_Value(RA.Degrees);
        __Add_Value(RA.Minutes);
        __Add_Value(RA.Seconds);

        __Add_Empty_Tag(Table);
        Table->Get().back().Key = "Dec";
        __Add_Value((Dec.Negative ? __Float64::FromBytes(__Float64(Dec.Degrees).Bytes | uint64(0x8000000000000000)).x : Dec.Degrees));
        __Add_Value(Dec.Minutes);
        __Add_Value(Dec.Seconds);
    };

    __Add_Coordinates(&ContentTable, Bar.RA, Bar.Dec);
    __Add_Key_Value(&ContentTable, L"Parallax", Bar.Parallax, FixedOutput, Prec);
    __Add_Key_Value(&ContentTable, L"AppMagn", Bar.AppMagn, FixedOutput, Prec);

    if (Bar.Type == "Star")
    {
        auto& Obj = *Bar.System->PObject;
        __Add_Key_Value(&ContentTable, L"AbsMagn", Obj.AbsMagn, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"MassSol", Obj.Mass / SolarMass, FixedOutput, Prec);
        auto Radius = vec3(Obj.Dimensions / 2.);
        auto MaxRad = cse::max({Radius.x, Radius.y, Radius.z});
        __Add_Key_Value(&ContentTable, L"Radius", MaxRad / 1000., FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"LumBol", Obj.LumBol / SolarLumBol, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Luminosity", Obj.Luminosity / SolarLum, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Teff", Obj.Temperature, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"FeH", Obj.FeH, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"CtoO", Obj.CtoO, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Age", Obj.Age / 1E+12, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"RotationEpoch", Obj.Rotation.RotationEpoch, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Obliquity", Obj.Rotation.Obliquity, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"EqAscendNode", Obj.Rotation.EqAscendNode, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"RotationOffset", Obj.Rotation.RotationOffset, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"RotationPeriod", Obj.Rotation.RotationPeriod / 3600., FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Oblateness", _ASOBJ Flattening(Obj), FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Precession", Obj.Rotation.Precession / JulianYear, FixedOutput, Prec);
    }

    MainTable.Get().front().SubTable = make_shared<decltype(ContentTable)>(ContentTable);
    return MainTable;
}

#endif

_CSE_END
