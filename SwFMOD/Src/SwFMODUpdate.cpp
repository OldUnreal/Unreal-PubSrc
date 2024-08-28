/* ============================================================================
	SwFMODUpdate.cpp:
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

void USwFMOD::OnLevelChanged(ULevel* NewLevel)
{
	guard(USwFMOD::OnLevelChanged);

	// Load geometry
	if (bOcclusion && !GIsEditor)
		LoadGeometry(NewLevel);

	// Stop any sound sources from wrong level.
	FMOD_RESULT result;
	for (FSwChannelEnumerator it(System); it; ++it)
	{
		FMOD::Channel* channel = *it;
		if (!IsChannelValid(channel))
			continue;

		// Channel data
		FMOD::Sound* sample = GetChannelSample(channel);
		if (!sample)
			continue;

		FMOD::ChannelGroup* chG;
		SWF_FMOD_CALL(channel->getChannelGroup(&chG));
		if (chG == MusicChannels) // Don't stop music.
			continue;

		AActor* actor = GetChannelId(channel).GetActor();

		if (!actor || actor->XLevel != NewLevel)
		{
			SWF_FMOD_CALL(channel->setUserData(NULL));
			SWF_FMOD_CALL(channel->stop());
		}
	}
	unguard;
}

void USwFMOD::Update( FPointRegion Region, FCoords& Coords )
{
	guard(USwFMOD::Update);
	FMOD_RESULT result;
	//SWF_LOG( NAME_DevSound, TEXT("%ls >> %ls :: [%ls],[%ls]"), SWF_PLOG, *ToStr(Region), *ToStr(Coords) );

	if( !Viewport || !Viewport->IsValid() || !Viewport->Actor || !Viewport->Actor->IsValid() )
		return;

	// Get viewactor
	AActor* ViewActor = GetViewActor();
	AActor* WeaponActor = (ViewActor->bIsPawn ? ((APawn*)ViewActor)->Weapon : NULL);
	ULevel* ViewLevel = ViewActor->XLevel;
	UBOOL bLevelChanged = (ViewLevel != ActiveLevel);

	// Check (sub)level change.
	if (bLevelChanged)
	{
		ActiveLevel = ViewLevel;
		OnLevelChanged(ViewLevel);
	}

	// Time passes...
	FLOAT DeltaTime = appSeconds() - LastTime;
	LastTime += DeltaTime;
	DeltaTime = Clamp( DeltaTime, 0.f, 1.f );

	// Is viewport realtime ?
	UBOOL Realtime = Viewport->IsRealtime() && !Viewport->Actor->Level->Pauser.Len();

	//
	// Update listener
	//

	// Set viewport coords
	FVector TempForward=Coords.ZAxis;
	if(!b3DCameraSounds)
		TempForward.Z=0.f;

	const FVector& location = Coords.Origin;
	const FVector& velocity = ViewActor->Velocity;
	const FVector& forward = TempForward;
	const FVector& up = -Coords.YAxis;
	BYTE bFirstPerson = Viewport->Actor->bBehindView;


	// verify
#if 0
	SWF_VERIFY_FVECTOR(location);
	SWF_VERIFY_FVECTOR(velocity);
	SWF_VERIFY_FVECTOR_NORMAL(forward);
	SWF_VERIFY_FVECTOR_NORMAL(up);
#endif

	// Set listener coords
	FMOD_VECTOR fm_pos = ToFMODVector(location);
	FMOD_VECTOR fm_vel = ToFMODVector(velocity);
	FMOD_VECTOR fm_fwd = ToFMODNormal(forward);
	FMOD_VECTOR fm_up = ToFMODNormal(up);

	// verify
#if 0
	SWF_VERIFY_FMODVECTOR(fm_pos);
	SWF_VERIFY_FMODVECTOR(fm_vel);
	SWF_VERIFY_FMODVECTOR_NORMAL(fm_fwd);
	SWF_VERIFY_FMODVECTOR_NORMAL(fm_up);
#endif

	guard(USwFMODAudio::Update::UpdateListener);
	// Update
	SWF_FMOD_CALL( System->set3DListenerAttributes( 0, &fm_pos, &fm_vel, &fm_fwd, &fm_up ) );
	unguard;


	//
	// Zone effects
	//
	guard(UpdateZone);
#if 0

	// Default zone properties
	UBOOL bWaterZone = 0;
	UBOOL bReverbZone = 0;
	UBOOL bRaytraceReverb = 0;
	BYTE MasterGain = 100;
	INT CutoffHz = 6000;
	BYTE Delay[6] = {20,34,0,0,0,0};
	BYTE Gain[6] = {150,70,0,0,0,0};

	// Get zone properties
	if( Region.Zone && Region.Zone->IsValid() )
	{
		bWaterZone = Region.Zone->bWaterZone;
		bReverbZone = Region.Zone->bReverbZone;
		bRaytraceReverb = Region.Zone->bRaytraceReverb;
		MasterGain = Region.Zone->MasterGain;
		CutoffHz = Region.Zone->CutoffHz;
		appMemcpy(Delay, Region.Zone->Delay, 6*sizeof(BYTE));
		appMemcpy(Gain, Region.Zone->Gain, 6*sizeof(BYTE));
	}
#endif
	AZoneInfo* Z = Viewport->Actor->CameraRegion.Zone;
	if( Z && CurrentEFXAmb!=Z->EFXAmbients )
	{
		CurrentEFXAmb = Z->EFXAmbients;
		System->setReverbProperties(&EFXPresets[CurrentEFXAmb]);
		bIsEmulatingReverb = 0;
		OldZone = NULL;
	}
	if( Z!=OldZone )
	{
		OldZone = Z;
		if( bEmulateReverb && Z && CurrentEFXAmb==REVERB_PRESET_NONE && Z->bReverbZone )
		{
			bIsEmulatingReverb = 1;
			FLOAT ReverbVolume	= Clamp((Z->MasterGain/255.0f), 0.001f, 1.f);
			// CombFilter to EFX
			FLOAT MaxDelay=0.f;
			FLOAT MaxGain=0.f;
			FLOAT MinDelay=0.f;
			FLOAT AvgDelay=0.f;
			FLOAT T=0.f;

			//Calculate reverb time from six comb filters
			for ( BYTE i=0;i<ARRAY_COUNT(Z->Delay);i++)
			{
				FLOAT DelayTime = Clamp(Z->Delay[i]/500.0f, 0.001f, 0.340f);
				FLOAT DelayGain = Clamp(Z->Gain[i]/255.0f, 0.001f, 0.999f);
				if ((DelayTime*DelayGain)>(MaxDelay*MaxGain))
				{
					MaxDelay=DelayTime;
					MaxGain=DelayGain;
				}
				if (DelayTime<MinDelay)
					MinDelay=DelayTime;
				AvgDelay+=(DelayTime/6.0f);
				T+=DelayTime;
			}
			FLOAT RoomSize = 3.0f*(AvgDelay-MinDelay)/(MaxDelay-MinDelay);
			BYTE PresetNum = 0;
			if(RoomSize<0.4)
				PresetNum = REVERB_PRESET_DUSTYROOM;
			else if(RoomSize<0.8)
				PresetNum = REVERB_PRESET_CASTLE_SMALLROOM;
			else if(RoomSize<1.2)
				PresetNum = REVERB_PRESET_ICEPALACE_ALCOVE;
			else if(RoomSize<1.6)
				PresetNum = REVERB_PRESET_CASTLE_SHORTPASSAGE;
			else if(RoomSize<2.0)
				PresetNum = REVERB_PRESET_ICEPALACE_SHORTPASSAGE;
			else if(RoomSize<2.4)
				PresetNum = REVERB_PRESET_CASTLE_MEDIUMROOM;
			else if(RoomSize<4)
				PresetNum = REVERB_PRESET_CASTLE_LONGPASSAGE;
			else if(RoomSize<6)
				PresetNum = REVERB_PRESET_SEWERPIPE;
			else if(RoomSize<8)
				PresetNum = REVERB_PRESET_OUTDOORS_DEEPCANYON;
			else PresetNum = REVERB_PRESET_QUARRY;

			FMOD_REVERB_PROPERTIES Revb = EFXPresets[PresetNum];
			Revb.DecayTime = Clamp<FLOAT>(RoomSize*2.f,0.1f,20.f);
			Revb.DecayLFRatio = Clamp<FLOAT>(RoomSize*0.3f,0.1f,2.f);
			Revb.DecayHFRatio = Clamp<FLOAT>(RoomSize*0.15f,0.1f,2.f);
			Revb.Diffusion*=ReverbVolume;
			Revb.EnvDiffusion*=ReverbVolume;
			Revb.Density*=ReverbVolume;
			Revb.DecayTime*=ReverbVolume;
			Revb.DecayHFRatio*=ReverbVolume;
			Revb.DecayLFRatio*=ReverbVolume;
			Revb.ReflectionsDelay*=ReverbVolume;
			Revb.ModulationDepth*=ReverbVolume;
			System->setReverbProperties(&Revb);
		}
		else if( bIsEmulatingReverb )
		{
			System->setReverbProperties(&EFXPresets[REVERB_PRESET_NONE]);
			bIsEmulatingReverb = 0;
		}
	}
	unguard;

	//
	// Update sounds.
	//
	guard(USwFMODAudio::Update::UpdateSounds);
	URenderBase* Rend = Viewport->GetOuterUClient()->Engine->Render;

	// Iterate through all channels
	for( FSwChannelEnumerator it(System); it; ++it )
	{
		FMOD::Channel* channel = *it;
		if( !IsChannelValid(channel) )
			continue;

		// Channel data
		FMOD::Sound* sample = GetChannelSample(channel);
		if( !sample )
			continue;

		USound* sound = Cast<USound>(GetSampleObject(sample));
		if( !sound )
			continue;

		FSwSoundId id = GetChannelId(channel);
		AActor* actor = id.GetActor();

		FMOD_MODE fmode;
		SWF_FMOD_CALL( channel->getMode(&fmode) );
		UBOOL bIs3D = HasFlag(fmode,FMOD_3D);

		// Sample defaults
		FLOAT deffrequency;
		FLOAT defvolume;
		FLOAT defpanning;
		INT defpriority;
		SWF_FMOD_CALL( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );

		if (actor && !actor->IsPendingKill())
		{
			if (actor->XLevel != ViewLevel)
			{
				// Stop wrong level sound source.
				SWF_FMOD_CALL(channel->setUserData(NULL));
				SWF_FMOD_CALL(channel->stop());
				continue;
			}

			// Ambient sounds
			if (id.GetSlot() == SLOT_Ambient)
			{
				if (actor->AmbientSound != sound
					|| (location - actor->Location).SizeSquared() > Square(actor->WorldSoundRadius() + AmbientHysteresis)
					|| !Realtime)
				{
					// Stop ambient sound
					//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: Ambient OUT [%d]"), SWF_PLOG, *ToStr(id) );
					SWF_FMOD_CALL(channel->setUserData(NULL));
					SWF_FMOD_CALL(channel->stop());
					continue;
				}
				else
				{
					// Update ambient sound properties.
					FLOAT volume = (actor->SoundVolume / 255.0f) * defvolume;
					FLOAT frequency = (actor->SoundPitch / 64.0f) * deffrequency;

					if (actor->LightType != LT_None)
					{
						static FPlane Color;
						volume *= actor->LightBrightness / 255.0;
						Rend->GlobalLighting((Viewport->Actor->ShowFlags & SHOW_PlayerCtrl) != 0, actor, volume, Color);
					}

					//SWF_VERIFY_FLOAT(volume);
					//SWF_VERIFY_FLOAT(frequency);

					SWF_FMOD_CALL(channel->setVolume(volume));
					SWF_FMOD_CALL(channel->setFrequency(frequency));

					if (bIs3D)
					{
						// Update 3D sound properties
						FLOAT mindist = ToFMODFloat(DistanceMin);
						FLOAT radius = ToFMODFloat(actor->WorldSoundRadius());
						TSwSortMinMax(mindist, radius);

						SWF_VERIFY_FLOAT(radius);
						SWF_VERIFY_FLOAT(mindist);

						SWF_FMOD_CALL(channel->set3DMinMaxDistance(mindist, radius));
					}
				}
			}

			if (bIs3D)
			{
				// Update 3D sound properties
				if (bFirstPerson && (actor == ViewActor || actor == WeaponActor))
				{
					SWF_FMOD_CALL(channel->set3DAttributes(&fm_pos, &fm_vel));
				}
				else
				{
					FMOD_VECTOR snd_pos = ToFMODVector(actor->Location);
					FMOD_VECTOR snd_vel = ToFMODVector(actor->Velocity);

#if 0
					SWF_VERIFY_FMODVECTOR(snd_pos);
					SWF_VERIFY_FMODVECTOR(snd_vel);
#endif

					SWF_FMOD_CALL(channel->set3DAttributes(&snd_pos, &snd_vel));
				}
			}
		}
		else if (bLevelChanged) // Stop all unknown sound sources when player travels to another sublevel.
		{
			SWF_FMOD_CALL(channel->setUserData(NULL));
			SWF_FMOD_CALL(channel->stop());
		}
		else if (id.GetSlot() == SLOT_Ambient) // Stop sound if too far off screen...
		{
			if( bIs3D )
			{
				FLOAT radius;
				FMOD_VECTOR snd_pos;
				SWF_FMOD_CALL(channel->get3DMinMaxDistance(NULL,&radius) );
				SWF_FMOD_CALL(channel->get3DAttributes(&snd_pos,NULL));

				if( (ToUEVector(snd_pos)-Coords.Origin).SizeSquared()>Square(ToUEFloat(radius)) )
				{
					SWF_FMOD_CALL( channel->setUserData(NULL) );
					SWF_FMOD_CALL( channel->stop() );
				}
			}
			else // Never keep looping 2D sounds around with unknown source.
			{
				SWF_FMOD_CALL( channel->setUserData(NULL) );
				SWF_FMOD_CALL( channel->stop() );
			}
		}
	}
	unguard;


	//
	// Play ambient sounds
	//
	if( Realtime )
	{
		guard(USwFMODAudio::Update::PlayAmbient);
		for (INT i = 0; i < ViewLevel->Actors.Num(); ++i)
		{
			AActor* Actor = ViewLevel->Actors(i);
			if(	Actor && Actor->AmbientSound && Actor->SoundRadius && Actor->SoundVolume
			&&	(location-Actor->Location).SizeSquared() <= Square(Actor->WorldSoundRadius()) )
			{
				FSwSoundId ambientid(Actor,SLOT_Ambient,0);
				//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: Ambient TEST IN [%ls]"), SWF_PLOG, *ToStr(ambientid) );

				// Find this sound in currently playing ones
				FMOD::Channel* ambientchannel = NULL;
				for( FSwChannelEnumerator it(System,AmbientChannels); it; ++it )
				{
					FMOD::Channel* channel = *it;
					if( IsChannelValid(channel) && GetChannelId(channel) == ambientid )
					{
						//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: Ambient FOUND IN [%ls]"), SWF_PLOG, *ToStr(GetChannelId(channel)) );
						ambientchannel = channel;
						break;
					}
				}

				// If not found play ambient
				if( !ambientchannel )
				{
					//SWF_LOG( NAME_DevSound, TEXT("%ls -- %ls :: Ambient PLAY IN [%ls][%ls]"), SWF_PLOG, *ToStr(ambientid), *ToStr(Actor->AmbientSound) );
					PlaySound( Actor, ambientid.GetId(), Actor->AmbientSound, Actor->Location, Actor->SoundVolume/255.0f, Actor->WorldSoundRadius(), Actor->SoundPitch/64.0f );
				}
			}
		}
		unguard;
	}


	//
	// Music
	//
	guard(UpdateMusic)

/*	REQUIREMENTS

	SongSection is updated at realtime by audio sys
	MTRAN_Fade* only fade out, not in
	music changes caused by transition only
	ttransition reset on change
	MTRAN_None = don't change
	MTRAN_Instant = instant change
	MTRAN_Segue = seamless?
	MTRAN_Fade = 1s fade
	MTRAN_FastFade = 1/3s fade
	MTRAN_SlowFade = 5s fade
*/
	// find music channel
	FMOD::Channel* musicchannel = NULL;
	for( FSwChannelEnumerator it(System,MusicChannels); it; ++it )
	{
		FMOD::Channel* channel = *it;
		if( !IsChannelValid(channel) )
			continue;

		if( !musicchannel )
		{
			musicchannel = channel;
		}
		else
		{
			// there can be only one music
//			SWF_LOG( NAME_DevSound, TEXT("%ls :: %ls :: StopMusic %ls"), SWF_PLOG, *PrintChannel(channel) );
			SWF_FMOD_CALL( channel->setUserData(NULL) );
			SWF_FMOD_CALL( channel->stop() );
		}
	}

	if( Viewport->Actor->Transition != MTRAN_None )
	{
		PlayMusic( Viewport->Actor->Song, Viewport->Actor->SongSection, static_cast<EMusicTransition>(Viewport->Actor->Transition) );
		Viewport->Actor->Transition = MTRAN_None;
	}
	else if( bSwitchMusicTrack )
	{
		if( !bNewTrackStarted )
		{
			if( (MusicFadeOutTime+=DeltaTime)>=MusicFadeOut )
			{
				bNewTrackStarted = 1;
				SWF_FMOD_CALL( musicchannel->setVolume(0.f) );
				StartMusic();
				if( !IsChannelValid(musicchannel,1) )
					bSwitchMusicTrack = 0; // No need for fade in as music stopped.
			}
			else SWF_FMOD_CALL( musicchannel->setVolume( (1.f-(MusicFadeOutTime / MusicFadeOut)) * MusicVolumeModifier) );
		}
		else if( (MusicFadeTime+=DeltaTime)>=MusicFade )
		{
			SWF_FMOD_CALL( musicchannel->setVolume(MusicVolumeModifier) );
			bSwitchMusicTrack = 0;
		}
		else SWF_FMOD_CALL( musicchannel->setVolume( (MusicFadeTime / MusicFade) * MusicVolumeModifier) );
	}

	unguard;

	// Update FMOD
	guard(UpdateFMOD);
	SWF_FMOD_CALL( System->update() );
	unguard;

	//SWF_LOG( NAME_DevSound, TEXT("%ls << %ls :: [%ls],[%ls]"), SWF_PLOG, *ToStr(Region), *ToStr(Coords) );
	unguard;
}



/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
