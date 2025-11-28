#include "CSE/Base/AdvMath.h"
#include "CSE/Base/ConstLists.h"
#include "CSE/Base/DateTime.h"
#include "CSE/Base/GLTypes.h"
#include "CSE/Object.h"

#if __has_include("CSE/Parser.h")
#include "CSE/Parser.h"
#endif

#if __has_include("CSE/Extensions/Hyperbolic.h")
#include "CSE/Extensions/Hyperbolic.h"
#endif

using namespace std;

_CSE_BEGIN

#ifdef _CSE_SCSTREAM

using namespace __scstream_table_helpers;

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

        __Get_Value_From_Table(&Obj.DateUpdated, CurrentTable, L"DateUpdated", Obj.DateUpdated);
        __Get_Value_From_Table(&Obj.DiscMethod, CurrentTable, L"DiscMethod", Obj.DiscMethod);
        __Get_Value_From_Table(&Obj.DiscDate, CurrentTable, L"DiscDate", Obj.DiscDate);
        __Get_Value_From_Table(&Obj.ParentBody, CurrentTable, L"ParentBody", Obj.ParentBody);
        if (Obj.Type == L"Star")
        {
            Obj.Class = L"Sun";
            __Get_Value_From_Table(&Obj.SpecClass, CurrentTable, L"Class", Obj.SpecClass);
        }
        else {__Get_Value_From_Table(&Obj.Class, CurrentTable, L"Class", Obj.Class);}
        __Get_Value_From_Table(&Obj.AsterType, CurrentTable, L"AsterType", Obj.AsterType);
        __Get_Value_From_Table(&Obj.CometType, CurrentTable, L"CometType", Obj.CometType);

        __Get_Value_With_Unit(&Obj.Mass, CurrentTable, L"Mass", _NoDataDbl, EarthMass,
        {
            {L"Kg", 1},
            {L"Jup", JupiterMass},
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
            Obj.Dimensions = 2. * (Radius - Radius * Flattening);
        }
        else {Obj.Dimensions = vec3(Dimensions);}
        __Get_Value_From_Table(&Obj.InertiaMoment, CurrentTable, L"InertiaMoment", Obj.InertiaMoment);
        __Get_Value_From_Table(&Obj.AlbedoBond, CurrentTable, L"AlbedoBond", Obj.AlbedoBond);
        __Get_Value_From_Table(&Obj.AlbedoGeom, CurrentTable, L"AlbedoGeom", Obj.AlbedoGeom);
        __Get_Value_From_Table(&Obj.EndogenousHeating, CurrentTable, L"EndogenousHeating", Obj.EndogenousHeating);
        __Get_Value_From_Table(&Obj.ThermalLuminosity, CurrentTable, L"ThermalLuminosity", Obj.ThermalLuminosity);
        __Get_Value_From_Table(&Obj.ThermalLuminosityBol, CurrentTable, L"ThermalLuminosityBol", Obj.ThermalLuminosityBol);
        __Get_Value_From_Table(&Obj.Temperature, CurrentTable, L"Teff", Obj.Temperature);
        if (IS_NO_DATA_DBL(Obj.Temperature))
        {
            __Get_Value_From_Table(&Obj.Temperature, CurrentTable, L"Temperature", Obj.Temperature);
        }
        __Get_Value_From_Table(&Obj.Luminosity, CurrentTable, L"Luminosity", Obj.Luminosity);
        if (IS_NO_DATA_DBL(Obj.Luminosity))
        {
            __Get_Value_From_Table(&Obj.Luminosity, CurrentTable, L"Lum", Obj.Luminosity);
        }
        __Get_Value_From_Table(&Obj.LumBol, CurrentTable, L"LumBol", Obj.LumBol);
        if (IS_NO_DATA_DBL(Obj.LumBol))
        {
            __Get_Value_From_Table(&Obj.LumBol, CurrentTable, L"LuminosityBol", Obj.LumBol);
        }
        Obj.Luminosity *= SolarLum;
        Obj.LumBol *= SolarLum;
        __Get_Value_From_Table(&Obj.FeH, CurrentTable, L"FeH", Obj.FeH);
        __Get_Value_From_Table(&Obj.CtoO, CurrentTable, L"CtoO", Obj.CtoO);
        __Get_Value_With_Unit(&Obj.Age, CurrentTable, L"Age", Obj.Age, 1E12, {});
        __Get_Value_From_Table(&Obj.KerrSpin, CurrentTable, L"KerrSpin", Obj.KerrSpin);
        __Get_Value_From_Table(&Obj.KerrCharge, CurrentTable, L"KerrCharge", Obj.KerrCharge);

        __Get_Value_From_Table(&Obj.Color, CurrentTable, L"Color", Obj.Color);
        __Get_Value_From_Table(&Obj.AbsMagn, CurrentTable, L"AbsMagn", Obj.AbsMagn);
        __Get_Value_From_Table(&Obj.SlopeParam, CurrentTable, L"SlopeParam", Obj.SlopeParam);
        __Get_Value_From_Table(&Obj.Brightness, CurrentTable, L"Brightness", Obj.Brightness);
        __Get_Value_From_Table(&Obj.BrightnessReal, CurrentTable, L"BrightnessReal", Obj.BrightnessReal);

        auto RotModelTable = __Find_Table_From_List(CurrentTable, L"RotationModel");
        if (RotModelTable != CurrentTableEnd)
        {
            RotModelTable->Value.front().GetQualified(&Obj.RotationModel);
        }
        else {Obj.RotationModel = L"Simple";}
        //if (Obj.RotationModel == L"Simple")
        //{
            __Get_Value_From_Table(&Obj.Rotation.RotationEpoch, CurrentTable, L"RotationEpoch", Obj.Rotation.RotationEpoch);
            __Get_Value_From_Table(&Obj.Rotation.Obliquity, CurrentTable, L"Obliquity", Obj.Rotation.Obliquity);
            __Get_Value_From_Table(&Obj.Rotation.EqAscendNode, CurrentTable, L"EqAscendNode", Obj.Rotation.EqAscendNode);
            __Get_Value_From_Table(&Obj.Rotation.RotationOffset, CurrentTable, L"RotationOffset", Obj.Rotation.RotationOffset);
            __Get_Value_With_Unit(&Obj.Rotation.RotationPeriod, CurrentTable, L"RotationPeriod", Obj.Rotation.RotationPeriod, 3600, {});
            __Get_Value_With_Unit(&Obj.Rotation.Precession, CurrentTable, L"Precession", Obj.Rotation.Precession, JulianYear, {});
            __Get_Value_From_Table(&Obj.Rotation.TidalLocked, CurrentTable, L"TidalLocked", false);
            if (Quadrant(Obj.Rotation.Obliquity) >= 2 &&
                Quadrant(Obj.Rotation.Obliquity) < 6 &&
                Obj.Rotation.RotationPeriod > 0)
            {
                Obj.Rotation.RotationPeriod = -Obj.Rotation.RotationPeriod;
            }
        //}
        if (Obj.RotationModel == L"IAU" && RotModelTable->SubTable)
        {
            auto RotModelSubTable = RotModelTable->SubTable;
            auto RotModelSubTableEnd = RotModelSubTable->Get().end();
            __Get_Value_From_Table(&Obj.RotationIAU.ValidRange, CurrentTable, L"ValidRange", Obj.RotationIAU.ValidRange);
            __Get_Value_From_Table(&Obj.RotationIAU.Epoch, RotModelSubTable, L"Epoch", float64(J2000));
            __Get_Value_From_Table(&Obj.RotationIAU.PoleRA, RotModelSubTable, L"PoleRA", Obj.RotationIAU.PoleRA);
            __Get_Value_From_Table(&Obj.RotationIAU.PoleRARate, RotModelSubTable, L"PoleRARate", Obj.RotationIAU.PoleRARate);
            __Get_Value_From_Table(&Obj.RotationIAU.PoleDec, RotModelSubTable, L"PoleDec", Obj.RotationIAU.PoleDec);
            __Get_Value_From_Table(&Obj.RotationIAU.PoleDecRate, RotModelSubTable, L"PoleDecRate", Obj.RotationIAU.PoleDecRate);
            __Get_Value_From_Table(&Obj.RotationIAU.PrimeMeridian, RotModelSubTable, L"PrimeMeridian", Obj.RotationIAU.PrimeMeridian);
            __Get_Value_From_Table(&Obj.RotationIAU.RotationRate, RotModelSubTable, L"RotationRate", Obj.RotationIAU.RotationRate);
            __Get_Value_From_Table(&Obj.RotationIAU.RotationAccel, RotModelSubTable, L"RotationAccel", Obj.RotationIAU.RotationAccel);
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
            vec3 StaticPos;
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
            vec3 FixedPos;
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
            __Get_Value_From_Table(&Obj.Orbit.RefPlane, OrbitSubTable, L"RefPlane", Obj.Orbit.RefPlane);
            __Get_Value_From_Table(&Obj.Orbit.Separation, OrbitSubTable, L"Separation", Obj.Orbit.Separation);
            Obj.Orbit.Separation *= AU;
            __Get_Value_From_Table(&Obj.Orbit.PositionAngle, OrbitSubTable, L"PositionAngle", Obj.Orbit.PositionAngle);
            __Get_Value_From_Table(&Obj.Orbit.AnalyticModel, OrbitSubTable, L"AnalyticModel", Obj.Orbit.AnalyticModel);
            if (Obj.Orbit.AnalyticModel != _NoDataStr) {Obj.Orbit.RefPlane = L"Analytic";}
            __Get_Value_From_Table(&Obj.Orbit.Epoch, OrbitSubTable, L"Epoch", float64(J2000));
            __Get_Value_With_Unit(&Obj.Orbit.Period, OrbitSubTable, L"Period", Obj.Orbit.Period, JulianYear, {{L"Days", SynodicDay}});
            __Get_Value_With_Unit(&Obj.Orbit.PericenterDist, OrbitSubTable, L"PericenterDist", Obj.Orbit.PericenterDist, AU, {{L"Km", 1000}});
            __Get_Value_From_Table(&Obj.Orbit.Eccentricity, OrbitSubTable, L"Eccentricity", Obj.Orbit.Eccentricity);
            if (IS_NO_DATA_DBL(Obj.Orbit.PericenterDist))
            {
                float64 SemiMajorAxis;
                __Get_Value_With_Unit(&SemiMajorAxis, OrbitSubTable, L"SemiMajorAxis", _NoDataDbl, AU, {{L"Km", 1000}});
                if (Obj.Orbit.Eccentricity > 1 && SemiMajorAxis > 0) {SemiMajorAxis = -SemiMajorAxis;}
                Obj.Orbit.PericenterDist = abs(SemiMajorAxis - SemiMajorAxis *
                    (IS_NO_DATA_DBL(Obj.Orbit.Eccentricity) ? 1 : Obj.Orbit.Eccentricity));
            }
            if ((!IS_NO_DATA_DBL(Obj.Orbit.PericenterDist) || !IS_NO_DATA_DBL(Obj.Orbit.Period))
                    && IS_NO_DATA_STR(Obj.Orbit.RefPlane) )
            {
                Obj.Orbit.RefPlane = L"Extrasolar";
            }
            __Get_Value_From_Table(&Obj.Orbit.GravParam, OrbitSubTable, L"GravParam", Obj.Orbit.GravParam);
            __Get_Value_From_Table(&Obj.Orbit.Inclination, OrbitSubTable, L"Inclination", Obj.Orbit.Inclination);
            if (Quadrant(Obj.Orbit.Inclination) >= 2 &&
                Quadrant(Obj.Orbit.Inclination) < 6 &&
                Obj.Orbit.Period > 0)
            {
                Obj.Orbit.Period = -Obj.Orbit.Period;
            }
            __Get_Value_From_Table(&Obj.Orbit.AscendingNode, OrbitSubTable, L"AscendingNode", Obj.Orbit.AscendingNode);
            __Get_Value_From_Table(&Obj.Orbit.AscNodePreces, OrbitSubTable, L"AscNodePreces", Obj.Orbit.AscNodePreces);
            __Get_Value_From_Table(&Obj.Orbit.ArgOfPericenter, OrbitSubTable, L"ArgOfPericenter", Obj.Orbit.ArgOfPericenter);
            if (IS_NO_DATA_DBL(Obj.Orbit.ArgOfPericenter))
            {
                __Get_Value_From_Table(&Obj.Orbit.ArgOfPericenter, OrbitSubTable, L"ArgOfPericen", Obj.Orbit.ArgOfPericenter);
            }
            if (IS_NO_DATA_DBL(Obj.Orbit.ArgOfPericenter))
            {
                float64 LongOfPericen;
                __Get_Value_From_Table(&LongOfPericen, OrbitSubTable, L"LongOfPericen", _NoDataDbl);
                Obj.Orbit.ArgOfPericenter = LongOfPericen -
                    (IS_NO_DATA_DBL(Obj.Orbit.AscendingNode) ? 0 : Obj.Orbit.AscendingNode);
            }
            __Get_Value_From_Table(&Obj.Orbit.ArgOfPeriPreces, OrbitSubTable, L"ArgOfPeriPreces", Obj.Orbit.ArgOfPeriPreces);
            __Get_Value_From_Table(&Obj.Orbit.MeanAnomaly, OrbitSubTable, L"MeanAnomaly", Obj.Orbit.MeanAnomaly);
            if (IS_NO_DATA_DBL(Obj.Orbit.MeanAnomaly))
            {
                float64 MeanLongitude;
                __Get_Value_From_Table(&MeanLongitude, OrbitSubTable, L"MeanLongitude", _NoDataDbl);
                Obj.Orbit.MeanAnomaly = MeanLongitude -
                    (IS_NO_DATA_DBL(Obj.Orbit.ArgOfPericenter) ? 0 : Obj.Orbit.ArgOfPericenter) -
                    (IS_NO_DATA_DBL(Obj.Orbit.AscendingNode) ? 0 : Obj.Orbit.AscendingNode);
            }
        }

        bool NoLife;
        __Get_Value_From_Table(&NoLife, CurrentTable, L"NoLife", false);
        auto LifeTables = __Find_Multi_Tables_From_List(CurrentTable, L"Life");
        if (!NoLife && !LifeTables.empty())
        {
            auto GetLifeFromTable = [](_SC SharedTablePointer Table, Object::LifeParams* Dst)
            {
                __Get_Value_From_Table(&Dst->Class, Table, L"Class", Dst->Class);
                __Get_Value_From_Table(&Dst->Type, Table, L"Type", Dst->Type);
                ustring Biomes;
                __Get_Value_From_Table(&Biomes, Table, L"Biome", ustring(_NoDataStr));
                Dst->Biome = __Str_Split(Biomes);
                __Get_Value_From_Table(&Dst->Panspermia, Table, L"Panspermia", false);
            };
            switch (LifeTables.size())
            {
            case 2:
                GetLifeFromTable(LifeTables[1]->SubTable, &Obj.Life[1]);
                ++Obj.LifeCount;
            case 1:
                GetLifeFromTable(LifeTables[0]->SubTable, &Obj.Life[0]);
                ++Obj.LifeCount;
                break;
            default:
                break;
            }
        }
        //else {Obj.LifeCount = 0;}

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
            __Get_Value_From_Table(&Obj.Surface.DiffMap, SurfaceSubTable, L"DiffMap", Obj.Surface.DiffMap);
            __Get_Value_From_Table(&Obj.Surface.DiffMapAlpha, SurfaceSubTable, L"DiffMapAlpha", Obj.Surface.DiffMapAlpha);
            __Get_Value_From_Table(&Obj.Surface.BumpMap, SurfaceSubTable, L"BumpMap", Obj.Surface.BumpMap);
            __Get_Value_From_Table(&Obj.Surface.BumpHeight, SurfaceSubTable, L"BumpHeight", Obj.Surface.BumpHeight);
            __Get_Value_From_Table(&Obj.Surface.BumpOffset, SurfaceSubTable, L"BumpOffset", Obj.Surface.BumpOffset);
            __Get_Value_From_Table(&Obj.Surface.GlowMap, SurfaceSubTable, L"GlowMap", Obj.Surface.GlowMap);
            __Get_Value_From_Table(&Obj.Surface.GlowMode, SurfaceSubTable, L"GlowMode", Obj.Surface.GlowMode);
            __Get_Value_From_Table(&Obj.Surface.GlowColor, SurfaceSubTable, L"GlowColor", Obj.Surface.GlowColor);
            __Get_Value_From_Table(&Obj.Surface.GlowBright, SurfaceSubTable, L"GlowBright", Obj.Surface.GlowBright);
            __Get_Value_From_Table(&Obj.Surface.SpecMap, SurfaceSubTable, L"SpecMap", Obj.Surface.SpecMap);
            __Get_Value_From_Table(&Obj.Surface.FlipMap, SurfaceSubTable, L"FlipMap", Obj.Surface.FlipMap);
            __Get_Value_From_Table(&Obj.Surface.Gamma, SurfaceSubTable, L"Gamma", Obj.Surface.Gamma);
            __Get_Value_From_Table(&Obj.Surface.Hapke, SurfaceSubTable, L"Hapke", Obj.Surface.Hapke);
            __Get_Value_From_Table(&Obj.Surface.SpotBright, SurfaceSubTable, L"SpotBright", Obj.Surface.SpotBright);
            __Get_Value_From_Table(&Obj.Surface.SpotWidth, SurfaceSubTable, L"SpotWidth", Obj.Surface.SpotWidth);
            __Get_Value_From_Table(&Obj.Surface.SpotBrightCB, SurfaceSubTable, L"SpotBrightCB", Obj.Surface.SpotBrightCB);
            __Get_Value_From_Table(&Obj.Surface.SpotWidthCB, SurfaceSubTable, L"SpotWidthCB", Obj.Surface.SpotWidthCB);
            __Get_Value_From_Table(&Obj.Surface.TempMapMaxTemp, SurfaceSubTable, L"TempMapMaxTemp", Obj.Surface.TempMapMaxTemp);
            __Get_Value_From_Table(&Obj.Surface.DayAmbient, SurfaceSubTable, L"DayAmbient", Obj.Surface.DayAmbient);
            __Get_Value_From_Table(&Obj.Surface.RingsWinter, SurfaceSubTable, L"RingsWinter", Obj.Surface.RingsWinter);
            __Get_Value_From_Table(&Obj.Surface.ModulateColor, SurfaceSubTable, L"ModulateColor", Obj.Surface.ModulateColor);
            __Get_Value_From_Table(&Obj.Surface.ModulateBright, SurfaceSubTable, L"ModulateBright", Obj.Surface.ModulateBright);
            __Get_Value_From_Table(&Obj.Surface.SpecBrightWater, SurfaceSubTable, L"SpecBrightWater", Obj.Surface.SpecBrightWater);
            __Get_Value_From_Table(&Obj.Surface.SpecBrightIce, SurfaceSubTable, L"SpecBrightIce", Obj.Surface.SpecBrightIce);
            __Get_Value_From_Table(&Obj.Surface.RoughnessWater, SurfaceSubTable, L"RoughnessWater", Obj.Surface.RoughnessWater);
            __Get_Value_From_Table(&Obj.Surface.RoughnessIce, SurfaceSubTable, L"RoughnessIce", Obj.Surface.RoughnessIce);
            __Get_Value_From_Table(&Obj.Surface.SpecularScale, SurfaceSubTable, L"SpecularScale", Obj.Surface.SpecularScale);
            __Get_Value_From_Table(&Obj.Surface.RoughnessBias, SurfaceSubTable, L"RoughnessBias", Obj.Surface.RoughnessBias);
            __Get_Value_From_Table(&Obj.Surface.RoughnessScale, SurfaceSubTable, L"RoughnessScale", Obj.Surface.RoughnessScale);
            __Get_Value_From_Table(&Obj.Surface.Preset, SurfaceSubTable, L"Preset", Obj.Surface.Preset);
            __Get_Value_From_Table(&Obj.Surface.SurfStyle, SurfaceSubTable, L"SurfStyle", Obj.Surface.SurfStyle);
            __Get_Value_From_Table(&Obj.Surface.Randomize, SurfaceSubTable, L"Randomize", Obj.Surface.Randomize);
            __Get_Value_From_Table(&Obj.Surface.colorDistMagn, SurfaceSubTable, L"colorDistMagn", Obj.Surface.colorDistMagn);
            __Get_Value_From_Table(&Obj.Surface.colorDistFreq, SurfaceSubTable, L"colorDistFreq", Obj.Surface.colorDistFreq);
            __Get_Value_From_Table(&Obj.Surface.detailScale, SurfaceSubTable, L"detailScale", Obj.Surface.detailScale);
            __Get_Value_From_Table(&Obj.Surface.drivenDarkening, SurfaceSubTable, L"drivenDarkening", Obj.Surface.drivenDarkening);
            __Get_Value_From_Table(&Obj.Surface.seaLevel, SurfaceSubTable, L"seaLevel", Obj.Surface.seaLevel);
            __Get_Value_From_Table(&Obj.Surface.snowLevel, SurfaceSubTable, L"snowLevel", Obj.Surface.snowLevel);
            __Get_Value_From_Table(&Obj.Surface.tropicLatitude, SurfaceSubTable, L"tropicLatitude", Obj.Surface.tropicLatitude);
            __Get_Value_From_Table(&Obj.Surface.icecapLatitude, SurfaceSubTable, L"icecapLatitude", Obj.Surface.icecapLatitude);
            __Get_Value_From_Table(&Obj.Surface.icecapHeight, SurfaceSubTable, L"icecapHeight", Obj.Surface.icecapHeight);
            __Get_Value_From_Table(&Obj.Surface.climatePole, SurfaceSubTable, L"climatePole", Obj.Surface.climatePole);
            __Get_Value_From_Table(&Obj.Surface.climateTropic, SurfaceSubTable, L"climateTropic", Obj.Surface.climateTropic);
            __Get_Value_From_Table(&Obj.Surface.climateEquator, SurfaceSubTable, L"climateEquator", Obj.Surface.climateEquator);
            __Get_Value_From_Table(&Obj.Surface.climateSteppeMin, SurfaceSubTable, L"climateSteppeMin", Obj.Surface.climateSteppeMin);
            __Get_Value_From_Table(&Obj.Surface.climateSteppeMax, SurfaceSubTable, L"climateSteppeMax", Obj.Surface.climateSteppeMax);
            __Get_Value_From_Table(&Obj.Surface.climateForestMin, SurfaceSubTable, L"climateForestMin", Obj.Surface.climateForestMin);
            __Get_Value_From_Table(&Obj.Surface.climateForestMax, SurfaceSubTable, L"climateForestMax", Obj.Surface.climateForestMax);
            __Get_Value_From_Table(&Obj.Surface.climateGrassMin, SurfaceSubTable, L"climateGrassMin", Obj.Surface.climateGrassMin);
            __Get_Value_From_Table(&Obj.Surface.climateGrassMax, SurfaceSubTable, L"climateGrassMax", Obj.Surface.climateGrassMax);
            __Get_Value_From_Table(&Obj.Surface.humidity, SurfaceSubTable, L"humidity", Obj.Surface.humidity);
            __Get_Value_From_Table(&Obj.Surface.heightTempGrad, SurfaceSubTable, L"heightTempGrad", Obj.Surface.heightTempGrad);
            __Get_Value_From_Table(&Obj.Surface.beachWidth, SurfaceSubTable, L"beachWidth", Obj.Surface.beachWidth);
            __Get_Value_From_Table(&Obj.Surface.tropicWidth, SurfaceSubTable, L"tropicWidth", Obj.Surface.tropicWidth);
            __Get_Value_From_Table(&Obj.Surface.mainFreq, SurfaceSubTable, L"mainFreq", Obj.Surface.mainFreq);
            __Get_Value_From_Table(&Obj.Surface.venusFreq, SurfaceSubTable, L"venusFreq", Obj.Surface.venusFreq);
            __Get_Value_From_Table(&Obj.Surface.venusMagn, SurfaceSubTable, L"venusMagn", Obj.Surface.venusMagn);
            __Get_Value_From_Table(&Obj.Surface.mareFreq, SurfaceSubTable, L"mareFreq", Obj.Surface.mareFreq);
            __Get_Value_From_Table(&Obj.Surface.mareDensity, SurfaceSubTable, L"mareDensity", Obj.Surface.mareDensity);
            __Get_Value_From_Table(&Obj.Surface.terraceProb, SurfaceSubTable, L"terraceProb", Obj.Surface.terraceProb);
            __Get_Value_From_Table(&Obj.Surface.erosion, SurfaceSubTable, L"erosion", Obj.Surface.erosion);
            __Get_Value_From_Table(&Obj.Surface.montesMagn, SurfaceSubTable, L"montesMagn", Obj.Surface.montesMagn);
            __Get_Value_From_Table(&Obj.Surface.montesFreq, SurfaceSubTable, L"montesFreq", Obj.Surface.montesFreq);
            __Get_Value_From_Table(&Obj.Surface.montesSpiky, SurfaceSubTable, L"montesSpiky", Obj.Surface.montesSpiky);
            __Get_Value_From_Table(&Obj.Surface.montesFraction, SurfaceSubTable, L"montesFraction", Obj.Surface.montesFraction);
            __Get_Value_From_Table(&Obj.Surface.dunesMagn, SurfaceSubTable, L"dunesMagn", Obj.Surface.dunesMagn);
            __Get_Value_From_Table(&Obj.Surface.dunesFreq, SurfaceSubTable, L"dunesFreq", Obj.Surface.dunesFreq);
            __Get_Value_From_Table(&Obj.Surface.dunesFraction, SurfaceSubTable, L"dunesFraction", Obj.Surface.dunesFraction);
            __Get_Value_From_Table(&Obj.Surface.hillsMagn, SurfaceSubTable, L"hillsMagn", Obj.Surface.hillsMagn);
            __Get_Value_From_Table(&Obj.Surface.hillsFreq, SurfaceSubTable, L"hillsFreq", Obj.Surface.hillsFreq);
            __Get_Value_From_Table(&Obj.Surface.hillsFraction, SurfaceSubTable, L"hillsFraction", Obj.Surface.hillsFraction);
            __Get_Value_From_Table(&Obj.Surface.hills2Fraction, SurfaceSubTable, L"hills2Fraction", Obj.Surface.hills2Fraction);
            __Get_Value_From_Table(&Obj.Surface.riversMagn, SurfaceSubTable, L"riversMagn", Obj.Surface.riversMagn);
            __Get_Value_From_Table(&Obj.Surface.riversFreq, SurfaceSubTable, L"riversFreq", Obj.Surface.riversFreq);
            __Get_Value_From_Table(&Obj.Surface.riversSin, SurfaceSubTable, L"riversSin", Obj.Surface.riversSin);
            __Get_Value_From_Table(&Obj.Surface.riftsMagn, SurfaceSubTable, L"riftsMagn", Obj.Surface.riftsMagn);
            __Get_Value_From_Table(&Obj.Surface.riftsFreq, SurfaceSubTable, L"riftsFreq", Obj.Surface.riftsFreq);
            __Get_Value_From_Table(&Obj.Surface.riftsSin, SurfaceSubTable, L"riftsSin", Obj.Surface.riftsSin);
            __Get_Value_From_Table(&Obj.Surface.eqridgeMagn, SurfaceSubTable, L"eqridgeMagn", Obj.Surface.eqridgeMagn);
            __Get_Value_From_Table(&Obj.Surface.eqridgeWidth, SurfaceSubTable, L"eqridgeWidth", Obj.Surface.eqridgeWidth);
            __Get_Value_From_Table(&Obj.Surface.eqridgeModMagn, SurfaceSubTable, L"eqridgeModMagn", Obj.Surface.eqridgeModMagn);
            __Get_Value_From_Table(&Obj.Surface.eqridgeModFreq, SurfaceSubTable, L"eqridgeModFreq", Obj.Surface.eqridgeModFreq);
            __Get_Value_From_Table(&Obj.Surface.canyonsMagn, SurfaceSubTable, L"canyonsMagn", Obj.Surface.canyonsMagn);
            __Get_Value_From_Table(&Obj.Surface.canyonsFreq, SurfaceSubTable, L"canyonsFreq", Obj.Surface.canyonsFreq);
            __Get_Value_From_Table(&Obj.Surface.canyonsFraction, SurfaceSubTable, L"canyonsFraction", Obj.Surface.canyonsFraction);
            __Get_Value_From_Table(&Obj.Surface.cracksMagn, SurfaceSubTable, L"cracksMagn", Obj.Surface.cracksMagn);
            __Get_Value_From_Table(&Obj.Surface.cracksFreq, SurfaceSubTable, L"cracksFreq", Obj.Surface.cracksFreq);
            __Get_Value_From_Table(&Obj.Surface.cracksOctaves, SurfaceSubTable, L"cracksOctaves", Obj.Surface.cracksOctaves);
            __Get_Value_From_Table(&Obj.Surface.craterMagn, SurfaceSubTable, L"craterMagn", Obj.Surface.craterMagn);
            __Get_Value_From_Table(&Obj.Surface.craterFreq, SurfaceSubTable, L"craterFreq", Obj.Surface.craterFreq);
            __Get_Value_From_Table(&Obj.Surface.craterDensity, SurfaceSubTable, L"craterDensity", Obj.Surface.craterDensity);
            __Get_Value_From_Table(&Obj.Surface.craterOctaves, SurfaceSubTable, L"craterOctaves", Obj.Surface.craterOctaves);
            __Get_Value_From_Table(&Obj.Surface.craterRayedFactor, SurfaceSubTable, L"craterRayedFactor", Obj.Surface.craterRayedFactor);
            __Get_Value_From_Table(&Obj.Surface.volcanoMagn, SurfaceSubTable, L"volcanoMagn", Obj.Surface.volcanoMagn);
            __Get_Value_From_Table(&Obj.Surface.volcanoFreq, SurfaceSubTable, L"volcanoFreq", Obj.Surface.volcanoFreq);
            __Get_Value_From_Table(&Obj.Surface.volcanoDensity, SurfaceSubTable, L"volcanoDensity", Obj.Surface.volcanoDensity);
            __Get_Value_From_Table(&Obj.Surface.volcanoOctaves, SurfaceSubTable, L"volcanoOctaves", Obj.Surface.volcanoOctaves);
            __Get_Value_From_Table(&Obj.Surface.volcanoActivity, SurfaceSubTable, L"volcanoActivity", Obj.Surface.volcanoActivity);
            __Get_Value_From_Table(&Obj.Surface.volcanoFlows, SurfaceSubTable, L"volcanoFlows", Obj.Surface.volcanoFlows);
            __Get_Value_From_Table(&Obj.Surface.volcanoRadius, SurfaceSubTable, L"volcanoRadius", Obj.Surface.volcanoRadius);
            __Get_Value_From_Table(&Obj.Surface.volcanoTemp, SurfaceSubTable, L"volcanoTemp", Obj.Surface.volcanoTemp);
            __Get_Value_From_Table(&Obj.Surface.lavaCoverTidal, SurfaceSubTable, L"lavaCoverTidal", Obj.Surface.lavaCoverTidal);
            __Get_Value_From_Table(&Obj.Surface.lavaCoverSun, SurfaceSubTable, L"lavaCoverSun", Obj.Surface.lavaCoverSun);
            __Get_Value_From_Table(&Obj.Surface.lavaCoverYoung, SurfaceSubTable, L"lavaCoverYoung", Obj.Surface.lavaCoverYoung);
            __Get_Value_From_Table(&Obj.Clouds.stripeZones, SurfaceSubTable, L"stripeZones", Obj.Clouds.stripeZones);
            __Get_Value_From_Table(&Obj.Clouds.stripeFluct, SurfaceSubTable, L"stripeFluct", Obj.Clouds.stripeFluct);
            __Get_Value_From_Table(&Obj.Clouds.stripeTwist, SurfaceSubTable, L"stripeTwist", Obj.Clouds.stripeTwist);
            __Get_Value_From_Table(&Obj.Clouds.cycloneMagn, SurfaceSubTable, L"cycloneMagn", Obj.Clouds.cycloneMagn);
            __Get_Value_From_Table(&Obj.Clouds.cycloneFreq, SurfaceSubTable, L"cycloneFreq", Obj.Clouds.cycloneFreq);
            __Get_Value_From_Table(&Obj.Clouds.cycloneDensity, SurfaceSubTable, L"cycloneDensity", Obj.Clouds.cycloneDensity);
            __Get_Value_From_Table(&Obj.Clouds.cycloneOctaves, SurfaceSubTable, L"cycloneOctaves", Obj.Clouds.cycloneOctaves);
            __Get_Value_From_Table(&Obj.Clouds.cycloneMagn2, SurfaceSubTable, L"cycloneMagn2", Obj.Clouds.cycloneMagn2);
            __Get_Value_From_Table(&Obj.Clouds.cycloneFreq2, SurfaceSubTable, L"cycloneFreq2", Obj.Clouds.cycloneFreq2);
            __Get_Value_From_Table(&Obj.Clouds.cycloneDensity2, SurfaceSubTable, L"cycloneDensity2", Obj.Clouds.cycloneDensity2);
            __Get_Value_From_Table(&Obj.Clouds.cycloneOctaves2, SurfaceSubTable, L"cycloneOctaves2", Obj.Clouds.cycloneOctaves2);
        }

        __Get_Value_From_Table(&Obj.NoOcean, CurrentTable, L"NoOcean", Obj.NoOcean);
        auto OceanTable = __Find_Table_From_List(CurrentTable, L"Ocean");
        if (OceanTable != CurrentTableEnd)
        {
            Obj.NoOcean = false;
            auto OceanSubTable = OceanTable->SubTable;
            __Get_Value_From_Table(&Obj.Ocean.Height, OceanSubTable, L"Height", Obj.Ocean.Height);
            if (IS_NO_DATA_DBL(Obj.Ocean.Height))
            {
                __Get_Value_From_Table(&Obj.Ocean.Height, OceanSubTable, L"Depth", Obj.Ocean.Height);
            }
            Obj.Ocean.Height *= 1000;
            __Get_Value_From_Table(&Obj.Ocean.Hapke, OceanSubTable, L"Hapke", Obj.Ocean.Hapke);
            __Get_Value_From_Table(&Obj.Ocean.SpotBright, OceanSubTable, L"SpotBright", Obj.Ocean.SpotBright);
            __Get_Value_From_Table(&Obj.Ocean.SpotWidth, OceanSubTable, L"SpotWidth", Obj.Ocean.SpotWidth);
            __Get_Value_From_Table(&Obj.Ocean.SpotBrightCB, OceanSubTable, L"SpotBrightCB", Obj.Ocean.SpotBrightCB);
            __Get_Value_From_Table(&Obj.Ocean.SpotWidthCB, OceanSubTable, L"SpotWidthCB", Obj.Ocean.SpotWidthCB);
            __Get_Value_From_Table(&Obj.Ocean.TempMapMaxTemp, OceanSubTable, L"TempMapMaxTemp", Obj.Ocean.TempMapMaxTemp);
            __Get_Value_From_Table(&Obj.Ocean.DayAmbient, OceanSubTable, L"DayAmbient", Obj.Ocean.DayAmbient);
            __Get_Value_From_Table(&Obj.Ocean.RingsWinter, OceanSubTable, L"RingsWinter", Obj.Ocean.RingsWinter);
            __Get_Value_From_Table(&Obj.Ocean.ModulateColor, OceanSubTable, L"ModulateColor", Obj.Ocean.ModulateColor);
            __Get_Value_From_Table(&Obj.Ocean.ModulateBright, OceanSubTable, L"ModulateBright", Obj.Ocean.ModulateBright);
            auto CompTable = __Find_Table_From_List(OceanSubTable, L"Composition");
            if (CompTable != OceanSubTable->Get().end()) {Obj.Ocean.Composition = TableToCompositeMap(CompTable->SubTable);}
        }
        //else {Obj.NoOcean = true;}

        __Get_Value_From_Table(&Obj.NoClouds, CurrentTable, L"NoClouds", Obj.NoClouds);
        auto CloudsTables = __Find_Multi_Tables_From_List(CurrentTable, L"Clouds");
        if (!CloudsTables.empty())
        {
            Obj.NoClouds = false;
            for (int i = 0; i < CloudsTables.size(); ++i)
            {
                auto CloudsTable = CloudsTables[i]->SubTable;
                Object::CloudParams::CloudLayerParam CloudLayer;
                __Get_Value_From_Table(&CloudLayer.DiffMap, CloudsTable, L"DiffMap", CloudLayer.DiffMap);
                __Get_Value_From_Table(&CloudLayer.DiffMapAlpha, CloudsTable, L"DiffMapAlpha", CloudLayer.DiffMapAlpha);
                __Get_Value_From_Table(&CloudLayer.BumpMap, CloudsTable, L"BumpMap", CloudLayer.BumpMap);
                __Get_Value_From_Table(&CloudLayer.BumpHeight, CloudsTable, L"BumpHeight", CloudLayer.BumpHeight);
                __Get_Value_From_Table(&CloudLayer.BumpOffset, CloudsTable, L"BumpOffset", CloudLayer.BumpOffset);
                __Get_Value_From_Table(&CloudLayer.GlowMap, CloudsTable, L"GlowMap", CloudLayer.GlowMap);
                __Get_Value_From_Table(&CloudLayer.GlowMode, CloudsTable, L"GlowMode", CloudLayer.GlowMode);
                __Get_Value_From_Table(&CloudLayer.GlowColor, CloudsTable, L"GlowColor", CloudLayer.GlowColor);
                __Get_Value_From_Table(&CloudLayer.GlowBright, CloudsTable, L"GlowBright", CloudLayer.GlowBright);
                __Get_Value_From_Table(&CloudLayer.SpecMap, CloudsTable, L"SpecMap", CloudLayer.SpecMap);
                __Get_Value_From_Table(&CloudLayer.FlipMap, CloudsTable, L"FlipMap", CloudLayer.FlipMap);
                __Get_Value_From_Table(&CloudLayer.Gamma, CloudsTable, L"Gamma", CloudLayer.Gamma);
                __Get_Value_From_Table(&CloudLayer.Hapke, CloudsTable, L"Hapke", CloudLayer.Hapke);
                __Get_Value_From_Table(&CloudLayer.SpotBright, CloudsTable, L"SpotBright", CloudLayer.SpotBright);
                __Get_Value_From_Table(&CloudLayer.SpotWidth, CloudsTable, L"SpotWidth", CloudLayer.SpotWidth);
                __Get_Value_From_Table(&CloudLayer.SpotBrightCB, CloudsTable, L"SpotBrightCB", CloudLayer.SpotBrightCB);
                __Get_Value_From_Table(&CloudLayer.SpotWidthCB, CloudsTable, L"SpotWidthCB", CloudLayer.SpotWidthCB);
                __Get_Value_From_Table(&CloudLayer.TempMapMaxTemp, CloudsTable, L"TempMapMaxTemp", CloudLayer.TempMapMaxTemp);
                __Get_Value_From_Table(&CloudLayer.DayAmbient, CloudsTable, L"DayAmbient", CloudLayer.DayAmbient);
                __Get_Value_From_Table(&CloudLayer.RingsWinter, CloudsTable, L"RingsWinter", CloudLayer.RingsWinter);
                __Get_Value_From_Table(&CloudLayer.ModulateColor, CloudsTable, L"ModulateColor", CloudLayer.ModulateColor);
                __Get_Value_From_Table(&CloudLayer.ModulateBright, CloudsTable, L"ModulateBright", CloudLayer.ModulateBright);
                __Get_Value_From_Table(&CloudLayer.Height, CloudsTable, L"Height", CloudLayer.Height);
                CloudLayer.Height *= 1000.;
                __Get_Value_From_Table(&CloudLayer.Velocity, CloudsTable, L"Velocity", CloudLayer.Velocity);
                CloudLayer.Velocity /= 3.6;
                __Get_Value_From_Table(&CloudLayer.RotationOffset, CloudsTable, L"RotationOffset", CloudLayer.RotationOffset);
                __Get_Value_From_Table(&CloudLayer.SubsurfScatBright, CloudsTable, L"SubsurfScatBright", CloudLayer.SubsurfScatBright);
                __Get_Value_From_Table(&CloudLayer.SubsurfScatPower, CloudsTable, L"SubsurfScatPower", CloudLayer.SubsurfScatPower);
                __Get_Value_From_Table(&CloudLayer.Opacity, CloudsTable, L"Opacity", CloudLayer.Opacity);
                Obj.Clouds.Layers.push_back(CloudLayer);
            }
            auto FirstCloudsTable = CloudsTables[0]->SubTable;
            __Get_Value_From_Table(&Obj.Clouds.TidalLocked, FirstCloudsTable, L"TidalLocked", Obj.Clouds.TidalLocked);
            __Get_Value_From_Table(&Obj.Clouds.Coverage, FirstCloudsTable, L"Coverage", Obj.Clouds.Coverage);
            __Get_Value_From_Table(&Obj.Clouds.mainFreq, FirstCloudsTable, L"mainFreq", Obj.Clouds.mainFreq);
            __Get_Value_From_Table(&Obj.Clouds.mainOctaves, FirstCloudsTable, L"mainOctaves", Obj.Clouds.mainOctaves);
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
        //else {Obj.NoClouds = true;}

        __Get_Value_From_Table(&Obj.NoAtmosphere, CurrentTable, L"NoAtmosphere", Obj.NoAtmosphere);
        auto AtmosphereTable = __Find_Table_From_List(CurrentTable, L"Atmosphere");
        if (AtmosphereTable != CurrentTableEnd)
        {
            Obj.NoAtmosphere = false;
            auto AtmoSubTable = AtmosphereTable->SubTable;
            __Get_Value_From_Table(&Obj.Atmosphere.Model, AtmoSubTable, L"Model", Obj.Atmosphere.Model);
            __Get_Value_From_Table(&Obj.Atmosphere.Height, AtmoSubTable, L"Height", Obj.Atmosphere.Height);
            Obj.Atmosphere.Height *= 1000;
            __Get_Value_From_Table(&Obj.Atmosphere.Density, AtmoSubTable, L"Density", Obj.Atmosphere.Density);
            __Get_Value_From_Table(&Obj.Atmosphere.Pressure, AtmoSubTable, L"Pressure", Obj.Atmosphere.Pressure);
            Obj.Atmosphere.Pressure *= StandardAtm;
            __Get_Value_From_Table(&Obj.Atmosphere.Adiabat, AtmoSubTable, L"Adiabat", Obj.Atmosphere.Adiabat);
            __Get_Value_From_Table(&Obj.Atmosphere.Greenhouse, AtmoSubTable, L"Greenhouse", Obj.Atmosphere.Greenhouse);
            __Get_Value_From_Table(&Obj.Atmosphere.Bright, AtmoSubTable, L"Bright", Obj.Atmosphere.Bright);
            __Get_Value_From_Table(&Obj.Atmosphere.Opacity, AtmoSubTable, L"Opacity", Obj.Atmosphere.Opacity);
            __Get_Value_From_Table(&Obj.Atmosphere.SkyLight, AtmoSubTable, L"SkyLight", Obj.Atmosphere.SkyLight);
            __Get_Value_From_Table(&Obj.Atmosphere.Hue, AtmoSubTable, L"Hue", Obj.Atmosphere.Hue);
            __Get_Value_From_Table(&Obj.Atmosphere.Saturation, AtmoSubTable, L"Saturation", Obj.Atmosphere.Saturation);
            auto CompTable = __Find_Table_From_List(AtmoSubTable, L"Composition");
            if (CompTable != AtmoSubTable->Get().end()) {Obj.Atmosphere.Composition = TableToCompositeMap(CompTable->SubTable);}
        }
        //else {Obj.NoAtmosphere = true;}

        auto ClimateTable = __Find_Table_From_List(CurrentTable, L"Climate");
        if (ClimateTable != CurrentTableEnd)
        {
            Obj.EnableClimate = true;
            auto ClimateSubTable = ClimateTable->SubTable;
            __Get_Value_From_Table(&Obj.Climate.GlobalWindSpeed, ClimateSubTable, L"GlobalWindSpeed", Obj.Climate.GlobalWindSpeed);
            __Get_Value_From_Table(&Obj.Climate.MinSurfaceTemp, ClimateSubTable, L"MinSurfaceTemp", Obj.Climate.MinSurfaceTemp);
            __Get_Value_From_Table(&Obj.Climate.MaxSurfaceTemp, ClimateSubTable, L"MaxSurfaceTemp", Obj.Climate.MaxSurfaceTemp);
            __Get_Value_From_Table(&Obj.Climate.AtmoProfile, ClimateSubTable, L"AtmoProfile", Obj.Climate.AtmoProfile);
        }

        __Get_Value_From_Table(&Obj.NoAurora, CurrentTable, L"NoAurora", Obj.NoAurora);
        auto AuroraTable = __Find_Table_From_List(CurrentTable, L"Aurora");
        if (AuroraTable != CurrentTableEnd)
        {
            Obj.NoAurora = false;
            auto AuroraSubTable = AuroraTable->SubTable;
            __Get_Value_From_Table(&Obj.Aurora.Height, AuroraSubTable, L"Height", Obj.Aurora.Height);
            Obj.Aurora.Height *= 1000.;
            __Get_Value_From_Table(&Obj.Aurora.NorthLat, AuroraSubTable, L"NorthLat", Obj.Aurora.NorthLat);
            __Get_Value_From_Table(&Obj.Aurora.NorthLon, AuroraSubTable, L"NorthLon", Obj.Aurora.NorthLon);
            __Get_Value_From_Table(&Obj.Aurora.NorthRadius, AuroraSubTable, L"NorthRadius", Obj.Aurora.NorthRadius);
            Obj.Aurora.NorthRadius *= 1000.;
            __Get_Value_From_Table(&Obj.Aurora.NorthWidth, AuroraSubTable, L"NorthWidth", Obj.Aurora.NorthWidth);
            Obj.Aurora.NorthWidth *= 1000.;
            __Get_Value_From_Table(&Obj.Aurora.NorthRings, AuroraSubTable, L"NorthRings", Obj.Aurora.NorthRings);
            __Get_Value_From_Table(&Obj.Aurora.NorthBright, AuroraSubTable, L"NorthBright", Obj.Aurora.NorthBright);
            __Get_Value_From_Table(&Obj.Aurora.NorthFlashFreq, AuroraSubTable, L"NorthFlashFreq", Obj.Aurora.NorthFlashFreq);
            __Get_Value_From_Table(&Obj.Aurora.NorthMoveSpeed, AuroraSubTable, L"NorthMoveSpeed", Obj.Aurora.NorthMoveSpeed);
            __Get_Value_From_Table(&Obj.Aurora.NorthParticles, AuroraSubTable, L"NorthParticles", Obj.Aurora.NorthParticles);
            __Get_Value_From_Table(&Obj.Aurora.SouthLat, AuroraSubTable, L"SouthLat", Obj.Aurora.SouthLat);
            __Get_Value_From_Table(&Obj.Aurora.SouthLon, AuroraSubTable, L"SouthLon", Obj.Aurora.SouthLon);
            __Get_Value_From_Table(&Obj.Aurora.SouthRadius, AuroraSubTable, L"SouthRadius", Obj.Aurora.SouthRadius);
            Obj.Aurora.SouthRadius *= 1000.;
            __Get_Value_From_Table(&Obj.Aurora.SouthWidth, AuroraSubTable, L"SouthWidth", Obj.Aurora.SouthWidth);
            Obj.Aurora.SouthWidth *= 1000.;
            __Get_Value_From_Table(&Obj.Aurora.SouthRings, AuroraSubTable, L"SouthRings", Obj.Aurora.SouthRings);
            __Get_Value_From_Table(&Obj.Aurora.SouthBright, AuroraSubTable, L"SouthBright", Obj.Aurora.SouthBright);
            __Get_Value_From_Table(&Obj.Aurora.SouthFlashFreq, AuroraSubTable, L"SouthFlashFreq", Obj.Aurora.SouthFlashFreq);
            __Get_Value_From_Table(&Obj.Aurora.SouthMoveSpeed, AuroraSubTable, L"SouthMoveSpeed", Obj.Aurora.SouthMoveSpeed);
            __Get_Value_From_Table(&Obj.Aurora.SouthParticles, AuroraSubTable, L"SouthParticles", Obj.Aurora.SouthParticles);
            __Get_Value_From_Table(&Obj.Aurora.TopColor, AuroraSubTable, L"TopColor", Obj.Aurora.TopColor);
            __Get_Value_From_Table(&Obj.Aurora.BottomColor, AuroraSubTable, L"BottomColor", Obj.Aurora.BottomColor);
        }
        //else {Obj.NoAurora = true;}

        __Get_Value_From_Table(&Obj.NoRings, CurrentTable, L"NoRings", Obj.NoRings);
        auto RingsTable = __Find_Table_From_List(CurrentTable, L"Rings");
        if (RingsTable != CurrentTableEnd)
        {
            Obj.NoRings = false;
            auto RingsSubTable = RingsTable->SubTable;
            __Get_Value_From_Table(&Obj.Rings.Texture, RingsSubTable, L"Texture", Obj.Rings.Texture);
            __Get_Value_From_Table(&Obj.Rings.InnerRadius, RingsSubTable, L"InnerRadius", Obj.Rings.InnerRadius);
            Obj.Rings.InnerRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.OuterRadius, RingsSubTable, L"OuterRadius", Obj.Rings.OuterRadius);
            Obj.Rings.OuterRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.EdgeRadius, RingsSubTable, L"EdgeRadius", Obj.Rings.EdgeRadius);
            Obj.Rings.EdgeRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.MeanRadius, RingsSubTable, L"MeanRadius", Obj.Rings.MeanRadius);
            Obj.Rings.MeanRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.Thickness, RingsSubTable, L"Thickness", Obj.Rings.Thickness);
            Obj.Rings.Thickness *= 1000;
            __Get_Value_From_Table(&Obj.Rings.RocksMaxSize, RingsSubTable, L"RocksMaxSize", Obj.Rings.RocksMaxSize);
            Obj.Rings.RocksMaxSize *= 1000;
            __Get_Value_From_Table(&Obj.Rings.RocksSpacing, RingsSubTable, L"RocksSpacing", Obj.Rings.RocksSpacing);
            __Get_Value_From_Table(&Obj.Rings.DustDrawDist, RingsSubTable, L"DustDrawDist", Obj.Rings.DustDrawDist);
            __Get_Value_From_Table(&Obj.Rings.ChartRadius, RingsSubTable, L"ChartRadius", Obj.Rings.ChartRadius);
            Obj.Rings.ChartRadius *= 1000;
            __Get_Value_From_Table(&Obj.Rings.RotationPeriod, RingsSubTable, L"RotationPeriod", Obj.Rings.RotationPeriod);
            Obj.Rings.RotationPeriod *= 3600;
            __Get_Value_From_Table(&Obj.Rings.Brightness, RingsSubTable, L"Brightness", Obj.Rings.Brightness);
            __Get_Value_From_Table(&Obj.Rings.FrontBright, RingsSubTable, L"FrontBright", Obj.Rings.FrontBright);
            __Get_Value_From_Table(&Obj.Rings.BackBright, RingsSubTable, L"BackBright", Obj.Rings.BackBright);
            __Get_Value_From_Table(&Obj.Rings.Density, RingsSubTable, L"Density", Obj.Rings.Density);
            __Get_Value_From_Table(&Obj.Rings.Opacity, RingsSubTable, L"Opacity", Obj.Rings.Opacity);
            __Get_Value_From_Table(&Obj.Rings.SelfShadow, RingsSubTable, L"SelfShadow", Obj.Rings.SelfShadow);
            __Get_Value_From_Table(&Obj.Rings.PlanetShadow, RingsSubTable, L"PlanetShadow", Obj.Rings.PlanetShadow);
            __Get_Value_From_Table(&Obj.Rings.Hapke, RingsSubTable, L"Hapke", Obj.Rings.Hapke);
            __Get_Value_From_Table(&Obj.Rings.SpotBright, RingsSubTable, L"SpotBright", Obj.Rings.SpotBright);
            __Get_Value_From_Table(&Obj.Rings.SpotWidth, RingsSubTable, L"SpotWidth", Obj.Rings.SpotWidth);
            __Get_Value_From_Table(&Obj.Rings.SpotBrightCB, RingsSubTable, L"SpotBrightCB", Obj.Rings.SpotBrightCB);
            __Get_Value_From_Table(&Obj.Rings.SpotWidthCB, RingsSubTable, L"SpotWidthCB", Obj.Rings.SpotWidthCB);
            __Get_Value_From_Table(&Obj.Rings.frequency, RingsSubTable, L"frequency", Obj.Rings.frequency);
            __Get_Value_From_Table(&Obj.Rings.densityScale, RingsSubTable, L"densityScale", Obj.Rings.densityScale);
            __Get_Value_From_Table(&Obj.Rings.densityOffset, RingsSubTable, L"densityOffset", Obj.Rings.densityOffset);
            __Get_Value_From_Table(&Obj.Rings.densityPower, RingsSubTable, L"densityPower", Obj.Rings.densityPower);
            __Get_Value_From_Table(&Obj.Rings.colorContrast, RingsSubTable, L"colorContrast", Obj.Rings.colorContrast);
            __Get_Value_From_Table(&Obj.Rings.FrontColor, RingsSubTable, L"FrontColor", Obj.Rings.FrontColor);
            __Get_Value_From_Table(&Obj.Rings.BackThickColor, RingsSubTable, L"BackThickColor", Obj.Rings.BackThickColor);
            __Get_Value_From_Table(&Obj.Rings.BackIceColor, RingsSubTable, L"BackIceColor", Obj.Rings.BackIceColor);
            __Get_Value_From_Table(&Obj.Rings.BackDustColor, RingsSubTable, L"BackDustColor", Obj.Rings.BackDustColor);
        }
        //else {Obj.NoRings = true;}

        __Get_Value_From_Table(&Obj.NoAccretionDisk, CurrentTable, L"NoAccretionDisk", Obj.NoAccretionDisk);
        auto AccDiskTable = __Find_Table_From_List(CurrentTable, L"AccretionDisk");
        if (AccDiskTable != CurrentTableEnd)
        {
            Obj.NoAccretionDisk = false;
            auto AccDiskSubTable = AccDiskTable->SubTable;
            __Get_Value_With_Unit(&Obj.AccretionDisk.InnerRadius, AccDiskSubTable, L"InnerRadius", Obj.AccretionDisk.InnerRadius, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.OuterRadius, AccDiskSubTable, L"OuterRadius", Obj.AccretionDisk.OuterRadius, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.InnerThickness, AccDiskSubTable, L"InnerThickness", Obj.AccretionDisk.InnerThickness, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.OuterThickness, AccDiskSubTable, L"OuterThickness", Obj.AccretionDisk.OuterThickness, AU, {{L"Km", 1000}});
            __Get_Value_From_Table(&Obj.AccretionDisk.ThicknessPow, AccDiskSubTable, L"ThicknessPow", Obj.AccretionDisk.ThicknessPow);
            __Get_Value_From_Table(&Obj.AccretionDisk.DetailScaleR, AccDiskSubTable, L"DetailScaleR", Obj.AccretionDisk.DetailScaleR);
            __Get_Value_From_Table(&Obj.AccretionDisk.DetailScaleV, AccDiskSubTable, L"DetailScaleV", Obj.AccretionDisk.DetailScaleV);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveDistortionX, AccDiskSubTable, L"OctaveDistortionX", Obj.AccretionDisk.OctaveDistortionX);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveDistortionY, AccDiskSubTable, L"OctaveDistortionY", Obj.AccretionDisk.OctaveDistortionY);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveDistortionZ, AccDiskSubTable, L"OctaveDistortionZ", Obj.AccretionDisk.OctaveDistortionZ);
            __Get_Value_From_Table(&Obj.AccretionDisk.OctaveScale, AccDiskSubTable, L"OctaveScale", Obj.AccretionDisk.OctaveScale);
            __Get_Value_From_Table(&Obj.AccretionDisk.DiskNoiseContrast, AccDiskSubTable, L"DiskNoiseContrast", Obj.AccretionDisk.DiskNoiseContrast);
            __Get_Value_From_Table(&Obj.AccretionDisk.DiskTempContrast, AccDiskSubTable, L"DiskTempContrast", Obj.AccretionDisk.DiskTempContrast);
            __Get_Value_From_Table(&Obj.AccretionDisk.DiskOuterSpin, AccDiskSubTable, L"DiskOuterSpin", Obj.AccretionDisk.DiskOuterSpin);
            __Get_Value_From_Table(&Obj.AccretionDisk.TwistMagn, AccDiskSubTable, L"TwistMagn", Obj.AccretionDisk.TwistMagn);
            __Get_Value_From_Table(&Obj.AccretionDisk.AccretionRate, AccDiskSubTable, L"AccretionRate", Obj.AccretionDisk.AccretionRate);
            __Get_Value_From_Table(&Obj.AccretionDisk.Temperature, AccDiskSubTable, L"Temperature", Obj.AccretionDisk.Temperature);
            __Get_Value_From_Table(&Obj.AccretionDisk.Density, AccDiskSubTable, L"Density", Obj.AccretionDisk.Density);
            __Get_Value_From_Table(&Obj.AccretionDisk.Luminosity, AccDiskSubTable, L"Luminosity", Obj.AccretionDisk.Luminosity);
            Obj.AccretionDisk.Luminosity *= SolarLum;
            __Get_Value_From_Table(&Obj.AccretionDisk.LuminosityBol, AccDiskSubTable, L"LuminosityBol", Obj.AccretionDisk.LuminosityBol);
            Obj.AccretionDisk.LuminosityBol *= SolarLumBol;
            __Get_Value_From_Table(&Obj.AccretionDisk.Brightness, AccDiskSubTable, L"Brightness", Obj.AccretionDisk.Brightness);

            __Get_Value_With_Unit(&Obj.AccretionDisk.JetLength, AccDiskSubTable, L"JetLength", Obj.AccretionDisk.JetLength, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.JetStartRadius, AccDiskSubTable, L"JetStartRadius", Obj.AccretionDisk.JetStartRadius, AU, {{L"Km", 1000}});
            __Get_Value_With_Unit(&Obj.AccretionDisk.JetEndRadius, AccDiskSubTable, L"JetEndRadius", Obj.AccretionDisk.JetEndRadius, AU, {{L"Km", 1000}});
            __Get_Value_From_Table(&Obj.AccretionDisk.JetStartTemp, AccDiskSubTable, L"JetStartTemp", Obj.AccretionDisk.JetStartTemp);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetEndTemp, AccDiskSubTable, L"JetEndTemp", Obj.AccretionDisk.JetEndTemp);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetVelocity, AccDiskSubTable, L"JetVelocity", Obj.AccretionDisk.JetVelocity);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetDistortion, AccDiskSubTable, L"JetDistortion", Obj.AccretionDisk.JetDistortion);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetThickness, AccDiskSubTable, L"JetThickness", Obj.AccretionDisk.JetThickness);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetDensity, AccDiskSubTable, L"JetDensity", Obj.AccretionDisk.JetDensity);
            __Get_Value_From_Table(&Obj.AccretionDisk.JetBrightness, AccDiskSubTable, L"JetBrightness", Obj.AccretionDisk.JetBrightness);

            __Get_Value_From_Table(&Obj.AccretionDisk.LightingBright, AccDiskSubTable, L"LightingBright", Obj.AccretionDisk.LightingBright);
            __Get_Value_From_Table(&Obj.AccretionDisk.ShadowContrast, AccDiskSubTable, L"ShadowContrast", Obj.AccretionDisk.ShadowContrast);
            __Get_Value_From_Table(&Obj.AccretionDisk.ShadowLength, AccDiskSubTable, L"ShadowLength", Obj.AccretionDisk.ShadowLength);
        }
        //else {Obj.NoAccretionDisk = true;}

        __Get_Value_From_Table(&Obj.NoCorona, CurrentTable, L"NoCorona", Obj.NoCorona);
        auto CoronaTable = __Find_Table_From_List(CurrentTable, L"Corona");
        if (CoronaTable != CurrentTableEnd)
        {
            Obj.NoCorona = false;
            auto CoronaSubTable = CoronaTable->SubTable;
            __Get_Value_From_Table(&Obj.Corona.Radius, CoronaSubTable, L"Radius", Obj.Corona.Radius);
            __Get_Value_From_Table(&Obj.Corona.Period, CoronaSubTable, L"Period", Obj.Corona.Period);
            __Get_Value_From_Table(&Obj.Corona.Brightness, CoronaSubTable, L"Brightness", Obj.Corona.Brightness);
            __Get_Value_From_Table(&Obj.Corona.RayDensity, CoronaSubTable, L"RayDensity", Obj.Corona.RayDensity);
            __Get_Value_From_Table(&Obj.Corona.RayCurv, CoronaSubTable, L"RayCurv", Obj.Corona.RayCurv);
        }
        //else {Obj.NoCorona = true;}

        __Get_Value_From_Table(&Obj.NoCometTail, CurrentTable, L"NoCometTail", Obj.NoCometTail);
        auto CometTailTable = __Find_Table_From_List(CurrentTable, L"CometTail");
        if (CometTailTable != CurrentTableEnd)
        {
            Obj.NoCometTail = false;
            auto CometTailSubTable = CometTailTable->SubTable;
            __Get_Value_From_Table(&Obj.CometTail.MaxLength, CometTailSubTable, L"MaxLength", Obj.CometTail.MaxLength);
            __Get_Value_From_Table(&Obj.CometTail.GasToDust, CometTailSubTable, L"GasToDust", Obj.CometTail.GasToDust);
            __Get_Value_From_Table(&Obj.CometTail.Particles, CometTailSubTable, L"Particles", Obj.CometTail.Particles);
            __Get_Value_From_Table(&Obj.CometTail.GasBright, CometTailSubTable, L"GasBright", Obj.CometTail.GasBright);
            __Get_Value_From_Table(&Obj.CometTail.DustBright, CometTailSubTable, L"DustBright", Obj.CometTail.DustBright);
            __Get_Value_From_Table(&Obj.CometTail.GasColor, CometTailSubTable, L"GasColor", Obj.CometTail.GasColor);
            __Get_Value_From_Table(&Obj.CometTail.DustColor, CometTailSubTable, L"DustColor", Obj.CometTail.DustColor);
        }
        //else {Obj.NoCometTail = true;}
    }

    return Obj;
}

template<> Object GetObject(_SC SharedTablePointer Table, ustring Name)
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
    return GetObjectFromKeyValue(*it);
}

template<> _SC SCSTable MakeTable(Object Obj, int Fl, std::streamsize Prec)
{
    _SC SCSTable MainTable, ContentTable;
    bool FixedOutput = !(Fl & __Object_Manipulator::Scientific);
    bool Booleans = (Fl & __Object_Manipulator::Booleans);
    __Add_Key_Value(&MainTable, Obj.Type, __Str_List_To_String(Obj.Name), Fl, Prec);

    __Add_Key_Value(&ContentTable, L"DateUpdated", Obj.DateUpdated, FixedOutput, Prec);
    __Add_Key_Value(&ContentTable, L"DiscMethod", Obj.DiscMethod, FixedOutput, Prec);
    __Add_Key_Value(&ContentTable, L"DiscDate", Obj.DiscDate, FixedOutput, Prec);
    __Add_Key_Value(&ContentTable, L"ParentBody", Obj.ParentBody, FixedOutput, Prec);
    if (Obj.Type == L"Star")
    {
        __Add_Key_Value(&ContentTable, L"Class", Obj.SpecClass, FixedOutput, Prec);
    }
    else {__Add_Key_Value(&ContentTable, L"Class", Obj.Class, FixedOutput, Prec);}
    __Add_Key_Value(&ContentTable, L"AsterType", Obj.AsterType, FixedOutput, Prec);
    __Add_Key_Value(&ContentTable, L"CometType", Obj.CometType, FixedOutput, Prec);

    __Add_Key_Value(&ContentTable, L"MassKg", Obj.Mass, FixedOutput, Prec);
    if (Fl & __Object_Manipulator::AutoRadius)
    {
        if (Obj.Dimensions.x == Obj.Dimensions.y && Obj.Dimensions.y == Obj.Dimensions.z)
        {
            if (Fl & __Object_Manipulator::FlatObjDim)
            {
                __Add_Key_Value(&ContentTable, L"Radius", Obj.Dimensions.x / 2. / 1000., FixedOutput, Prec);
            }
            else
            {
                __Add_Key_Value(&ContentTable, L"Radius", Obj.Dimensions.x / 2. / 1000., FixedOutput, Prec);
                __Add_Key_Value(&ContentTable, L"Oblateness", vec3{0, 0, 0}, FixedOutput, Prec);
            }
        }
        else
        {
            if (Fl & __Object_Manipulator::FlatObjDim)
            {
                __Add_Key_Value(&ContentTable, L"Dimensions", vec3(Obj.Dimensions / 1000.), FixedOutput, Prec);
            }
            else
            {
                auto Radius = vec3(Obj.Dimensions / 2.);
                auto MaxRad = cse::max({Radius.x, Radius.y, Radius.z});
                __Add_Key_Value(&ContentTable, L"Radius", MaxRad / 1000., FixedOutput, Prec);
                __Add_Key_Value(&ContentTable, L"Oblateness", _ASOBJ Flattening(Obj), FixedOutput, Prec);
            }
        }
    }
    else {__Add_Key_Value(&ContentTable, L"Dimensions", vec3(Obj.Dimensions / 1000.), FixedOutput, Prec);}

    if (Fl & __Object_Manipulator::Physical)
    {
        __Add_Key_Value(&ContentTable, L"InertiaMoment", Obj.InertiaMoment, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"AlbedoBond", Obj.AlbedoBond, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"AlbedoGeom", Obj.AlbedoGeom, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"EndogenousHeating", Obj.EndogenousHeating, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"ThermalLuminosity", Obj.ThermalLuminosity, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"ThermalLuminosityBol", Obj.ThermalLuminosityBol, FixedOutput, Prec);
        if (Obj.Type == L"Star")
        {
            __Add_Key_Value(&ContentTable, L"Teff", Obj.Temperature, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"Luminosity", Obj.Luminosity / SolarLum, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"LumBol", Obj.LumBol / SolarLum, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"FeH", Obj.FeH, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"CtoO", Obj.CtoO, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"Age", Obj.Age / 1E+12, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"KerrSpin", Obj.KerrSpin, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"KerrCharge", Obj.KerrCharge, FixedOutput, Prec);
        }
    }

    if (Fl & __Object_Manipulator::Optical)
    {
        __Add_Key_Value(&ContentTable, L"Color", Obj.Color, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"AbsMagn", Obj.AbsMagn, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"SlopeParam", Obj.SlopeParam, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Brightness", Obj.Brightness, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"BrightnessReal", Obj.BrightnessReal, FixedOutput, Prec);
    }

    if (Fl & __Object_Manipulator::Rotation)
    {
        __Add_Key_Value(&ContentTable, L"RotationEpoch", Obj.Rotation.RotationEpoch, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Obliquity", Obj.Rotation.Obliquity, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"EqAscendNode", Obj.Rotation.EqAscendNode, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"RotationOffset", Obj.Rotation.RotationOffset, FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"RotationPeriod", Obj.Rotation.RotationPeriod / 3600., FixedOutput, Prec);
        __Add_Key_Value(&ContentTable, L"Precession", Obj.Rotation.Precession / JulianYear, FixedOutput, Prec);
        if (Obj.Rotation.TidalLocked && (Fl & __Object_Manipulator::FTidalLock))
        {
            __Add_Key_Value(&ContentTable, L"TidalLocked", Obj.Rotation.TidalLocked, 0, Prec);
        }
        else if (Obj.RotationModel == L"IAU")
        {
            __Add_Key_Value(&ContentTable, L"RotationModel", Obj.RotationModel, FixedOutput, Prec);
            _SC SCSTable IAUTable;
            __Add_Key_Value(&IAUTable, L"ValidRange", Obj.RotationIAU.ValidRange, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"Epoch", Obj.RotationIAU.Epoch, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"PoleRA", Obj.RotationIAU.PoleRA, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"PoleRARate", Obj.RotationIAU.PoleRARate, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"PoleDec", Obj.RotationIAU.PoleDec, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"PoleDecRate", Obj.RotationIAU.PoleDecRate, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"PrimeMeridian", Obj.RotationIAU.PrimeMeridian, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"RotationRate", Obj.RotationIAU.RotationRate, FixedOutput, Prec);
            __Add_Key_Value(&IAUTable, L"RotationAccel", Obj.RotationIAU.RotationAccel, FixedOutput, Prec);
            if (!Obj.RotationIAU.PeriodicTerms.empty())
            {
                _SC ValueType PeriodicTerms;
                ustringlist VList;
                for (int i = 0; i < Obj.RotationIAU.PeriodicTerms.size(); ++i)
                {
                    for (int j = 0; j < 6; ++j)
                    {
                        std::wostringstream ValueStr;
                        if (FixedOutput) {ValueStr << fixed;}
                        ValueStr.precision(Prec);
                        ValueStr << Obj.RotationIAU.PeriodicTerms[i][j];
                        VList.push_back(ustring(ValueStr.str()));
                    }
                }
                PeriodicTerms.Type = PeriodicTerms.Matrix;
                PeriodicTerms.Value = VList;
                _SC SCSTable::SCKeyValue PTKV;
                if (Obj.RotationIAU.UsingSecular) {PTKV.Key = L"PeriodicTermsSecular";}
                else {PTKV.Key = L"PeriodicTermsDiurnal";}
                PTKV.Value.push_back(PeriodicTerms);
                IAUTable.Get().push_back(PTKV);
            }
            ContentTable.Get().back().SubTable = make_shared<decltype(IAUTable)>(IAUTable);
        }
    }

    /**
     * @brief Smart orbit elements output
     *
     *     Object	            Elements used
     *     Major planet	        e, a, i, Ω, ϖ, L0
     *     Comet	            e, q, i, Ω, ω, T0
     *     Asteroid	            e, a, i, Ω, ω, M0
     *     Two - line elements	e, i, Ω, ω, n, M0
     *
     * https://en.wikipedia.org/wiki/Orbital_elements
     */
    if (!IS_NO_DATA_STR(Obj.Orbit.RefPlane))
    {
        if (Fl & __Object_Manipulator::AutoOrbit)
        {
            if (Obj.Orbit.RefPlane == L"Static")
            {
                __Add_Key_Value(&ContentTable, L"StaticPosXYZ", Obj.Position, FixedOutput, Prec);
            }
            else if (Obj.Orbit.RefPlane == L"Fixed")
            {
                __Add_Key_Value(&ContentTable, L"FixedPosXYZ", Obj.Position, FixedOutput, Prec);
            }
            else if (Obj.Orbit.RefPlane == L"Analytic")
            {
                __Add_Empty_Tag(&ContentTable);
                _SC SCSTable OrbitTable;
                ContentTable.Get().back().Key = L"Orbit";
                __Add_Key_Value(&OrbitTable, L"RefPlane", Obj.Orbit.RefPlane, FixedOutput, Prec);
                __Add_Key_Value(&OrbitTable, L"AnalyticModel", Obj.Orbit.AnalyticModel, FixedOutput, Prec);
                ContentTable.Get().back().SubTable = make_shared<decltype(OrbitTable)>(OrbitTable);
            }
            else
            {
                __Add_Empty_Tag(&ContentTable);
                _SC SCSTable OrbitTable;
                __Add_Key_Value(&OrbitTable, L"RefPlane", Obj.Orbit.RefPlane, FixedOutput, Prec);
                if (Obj.Orbit.Binary)
                {
                    ContentTable.Get().back().Key = L"BinaryOrbit";
                    __Add_Key_Value(&OrbitTable, L"Epoch", Obj.Orbit.Epoch, FixedOutput, Prec);
                    __Add_Key_Value(&OrbitTable, L"Separation", Obj.Orbit.Separation / AU, FixedOutput, Prec);
                    __Add_Key_Value(&OrbitTable, L"PositionAngle", Obj.Orbit.PositionAngle, FixedOutput, Prec);
                }
                else
                {
                    ContentTable.Get().back().Key = L"Orbit";
                    if (Obj.Type == L"Planet")
                    {
                        __Add_Key_Value(&OrbitTable, L"Epoch", Obj.Orbit.Epoch, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Eccentricity", Obj.Orbit.Eccentricity, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"SemiMajorAxisKm", _ASOBJ SemiMajorAxis(Obj) / 1000., FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Inclination", Obj.Orbit.Inclination, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"AscendingNode", Obj.Orbit.AscendingNode, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"LongOfPericen", _ASOBJ LongitudeOfPerihelion(Obj), FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"MeanLongitude", _ASOBJ MeanLongitude(Obj), FixedOutput, Prec);
                    }
                    else if (Obj.Type == L"Comet")
                    {
                        __Add_Key_Value(&OrbitTable, L"Epoch", Obj.Orbit.Epoch, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Eccentricity", Obj.Orbit.Eccentricity, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"PericenterDist", Obj.Orbit.PericenterDist / AU, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Inclination", Obj.Orbit.Inclination, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"AscendingNode", Obj.Orbit.AscendingNode, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"ArgOfPericenter", Obj.Orbit.ArgOfPericenter, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"MeanAnomaly", Obj.Orbit.MeanAnomaly, FixedOutput, Prec);
                    }
                    else if (Obj.Type == L"Spacecraft" || Obj.Type == L"Structure" || Obj.Type == L"Artifact")
                    {
                        __Add_Key_Value(&OrbitTable, L"Epoch", Obj.Orbit.Epoch, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Eccentricity", Obj.Orbit.Eccentricity, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Inclination", Obj.Orbit.Inclination, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"AscendingNode", Obj.Orbit.AscendingNode, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"ArgOfPericenter", Obj.Orbit.ArgOfPericenter, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"MeanMotion", _ASOBJ MeanMotion(Obj), FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"MeanAnomaly", Obj.Orbit.MeanAnomaly, FixedOutput, Prec);
                    }
                    else
                    {
                        __Add_Key_Value(&OrbitTable, L"Epoch", Obj.Orbit.Epoch, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Eccentricity", Obj.Orbit.Eccentricity, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"SemiMajorAxisKm", _ASOBJ SemiMajorAxis(Obj) / 1000., FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"PeriodDays", Obj.Orbit.Period / SynodicDay, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"Inclination", Obj.Orbit.Inclination, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"AscendingNode", Obj.Orbit.AscendingNode, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"ArgOfPericenter", Obj.Orbit.ArgOfPericenter, FixedOutput, Prec);
                        __Add_Key_Value(&OrbitTable, L"MeanAnomaly", Obj.Orbit.MeanAnomaly, FixedOutput, Prec);
                    }
                }
                ContentTable.Get().back().SubTable = make_shared<decltype(OrbitTable)>(OrbitTable);
            }
        }
        else
        {
            __Add_Key_Value(&ContentTable, L"StaticPosXYZ", Obj.Position, FixedOutput, Prec);
            __Add_Key_Value(&ContentTable, L"FixedPosXYZ", Obj.Position, FixedOutput, Prec);
            __Add_Empty_Tag(&ContentTable);
            _SC SCSTable OrbitTable;
            if (Obj.Orbit.Binary) {ContentTable.Get().back().Key = L"BinaryOrbit";}
            else {ContentTable.Get().back().Key = L"Orbit";}
            __Add_Key_Value(&OrbitTable, L"RefPlane", Obj.Orbit.RefPlane, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"AnalyticModel", Obj.Orbit.AnalyticModel, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"Epoch", Obj.Orbit.Epoch, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"Separation", Obj.Orbit.Separation, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"PositionAngle", Obj.Orbit.PositionAngle, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"PeriodDays", Obj.Orbit.Period / SynodicDay, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"SemiMajorAxisKm", _ASOBJ SemiMajorAxis(Obj) / 1000., FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"GravParam", Obj.Orbit.GravParam, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"Eccentricity", Obj.Orbit.Eccentricity, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"Inclination", Obj.Orbit.Inclination, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"AscendingNode", Obj.Orbit.AscendingNode, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"AscNodePreces", Obj.Orbit.AscNodePreces, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"ArgOfPericenter", Obj.Orbit.ArgOfPericenter, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"ArgOfPeriPreces", Obj.Orbit.ArgOfPeriPreces, FixedOutput, Prec);
            __Add_Key_Value(&OrbitTable, L"MeanAnomaly", Obj.Orbit.MeanAnomaly, FixedOutput, Prec);
            ContentTable.Get().back().SubTable = make_shared<decltype(OrbitTable)>(OrbitTable);
        }
    }

    //if (Booleans) {__Add_Key_Value(&ContentTable, L"NoLife", bool(Obj.LifeCount), 0, Prec);}
    if ((Fl & __Object_Manipulator::Life) && Obj.LifeCount)
    {
        auto AddLife = [&](Object::LifeParams Life)
        {
            __Add_Empty_Tag(&ContentTable);
            ContentTable.Get().back().Key = L"Life";
            _SC SCSTable LifeTable;
            __Add_Key_Value(&LifeTable, L"Class", Life.Class, FixedOutput, Prec);
            __Add_Key_Value(&LifeTable, L"Type", Life.Type, FixedOutput, Prec);
            __Add_Key_Value(&LifeTable, L"Biome", __Str_List_To_String(Life.Biome), FixedOutput, Prec);
            __Add_Key_Value(&LifeTable, L"Panspermia", Life.Panspermia, 0, Prec);
            ContentTable.Get().back().SubTable = make_shared<decltype(LifeTable)>(LifeTable);
        };
        switch (Obj.LifeCount)
        {
        case 2:
            AddLife(Obj.Life[1]);
        case 1:
            AddLife(Obj.Life[0]);
            break;
        default:
            break;
        }
    }

    auto AddCompositeTable = [&](_SC SCSTable* Table, CompositionType Composition)
    {
        auto it = Composition.begin();
        auto end = Composition.end();
        for (; it != end; ++it)
        {
            __Add_Key_Value(Table, it->first, it->second, FixedOutput, Prec);
        }
    };

    if ((Fl & __Object_Manipulator::Interior) && !Obj.Interior.empty())
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Interior";
        _SC SCSTable InterTable;
        __Add_Empty_Tag(&InterTable);
        InterTable.Get().back().Key = L"Composition";
        _SC SCSTable CompoTable;
        AddCompositeTable(&CompoTable, Obj.Interior);
        InterTable.Get().back().SubTable = make_shared<decltype(CompoTable)>(CompoTable);
        ContentTable.Get().back().SubTable = make_shared<decltype(InterTable)>(InterTable);
    }

    if ((Fl & __Object_Manipulator::Surface) && Obj.EnableSurface)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Surface";
        _SC SCSTable SurfaceTable;
        __Add_Key_Value(&SurfaceTable, L"DiffMap", Obj.Surface.DiffMap, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"DiffMapAlpha", Obj.Surface.DiffMapAlpha, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"BumpMap", Obj.Surface.BumpMap, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"BumpHeight", Obj.Surface.BumpHeight, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"BumpOffset", Obj.Surface.BumpOffset, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"GlowMap", Obj.Surface.GlowMap, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"GlowMode", Obj.Surface.GlowMode, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"GlowColor", Obj.Surface.GlowColor, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"GlowBright", Obj.Surface.GlowBright, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpecMap", Obj.Surface.SpecMap, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"FlipMap", Obj.Surface.FlipMap, 0, Prec);
        __Add_Key_Value(&SurfaceTable, L"Gamma", Obj.Surface.Gamma, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"Hapke", Obj.Surface.Hapke, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpotBright", Obj.Surface.SpotBright, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpotWidth", Obj.Surface.SpotWidth, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpotBrightCB", Obj.Surface.SpotBrightCB, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpotWidthCB", Obj.Surface.SpotWidthCB, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"TempMapMaxTemp", Obj.Surface.TempMapMaxTemp, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"DayAmbient", Obj.Surface.DayAmbient, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"RingsWinter", Obj.Surface.RingsWinter, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"ModulateColor", Obj.Surface.ModulateColor, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"ModulateBright", Obj.Surface.ModulateBright, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"Preset", Obj.Surface.Preset, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SurfStyle", Obj.Surface.SurfStyle, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"Randomize", Obj.Surface.Randomize, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"colorDistMagn", Obj.Surface.colorDistMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"colorDistFreq", Obj.Surface.colorDistFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"detailScale", Obj.Surface.detailScale, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"drivenDarkening", Obj.Surface.drivenDarkening, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"seaLevel", Obj.Surface.seaLevel, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"snowLevel", Obj.Surface.snowLevel, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"tropicLatitude", Obj.Surface.tropicLatitude, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"icecapLatitude", Obj.Surface.icecapLatitude, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"icecapHeight", Obj.Surface.icecapHeight, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climatePole", Obj.Surface.climatePole, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateTropic", Obj.Surface.climateTropic, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateEquator", Obj.Surface.climateEquator, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateSteppeMin", Obj.Surface.climateSteppeMin, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateSteppeMax", Obj.Surface.climateSteppeMax, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateForestMin", Obj.Surface.climateForestMin, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateForestMax", Obj.Surface.climateForestMax, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateGrassMin", Obj.Surface.climateGrassMin, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"climateGrassMax", Obj.Surface.climateGrassMax, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"humidity", Obj.Surface.humidity, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"heightTempGrad", Obj.Surface.heightTempGrad, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"beachWidth", Obj.Surface.beachWidth, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"tropicWidth", Obj.Surface.tropicWidth, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"mainFreq", Obj.Surface.mainFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"venusFreq", Obj.Surface.venusFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"venusMagn", Obj.Surface.venusMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"mareFreq", Obj.Surface.mareFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"mareDensity", Obj.Surface.mareDensity, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"terraceProb", Obj.Surface.terraceProb, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"erosion", Obj.Surface.erosion, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"montesMagn", Obj.Surface.montesMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"montesFreq", Obj.Surface.montesFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"montesSpiky", Obj.Surface.montesSpiky, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"montesFraction", Obj.Surface.montesFraction, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"dunesMagn", Obj.Surface.dunesMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"dunesFreq", Obj.Surface.dunesFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"dunesFraction", Obj.Surface.dunesFraction, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"hillsMagn", Obj.Surface.hillsMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"hillsFreq", Obj.Surface.hillsFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"hillsFraction", Obj.Surface.hillsFraction, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"hills2Fraction", Obj.Surface.hills2Fraction, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"riversMagn", Obj.Surface.riversMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"riversFreq", Obj.Surface.riversFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"riversSin", Obj.Surface.riversSin, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"riftsMagn", Obj.Surface.riftsMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"riftsFreq", Obj.Surface.riftsFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"riftsSin", Obj.Surface.riftsSin, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"eqridgeMagn", Obj.Surface.eqridgeMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"eqridgeWidth", Obj.Surface.eqridgeWidth, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"eqridgeModMagn", Obj.Surface.eqridgeModMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"eqridgeModFreq", Obj.Surface.eqridgeModFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"canyonsMagn", Obj.Surface.canyonsMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"canyonsFreq", Obj.Surface.canyonsFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"canyonsFraction", Obj.Surface.canyonsFraction, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"cracksMagn", Obj.Surface.cracksMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"cracksFreq", Obj.Surface.cracksFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"cracksOctaves", Obj.Surface.cracksOctaves, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"craterMagn", Obj.Surface.craterMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"craterFreq", Obj.Surface.craterFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"craterDensity", Obj.Surface.craterDensity, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"craterOctaves", Obj.Surface.craterOctaves, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"craterRayedFactor", Obj.Surface.craterRayedFactor, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoMagn", Obj.Surface.volcanoMagn, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoFreq", Obj.Surface.volcanoFreq, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoDensity", Obj.Surface.volcanoDensity, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoOctaves", Obj.Surface.volcanoOctaves, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoActivity", Obj.Surface.volcanoActivity, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoFlows", Obj.Surface.volcanoFlows, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoRadius", Obj.Surface.volcanoRadius, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"volcanoTemp", Obj.Surface.volcanoTemp, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"lavaCoverTidal", Obj.Surface.lavaCoverTidal, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"lavaCoverSun", Obj.Surface.lavaCoverSun, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"lavaCoverYoung", Obj.Surface.lavaCoverYoung, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpecBrightWater", Obj.Surface.SpecBrightWater, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpecBrightIce", Obj.Surface.SpecBrightIce, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"RoughnessWater", Obj.Surface.RoughnessWater, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"RoughnessIce", Obj.Surface.RoughnessIce, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"SpecularScale", Obj.Surface.SpecularScale, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"RoughnessBias", Obj.Surface.RoughnessBias, FixedOutput, Prec);
        __Add_Key_Value(&SurfaceTable, L"RoughnessScale", Obj.Surface.RoughnessScale, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(SurfaceTable)>(SurfaceTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoOcean", Obj.NoOcean, 0, Prec);}
    if ((Fl & __Object_Manipulator::Ocean) && !Obj.NoOcean)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Ocean";
        _SC SCSTable OceanTable;
        __Add_Key_Value(&OceanTable, L"Height", Obj.Ocean.Height / 1000., FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"Hapke", Obj.Ocean.Hapke, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"SpotBright", Obj.Ocean.SpotBright, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"SpotWidth", Obj.Ocean.SpotWidth, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"SpotBrightCB", Obj.Ocean.SpotBrightCB, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"SpotWidthCB", Obj.Ocean.SpotWidthCB, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"TempMapMaxTemp", Obj.Ocean.TempMapMaxTemp, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"DayAmbient", Obj.Ocean.DayAmbient, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"RingsWinter", Obj.Ocean.RingsWinter, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"ModulateColor", Obj.Ocean.ModulateColor, FixedOutput, Prec);
        __Add_Key_Value(&OceanTable, L"ModulateBright", Obj.Ocean.ModulateBright, FixedOutput, Prec);
        if (!Obj.Ocean.Composition.empty())
        {
            __Add_Empty_Tag(&OceanTable);
            OceanTable.Get().back().Key = L"Composition";
            _SC SCSTable CompoTable;
            AddCompositeTable(&CompoTable, Obj.Ocean.Composition);
            OceanTable.Get().back().SubTable = make_shared<decltype(CompoTable)>(CompoTable);
        }
        ContentTable.Get().back().SubTable = make_shared<decltype(OceanTable)>(OceanTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoClouds", Obj.NoClouds, 0, Prec);}
    if ((Fl & __Object_Manipulator::Clouds) && !Obj.NoClouds)
    {
        auto AddClouds = [&](Object::CloudParams::CloudLayerParam Layer)
        {
            __Add_Empty_Tag(&ContentTable);
            ContentTable.Get().back().Key = L"Clouds";
            _SC SCSTable CloudLayerTable;
            __Add_Key_Value(&CloudLayerTable, L"DiffMap", Layer.DiffMap, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"DiffMapAlpha", Layer.DiffMapAlpha, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"BumpMap", Layer.BumpMap, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"BumpHeight", Layer.BumpHeight, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"BumpOffset", Layer.BumpOffset, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"GlowMap", Layer.GlowMap, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"GlowMode", Layer.GlowMode, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"GlowColor", Layer.GlowColor, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"GlowBright", Layer.GlowBright, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SpecMap", Layer.SpecMap, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"FlipMap", Layer.FlipMap, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"Gamma", Layer.Gamma, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"Hapke", Layer.Hapke, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SpotBright", Layer.SpotBright, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SpotWidth", Layer.SpotWidth, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SpotBrightCB", Layer.SpotBrightCB, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SpotWidthCB", Layer.SpotWidthCB, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"TempMapMaxTemp", Layer.TempMapMaxTemp, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"DayAmbient", Layer.DayAmbient, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"RingsWinter", Layer.RingsWinter, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"ModulateColor", Layer.ModulateColor, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"ModulateBright", Layer.ModulateBright, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"Height", Layer.Height / 1000., FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"Velocity", Layer.Velocity * 3.6, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"RotationOffset", Layer.RotationOffset, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SubsurfScatBright", Layer.SubsurfScatBright, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"SubsurfScatPower", Layer.SubsurfScatPower, FixedOutput, Prec);
            __Add_Key_Value(&CloudLayerTable, L"Opacity", Layer.Opacity, FixedOutput, Prec);
            ContentTable.Get().back().SubTable = make_shared<decltype(CloudLayerTable)>(CloudLayerTable);
        };
        if (!Obj.Clouds.Layers.empty()){AddClouds(Obj.Clouds.Layers[0]);}
        else
        {
            __Add_Empty_Tag(&ContentTable);
            ContentTable.Get().back().Key = L"Clouds";
        }
        auto CloudIt = ContentTable.Get().end() - 1;
        #if STRIPE_CYCLONE_OUTPUT_TO_CLOUDS
        auto CloudAppIt = CloudIt;
        #else
        auto CloudAppIt = find_if(ContentTable.Get().begin(), ContentTable.Get().end(),
        [](_SC SCSTable::ValueType Tbl)
        {
            return Tbl.Key == L"Surface";
        });
        if (CloudAppIt == ContentTable.Get().end())
        {
            __Add_Empty_Tag(&ContentTable);
            ContentTable.Get().back().Key = L"Surface";
            CloudAppIt = ContentTable.Get().end() - 1;
        }
        #endif
        _SC SCSTable CloudTable, ClApperTable;
        __Add_Key_Value(&CloudTable, L"TidalLocked", Obj.Clouds.TidalLocked, 0, Prec);
        __Add_Key_Value(&CloudTable, L"Coverage", Obj.Clouds.Coverage, FixedOutput, Prec);
        __Add_Key_Value(&CloudTable, L"mainFreq", Obj.Clouds.mainFreq, FixedOutput, Prec);
        __Add_Key_Value(&CloudTable, L"mainOctaves", Obj.Clouds.mainOctaves, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"stripeZones", Obj.Clouds.stripeZones, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"stripeFluct", Obj.Clouds.stripeFluct, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"stripeTwist", Obj.Clouds.stripeTwist, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneMagn", Obj.Clouds.cycloneMagn, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneFreq", Obj.Clouds.cycloneFreq, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneDensity", Obj.Clouds.cycloneDensity, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneOctaves", Obj.Clouds.cycloneOctaves, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneMagn2", Obj.Clouds.cycloneMagn2, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneFreq2", Obj.Clouds.cycloneFreq2, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneDensity2", Obj.Clouds.cycloneDensity2, FixedOutput, Prec);
        __Add_Key_Value(&ClApperTable, L"cycloneOctaves2", Obj.Clouds.cycloneOctaves2, FixedOutput, Prec);
        if (!CloudTable.Get().empty())
        {
            CloudIt->SubTable->Get().insert(CloudIt->SubTable->Get().end(),
                CloudTable.Get().begin(), CloudTable.Get().end());
        }
        if (!ClApperTable.Get().empty())
        {
            CloudAppIt->SubTable->Get().insert(CloudAppIt->SubTable->Get().end(),
                ClApperTable.Get().begin(), ClApperTable.Get().end());
        }
        if (Obj.Clouds.Layers.size() > 1)
        {
            for (int i = 1; i < Obj.Clouds.Layers.size(); ++i)
            {
                AddClouds(Obj.Clouds.Layers[i]);
            }
        }
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoAtmosphere", Obj.NoAtmosphere, 0, Prec);}
    if ((Fl & __Object_Manipulator::Atmosphere) && !Obj.NoAtmosphere)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Atmosphere";
        _SC SCSTable AtmosphereTable;
        __Add_Key_Value(&AtmosphereTable, L"Model", Obj.Atmosphere.Model, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Height", Obj.Atmosphere.Height / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Density", Obj.Atmosphere.Density, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Pressure", Obj.Atmosphere.Pressure / StandardAtm, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Adiabat", Obj.Atmosphere.Adiabat, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Greenhouse", Obj.Atmosphere.Greenhouse, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Bright", Obj.Atmosphere.Bright, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Opacity", Obj.Atmosphere.Opacity, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"SkyLight", Obj.Atmosphere.SkyLight, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Hue", Obj.Atmosphere.Hue, FixedOutput, Prec);
        __Add_Key_Value(&AtmosphereTable, L"Saturation", Obj.Atmosphere.Saturation, FixedOutput, Prec);
        if (!Obj.Atmosphere.Composition.empty())
        {
            __Add_Empty_Tag(&AtmosphereTable);
            AtmosphereTable.Get().back().Key = L"Composition";
            _SC SCSTable CompoTable;
            AddCompositeTable(&CompoTable, Obj.Atmosphere.Composition);
            AtmosphereTable.Get().back().SubTable = make_shared<decltype(CompoTable)>(CompoTable);
        }
        ContentTable.Get().back().SubTable = make_shared<decltype(AtmosphereTable)>(AtmosphereTable);
    }

    if ((Fl & __Object_Manipulator::Climate) && Obj.EnableClimate)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Climate";
        _SC SCSTable ClimateTable;
        __Add_Key_Value(&ClimateTable, L"GlobalWindSpeed", Obj.Climate.GlobalWindSpeed, FixedOutput, Prec);
        __Add_Key_Value(&ClimateTable, L"MinSurfaceTemp", Obj.Climate.MinSurfaceTemp, FixedOutput, Prec);
        __Add_Key_Value(&ClimateTable, L"MaxSurfaceTemp", Obj.Climate.MaxSurfaceTemp, FixedOutput, Prec);
        __Add_Key_Value(&ClimateTable, L"AtmoProfile", Obj.Climate.AtmoProfile, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(ClimateTable)>(ClimateTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoAurora", Obj.NoAurora, 0, Prec);}
    if ((Fl & __Object_Manipulator::Aurora) && !Obj.NoAurora)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Aurora";
        _SC SCSTable AuroraTable;
        __Add_Key_Value(&AuroraTable, L"Height", Obj.Aurora.Height / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthLat", Obj.Aurora.NorthLat, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthLon", Obj.Aurora.NorthLon, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthRadius", Obj.Aurora.NorthRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthWidth", Obj.Aurora.NorthWidth / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthRings", Obj.Aurora.NorthRings, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthBright", Obj.Aurora.NorthBright, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthFlashFreq", Obj.Aurora.NorthFlashFreq, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthMoveSpeed", Obj.Aurora.NorthMoveSpeed, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"NorthParticles", Obj.Aurora.NorthParticles, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthLat", Obj.Aurora.SouthLat, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthLon", Obj.Aurora.SouthLon, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthRadius", Obj.Aurora.SouthRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthWidth", Obj.Aurora.SouthWidth / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthRings", Obj.Aurora.SouthRings, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthBright", Obj.Aurora.SouthBright, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthFlashFreq", Obj.Aurora.SouthFlashFreq, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthMoveSpeed", Obj.Aurora.SouthMoveSpeed, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"SouthParticles", Obj.Aurora.SouthParticles, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"TopColor", Obj.Aurora.TopColor, FixedOutput, Prec);
        __Add_Key_Value(&AuroraTable, L"BottomColor", Obj.Aurora.BottomColor, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(AuroraTable)>(AuroraTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoRings", Obj.NoRings, 0, Prec);}
    if ((Fl & __Object_Manipulator::Rings) && Obj.Type != L"Star" && !Obj.NoRings)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Rings";
        _SC SCSTable RingsTable;
        __Add_Key_Value(&RingsTable, L"Texture", Obj.Rings.Texture, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"InnerRadius", Obj.Rings.InnerRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"OuterRadius", Obj.Rings.OuterRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"EdgeRadius", Obj.Rings.EdgeRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"MeanRadius", Obj.Rings.MeanRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"Thickness", Obj.Rings.Thickness, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"RocksMaxSize", Obj.Rings.RocksMaxSize, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"RocksSpacing", Obj.Rings.RocksSpacing, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"DustDrawDist", Obj.Rings.DustDrawDist, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"ChartRadius", Obj.Rings.ChartRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"RotationPeriod", Obj.Rings.RotationPeriod / 3600., FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"Brightness", Obj.Rings.Brightness, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"FrontBright", Obj.Rings.FrontBright, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"BackBright", Obj.Rings.BackBright, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"Density", Obj.Rings.Density, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"Opacity", Obj.Rings.Opacity, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"SelfShadow", Obj.Rings.SelfShadow, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"PlanetShadow", Obj.Rings.PlanetShadow, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"frequency", Obj.Rings.frequency, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"densityScale", Obj.Rings.densityScale, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"densityOffset", Obj.Rings.densityOffset, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"densityPower", Obj.Rings.densityPower, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"colorContrast", Obj.Rings.colorContrast, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"FrontColor", Obj.Rings.FrontColor, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"BackThickColor", Obj.Rings.BackThickColor, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"BackIceColor", Obj.Rings.BackIceColor, FixedOutput, Prec);
        __Add_Key_Value(&RingsTable, L"BackDustColor", Obj.Rings.BackDustColor, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(RingsTable)>(RingsTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoAccretionDisk", Obj.NoAccretionDisk, 0, Prec);}
    if ((Fl & __Object_Manipulator::AccDisk) && Obj.Type == L"Star" && !Obj.NoAccretionDisk)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"AccretionDisk";
        _SC SCSTable AccDiskTable;
        __Add_Key_Value(&AccDiskTable, L"InnerRadiusKm", Obj.AccretionDisk.InnerRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"OuterRadiusKm", Obj.AccretionDisk.OuterRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"InnerThicknessKm", Obj.AccretionDisk.InnerThickness / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"OuterThicknessKm", Obj.AccretionDisk.OuterThickness / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"ThicknessPow", Obj.AccretionDisk.ThicknessPow, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"DetailScaleR", Obj.AccretionDisk.DetailScaleR, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"DetailScaleV", Obj.AccretionDisk.DetailScaleV, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"OctaveDistortionX", Obj.AccretionDisk.OctaveDistortionX, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"OctaveDistortionY", Obj.AccretionDisk.OctaveDistortionY, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"OctaveDistortionZ", Obj.AccretionDisk.OctaveDistortionZ, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"OctaveScale", Obj.AccretionDisk.OctaveScale, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"DiskNoiseContrast", Obj.AccretionDisk.DiskNoiseContrast, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"DiskTempContrast", Obj.AccretionDisk.DiskTempContrast, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"DiskOuterSpin", Obj.AccretionDisk.DiskOuterSpin, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"TwistMagn", Obj.AccretionDisk.TwistMagn, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"AccretionRate", Obj.AccretionDisk.AccretionRate, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"Temperature", Obj.AccretionDisk.Temperature, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"Density", Obj.AccretionDisk.Density, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"Luminosity", Obj.AccretionDisk.Luminosity / SolarLum, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"LuminosityBol", Obj.AccretionDisk.LuminosityBol / SolarLumBol, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"Brightness", Obj.AccretionDisk.Brightness, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetLengthKm", Obj.AccretionDisk.JetLength / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetStartRadiusKm", Obj.AccretionDisk.JetStartRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetEndRadiusKm", Obj.AccretionDisk.JetEndRadius / 1000., FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetStartTemp", Obj.AccretionDisk.JetStartTemp, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetEndTemp", Obj.AccretionDisk.JetEndTemp, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetVelocity", Obj.AccretionDisk.JetVelocity, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetDistortion", Obj.AccretionDisk.JetDistortion, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetThickness", Obj.AccretionDisk.JetThickness, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetDensity", Obj.AccretionDisk.JetDensity, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"JetBrightness", Obj.AccretionDisk.JetBrightness, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"LightingBright", Obj.AccretionDisk.LightingBright, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"ShadowContrast", Obj.AccretionDisk.ShadowContrast, FixedOutput, Prec);
        __Add_Key_Value(&AccDiskTable, L"ShadowLength", Obj.AccretionDisk.ShadowLength, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(AccDiskTable)>(AccDiskTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoCorona", Obj.NoCorona, 0, Prec);}
    if ((Fl & __Object_Manipulator::Corona) && Obj.Type == L"Star" && !Obj.NoCorona)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"Corona";
        _SC SCSTable CoronaTable;
        __Add_Key_Value(&CoronaTable, L"Radius", Obj.Corona.Radius, FixedOutput, Prec);
        __Add_Key_Value(&CoronaTable, L"Period", Obj.Corona.Period, FixedOutput, Prec);
        __Add_Key_Value(&CoronaTable, L"Brightness", Obj.Corona.Brightness, FixedOutput, Prec);
        __Add_Key_Value(&CoronaTable, L"RayDensity", Obj.Corona.RayDensity, FixedOutput, Prec);
        __Add_Key_Value(&CoronaTable, L"RayCurv", Obj.Corona.RayCurv, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(CoronaTable)>(CoronaTable);
    }

    if (Booleans) {__Add_Key_Value(&ContentTable, L"NoCometTail", Obj.NoCometTail, 0, Prec);}
    if ((Fl & __Object_Manipulator::CometTail) && !Obj.NoCometTail)
    {
        __Add_Empty_Tag(&ContentTable);
        ContentTable.Get().back().Key = L"CometTail";
        _SC SCSTable CometTailTable;
        __Add_Key_Value(&CometTailTable, L"MaxLength", Obj.CometTail.MaxLength, FixedOutput, Prec);
        __Add_Key_Value(&CometTailTable, L"GasToDust", Obj.CometTail.GasToDust, FixedOutput, Prec);
        __Add_Key_Value(&CometTailTable, L"Particles", Obj.CometTail.Particles, FixedOutput, Prec);
        __Add_Key_Value(&CometTailTable, L"GasBright", Obj.CometTail.GasBright, FixedOutput, Prec);
        __Add_Key_Value(&CometTailTable, L"DustBright", Obj.CometTail.DustBright, FixedOutput, Prec);
        __Add_Key_Value(&CometTailTable, L"GasColor", Obj.CometTail.GasColor, FixedOutput, Prec);
        __Add_Key_Value(&CometTailTable, L"DustColor", Obj.CometTail.DustColor, FixedOutput, Prec);
        ContentTable.Get().back().SubTable = make_shared<decltype(CometTailTable)>(CometTailTable);
    }

    MainTable.Get().front().SubTable = make_shared<decltype(ContentTable)>(ContentTable);
    return MainTable;
}

#endif

// Object Functions
_ASOBJ_BEGIN

float64 Aphelion(Object Obj)
{
    return SemiMajorAxis(Obj) * 2. - Perihelion(Obj);
}

float64 Perihelion(Object Obj)
{
    return Obj.Orbit.PericenterDist;
}

float64 SemiMajorAxis(Object Obj)
{
    float64 Ecc = IS_NO_DATA_DBL(Eccentricity(Obj)) ? 0 : Eccentricity(Obj);
    return Perihelion(Obj) / (1. - Ecc);
}

float64 MeanMotion(Object Obj)
{
    return 360. / SiderealOrbitalPeriod(Obj); // Degrees per second
}

float64 Eccentricity(Object Obj)
{
    return Obj.Orbit.Eccentricity;
}

float64 SiderealOrbitalPeriod(Object Obj)
{
    return Obj.Orbit.Period;
}

// float64 AverageOrbitalSpeed(Object Obj)
// {

// }

float64 MeanAnomaly(Object Obj)
{
    return Obj.Orbit.MeanAnomaly;
}

float64 MeanLongitude(Object Obj)
{
    float64 _AscendingNode = LongitudeOfAscendingNode(Obj);
    float64 _ArgOfPericenter = IS_NO_DATA_DBL(ArgumentOfPerihelion(Obj)) ? 0 : ArgumentOfPerihelion(Obj);
    float64 _MeanAnomaly = IS_NO_DATA_DBL(MeanAnomaly(Obj)) ? 0 : MeanAnomaly(Obj);
    return _AscendingNode + _ArgOfPericenter + _MeanAnomaly;
}

float64 Inclination(Object Obj)
{
    return Obj.Orbit.Inclination;
}

float64 LongitudeOfAscendingNode(Object Obj)
{
    return Obj.Orbit.AscendingNode;
}

CSEDateTime TimeOfPerihelion(Object Obj)
{
    return JDToDateTime(Obj.Orbit.Epoch);
}

float64 ArgumentOfPerihelion(Object Obj)
{
    return Obj.Orbit.ArgOfPericenter;
}

float64 LongitudeOfPerihelion(Object Obj)
{
    float64 _AscendingNode = LongitudeOfAscendingNode(Obj);
    float64 _ArgOfPericenter = IS_NO_DATA_DBL(ArgumentOfPerihelion(Obj)) ? 0 : ArgumentOfPerihelion(Obj);
    return _AscendingNode + _ArgOfPericenter;
}



float64 MeanRadius(Object Obj)
{
    return cbrt(Obj.Dimensions.x * Obj.Dimensions.y * Obj.Dimensions.z) / 2.; // Geometric mean
}

float64 EquatorialRadius(Object Obj)
{
    return max(Obj.Dimensions.x, Obj.Dimensions.z) / 2.;
}

float64 PolarRadius(Object Obj)
{
    return Obj.Dimensions.y / 2.;
}

vec3 Flattening(Object Obj)
{
    auto Radius = vec3(Obj.Dimensions / 2.);
    auto MaxRad = cse::max({Radius.x, Radius.y, Radius.z});
    return (MaxRad - Radius) / MaxRad;
}

float64 EquatorialCircumference(Object Obj, const SciCxx::IntegralFunction& IFunc)
{
    float64 a = max(Obj.Dimensions.x, Obj.Dimensions.z) / 2.;
    float64 b = min(Obj.Dimensions.x, Obj.Dimensions.z) / 2.;
    if (a == b) {return 2. * CSE_PI * a;}

    float64 e2 = 1. - pow(b / a, 2);
    auto E = [e2](float64 tet)
    {
        return sqrt(1. - e2 * pow(sin(Angle::FromRadians(tet)), 2));
    };
    return 4. * a * IFunc(E, 0, CSE_PI_D2);
}

float64 MeridionalCircumference(Object Obj, const SciCxx::IntegralFunction& IFunc)
{
    float64 a = EquatorialRadius(Obj);
    float64 b = PolarRadius(Obj);
    if (a == b) {return 2. * CSE_PI * a;}

    float64 e2 = 1. - pow(b / a, 2);
    auto E = [e2](float64 tet)
    {
        return sqrt(1. - e2 * pow(sin(Angle::FromRadians(tet)), 2));
    };
    return 4. * a * IFunc(E, 0, CSE_PI_D2);
}

float64 SurfaceArea(Object Obj, const SciCxx::IntegralFunction& IFunc)
{
    std::array<float64, 3> Radius = Obj.Dimensions / 2.;
    std::sort(Radius.begin(), Radius.end());
    float64 a = Radius[2], b = Radius[1], c = Radius[0];
    if (a == b && b == c) {return 4. * CSE_PI * a * a;}

    auto F = [&IFunc](float64 phi, float64 k2)
    {
        auto EllipticalIntegral1st = [k2](float64 tet)
        {
            return inversesqrt(1. - k2 * pow(sin(Angle::FromRadians(tet)), 2));
        };
        return IFunc(EllipticalIntegral1st, 0, phi);
    };

    auto E = [&IFunc](float64 phi, float64 k2)
    {
        auto EllipticalIntegral2nd = [k2](float64 tet)
        {
            return sqrt(1. - k2 * pow(sin(Angle::FromRadians(tet)), 2));
        };
        return IFunc(EllipticalIntegral2nd, 0, phi);
    };

    Angle phi = arccos(c / a);
    float64 k2 = (a * a * (b * b - c * c)) / (b * b * (a * a - c * c));
    return 2. * CSE_PI * c * c + (2. * CSE_PI * a * b) / sin(phi) *
        (E(phi.ToRadians(), k2) * pow(sin(phi), 2) + F(phi.ToRadians(), k2) * pow(cos(phi), 2));
}

float64 Volume(Object Obj)
{
    vec3 Radius = Obj.Dimensions / 2.;
    return (4. / 3.) * CSE_PI * Radius.x * Radius.y * Radius.z;
}

float64 Mass(Object Obj)
{
    return Obj.Mass;
}

float64 MeanDensity(Object Obj)
{
    return Mass(Obj) / Volume(Obj);
}

float64 SurfaceGravity(Object Obj)
{
    return GravConstant * Mass(Obj) / pow(MeanRadius(Obj), 2);
}

float64 EscapeVelocity(Object Obj)
{
    return sqrt(2. * GravConstant * Mass(Obj) / EquatorialRadius(Obj));
}

float64 SynodicRotationPeriod(Object Obj)
{
    if (Obj.Rotation.TidalLocked || Obj.Rotation.RotationPeriod == Obj.Orbit.Period)
    {
        return __Float64::FromBytes(POS_INF_DOUBLE);
    }

    float64 RotationPeriod, OrbitalPeriod;
    if (Quadrant(Obj.Rotation.Obliquity) >= 2 &&
        Quadrant(Obj.Rotation.Obliquity) < 6 &&
        Obj.Rotation.RotationPeriod > 0)
    {
        RotationPeriod = -Obj.Rotation.RotationPeriod;
    }
    else {RotationPeriod = Obj.Rotation.RotationPeriod;}

    if (Quadrant(Obj.Orbit.Inclination) >= 2 &&
        Quadrant(Obj.Orbit.Inclination) < 6 &&
        Obj.Orbit.Period > 0)
    {
        OrbitalPeriod = -Obj.Orbit.Period;
    }
    else {OrbitalPeriod = Obj.Orbit.Period;}

    return (RotationPeriod * OrbitalPeriod) / (OrbitalPeriod - RotationPeriod);
}

float64 SiderealRotationPeriod(Object Obj)
{
    if (Obj.Rotation.TidalLocked) {return Obj.Orbit.Period;}
    return Obj.Rotation.RotationPeriod;
}

float64 EquatorialRotationVelocity(Object Obj)
{
    return EquatorialCircumference(Obj) / SiderealRotationPeriod(Obj);
}

float64 AxialTilt(Object Obj)
{
    return Obj.Rotation.Obliquity;
}

float64 GeometricAlbedo(Object Obj)
{
    return Obj.AlbedoGeom;
}

float64 BondAlbedo(Object Obj)
{
    return Obj.AlbedoBond;
}

float64 EffectiveTemperature(Object Obj)
{
    return Obj.Temperature;
}

float64 EquilibriumTemperature(Object Parent, Object Companion, float64 Separation)
{
    return yroot((Parent.LumBol / (4. * CSE_PI * Separation * Separation)) *
        ((1. - Companion.AlbedoBond) / (StBConstant * (Companion.Rotation.TidalLocked ? 2 : 4))), 4);
}

float64 AbsoluteMagnitude(Object Obj)
{
    return Obj.AbsMagn;
}

_ASOBJ_END

_CSE_END
