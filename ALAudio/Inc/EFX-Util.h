/*******************************************************************\
*                                                                   *
*  EFX-UTIL.H - EFX Utilities functions and Reverb Presets          *
*                                                                   *
*               File revision 1.0                                   *
*                                                                   *
\*******************************************************************/

#ifndef EFX_UTIL_H_INCLUDED
#define EFX_UTIL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#pragma pack(push, OBJECT_ALIGNMENT)

#ifndef EAXVECTOR_DEFINED
#define EAXVECTOR_DEFINED
typedef struct _EAXVECTOR {
	float x;
	float y;
	float z;
} EAXVECTOR;
#endif

#ifndef EAXREVERBPROPERTIES_DEFINED
#define EAXREVERBPROPERTIES_DEFINED
typedef struct _EAXREVERBPROPERTIES
{
    unsigned long ulEnvironment;
    float flEnvironmentSize;
    float flEnvironmentDiffusion;
    long lRoom;
    long lRoomHF;
    long lRoomLF;
    float flDecayTime;
    float flDecayHFRatio;
    float flDecayLFRatio;
    long lReflections;
    float flReflectionsDelay;
    EAXVECTOR vReflectionsPan;
    long lReverb;
    float flReverbDelay;
    EAXVECTOR vReverbPan;
    float flEchoTime;
    float flEchoDepth;
    float flModulationTime;
    float flModulationDepth;
    float flAirAbsorptionHF;
    float flHFReference;
    float flLFReference;
    float flRoomRolloffFactor;
    unsigned long ulFlags;
} EAXREVERBPROPERTIES, *LPEAXREVERBPROPERTIES;
#endif

#ifndef EFXEAXREVERBPROPERTIES_DEFINED
#define EFXEAXREVERBPROPERTIES_DEFINED
typedef struct
{
	float flDensity;
	float flDiffusion;
	float flGain;
	float flGainHF;
	float flGainLF;
	float flDecayTime;
	float flDecayHFRatio;
	float flDecayLFRatio;
	float flReflectionsGain;
	float flReflectionsDelay;
	float flReflectionsPan[3];
	float flLateReverbGain;
	float flLateReverbDelay;
	float flLateReverbPan[3];
	float flEchoTime;
	float flEchoDepth;
	float flModulationTime;
	float flModulationDepth;
	float flAirAbsorptionGainHF;
	float flHFReference;
	float flLFReference;
	float flRoomRolloffFactor;
	int	iDecayHFLimit;
} EFXEAXREVERBPROPERTIES, *LPEFXEAXREVERBPROPERTIES;
#endif

#ifndef EAXOBSTRUCTIONPROPERTIES_DEFINED
#define EAXOBSTRUCTIONPROPERTIES_DEFINED
typedef struct _EAXOBSTRUCTIONPROPERTIES
{
    long          lObstruction;
    float         flObstructionLFRatio;
} EAXOBSTRUCTIONPROPERTIES, *LPEAXOBSTRUCTIONPROPERTIES;
#endif

#ifndef EAXOCCLUSIONPROPERTIES_DEFINED
#define EAXOCCLUSIONPROPERTIES_DEFINED
typedef struct _EAXOCCLUSIONPROPERTIES
{
    long          lOcclusion;
    float         flOcclusionLFRatio;
    float         flOcclusionRoomRatio;
    float         flOcclusionDirectRatio;
} EAXOCCLUSIONPROPERTIES, *LPEAXOCCLUSIONPROPERTIES;
#endif

#ifndef EAXEXCLUSIONPROPERTIES_DEFINED
#define EAXEXCLUSIONPROPERTIES_DEFINED
typedef struct _EAXEXCLUSIONPROPERTIES
{
    long          lExclusion;
    float         flExclusionLFRatio;
} EAXEXCLUSIONPROPERTIES, *LPEAXEXCLUSIONPROPERTIES;
#endif

#ifndef EFXLOWPASSFILTER_DEFINED
#define EFXLOWPASSFILTER_DEFINED
typedef struct _EFXLOWPASSFILTER
{
	float		flGain;
	float		flGainHF;
} EFXLOWPASSFILTER, *LPEFXLOWPASSFILTER;
#endif

#ifdef EFXUTILDLL_EXPORTS
 #define EFX_API __declspec(dllexport)
#else
 #define EFX_API
#endif

EFX_API void __cdecl ConvertReverbParameters(EAXREVERBPROPERTIES *pEAXProp, EFXEAXREVERBPROPERTIES *pEFXEAXReverb);
EFX_API void __cdecl ConvertObstructionParameters(EAXOBSTRUCTIONPROPERTIES *pObProp, EFXLOWPASSFILTER *pDirectLowPassFilter);
EFX_API void __cdecl ConvertExclusionParameters(EAXEXCLUSIONPROPERTIES *pExProp, EFXLOWPASSFILTER *pSendLowPassFilter);
EFX_API void __cdecl ConvertOcclusionParameters(EAXOCCLUSIONPROPERTIES *pOcProp, EFXLOWPASSFILTER *pDirectLowPassFilter, EFXLOWPASSFILTER *pSendLowPassFilter);
EFX_API void __cdecl AdjustEnvironmentSize(EAXREVERBPROPERTIES *pEAXProp, float flEnvironmentSize);

// Used by DSPROPERTY_EAXLISTENER_FLAGS
//
// Note: The number and order of flags may change in future EAX versions.
//       It is recommended to use the flag defines as follows:
//              myFlags = EAXLISTENERFLAGS_DECAYTIMESCALE | EAXLISTENERFLAGS_REVERBSCALE;
//       instead of:
//              myFlags = 0x00000009;
//
// These flags determine what properties are affected by environment size.
#define EAXLISTENERFLAGS_DECAYTIMESCALE        0x00000001 // reverberation decay time
#define EAXLISTENERFLAGS_REFLECTIONSSCALE      0x00000002 // reflection level
#define EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE 0x00000004 // initial reflection delay time
#define EAXLISTENERFLAGS_REVERBSCALE           0x00000008 // reflections level
#define EAXLISTENERFLAGS_REVERBDELAYSCALE      0x00000010 // late reverberation delay time

// This flag limits high-frequency decay time according to air absorption.
#define EAXLISTENERFLAGS_DECAYHFLIMIT          0x00000020

#define EAXLISTENERFLAGS_RESERVED              0xFFFFFFC0 // reserved future use

// property ranges and defaults:

#define EAXLISTENER_MINROOM                       (-10000)
#define EAXLISTENER_MAXROOM                       0
#define EAXLISTENER_DEFAULTROOM                   (-1000)

#define EAXLISTENER_MINROOMHF                     (-10000)
#define EAXLISTENER_MAXROOMHF                     0
#define EAXLISTENER_DEFAULTROOMHF                 (-100)

#define EAXLISTENER_MINROOMROLLOFFFACTOR          0.0f
#define EAXLISTENER_MAXROOMROLLOFFFACTOR          10.0f
#define EAXLISTENER_DEFAULTROOMROLLOFFFACTOR      0.0f

#define EAXLISTENER_MINDECAYTIME                  0.1f
#define EAXLISTENER_MAXDECAYTIME                  20.0f
#define EAXLISTENER_DEFAULTDECAYTIME              1.49f

#define EAXLISTENER_MINDECAYHFRATIO               0.1f
#define EAXLISTENER_MAXDECAYHFRATIO               2.0f
#define EAXLISTENER_DEFAULTDECAYHFRATIO           0.83f

#define EAXLISTENER_MINREFLECTIONS                (-10000)
#define EAXLISTENER_MAXREFLECTIONS                1000
#define EAXLISTENER_DEFAULTREFLECTIONS            (-2602)

#define EAXLISTENER_MINREFLECTIONSDELAY           0.0f
#define EAXLISTENER_MAXREFLECTIONSDELAY           0.3f
#define EAXLISTENER_DEFAULTREFLECTIONSDELAY       0.007f

#define EAXLISTENER_MINREVERB                     (-10000)
#define EAXLISTENER_MAXREVERB                     2000
#define EAXLISTENER_DEFAULTREVERB                 200

#define EAXLISTENER_MINREVERBDELAY                0.0f
#define EAXLISTENER_MAXREVERBDELAY                0.1f
#define EAXLISTENER_DEFAULTREVERBDELAY            0.011f

#define EAXLISTENER_MINENVIRONMENT                0
#define EAXLISTENER_MAXENVIRONMENT                (EAX_ENVIRONMENT_COUNT-1)
#define EAXLISTENER_DEFAULTENVIRONMENT            EAX_ENVIRONMENT_GENERIC

#define EAXLISTENER_MINENVIRONMENTSIZE            1.0f
#define EAXLISTENER_MAXENVIRONMENTSIZE            100.0f
#define EAXLISTENER_DEFAULTENVIRONMENTSIZE        7.5f

#define EAXLISTENER_MINENVIRONMENTDIFFUSION       0.0f
#define EAXLISTENER_MAXENVIRONMENTDIFFUSION       1.0f
#define EAXLISTENER_DEFAULTENVIRONMENTDIFFUSION   1.0f

#define EAXLISTENER_MINAIRABSORPTIONHF            (-100.0f)
#define EAXLISTENER_MAXAIRABSORPTIONHF            0.0f
#define EAXLISTENER_DEFAULTAIRABSORPTIONHF        (-5.0f)

#define EAXLISTENER_DEFAULTFLAGS                  (EAXLISTENERFLAGS_DECAYTIMESCALE |        \
                                                   EAXLISTENERFLAGS_REFLECTIONSSCALE |      \
                                                   EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE | \
                                                   EAXLISTENERFLAGS_REVERBSCALE |           \
                                                   EAXLISTENERFLAGS_REVERBDELAYSCALE |      \
                                                   EAXLISTENERFLAGS_DECAYHFLIMIT)

/***********************************************************************************************\
*
* EAX Reverb Presets in legacy format - use ConvertReverbParameters() to convert to
* EFX EAX Reverb Presets for use with the OpenAL Effects Extension.
*
************************************************************************************************/

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_GENERIC \
	{0,		7.5f,	1.000f,	-1000,	-100,	0,		1.49f,	0.83f,	1.00f,	-2602,	0.007f,	0.00f,0.00f,0.00f,	200,	0.011f,		0.00f,0.00f,0.00f,	0.250f,	0.000f,	0.250f,	0.000f,	-5.0f,	5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_PADDEDCELL \
	{1,		1.4f,	1.000f,	-1000,	-6000,	0,		0.17f,	0.10f,	1.00f,	-1204,	0.001f, 0.00f,0.00f,0.00f,  207,	0.002f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,	5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_ROOM \
	{2,		1.9f,	1.000f,	-1000,	-454,	0,		0.40f,	0.83f,	1.00f,  -1646,	0.002f, 0.00f,0.00f,0.00f,	53,		0.003f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_BATHROOM \
	{3,		1.4f,	1.000f,	-1000,  -1200,	0,		1.49f,	0.54f,	1.00f,  -370,	0.007f, 0.00f,0.00f,0.00f,	1030,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_LIVINGROOM \
	{4,		2.5f,	1.000f,	-1000,  -6000,	0,		0.50f,	0.10f,	1.00f,  -1376,	0.003f, 0.00f,0.00f,0.00f,	-1104,	0.004f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_STONEROOM \
	{5,		11.6f,	1.000f,  -1000, -300,	0,		2.31f,	0.64f,	1.00f,	-711,	0.012f, 0.00f,0.00f,0.00f,	83,		0.017f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_AUDITORIUM \
	{6,		21.6f,	1.000f,  -1000,	-476,	0,		4.32f,	0.59f,	1.00f,	-789,	0.020f, 0.00f,0.00f,0.00f,	-289,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_CONCERTHALL \
	{7,		19.6f,	1.000f,  -1000,	-500,	0,		3.92f,	0.70f,	1.00f,  -1230,	0.020f, 0.00f,0.00f,0.00f,  -02,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_CAVE \
	{8,		14.6f,	1.000f,  -1000,	0,		0,		2.91f,	1.30f,	1.00f,  -602,	0.015f, 0.00f,0.00f,0.00f,	-302,	0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,	0.00f,	0x1f }
#define EAXREVERB_PRESET_ARENA \
	{9,		36.2f,	1.000f,  -1000,	-698,	0,		7.24f,	0.33f,	1.00f,  -1166,	0.020f, 0.00f,0.00f,0.00f,  16,		0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAXREVERB_PRESET_HANGAR \
	{10,	50.3f,	1.000f,  -1000,	-1000,	0,		10.05f, 0.23f,	1.00f,  -602,	0.020f, 0.00f,0.00f,0.00f,  198,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_CARPETTEDHALLWAY \
	{11,	1.9f,	1.000f,	-1000,	-4000,	0,		0.30f,	0.10f,	1.00f,  -1831,	0.002f, 0.00f,0.00f,0.00f,	-1630,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_HALLWAY \
	{12,	1.8f,	1.000f,	-1000,	-300,	0,		1.49f,	0.59f,	1.00f,  -1219,	0.007f, 0.00f,0.00f,0.00f,  441,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_STONECORRIDOR \
	{13,	13.5f,	1.000f,	-1000,	-237,	0,		2.70f,	0.79f,	1.00f,  -1214,	0.013f, 0.00f,0.00f,0.00f,  395,	0.020f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_ALLEY \
	{14,	7.5f,	0.300f,	-1000,	-270,	0,		1.49f,	0.86f,	1.00f,  -1204,	0.007f, 0.00f,0.00f,0.00f,  -4,		0.011f,		0.00f,0.00f,0.00f,	0.125f, 0.950f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_FOREST \
	{15,	38.0f,	0.300f,	-1000,	-3300,	0,		1.49f,	0.54f,	1.00f,  -2560,	0.162f, 0.00f,0.00f,0.00f,	-229,	0.088f,		0.00f,0.00f,0.00f,	0.125f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_CITY \
	{16,	7.5f,	0.500f,	-1000,	-800,	0,		1.49f,	0.67f,	1.00f,  -2273,	0.007f, 0.00f,0.00f,0.00f,	-1691,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_MOUNTAINS \
	{17,	100.0f, 0.270f,	-1000,	-2500,	0,		1.49f,	0.21f,	1.00f,  -2780,	0.300f, 0.00f,0.00f,0.00f,	-1434,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAXREVERB_PRESET_QUARRY \
	{18,	17.5f,	1.000f,	-1000,	-1000,	0,		1.49f,	0.83f,	1.00f,	-10000, 0.061f, 0.00f,0.00f,0.00f,  500,	0.025f,		0.00f,0.00f,0.00f,	0.125f, 0.700f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_PLAIN \
	{19,	42.5f,	0.210f,	-1000,	-2000,	0,		1.49f,	0.50f,	1.00f,  -2466,	0.179f, 0.00f,0.00f,0.00f,	-1926,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_PARKINGLOT \
	{20,	8.3f,	1.000f,	-1000,	0,		0,		1.65f,	1.50f,	1.00f,  -1363,	0.008f, 0.00f,0.00f,0.00f,	-1153,	0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAXREVERB_PRESET_SEWERPIPE \
	{21,	1.7f,	0.800f,	-1000,	-1000,	0,		2.81f,	0.14f,	1.00f,	429,	0.014f, 0.00f,0.00f,0.00f,	1023,	0.021f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_UNDERWATER \
	{22,	1.8f,	1.000f,	-1000,  -4000,	0,		1.49f,	0.10f,	1.00f,  -449,	0.007f, 0.00f,0.00f,0.00f,	1700,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 1.180f, 0.348f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_DRUGGED \
	{23,	1.9f,	0.500f,	-1000,	0,		0,		8.39f,	1.39f,	1.00f,  -115,	0.002f, 0.00f,0.00f,0.00f,  985,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 1.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAXREVERB_PRESET_DIZZY \
	{24,	1.8f,	0.600f,	-1000,	-400,	0,		17.23f, 0.56f,	1.00f,  -1713,	0.020f, 0.00f,0.00f,0.00f,	-613,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.810f, 0.310f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAXREVERB_PRESET_PSYCHOTIC \
	{25,	1.0f,	0.500f,	-1000,	-151,	0,		7.56f,	0.91f,	1.00f,  -626,	0.020f, 0.00f,0.00f,0.00f,  774,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 4.000f, 1.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }


// CASTLE PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_CASTLE_SMALLROOM \
	{ 26,   8.3f,	0.890f,	-1000,	-800,	-2000,	1.22f,	0.83f,	0.31f,	-100,	0.022f,	0.00f,0.00f,0.00f,	600,	0.011f,		0.00f,0.00f,0.00f,	0.138f,	0.080f,	0.250f,	0.000f,	-5.0f,	5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_SHORTPASSAGE \
	{ 26,   8.3f,	0.890f, -1000,  -1000,  -2000,  2.32f,	0.83f,	0.31f,	-100,	0.007f, 0.00f,0.00f,0.00f,  200,		0.023f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_MEDIUMROOM \
	{ 26,   8.3f,	0.930f, -1000,  -1100,  -2000,  2.04f,	0.83f,	0.46f,  -400,	0.022f, 0.00f,0.00f,0.00f,	400,	0.011f,		0.00f,0.00f,0.00f,	0.155f, 0.030f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_LONGPASSAGE \
	{ 26,   8.3f,	0.890f, -1000,  -800,	-2000,  3.42f,	0.83f,	0.31f,  -100,	0.007f, 0.00f,0.00f,0.00f,	300,	0.023f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_LARGEROOM \
	{ 26,   8.3f,	0.820f, -1000,  -1100,  -1800,  2.53f,	0.83f,	0.50f,  -700,	0.034f, 0.00f,0.00f,0.00f,	200,		0.016f,		0.00f,0.00f,0.00f,	0.185f, 0.070f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_HALL \
	{ 26,   8.3f,	0.810f, -1000,  -1100,  -1500,  3.14f,	0.79f,	0.62f,  -1500,	0.056f, 0.00f,0.00f,0.00f,	100,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_CUPBOARD \
	{ 26,   8.3f,	0.890f, -1000,  -1100,  -2000,  0.67f,	0.87f,	0.31f,  300,	0.010f,	0.00f,0.00f,0.00f,	1100,	0.007f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,  5168.6f,	139.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CASTLE_COURTYARD \
	{ 26,   8.3f,	0.420f, -1000,  -700,   -1400,	2.13f,	0.61f,	0.23f,  -1300,	0.160f, 0.00f,0.00f,0.00f,	-300,	0.036f,		0.00f,0.00f,0.00f,	0.250f, 0.370f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x1f }
#define EAXREVERB_PRESET_CASTLE_ALCOVE \
	{ 26,   8.3f,	0.890f,	-1000,  -600,	-2000,  1.64f,	0.87f,	0.31f,  00,	0.007f, 0.00f,0.00f,0.00f,		300,	0.034f,		0.00f,0.00f,0.00f,	0.138f, 0.080f, 0.250f, 0.000f, -5.0f,	5168.6f,	139.5f,  0.00f, 0x20 }


// FACTORY PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_FACTORY_ALCOVE \
	{ 26,   1.8f,	0.590f,  -1200, -200,   -600,	3.14f,	0.65f,	1.31f,  300,	0.010f, 0.00f,0.00f,0.00f,	000,	0.038f,		0.00f,0.00f,0.00f,	0.114f, 0.100f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_SHORTPASSAGE \
	{ 26,   1.8f,	0.640f,  -1200, -200,   -600,	2.53f,	0.65f,	1.31f,  0,		0.010f, 0.00f,0.00f,0.00f,	200,	0.038f,		0.00f,0.00f,0.00f,	0.135f, 0.230f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_MEDIUMROOM \
	{ 26,   1.9f,	0.820f,  -1200, -200,   -600,	2.76f,	0.65f,	1.31f,  -1100,	0.022f, 0.00f,0.00f,0.00f,	300,	0.023f,		0.00f,0.00f,0.00f,	0.174f, 0.070f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_LONGPASSAGE \
	{ 26,   1.8f,	0.640f,  -1200, -200,   -600,	4.06f,	0.65f,	1.31f,  0,		0.020f, 0.00f,0.00f,0.00f,	200,	0.037f,		0.00f,0.00f,0.00f,	0.135f, 0.230f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_LARGEROOM \
	{ 26,   1.9f,	0.750f,  -1200, -300,   -400,	4.24f,	0.51f,	1.31f,  -1500,	0.039f, 0.00f,0.00f,0.00f,	100,		0.023f,		0.00f,0.00f,0.00f,	0.231f, 0.070f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_HALL \
	{ 26,   1.9f,	0.750f,  -1000, -300,   -400,	7.43f,	0.51f,	1.31f,  -2400,	0.073f, 0.00f,0.00f,0.00f,	-100,	0.027f,		0.00f,0.00f,0.00f,	0.250f, 0.070f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_CUPBOARD \
	{ 26,   1.7f,	0.630f,  -1200, -200,   -600,	0.49f,	0.65f,	1.31f,  200,	0.010f, 0.00f,0.00f,0.00f,	600,	0.032f,		0.00f,0.00f,0.00f,	0.107f, 0.070f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_COURTYARD \
	{ 26,   1.7f,	0.570f,  -1000, -1000,  -400,	2.32f,	0.29f,	0.56f,  -1300,	0.140f, 0.00f,0.00f,0.00f,	-800,	0.039f,		0.00f,0.00f,0.00f,	0.250f, 0.290f, 0.250f, 0.000f, -5.0f,  3762.6f,	362.5f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_FACTORY_SMALLROOM \
	{ 26,   1.8f,	0.820f,  -1000,	-200,   -600,	1.72f,	0.65f,	1.31f,  -300,	0.010f, 0.00f,0.00f,0.00f,	500,	0.024f,		0.00f,0.00f,0.00f,	0.119f, 0.070f, 0.250f, 0.000f, -5.0f,	3762.6f,	362.5f,  0.00f, 0x20 }


// ICE PALACE PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_ICEPALACE_ALCOVE \
	{ 26,   2.7f,	0.840f, -1000,  -500,	-1100,  2.76f,	1.46f,	0.28f,  100,	0.010f, 0.00f,0.00f,0.00f,	-100,	0.030f,		0.00f,0.00f,0.00f,	0.161f, 0.090f, 0.250f, 0.000f,	-5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_SHORTPASSAGE \
	{ 26,   2.7f,	0.750f, -1000,  -500,	-1100,  1.79f,	1.46f,	0.28f,  -600,	0.010f, 0.00f,0.00f,0.00f,	100,		0.019f,		0.00f,0.00f,0.00f,	0.177f, 0.090f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_MEDIUMROOM \
	{ 26,   2.7f,	0.870f, -1000,  -500,   -700,	2.22f,	1.53f,	0.32f,  -800,	0.039f, 0.00f,0.00f,0.00f,	100,	0.027f,		0.00f,0.00f,0.00f,	0.186f, 0.120f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_LONGPASSAGE \
	{ 26,   2.7f,	0.770f, -1000,  -500,   -800,	3.01f,	1.46f,	0.28f,  -200,	0.012f, 0.00f,0.00f,0.00f,	200,	0.025f,		0.00f,0.00f,0.00f,	0.186f, 0.040f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_LARGEROOM \
	{ 26,   2.9f,	0.810f, -1000,  -500,   -700,	3.14f,	1.53f,	0.32f,  -1200,	0.039f, 0.00f,0.00f,0.00f,	000,	0.027f,		0.00f,0.00f,0.00f,	0.214f, 0.110f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_HALL \
	{ 26,   2.9f,	0.760f, -1000,  -700,   -500,	5.49f,	1.53f,	0.38f,  -1900,	0.054f, 0.00f,0.00f,0.00f,	-400,	0.052f,		0.00f,0.00f,0.00f,	0.226f, 0.110f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_CUPBOARD \
	{ 26,   2.7f,	0.830f, -1000,  -600,	-1300,  0.76f,	1.53f,	0.26f,  100,	0.012f, 0.00f,0.00f,0.00f,	600,	0.016f,		0.00f,0.00f,0.00f,	0.143f, 0.080f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_COURTYARD \
	{ 26,   2.9f,	0.590f, -1000,  -1100,  -1000,  2.04f,	1.20f,	0.38f,  -1000,	0.173f, 0.00f,0.00f,0.00f,	-1000,	0.043f,		0.00f,0.00f,0.00f,	0.235f, 0.480f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }
#define EAXREVERB_PRESET_ICEPALACE_SMALLROOM \
	{ 26,   2.7f,	0.840f, -1000,  -500,	-1100,  1.51f,	1.53f,	0.27f,	-100,	0.010f, 0.00f,0.00f,0.00f,	300,	0.011f,		0.00f,0.00f,0.00f,	0.164f, 0.140f, 0.250f, 0.000f, -5.0f,	12428.5f,	99.6f,  0.00f,	0x20 }


// SPACE STATION PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_SPACESTATION_ALCOVE \
	{ 26,   1.5f,	0.780f, -1000,  -300,   -100,	1.16f,	0.81f,	0.55f,  300,	0.007f, 0.00f,0.00f,0.00f,	000,	0.018f,		0.00f,0.00f,0.00f,	0.192f, 0.210f, 0.250f, 0.000f,	-5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_MEDIUMROOM \
	{ 26,   1.5f,	0.750f, -1000,  -400,   -100,	3.01f,	0.50f,	0.55f,  -800,	0.034f, 0.00f,0.00f,0.00f,	100,		0.035f,		0.00f,0.00f,0.00f,	0.209f, 0.310f, 0.250f, 0.000f,	-5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_SHORTPASSAGE \
	{ 26,   1.5f,	0.870f, -1000,  -400,   -100,	3.57f,	0.50f,	0.55f,  0,		0.012f, 0.00f,0.00f,0.00f,	100,		0.016f,		0.00f,0.00f,0.00f,	0.172f, 0.200f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_LONGPASSAGE \
	{ 26,   1.9f,	0.820f, -1000,  -400,   -100,	4.62f,	0.62f,	0.55f,  0,		0.012f, 0.00f,0.00f,0.00f,	200,		0.031f,		0.00f,0.00f,0.00f,	0.250f, 0.230f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_LARGEROOM \
	{ 26,   1.8f,	0.810f, -1000,  -400,   -100,	3.89f,	0.38f,	0.61f,  -1000,	0.056f, 0.00f,0.00f,0.00f,	-100,	0.035f,		0.00f,0.00f,0.00f,	0.233f, 0.280f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_HALL \
	{ 26,   1.9f,	0.870f, -1000,  -400,   -100,	7.11f,	0.38f,	0.61f,  -1500,	0.100f, 0.00f,0.00f,0.00f,	-400,	0.047f,		0.00f,0.00f,0.00f,	0.250f, 0.250f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_CUPBOARD \
	{ 26,   1.4f,	0.560f, -1000,  -300,   -100,	0.79f,	0.81f,	0.55f,  300,	0.007f, 0.00f,0.00f,0.00f,	500,	0.018f,		0.00f,0.00f,0.00f,	0.181f, 0.310f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPACESTATION_SMALLROOM \
	{ 26,   1.5f,	0.700f, -1000,  -300,   -100,	1.72f,	0.82f,	0.55f,	-200,	0.007f, 0.00f,0.00f,0.00f,	300,	0.013f,		0.00f,0.00f,0.00f,	0.188f, 0.260f, 0.250f, 0.000f, -5.0f,  3316.1f,	458.2f,  0.00f, 0x20 }


// WOODEN GALLEON PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_WOODEN_ALCOVE \
	{ 26,   7.5f,	1.000f, -1000,  -1800,  -1000,  1.22f,	0.62f,	0.91f,	100,	0.012f, 0.00f,0.00f,0.00f,	-300,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_SHORTPASSAGE \
	{ 26,   7.5f,	1.000f, -1000,  -1800,  -1000,  1.75f,	0.50f,	0.87f,	-100,	0.012f, 0.00f,0.00f,0.00f,	-400,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_MEDIUMROOM \
	{ 26,   7.5f,	1.000f, -1000,  -2000,  -1100,  1.47f,	0.42f,	0.82f,	-100,	0.049f, 0.00f,0.00f,0.00f,	-100,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_LONGPASSAGE \
	{ 26,   7.5f,	1.000f, -1000,  -2000,  -1000,  1.99f,	0.40f,	0.79f,	000,	0.020f, 0.00f,0.00f,0.00f,	-700,	0.036f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_LARGEROOM \
	{ 26,   7.5f,	1.000f, -1000,  -2100,  -1100,  2.65f,	0.33f,	0.82f,	-100,	0.066f, 0.00f,0.00f,0.00f,	-200,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_HALL \
	{ 26,   7.5f,	1.000f, -1000,  -2200,  -1100,  3.45f,	0.30f,	0.82f,	-100,	0.088f, 0.00f,0.00f,0.00f,	-200,	0.063f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_CUPBOARD \
	{ 26,   7.5f,	1.000f, -1000,  -1700,  -1000,  0.56f,	0.46f,	0.91f,	100,	0.012f, 0.00f,0.00f,0.00f,	100,	0.028f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_SMALLROOM \
	{ 26,   7.5f,	1.000f, -1000,  -1900,  -1000,  0.79f,	0.32f,	0.87f,	00,		0.032f, 0.00f,0.00f,0.00f,	-100,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_WOODEN_COURTYARD \
	{ 26,   7.5f,	0.650f, -1000,  -2200,  -1000,  1.79f,	0.35f,	0.79f,	-500,	0.123f, 0.00f,0.00f,0.00f,	-2000,	0.032f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  4705.0f,	99.6f,  0.00f,	0x3f }


// SPORTS PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_SPORT_EMPTYSTADIUM \
	{ 26,   7.2f,	1.000f, -1000,  -700,   -200,	6.26f,	0.51f,	1.10f,  -2400,	0.183f, 0.00f,0.00f,0.00f,	-800,	0.038f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPORT_SQUASHCOURT \
	{ 26,   7.5f,	0.750f, -1000,  -1000,  -200,	2.22f,	0.91f,	1.16f,  -700,	0.007f, 0.00f,0.00f,0.00f,	-200,	0.011f,		0.00f,0.00f,0.00f,	0.126f, 0.190f, 0.250f, 0.000f, -5.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPORT_SMALLSWIMMINGPOOL \
	{ 26,  36.2f,	0.700f, -1000,  -200,   -100,	2.76f,	1.25f,	1.14f,  -400,	0.020f, 0.00f,0.00f,0.00f,	-200,	0.030f,		0.00f,0.00f,0.00f,	0.179f, 0.150f, 0.895f, 0.190f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_SPORT_LARGESWIMMINGPOOL\
	{ 26,  36.2f,	0.820f, -1000,  -200,   0,		5.49f,	1.31f,	1.14f,  -700,	0.039f, 0.00f,0.00f,0.00f,	-600,	0.049f,		0.00f,0.00f,0.00f,	0.222f, 0.550f, 1.159f, 0.210f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_SPORT_GYMNASIUM \
	{ 26,   7.5f,	0.810f, -1000,  -700,   -100,	3.14f,	1.06f,	1.35f,  -800,	0.029f, 0.00f,0.00f,0.00f,	-500,	0.045f,		0.00f,0.00f,0.00f,	0.146f, 0.140f, 0.250f, 0.000f, -5.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPORT_FULLSTADIUM \
	{ 26,   7.2f,	1.000f, -1000,  -2300,  -200,	5.25f,	0.17f,	0.80f,  -2000,	0.188f, 0.00f,0.00f,0.00f,	-1100,	0.038f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_SPORT_STADIUMTANNOY \
	{ 26,   3.0f,	0.780f, -1000,   -500,   -600,	2.53f,	0.88f,	0.68f,  -1100,	0.230f, 0.00f,0.00f,0.00f,	-600,	0.063f,		0.00f,0.00f,0.00f,	0.250f, 0.200f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }


// PREFAB PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_PREFAB_WORKSHOP \
	{ 26,   1.9f,	1.000f, -1000,  -1700,  -800,	0.76f,	1.00f,	1.00f,	0,		0.012f, 0.00f,0.00f,0.00f,	100,		0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_PREFAB_SCHOOLROOM \
	{ 26,   1.86f,	0.690f, -1000,  -400,   -600,	0.98f,	0.45f,	0.18f,  300,	0.017f, 0.00f,0.00f,0.00f,  300,	0.015f,		0.00f,0.00f,0.00f,	0.095f, 0.140f, 0.250f, 0.000f, -5.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_PREFAB_PRACTISEROOM \
	{ 26,   1.86f,	0.870f, -1000,  -800,   -600,	1.12f,	0.56f,	0.18f,  200,	0.010f, 0.00f,0.00f,0.00f,	300,	0.011f,		0.00f,0.00f,0.00f,	0.095f, 0.140f, 0.250f, 0.000f, -5.0f,  7176.9f,	211.2f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_PREFAB_OUTHOUSE \
	{ 26,  80.3f,	0.820f, -1000,  -1900,  -1600,  1.38f,	0.38f,	0.35f,	-100,	0.024f, 0.00f,0.00f,-0.00f,	-400,	0.044f,		0.00f,0.00f,0.00f,	0.121f, 0.170f, 0.250f, 0.000f, -5.0f,  2854.4f,	107.5f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_PREFAB_CARAVAN \
	{ 26,   8.3f,	1.000f, -1000,  -2100,  -1800,  0.43f,	1.50f,	1.00f,  0,		0.012f, 0.00f,0.00f,0.00f,	600,	0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x1f }
			// for US developers, a caravan is the same as a trailer =o)


// DOME AND PIPE PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_DOME_TOMB \
	{ 26,  51.8f,	0.790f, -1000,  -900,	-1300,  4.18f,	0.21f,	0.10f,  -825,	0.030f, 0.00f,0.00f,0.00f,	450,	0.022f,		0.00f,0.00f,0.00f,	0.177f, 0.190f, 0.250f, 0.000f,	-5.0f,  2854.4f,	20.0f,  0.00f,	0x0 }
#define EAXREVERB_PRESET_PIPE_SMALL \
	{ 26,  50.3f,	1.000f, -1000,  -900,	-1300,  5.04f,	0.10f,	0.10f,  -600,	0.032f, 0.00f,0.00f,0.00f,	800,	0.015f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_DOME_SAINTPAULS \
	{ 26,  50.3f,	0.870f, -1000,  -900,	-1300,  10.48f,	0.19f,	0.10f,  -1500,	0.090f, 0.00f,0.00f,0.00f,	200,	0.042f,		0.00f,0.00f,0.00f,	0.250f, 0.120f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_PIPE_LONGTHIN \
	{ 26,   1.6f,	0.910f, -1000,  -700,	-1100,  9.21f,	0.18f,	0.10f,  -300,	0.010f, 0.00f,0.00f,0.00f,	-300,	0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x0 }
#define EAXREVERB_PRESET_PIPE_LARGE \
	{ 26,  50.3f,	1.000f, -1000,  -900,	-1300,  8.45f,	0.10f,	0.10f,  -800,	0.046f, 0.00f,0.00f,0.00f,  400,	0.032f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x3f }
#define EAXREVERB_PRESET_PIPE_RESONANT \
	{ 26,   1.3f,	0.910f, -1000,  -700,	-1100,  6.81f,	0.18f,	0.10f,  -300,	0.010f, 0.00f,0.00f,0.00f,	00,		0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  2854.4f,	20.0f,  0.00f,	0x0 }


// OUTDOORS PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_OUTDOORS_BACKYARD \
	{ 26,  80.3f,	0.450f,	-1000,  -1200,  -600,	1.12f,	0.34f,	0.46f,  -700,	0.069f, 0.00f,0.00f,-0.00f,	-300,	0.023f,		0.00f,0.00f,0.00f,	0.218f, 0.340f, 0.250f, 0.000f,	-5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_OUTDOORS_ROLLINGPLAINS \
	{ 26,  80.3f,	0.000f,	-1000,  -3900,  -400,	2.13f,	0.21f,	0.46f,  -1500,	0.300f, 0.00f,0.00f,-0.00f,	-700,	0.019f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_OUTDOORS_DEEPCANYON \
	{ 26,  80.3f,	0.740f,	-1000,  -1500,  -400,	3.89f,	0.21f,	0.46f,  -1000,	0.223f, 0.00f,0.00f,-0.00f,	-900,	0.019f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_OUTDOORS_CREEK \
	{ 26,  80.3f,	0.350f,	-1000,  -1500,  -600,	2.13f,	0.21f,	0.46f,  -800,	0.115f, 0.00f,0.00f,-0.00f,	-1400,	0.031f,		0.00f,0.00f,0.00f,	0.218f, 0.340f, 0.250f, 0.000f, -5.0f,  4399.1f,	242.9f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_OUTDOORS_VALLEY \
	{ 26,  80.3f,	0.280f,	-1000,  -3100,	-1600,  2.88f,	0.26f,	0.35f,  -1700,	0.263f, 0.00f,0.00f,-0.00f,	-800,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 0.340f, 0.250f, 0.000f, -5.0f,  2854.4f,	107.5f,  0.00f, 0x0 }


// MOOD PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_MOOD_HEAVEN \
	{ 26,  19.6f,	0.940f,  -1000, -200,   -700,	5.04f,	1.12f,	0.56f,  -1230,	0.020f, 0.00f,0.00f,0.00f,	200,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.080f, 2.742f, 0.050f, -2.0f,  5000.0f,	250.0f,  0.00f, 0x3f }
#define EAXREVERB_PRESET_MOOD_HELL \
	{ 26, 100.0f,	0.570f,  -1000, -900,   -700,	3.57f,	0.49f,	2.00f,	-10000, 0.020f, 0.00f,0.00f,0.00f,	300,	0.030f,		0.00f,0.00f,0.00f,	0.110f, 0.040f, 2.109f, 0.520f, -5.0f,  5000.0f,	139.5f,  0.00f, 0x40 }
#define EAXREVERB_PRESET_MOOD_MEMORY \
	{ 26,   8.0f,	0.850f,  -1000, -400,   -900,	4.06f,	0.82f,	0.56f,  -2800,	0.000f, 0.00f,0.00f,0.00f,	100,	0.000f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.474f, 0.450f, -10.0f,  5000.0f,	250.0f,  0.00f, 0x0 }


// DRIVING SIMULATION PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_DRIVING_COMMENTATOR \
	{ 26,   3.0f,	0.000f, -1000,  -500,   -600,	2.42f,	0.88f,	0.68f,  -1400,	0.093f, 0.00f,0.00f,0.00f,	-1200,	0.017f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -10.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_DRIVING_PITGARAGE \
	{ 26,   1.9f,	0.590f, -1000,  -300,   -500,	1.72f,	0.93f,	0.87f,  -500,	0.000f, 0.00f,0.00f,0.00f,	200,		0.016f,		0.00f,0.00f,0.00f,	0.250f, 0.110f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_DRIVING_INCAR_RACER \
	{ 26,   1.1f,	0.800f, -1000,   0,		-200,	0.17f,	2.00f,	0.41f,  500,	0.007f, 0.00f,0.00f,0.00f,	-300,	0.015f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10268.2f,	251.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_DRIVING_INCAR_SPORTS \
	{ 26,   1.1f,	0.800f, -1000,	-400,   0,		0.17f,	0.75f,	0.41f,  0,		0.010f, 0.00f,0.00f,0.00f,	-500,	0.000f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10268.2f,	251.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_DRIVING_INCAR_LUXURY \
	{ 26,   1.6f,	1.000f, -1000,	-2000,  -600,	0.13f,	0.41f,	0.46f,  -200,	0.010f, 0.00f,0.00f,0.00f,	400,	0.010f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10268.2f,	251.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_DRIVING_FULLGRANDSTAND \
	{ 26,   8.3f,	1.000f, -1000,  -1100,  -400,	3.01f,	1.37f,	1.28f,  -900,	0.090f, 0.00f,0.00f,0.00f,	-1500,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10420.2f,	250.0f,  0.00f, 0x1f }
#define EAXREVERB_PRESET_DRIVING_EMPTYGRANDSTAND \
	{ 26,   8.3f,	1.000f, -1000,   0,		-200,	4.62f,	1.75f,	1.40f,  -1363,	0.090f, 0.00f,0.00f,0.00f,	-1200,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,	10420.2f,	250.0f,  0.00f, 0x1f }
#define EAXREVERB_PRESET_DRIVING_TUNNEL \
	{ 26,   3.1f,	0.810f, -1000,   -800,	-100,	3.42f,	0.94f,	1.31f,  -300,	0.051f, 0.00f,0.00f,0.00f,  -300,	0.047f,		0.00f,0.00f,0.00f,	0.214f, 0.050f, 0.250f, 0.000f, -5.0f,  5000.0f,	155.3f,  0.00f, 0x20 }


// CITY PRESETS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_CITY_STREETS \
	{ 26,   3.0f,	0.780f, -1000,  -300,   -100,	1.79f,	1.12f,	0.91f,  -1100,	0.046f, 0.00f,0.00f,0.00f,	-1400,	0.028f,		0.00f,0.00f,0.00f,	0.250f, 0.200f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CITY_SUBWAY \
	{ 26,   3.0f,	0.740f, -1000,  -300,   -100,	3.01f,	1.23f,	0.91f,   -300,	0.046f, 0.00f,0.00f,0.00f,	200,	0.028f,		0.00f,0.00f,0.00f,	0.125f, 0.210f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CITY_MUSEUM \
	{ 26,  80.3f,	0.820f, -1000,  -1500,  -1500,  3.28f,	1.40f,	0.57f,  -1200,	0.039f, 0.00f,0.00f,-0.00f, -100,	0.034f,		0.00f,0.00f,0.00f,	0.130f, 0.170f, 0.250f, 0.000f, -5.0f,  2854.4f,	107.5f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_CITY_LIBRARY \
	{ 26,  80.3f,	0.820f, -1000,  -1100,  -2100,  2.76f,	0.89f,	0.41f,  -900,	0.029f, 0.00f,0.00f,-0.00f, -100,	0.020f,		0.00f,0.00f,0.00f,	0.130f, 0.170f, 0.250f, 0.000f, -5.0f,  2854.4f,	107.5f,  0.00f, 0x0 }
#define EAXREVERB_PRESET_CITY_UNDERPASS \
	{ 26,   3.0f,	0.820f, -1000,  -700,   -100,	3.57f,	1.12f,	0.91f,  -800,	0.059f, 0.00f,0.00f,0.00f,	-100,	0.037f,		0.00f,0.00f,0.00f,	0.250f, 0.140f, 0.250f, 0.000f, -7.0f,  5000.0f,	250.0f,  0.00f, 0x20 }
#define EAXREVERB_PRESET_CITY_ABANDONED \
	{ 26,   3.0f,	0.690f, -1000,  -200,   -100,	3.28f,	1.17f,	0.91f,  -700,	0.044f, 0.00f,0.00f,0.00f,	-1100,	0.024f,		0.00f,0.00f,0.00f,	0.250f, 0.200f, 0.250f, 0.000f, -3.0f,  5000.0f,	250.0f,  0.00f, 0x20 }


// MISC ROOMS

//	Env		Size	Diffus	Room	RoomHF	RoomLF	DecTm	DcHF	DcLF	Refl	RefDel	Ref Pan				Revb	RevDel		Rev Pan				EchTm	EchDp	ModTm	ModDp	AirAbs	HFRef		LFRef	RRlOff	FLAGS
#define EAXREVERB_PRESET_DUSTYROOM  \
	{ 26,   1.8f,	0.560f,	-1000,	-200,	-300,	1.79f,	0.38f,	0.21f,	-600,	0.002f,	0.00f,0.00f,0.00f,	200,	0.006f,		0.00f,0.00f,0.00f,	0.202f, 0.050f, 0.250f, 0.000f, -10.0f,  13046.0f,	163.3f,	0.00f,	0x20 }
#define EAXREVERB_PRESET_CHAPEL \
	{ 26,  19.6f,	0.840f,	-1000,  -500,	0,		4.62f,	0.64f,	1.23f,  -700,	0.032f,	0.00f,0.00f,0.00f,	-200,	0.049f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.110f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAXREVERB_PRESET_SMALLWATERROOM \
	{ 26,  36.2f,	0.700f, -1000,  -698,   0,		1.51f,	1.25f,	1.14f,  -100,	0.020f, 0.00f,0.00f,0.00f,	300,	0.030f,		0.00f,0.00f,0.00f,	0.179f, 0.150f, 0.895f, 0.190f, -7.0f,  5000.0f,	250.0f, 0.00f, 0x0 }

//EFX implementation of converted EAX environment presets
//													flDensity		flDiffusion		flGain			flGainHF		flGainLF	,	flDecayTime		flDecayHFRatio	flDecayLFRatio	flReflectionsGain		flReflectionsDelay		flReflectionsPan[0]		flReflectionsPan[1]		flReflectionsPan[2]		flLateReverbGain		flLateReverbDelay		flLateReverbPan[0]		flLateReverbPan[1]		flLateReverbPan[2]		flEchoTime		flEchoDepth		flModulationTime		flModulationDepth		flAirAbsorptionGainHF		flHFReference		flLFReference		flRoomRolloffFactor		iDecayHflimit
#define	REVERB_PRESET_GENERIC					{	1.000000	,	1.000000	,	0.8063814	,	0.891251	,	1.000000	,	1.490000	,	0.830000	,	1.000000		,	0.050003			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	0
#define	REVERB_PRESET_PADDEDCELL				{	0.171500	,	1.000000	,	0.8063814	,	0.001000	,	1.000000	,	0.170000	,	0.100000	,	1.000000		,	0.250035			,	0.001000			,	0.000000			,	0.000000			,	0.000000			,	1.269112			,	0.002000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	1
#define	REVERB_PRESET_ROOM						{	0.428687	,	1.000000	,	0.8063814	,	0.592925	,	1.000000	,	0.400000	,	0.830000	,	1.000000		,	0.150314			,	0.002000			,	0.000000			,	0.000000			,	0.000000			,	1.062919			,	0.003000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	2
#define	REVERB_PRESET_BATHROOM					{	0.171500	,	1.000000	,	0.8063814	,	0.251189	,	1.000000	,	1.490000	,	0.540000	,	1.000000		,	0.653131			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	3.273407			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	3
#define	REVERB_PRESET_LIVINGROOM				{	0.976563	,	1.000000	,	0.8063814	,	0.001000	,	1.000000	,	0.500000	,	0.100000	,	1.000000		,	0.205116			,	0.003000			,	0.000000			,	0.000000			,	0.000000			,	0.280543			,	0.004000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	4
#define	REVERB_PRESET_STONEROOM					{	1.000000	,	1.000000	,	0.8063814	,	0.707946	,	1.000000	,	2.310000	,	0.640000	,	1.000000		,	0.441062			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.100272			,	0.017000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	5
#define	REVERB_PRESET_AUDITORIUM				{	1.000000	,	1.000000	,	0.8063814	,	0.578096	,	1.000000	,	4.320000	,	0.590000	,	1.000000		,	0.403181			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.716968			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	6
#define	REVERB_PRESET_CONCERTHALL				{	1.000000	,	1.000000	,	0.8063814	,	0.562341	,	1.000000	,	3.920000	,	0.700000	,	1.000000		,	0.242661			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.997700			,	0.029000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	7
#define	REVERB_PRESET_CAVE						{	1.000000	,	1.000000	,	0.8063814	,	1.000000	,	1.000000	,	2.910000	,	1.300000	,	1.000000		,	0.500035			,	0.015000			,	0.000000			,	0.000000			,	0.000000			,	0.706318			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	8
#define	REVERB_PRESET_ARENA						{	1.000000	,	1.000000	,	0.8063814	,	0.447713	,	1.000000	,	7.240000	,	0.330000	,	1.000000		,	0.261216			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	1.018591			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	9
#define	REVERB_PRESET_HANGAR					{	1.000000	,	1.000000	,	0.8063814	,	0.316228	,	1.000000	,	10.050000	,	0.230000	,	1.000000		,	0.500035			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	1.256030			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	10
#define	REVERB_PRESET_CARPETTEDHALLWAY			{	0.428687	,	1.000000	,	0.8063814	,	0.010000	,	1.000000	,	0.300000	,	0.100000	,	1.000000		,	0.121479			,	0.002000			,	0.000000			,	0.000000			,	0.000000			,	0.153109			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	11
#define	REVERB_PRESET_HALLWAY					{	0.364500	,	1.000000	,	0.8063814	,	0.707946	,	1.000000	,	1.490000	,	0.590000	,	1.000000		,	0.245754			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.661499			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	12
#define	REVERB_PRESET_STONECORRIDOR				{	1.000000	,	1.000000	,	0.8063814	,	0.761202	,	1.000000	,	2.700000	,	0.790000	,	1.000000		,	0.247172			,	0.013000			,	0.000000			,	0.000000			,	0.000000			,	1.575796			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	13
#define	REVERB_PRESET_ALLEY						{	1.000000	,	0.300000	,	0.8063814	,	0.732825	,	1.000000	,	1.490000	,	0.860000	,	1.000000		,	0.250035			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	0.995405			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.125000	,	0.950000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	14
#define	REVERB_PRESET_FOREST					{	1.000000	,	0.300000	,	0.8063814	,	0.022387	,	1.000000	,	1.490000	,	0.540000	,	1.000000		,	0.052481			,	0.162000			,	0.000000			,	0.000000			,	0.000000			,	0.768245			,	0.088000			,	0.000000			,	0.000000			,	0.000000			,	0.125000	,	1.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	15
#define	REVERB_PRESET_CITY						{	1.000000	,	0.500000	,	0.8063814	,	0.398107	,	1.000000	,	1.490000	,	0.670000	,	1.000000		,	0.073030			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	0.142725			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	16
#define	REVERB_PRESET_MOUNTAINS					{	1.000000	,	0.270000	,	0.8063814	,	0.056234	,	1.000000	,	1.490000	,	0.210000	,	1.000000		,	0.040738			,	0.300000			,	0.000000			,	0.000000			,	0.000000			,	0.191867			,	0.100000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	1.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	17
#define	REVERB_PRESET_QUARRY					{	1.000000	,	1.000000	,	0.8063814	,	0.316228	,	1.000000	,	1.490000	,	0.830000	,	1.000000		,	0.000000			,	0.061000			,	0.000000			,	0.000000			,	0.000000			,	1.778279			,	0.025000			,	0.000000			,	0.000000			,	0.000000			,	0.125000	,	0.700000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	18
#define	REVERB_PRESET_PLAIN						{	1.000000	,	0.210000	,	0.8063814	,	0.100000	,	1.000000	,	1.490000	,	0.500000	,	1.000000		,	0.058479			,	0.179000			,	0.000000			,	0.000000			,	0.000000			,	0.108893			,	0.100000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	1.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	19
#define	REVERB_PRESET_PARKINGLOT				{	1.000000	,	1.000000	,	0.8063814	,	1.000000	,	1.000000	,	1.650000	,	1.500000	,	1.000000		,	0.208209			,	0.008000			,	0.000000			,	0.000000			,	0.000000			,	0.265155			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	20
#define	REVERB_PRESET_SEWERPIPE					{	0.307063	,	0.800000	,	0.8063814	,	0.316228	,	1.000000	,	2.810000	,	0.140000	,	1.000000		,	1.638702			,	0.014000			,	0.000000			,	0.000000			,	0.000000			,	3.247133			,	0.021000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	21
#define	REVERB_PRESET_UNDERWATER				{	0.364500	,	1.000000	,	0.8063814	,	0.010000	,	1.000000	,	1.490000	,	0.100000	,	1.000000		,	0.596348			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	7.079458			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	1.180000			,	0.348000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	22
#define	REVERB_PRESET_DRUGGED					{	0.428687	,	0.500000	,	0.8063814	,	1.000000	,	1.000000	,	8.390000	,	1.390000	,	1.000000		,	0.875992			,	0.002000			,	0.000000			,	0.000000			,	0.000000			,	3.108136			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	1.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	23
#define	REVERB_PRESET_DIZZY						{	0.364500	,	0.600000	,	0.8063814	,	0.630957	,	1.000000	,	17.230000	,	0.560000	,	1.000000		,	0.139155			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.493742			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	1.000000	,	0.810000			,	0.310000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	24
#define	REVERB_PRESET_PSYCHOTIC					{	0.062500	,	0.500000	,	0.8063814	,	0.840427	,	1.000000	,	7.560000	,	0.910000	,	1.000000		,	0.486407			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	2.437811			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	4.000000			,	1.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	25
#define	REVERB_PRESET_CASTLE_SMALLROOM			{	1.000000	,	0.890000	,	0.8063814	,	0.398107	,	0.100000	,	1.220000	,	0.830000	,	0.310000		,	0.891251			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	1.995262			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.138000	,	0.080000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	26
#define	REVERB_PRESET_CASTLE_SHORTPASSAGE		{	1.000000	,	0.890000	,	0.8063814	,	0.316228	,	0.100000	,	2.320000	,	0.830000	,	0.310000		,	0.891251			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.023000			,	0.000000			,	0.000000			,	0.000000			,	0.138000	,	0.080000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	27
#define	REVERB_PRESET_CASTLE_MEDIUMROOM			{	1.000000	,	0.930000	,	0.8063814	,	0.281838	,	0.100000	,	2.040000	,	0.830000	,	0.460000		,	0.630957			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	1.584893			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.155000	,	0.030000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	28
#define	REVERB_PRESET_CASTLE_LONGPASSAGE		{	1.000000	,	0.890000	,	0.8063814	,	0.398107	,	0.100000	,	3.420000	,	0.830000	,	0.310000		,	0.891251			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.023000			,	0.000000			,	0.000000			,	0.000000			,	0.138000	,	0.080000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	29
#define	REVERB_PRESET_CASTLE_LARGEROOM			{	1.000000	,	0.820000	,	0.8063814	,	0.281838	,	0.125893	,	2.530000	,	0.830000	,	0.500000		,	0.446684			,	0.034000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.016000			,	0.000000			,	0.000000			,	0.000000			,	0.185000	,	0.070000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	30
#define	REVERB_PRESET_CASTLE_HALL				{	1.000000	,	0.810000	,	0.8063814	,	0.281838	,	0.177828	,	3.140000	,	0.790000	,	0.620000		,	0.177828			,	0.056000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.024000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	31
#define	REVERB_PRESET_CASTLE_CUPBOARD			{	1.000000	,	0.890000	,	0.8063814	,	0.281838	,	0.100000	,	0.670000	,	0.870000	,	0.310000		,	1.412538			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	3.548134			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	0.138000	,	0.080000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	32
#define	REVERB_PRESET_CASTLE_COURTYARD			{	1.000000	,	0.420000	,	0.8063814	,	0.446684	,	0.199526	,	2.130000	,	0.610000	,	0.230000		,	0.223872			,	0.160000			,	0.000000			,	0.000000			,	0.000000			,	0.707946			,	0.036000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.370000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	33
#define	REVERB_PRESET_CASTLE_ALCOVE				{	1.000000	,	0.890000	,	0.8063814	,	0.501187	,	0.100000	,	1.640000	,	0.870000	,	0.310000		,	1.000000			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.034000			,	0.000000			,	0.000000			,	0.000000			,	0.138000	,	0.080000	,	0.250000			,	0.000000			,	0.994260				,	5168.600098		,	139.500000		,	0.000000			,	1	}	// Preset Nr.:	34
#define	REVERB_PRESET_FACTORY_ALCOVE			{	0.364500	,	0.590000	,	0.64053195	,	0.794328	,	0.501187	,	3.140000	,	0.650000	,	1.310000		,	1.412538			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.000000			,	0.038000			,	0.000000			,	0.000000			,	0.000000			,	0.114000	,	0.100000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	35
#define	REVERB_PRESET_FACTORY_SHORTPASSAGE		{	0.364500	,	0.640000	,	0.64053195	,	0.794328	,	0.501187	,	2.530000	,	0.650000	,	1.310000		,	1.000000			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.038000			,	0.000000			,	0.000000			,	0.000000			,	0.135000	,	0.230000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	36
#define	REVERB_PRESET_FACTORY_MEDIUMROOM		{	0.428687	,	0.820000	,	0.64053195	,	0.794328	,	0.501187	,	2.760000	,	0.650000	,	1.310000		,	0.281838			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.023000			,	0.000000			,	0.000000			,	0.000000			,	0.174000	,	0.070000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	37
#define	REVERB_PRESET_FACTORY_LONGPASSAGE		{	0.364500	,	0.640000	,	0.64053195	,	0.794328	,	0.501187	,	4.060000	,	0.650000	,	1.310000		,	1.000000			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.037000			,	0.000000			,	0.000000			,	0.000000			,	0.135000	,	0.230000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	38
#define	REVERB_PRESET_FACTORY_LARGEROOM			{	0.428687	,	0.750000	,	0.64053195	,	0.707946	,	0.630957	,	4.240000	,	0.510000	,	1.310000		,	0.177828			,	0.039000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.023000			,	0.000000			,	0.000000			,	0.000000			,	0.231000	,	0.070000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	39
#define	REVERB_PRESET_FACTORY_HALL				{	0.428687	,	0.750000	,	0.8063814	,	0.707946	,	0.630957	,	7.430000	,	0.510000	,	1.310000		,	0.063096			,	0.073000			,	0.000000			,	0.000000			,	0.000000			,	0.891251			,	0.027000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.070000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	40
#define	REVERB_PRESET_FACTORY_CUPBOARD			{	0.307063	,	0.630000	,	0.64053195	,	0.794328	,	0.501187	,	0.490000	,	0.650000	,	1.310000		,	1.258925			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.995262			,	0.032000			,	0.000000			,	0.000000			,	0.000000			,	0.107000	,	0.070000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	41
#define	REVERB_PRESET_FACTORY_COURTYARD			{	0.307063	,	0.570000	,	0.8063814	,	0.316228	,	0.630957	,	2.320000	,	0.290000	,	0.560000		,	0.223872			,	0.140000			,	0.000000			,	0.000000			,	0.000000			,	0.398107			,	0.039000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.290000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	42
#define	REVERB_PRESET_FACTORY_SMALLROOM			{	0.364500	,	0.820000	,	0.8063814	,	0.794328	,	0.501187	,	1.720000	,	0.650000	,	1.310000		,	0.707946			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.778279			,	0.024000			,	0.000000			,	0.000000			,	0.000000			,	0.119000	,	0.070000	,	0.250000			,	0.000000			,	0.994260				,	3762.600098		,	362.500000		,	0.000000			,	1	}	// Preset Nr.:	43
#define	REVERB_PRESET_ICEPALACE_ALCOVE			{	1.000000	,	0.840000	,	0.8063814	,	0.562341	,	0.281838	,	2.760000	,	1.460000	,	0.280000		,	1.122018			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	0.891251			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.161000	,	0.090000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	44
#define	REVERB_PRESET_ICEPALACE_SHORTPASSAGE	{	1.000000	,	0.750000	,	0.8063814	,	0.562341	,	0.281838	,	1.790000	,	1.460000	,	0.280000		,	0.501187			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.019000			,	0.000000			,	0.000000			,	0.000000			,	0.177000	,	0.090000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	45
#define	REVERB_PRESET_ICEPALACE_MEDIUMROOM		{	1.000000	,	0.870000	,	0.8063814	,	0.562341	,	0.446684	,	2.220000	,	1.530000	,	0.320000		,	0.398107			,	0.039000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.027000			,	0.000000			,	0.000000			,	0.000000			,	0.186000	,	0.120000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	46
#define	REVERB_PRESET_ICEPALACE_LONGPASSAGE		{	1.000000	,	0.770000	,	0.8063814	,	0.562341	,	0.398107	,	3.010000	,	1.460000	,	0.280000		,	0.794328			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.025000			,	0.000000			,	0.000000			,	0.000000			,	0.186000	,	0.040000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	47
#define	REVERB_PRESET_ICEPALACE_LARGEROOM		{	1.000000	,	0.810000	,	0.8063814	,	0.562341	,	0.446684	,	3.140000	,	1.530000	,	0.320000		,	0.251189			,	0.039000			,	0.000000			,	0.000000			,	0.000000			,	1.000000			,	0.027000			,	0.000000			,	0.000000			,	0.000000			,	0.214000	,	0.110000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	48
#define	REVERB_PRESET_ICEPALACE_HALL			{	1.000000	,	0.760000	,	0.8063814	,	0.446684	,	0.562341	,	5.490000	,	1.530000	,	0.380000		,	0.112202			,	0.054000			,	0.000000			,	0.000000			,	0.000000			,	0.630957			,	0.052000			,	0.000000			,	0.000000			,	0.000000			,	0.226000	,	0.110000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	49
#define	REVERB_PRESET_ICEPALACE_CUPBOARD		{	1.000000	,	0.830000	,	0.8063814	,	0.501187	,	0.223872	,	0.760000	,	1.530000	,	0.260000		,	1.122018			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.995262			,	0.016000			,	0.000000			,	0.000000			,	0.000000			,	0.143000	,	0.080000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	50
#define	REVERB_PRESET_ICEPALACE_COURTYARD		{	1.000000	,	0.590000	,	0.8063814	,	0.281838	,	0.316228	,	2.040000	,	1.200000	,	0.380000		,	0.316228			,	0.173000			,	0.000000			,	0.000000			,	0.000000			,	0.316228			,	0.043000			,	0.000000			,	0.000000			,	0.000000			,	0.235000	,	0.480000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	51
#define	REVERB_PRESET_ICEPALACE_SMALLROOM		{	1.000000	,	0.840000	,	0.8063814	,	0.562341	,	0.281838	,	1.510000	,	1.530000	,	0.270000		,	0.891251			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.164000	,	0.140000	,	0.250000			,	0.000000			,	0.994260				,	12428.500000	,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	52
#define	REVERB_PRESET_SPACESTATION_ALCOVE		{	0.210938	,	0.780000	,	0.8063814	,	0.707946	,	0.891251	,	1.160000	,	0.810000	,	0.550000		,	1.412538			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.000000			,	0.018000			,	0.000000			,	0.000000			,	0.000000			,	0.192000	,	0.210000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	53
#define	REVERB_PRESET_SPACESTATION_MEDIUMROOM	{	0.210938	,	0.750000	,	0.8063814	,	0.630957	,	0.891251	,	3.010000	,	0.500000	,	0.550000		,	0.398107			,	0.034000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.035000			,	0.000000			,	0.000000			,	0.000000			,	0.209000	,	0.310000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	54
#define	REVERB_PRESET_SPACESTATION_SHORTPASSAGE	{	0.210938	,	0.870000	,	0.8063814	,	0.630957	,	0.891251	,	3.570000	,	0.500000	,	0.550000		,	1.000000			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.016000			,	0.000000			,	0.000000			,	0.000000			,	0.172000	,	0.200000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	55
#define	REVERB_PRESET_SPACESTATION_LONGPASSAGE	{	0.428687	,	0.820000	,	0.8063814	,	0.630957	,	0.891251	,	4.620000	,	0.620000	,	0.550000		,	1.000000			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.031000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.230000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	56
#define	REVERB_PRESET_SPACESTATION_LARGEROOM	{	0.364500	,	0.810000	,	0.8063814	,	0.630957	,	0.891251	,	3.890000	,	0.380000	,	0.610000		,	0.316228			,	0.056000			,	0.000000			,	0.000000			,	0.000000			,	0.891251			,	0.035000			,	0.000000			,	0.000000			,	0.000000			,	0.233000	,	0.280000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	57
#define	REVERB_PRESET_SPACESTATION_HALL			{	0.428687	,	0.870000	,	0.8063814	,	0.630957	,	0.891251	,	7.110000	,	0.380000	,	0.610000		,	0.177828			,	0.100000			,	0.000000			,	0.000000			,	0.000000			,	0.630957			,	0.047000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.250000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	58
#define	REVERB_PRESET_SPACESTATION_CUPBOARD		{	0.171500	,	0.560000	,	0.8063814	,	0.707946	,	0.891251	,	0.790000	,	0.810000	,	0.550000		,	1.412538			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.778279			,	0.018000			,	0.000000			,	0.000000			,	0.000000			,	0.181000	,	0.310000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	59
#define	REVERB_PRESET_SPACESTATION_SMALLROOM	{	0.210938	,	0.700000	,	0.8063814	,	0.707946	,	0.891251	,	1.720000	,	0.820000	,	0.550000		,	0.794328			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.013000			,	0.000000			,	0.000000			,	0.000000			,	0.188000	,	0.260000	,	0.250000			,	0.000000			,	0.994260				,	3316.100098		,	458.200012		,	0.000000			,	1	}	// Preset Nr.:	60
#define	REVERB_PRESET_WOODEN_ALCOVE				{	1.000000	,	1.000000	,	0.8063814	,	0.125893	,	0.316228	,	1.220000	,	0.620000	,	0.910000		,	1.122018			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	0.707946			,	0.024000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	61
#define	REVERB_PRESET_WOODEN_SHORTPASSAGE		{	1.000000	,	1.000000	,	0.8063814	,	0.125893	,	0.316228	,	1.750000	,	0.500000	,	0.870000		,	0.891251			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	0.630957			,	0.024000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	62
#define	REVERB_PRESET_WOODEN_MEDIUMROOM			{	1.000000	,	1.000000	,	0.8063814	,	0.100000	,	0.281838	,	1.470000	,	0.420000	,	0.820000		,	0.891251			,	0.049000			,	0.000000			,	0.000000			,	0.000000			,	0.891251			,	0.029000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	63
#define	REVERB_PRESET_WOODEN_LONGPASSAGE		{	1.000000	,	1.000000	,	0.8063814	,	0.100000	,	0.316228	,	1.990000	,	0.400000	,	0.790000		,	1.000000			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.446684			,	0.036000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	64
#define	REVERB_PRESET_WOODEN_LARGEROOM			{	1.000000	,	1.000000	,	0.8063814	,	0.089125	,	0.281838	,	2.650000	,	0.330000	,	0.820000		,	0.891251			,	0.066000			,	0.000000			,	0.000000			,	0.000000			,	0.794328			,	0.049000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	65
#define	REVERB_PRESET_WOODEN_HALL				{	1.000000	,	1.000000	,	0.8063814	,	0.079433	,	0.281838	,	3.450000	,	0.300000	,	0.820000		,	0.891251			,	0.088000			,	0.000000			,	0.000000			,	0.000000			,	0.794328			,	0.063000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	66
#define	REVERB_PRESET_WOODEN_CUPBOARD			{	1.000000	,	1.000000	,	0.8063814	,	0.141254	,	0.316228	,	0.560000	,	0.460000	,	0.910000		,	1.122018			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.028000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	67
#define	REVERB_PRESET_WOODEN_SMALLROOM			{	1.000000	,	1.000000	,	0.8063814	,	0.112202	,	0.316228	,	0.790000	,	0.320000	,	0.870000		,	1.000000			,	0.032000			,	0.000000			,	0.000000			,	0.000000			,	0.891251			,	0.029000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	68
#define	REVERB_PRESET_WOODEN_COURTYARD			{	1.000000	,	0.650000	,	0.8063814	,	0.079433	,	0.316228	,	1.790000	,	0.350000	,	0.790000		,	0.562341			,	0.123000			,	0.000000			,	0.000000			,	0.000000			,	0.100000			,	0.032000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	4705.000000		,	99.599998		,	0.000000			,	1	}	// Preset Nr.:	69
#define	REVERB_PRESET_SPORT_EMPTYSTADIUM		{	1.000000	,	1.000000	,	0.8063814	,	0.446684	,	0.794328	,	6.260000	,	0.510000	,	1.100000		,	0.063096			,	0.183000			,	0.000000			,	0.000000			,	0.000000			,	0.398107			,	0.038000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	70
#define	REVERB_PRESET_SPORT_SQUASHCOURT			{	1.000000	,	0.750000	,	0.8063814	,	0.316228	,	0.794328	,	2.220000	,	0.910000	,	1.160000		,	0.446684			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	0.794328			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.126000	,	0.190000	,	0.250000			,	0.000000			,	0.994260				,	7176.899902		,	211.199997		,	0.000000			,	1	}	// Preset Nr.:	71
#define	REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL	{	1.000000	,	0.700000	,	0.8063814	,	0.794328	,	0.891251	,	2.760000	,	1.250000	,	1.140000		,	0.630957			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.794328			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.179000	,	0.150000	,	0.895000			,	0.190000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	72
#define	REVERB_PRESET_SPORT_LARGESWIMMINGPOOL	{	1.000000	,	0.820000	,	0.8063814	,	0.794328	,	1.000000	,	5.490000	,	1.310000	,	1.140000		,	0.446684			,	0.039000			,	0.000000			,	0.000000			,	0.000000			,	0.501187			,	0.049000			,	0.000000			,	0.000000			,	0.000000			,	0.222000	,	0.550000	,	1.159000			,	0.210000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	73
#define	REVERB_PRESET_SPORT_GYMNASIUM			{	1.000000	,	0.810000	,	0.8063814	,	0.446684	,	0.891251	,	3.140000	,	1.060000	,	1.350000		,	0.398107			,	0.029000			,	0.000000			,	0.000000			,	0.000000			,	0.562341			,	0.045000			,	0.000000			,	0.000000			,	0.000000			,	0.146000	,	0.140000	,	0.250000			,	0.000000			,	0.994260				,	7176.899902		,	211.199997		,	0.000000			,	1	}	// Preset Nr.:	74
#define	REVERB_PRESET_SPORT_FULLSTADIUM			{	1.000000	,	1.000000	,	0.8063814	,	0.070795	,	0.794328	,	5.250000	,	0.170000	,	0.800000		,	0.100000			,	0.188000			,	0.000000			,	0.000000			,	0.000000			,	0.281838			,	0.038000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	75
#define	REVERB_PRESET_SPORT_STADIUMTANNOY		{	1.000000	,	0.780000	,	0.8063814	,	0.562341	,	0.501187	,	2.530000	,	0.880000	,	0.680000		,	0.281838			,	0.230000			,	0.000000			,	0.000000			,	0.000000			,	0.501187			,	0.063000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.200000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	76
#define	REVERB_PRESET_PREFAB_WORKSHOP			{	0.428687	,	1.000000	,	0.8063814	,	0.141254	,	0.398107	,	0.760000	,	1.000000	,	1.000000		,	1.000000			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	77
#define	REVERB_PRESET_PREFAB_SCHOOLROOM			{	0.402178	,	0.690000	,	0.8063814	,	0.630957	,	0.501187	,	0.980000	,	0.450000	,	0.180000		,	1.412538			,	0.017000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.015000			,	0.000000			,	0.000000			,	0.000000			,	0.095000	,	0.140000	,	0.250000			,	0.000000			,	0.994260				,	7176.899902		,	211.199997		,	0.000000			,	1	}	// Preset Nr.:	78
#define	REVERB_PRESET_PREFAB_PRACTISEROOM		{	0.402178	,	0.870000	,	0.8063814	,	0.398107	,	0.501187	,	1.120000	,	0.560000	,	0.180000		,	1.258925			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.095000	,	0.140000	,	0.250000			,	0.000000			,	0.994260				,	7176.899902		,	211.199997		,	0.000000			,	1	}	// Preset Nr.:	79
#define	REVERB_PRESET_PREFAB_OUTHOUSE			{	1.000000	,	0.820000	,	0.8063814	,	0.112202	,	0.158489	,	1.380000	,	0.380000	,	0.350000		,	0.891251			,	0.024000			,	0.000000			,	0.000000			,	-0.000000			,	0.630957			,	0.044000			,	0.000000			,	0.000000			,	0.000000			,	0.121000	,	0.170000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	107.500000		,	0.000000			,	0	}	// Preset Nr.:	80
#define	REVERB_PRESET_PREFAB_CARAVAN			{	1.000000	,	1.000000	,	0.8063814	,	0.089125	,	0.125893	,	0.430000	,	1.500000	,	1.000000		,	1.000000			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	1.995262			,	0.012000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	81
#define	REVERB_PRESET_DOME_TOMB					{	1.000000	,	0.790000	,	0.8063814	,	0.354813	,	0.223872	,	4.180000	,	0.210000	,	0.100000		,	0.386812			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	1.678804			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	0.177000	,	0.190000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	20.000000		,	0.000000			,	0	}	// Preset Nr.:	82
#define	REVERB_PRESET_PIPE_SMALL				{	1.000000	,	1.000000	,	0.8063814	,	0.354813	,	0.223872	,	5.040000	,	0.100000	,	0.100000		,	0.501187			,	0.032000			,	0.000000			,	0.000000			,	0.000000			,	2.511886			,	0.015000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	20.000000		,	0.000000			,	1	}	// Preset Nr.:	83
#define	REVERB_PRESET_DOME_SAINTPAULS			{	1.000000	,	0.870000	,	0.8063814	,	0.354813	,	0.223872	,	10.480000	,	0.190000	,	0.100000		,	0.177828			,	0.090000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.042000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.120000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	20.000000		,	0.000000			,	1	}	// Preset Nr.:	84
#define	REVERB_PRESET_PIPE_LONGTHIN				{	0.256000	,	0.910000	,	0.8063814	,	0.446684	,	0.281838	,	9.210000	,	0.180000	,	0.100000		,	0.707946			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	0.707946			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	20.000000		,	0.000000			,	0	}	// Preset Nr.:	85
#define	REVERB_PRESET_PIPE_LARGE				{	1.000000	,	1.000000	,	0.8063814	,	0.354813	,	0.223872	,	8.450000	,	0.100000	,	0.100000		,	0.398107			,	0.046000			,	0.000000			,	0.000000			,	0.000000			,	1.584893			,	0.032000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	20.000000		,	0.000000			,	1	}	// Preset Nr.:	86
#define	REVERB_PRESET_PIPE_RESONANT				{	0.137312	,	0.910000	,	0.8063814	,	0.446684	,	0.281838	,	6.810000	,	0.180000	,	0.100000		,	0.707946			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.000000			,	0.022000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	20.000000		,	0.000000			,	0	}	// Preset Nr.:	87
#define	REVERB_PRESET_OUTDOORS_BACKYARD			{	1.000000	,	0.450000	,	0.8063814	,	0.251189	,	0.501187	,	1.120000	,	0.340000	,	0.460000		,	0.446684			,	0.069000			,	0.000000			,	0.000000			,	-0.000000			,	0.707946			,	0.023000			,	0.000000			,	0.000000			,	0.000000			,	0.218000	,	0.340000	,	0.250000			,	0.000000			,	0.994260				,	4399.100098		,	242.899994		,	0.000000			,	0	}	// Preset Nr.:	88
#define	REVERB_PRESET_OUTDOORS_ROLLINGPLAINS	{	1.000000	,	0.000000	,	0.8063814	,	0.011220	,	0.630957	,	2.130000	,	0.210000	,	0.460000		,	0.177828			,	0.300000			,	0.000000			,	0.000000			,	-0.000000			,	0.446684			,	0.019000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	1.000000	,	0.250000			,	0.000000			,	0.994260				,	4399.100098		,	242.899994		,	0.000000			,	0	}	// Preset Nr.:	89
#define	REVERB_PRESET_OUTDOORS_DEEPCANYON		{	1.000000	,	0.740000	,	0.8063814	,	0.177828	,	0.630957	,	3.890000	,	0.210000	,	0.460000		,	0.316228			,	0.223000			,	0.000000			,	0.000000			,	-0.000000			,	0.354813			,	0.019000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	1.000000	,	0.250000			,	0.000000			,	0.994260				,	4399.100098		,	242.899994		,	0.000000			,	0	}	// Preset Nr.:	90
#define	REVERB_PRESET_OUTDOORS_CREEK			{	1.000000	,	0.350000	,	0.8063814	,	0.177828	,	0.501187	,	2.130000	,	0.210000	,	0.460000		,	0.398107			,	0.115000			,	0.000000			,	0.000000			,	-0.000000			,	0.199526			,	0.031000			,	0.000000			,	0.000000			,	0.000000			,	0.218000	,	0.340000	,	0.250000			,	0.000000			,	0.994260				,	4399.100098		,	242.899994		,	0.000000			,	0	}	// Preset Nr.:	91
#define	REVERB_PRESET_OUTDOORS_VALLEY			{	1.000000	,	0.280000	,	0.8063814	,	0.028184	,	0.158489	,	2.880000	,	0.260000	,	0.350000		,	0.141254			,	0.263000			,	0.000000			,	0.000000			,	-0.000000			,	0.398107			,	0.100000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.340000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	107.500000		,	0.000000			,	0	}	// Preset Nr.:	92
#define	REVERB_PRESET_MOOD_HEAVEN				{	1.000000	,	0.940000	,	0.8063814	,	0.794328	,	0.446684	,	5.040000	,	1.120000	,	0.560000		,	0.242661			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.029000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.080000	,	2.742000			,	0.050000			,	0.997700				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	93
#define	REVERB_PRESET_MOOD_HELL					{	1.000000	,	0.570000	,	0.8063814	,	0.354813	,	0.446684	,	3.570000	,	0.490000	,	2.000000		,	0.000000			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.110000	,	0.040000	,	2.109000			,	0.520000			,	0.994260				,	5000.000000		,	139.500000		,	0.000000			,	0	}	// Preset Nr.:	94
#define	REVERB_PRESET_MOOD_MEMORY				{	1.000000	,	0.850000	,	0.8063814	,	0.630957	,	0.354813	,	4.060000	,	0.820000	,	0.560000		,	0.039811			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	1.122018			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.474000			,	0.450000			,	0.988553				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	95
#define	REVERB_PRESET_DRIVING_COMMENTATOR		{	1.000000	,	0.000000	,	0.8063814	,	0.562341	,	0.501187	,	2.420000	,	0.880000	,	0.680000		,	0.199526			,	0.093000			,	0.000000			,	0.000000			,	0.000000			,	0.251189			,	0.017000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	1.000000	,	0.250000			,	0.000000			,	0.988553				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	96
#define	REVERB_PRESET_DRIVING_PITGARAGE			{	0.428687	,	0.590000	,	0.8063814	,	0.707946	,	0.562341	,	1.720000	,	0.930000	,	0.870000		,	0.562341			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.016000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.110000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	97
#define	REVERB_PRESET_DRIVING_INCAR_RACER		{	0.083188	,	0.800000	,	0.8063814	,	1.000000	,	0.794328	,	0.170000	,	2.000000	,	0.410000		,	1.778279			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	0.707946			,	0.015000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	10268.200195	,	251.000000		,	0.000000			,	1	}	// Preset Nr.:	98
#define	REVERB_PRESET_DRIVING_INCAR_SPORTS		{	0.083188	,	0.800000	,	0.8063814	,	0.630957	,	1.000000	,	0.170000	,	0.750000	,	0.410000		,	1.000000			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	0.562341			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	10268.200195	,	251.000000		,	0.000000			,	1	}	// Preset Nr.:	99
#define	REVERB_PRESET_DRIVING_INCAR_LUXURY		{	0.256000	,	1.000000	,	0.8063814	,	0.100000	,	0.501187	,	0.130000	,	0.410000	,	0.460000		,	0.794328			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	1.584893			,	0.010000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	10268.200195	,	251.000000		,	0.000000			,	1	}	// Preset Nr.:	100
#define	REVERB_PRESET_DRIVING_FULLGRANDSTAND	{	1.000000	,	1.000000	,	0.8063814	,	0.281838	,	0.630957	,	3.010000	,	1.370000	,	1.280000		,	0.354813			,	0.090000			,	0.000000			,	0.000000			,	0.000000			,	0.177828			,	0.049000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	10420.200195	,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	101
#define	REVERB_PRESET_DRIVING_EMPTYGRANDSTAND	{	1.000000	,	1.000000	,	0.8063814	,	1.000000	,	0.794328	,	4.620000	,	1.750000	,	1.400000		,	0.208209			,	0.090000			,	0.000000			,	0.000000			,	0.000000			,	0.251189			,	0.049000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.000000			,	0.994260				,	10420.200195	,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	102
#define	REVERB_PRESET_DRIVING_TUNNEL			{	1.000000	,	0.810000	,	0.8063814	,	0.398107	,	0.891251	,	3.420000	,	0.940000	,	1.310000		,	0.707946			,	0.051000			,	0.000000			,	0.000000			,	0.000000			,	0.707946			,	0.047000			,	0.000000			,	0.000000			,	0.000000			,	0.214000	,	0.050000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	155.300003		,	0.000000			,	1	}	// Preset Nr.:	103
#define	REVERB_PRESET_CITY_STREETS				{	1.000000	,	0.780000	,	0.8063814	,	0.707946	,	0.891251	,	1.790000	,	1.120000	,	0.910000		,	0.281838			,	0.046000			,	0.000000			,	0.000000			,	0.000000			,	0.199526			,	0.028000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.200000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	104
#define	REVERB_PRESET_CITY_SUBWAY				{	1.000000	,	0.740000	,	0.8063814	,	0.707946	,	0.891251	,	3.010000	,	1.230000	,	0.910000		,	0.707946			,	0.046000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.028000			,	0.000000			,	0.000000			,	0.000000			,	0.125000	,	0.210000	,	0.250000			,	0.000000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	105
#define	REVERB_PRESET_CITY_MUSEUM				{	1.000000	,	0.820000	,	0.8063814	,	0.177828	,	0.177828	,	3.280000	,	1.400000	,	0.570000		,	0.251189			,	0.039000			,	0.000000			,	0.000000			,	-0.000000			,	0.891251			,	0.034000			,	0.000000			,	0.000000			,	0.000000			,	0.130000	,	0.170000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	107.500000		,	0.000000			,	0	}	// Preset Nr.:	106
#define	REVERB_PRESET_CITY_LIBRARY				{	1.000000	,	0.820000	,	0.8063814	,	0.281838	,	0.089125	,	2.760000	,	0.890000	,	0.410000		,	0.354813			,	0.029000			,	0.000000			,	0.000000			,	-0.000000			,	0.891251			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	0.130000	,	0.170000	,	0.250000			,	0.000000			,	0.994260				,	2854.399902		,	107.500000		,	0.000000			,	0	}	// Preset Nr.:	107
#define	REVERB_PRESET_CITY_UNDERPASS			{	1.000000	,	0.820000	,	0.8063814	,	0.446684	,	0.891251	,	3.570000	,	1.120000	,	0.910000		,	0.398107			,	0.059000			,	0.000000			,	0.000000			,	0.000000			,	0.891251			,	0.037000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.140000	,	0.250000			,	0.000000			,	0.991973				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	108
#define	REVERB_PRESET_CITY_ABANDONED			{	1.000000	,	0.690000	,	0.8063814	,	0.794328	,	0.891251	,	3.280000	,	1.170000	,	0.910000		,	0.446684			,	0.044000			,	0.000000			,	0.000000			,	0.000000			,	0.281838			,	0.024000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.200000	,	0.250000			,	0.000000			,	0.996552				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	109
#define	REVERB_PRESET_DUSTYROOM					{	0.364500	,	0.560000	,	0.8063814	,	0.794328	,	0.707946	,	1.790000	,	0.380000	,	0.210000		,	0.501187			,	0.002000			,	0.000000			,	0.000000			,	0.000000			,	1.258925			,	0.006000			,	0.000000			,	0.000000			,	0.000000			,	0.202000	,	0.050000	,	0.250000			,	0.000000			,	0.988553				,	13046.000000	,	163.300003		,	0.000000			,	1	}	// Preset Nr.:	110
#define	REVERB_PRESET_CHAPEL					{	1.000000	,	0.840000	,	0.8063814	,	0.562341	,	1.000000	,	4.620000	,	0.640000	,	1.230000		,	0.446684			,	0.032000			,	0.000000			,	0.000000			,	0.000000			,	0.794328			,	0.049000			,	0.000000			,	0.000000			,	0.000000			,	0.250000	,	0.000000	,	0.250000			,	0.110000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	111
#define	REVERB_PRESET_SMALLWATERROOM			{	1.000000	,	0.700000	,	0.8063814	,	0.447713	,	1.000000	,	1.510000	,	1.250000	,	1.140000		,	0.891251			,	0.020000			,	0.000000			,	0.000000			,	0.000000			,	1.412538			,	0.030000			,	0.000000			,	0.000000			,	0.000000			,	0.179000	,	0.150000	,	0.895000			,	0.190000			,	0.991973				,	5000.000000		,	250.000000		,	0.000000			,	0	}	// Preset Nr.:	112
#define	REVERB_PRESET_UNDERSLIME				{	0.800000	,	1.000000	,	0.8063814	,	0.005000	,	1.000000	,	5.000000	,	0.100000	,	1.000000		,	0.596348			,	0.007000			,	0.000000			,	0.000000			,	0.000000			,	7.079458			,	0.011000			,	0.000000			,	0.000000			,	0.000000			,	0.100000	,	0.000000	,	1.180000			,	0.348000			,	0.994260				,	5000.000000		,	250.000000		,	0.000000			,	1	}	// Preset Nr.:	113
#define	REVERB_PRESET_NONE						{	0.000000	,	0.000000	,	0.000000	,	0.000000	,	0.000000	,	0.000000	,	0.000000	,	0.000000		,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000			,	0.000000	,	0.000000	,	0.000000			,	0.000000			,	0.000000				,	0000.000000		,	000.000000		,	0.000000			,	0	}	// Preset Nr.:	114
#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EFX-UTIL_H_INCLUDED
