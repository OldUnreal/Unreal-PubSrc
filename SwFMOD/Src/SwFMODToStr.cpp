/* ============================================================================
	SwFMODToStr.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */

// Includes.
#include "SwFMOD.h"



/* ----------------------------------------------------------------------------
	SwTimeStr.
---------------------------------------------------------------------------- */
FString SwTimeStr( UBOOL bShort )
{
	INT tYear, tMonth, tDayOfWeek, tDay, tHour, tMin, tSec, tMSec;
	appSystemTime( tYear, tMonth, tDayOfWeek, tDay, tHour, tMin, tSec, tMSec );
	FString Result = FString::Printf( TEXT("[%02d:%02d:%03d]"), tMin, tSec, tMSec );
	return Result;
}

/* ----------------------------------------------------------------------------
	SwGetBytesString
---------------------------------------------------------------------------- */
FString SwGetBytesString( int i )
{
	if( i > 0 )
		i /= 1024;

	return FString::Printf(TEXT("%i"), i);

	/*if( i < 0 )
	{
		return TEXT("NEGATIVE");
	}
	else if( i < 1024 )
	{
		return FString::Printf(TEXT("%ub"), i);
	}
	else if( i < 1048576 )
	{
		float f = i / 1024.0f;
		return FString::Printf(TEXT("%.03fkb"), f);
	}
	else if( i < 1073741824 )
	{
		float f = i / 1048576.0f;
		return FString::Printf(TEXT("%.03fmb"), f);
	}*/
}


/* ----------------------------------------------------------------------------
	ToStr :: UObject
---------------------------------------------------------------------------- */
FString ToStr( UObject* p, UBOOL bShort )
{
	FString s = SwGetName(p);
	if( p == NULL || bShort )
		return s;

	s += SWF_SEP + FString::Printf(TEXT("%ls"),p->GetFullName());
	s += SWF_SEP + FString::Printf(TEXT("%.08x"),p->GetFlags());
	return s;
}


/* ----------------------------------------------------------------------------
	ToStr :: AActor
---------------------------------------------------------------------------- */
FString ToStr( AActor* p, UBOOL bShort )
{
	FString s = SwGetName(p);
	if( p == NULL || bShort )
		return s;

	s += SWF_SEP + FString::Printf(TEXT("%.08x"),p->GetFlags());
	s += SWF_SEP + FString::Printf(TEXT("%d%d%d"),p->Role,p->RemoteRole,p->Level->NetMode);

	return s;
}

/* ----------------------------------------------------------------------------
	ToStr :: ETravelType
---------------------------------------------------------------------------- */
FString ToStr( ETravelType a, UBOOL bShort )
{
	switch( a )
	{
	case TRAVEL_Absolute: return FString(TEXT("ABSOLUTE"));
	case TRAVEL_Partial: return FString(TEXT("PARTIAL"));
	case TRAVEL_Relative: return FString(TEXT("RELATIVE"));
	default: return FString(TEXT("UNKNOWN"));
	}
}


/* ----------------------------------------------------------------------------
	ToStr :: ENetRole
---------------------------------------------------------------------------- */
FString ToStr( ENetRole a, UBOOL bShort )
{
	switch( a )
	{
	case ROLE_None: return FString(TEXT("ROLE_None"));
	case ROLE_DumbProxy: return FString(TEXT("ROLE_DumbProxy"));
	case ROLE_SimulatedProxy: return FString(TEXT("ROLE_SimulatedProxy"));
	case ROLE_AutonomousProxy: return FString(TEXT("ROLE_AutonomousProxy"));
	case ROLE_Authority: return FString(TEXT("ROLE_Authority"));
	default: return FString(TEXT("UNKNOWN"));
	}
}

/* ----------------------------------------------------------------------------
	ToStr :: FMemStack
---------------------------------------------------------------------------- */
FString ToStr( FMemStack* p, UBOOL bShort )
{
	return FString::Printf(TEXT("MS:%p %d"), p, p->GetByteCount());
}

/* ----------------------------------------------------------------------------
	ToStr :: FCheckResult
---------------------------------------------------------------------------- */
FString ToStr( FCheckResult* c, UBOOL bShort )
{
	FString S;
	int i = 0;

	while( c )
	{
		FString aname;
		if( c->Actor )
			aname = c->Actor->GetFullName();

		FString pname;
		if( c->Primitive )
			pname = c->Primitive->GetFullName();

		S += FString::Printf( TEXT("FCR[%d]< P[%ls] L[%ls] N[%ls] T[%f] I[%d] A[%ls] N[%p] > ")
		, i
		, *pname
		, *ToStr(c->Location)
		, *ToStr(c->Normal)
		, c->Time
		, c->Item
		, *aname
		, c->Next
		);


		i++;
		c = c->GetNext();
	}

	return S;
}

/* ----------------------------------------------------------------------------
	ToStr :: ULevel
---------------------------------------------------------------------------- */
FString ToStr( ULevel* p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("LV:%p"), p);
	if( p == NULL || bShort )
		return s;

	s += SWF_SEP + FString::Printf(TEXT("%ls"),p->GetFullName());
	return s;
}


/* ----------------------------------------------------------------------------
	ToStr :: FFileManager
---------------------------------------------------------------------------- */
FString ToStr( FFileManager* p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("FM:%p"), p);
	if( p == NULL || bShort )
		return s;
	return s;
}

/* ----------------------------------------------------------------------------
	ToStr :: USound
---------------------------------------------------------------------------- */
FString ToStr( FSoundData p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("%d"), p.Num());
	return s;
}


/* ----------------------------------------------------------------------------
	ToStr :: USound
---------------------------------------------------------------------------- */
FString ToStr( USound* p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("SO:%p"), p);
	if( p == NULL || bShort )
		return s;

	s += SWF_SEP + p->GetName();
	s += SWF_SEP + *ToStr(p->FileType);
	s += SWF_SEP + *ToStr(p->OriginalSize);
	s += SWF_SEP + *ToStr(p->Duration);
	s += SWF_SEP + FString::Printf(TEXT("%p"), p->Handle);
	//if( p->Handle )
	//	s += SWF_SEP + *ToStr(p->Data);

	return s;
}

/* ----------------------------------------------------------------------------
	ToStr :: UMusic
---------------------------------------------------------------------------- */
FString ToStr( UMusic* p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("MU:%p"), p);
	if( p == NULL || bShort )
		return s;

	s += SWF_SEP + p->GetName();
	//s += SWF_SEP + *ToStr(p->FileType);

	return s;
}

/* ----------------------------------------------------------------------------
	ToStr :: FCoords
---------------------------------------------------------------------------- */
FString ToStr( const FCoords p, UBOOL bShort )
{
	FString s = *ToStr(p.Origin);
	if( bShort )
		return s;

	s += SWF_SEP + *ToStr(p.XAxis);
	s += SWF_SEP + *ToStr(p.YAxis);
	s += SWF_SEP + *ToStr(p.ZAxis);
	return s;
}


/* ----------------------------------------------------------------------------
	ToStr :: FPointRegion
---------------------------------------------------------------------------- */
FString ToStr( const FPointRegion p, UBOOL bShort )
{
	FString s = *ToStr(p.Zone);
	if( bShort )
		return s;

	s += SWF_SEP + *ToStr(p.iLeaf);
	s += SWF_SEP + *ToStr(p.ZoneNumber);
	return s;
}

/* ----------------------------------------------------------------------------
	ToStr :: FSwSoundId
---------------------------------------------------------------------------- */
FString ToStr( const FSwSoundId p, UBOOL bShort )
{
	if( bShort )
		return FString::Printf( TEXT("%u"), p.Id);
	else
		return FString::Printf( TEXT("%u:%u:%u:%ls"), p.Id, int(p.GetSlot()), int(p.GetNoOverride()), *ToStr(p.GetActor()) );
}

/* ----------------------------------------------------------------------------
	ToStr :: FSwVersion
---------------------------------------------------------------------------- */
FString ToStr( const FSwVersion p, UBOOL bShort )
{
	return FString::Printf( TEXT("%u.%u.%u"), p.GetMajor(), p.GetMinor(), p.GetDev() );
}


/* ----------------------------------------------------------------------------
	ToStr :: FMOD_CAPS
---------------------------------------------------------------------------- */
FString FMOD_CAPSToStr( const FMOD_CAPS p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("%.08x"), p);
	if( bShort )
		return s;

	if( HasFlag( p, FMOD_CAPS_HARDWARE ) ) s += FString(appSpc(1)) + TEXT("HARDWARE");
	if( HasFlag( p, FMOD_CAPS_HARDWARE_EMULATED ) ) s += FString(appSpc(1)) + TEXT("HARDWARE_EMULATED");
	if( HasFlag( p, FMOD_CAPS_OUTPUT_MULTICHANNEL ) ) s += FString(appSpc(1)) + TEXT("MULTICHANNEL");
	if( HasFlag( p, FMOD_CAPS_OUTPUT_FORMAT_PCM8 ) ) s += FString(appSpc(1)) + TEXT("PCM8");
	if( HasFlag( p, FMOD_CAPS_OUTPUT_FORMAT_PCM16 ) ) s += FString(appSpc(1)) + TEXT("PCM16");
	if( HasFlag( p, FMOD_CAPS_OUTPUT_FORMAT_PCM24 ) ) s += FString(appSpc(1)) + TEXT("PC24");
	if( HasFlag( p, FMOD_CAPS_OUTPUT_FORMAT_PCM32 ) ) s += FString(appSpc(1)) + TEXT("PCM32");
	if( HasFlag( p, FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT ) ) s += FString(appSpc(1)) + TEXT("PCMFLOAT");
	/*if( HasFlag( p, FMOD_CAPS_REVERB_EAX2 ) ) s += FString(appSpc(1)) + TEXT("EAX2");
	if( HasFlag( p, FMOD_CAPS_REVERB_EAX3 ) ) s += FString(appSpc(1)) + TEXT("EAX3");
	if( HasFlag( p, FMOD_CAPS_REVERB_EAX4 ) ) s += FString(appSpc(1)) + TEXT("EAX4");
	if( HasFlag( p, FMOD_CAPS_REVERB_EAX5 ) ) s += FString(appSpc(1)) + TEXT("EAX5");
	if( HasFlag( p, FMOD_CAPS_REVERB_I3DL2 ) ) s += FString(appSpc(1)) + TEXT("I3DL2");*/
	if( HasFlag( p, FMOD_CAPS_REVERB_LIMITED ) ) s += FString(appSpc(1)) + TEXT("REVERB");

	return s;
}


/* ----------------------------------------------------------------------------
	ToStr :: FMOD_SPEAKERMODE
---------------------------------------------------------------------------- */
FString FMOD_SPEAKERMODEToStr( const FMOD_SPEAKERMODE p, UBOOL bShort )
{
	FString s = FString::Printf(TEXT("%.08x"), p);
	if( bShort )
		return s;

	switch( p )
	{
	case FMOD_SPEAKERMODE_RAW:			s += FString(appSpc(1)) + TEXT("RAW"); break;
	case FMOD_SPEAKERMODE_MONO:			s += FString(appSpc(1)) + TEXT("MONO"); break;
	case FMOD_SPEAKERMODE_STEREO:		s += FString(appSpc(1)) + TEXT("STEREO"); break;
	case FMOD_SPEAKERMODE_QUAD:			s += FString(appSpc(1)) + TEXT("QUAD"); break;
	case FMOD_SPEAKERMODE_SURROUND:		s += FString(appSpc(1)) + TEXT("SURROUND"); break;
	case FMOD_SPEAKERMODE_5POINT1:		s += FString(appSpc(1)) + TEXT("5POINT1"); break;
	case FMOD_SPEAKERMODE_7POINT1:		s += FString(appSpc(1)) + TEXT("7POINT1"); break;
	case FMOD_SPEAKERMODE_SRS5_1_MATRIX: s += FString(appSpc(1)) + TEXT("SRS5_1_MATRIX"); break;
	case FMOD_SPEAKERMODE_MYEARS:		s += FString(appSpc(1)) + TEXT("MYEARS"); break;
	default:							s += FString(appSpc(1)) + TEXT("UNKNOWN"); break;
	}

	return s;
}

/* ----------------------------------------------------------------------------
	ToStr :: FMOD_ADVANCEDSETTINGS
---------------------------------------------------------------------------- */
FString ToStr( const FMOD_ADVANCEDSETTINGS p, UBOOL bShort )
{
	FString s;

	s += TEXT("<");

	s += SWF_STSTR(p.maxMPEGcodecs);
	s += SWF_STSTR(p.maxADPCMcodecs);
	s += SWF_STSTR(p.maxXMAcodecs);
	s += SWF_STSTR(p.maxMPEGcodecs);
//	s += SWF_STSTR(p.ASIONumChannels);
//	s += SWF_STSTR(p.ASIOChannelList);
//	s += SWF_STSTR(p.ASIOSpeakerList);
	s += SWF_STSTR(p.max3DReverbDSPs);
	s += SWF_STSTR(p.HRTFMinAngle);
	s += SWF_STSTR(p.HRTFMaxAngle);
	s += SWF_STSTR(p.HRTFFreq);
	s += SWF_STSTR(p.vol0virtualvol);
	s += TEXT(">");

	return s;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
