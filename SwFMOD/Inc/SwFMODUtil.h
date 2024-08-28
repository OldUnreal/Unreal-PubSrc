/* ============================================================================
	SwFMODUtil.h:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */
#ifndef SWFMODUTIL_H
#define SWFMODUTIL_H



/* ----------------------------------------------------------------------------
	Helpers
---------------------------------------------------------------------------- */

inline bool HasFlag( INT val, INT flag ) { return (val & flag) == flag; }

template<class A, class B> inline void TSwOverride( A property, B override )
{
	if( override != -1 )
	{
		property = static_cast<A>(override);
	}
}


inline UBOOL SwVerifyFloat(FLOAT f)
{
	return !appIsNan(f);
}


template<class T> inline void TSwSortMinMax( T& vmin, T& vmax )
{
	if( vmin > vmax )
	{
		T temp = vmax;
		vmax = vmin;
		vmin = temp;
	}
}

inline void SwDebugLog( EName Type, FString str, const TCHAR* func=NULL, const char* file=NULL, DWORD line=0, UBOOL bTime=0 )
{
	FString s;

	if( bTime )
	{
		s += FString::Printf( TEXT("%ls"), *SwTimeStr() );
	}

	if( func )
	{
		s += s ? TEXT(" :: ") : TEXT("");
		s += FString::Printf( TEXT("%ls"), func );
	}

	if( file )
	{
		s += s ? TEXT(" ") : TEXT("");
		s += FString::Printf( TEXT("%ls:%u"), appFromAnsi(file), line );
	}

	s += s ? TEXT(" :: ") : TEXT("");
	GLog->Logf( Type, TEXT("%ls%ls"), *s, *str );
}



/* ----------------------------------------------------------------------------
	FSwVersion
---------------------------------------------------------------------------- */
struct FSwVersion
{
	UINT raw;

	FSwVersion( UINT InRaw=-1 ) : raw(InRaw) {}
	UINT* GetRaw() { return &raw; }
	UINT GetDev() const { return raw & 0xFF; }
	UINT GetMinor() const { return (raw>>8) & 0xFF; }
	UINT GetMajor() const { return (raw>>16) & 0xFFFF; }
	operator UBOOL() const { return raw != -1; }
};


/* ----------------------------------------------------------------------------
	FSwSoundId
---------------------------------------------------------------------------- */
struct FSwSoundId
{
	QWORD Id;

	FSwSoundId() : Id(0) {}
	explicit FSwSoundId( QWORD InId ) : Id(InId) {}
	explicit FSwSoundId( void* InUserData ) : Id(reinterpret_cast<QWORD>(InUserData)) {}

	explicit FSwSoundId( AActor* InActor, ESoundSlot InSlot, UBOOL NoOverride )
	{
		SetActor(InActor);
		SetSlot(InSlot);
		SetNoOverride(NoOverride);
	}

	inline QWORD GetId() const							{ return Id; }
	inline QWORD GetIndex() const						{ return (Id >> 4) & 0x0FFFFFFF; }
	inline void* ToUserData() const					{ return reinterpret_cast<void*>(Id); }

	inline AActor* GetActor() const					{ return CastFastUnverified<AActor>(UObject::GetIndexedObject(GetIndex()), CLCAST_AActor); }
	inline ESoundSlot GetSlot() const					{ return static_cast<ESoundSlot>((Id >> 1) & 0x07); }
	inline UBOOL GetNoOverride() const					{ return static_cast<UBOOL>(Id & 0x01); }

	inline void SetActor( AActor* Actor )				{ Id = (Id & 0x0F) | (Actor ? ((Actor->GetIndex() & 0x0FFFFFFF)<<4) : 0); }
	inline void SetSlot( ESoundSlot Slot )				{ Id = (Id & 0xFFFFFFF1) | ((Slot & 0x07) << 1); }
	inline void SetNoOverride( DWORD NoOverride )		{ Id = (Id & 0xFFFFFFFE) | (NoOverride & 0x01); }

	friend UBOOL operator ==( const FSwSoundId& A, const FSwSoundId& B )
	{
		return A.Id == B.Id;
	}
};


/* ----------------------------------------------------------------------------
	FSwChannelEnumerator
---------------------------------------------------------------------------- */
struct FSwChannelEnumerator
{
private:
	FMOD::ChannelGroup					*Group,*ParentGroup;
	INT									ChannelIndex,NumChannels,GroupIndex,NumGroups;
	BYTE								bKeepGoing:1;
	FMOD::Channel*						channel;

public:
	explicit FSwChannelEnumerator( FMOD::System* InSystem, FMOD::ChannelGroup* InGroup=NULL )
		: Group(InGroup), ChannelIndex(-1), GroupIndex(-1), bKeepGoing(1), channel(NULL)
	{
		guardSlow(FSwChannelEnumerator::FChannelEnumerator);

		// use master group if unspecified
		if( !Group )
		{
			if( InSystem->getMasterChannelGroup(&Group)!=FMOD_OK || Group->getNumGroups(&NumGroups)!=FMOD_OK )
			{
				bKeepGoing = 0;
				return;
			}
			ParentGroup = Group;
		}
		else ParentGroup = NULL;

		if( Group->getNumChannels(&NumChannels)!=FMOD_OK )
			bKeepGoing = 0;
		else GetNext();
		unguardSlow;
	}

	void GetNext()
	{
		guardSlow(FSwChannelEnumerator::GetNext);

		// Get next child channel.
		while( ++ChannelIndex<NumChannels )
		{
			if( Group->getChannel(ChannelIndex,&channel) == FMOD_OK )
				return;
		}

		// Get next child group.
		if( ParentGroup )
		{
			while( ++GroupIndex<NumGroups )
			{
				if( ParentGroup->getGroup(GroupIndex,&Group)==FMOD_OK && Group->getNumChannels(&NumChannels)==FMOD_OK && NumChannels )
				{
					ChannelIndex = -1;
					GetNext();
					return;
				}
			}
		}
		bKeepGoing = 0;

		unguardSlow;
	}
	void operator++()					{ GetNext(); }
	operator UBOOL() const				{ return bKeepGoing; }
	FMOD::Channel* operator*() const	{ return channel; }
};


/* ----------------------------------------------------------------------------
	FMOD::Channel accessors
---------------------------------------------------------------------------- */
inline UBOOL IsChannelValid( FMOD::Channel* Channel, UBOOL bCheckPlaying=0 )
{
	guard(IsChannelValid);
	FMOD_RESULT result;
	if( Channel )
	{
		bool test;
		result = Channel->isPlaying(&test);
		if( result == FMOD_OK && (!bCheckPlaying || test) )
		{
			return true;
		}
	}
	return 0;
	unguard;
}

inline FMOD::Sound* GetChannelSample( FMOD::Channel* Channel )
{
	guard(GetChannelSample);
	FMOD_RESULT result;
	if( Channel )
	{
		FMOD::Sound* sound;
		SWF_FMOD_CALL( Channel->getCurrentSound(&sound) );
		if( result == FMOD_OK )
		{
			return sound;
		}
	}
	return NULL;
	unguard;
}

inline UObject* GetChannelObject( FMOD::Channel* Channel )
{
	guard(GetChannelObject);
	FMOD_RESULT result;

	if( Channel )
	{
		FMOD::Sound* sound;
		SWF_FMOD_CALL( Channel->getCurrentSound(&sound) );
		if( result == FMOD_OK && sound )
		{
			void* data;
			SWF_FMOD_CALL( sound->getUserData(&data) );
			if( result == FMOD_OK && data )
			{
				UObject* object = static_cast<UObject*>(data);
				if( object->IsValid() )
					return object;
			}
		}
	}
	return NULL;
	unguard;
}

inline FSwSoundId GetChannelId( FMOD::Channel* Channel )
{
	guard(GetChannelId);
	FMOD_RESULT result;
	if( Channel )
	{
		void* data;
		SWF_FMOD_CALL( Channel->getUserData(&data) );
		if( result == FMOD_OK )
		{
			return FSwSoundId(data);
		}
	}
	return FSwSoundId();
	unguard;
}

inline AActor* GetChannelActor( FMOD::Channel* Channel )
{
	guard(GetChannelActor);
	return GetChannelId(Channel).GetActor();
	unguard;
}


/* ----------------------------------------------------------------------------
	Handle accessors for USound, UMusic and FMOD::Sound
---------------------------------------------------------------------------- */
inline FMOD::Sound* GetSoundSample( USound* Sound )
{
	return static_cast<FMOD::Sound*>(Sound->Handle);
}

inline FMOD::Sound* GetMusicSample( UMusic* Music )
{
	return static_cast<FMOD::Sound*>(Music->Handle);
}

inline UObject* GetSampleObject( FMOD::Sound* Sample )
{
	FMOD_RESULT result;

	if( Sample )
	{
		void* data = NULL;
		result = Sample->getUserData(&data);
		if( result == FMOD_OK && data )
		{
			UObject* object = static_cast<UObject*>(data);
			if( object->IsValid() )
				return object;
		}
	}
	return NULL;
}


/* ----------------------------------------------------------------------------
	Unit conversion
---------------------------------------------------------------------------- */
#define ToFMODFloat(f) (f*GUUToMeters)
#define ToUEFloat(f) (f*GUUToUEMeters)

inline FMOD_VECTOR ToFMODNormal( const FVector& v )
{
	// This means +X is right, +Y is up, and +Z is forwards.
	FMOD_VECTOR f = { v.Y, v.Z, v.X };
	return f;
}

inline FMOD_VECTOR ToFMODVector( const FVector& v )
{
	// This means +X is right, +Y is up, and +Z is forwards.
	FMOD_VECTOR f = { ToFMODFloat(v.Y), ToFMODFloat(v.Z), ToFMODFloat(v.X) };
	return f;
}

inline FVector ToUEVector( const FMOD_VECTOR& v )
{
	return FVector(ToUEFloat(v.z), ToUEFloat(v.x), ToUEFloat(v.y));
}

inline FVector ToUENormal( const FMOD_VECTOR& v )
{
	return FVector(v.z, v.x, v.y);
}



/* ----------------------------------------------------------------------------
	Music
---------------------------------------------------------------------------- */

inline BYTE IsStreamingTrack( FMOD::Sound* S )
{
	FMOD_SOUND_TYPE T;
	S->getFormat(&T,NULL,NULL,NULL);
	return (T!=FMOD_SOUND_TYPE_IT && T!=FMOD_SOUND_TYPE_MOD && T!=FMOD_SOUND_TYPE_S3M && T!=FMOD_SOUND_TYPE_XM && T!=FMOD_SOUND_TYPE_MIDI);
}

#endif

/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
