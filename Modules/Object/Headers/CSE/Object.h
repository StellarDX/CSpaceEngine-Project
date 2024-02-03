// Astronomical object defination for CSE
// A huge structure

#pragma once

#ifndef __SEOBJECT__
#define __SEOBJECT__

#include <CSE/CSEBase/CSEBase.h>
#include <CSE/CSEBase/DateTime.h>
#include <CSE/CSEBase/GLTypes.h>
#include <vector>
#include <map>

#if __has_include(<CSE/SCStream.h>)
#include <CSE/SCStream.h>
#endif

#if defined _MSC_VER
#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
#if defined _STL_DISABLE_CLANG_WARNINGS
_STL_DISABLE_CLANG_WARNINGS
#endif
#pragma push_macro("new")
#undef new
#endif

_CSE_BEGIN

#define _ASOBJ_BEGIN namespace AsObject {
#define _ASOBJ_END   }
#define _ASOBJ       AsObject::

using CompositionType = std::map<ustring, float64>;

_ASOBJ_BEGIN

struct OrbitState
{
    ustring   RefPlane       = _NoDataStr;
    float64   GravParam      = _NoDataDbl;
    float64   Time           = _NoDataDbl;
    vec3      Position       = vec3(_NoDataDbl);
    vec3      Velocity       = vec3(_NoDataDbl);
};

struct SurfaceTextureParams
{
    ustring   DiffMap        = _NoDataStr;       // Surface map path
    ustring   DiffMapAlpha   = _NoDataStr;       //
    ustring   BumpMap        = _NoDataStr;       // Elevation map path
    float64   BumpHeight     = _NoDataDbl;       // height scale of the Bump map in metres
    float64   BumpOffset     = _NoDataDbl;       // negative offset of the landscape in metres.
    ustring   GlowMap        = _NoDataStr;       // Glow map path
    ustring   GlowMode       = _NoDataStr;       //
    vec3      GlowColor      = vec3(_NoDataDbl); //
    float64   GlowBright     = _NoDataDbl;       //
    ustring   SpecMap        = _NoDataStr;       // Specular map path
    bool      FlipMap        = false;            // Flip the map by 180°
};

struct HapkeParams
{
    float64   Gamma          = _NoDataDbl;
    float64   Hapke          = _NoDataDbl;
    float64   SpotBright     = _NoDataDbl;
    float64   SpotWidth      = _NoDataDbl;
    float64   SpotBrightCB   = _NoDataDbl;
    float64   SpotWidthCB    = _NoDataDbl;
    float64   TempMapMaxTemp = _NoDataDbl;
    float64   DayAmbient     = _NoDataDbl;
    float64   RingsWinter    = _NoDataDbl;
    vec3      ModulateColor  = vec3(_NoDataDbl);
    float64   ModulateBright = _NoDataDbl;
};

_ASOBJ_END

////////////////////////////////////////////////////////////////////////////////
//                          SpaceEngine Object Class                          //
////////////////////////////////////////////////////////////////////////////////

struct Object : public SEObject
{
    /*--------------------------------------------------------------*\
    |                           Basic Info                           |
    \*--------------------------------------------------------------*/

    ustring         Type                 = _NoDataStr;       // Object type
    ustringlist     Name                 = {};               // Object Names
    _TIME CSEDate   DateUpdated          = _TIME CSEDate();  // Last update date
    ustring         DiscMethod           = _NoDataStr;       // Discovery method
    _TIME CSEDate   DiscDate             = _TIME CSEDate();  // Discovery date
    ustring         ParentBody           = _NoDataStr;       // Parent body
    ustring         Class                = _NoDataStr;       // Classification
    ustring         AsterType            = _NoDataStr;       // Only for asteroids
    ustring         SpecClass            = _NoDataStr;       // Only for Stars

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                       Physical properties                      |
    \*--------------------------------------------------------------*/

    float64         Mass                 = _NoDataDbl;       // Mass in Kg (M)
    vec3            Dimensions           = vec3(_NoDataDbl); // Dimensions in Metres
    float64         InertiaMoment        = _NoDataDbl;       // Moment of inertia factor (I)
    float64         AlbedoBond           = _NoDataDbl;       // Bond albedo
    float64         AlbedoGeom           = _NoDataDbl;       // Geometric albedo
    float64         EndogenousHeating    = _NoDataDbl;       // Hack: make the object glowing hot despite on actual heating,
    float64         ThermalLuminosity    = _NoDataDbl;       // and make it not emit any thermal light
    float64         ThermalLuminosityBol = _NoDataDbl;       //
    float64         Temperature          = _NoDataDbl;       // Effective temperature
    float64         Luminosity           = _NoDataDbl;       // Visual luminosity in Watts (Lv)
    float64         LumBol               = _NoDataDbl;       // Bolometric luminosity in Watts (Lbol)
    float64         FeH                  = _NoDataDbl;       // Metallicity
    float64         CtoO                 = _NoDataDbl;       //
    float64         Age                  = _NoDataDbl;       // Age in years
    float64         KerrSpin             = _NoDataDbl;       // Angular momentum (J)
    float64         KerrCharge           = _NoDataDbl;       // Electric charge (Q)

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                       Optical properties                       |
    \*--------------------------------------------------------------*/

    vec3            Color                = vec3(_NoDataDbl);
    float64         Brightness           = _NoDataDbl;
    float64         BrightnessReal       = _NoDataDbl;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                            Rotation                            |
    \*--------------------------------------------------------------*/

    ustring         RotationModel        = _NoDataStr;

    struct SimpleRotationModel // Simply uniform rotation parameters, used only if RotationModel is missing
    {
        float64     RotationEpoch     = _NoDataDbl; // J2000
        float64     Obliquity         = _NoDataDbl; // degrees
        float64     EqAscendNode      = _NoDataDbl; // degrees
        float64     RotationOffset    = _NoDataDbl; // degrees
        float64     RotationPeriod    = _NoDataDbl; // seconds
        float64     Precession        = _NoDataDbl; // seconds
        bool        TidalLocked       = false;      // Tidal locked
    }Rotation;

    struct IAURotationModel
    {
        float64     Epoch             = _NoDataDbl; // J2000
        float64     PoleRA            = _NoDataDbl; // degrees
        float64     PoleRARate        = _NoDataDbl; // degrees/century
        float64     PoleDec           = _NoDataDbl; // degrees
        float64     PoleDecRate       = _NoDataDbl; // degrees/century
        float64     PrimeMeridian     = _NoDataDbl; // degrees
        float64     RotationRate      = _NoDataDbl; // degrees/day
        float64     RotationAccel     = _NoDataDbl; // degrees/century^2
        bool        UsingSecular      = false;      // Using century as time unit instrad of day in PeriodicTerms

        /**
         * Format of PeriodicTerms table:
         *
         *     (PoleRA amp, PoleDec amp, PrimeMer amp, phase, freq, freq_rate)
         *
         * And the final value can be calculated below:
         *
         *     @return FinalRA = PoleRA + PoleRARate * Time + SUM(PoleRAAmp[i] * sin(Phase[i] + Freq[i] * Time + FreqRate * pow(Time, 2)))
         *     @return FinalDec = PoleDec + PoleDecRate * Time + SUM(PoleDecAmp[i] * cos(Phase[i] + Freq[i] * Time + FreqRate * pow(Time, 2)))
         *     @return FinalRotation = PrimeMeridian + RotationRate * Time + RotationAccel * pow(Time, 2) + SUM(PrimeMerAmp[i] * sin(Phase[i] + Freq[i] * Time + FreqRate * pow(Time, 2)))
         *
         * References:
         * @link https://github.com/Bill-Gray/find_orb/blob/master/cospar.txt
         * @link https://github.com/cosmonium/cosmonium
         */
        std::vector<std::array<float64, 6>> PeriodicTerms;
    }RotationIAU;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                              Orbit                             |
    \*--------------------------------------------------------------*/

    bool            FixedPos             = false;
    vec3            Position             = vec3(_NoDataDbl);
    struct OrbitParams
    {
        // Binary
        bool        Binary            = false;      // Enable when orbit tag is "BinaryOrbit"
        float64     Separation        = _NoDataDbl; // Separation between this object and parent body in meters
        float64     PositionAngle     = _NoDataDbl; // Position Angle

        // Non-Binary
        ustring     AnalyticModel     = _NoDataStr; //
        ustring     RefPlane          = _NoDataStr; // Reference system
        float64     Epoch             = _NoDataDbl; // Epoch in JD
        float64     Period            = _NoDataDbl; // Period in seconds (T)
        float64     PericenterDist    = _NoDataDbl; // Pericenter distance in metres
        float64     GravParam         = _NoDataDbl; // Gravity parameter
        float64     Eccentricity      = _NoDataDbl; // Eccentricity
        float64     Inclination       = _NoDataDbl; // Inclination in degrees
        float64     AscendingNode     = _NoDataDbl; // Logitude on ascending node in degrees
        float64     AscNodePreces     = _NoDataDbl; // Ascending node precession period in seconds
        float64     ArgOfPericenter   = _NoDataDbl; // Argument of pericenter in degrees
        float64     ArgOfPeriPreces   = _NoDataDbl; // Argument of pericenter precession period in seconds
        float64     MeanAnomaly       = _NoDataDbl; // Mean anomaly
    }Orbit;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                              Life                              |
    \*--------------------------------------------------------------*/

    bool            NoLife               = false;
    struct LifeParams
    {
        ustring     Class             = _NoDataStr;
        ustring     Type              = _NoDataStr;
        ustringlist Biome             = {};
        bool        Panspermia        = false;
    }Life[2];

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                            Interior                            |
    \*--------------------------------------------------------------*/

    CompositionType Interior        = {};                    // mass fraction, values in percent

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                             Surface                            |
    \*--------------------------------------------------------------*/

    bool            EnableSurface        = false;
    struct LandscapeParams : public _ASOBJ SurfaceTextureParams, _ASOBJ HapkeParams
    {
        ustring     Preset            = _NoDataStr;
        float64     SurfStyle         = _NoDataDbl;
        vec3        Randomize         = vec3(_NoDataDbl);
        float64     colorDistMagn     = _NoDataDbl;
        float64     colorDistFreq     = _NoDataDbl;
        float64     detailScale       = _NoDataDbl;
        float64     drivenDarkening   = _NoDataDbl;
        float64     seaLevel          = _NoDataDbl;
        float64     snowLevel         = _NoDataDbl;
        float64     tropicLatitude    = _NoDataDbl;
        float64     icecapLatitude    = _NoDataDbl;
        float64     icecapHeight      = _NoDataDbl;
        float64     climatePole       = _NoDataDbl;
        float64     climateTropic     = _NoDataDbl;
        float64     climateEquator    = _NoDataDbl;
        float64     climateSteppeMin  = _NoDataDbl;
        float64     climateSteppeMax  = _NoDataDbl;
        float64     climateForestMin  = _NoDataDbl;
        float64     climateForestMax  = _NoDataDbl;
        float64     climateGrassMin   = _NoDataDbl;
        float64     climateGrassMax   = _NoDataDbl;
        float64     humidity          = _NoDataDbl;
        float64     heightTempGrad    = _NoDataDbl;
        float64     beachWidth        = _NoDataDbl;
        float64     tropicWidth       = _NoDataDbl;
        float64     mainFreq          = _NoDataDbl;
        float64     venusFreq         = _NoDataDbl;
        float64     venusMagn         = _NoDataDbl;
        float64     mareFreq          = _NoDataDbl;
        float64     mareDensity       = _NoDataDbl;
        float64     terraceProb       = _NoDataDbl;
        float64     erosion           = _NoDataDbl;
        float64     montesMagn        = _NoDataDbl;
        float64     montesFreq        = _NoDataDbl;
        float64     montesSpiky       = _NoDataDbl;
        float64     montesFraction    = _NoDataDbl;
        float64     dunesMagn         = _NoDataDbl;
        float64     dunesFreq         = _NoDataDbl;
        float64     dunesFraction     = _NoDataDbl;
        float64     hillsMagn         = _NoDataDbl;
        float64     hillsFreq         = _NoDataDbl;
        float64     hillsFraction     = _NoDataDbl;
        float64     hills2Fraction    = _NoDataDbl;
        float64     riversMagn        = _NoDataDbl;
        float64     riversFreq        = _NoDataDbl;
        float64     riversSin         = _NoDataDbl;
        float64     riftsMagn         = _NoDataDbl;
        float64     riftsFreq         = _NoDataDbl;
        float64     riftsSin          = _NoDataDbl;
        float64     eqridgeMagn       = _NoDataDbl;
        float64     eqridgeWidth      = _NoDataDbl;
        float64     eqridgeModMagn    = _NoDataDbl;
        float64     eqridgeModFreq    = _NoDataDbl;
        float64     canyonsMagn       = _NoDataDbl;
        float64     canyonsFreq       = _NoDataDbl;
        float64     canyonsFraction   = _NoDataDbl;
        float64     cracksMagn        = _NoDataDbl;
        float64     cracksFreq        = _NoDataDbl;
        uint64      cracksOctaves     = _NoDataInt;
        float64     craterMagn        = _NoDataDbl;
        float64     craterFreq        = _NoDataDbl;
        float64     craterDensity     = _NoDataDbl;
        uint64      craterOctaves     = _NoDataInt;
        uint64      craterRayedFactor = _NoDataInt;
        float64     volcanoMagn       = _NoDataDbl;
        float64     volcanoFreq       = _NoDataDbl;
        float64     volcanoDensity    = _NoDataDbl;
        uint64      volcanoOctaves    = _NoDataInt;
        float64     volcanoActivity   = _NoDataDbl;
        float64     volcanoFlows      = _NoDataDbl;
        float64     volcanoRadius     = _NoDataDbl;
        float64     volcanoTemp       = _NoDataDbl;
        float64     lavaCoverTidal    = _NoDataDbl;
        float64     lavaCoverSun      = _NoDataDbl;
        float64     lavaCoverYoung    = _NoDataDbl;

        //float64     stripeZones       = _NoDataDbl;
        //float64     stripeFluct       = _NoDataDbl;
        //float64     stripeTwist       = _NoDataDbl;
        //float64     cycloneMagn       = _NoDataDbl;
        //float64     cycloneFreq       = _NoDataDbl;
        //float64     cycloneDensity    = _NoDataDbl;
        //uint64      cycloneOctaves    = _NoDataInt;
        //float64     cycloneMagn2      = _NoDataDbl;
        //float64     cycloneFreq2      = _NoDataDbl;
        //float64     cycloneDensity2   = _NoDataDbl;
        //uint64      cycloneOctaves2   = _NoDataInt;

        float64     SpecBrightWater   = _NoDataDbl;
        float64     SpecBrightIce     = _NoDataDbl;
        float64     RoughnessWater    = _NoDataDbl;
        float64     RoughnessIce      = _NoDataDbl;
        float64     SpecularScale     = _NoDataDbl;
        float64     RoughnessBias     = _NoDataDbl;
        float64     RoughnessScale    = _NoDataDbl;
    }Surface;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                              Ocean                             |
    \*--------------------------------------------------------------*/

    bool            NoOcean              = false;
    struct OceanParams : public _ASOBJ HapkeParams
    {
        float64     Height            = _NoDataDbl;

        // Compositions, value in percent
        CompositionType Composition;
    }Ocean;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                             Clouds                             |
    \*--------------------------------------------------------------*/

    bool            NoClouds             = false;
    struct CloudParams
    {
        bool        TidalLocked       = false;

        struct CloudLayerParam : public _ASOBJ SurfaceTextureParams, _ASOBJ HapkeParams
        {
            float64 Height            = _NoDataDbl; // Km
            float64 Velocity          = _NoDataDbl; // Km/h
            float64 RotationOffset    = _NoDataDbl;
            float64 SubsurfScatBright = _NoDataDbl;
            float64 SubsurfScatPower  = _NoDataDbl;
            float64 Opacity           = _NoDataDbl;
        };
        std::vector<CloudLayerParam> Layers;

        float64     Coverage          = _NoDataDbl;
        float64     mainFreq          = _NoDataDbl;
        uint64      mainOctaves       = _NoDataDbl;
        float64     stripeZones       = _NoDataDbl;
        float64     stripeFluct       = _NoDataDbl;
        float64     stripeTwist       = _NoDataDbl;
        float64     cycloneMagn       = _NoDataDbl;
        float64     cycloneFreq       = _NoDataDbl;
        float64     cycloneDensity    = _NoDataDbl;
        uint64      cycloneOctaves    = _NoDataInt;
        float64     cycloneMagn2      = _NoDataDbl;
        float64     cycloneFreq2      = _NoDataDbl;
        float64     cycloneDensity2   = _NoDataDbl;
        uint64      cycloneOctaves2   = _NoDataInt;
    }Clouds;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                           Atmosphere                           |
    \*--------------------------------------------------------------*/

    bool            NoAtmosphere         = false;
    struct AtmoParams
    {
        ustring     Model             = _NoDataStr;
        float64     Height            = _NoDataDbl;
        float64     Density           = _NoDataDbl;
        float64     Pressure          = _NoDataDbl;
        float64     Adiabat           = _NoDataDbl;
        float64     Greenhouse        = _NoDataDbl;
        float64     Bright            = _NoDataDbl;
        float64     Opacity           = _NoDataDbl;
        float64     SkyLight          = _NoDataDbl;
        float64     Hue               = _NoDataDbl;
        float64     Saturation        = _NoDataDbl;

        // Compositions, value in percent
        CompositionType Composition;
    }Atmosphere;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                             Climate                            |
    \*--------------------------------------------------------------*/

    bool            EnableClimate        = false;
    struct ClimateParams
    {
        float64     GlobalWindSpeed   = _NoDataDbl;
        float64     MinSurfaceTemp    = _NoDataDbl;
        float64     MaxSurfaceTemp    = _NoDataDbl;
        ustring     AtmoProfile       = _NoDataStr;
    }Climate;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                             Aurora                             |
    \*--------------------------------------------------------------*/

    bool            NoAurora             = false;
    struct AuroraParams
    {
        float64     Height            = _NoDataDbl;
        float64     NorthLat          = _NoDataDbl;
        float64     NorthLon          = _NoDataDbl;
        float64     NorthRadius       = _NoDataDbl;
        float64     NorthWidth        = _NoDataDbl;
        uint64      NorthRings        = _NoDataInt;
        float64     NorthBright       = _NoDataDbl;
        float64     NorthFlashFreq    = _NoDataDbl;
        float64     NorthMoveSpeed    = _NoDataDbl;
        uint64      NorthParticles    = _NoDataInt;
        float64     SouthLat          = _NoDataDbl;
        float64     SouthLon          = _NoDataDbl;
        float64     SouthRadius       = _NoDataDbl;
        float64     SouthWidth        = _NoDataDbl;
        uint64      SouthRings        = _NoDataInt;
        float64     SouthBright       = _NoDataDbl;
        float64     SouthFlashFreq    = _NoDataDbl;
        float64     SouthMoveSpeed    = _NoDataDbl;
        uint64      SouthParticles    = _NoDataInt;
        vec3        TopColor          = vec3(_NoDataDbl);
        vec3        BottomColor       = vec3(_NoDataDbl);
    }Aurora;

    // ------------------------------------------------------------ //


    /*--------------------------------------------------------------*\
    |                              Rings                             |
    \*--------------------------------------------------------------*/

    bool            NoRings              = false;
    struct RingsParams : public _ASOBJ HapkeParams
    {
        ustring     Texture           = _NoDataStr;
        float64     InnerRadius       = _NoDataDbl;	// meters
        float64     OuterRadius       = _NoDataDbl;	// meters
        float64     EdgeRadius        = _NoDataDbl;	// outer radius of the dense part of rings  (skipping the E ring)
        float64     MeanRadius        = _NoDataDbl;	// radius of the most denser part for the "rings winter" effect
        float64     Thickness         = _NoDataDbl;	// meters
        float64     RocksMaxSize      = _NoDataDbl;	// meters
        float64     RocksSpacing      = _NoDataDbl;
        float64     DustDrawDist      = _NoDataDbl;
        float64     ChartRadius       = _NoDataDbl;	// outer radius of rings for the chart mode (skipping the E ring)
        float64     RotationPeriod    = _NoDataDbl;
        float64     Brightness        = _NoDataDbl;
        float64     FrontBright       = _NoDataDbl;
        float64     BackBright        = _NoDataDbl;
        float64     Density           = _NoDataDbl;
        float64     Opacity           = _NoDataDbl;
        float64     SelfShadow        = _NoDataDbl;
        float64     PlanetShadow      = _NoDataDbl;
        float64     frequency         = _NoDataDbl;
        float64     densityScale      = _NoDataDbl;
        float64     densityOffset     = _NoDataDbl;
        float64     densityPower      = _NoDataDbl;
        float64     colorContrast     = _NoDataDbl;
        vec3        FrontColor        = vec3(_NoDataDbl);
        vec3        BackThickColor    = vec3(_NoDataDbl);
        vec3        BackIceColor      = vec3(_NoDataDbl);
        vec3        BackDustColor     = vec3(_NoDataDbl);
    }Rings;

    /*--------------------------------------------------------------*\
    |                         Accretion Disk                         |
    \*--------------------------------------------------------------*/

    bool            NoAccretionDisk      = false;
    struct DiskParams
    {
        float64     InnerRadius       = _NoDataDbl;
        float64     OuterRadius       = _NoDataDbl;
        float64     InnerThickness    = _NoDataDbl;
        float64     OuterThickness    = _NoDataDbl;
        float64     ThicknessPow      = _NoDataDbl;
        float64     DetailScaleR      = _NoDataDbl;
        float64     DetailScaleV      = _NoDataDbl;
        float64     OctaveDistortionX = _NoDataDbl;
        float64     OctaveDistortionY = _NoDataDbl;
        float64     OctaveDistortionZ = _NoDataDbl;
        float64     OctaveScale       = _NoDataDbl;
        float64     DiskNoiseContrast = _NoDataDbl;
        float64     DiskTempContrast  = _NoDataDbl;
        float64     DiskOuterSpin     = _NoDataDbl;
        float64     TwistMagn         = _NoDataDbl;
        float64     AccretionRate     = _NoDataDbl;
        float64     Temperature       = _NoDataDbl;
        float64     Density           = _NoDataDbl;
        float64     Luminosity        = _NoDataDbl;
        float64     LuminosityBol     = _NoDataDbl;
        float64     Brightness        = _NoDataDbl;

        float64     JetLength         = _NoDataDbl;
        float64     JetStartRadius    = _NoDataDbl;
        float64     JetEndRadius      = _NoDataDbl;
        float64     JetStartTemp      = _NoDataDbl;
        float64     JetEndTemp        = _NoDataDbl;
        float64     JetVelocity       = _NoDataDbl;
        float64     JetDistortion     = _NoDataDbl;
        float64     JetThickness      = _NoDataDbl;
        float64     JetDensity        = _NoDataDbl;
        float64     JetBrightness     = _NoDataDbl;

        float64     LightingBright    = _NoDataDbl;
        float64     ShadowContrast    = _NoDataDbl;
        float64     ShadowLength      = _NoDataDbl;
    }AccretionDisk;

    /*--------------------------------------------------------------*\
    |                             Corona                             |
    \*--------------------------------------------------------------*/

    bool            NoCorona             = false;
    struct CoronaParams
    {
        float64     Radius            = _NoDataDbl;
        float64     Period            = _NoDataDbl;
        float64     Brightness        = _NoDataDbl;
        float64     RayDensity        = _NoDataDbl;
        float64     RayCurv           = _NoDataDbl;
    }Corona;

    /*--------------------------------------------------------------*\
    |                           Comet Tail                           |
    \*--------------------------------------------------------------*/

    bool            NoCometTail         = false;
    struct CometTailParams
    {
        float64     MaxLength         = _NoDataDbl;
        float64     GasToDust         = _NoDataDbl;
        uint64      Particles         = _NoDataInt;
        float64     GasBright         = _NoDataDbl;
        float64     DustBright        = _NoDataDbl;
        vec3        GasColor          = vec3(_NoDataDbl);
        vec3        DustColor         = vec3(_NoDataDbl);
    }CometTail;
};

// IOSCStream Compatibility
#ifdef _CSE_SCSTREAM
#ifdef GetObject
#undef GetObject
#endif
Object GetObjectFromKeyValue(_SC SCSTable::SCKeyValue KeyValue);
template<> Object GetObject(_SC SharedTablePointer Table, ustring Name);
template<> _SC SCSTable MakeTable(Object Object);
#endif

_CSE_END

#if defined _MSC_VER
#pragma pop_macro("new")
#if defined _STL_RESTORE_CLANG_WARNINGS
    _STL_RESTORE_CLANG_WARNINGS
#endif
#pragma warning(pop)
#pragma pack(pop)
#endif

#endif