/* ============================================================================
	SwFMODToStr.h:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */
#ifndef SWFMODTOSTR_H
#define SWFMODTOSTR_H


/* ----------------------------------------------------------------------------
	SwTimeStr.
---------------------------------------------------------------------------- */
extern FString SwTimeStr( UBOOL bShort=0 );
extern FString SwGetBytesString( int i );


/* ----------------------------------------------------------------------------
	SwGetName
---------------------------------------------------------------------------- */
inline FString SwGetName( UObject* p, UBOOL bShort=0 )
{
	return p ? p->GetName() : TEXT("None");
}


/* -----------------------------------------------------------------------------
	SwGetPath
---------------------------------------------------------------------------- */
inline FString SwGetPathName( UObject* p, UBOOL bShort=0 )
{
	return p ? p->GetPathName() : TEXT("None");
}


/* ----------------------------------------------------------------------------
	ToStr :: Basic
---------------------------------------------------------------------------- */
inline FString ToStr( const FString& p, UBOOL bShort=0 )		{ return p; }
inline FString ToStr( const TCHAR* p, UBOOL bShort=0 )			{ return FString(p); }
inline FString ToStr( const FName p, UBOOL bShort=0 )			{ return *p; }
inline FString ToStr( const FMOD_RESULT p, UBOOL bShort=0 )		{ return appFromAnsi(FMOD_ErrorString(p));  }
inline FString ToStr( const FMOD_VECTOR p, UBOOL bShort=0 )		{ return FString::Printf(TEXT("%f,%f,%f"), p.x, p.y, p.z); }
inline FString ToStr( const FVector p, UBOOL bShort=0 )			{ return FString::Printf(TEXT("%f,%f,%f"), p.X, p.Y, p.Z); }
inline FString ToStr( const FRotator p, UBOOL bShort=0 )		{ return FString::Printf(TEXT("%d,%d,%d"), p.Pitch, p.Yaw, p.Roll); }
inline FString ToStr( const float p, UBOOL bShort=0 )			{ return FString::Printf(TEXT("%f"), p);  }
inline FString ToStr( const unsigned int p, UBOOL bShort=0 )	{ return FString::Printf(TEXT("%u"), p);  }
inline FString ToStr( const signed int p, UBOOL bShort=0 )		{ return FString::Printf(TEXT("%i"), p);  }
inline FString ToStr( const unsigned long p, UBOOL bShort=0 )	{ return FString::Printf(TEXT("%lu"), p);  }
inline FString ToStr( const signed long p, UBOOL bShort=0 )		{ return FString::Printf(TEXT("%li"), p);  }


/* ----------------------------------------------------------------------------
	ToStr :: Complex
---------------------------------------------------------------------------- */
extern FString ToStr( UObject* p, UBOOL bShort=1 );
extern FString ToStr( AActor* p, UBOOL bShort=1 );
extern FString ToStr( ETravelType a, UBOOL bShort=0 );
extern FString ToStr( ENetRole a, UBOOL bShort=0 );
extern FString ToStr( FMemStack* p, UBOOL bShort=0 );
extern FString ToStr( FCheckResult* c, UBOOL bShort=0 );
extern FString ToStr( ULevel* p, UBOOL bShort=0 );
extern FString ToStr( FFileManager* p, UBOOL bShort=0 );
extern FString ToStr( FSoundData p, UBOOL bShort=0 );
extern FString ToStr( USound* p, UBOOL bShort=0 );
extern FString ToStr( UMusic* p, UBOOL bShort=0 );
extern FString ToStr( const FCoords p, UBOOL bShort=0 );
extern FString ToStr( const FPointRegion p, UBOOL bShort=0 );
extern FString ToStr( const FSwSoundId p, UBOOL bShort=0 );
extern FString ToStr( const FSwVersion p, UBOOL bShort=0 );

extern FString FMOD_CAPSToStr( const FMOD_CAPS p, UBOOL bShort=0 );
extern FString FMOD_SPEAKERMODEToStr( const FMOD_SPEAKERMODE p, UBOOL bShort=0 );
extern FString ToStr( const FMOD_ADVANCEDSETTINGS p, UBOOL bShort=0 );





#endif
/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
