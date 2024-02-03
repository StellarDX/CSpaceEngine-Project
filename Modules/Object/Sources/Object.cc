#include "CSE/CSEBase/AdvMath.h"
#include "CSE/CSEBase/ConstLists.h"
#include "CSE/Object.h"

#if __has_include(<CSE/SCStream.h>)
#include <CSE/SCStream.h>
#endif

using namespace std;

_CSE_BEGIN

#ifdef _CSE_SCSTREAM

Object GetObjectFromKeyValue(_SC SCSTable::SCKeyValue KeyValue)
{
    Object Obj;

    Obj.Type = KeyValue.Key;
    ustring NameStr;
    KeyValue.Value.front().GetQualified(&NameStr);
    Obj.Name = __Str_Split(NameStr);

    if (KeyValue.SubTable)
    {
        auto& CurrentTable = KeyValue.SubTable;
        auto CurrentTableEnd = CurrentTable->Get().end();

        __Get_Value_From_Table(&Obj.DateUpdated, CurrentTable, L"DateUpdated", _TIME CSEDate());
        __Get_Value_From_Table(&Obj.DiscMethod, CurrentTable, L"DiscMethod", ustring(_NoDataStr));
        __Get_Value_From_Table(&Obj.DiscDate, CurrentTable, L"DiscDate", _TIME CSEDate());
        __Get_Value_From_Table(&Obj.ParentBody, CurrentTable, L"ParentBody", ustring(_NoDataStr));
        if (Obj.Type == L"Star")
        {
            Obj.Class = L"Sun";
            __Get_Value_From_Table(&Obj.SpecClass, CurrentTable, L"Class", ustring(_NoDataStr));
        }
        else {__Get_Value_From_Table(&Obj.Class, CurrentTable, L"Class", ustring(_NoDataStr));}
        __Get_Value_From_Table(&Obj.AsterType, CurrentTable, L"AsterType", ustring(_NoDataStr));

        __Get_Value_With_Unit(&Obj.Mass, CurrentTable, L"Mass", _NoDataDbl, EarthMass,
        {
            {L"Kg", 1},
            {L"Sol", SolarMass}
        });
        array<float64, 3> Dimensions;
        __Get_Value_With_Unit(&Dimensions, CurrentTable, L"Dimensions",
        {_NoDataDbl, _NoDataDbl, _NoDataDbl}, 1000, {{L"Sol", SolarRadius}});
        if (IS_NO_DATA_DBL(Dimensions[0]))
        {
            float64 Radius;
            array<float64, 3> Flattening;
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
            Obj.Dimensions = 2. * (Radius - Radius * vec3(Flattening));
        }
        else {Obj.Dimensions = vec3(Dimensions);}
        __Get_Value_From_Table(&Obj.InertiaMoment, CurrentTable, L"InertiaMoment", _NoDataDbl);
        __Get_Value_From_Table(&Obj.AlbedoBond, CurrentTable, L"AlbedoBond", _NoDataDbl);
        __Get_Value_From_Table(&Obj.AlbedoGeom, CurrentTable, L"AlbedoGeom", _NoDataDbl);
        __Get_Value_From_Table(&Obj.EndogenousHeating, CurrentTable, L"EndogenousHeating", _NoDataDbl);
        __Get_Value_From_Table(&Obj.ThermalLuminosity, CurrentTable, L"ThermalLuminosity", _NoDataDbl);
        __Get_Value_From_Table(&Obj.ThermalLuminosityBol, CurrentTable, L"ThermalLuminosityBol", _NoDataDbl);
        __Get_Value_From_Table(&Obj.Temperature, CurrentTable, L"Teff", _NoDataDbl);
        if (IS_NO_DATA_DBL(Obj.Temperature))
        {
            __Get_Value_From_Table(&Obj.Temperature, CurrentTable, L"Temperature", _NoDataDbl);
        }
        __Get_Value_From_Table(&Obj.Luminosity, CurrentTable, L"Luminosity", _NoDataDbl);
        if (IS_NO_DATA_DBL(Obj.Luminosity))
        {
            __Get_Value_From_Table(&Obj.Luminosity, CurrentTable, L"Lum", _NoDataDbl);
        }
        __Get_Value_From_Table(&Obj.LumBol, CurrentTable, L"LumBol", _NoDataDbl);
        if (IS_NO_DATA_DBL(Obj.LumBol))
        {
            __Get_Value_From_Table(&Obj.LumBol, CurrentTable, L"LuminosityBol", _NoDataDbl);
        }
        Obj.Luminosity *= SolarLum;
        Obj.LumBol *= SolarLumBol;
        __Get_Value_From_Table(&Obj.FeH, CurrentTable, L"FeH", _NoDataDbl);
        __Get_Value_From_Table(&Obj.CtoO, CurrentTable, L"CtoO", _NoDataDbl);
        __Get_Value_With_Unit(&Obj.Age, CurrentTable, L"Age", _NoDataDbl, 1E12, {});
        __Get_Value_From_Table(&Obj.KerrSpin, CurrentTable, L"KerrSpin", _NoDataDbl);
        __Get_Value_From_Table(&Obj.KerrCharge, CurrentTable, L"KerrCharge", _NoDataDbl);

        std::array<float64, 3> Color;
        __Get_Value_From_Table(&Color, CurrentTable, L"Color", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
        Obj.Color = vec3(Color);
        __Get_Value_From_Table(&Obj.Brightness, CurrentTable, L"Brightness", _NoDataDbl);
        __Get_Value_From_Table(&Obj.BrightnessReal, CurrentTable, L"BrightnessReal", _NoDataDbl);

        auto RotModelTable = __Find_Table_From_List(CurrentTable, L"RotationModel");
        if (RotModelTable != CurrentTableEnd)
        {
            RotModelTable->Value.front().GetQualified(&Obj.RotationModel);
        }
        else {Obj.RotationModel = L"Simple";}
        //if (Obj.RotationModel == L"Simple")
        //{
            __Get_Value_From_Table(&Obj.Rotation.RotationEpoch, CurrentTable, L"RotationEpoch", float64(J2000));
            __Get_Value_From_Table(&Obj.Rotation.Obliquity, CurrentTable, L"Obliquity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rotation.EqAscendNode, CurrentTable, L"EqAscendNode", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rotation.RotationOffset, CurrentTable, L"RotationOffset", _NoDataDbl);
            __Get_Value_With_Unit(&Obj.Rotation.RotationPeriod, CurrentTable, L"RotationPeriod", _NoDataDbl, 3600, {});
            __Get_Value_With_Unit(&Obj.Rotation.Precession, CurrentTable, L"Precession", _NoDataDbl, JulianYear, {});
            __Get_Value_From_Table(&Obj.Rotation.TidalLocked, CurrentTable, L"TidalLocked", false);
        //}
        if (Obj.RotationModel == L"IAU" && RotModelTable->SubTable)
        {
            auto RotModelSubTable = RotModelTable->SubTable;
            auto RotModelSubTableEnd = RotModelSubTable->Get().end();
            __Get_Value_From_Table(&Obj.RotationIAU.Epoch, RotModelSubTable, L"Epoch", float64(J2000));
            __Get_Value_From_Table(&Obj.RotationIAU.PoleRA, RotModelSubTable, L"PoleRA", _NoDataDbl);
            __Get_Value_From_Table(&Obj.RotationIAU.PoleRARate, RotModelSubTable, L"PoleRARate", _NoDataDbl);
            __Get_Value_From_Table(&Obj.RotationIAU.PoleDec, RotModelSubTable, L"PoleDec", _NoDataDbl);
            __Get_Value_From_Table(&Obj.RotationIAU.PoleDecRate, RotModelSubTable, L"PoleDecRate", _NoDataDbl);
            __Get_Value_From_Table(&Obj.RotationIAU.PrimeMeridian, RotModelSubTable, L"PrimeMeridian", _NoDataDbl);
            __Get_Value_From_Table(&Obj.RotationIAU.RotationRate, RotModelSubTable, L"RotationRate", _NoDataDbl);
            __Get_Value_From_Table(&Obj.RotationIAU.RotationAccel, RotModelSubTable, L"RotationAccel", _NoDataDbl);
            auto PTTable = __Find_Table_With_Unit(RotModelSubTable, L"PeriodicTerms");
            if (PTTable != RotModelSubTableEnd)
            {
                if (PTTable->Key == L"PeriodicTermsSecular") {Obj.RotationIAU.UsingSecular = true;}
                size_t len = 6;
                size_t count = PTTable->Value.front().Value.size();
                std::array<float64, 6> Buffer;
                for (int i = 0; i < count; ++i)
                {
                    Buffer[i % 6] = stod(PTTable->Value.front().Value[i]);
                    if (i % len == len - 1)
                    {
                        Obj.RotationIAU.PeriodicTerms.push_back(Buffer);
                    }
                }
            }
        }

        auto StaticPosTable = __Find_Table_With_Unit(CurrentTable, L"StaticPos");
        if (StaticPosTable != CurrentTableEnd)
        {
            std::array<float64, 3> StaticPos;
            StaticPosTable->Value.front().GetAsArray(&StaticPos);
            if (StaticPosTable->Key == L"StaticPosPolar")
            {
                Obj.Position = PolarToXYZ(vec3(StaticPos));
            }
            else {Obj.Position = vec3(StaticPos);}
            Obj.Orbit.RefPlane = L"Static";
        }
        auto FixedPosTable = __Find_Table_With_Unit(CurrentTable, L"Fixed");
        if (FixedPosTable != CurrentTableEnd)
        {
            std::array<float64, 3> FixedPos;
            FixedPosTable->Value.front().GetAsArray(&FixedPos);
            if (StaticPosTable->Key == L"FixedPosPolar")
            {
                Obj.Position = PolarToXYZ(vec3(FixedPos));
            }
            else {Obj.Position = vec3(FixedPos);}
            Obj.Orbit.RefPlane = L"Fixed";
        }
        auto OrbitTable = __Find_Table_With_KeyWord(CurrentTable, L"Orbit");
        if (OrbitTable != CurrentTableEnd)
        {
            auto OrbitSubTable = OrbitTable->SubTable;
            if (OrbitTable->Key == L"BinaryOrbit") {Obj.Orbit.Binary = true;}
            __Get_Value_From_Table(&Obj.Orbit.Separation, OrbitSubTable, L"Separation", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.PositionAngle, OrbitSubTable, L"PositionAngle", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.AnalyticModel, OrbitSubTable, L"AnalyticModel", ustring(_NoDataStr));
            if (Obj.Orbit.AnalyticModel != _NoDataStr) {Obj.Orbit.RefPlane = L"Analytic";}
            __Get_Value_From_Table(&Obj.Orbit.Epoch, OrbitSubTable, L"Epoch", float64(J2000));
            __Get_Value_With_Unit(&Obj.Orbit.Period, OrbitSubTable, L"Period", _NoDataDbl, JulianYear, {{L"Days", SynodicDay}});
            __Get_Value_With_Unit(&Obj.Orbit.PericenterDist, OrbitSubTable, L"PericenterDist", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_From_Table(&Obj.Orbit.Eccentricity, OrbitSubTable, L"Eccentricity", _NoDataDbl);
            if (IS_NO_DATA_DBL(Obj.Orbit.PericenterDist))
            {
                float64 SemiMajorAxis;
                __Get_Value_With_Unit(&SemiMajorAxis, OrbitSubTable, L"SemiMajorAxis", _NoDataDbl, AU, {{L"Km", 1000}});
                Obj.Orbit.PericenterDist = abs(SemiMajorAxis - SemiMajorAxis *
                    (IS_NO_DATA_DBL(Obj.Orbit.Eccentricity) ? 1 : Obj.Orbit.Eccentricity));
            }
            __Get_Value_From_Table(&Obj.Orbit.GravParam, OrbitSubTable, L"GravParam", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.Inclination, OrbitSubTable, L"Inclination", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.AscendingNode, OrbitSubTable, L"AscendingNode", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.AscNodePreces, OrbitSubTable, L"AscNodePreces", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.ArgOfPericenter, OrbitSubTable, L"ArgOfPericenter", _NoDataDbl);
            if (IS_NO_DATA_DBL(Obj.Orbit.ArgOfPericenter))
            {
                __Get_Value_From_Table(&Obj.Orbit.ArgOfPericenter, OrbitSubTable, L"ArgOfPericen", _NoDataDbl);
            }
            if (IS_NO_DATA_DBL(Obj.Orbit.ArgOfPericenter))
            {
                float64 LongOfPericen;
                __Get_Value_From_Table(&LongOfPericen, OrbitSubTable, L"LongOfPericen", _NoDataDbl);
                Obj.Orbit.ArgOfPericenter = LongOfPericen -
                    (IS_NO_DATA_DBL(Obj.Orbit.AscendingNode) ? 0 : Obj.Orbit.AscendingNode);
            }
            __Get_Value_From_Table(&Obj.Orbit.ArgOfPeriPreces, OrbitSubTable, L"ArgOfPeriPreces", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Orbit.MeanAnomaly, OrbitSubTable, L"MeanAnomaly", _NoDataDbl);
            if (IS_NO_DATA_DBL(Obj.Orbit.MeanAnomaly))
            {
                float64 MeanLongitude;
                __Get_Value_From_Table(&MeanLongitude, OrbitSubTable, L"MeanLongitude", _NoDataDbl);
                Obj.Orbit.MeanAnomaly = MeanLongitude -
                    (IS_NO_DATA_DBL(Obj.Orbit.ArgOfPericenter) ? 0 : Obj.Orbit.ArgOfPericenter) -
                    (IS_NO_DATA_DBL(Obj.Orbit.AscendingNode) ? 0 : Obj.Orbit.AscendingNode);
            }
        }

        __Get_Value_From_Table(&Obj.NoLife, CurrentTable, L"NoLife", false);
        auto LifeTables = __Find_Multi_Tables_From_List(CurrentTable, L"Life");
        if (!Obj.NoLife && !LifeTables.empty())
        {
            auto GetLifeFromTable = [](_SC SharedTablePointer Table, Object::LifeParams* Dst)
            {
                __Get_Value_From_Table(&Dst->Class, Table, L"Class", ustring(_NoDataStr));
                __Get_Value_From_Table(&Dst->Type, Table, L"Type", ustring(_NoDataStr));
                ustring Biomes;
                __Get_Value_From_Table(&Biomes, Table, L"Biome", ustring(_NoDataStr));
                Dst->Biome = __Str_Split(Biomes);
                __Get_Value_From_Table(&Dst->Panspermia, Table, L"Panspermia", false);
            };
            switch (LifeTables.size())
            {
            case 2:
                GetLifeFromTable(LifeTables[1]->SubTable, &Obj.Life[1]);
            case 1:
                GetLifeFromTable(LifeTables[0]->SubTable, &Obj.Life[0]);
                break;
            default:
                break;
            }
        }

        auto TableToCompositeMap = [](_SC SharedTablePointer Table)
        {
            CompositionType Composition;
            auto it = Table->Get().begin();
            auto end = Table->Get().end();
            for (; it != end; ++it)
            {
                float64 Value;
                it->Value.front().GetQualified(&Value);
                Composition.insert({it->Key, Value});
            }
            return Composition;
        };

        auto InteriorTable = __Find_Table_From_List(CurrentTable, L"Interior");
        if (InteriorTable != CurrentTableEnd)
        {
            auto InteriorSubTable = InteriorTable->SubTable;
            auto CompTable = __Find_Table_From_List(InteriorSubTable, L"Composition");
            if (CompTable != InteriorSubTable->Get().end()) {Obj.Interior = TableToCompositeMap(CompTable->SubTable);}
        }

        auto SurfaceTable = __Find_Table_From_List(CurrentTable, L"Surface");
        if (SurfaceTable != CurrentTableEnd)
        {
            Obj.EnableSurface = true;
            auto SurfaceSubTable = SurfaceTable->SubTable;
            __Get_Value_From_Table(&Obj.Surface.DiffMap, SurfaceSubTable, L"DiffMap", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Surface.DiffMapAlpha, SurfaceSubTable, L"DiffMapAlpha", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Surface.BumpMap, SurfaceSubTable, L"BumpMap", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Surface.BumpHeight, SurfaceSubTable, L"BumpHeight", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.BumpOffset, SurfaceSubTable, L"BumpOffset", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.GlowMap, SurfaceSubTable, L"GlowMap", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Surface.GlowMode, SurfaceSubTable, L"GlowMode", ustring(_NoDataStr));
            std::array<float64, 3> GlowColor;
            __Get_Value_From_Table(&GlowColor, SurfaceSubTable, L"GlowColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Surface.GlowColor = vec3(GlowColor);
            __Get_Value_From_Table(&Obj.Surface.GlowBright, SurfaceSubTable, L"GlowBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpecMap, SurfaceSubTable, L"SpecMap", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Surface.FlipMap, SurfaceSubTable, L"FlipMap", false);
            __Get_Value_From_Table(&Obj.Surface.Gamma, SurfaceSubTable, L"Gamma", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.Hapke, SurfaceSubTable, L"Hapke", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpotBright, SurfaceSubTable, L"SpotBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpotWidth, SurfaceSubTable, L"SpotWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpotBrightCB, SurfaceSubTable, L"SpotBrightCB", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpotWidthCB, SurfaceSubTable, L"SpotWidthCB", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.TempMapMaxTemp, SurfaceSubTable, L"TempMapMaxTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.DayAmbient, SurfaceSubTable, L"DayAmbient", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.RingsWinter, SurfaceSubTable, L"RingsWinter", _NoDataDbl);
            std::array<float64, 3> ModulateColor;
            __Get_Value_From_Table(&ModulateColor, SurfaceSubTable, L"ModulateColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Surface.ModulateColor = vec3(ModulateColor);
            __Get_Value_From_Table(&Obj.Surface.ModulateBright, SurfaceSubTable, L"ModulateBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpecBrightWater, SurfaceSubTable, L"SpecBrightWater", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpecBrightIce, SurfaceSubTable, L"SpecBrightIce", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.RoughnessWater, SurfaceSubTable, L"RoughnessWater", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.RoughnessIce, SurfaceSubTable, L"RoughnessIce", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.SpecularScale, SurfaceSubTable, L"SpecularScale", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.RoughnessBias, SurfaceSubTable, L"RoughnessBias", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.RoughnessScale, SurfaceSubTable, L"RoughnessScale", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.Preset, SurfaceSubTable, L"Preset", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Surface.SurfStyle, SurfaceSubTable, L"SurfStyle", _NoDataDbl);
            std::array<float64, 3> Randomize;
            __Get_Value_From_Table(&Randomize, SurfaceSubTable, L"Randomize", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Surface.Randomize = vec3(Randomize);
            __Get_Value_From_Table(&Obj.Surface.colorDistMagn, SurfaceSubTable, L"colorDistMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.colorDistFreq, SurfaceSubTable, L"colorDistFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.detailScale, SurfaceSubTable, L"detailScale", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.drivenDarkening, SurfaceSubTable, L"drivenDarkening", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.seaLevel, SurfaceSubTable, L"seaLevel", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.snowLevel, SurfaceSubTable, L"snowLevel", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.tropicLatitude, SurfaceSubTable, L"tropicLatitude", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.icecapLatitude, SurfaceSubTable, L"icecapLatitude", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.icecapHeight, SurfaceSubTable, L"icecapHeight", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climatePole, SurfaceSubTable, L"climatePole", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateTropic, SurfaceSubTable, L"climateTropic", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateEquator, SurfaceSubTable, L"climateEquator", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateSteppeMin, SurfaceSubTable, L"climateSteppeMin", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateSteppeMax, SurfaceSubTable, L"climateSteppeMax", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateForestMin, SurfaceSubTable, L"climateForestMin", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateForestMax, SurfaceSubTable, L"climateForestMax", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateGrassMin, SurfaceSubTable, L"climateGrassMin", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.climateGrassMax, SurfaceSubTable, L"climateGrassMax", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.humidity, SurfaceSubTable, L"humidity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.heightTempGrad, SurfaceSubTable, L"heightTempGrad", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.beachWidth, SurfaceSubTable, L"beachWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.tropicWidth, SurfaceSubTable, L"tropicWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.mainFreq, SurfaceSubTable, L"mainFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.venusFreq, SurfaceSubTable, L"venusFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.venusMagn, SurfaceSubTable, L"venusMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.mareFreq, SurfaceSubTable, L"mareFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.mareDensity, SurfaceSubTable, L"mareDensity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.terraceProb, SurfaceSubTable, L"terraceProb", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.erosion, SurfaceSubTable, L"erosion", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.montesMagn, SurfaceSubTable, L"montesMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.montesFreq, SurfaceSubTable, L"montesFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.montesSpiky, SurfaceSubTable, L"montesSpiky", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.montesFraction, SurfaceSubTable, L"montesFraction", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.dunesMagn, SurfaceSubTable, L"dunesMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.dunesFreq, SurfaceSubTable, L"dunesFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.dunesFraction, SurfaceSubTable, L"dunesFraction", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.hillsMagn, SurfaceSubTable, L"hillsMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.hillsFreq, SurfaceSubTable, L"hillsFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.hillsFraction, SurfaceSubTable, L"hillsFraction", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.hills2Fraction, SurfaceSubTable, L"hills2Fraction", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.riversMagn, SurfaceSubTable, L"riversMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.riversFreq, SurfaceSubTable, L"riversFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.riversSin, SurfaceSubTable, L"riversSin", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.riftsMagn, SurfaceSubTable, L"riftsMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.riftsFreq, SurfaceSubTable, L"riftsFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.riftsSin, SurfaceSubTable, L"riftsSin", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.eqridgeMagn, SurfaceSubTable, L"eqridgeMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.eqridgeWidth, SurfaceSubTable, L"eqridgeWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.eqridgeModMagn, SurfaceSubTable, L"eqridgeModMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.eqridgeModFreq, SurfaceSubTable, L"eqridgeModFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.canyonsMagn, SurfaceSubTable, L"canyonsMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.canyonsFreq, SurfaceSubTable, L"canyonsFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.canyonsFraction, SurfaceSubTable, L"canyonsFraction", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.cracksMagn, SurfaceSubTable, L"cracksMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.cracksFreq, SurfaceSubTable, L"cracksFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.cracksOctaves, SurfaceSubTable, L"cracksOctaves", _NoDataInt);
            __Get_Value_From_Table(&Obj.Surface.craterMagn, SurfaceSubTable, L"craterMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.craterFreq, SurfaceSubTable, L"craterFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.craterDensity, SurfaceSubTable, L"craterDensity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.craterOctaves, SurfaceSubTable, L"craterOctaves", _NoDataInt);
            __Get_Value_From_Table(&Obj.Surface.craterRayedFactor, SurfaceSubTable, L"craterRayedFactor", _NoDataInt);
            __Get_Value_From_Table(&Obj.Surface.volcanoMagn, SurfaceSubTable, L"volcanoMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.volcanoFreq, SurfaceSubTable, L"volcanoFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.volcanoDensity, SurfaceSubTable, L"volcanoDensity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.volcanoOctaves, SurfaceSubTable, L"volcanoOctaves", _NoDataInt);
            __Get_Value_From_Table(&Obj.Surface.volcanoActivity, SurfaceSubTable, L"volcanoActivity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.volcanoFlows, SurfaceSubTable, L"volcanoFlows", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.volcanoRadius, SurfaceSubTable, L"volcanoRadius", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.volcanoTemp, SurfaceSubTable, L"volcanoTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.lavaCoverTidal, SurfaceSubTable, L"lavaCoverTidal", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.lavaCoverSun, SurfaceSubTable, L"lavaCoverSun", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Surface.lavaCoverYoung, SurfaceSubTable, L"lavaCoverYoung", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.stripeZones, SurfaceSubTable, L"stripeZones", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.stripeFluct, SurfaceSubTable, L"stripeFluct", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.stripeTwist, SurfaceSubTable, L"stripeTwist", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneMagn, SurfaceSubTable, L"cycloneMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneFreq, SurfaceSubTable, L"cycloneFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneDensity, SurfaceSubTable, L"cycloneDensity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneOctaves, SurfaceSubTable, L"cycloneOctaves", _NoDataInt);
            __Get_Value_From_Table(&Obj.Clouds.cycloneMagn2, SurfaceSubTable, L"cycloneMagn2", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneFreq2, SurfaceSubTable, L"cycloneFreq2", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneDensity2, SurfaceSubTable, L"cycloneDensity2", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.cycloneOctaves2, SurfaceSubTable, L"cycloneOctaves2", _NoDataInt);
        }

        __Get_Value_From_Table(&Obj.NoOcean, CurrentTable, L"NoOcean", false);
        auto OceanTable = __Find_Table_From_List(CurrentTable, L"Ocean");
        if (!Obj.NoOcean && OceanTable != CurrentTableEnd)
        {
            auto OceanSubTable = OceanTable->SubTable;
            __Get_Value_From_Table(&Obj.Ocean.Height, OceanSubTable, L"Height", _NoDataDbl);
            if (IS_NO_DATA_DBL(Obj.Ocean.Height))
            {
                __Get_Value_From_Table(&Obj.Ocean.Height, OceanSubTable, L"Depth", _NoDataDbl);
            }
            __Get_Value_From_Table(&Obj.Ocean.Hapke, OceanSubTable, L"Hapke", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.SpotBright, OceanSubTable, L"SpotBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.SpotWidth, OceanSubTable, L"SpotWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.SpotBrightCB, OceanSubTable, L"SpotBrightCB", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.SpotWidthCB, OceanSubTable, L"SpotWidthCB", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.TempMapMaxTemp, OceanSubTable, L"TempMapMaxTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.DayAmbient, OceanSubTable, L"DayAmbient", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Ocean.RingsWinter, OceanSubTable, L"RingsWinter", _NoDataDbl);
            std::array<float64, 3> ModulateColor;
            __Get_Value_From_Table(&ModulateColor, OceanSubTable, L"ModulateColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Ocean.ModulateColor = vec3(ModulateColor);
            __Get_Value_From_Table(&Obj.Ocean.ModulateBright, OceanSubTable, L"ModulateBright", _NoDataDbl);
            auto CompTable = __Find_Table_From_List(OceanSubTable, L"Composition");
            if (CompTable != OceanSubTable->Get().end()) {Obj.Ocean.Composition = TableToCompositeMap(CompTable->SubTable);}
        }

        __Get_Value_From_Table(&Obj.NoClouds, CurrentTable, L"NoClouds", false);
        auto CloudsTables = __Find_Multi_Tables_From_List(CurrentTable, L"Clouds");
        if (!Obj.NoClouds && !CloudsTables.empty())
        {
            for (int i = 0; i < CloudsTables.size(); ++i)
            {
                auto CloudsTable = CloudsTables[i]->SubTable;
                Object::CloudParams::CloudLayerParam CloudLayer;
                __Get_Value_From_Table(&CloudLayer.DiffMap, CloudsTable, L"DiffMap", ustring(_NoDataStr));
                __Get_Value_From_Table(&CloudLayer.DiffMapAlpha, CloudsTable, L"DiffMapAlpha", ustring(_NoDataStr));
                __Get_Value_From_Table(&CloudLayer.BumpMap, CloudsTable, L"BumpMap", ustring(_NoDataStr));
                __Get_Value_From_Table(&CloudLayer.BumpHeight, CloudsTable, L"BumpHeight", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.BumpOffset, CloudsTable, L"BumpOffset", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.GlowMap, CloudsTable, L"GlowMap", ustring(_NoDataStr));
                __Get_Value_From_Table(&CloudLayer.GlowMode, CloudsTable, L"GlowMode", ustring(_NoDataStr));
                std::array<float64, 3> GlowColor;
                __Get_Value_From_Table(&GlowColor, CloudsTable, L"GlowColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
                CloudLayer.GlowColor = vec3(GlowColor);
                __Get_Value_From_Table(&CloudLayer.GlowBright, CloudsTable, L"GlowBright", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SpecMap, CloudsTable, L"SpecMap", ustring(_NoDataStr));
                __Get_Value_From_Table(&CloudLayer.FlipMap, CloudsTable, L"FlipMap", false);
                __Get_Value_From_Table(&CloudLayer.Gamma, CloudsTable, L"Gamma", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.Hapke, CloudsTable, L"Hapke", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SpotBright, CloudsTable, L"SpotBright", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SpotWidth, CloudsTable, L"SpotWidth", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SpotBrightCB, CloudsTable, L"SpotBrightCB", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SpotWidthCB, CloudsTable, L"SpotWidthCB", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.TempMapMaxTemp, CloudsTable, L"TempMapMaxTemp", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.DayAmbient, CloudsTable, L"DayAmbient", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.RingsWinter, CloudsTable, L"RingsWinter", _NoDataDbl);
                std::array<float64, 3> ModulateColor;
                __Get_Value_From_Table(&ModulateColor, CloudsTable, L"ModulateColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
                CloudLayer.ModulateColor = vec3(ModulateColor);
                __Get_Value_From_Table(&CloudLayer.ModulateBright, CloudsTable, L"ModulateBright", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.Height, CloudsTable, L"Height", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.Velocity, CloudsTable, L"Velocity", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.RotationOffset, CloudsTable, L"RotationOffset", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SubsurfScatBright, CloudsTable, L"SubsurfScatBright", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.SubsurfScatPower, CloudsTable, L"SubsurfScatPower", _NoDataDbl);
                __Get_Value_From_Table(&CloudLayer.Opacity, CloudsTable, L"Opacity", _NoDataDbl);
                Obj.Clouds.Layers.push_back(CloudLayer);
            }
            auto FirstCloudsTable = CloudsTables[0]->SubTable;
            __Get_Value_From_Table(&Obj.Clouds.TidalLocked, FirstCloudsTable, L"TidalLocked", false);
            __Get_Value_From_Table(&Obj.Clouds.Coverage, FirstCloudsTable, L"Coverage", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.mainFreq, FirstCloudsTable, L"mainFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Clouds.mainOctaves, FirstCloudsTable, L"mainOctaves", _NoDataInt);
            __Get_Value_From_Table(&Obj.Clouds.stripeZones, FirstCloudsTable, L"stripeZones", Obj.Clouds.stripeZones);
            __Get_Value_From_Table(&Obj.Clouds.stripeFluct, FirstCloudsTable, L"stripeFluct", Obj.Clouds.stripeFluct);
            __Get_Value_From_Table(&Obj.Clouds.stripeTwist, FirstCloudsTable, L"stripeTwist", Obj.Clouds.stripeTwist);
            __Get_Value_From_Table(&Obj.Clouds.cycloneMagn, FirstCloudsTable, L"cycloneMagn", Obj.Clouds.cycloneMagn);
            __Get_Value_From_Table(&Obj.Clouds.cycloneFreq, FirstCloudsTable, L"cycloneFreq", Obj.Clouds.cycloneFreq);
            __Get_Value_From_Table(&Obj.Clouds.cycloneDensity, FirstCloudsTable, L"cycloneDensity", Obj.Clouds.cycloneDensity);
            __Get_Value_From_Table(&Obj.Clouds.cycloneOctaves, FirstCloudsTable, L"cycloneOctaves", Obj.Clouds.cycloneOctaves);
            __Get_Value_From_Table(&Obj.Clouds.cycloneMagn2, FirstCloudsTable, L"cycloneMagn2", Obj.Clouds.cycloneMagn2);
            __Get_Value_From_Table(&Obj.Clouds.cycloneFreq2, FirstCloudsTable, L"cycloneFreq2", Obj.Clouds.cycloneFreq2);
            __Get_Value_From_Table(&Obj.Clouds.cycloneDensity2, FirstCloudsTable, L"cycloneDensity2", Obj.Clouds.cycloneDensity2);
            __Get_Value_From_Table(&Obj.Clouds.cycloneOctaves2, FirstCloudsTable, L"cycloneOctaves2", Obj.Clouds.cycloneOctaves2);
        }

        __Get_Value_From_Table(&Obj.NoAtmosphere, CurrentTable, L"NoAtmosphere", false);
        auto AtmosphereTable = __Find_Table_From_List(CurrentTable, L"Atmosphere");
        if (!Obj.NoAtmosphere && AtmosphereTable != CurrentTableEnd)
        {
            auto AtmoSubTable = AtmosphereTable->SubTable;
            __Get_Value_From_Table(&Obj.Atmosphere.Model, AtmoSubTable, L"Model", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Atmosphere.Height, AtmoSubTable, L"Height", _NoDataDbl);
            Obj.Atmosphere.Height *= 1000;
            __Get_Value_From_Table(&Obj.Atmosphere.Density, AtmoSubTable, L"Density", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.Pressure, AtmoSubTable, L"Pressure", _NoDataDbl);
            Obj.Atmosphere.Pressure *= StandardAtm;
            __Get_Value_From_Table(&Obj.Atmosphere.Adiabat, AtmoSubTable, L"Adiabat", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.Greenhouse, AtmoSubTable, L"Greenhouse", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.Bright, AtmoSubTable, L"Bright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.Opacity, AtmoSubTable, L"Opacity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.SkyLight, AtmoSubTable, L"SkyLight", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.Hue, AtmoSubTable, L"Hue", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Atmosphere.Saturation, AtmoSubTable, L"Saturation", _NoDataDbl);
            auto CompTable = __Find_Table_From_List(AtmoSubTable, L"Composition");
            if (CompTable != AtmoSubTable->Get().end()) {Obj.Atmosphere.Composition = TableToCompositeMap(CompTable->SubTable);}
        }

        auto ClimateTable = __Find_Table_From_List(CurrentTable, L"Climate");
        if (ClimateTable != CurrentTableEnd)
        {
            Obj.EnableClimate = true;
            auto ClimateSubTable = ClimateTable->SubTable;
            __Get_Value_From_Table(&Obj.Climate.GlobalWindSpeed, ClimateSubTable, L"GlobalWindSpeed", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Climate.MinSurfaceTemp, ClimateSubTable, L"MinSurfaceTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Climate.MaxSurfaceTemp, ClimateSubTable, L"MaxSurfaceTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Climate.AtmoProfile, ClimateSubTable, L"AtmoProfile", ustring(_NoDataStr));
        }

        __Get_Value_From_Table(&Obj.NoAurora, CurrentTable, L"NoAurora", false);
        auto AuroraTable = __Find_Table_From_List(CurrentTable, L"Aurora");
        if (!Obj.NoAurora && AuroraTable != CurrentTableEnd)
        {
            auto AuroraSubTable = AuroraTable->SubTable;
            __Get_Value_From_Table(&Obj.Aurora.Height, AuroraSubTable, L"Height", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthLat, AuroraSubTable, L"NorthLat", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthLon, AuroraSubTable, L"NorthLon", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthRadius, AuroraSubTable, L"NorthRadius", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthWidth, AuroraSubTable, L"NorthWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthRings, AuroraSubTable, L"NorthRings", _NoDataInt);
            __Get_Value_From_Table(&Obj.Aurora.NorthBright, AuroraSubTable, L"NorthBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthFlashFreq, AuroraSubTable, L"NorthFlashFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthMoveSpeed, AuroraSubTable, L"NorthMoveSpeed", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.NorthParticles, AuroraSubTable, L"NorthParticles", _NoDataInt);
            __Get_Value_From_Table(&Obj.Aurora.SouthLat, AuroraSubTable, L"SouthLat", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthLon, AuroraSubTable, L"SouthLon", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthRadius, AuroraSubTable, L"SouthRadius", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthWidth, AuroraSubTable, L"SouthWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthRings, AuroraSubTable, L"SouthRings", _NoDataInt);
            __Get_Value_From_Table(&Obj.Aurora.SouthBright, AuroraSubTable, L"SouthBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthFlashFreq, AuroraSubTable, L"SouthFlashFreq", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthMoveSpeed, AuroraSubTable, L"SouthMoveSpeed", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Aurora.SouthParticles, AuroraSubTable, L"SouthParticles", _NoDataInt);
            std::array<float64, 3> TopColor;
            __Get_Value_From_Table(&TopColor, AuroraSubTable, L"TopColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Aurora.TopColor = vec3(TopColor);
            std::array<float64, 3> BottomColor;
            __Get_Value_From_Table(&BottomColor, AuroraSubTable, L"BottomColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Aurora.BottomColor = vec3(BottomColor);
        }

        __Get_Value_From_Table(&Obj.NoRings, CurrentTable, L"NoRings", false);
        auto RingsTable = __Find_Table_From_List(CurrentTable, L"Rings");
        if (!Obj.NoRings && RingsTable != CurrentTableEnd)
        {
            auto RingsSubTable = RingsTable->SubTable;
            __Get_Value_From_Table(&Obj.Rings.Texture, RingsSubTable, L"Texture", ustring(_NoDataStr));
            __Get_Value_From_Table(&Obj.Rings.InnerRadius, RingsSubTable, L"InnerRadius", _NoDataDbl);
            Obj.Rings.InnerRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.OuterRadius, RingsSubTable, L"OuterRadius", _NoDataDbl);
            Obj.Rings.OuterRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.EdgeRadius, RingsSubTable, L"EdgeRadius", _NoDataDbl);
            Obj.Rings.EdgeRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.MeanRadius, RingsSubTable, L"MeanRadius", _NoDataDbl);
            Obj.Rings.MeanRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.Thickness, RingsSubTable, L"Thickness", _NoDataDbl);
            Obj.Rings.Thickness *= 1000;
            __Get_Value_From_Table(&Obj.Rings.RocksMaxSize, RingsSubTable, L"RocksMaxSize", _NoDataDbl);
            Obj.Rings.RocksMaxSize *= 1000;
            __Get_Value_From_Table(&Obj.Rings.RocksSpacing, RingsSubTable, L"RocksSpacing", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.DustDrawDist, RingsSubTable, L"DustDrawDist", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.ChartRadius, RingsSubTable, L"ChartRadius", _NoDataDbl);
            Obj.Rings.ChartRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.RotationPeriod, RingsSubTable, L"RotationPeriod", _NoDataDbl);
            Obj.Rings.RotationPeriod *= 3600;
            __Get_Value_From_Table(&Obj.Rings.Brightness, RingsSubTable, L"Brightness", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.FrontBright, RingsSubTable, L"FrontBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.BackBright, RingsSubTable, L"BackBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.Density, RingsSubTable, L"Density", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.Opacity, RingsSubTable, L"Opacity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.SelfShadow, RingsSubTable, L"SelfShadow", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.PlanetShadow, RingsSubTable, L"PlanetShadow", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.Hapke, RingsSubTable, L"Hapke", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.SpotBright, RingsSubTable, L"SpotBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.SpotWidth, RingsSubTable, L"SpotWidth", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.SpotBrightCB, RingsSubTable, L"SpotBrightCB", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.SpotWidthCB, RingsSubTable, L"SpotWidthCB", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.frequency, RingsSubTable, L"frequency", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.densityScale, RingsSubTable, L"densityScale", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.densityOffset, RingsSubTable, L"densityOffset", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.densityPower, RingsSubTable, L"densityPower", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Rings.colorContrast, RingsSubTable, L"colorContrast", _NoDataDbl);
            std::array<float64, 3> FrontColor;
            __Get_Value_From_Table(&FrontColor, RingsSubTable, L"FrontColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Rings.FrontColor = vec3(FrontColor);
            std::array<float64, 3> BackThickColor;
            __Get_Value_From_Table(&BackThickColor, RingsSubTable, L"BackThickColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Rings.BackThickColor = vec3(BackThickColor);
            std::array<float64, 3> BackIceColor;
            __Get_Value_From_Table(&BackIceColor, RingsSubTable, L"BackIceColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Rings.BackIceColor = vec3(BackIceColor);
            std::array<float64, 3> BackDustColor;
            __Get_Value_From_Table(&BackDustColor, RingsSubTable, L"BackDustColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.Rings.BackDustColor = vec3(BackDustColor);
        }

        __Get_Value_From_Table(&Obj.NoAccretionDisk, CurrentTable, L"NoAccretionDisk", false);
        auto AccDiskTable = __Find_Table_From_List(CurrentTable, L"AccretionDisk");
        if (!Obj.NoAccretionDisk && AccDiskTable != CurrentTableEnd)
        {
            auto AccDiskSubTable = AccDiskTable->SubTable;
            __Get_Value_With_Unit(&Obj.AccretionDisk.InnerRadius, AccDiskSubTable, L"InnerRadius", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.OuterRadius, AccDiskSubTable, L"OuterRadius", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.InnerThickness, AccDiskSubTable, L"InnerThickness", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.OuterThickness, AccDiskSubTable, L"OuterThickness", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_From_Table(&Obj.AccretionDisk.ThicknessPow, AccDiskSubTable, L"ThicknessPow", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.DetailScaleR, AccDiskSubTable, L"DetailScaleR", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.DetailScaleV, AccDiskSubTable, L"DetailScaleV", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveDistortionX, AccDiskSubTable, L"OctaveDistortionX", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveDistortionY, AccDiskSubTable, L"OctaveDistortionY", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveDistortionZ, AccDiskSubTable, L"OctaveDistortionZ", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveScale, AccDiskSubTable, L"OctaveScale", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.DiskNoiseContrast, AccDiskSubTable, L"DiskNoiseContrast", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.DiskTempContrast, AccDiskSubTable, L"DiskTempContrast", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.DiskOuterSpin, AccDiskSubTable, L"DiskOuterSpin", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.TwistMagn, AccDiskSubTable, L"TwistMagn", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.AccretionRate, AccDiskSubTable, L"AccretionRate", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.Temperature, AccDiskSubTable, L"Temperature", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.Density, AccDiskSubTable, L"Density", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.Luminosity, AccDiskSubTable, L"Luminosity", _NoDataDbl);
            Obj.AccretionDisk.Luminosity *= SolarLum;
            __Get_Value_From_Table(&Obj.AccretionDisk.LuminosityBol, AccDiskSubTable, L"LuminosityBol", _NoDataDbl);
            Obj.AccretionDisk.Luminosity *= SolarLumBol;
            __Get_Value_From_Table(&Obj.AccretionDisk.Brightness, AccDiskSubTable, L"Brightness", _NoDataDbl);

            __Get_Value_With_Unit(&Obj.AccretionDisk.JetLength, AccDiskSubTable, L"JetLength", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.JetStartRadius, AccDiskSubTable, L"JetStartRadius", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.JetEndRadius, AccDiskSubTable, L"JetEndRadius", _NoDataDbl, AU, {{L"Km", 1000}});
            __Get_Value_From_Table(&Obj.AccretionDisk.JetStartTemp, AccDiskSubTable, L"JetStartTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetEndTemp, AccDiskSubTable, L"JetEndTemp", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetVelocity, AccDiskSubTable, L"JetVelocity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetDistortion, AccDiskSubTable, L"JetDistortion", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetThickness, AccDiskSubTable, L"JetThickness", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetDensity, AccDiskSubTable, L"JetDensity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetBrightness, AccDiskSubTable, L"JetBrightness", _NoDataDbl);

            __Get_Value_From_Table(&Obj.AccretionDisk.LightingBright, AccDiskSubTable, L"LightingBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.ShadowContrast, AccDiskSubTable, L"ShadowContrast", _NoDataDbl);
            __Get_Value_From_Table(&Obj.AccretionDisk.ShadowLength, AccDiskSubTable, L"ShadowLength", _NoDataDbl);
        }

        __Get_Value_From_Table(&Obj.NoCorona, CurrentTable, L"NoCorona", false);
        auto CoronaTable = __Find_Table_From_List(CurrentTable, L"Corona");
        if (!Obj.NoCorona && CoronaTable != CurrentTableEnd)
        {
            auto CoronaSubTable = CoronaTable->SubTable;
            __Get_Value_From_Table(&Obj.Corona.Radius, CoronaSubTable, L"Radius", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Corona.Period, CoronaSubTable, L"Period", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Corona.Brightness, CoronaSubTable, L"Brightness", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Corona.RayDensity, CoronaSubTable, L"RayDensity", _NoDataDbl);
            __Get_Value_From_Table(&Obj.Corona.RayCurv, CoronaSubTable, L"RayCurv", _NoDataDbl);
        }

        __Get_Value_From_Table(&Obj.NoCometTail, CurrentTable, L"NoCometTail", false);
        auto CometTailTable = __Find_Table_From_List(CurrentTable, L"CometTail");
        if (!Obj.NoCometTail && CometTailTable != CurrentTableEnd)
        {
            auto CometTailSubTable = CometTailTable->SubTable;
            __Get_Value_From_Table(&Obj.CometTail.MaxLength, CometTailSubTable, L"MaxLength", _NoDataDbl);
            __Get_Value_From_Table(&Obj.CometTail.GasToDust, CometTailSubTable, L"GasToDust", _NoDataDbl);
            __Get_Value_From_Table(&Obj.CometTail.Particles, CometTailSubTable, L"Particles", _NoDataInt);
            __Get_Value_From_Table(&Obj.CometTail.GasBright, CometTailSubTable, L"GasBright", _NoDataDbl);
            __Get_Value_From_Table(&Obj.CometTail.DustBright, CometTailSubTable, L"DustBright", _NoDataDbl);
            std::array<float64, 3> GasColor;
            __Get_Value_From_Table(&GasColor, CometTailSubTable, L"GasColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.CometTail.GasColor = vec3(GasColor);
            std::array<float64, 3> DustColor;
            __Get_Value_From_Table(&DustColor, CometTailSubTable, L"DustColor", {_NoDataDbl, _NoDataDbl, _NoDataDbl});
            Obj.CometTail.DustColor = vec3(DustColor);
        }
    }

    return Obj;
}

template<> Object GetObject(_SC SharedTablePointer Table, ustring Name)
{
    auto it = find_if(Table->Get().begin(), Table->Get().end(), [&](_SC SCSTable::ValueType Tbl)
    {
        if (Tbl.Value.front().GetTypeID() != Tbl.Value.front().VString) {return false;}
        ustring NameStr;
        Tbl.Value.front().GetQualified(&NameStr);
        auto NameList = __Str_Split(NameStr);
        return find(NameList.begin(), NameList.end(), Name) != NameList.end();
    });
    if (it == Table->Get().end()) {throw _SC ParseException("Object is not found in this scope.");}
    return GetObjectFromKeyValue(*it);
}

#endif

_CSE_END
