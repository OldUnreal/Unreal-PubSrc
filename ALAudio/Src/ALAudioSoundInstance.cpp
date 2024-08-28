/*=============================================================================
	ALAudioSoundInstance.cpp: ALAudio abstraction of a playing sound.

	Revision history:
	* Created by Fernando Velazquez (Higor)
=============================================================================*/

#include "ALAudio.h"


static void TransformCoordinates( ALfloat Dest[3], const FVector &Source)
{
	Dest[0] = Source.X;
	Dest[1] = Source.Y;
	Dest[2] = -Source.Z;
}


void ALAudioSoundInstance::Init()
{
    guard(ALAudioSoundInstance::Init);

    if (SourceID == 0)
        return;

	Radius            = 3.40282e+38; //std::numeric_limits<float>::max();
	Pitch             = 1.0;
	DopplerFactor     = 1.0;
	AttenuationFactor = 1.0;
	LastAudioOffset   = 0;
	LipsynchTime      = 0.0f;

	alSourcef( SourceID, AL_MAX_DISTANCE, Radius);
	alSourcef( SourceID, AL_REFERENCE_DISTANCE, 0.1*Radius);

	// The higher the value the higher the distance based attenuation.
	if ( GetSlot() != SLOT_Ambient )
		alSourcef( SourceID, AL_ROLLOFF_FACTOR, 1.1f);

	// Enable loop
	if ( SoundFlags & SF_Loop )
		alSourcei( SourceID, AL_LOOPING, AL_TRUE);

    unguard;
}


bool ALAudioSoundInstance::IsPlaying()
{
    guard(ALAudioSoundInstance::IsPlaying);

    if (SourceID == 0)
        return AL_NONE;

	ALint state = AL_STOPPED;
	alGetSourcei( SourceID, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;

	unguard;
}


void ALAudioSoundInstance::Stop()
{
    guard(ALAudioSoundInstance::Stop);

	if ( alIsSource(SourceID) )
	{
		alSourcei( SourceID, AL_LOOPING, FALSE); //TODO: Track state
		if ( IsPlaying() )
			alSourceStop(SourceID);
		alDeleteSources( 1, &SourceID);
		SourceID = 0;
	}

	unguard;
}

void ALAudioSoundInstance::SetRadius( FLOAT NewRadius)
{
    guardSlow(ALAudioSoundInstance::SetRadius);

    if (SourceID == 0)
        return;


	if ( Radius != NewRadius )
	{
		Radius = NewRadius;
		alSourcef( SourceID, AL_REFERENCE_DISTANCE, 0.1*Radius);
		alSourcef( SourceID, AL_MAX_DISTANCE, Radius);
	}

	unguardSlow;
}

void ALAudioSoundInstance::SetPitch( FLOAT NewPitch)
{
    guardSlow(ALAudioSoundInstance::SetPitch);

    if (SourceID == 0)
        return;

	if ( Pitch != NewPitch )
	{
		Pitch = NewPitch;
		alSourcef( SourceID, AL_PITCH, Pitch);
	}

	unguardSlow;
}

void ALAudioSoundInstance::SetDopplerFactor( FLOAT NewDopplerFactor)
{
    guardSlow(ALAudioSoundInstance::SetDopplerFactor);

    if (SourceID == 0)
        return;

	INT Slot = GetSlot();
	if ( (SoundFlags & SF_No3D) || (Slot == SLOT_Interface) )
		NewDopplerFactor = 0;
	else if ( Slot == SLOT_Misc || Slot == SLOT_Talk )
		NewDopplerFactor *= 0.5;

	if ( DopplerFactor != NewDopplerFactor )
	{
		DopplerFactor = NewDopplerFactor;
		alSourcef( SourceID, AL_DOPPLER_FACTOR, NewDopplerFactor);
	}

	unguardSlow;
}

void ALAudioSoundInstance::SetVolume( FLOAT NewVolume, FLOAT NewMaxVolume)
{
    guardSlow(ALAudioSoundInstance::SetVolume);

    if (SourceID == 0)
        return;

	alSourcef( SourceID, AL_GAIN, Clamp(NewVolume, 0.f, NewMaxVolume));
	alSourcef( SourceID, AL_MAX_GAIN, NewMaxVolume);

	unguardSlow;
}


void ALAudioSoundInstance::UpdateAttenuation( FLOAT NewAttenuationFactor, FLOAT DeltaTime)
{
    guardSlow(ALAudioSoundInstance::UpdateAttenuation);

    if (SourceID == 0)
        return;

	FLOAT OldAttenuationFactor = AttenuationFactor;

	// No transition
	if ( DeltaTime < 0 )
		AttenuationFactor = NewAttenuationFactor;
	else if ( AttenuationFactor != NewAttenuationFactor )
	{
		FLOAT Dir = NewAttenuationFactor < AttenuationFactor ? -1.f : 1.f;
		FLOAT DeltaAttenuation = Clamp( Abs(NewAttenuationFactor-AttenuationFactor), 0.f, DeltaTime);
		AttenuationFactor += DeltaAttenuation * Dir * 0.2f;
	}

	if ( AttenuationFactor != OldAttenuationFactor )
	{
		if ( AttenuationFactor == 1.f )
			alFilteri( Sound->Filter, AL_FILTER_TYPE, AL_FILTER_NULL);
		else
		{
			alFilteri(Sound->Filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			alFilterf(Sound->Filter, AL_LOWPASS_GAIN, 0.2f + 0.5f * AttenuationFactor );
			alFilterf(Sound->Filter, AL_LOWPASS_GAINHF, 0.1f + 0.8f * AttenuationFactor );
		}
		alSourcei(SourceID, AL_DIRECT_FILTER, Sound->Filter);
	}

	unguardSlow;
}

void ALAudioSoundInstance::Spatialize()
{
    guardSlow(ALAudioSoundInstance::Spatialize);

    if (SourceID == 0)
        return;

	if ( SoundFlags & SF_No3D )
	{
		alSourcei( SourceID, AL_SOURCE_SPATIALIZE_SOFT, AL_FALSE);
/*		alSourcei( SourceID, AL_SOURCE_RELATIVE, TRUE);
		ALfloat zup[] = { 0.0, -1.0, 0.0, 0.0, 0.0, -1.0 };
		alSourcefv( SourceID, AL_ORIENTATION, zup);
		alSource3f( SourceID, AL_POSITION, 0.0, 0.0, 0.0);
		alSource3f( SourceID, AL_VELOCITY, 0.0, 0.0, 0.0);
		alSource3f( SourceID, AL_DIRECTION, 0.0, 0.0, 0.0);*/
	}
	else
	{
		ALfloat Position[3]={0.f};
		ALfloat Velocity[3]={0.f};
		if ( Actor )
			TransformCoordinates( Velocity, Actor->Velocity);
		TransformCoordinates( Position, Location);
		alSourcefv( SourceID, AL_POSITION, Position);
		alSourcefv( SourceID, AL_VELOCITY, Velocity);
	}

	//Higor: WTF is this?
	//Smirftsch: ambient is supposed to be non directional.
	if (Actor)
	{
		bool bDirectional = Actor->bDirectional && (Radius > 1);
		if (GetSlot() == SLOT_Ambient && !bDirectional && !(SoundFlags & SF_No3D))
		{
			alSourcef(SourceID, AL_SOURCE_RADIUS, Radius); // OpenALSoft   doesn't support this (yet), interfering with some other extension. Creative OpenAL works, but glitched.
			// Either way it seems to be what the lerp function below does and doesn't make it non directional as I would it expect to be.

			//FVector Src_Pos = Lerp(CameraLocation, Location, PlayerDist / Radius);//oddawoddawassawarna?
			//TransformCoordinates(Position, Src_Pos);//wassawarnawakumba!
			//alSourcefv(SourceID, AL_POSITION, Position);
		}
	}

    unguardSlow;
}

void ALAudioSoundInstance::ProcessLoop()
{
    guardSlow(ALAudioSoundInstance::ProcessLoop);

    if (SourceID == 0)
        return;

	if (SoundFlags & SF_LoopingSource)
	{
		if (Sound->bLoopingSample && (Sound->LoopEnd > 0 || Sound->LoopStart > 0))
		{
			ALint ALOffset;
			alGetSourcei(SourceID, AL_SAMPLE_OFFSET, &ALOffset);

			// OpenAL has already looped this internally, move to LoopStart if we haven't already
			if (LastAudioOffset > ALOffset)
			{
				if (Sound->LoopStart > ALOffset)
				{
					ALOffset = Sound->LoopStart;
					alSourcei(SourceID, AL_SAMPLE_OFFSET, ALOffset);
				}
			}
			// Handle loop here if OpenAL hasn't done it
			else if (ALOffset >= Sound->LoopEnd)
			{
				ALOffset = Sound->LoopStart;
				alSourcei(SourceID, AL_SAMPLE_OFFSET, ALOffset);
			}
			LastAudioOffset = ALOffset;
		}
	}

	unguardSlow;
}


void ALAudioSoundInstance::UpdateActor()
{
    guardSlow(ALAudioSoundInstance::UpdateActor);

	// Update actor related vars
	if ( Actor )
	{
		// stijn: Galaxy always did this regardless of SoundFlags
		// This is needed in the UT intro movie for example
		Location = Actor->Location;

		// Update Water (voices are emitted from HeadRegion)
		INT Slot = GetSlot();
		const FPointRegion& Region = GetRegion( Actor, (Slot==SLOT_Talk || Slot==SLOT_Pain) );
		if ( Region.Zone && Region.Zone->bWaterZone )
			SoundFlags |= SF_WaterEmission;
		else
			SoundFlags &= ~SF_WaterEmission;
	}

	unguardSlow;
}

void ALAudioSoundInstance::UpdateVolume()
{
	guardSlow(ALAudioSoundInstance::UpdateVolume);
	//if (UseOriginalUnreal)
	//	Vol = Playing.Volume * (1.0 - (Playing.Location - (Viewport->Actor->ViewTarget ? Viewport->Actor->ViewTarget : Viewport->Actor)->Location).Size() / Playing.Radius);

	//OpenAL:
	/*
	distance = min(distance, AL_MAX_DISTANCE) // avoid negative gain
	gain = (1 – AL_ROLLOFF_FACTOR * (distance –  AL_REFERENCE_DISTANCE) / (AL_MAX_DISTANCE – AL_REFERENCE_DISTANCE))
	*/

	FLOAT ConfiguredAudioVolume;
	if (SoundFlags & SF_Speech)
		ConfiguredAudioVolume = ((FLOAT)UALAudioSubsystem::GALAudio->SpeechVolume / 255.f);
	else
		ConfiguredAudioVolume = ((FLOAT)UALAudioSubsystem::GALAudio->SoundVolume / 255.f);
	FLOAT AudioVolume = Volume * ConfiguredAudioVolume;

	// Scale Viewport sounds
	if (SoundFlags & SF_No3D)
		AudioVolume *= UALAudioSubsystem::GALAudio->ViewportVolumeIntensity;
	else if (Sound->bIsStereo)
	{
		// Marco: ALAudio doesn't do distance attenuate for stereo sounds, do it manually.
		AudioVolume *= Max<FLOAT>(1.f - Location.Distance(UALAudioSubsystem::GALAudio->GetCameraLocation()) / Radius, 0.f);
	}

	// Slightly amplify when both emitter and listener are underwater
	if ((SoundFlags & SF_WaterEmission))
	{
		AZoneInfo* Zone = UALAudioSubsystem::GALAudio->GetCameraZone();
		if (Zone && Zone->bWaterZone)
			AudioVolume *= 1.25f;
	}

	// Allow maximum volume to be a bit higher.
	SetVolume(AudioVolume, ConfiguredAudioVolume * 1.5);
	unguardSlow;
}
