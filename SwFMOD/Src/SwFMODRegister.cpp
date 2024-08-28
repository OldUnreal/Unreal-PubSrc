/* ============================================================================
	SwFMODRegister.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */

// TODO:: Duration

// Includes.
#include "SwFMOD.h"



void USwFMOD::RegisterSound( USound* Sound )
{
	guard(USwFMOD::RegisterSound);
	//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls]"), SWF_LOGP, *ToStr(Sound) );
	FMOD_RESULT result;

	checkSlow(Sound);
	if( !Sound->Handle )
	{
		FMOD::Sound* sound;

		// Load the data.
		TArray<BYTE>* Data = Sound->Data.GetAudio(0);
		debugfSlow( NAME_DevSound, TEXT("Register sound: %ls (%i)"), Sound->GetPathName(), Data->Num() );
		check(Data->Num()>0);

		// Sound extended data
#pragma pack(push)
#pragma pack(8)
		FMOD_CREATESOUNDEXINFO exinfo;
		appMemset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = Data->Num();
#pragma pack(pop)

		// Sound flags
		FMOD_MODE fmode = 0
			| FMOD_SOFTWARE
			| FMOD_IGNORETAGS
		//	| FMOD_LOOP_OFF
		//	| FMOD_2D
			| FMOD_OPENMEMORY
			| FMOD_CREATESAMPLE
			| FMOD_3D
			| FMOD_3D_LINEARROLLOFF
			;


		// Format-specific flags
		if( Sound->FileType == SoundType_WAV )
		{
			// WAV
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_WAV;
		}
		else if( Sound->FileType == SoundType_MP2 || Sound->FileType == SoundType_MP3 )
		{
			// MP3
			fmode |= FMOD_ACCURATETIME;
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;
		}
		else if (Sound->FileType == SoundType_OGG)
		{
			// Ogg
			fmode |= FMOD_ACCURATETIME;
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_OGGVORBIS;
		}
		else
		{
			// Unknown, try anyway
			fmode |= FMOD_ACCURATETIME;
		}

		// Create sound
		SWF_FMOD_CALL( System->createSound( static_cast<const char*>(Data->GetData()), fmode, &exinfo, &sound ));
		if( result == FMOD_OK )
		{
			// Register the sound.
			Sound->Handle = sound;
			SWF_FMOD_CALL( sound->setUserData(Sound) );

			unsigned int length;
			SWF_FMOD_CALL( sound->getLength(&length,FMOD_TIMEUNIT_MS) );
			Sound->Duration = length*0.001f;
		}
		else
		{
			//SWF_LOG_WARN( FString::Printf(TEXT("Couldn't register sound [%ls][%i][%ls]"), Sound->GetPathName(), Sound->Data.Num(), *ToStr(result)) );
			debugf(TEXT("Couldn't register sound [%ls][%i][%ls]"), Sound->GetPathName(), Data->Num(), *ToStr(result));
		}

		// Unload the data.
		Sound->Data.Unload();
	}

	unguard;
}

void USwFMOD::UnregisterSound( USound* Sound )
{
	guard(USwFMOD::UnregisterSound);
	//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls]"), SWF_LOGP, *ToStr(Sound) );
	FMOD_RESULT result;

	check(Sound);
	if( Sound->Handle )
	{
		debugfSlow( NAME_DevSound, TEXT("Unregister sound: %ls"), Sound->GetFullName() );

		// Stop this sound.
		FMOD::Sound* sound = GetSoundSample(Sound);
		if( sound )
		{
			SWF_FMOD_CALL( sound->setUserData(NULL) );
			SWF_FMOD_CALL( sound->release() );
		}

		// Reset handle
		Sound->Handle = NULL;
	}

	unguard;
}

inline const BYTE* FindAnsiString( const BYTE* InStr, const ANSICHAR* FindStr, INT MaxSeek )
{
	for( INT i=0; i<MaxSeek; ++i )
	{
		if( InStr[i]!=*FindStr )
			continue;
		const BYTE* B = &InStr[i];
		while( *FindStr && *FindStr==*B )
		{
			++B;
			++FindStr;
		}
		if( !*FindStr )
			return &InStr[i];
	}
	return NULL;
}
inline TCHAR* GetStringFrom( const BYTE* Start, const BYTE* End )
{
	TCHAR* Res = appStaticString1024();
	TCHAR* S = Res;
	while( Start<End )
	{
		*S = *Start;
		++S; ++Start;
	}
	*S = 0;
	return Res;
}
bool FindLoopPoint( const BYTE* DataStr, FLOAT& LoopStrt, FLOAT& LoopEnd )
{
	guard(FindLoopPoint);
	const BYTE* St = FindAnsiString(DataStr,"COMMENT=|LOOP:",350);
	if( !St )
		return false;
	St+=14;
	const BYTE* En = FindAnsiString(St,"-",20);
	if( !En )
		return false;
	LoopStrt = appAtof(GetStringFrom(St,En))*0.01f;
	St = En+1;
	En = FindAnsiString(St,"|",20);
	if( !En )
		return false;
	LoopEnd = appAtof(GetStringFrom(St,En))*0.01f;
	return true;
	unguard;
}

void USwFMOD::RegisterMusic( UMusic* Music )
{
	guard(USwFMOD::RegisterMusic);
	//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls]"), SWF_LOGP, *ToStr(Music) );
	FMOD_RESULT result;

	checkSlow(Music);
	if( !Music->Handle )
	{
		FMOD::Sound* sound;

		// Load the data.
		Music->Data.Load();
		SWF_LOG( NAME_DevSound, TEXT("Register music: %ls (%i)"), Music->GetPathName(), Music->Data.Num() );
		check(Music->Data.Num()>0);

		// Sound extended data
#pragma pack(push)
#pragma pack(8)
		FMOD_CREATESOUNDEXINFO exinfo;
		appMemset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = Music->Data.Num();
#pragma pack(pop)

		// Sound flags
		FMOD_MODE fmode = 0
			| FMOD_SOFTWARE
		//	| FMOD_IGNORETAGS
		//	| FMOD_LOOP_NORMAL
			| FMOD_2D
			| FMOD_OPENMEMORY
		//	| FMOD_ACCURATETIME
			| FMOD_CREATESTREAM
			;

		// Create sound
		result = System->createStream( static_cast<const char*>(Music->Data.GetData()), fmode, &exinfo, &sound );
		if( result!=FMOD_OK )
		{
			Music->Data.Unload();
			//SWF_LOG_WARN( FString::Printf(TEXT("Couldn't register music [%ls][%i][%ls]"), Music->GetPathName(), Music->Data.Num(), *ToStr(result)) );
			debugf(TEXT("Couldn't register music [%ls][%i][%ls]"), Music->GetPathName(), Music->Data.Num(), *ToStr(result));
			return;
		}

		if( IsStreamingTrack(sound) )
		{
			fmode |= FMOD_LOOP_NORMAL;
			sound->setMode(fmode);

			// Attempt to find loop info.
			FMOD_SOUND_TYPE T;
			FLOAT LStart,LEnd;
			sound->getFormat(&T,NULL,NULL,NULL);
			if( T==FMOD_SOUND_TYPE_OGGVORBIS && Music->Data.Num()>400 && FindLoopPoint(&(Music->Data(0)),LStart,LEnd) )
			{
				UINT iLen;
				sound->getLength(&iLen,FMOD_TIMEUNIT_MS);
				sound->setLoopPoints(appFloor(LStart*iLen),FMOD_TIMEUNIT_MS,appFloor(LEnd*iLen),FMOD_TIMEUNIT_MS);
			}
		}
		else Music->Data.Unload(); // Safe to unload.

		// Register the sound.
		Music->Handle = sound;
		SWF_FMOD_CALL( sound->setUserData(Music) );
	}

	unguard;
}

void USwFMOD::UnregisterMusic( UMusic* Music )
{
	guard(USwFMOD::UnregisterMusic);
	//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls]"), SWF_LOGP, *ToStr(Music) );
	FMOD_RESULT result;

	// If about to fade to specific song, cancel it.
	if( PendingMusic==Music )
		PendingMusic = NULL;

	check(Music);
	if( Music->Handle )
	{
		SWF_LOG( NAME_DevSound, TEXT("Unregister music: %ls"), Music->GetFullName() );

		// Stop this sound.
		FMOD::Sound* sound = GetMusicSample(Music);
		if( sound )
		{
			SWF_FMOD_CALL( sound->setUserData(NULL) );
			SWF_FMOD_CALL( sound->release() );
		}

		// Reset handle
		Music->Handle = NULL;

		// Unload
		Music->Data.Unload();
	}

	unguard;
}


FMOD::Sound* USwFMOD::RegisterSoundSample( USound* Sound )
{
	if( Sound )
	{
		if( !Sound->Handle )
			RegisterSound(Sound);
		return GetSoundSample(Sound);
	}
	return NULL;
}

FMOD::Sound* USwFMOD::RegisterMusicSample( UMusic* Music )
{
	if( Music )
	{
		if( !Music->Handle )
			RegisterMusic(Music);
		return GetMusicSample(Music);
	}
	return NULL;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
