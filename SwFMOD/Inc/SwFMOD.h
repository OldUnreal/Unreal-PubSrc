/* ============================================================================
	SwFMOD.h:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
===============================================================================
 - Keep in mind alignment: UE uses 4 bytes, FMOD 8 bytes.
 - NAME_DevSound log is suppressed by defualt
============================================================================ */
#ifndef SWFMOD_H
#define SWFMOD_H


// FMOD Includes.
#pragma pack(push)
#pragma pack(8)
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma pack(pop)


// Unreal Includes.
#include "Engine.h"
#include "UnRender.h"


// Setup
//#define SWFMOD_API DLL_EXPORT
#ifndef SWFMOD_API
#define SWFMOD_API DLL_IMPORT
#endif

#ifndef NAMES_ONLY
#define AUTOGENERATE_NAME(name) extern SWFMOD_API FName SWFMOD_##name;
#define AUTOGENERATE_FUNCTION(cls,idx,name)
#endif

// Globals
extern SWFMOD_API FName SWFMOD_SwFMOD;
extern SWFMOD_API FName SWFMOD_SwFMOD_WARNING;
extern SWFMOD_API FLOAT GUUToMeters,GUUToUEMeters;
extern SWFMOD_API DWORD GSwfErrorCount;

//extern char *FMOD_ErrorString(FMOD_RESULT errcode);

#define SWF_NAME			(EName)SWFMOD_SwFMOD.GetIndex()
#define SWF_NAME_WARNING	(EName)SWFMOD_SwFMOD_WARNING.GetIndex()


// Forward declarations
struct FSwSoundId;
struct FSwVersion;

// Utils
#include "SwFMODMacro.h"
#include "SwFMODToStr.h"
#include "SwFMODUtil.h"

// More EFX presets from ALAudio
#define EAXREVERB_PRESET_DRUGGED					{0,	23,	0.500f,	-1000,	0,	0,	8.39f,	1.39f,	1.00f,	-115,	0.002f,	985,	0.030f,	0.250f,	1.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_DIZZY						{0,	24,	0.600f,	-1000,	-400,	0,	17.23f,	0.56f,	1.00f,	-1713,	0.020f,	-613,	0.030f,	0.810f,	0.310f,	5000.0f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_PSYCHOTIC					{0,	25,	0.500f,	-1000,	-151,	0,	7.56f,	0.91f,	1.00f,	-626,	0.020f,	774,	0.030f,	4.000f,	1.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_CASTLE_SMALLROOM			{0,	26,	0.890f,	-1000,	-800,	-2000,	1.22f,	0.83f,	0.31f,	-100,	0.022f,	600,	0.011f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_SHORTPASSAGE		{0,	26,	0.890f,	-1000,	-1000,	-2000,	2.32f,	0.83f,	0.31f,	-100,	0.007f,	200,	0.023f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_MEDIUMROOM			{0,	26,	0.930f,	-1000,	-1100,	-2000,	2.04f,	0.83f,	0.46f,	-400,	0.022f,	400,	0.011f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_LONGPASSAGE			{0,	26,	0.890f,	-1000,	-800,	-2000,	3.42f,	0.83f,	0.31f,	-100,	0.007f,	300,	0.023f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_LARGEROOM			{0,	26,	0.820f,	-1000,	-1100,	-1800,	2.53f,	0.83f,	0.50f,	-700,	0.034f,	200,	0.016f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_HALL				{0,	26,	0.810f,	-1000,	-1100,	-1500,	3.14f,	0.79f,	0.62f,	-1500,	0.056f,	100,	0.024f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_CUPBOARD			{0,	26,	0.890f,	-1000,	-1100,	-2000,	0.67f,	0.87f,	0.31f,	300,	0.010f,	1100,	0.007f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CASTLE_COURTYARD			{0,	26,	0.420f,	-1000,	-700,	-1400,	2.13f,	0.61f,	0.23f,	-1300,	0.160f,	-300,	0.036f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_CASTLE_ALCOVE				{0,	26,	0.890f,	-1000,	-600,	-2000,	1.64f,	0.87f,	0.31f,	0,	0.007f,	300,	0.034f,	0.250f,	0.000f,	5168.6f,	139.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_ALCOVE				{0,	26,	0.590f,	-1200,	-200,	-600,	3.14f,	0.65f,	1.31f,	300,	0.010f,	0,	0.038f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_SHORTPASSAGE		{0,	26,	0.640f,	-1200,	-200,	-600,	2.53f,	0.65f,	1.31f,	0,	0.010f,	200,	0.038f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_MEDIUMROOM			{0,	26,	0.820f,	-1200,	-200,	-600,	2.76f,	0.65f,	1.31f,	-1100,	0.022f,	300,	0.023f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_LONGPASSAGE		{0,	26,	0.640f,	-1200,	-200,	-600,	4.06f,	0.65f,	1.31f,	0,	0.020f,	200,	0.037f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_LARGEROOM			{0,	26,	0.750f,	-1200,	-300,	-400,	4.24f,	0.51f,	1.31f,	-1500,	0.039f,	100,	0.023f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_HALL				{0,	26,	0.750f,	-1000,	-300,	-400,	7.43f,	0.51f,	1.31f,	-2400,	0.073f,	-100,	0.027f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_CUPBOARD			{0,	26,	0.630f,	-1200,	-200,	-600,	0.49f,	0.65f,	1.31f,	200,	0.010f,	600,	0.032f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_COURTYARD			{0,	26,	0.570f,	-1000,	-1000,	-400,	2.32f,	0.29f,	0.56f,	-1300,	0.140f,	-800,	0.039f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_FACTORY_SMALLROOM			{0,	26,	0.820f,	-1000,	-200,	-600,	1.72f,	0.65f,	1.31f,	-300,	0.010f,	500,	0.024f,	0.250f,	0.000f,	3762.6f,	362.5f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_ALCOVE			{0,	26,	0.840f,	-1000,	-500,	-1100,	2.76f,	1.46f,	0.28f,	100,	0.010f,	-100,	0.030f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_SHORTPASSAGE		{0,	26,	0.750f,	-1000,	-500,	-1100,	1.79f,	1.46f,	0.28f,	-600,	0.010f,	100,	0.019f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_MEDIUMROOM		{0,	26,	0.870f,	-1000,	-500,	-700,	2.22f,	1.53f,	0.32f,	-800,	0.039f,	100,	0.027f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_LONGPASSAGE		{0,	26,	0.770f,	-1000,	-500,	-800,	3.01f,	1.46f,	0.28f,	-200,	0.012f,	200,	0.025f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_LARGEROOM		{0,	26,	0.810f,	-1000,	-500,	-700,	3.14f,	1.53f,	0.32f,	-1200,	0.039f,	0,	0.027f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_HALL				{0,	26,	0.760f,	-1000,	-700,	-500,	5.49f,	1.53f,	0.38f,	-1900,	0.054f,	-400,	0.052f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_CUPBOARD			{0,	26,	0.830f,	-1000,	-600,	-1300,	0.76f,	1.53f,	0.26f,	100,	0.012f,	600,	0.016f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_COURTYARD		{0,	26,	0.590f,	-1000,	-1100,	-1000,	2.04f,	1.20f,	0.38f,	-1000,	0.173f,	-1000,	0.043f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_ICEPALACE_SMALLROOM		{0,	26,	0.840f,	-1000,	-500,	-1100,	1.51f,	1.53f,	0.27f,	-100,	0.010f,	300,	0.011f,	0.250f,	0.000f,	12428.5f,	99.6f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_ALCOVE		{0,	26,	0.780f,	-1000,	-300,	-100,	1.16f,	0.81f,	0.55f,	300,	0.007f,	0,	0.018f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_MEDIUMROOM	{0,	26,	0.750f,	-1000,	-400,	-100,	3.01f,	0.50f,	0.55f,	-800,	0.034f,	100,	0.035f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_SHORTPASSAGE	{0,	26,	0.870f,	-1000,	-400,	-100,	3.57f,	0.50f,	0.55f,	0,	0.012f,	100,	0.016f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_LONGPASSAGE	{0,	26,	0.820f,	-1000,	-400,	-100,	4.62f,	0.62f,	0.55f,	0,	0.012f,	200,	0.031f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_LARGEROOM		{0,	26,	0.810f,	-1000,	-400,	-100,	3.89f,	0.38f,	0.61f,	-1000,	0.056f,	-100,	0.035f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_HALL			{0,	26,	0.870f,	-1000,	-400,	-100,	7.11f,	0.38f,	0.61f,	-1500,	0.100f,	-400,	0.047f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_CUPBOARD		{0,	26,	0.560f,	-1000,	-300,	-100,	0.79f,	0.81f,	0.55f,	300,	0.007f,	500,	0.018f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPACESTATION_SMALLROOM		{0,	26,	0.700f,	-1000,	-300,	-100,	1.72f,	0.82f,	0.55f,	-200,	0.007f,	300,	0.013f,	0.250f,	0.000f,	3316.1f,	458.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_WOODEN_ALCOVE				{0,	26,	1.000f,	-1000,	-1800,	-1000,	1.22f,	0.62f,	0.91f,	100,	0.012f,	-300,	0.024f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_SHORTPASSAGE		{0,	26,	1.000f,	-1000,	-1800,	-1000,	1.75f,	0.50f,	0.87f,	-100,	0.012f,	-400,	0.024f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_MEDIUMROOM			{0,	26,	1.000f,	-1000,	-2000,	-1100,	1.47f,	0.42f,	0.82f,	-100,	0.049f,	-100,	0.029f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_LONGPASSAGE			{0,	26,	1.000f,	-1000,	-2000,	-1000,	1.99f,	0.40f,	0.79f,	0,	0.020f,	-700,	0.036f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_LARGEROOM			{0,	26,	1.000f,	-1000,	-2100,	-1100,	2.65f,	0.33f,	0.82f,	-100,	0.066f,	-200,	0.049f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_HALL				{0,	26,	1.000f,	-1000,	-2200,	-1100,	3.45f,	0.30f,	0.82f,	-100,	0.088f,	-200,	0.063f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_CUPBOARD			{0,	26,	1.000f,	-1000,	-1700,	-1000,	0.56f,	0.46f,	0.91f,	100,	0.012f,	100,	0.028f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_SMALLROOM			{0,	26,	1.000f,	-1000,	-1900,	-1000,	0.79f,	0.32f,	0.87f,	0,	0.032f,	-100,	0.029f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_WOODEN_COURTYARD			{0,	26,	0.650f,	-1000,	-2200,	-1000,	1.79f,	0.35f,	0.79f,	-500,	0.123f,	-2000,	0.032f,	0.250f,	0.000f,	4705.0f,	99.6f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_SPORT_EMPTYSTADIUM			{0,	26,	1.000f,	-1000,	-700,	-200,	6.26f,	0.51f,	1.10f,	-2400,	0.183f,	-800,	0.038f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPORT_SQUASHCOURT			{0,	26,	0.750f,	-1000,	-1000,	-200,	2.22f,	0.91f,	1.16f,	-700,	0.007f,	-200,	0.011f,	0.250f,	0.000f,	7176.9f,	211.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPORT_SMALLSWIMMINGPOOL	{0,	26,	0.700f,	-1000,	-200,	-100,	2.76f,	1.25f,	1.14f,	-400,	0.020f,	-200,	0.030f,	0.895f,	0.190f,	5000.0f,	250.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_SPORT_LARGESWIMMINGPOOL	{0,	26,	0.820f,	-1000,	-200,	0,	5.49f,	1.31f,	1.14f,	-700,	0.039f,	-600,	0.049f,	1.159f,	0.210f,	5000.0f,	250.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_SPORT_GYMNASIUM			{0,	26,	0.810f,	-1000,	-700,	-100,	3.14f,	1.06f,	1.35f,	-800,	0.029f,	-500,	0.045f,	0.250f,	0.000f,	7176.9f,	211.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPORT_FULLSTADIUM			{0,	26,	1.000f,	-1000,	-2300,	-200,	5.25f,	0.17f,	0.80f,	-2000,	0.188f,	-1100,	0.038f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_SPORT_STADIUMTANNOY		{0,	26,	0.780f,	-1000,	-500,	-600,	2.53f,	0.88f,	0.68f,	-1100,	0.230f,	-600,	0.063f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_PREFAB_WORKSHOP			{0,	26,	1.000f,	-1000,	-1700,	-800,	0.76f,	1.00f,	1.00f,	0,	0.012f,	100,	0.012f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_PREFAB_SCHOOLROOM			{0,	26,	0.690f,	-1000,	-400,	-600,	0.98f,	0.45f,	0.18f,	300,	0.017f,	300,	0.015f,	0.250f,	0.000f,	7176.9f,	211.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_PREFAB_PRACTISEROOM		{0,	26,	0.870f,	-1000,	-800,	-600,	1.12f,	0.56f,	0.18f,	200,	0.010f,	300,	0.011f,	0.250f,	0.000f,	7176.9f,	211.2f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_PREFAB_OUTHOUSE			{0,	26,	0.820f,	-1000,	-1900,	-1600,	1.38f,	0.38f,	0.35f,	-100,	0.024f,	-400,	0.044f,	0.250f,	0.000f,	2854.4f,	107.5f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_PREFAB_CARAVAN				{0,	26,	1.000f,	-1000,	-2100,	-1800,	0.43f,	1.50f,	1.00f,	0,	0.012f,	600,	0.012f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_DOME_TOMB					{0,	26,	0.790f,	-1000,	-900,	-1300,	4.18f,	0.21f,	0.10f,	-825,	0.030f,	450,	0.022f,	0.250f,	0.000f,	2854.4f,	20.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_PIPE_SMALL					{0,	26,	1.000f,	-1000,	-900,	-1300,	5.04f,	0.10f,	0.10f,	-600,	0.032f,	800,	0.015f,	0.250f,	0.000f,	2854.4f,	20.0f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_DOME_SAINTPAULS			{0,	26,	0.870f,	-1000,	-900,	-1300,	10.48f,	0.19f,	0.10f,	-1500,	0.090f,	200,	0.042f,	0.250f,	0.000f,	2854.4f,	20.0f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_PIPE_LONGTHIN				{0,	26,	0.910f,	-1000,	-700,	-1100,	9.21f,	0.18f,	0.10f,	-300,	0.010f,	-300,	0.022f,	0.250f,	0.000f,	2854.4f,	20.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_PIPE_LARGE					{0,	26,	1.000f,	-1000,	-900,	-1300,	8.45f,	0.10f,	0.10f,	-800,	0.046f,	400,	0.032f,	0.250f,	0.000f,	2854.4f,	20.0f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_PIPE_RESONANT				{0,	26,	0.910f,	-1000,	-700,	-1100,	6.81f,	0.18f,	0.10f,	-300,	0.010f,	0,	0.022f,	0.250f,	0.000f,	2854.4f,	20.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_OUTDOORS_BACKYARD			{0,	26,	0.450f,	-1000,	-1200,	-600,	1.12f,	0.34f,	0.46f,	-700,	0.069f,	-300,	0.023f,	0.250f,	0.000f,	4399.1f,	242.9f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_OUTDOORS_ROLLINGPLAINS		{0,	26,	0.000f,	-1000,	-3900,	-400,	2.13f,	0.21f,	0.46f,	-1500,	0.300f,	-700,	0.019f,	0.250f,	0.000f,	4399.1f,	242.9f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_OUTDOORS_DEEPCANYON		{0,	26,	0.740f,	-1000,	-1500,	-400,	3.89f,	0.21f,	0.46f,	-1000,	0.223f,	-900,	0.019f,	0.250f,	0.000f,	4399.1f,	242.9f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_OUTDOORS_CREEK				{0,	26,	0.350f,	-1000,	-1500,	-600,	2.13f,	0.21f,	0.46f,	-800,	0.115f,	-1400,	0.031f,	0.250f,	0.000f,	4399.1f,	242.9f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_OUTDOORS_VALLEY			{0,	26,	0.280f,	-1000,	-3100,	-1600,	2.88f,	0.26f,	0.35f,	-1700,	0.263f,	-800,	0.100f,	0.250f,	0.000f,	2854.4f,	107.5f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_MOOD_HEAVEN				{0,	26,	0.940f,	-1000,	-200,	-700,	5.04f,	1.12f,	0.56f,	-1230,	0.020f,	200,	0.029f,	2.742f,	0.050f,	5000.0f,	250.0f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_MOOD_HELL					{0,	26,	0.570f,	-1000,	-900,	-700,	3.57f,	0.49f,	2.00f,	-10000,	0.020f,	300,	0.030f,	2.109f,	0.520f,	5000.0f,	139.5f,	100.f,	100.f,	0x40}
#define EAXREVERB_PRESET_MOOD_MEMORY				{0,	26,	0.850f,	-1000,	-400,	-900,	4.06f,	0.82f,	0.56f,	-2800,	0.000f,	100,	0.000f,	0.474f,	0.450f,	5000.0f,	250.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_DRIVING_COMMENTATOR		{0,	26,	0.000f,	-1000,	-500,	-600,	2.42f,	0.88f,	0.68f,	-1400,	0.093f,	-1200,	0.017f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_DRIVING_PITGARAGE			{0,	26,	0.590f,	-1000,	-300,	-500,	1.72f,	0.93f,	0.87f,	-500,	0.000f,	200,	0.016f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_DRIVING_INCAR_RACER		{0,	26,	0.800f,	-1000,	0,	-200,	0.17f,	2.00f,	0.41f,	500,	0.007f,	-300,	0.015f,	0.250f,	0.000f,	10268.2f,	251.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_DRIVING_INCAR_SPORTS		{0,	26,	0.800f,	-1000,	-400,	0,	0.17f,	0.75f,	0.41f,	0,	0.010f,	-500,	0.000f,	0.250f,	0.000f,	10268.2f,	251.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_DRIVING_INCAR_LUXURY		{0,	26,	1.000f,	-1000,	-2000,	-600,	0.13f,	0.41f,	0.46f,	-200,	0.010f,	400,	0.010f,	0.250f,	0.000f,	10268.2f,	251.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_DRIVING_FULLGRANDSTAND		{0,	26,	1.000f,	-1000,	-1100,	-400,	3.01f,	1.37f,	1.28f,	-900,	0.090f,	-1500,	0.049f,	0.250f,	0.000f,	10420.2f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_DRIVING_EMPTYGRANDSTAND	{0,	26,	1.000f,	-1000,	0,	-200,	4.62f,	1.75f,	1.40f,	-1363,	0.090f,	-1200,	0.049f,	0.250f,	0.000f,	10420.2f,	250.0f,	100.f,	100.f,	0x1f}
#define EAXREVERB_PRESET_DRIVING_TUNNEL				{0,	26,	0.810f,	-1000,	-800,	-100,	3.42f,	0.94f,	1.31f,	-300,	0.051f,	-300,	0.047f,	0.250f,	0.000f,	5000.0f,	155.3f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CITY_STREETS				{0,	26,	0.780f,	-1000,	-300,	-100,	1.79f,	1.12f,	0.91f,	-1100,	0.046f,	-1400,	0.028f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CITY_SUBWAY				{0,	26,	0.740f,	-1000,	-300,	-100,	3.01f,	1.23f,	0.91f,	-300,	0.046f,	200,	0.028f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CITY_MUSEUM				{0,	26,	0.820f,	-1000,	-1500,	-1500,	3.28f,	1.40f,	0.57f,	-1200,	0.039f,	-100,	0.034f,	0.250f,	0.000f,	2854.4f,	107.5f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_CITY_LIBRARY				{0,	26,	0.820f,	-1000,	-1100,	-2100,	2.76f,	0.89f,	0.41f,	-900,	0.029f,	-100,	0.020f,	0.250f,	0.000f,	2854.4f,	107.5f,	100.f,	100.f,	0x0}
#define EAXREVERB_PRESET_CITY_UNDERPASS				{0,	26,	0.820f,	-1000,	-700,	-100,	3.57f,	1.12f,	0.91f,	-800,	0.059f,	-100,	0.037f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CITY_ABANDONED				{0,	26,	0.690f,	-1000,	-200,	-100,	3.28f,	1.17f,	0.91f,	-700,	0.044f,	-1100,	0.024f,	0.250f,	0.000f,	5000.0f,	250.0f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_DUSTYROOM					{0,	26,	0.560f,	-1000,	-200,	-300,	1.79f,	0.38f,	0.21f,	-600,	0.002f,	200,	0.006f,	0.250f,	0.000f,	13046.0f,	163.3f,	100.f,	100.f,	0x20}
#define EAXREVERB_PRESET_CHAPEL						{0,	26,	0.840f,	-1000,	-500,	0,	4.62f,	0.64f,	1.23f,	-700,	0.032f,	-200,	0.049f,	0.250f,	0.110f,	5000.0f,	250.0f,	100.f,	100.f,	0x3f}
#define EAXREVERB_PRESET_SMALLWATERROOM				{0,	26,	0.700f,	-1000,	-698,	0,	1.51f,	1.25f,	1.14f,	-100,	0.020f,	300,	0.030f,	0.895f,	0.190f,	5000.0f,	250.0f,	100.f,	100.f,	0x0}

/* ----------------------------------------------------------------------------
	USwFMOD.
-------------------------------------------------------------------------------
	Error checking rules:
	Construction: no-fail
	Destruction: no-fail
	Initialization: on failure log and return 0 to self destruct
	Other: on failure log, ignore or self destruct, never terminate the app
---------------------------------------------------------------------------- */

class SWFMOD_API USwFMOD : public UAudioSubsystem
{
	DECLARE_CLASS(USwFMOD,UAudioSubsystem,CLASS_Config, SwFMOD)

	// Configuration.
	FLOAT			VolumeMusic;
	FLOAT			VolumeSound;
	FLOAT			VolumeSpeech;
	FLOAT			AmbientFactor;
	FLOAT			AmbientHysteresis;

	BYTE			SampleRate;
	BYTE			SampleFormat;
	BYTE			SampleInterpolation;

	UBOOL			b3DCameraSounds;
	UBOOL			bLowSoundQuality;

	DWORD			Channels;
	DWORD			VirtualChannels;
	FLOAT			VirtualThreshold;

	BYTE			PriorityMusic;
	BYTE			PrioritySpeech;
	BYTE			PriorityAmbient;
	BYTE			PrioritySound;

	INT				StatGlobal;
	INT				StatChannelGroup;
	INT				StatChannels;
	INT				StatRender;
	INT				StatPositions;
	UBOOL			bLogPlugins;
	UBOOL			bPrecache;

	FLOAT			ToMeters;
	FLOAT			DopplerScale;
	FLOAT			DistanceFactor;
	FLOAT			DistanceMin;
	FLOAT			RolloffScale;

	UBOOL			bHRTF;
	FLOAT			HRTFMinAngle;
	FLOAT			HRTFMaxAngle;
	FLOAT			HRTFFreq;

	UBOOL			bOcclusion;
	UBOOL			bEmulateReverb;

	INT				OverrideOutput;
	INT				OverrideOutputChannels;
	INT				OverrideInputChannels;
	INT				OverrideDSPBufferLength;
	INT				OverrideDSPBufferCount;
	INT				OverrideInitFlags;
	INT				OverrideDebugFlags;
	INT				OverrideSpeakerMode;

	INT				Driver;
	INT				Max3D;

	AZoneInfo*		OldZone;
	UBOOL			bIsEmulatingReverb;

	// Internals.
	UBOOL			bInitialized;
	UViewport*		Viewport;
	ULevel*			ActiveLevel;

	FLOAT				MusicFade,MusicFadeTime,MusicFadeOut,MusicFadeOutTime,PendingSongVolume,MusicVolumeModifier;
	FTime				LastTime;
	UMusic*				PendingMusic;
	BYTE				PendingSongSection;
	BYTE				bSwitchMusicTrack,bNewTrackStarted;
	BYTE				CurrentEFXAmb;

	// FMOD
	FMOD::System*		System;
	FMOD::ChannelGroup* AmbientChannels;
	FMOD::ChannelGroup* EffectChannels;
	FMOD::ChannelGroup* MusicChannels;
	FMOD::ChannelGroup* CompressedChannels;
	FMOD::ChannelGroup* MasterChannel;
	FMOD::SoundGroup*	MusicGroup;
	FMOD::SoundGroup*	MasterGroup;
	FMOD::Geometry*		Geometry;
	FMOD::Channel*		Channel;
	FMOD::DSP*			DSP;
	FMOD::Sound*		Sound;
	static FMOD_REVERB_PROPERTIES EFXPresets[REVERB_MAX];

	// Static
	static const FName	SoundType_WAV;
	static const FName	SoundType_MP2;
	static const FName	SoundType_MP3;
	static const FName	SoundType_OGG;
	static const DWORD	OutputRates[];


	// Constructor.
	USwFMOD();
	void StaticConstructor();


	// UObject interface.
	virtual void Destroy();
	virtual void PostEditChange();
	virtual void ShutdownAfterError();


	// UAudioSubsystem interface.
	virtual UBOOL Init();
	virtual void SetViewport( UViewport* Viewport );
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar );
	virtual void Update( FPointRegion Region, FCoords& Listener );
	virtual void RegisterMusic( UMusic* Music );
	virtual void RegisterSound( USound* Music );
	virtual void UnregisterSound( USound* Sound );
	virtual void UnregisterMusic( UMusic* Music );
	virtual UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch );
	virtual void StopSoundId( INT Id );		// DEUS_EX CNN
	virtual void NoteDestroy( AActor* Actor );
	virtual UBOOL GetLowQualitySetting();
	virtual UViewport* GetViewport();
	virtual void RenderAudioGeometry( FSceneNode* Frame );
	virtual void PostRender( FSceneNode* Frame );
	virtual void SetInstantSoundVolume( BYTE NewVolume );		// DEUS_EX CNN - instantly sets the sound volume
	virtual void SetInstantSpeechVolume( BYTE NewVolume );		// DEUS_EX CNN - instantly sets the speech volume
	virtual void SetInstantMusicVolume( BYTE NewVolume );		// DEUS_EX CNN - instantly sets the music volume
	UBOOL ResolveDuration(USound* Sound)
	{
		RegisterSound(Sound);
		return (Sound->Duration >= 0.f);
	}

	// USwFMOD interface
	void VerifyConfig();
	UBOOL InitFMOD();
	void Shutdown();
	void OnLevelChanged(ULevel* NewLevel);

	FMOD::Sound* RegisterSoundSample( USound* Sound );
	FMOD::Sound* RegisterMusicSample( UMusic* Music );

	UBOOL PlayMusic( UMusic* music, BYTE SongSection, EMusicTransition Transition );
	UBOOL StartMusic();
	void ApplyChannelGroup( FMOD::ChannelGroup* ChannelGroup );
	void SetVolumes();

	void LoadGeometry(ULevel* Level);
	UBOOL IsObstructed( AActor* Actor );

	void DrawChannelGroup( FMOD::ChannelGroup* group, UCanvas* canvas, UFont* font, float x, float yl, float ym, DWORD detail=-1 );
	FString PrintChannel( FMOD::Channel* channel, FColor* color=NULL );

	void DumpDrivers();
	void DumpPlugins();
	void DumpSystem();
	void Flush();

	inline FMOD::Channel* GetMusicChannel()
	{
		FMOD::Channel* Res=NULL;
		for( FSwChannelEnumerator it(System,MusicChannels); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( !IsChannelValid(channel) )
				continue;

			if( !Res )
				Res = channel;
			else
			{
				// there can be only one music
				SWF_LOG( NAME_DevSound, TEXT("StopMusic %ls"), *PrintChannel(channel) );
				channel->setUserData(NULL);
				channel->stop();
			}
		}
		return Res;
	}
	inline AActor* GetViewActor() const
	{
		return Viewport->Actor->CalcCameraActor ? Viewport->Actor->CalcCameraActor : Viewport->Actor;
	}
};

#endif
/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
