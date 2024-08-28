/* ============================================================================
	SwFMODPlay.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */



// Includes.
#include "SwFMOD.h"



/*------------------------------------------------------------------------------------
	USwFMOD
------------------------------------------------------------------------------------*/

UBOOL USwFMOD::StartMusic()
{
	guard(USwFMOD::StartMusic);

	// find music channel
	FMOD_RESULT result;
	FMOD::Channel* musicchannel = GetMusicChannel();

	if( !PendingMusic )
	{
		// Simply stop music.
		SWF_FMOD_RCALL0( musicchannel->stop() );
		return 1;
	}

	// Register sample
	FMOD::Sound* sample = RegisterMusicSample(PendingMusic);
	if (!sample)
		return 0;

	// Sample defaults
	MusicVolumeModifier = PendingSongVolume;
	FLOAT deffrequency;
	FLOAT defvolume;
	FLOAT defpanning;
	INT defpriority;
	FMOD_MODE fmode;
	SWF_FMOD_RCALL0( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );
	SWF_FMOD_RCALL0( sample->getMode(&fmode) );

	// Channel defaults
	FMOD::ChannelGroup* channelgroup = MusicChannels;
	FMOD_CHANNELINDEX channelindex = IsChannelValid(musicchannel) ? FMOD_CHANNEL_REUSE : FMOD_CHANNEL_FREE;
	FLOAT frequency = deffrequency * 1.0f;
	FLOAT volume = defvolume * 1.0f * MusicVolumeModifier;
	FLOAT panning = defpanning * 1.0f;
	INT priority = PriorityMusic;


	// Update FMOD to free finished channels
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	// Play stream
	SWF_FMOD_RCALL0( System->playSound(channelindex, sample, true, &musicchannel) );

	// Update channel
	SWF_FMOD_RCALL0( musicchannel->setUserData(NULL) );
	SWF_FMOD_RCALL0( musicchannel->setChannelGroup(channelgroup) );
	SWF_FMOD_RCALL0( musicchannel->setMode(fmode) );
	if( !HasFlag(fmode,FMOD_3D) )
	{
		SWF_FMOD_RCALL0( musicchannel->setPan(panning) );
	}
	SWF_FMOD_RCALL0( musicchannel->setFrequency(frequency) );
	SWF_FMOD_RCALL0( musicchannel->setVolume(volume) );
	SWF_FMOD_RCALL0( musicchannel->setPriority(priority) );

	// Set position
	if( IsStreamingTrack(sample) )
	{
		UINT Len;
		sample->getLength(&Len,FMOD_TIMEUNIT_MS);
		if( PendingSongSection )
			Len = appFloor(FLOAT(PendingSongSection)/255.f*FLOAT(Len));
		else Len = 0;
		musicchannel->setPosition(Len,FMOD_TIMEUNIT_MS);
	}
	else
	{
		musicchannel->setPosition(PendingSongSection,FMOD_TIMEUNIT_MODORDER);
	}

	SWF_FMOD_RCALL0( musicchannel->setPaused(false) );

	// Apply channel group's properties
	ApplyChannelGroup(channelgroup);

	// Update FMOD to apply channel updates immediately
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	return 1;
	unguard;
}
UBOOL USwFMOD::PlayMusic( UMusic* Music, BYTE SongSection, EMusicTransition Transition )
{
	guard(USwFMOD::PlayMusic);

	if( SongSection==255 )
		Music = NULL;

	if( Music )
	{
		// Register sample
		FMOD::Sound* sample = RegisterMusicSample(Music);
		if( !sample )
			return 0; // Failed to init music format.
	}

	// Music switch in before old switch has finished
	FLOAT Alpha = 0.f;
	BYTE bKeepFadingOut=0;
	if( bSwitchMusicTrack )
	{
		if( MusicFadeOutTime<MusicFadeOut )
			bKeepFadingOut = 1;
		else if( MusicFadeTime<MusicFade )
			Alpha = -(MusicFadeTime/MusicFade);
		else bSwitchMusicTrack = 0;
	}

	PendingMusic = Music;
	PendingSongSection = SongSection;
	PendingSongVolume = (FLOAT)Viewport->Actor->SongVolume / 255.f;

	FLOAT OldFadeOut = MusicFadeOut;
	MusicFadeTime = 0.f;
	if( !bKeepFadingOut )
		MusicFadeOutTime = 0.f;

	switch( Transition )
	{
	case MTRAN_Instant:
		MusicFade = 0.1f;
		MusicFadeOut = 0.1f;
		break;
	case MTRAN_Segue:
		MusicFade = 3.f;
		MusicFadeOut = 7.f;
		break;
	case MTRAN_Fade:
		MusicFade = 3.f;
		MusicFadeOut = 3.f;
		break;
	case MTRAN_FastFade:
		MusicFade = 1.f;
		MusicFadeOut = 1.f;
		break;
	case MTRAN_SlowFade:
		MusicFade = 5.f;
		MusicFadeOut = 5.f;
		break;
	default:
		MusicFade = 0.f;
		MusicFadeOut = 0.f;
	}
	if( bSwitchMusicTrack )
	{
		if( bKeepFadingOut )
			MusicFadeOut = OldFadeOut;
		else MusicFadeTime = MusicFade*Alpha;
	}
	bSwitchMusicTrack = 1;
	bNewTrackStarted = 0;

	FMOD::Channel* channel = GetMusicChannel();
	if( channel )
	{
		bool bIsPlaying;
		channel->isPlaying(&bIsPlaying);
		if( !bIsPlaying )
		{
			MusicFadeOut = 0.f; // Make sure no fade out time if no old music.
			if( !Music )
				bSwitchMusicTrack = 0; // No need to do anything as matter of fact.
		}
	}
	return 1;
	unguard;
}

UBOOL USwFMOD::PlaySound
(
	AActor*	Actor,
	INT		Id,
	USound*	Sound,
	FVector	Location,
	FLOAT	Volume,
	FLOAT	Radius,
	FLOAT	Pitch
)
{
	guard(USwFMOD::PlaySound);
	//SWF_LOG( NAME_DevSound, TEXT("%ls >> %ls :: [%ls],[%ls],[%ls],[%ls],[%ls],[%ls],[%ls]"), SWF_PLOG, *ToStr(FSwSoundId(Id)), *ToStr(Actor,true), *ToStr(Sound), *ToStr(Location), *ToStr(Volume), *ToStr(Radius), *ToStr(Pitch) );
	FMOD_RESULT result;

	if( !Viewport || !Sound )
		return 0;

	if( Sound==((USound*)-1) ) // Special meaning for stop.
	{
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel,1) )
			{
				FMOD::ChannelGroup* Grp;
				SWF_FMOD_CALL( channel->getChannelGroup(&Grp) );
				if( Grp==MusicChannels )
					continue; // Don't stop music channels.

				SWF_FMOD_CALL( channel->setUserData(NULL) );
				SWF_FMOD_CALL( channel->stop() );
			}
		}
		return 0;
	}

	AActor* ViewActor = GetViewActor();
	if (Actor && ViewActor->XLevel != Actor->XLevel) // Filter invalid sound source level.
		return 0;

	// Sound slot rules
	FSwSoundId SoundId((QWORD)Id);
	if (Actor && !Actor->IsPendingKill())
	{
		SoundId.SetActor(Actor);

		if (SoundId.GetSlot() != SLOT_None)
		{
			for (FSwChannelEnumerator it(System); it; ++it)
			{
				FMOD::Channel* channel = *it;
				if (IsChannelValid(channel))
				{
					FSwSoundId cid = GetChannelId(channel);

					// If Actor is the same and Slot is the same:
					// - prevent this sound from plaing if NoOverride
					// - stop the old sound otherwise
					if (cid.GetSlot() == SoundId.GetSlot() && cid.GetActor() == Actor)
					{
						if (cid.GetNoOverride())
						{
							//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: NO OVERRIDE <%ls> <%ls>"), SWF_PLOG, *ToStr(FSwSoundId(Id)), *PrintChannel(channel) );
							return 0;
						}
						//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: STOP <%ls> <%ls>"), SWF_PLOG, *ToStr(SoundId), *PrintChannel(channel) );
						SWF_FMOD_CALL(channel->setUserData(NULL));
						SWF_FMOD_CALL(channel->stop());
					}
				}
			}
		}
	}

	// Register sample
	FMOD::Sound* sample = RegisterSoundSample(Sound);
	if( !sample )
		return 0;

	// Sample defaults
	FLOAT deffrequency;
	FLOAT defvolume;
	FLOAT defpanning;
	INT defpriority;
	FMOD_MODE fmode;
	SWF_FMOD_RCALL0( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );
	SWF_FMOD_RCALL0( sample->getMode(&fmode) );

	// Channel defaults
	FMOD::ChannelGroup* channelgroup = EffectChannels;
	FMOD_CHANNELINDEX channelindex = FMOD_CHANNEL_FREE;
	FLOAT mindist = ToFMODFloat(DistanceMin);
	FLOAT radius = ToFMODFloat(Radius);
	TSwSortMinMax( mindist, radius );
	FMOD_VECTOR location = ToFMODVector(Actor ? Actor->Location : Location);
	FMOD_VECTOR velocity = ToFMODVector(Actor ? Actor->Velocity : FVector(0, 0, 0));
	FLOAT frequency = deffrequency * Pitch;
	FLOAT volume = defvolume * Volume;
	FLOAT panning = defpanning * 1.0f;
	INT priority = PrioritySound;

	// 2D or 3D?
	UBOOL bIs3D = b3DCameraSounds || !Actor || (Actor!=ViewActor && (!ViewActor->bIsPawn || ((APawn*)ViewActor)->Weapon!=Actor)) || Viewport->Actor->bBehindView;

	if (bIs3D)
	{
		fmode &= ~FMOD_2D;
		fmode |= FMOD_3D | FMOD_3D_LINEARROLLOFF;

		// Radius, location & Velocity
#if 0
		SWF_VERIFY_FLOAT( mindist );
		SWF_VERIFY_FLOAT( radius );
		SWF_VERIFY_FMODVECTOR(location);
		SWF_VERIFY_FMODVECTOR(velocity);
#endif
	}
	else
	{
		fmode &= ~FMOD_3D;
		fmode |= FMOD_2D;
	}

	// Sound effects other than WAV go into compressed group (conversations most likely)
	// Smirftsch - bad idea, chance of being false especially when using custom content. SLOT_Talk should determine exactly, at least that's what it is for.
	if( SoundId.GetSlot() == SLOT_Talk) // Sound->FileType != SoundType_WAV )
	{
		priority = PrioritySpeech;
		channelgroup = CompressedChannels;
		//fmode |= FMOD_CREATESTREAM;
	}

	// Ambient sounds
	if( SoundId.GetSlot() == SLOT_Ambient )
	{
		priority = PriorityAmbient;
		channelgroup = AmbientChannels;
		fmode &= ~FMOD_LOOP_OFF;
		fmode |= FMOD_LOOP_NORMAL;
	}

	// Update FMOD to free finished channels
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	// Play sound
	FMOD::Channel* channel;
    SWF_FMOD_RCALL0( System->playSound(channelindex, sample, true, &channel) );
//	SWF_FMOD_RCALL0( VerifyNewChannel(channel) );

	// Update channel
	SWF_FMOD_RCALL0( channel->setUserData( SoundId.ToUserData() ) );
	SWF_FMOD_RCALL0( channel->setChannelGroup(channelgroup) );
	SWF_FMOD_RCALL0( channel->setMode(fmode) );
	if( HasFlag(fmode,FMOD_3D) )
	{
		SWF_FMOD_RCALL0( channel->set3DMinMaxDistance(mindist, radius) );
		SWF_FMOD_RCALL0( channel->set3DAttributes(&location,&velocity) );
	}
	else
	{
		SWF_FMOD_RCALL0( channel->setPan(panning) );
	}
	SWF_FMOD_RCALL0( channel->setFrequency(frequency) );
	SWF_FMOD_RCALL0( channel->setVolume(volume) );
	SWF_FMOD_RCALL0( channel->setPriority(priority) );
	SWF_FMOD_RCALL0( channel->setPaused(false) );

	// Apply channel group's properties
	ApplyChannelGroup(channelgroup);

	// Update FMOD to apply channel updates immediately
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	//SWF_LOG( NAME_DevSound, TEXT("%ls << %ls :: [%ls],[%ls],[%ls],[%ls],[%ls],[%ls],[%ls]"), SWF_PLOG, *ToStr(Actor,true), *ToStr(Id), *ToStr(Sound), *ToStr(Location), *ToStr(Volume), *ToStr(Radius), *ToStr(Pitch) );
	return 1;
	unguardf((TEXT("(%ls)"), Sound->GetFullName()));
}

void USwFMOD::ApplyChannelGroup( FMOD::ChannelGroup* ChannelGroup )
{
	guard(USwFMOD::ApplyChannelGroup);
	FMOD_RESULT result;

	FLOAT cvol;
	SWF_FMOD_CALL( ChannelGroup->getVolume(&cvol) );
	SWF_FMOD_CALL( ChannelGroup->setVolume(cvol) );

	unguard;
}

void USwFMOD::NoteDestroy( AActor* Actor )
{
	guard(USwFMOD::NoteDestroy);
	//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls]"), SWF_PLOG, *ToStr(Actor) );
	FMOD_RESULT result;
	INT count = 0;

	if( Actor && Actor->IsValid() )
	{
		// Stop sound
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				FSwSoundId id = GetChannelId(channel);
				if( id.GetActor() == Actor )
				{
					//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: %ls"), SWF_PLOG, *PrintChannel(channel) );
					if( id.GetSlot() == SLOT_Ambient )
					{
						SWF_FMOD_CALL( channel->setUserData(NULL) );
						SWF_FMOD_CALL( channel->stop() );
					}
					else
					{
						id.SetActor(NULL);
						SWF_FMOD_CALL( channel->setUserData(id.ToUserData()) );
					}
					++count;
				}
			}
		}

		// Update system
		if( count )
		{
			guard(UpdateFMOD);
			SWF_FMOD_CALL( System->update() );
			unguard;
		}
	}

	unguard;
}

void USwFMOD::StopSoundId( INT Id )
{
	guard(USwFMOD::StopSoundId);
	//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls]"), SWF_PLOG, *ToStr(FSwSoundId(Id)) );
	FMOD_RESULT result;
	INT count = 0;

	if( Id != 0 )
	{
		// Stop sound
		FSwSoundId soundid = FSwSoundId(Id);
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				FSwSoundId cid = GetChannelId(channel);
				if( cid == soundid )
				{
					//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: [%ls] [%ls]"), SWF_PLOG, *ToStr(soundid), *PrintChannel(channel) );
					SWF_FMOD_CALL( channel->setUserData(NULL) );
					SWF_FMOD_CALL( channel->stop() );
					++count;
				}
			}
		}

		// Update system
		if( count )
		{
			guard(UpdateFMOD);
			SWF_FMOD_CALL( System->update() );
			unguard;
		}
	}

	unguard;
}




/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
