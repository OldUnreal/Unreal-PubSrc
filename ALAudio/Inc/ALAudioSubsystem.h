/*=============================================================================
ALAudioSubsystem.cpp: Unreal OpenAL Audio interface object.
Copyright 2015 Oldunreal.

Revision history:
* Old initial version by Joseph I. Valenzuela (based on AudioSubsystem.cpp)
* Heavily modified and completely rewritten by Jochen 'Smirftsch' Görnitz
* Porting and porting help for other UEngine1 games, code cleanups, efx refactoring and lipsynch by Sebastian 'Hanfling' Kaufel
=============================================================================*/

#define AL_ALEXT_PROTOTYPES 1

#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/efx.h>
#include <AL/alext.h>

#ifdef ALURE
#define ALURE_STATIC_LIBRARY
# if MACOSX
#  include "alure.h"
# else
#  include <AL/alure.h>
# endif
#else
#include <AL/alut.h>
#endif

#include <math.h>
#include "ALAudioTypes.h"
#include "EFX-Util.h"
// libxmp music
#include "xmp.h"
// ogg music
#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisenc.h"
#include "vorbis/vorbisfile.h"

#define TRUE 1
#define FALSE 0
#define PlayBuffer 1

#ifdef __GNUG__
#include <unistd.h>
#endif

/*------------------------------------------------------------------------------------
Helpers
------------------------------------------------------------------------------------*/

// Constants.
#define MAX_EFFECTS_CHANNELS 256
#define EFFECT_FACTOR        1.0
#define MUSIC_BUFFERS		 4
#define MUSIC_BUFFER_SIZE	 16384
#define SOUND_BUFFER_SIZE	 4096
#define CPP_PROPERTY_LOCAL(name) name, CPP_PROPERTY(name)

// Use 30 Hz as in stock Deus Ex for now, make it configurable later, bla bla yak yak.
#define LIPSYNCH_FREQUENCY 30.f

// Utility Macros.
#define safecall(f) \
{ \
	guard(f); \
	INT Error=f; \
	if( Error==0 ) \
		debugf( NAME_Warning, TEXT("%ls failed: %i"), TEXT(#f), Error ); \
	unguard; \
}
#define silentcall(f) \
{ \
	guard(f); \
	f; \
	unguard; \
}

#ifdef _WIN32
#define FASTCALL	__fastcall
#else
#define FASTCALL __attribute__((fastcall))
#endif

typedef struct _memory_ogg_file
{
	BYTE* curPtr;
	BYTE* filePtr;
	size_t fileSize;
} memory_ogg_file;

/*------------------------------------------------------------------------------------
PlayingSounds and the associated data type are used in the queue of playing sounds.
------------------------------------------------------------------------------------*/

enum ESoundFlags
{
	SF_AmbientSound    = 0x00000001, //Automatically played ambient sound
	SF_LoopingSource   = 0x00000002,
	SF_Speech          = 0x00000004,
	SF_No3D            = 0x00000008,
	SF_NoFilter        = 0x00000010,
	SF_WaterEmission   = 0x00000020,

	SF_Loop            = SF_AmbientSound | SF_LoopingSource,
};

struct ALAudioSoundInstance
{
	AActor* Actor;
	ULevel* Level;
	struct ALAudioSoundHandle* Sound;
	FLOAT Priority;
	INT Id;
	ALuint SourceID;
	FLOAT Volume;
	FLOAT MaxVolume;
	FLOAT Radius;
	FLOAT Pitch;
	FLOAT DopplerFactor;
	FLOAT AttenuationFactor;
	FVector Location;
	FVector CameraLocation;
	DWORD SoundFlags;
	INT LastAudioOffset;
	FTIME LipsynchTime;

	ALAudioSoundInstance()
		: Actor(nullptr)
		, Level(nullptr)
		, Sound(nullptr)
		, SourceID(0)
	{}

	// Initialize default state.
	void Init();

	// Sound control
	bool IsPlaying();
	void Stop();
	void ProcessLoop();

	void UpdateActor();
	void UpdateVolume();

	// Modify sound state.
	void SetRadius( FLOAT NewRadius);
	void SetPitch( FLOAT NewPitch);
	void SetDopplerFactor( FLOAT NewDopplerFactor);
	void SetVolume( FLOAT NewVolume, FLOAT NewMaxVolume);

	void UpdateAttenuation( FLOAT NewAttenuationFactor, FLOAT DeltaTime=-1);
	void Spatialize();

	// Inlines
	INT GetSlot() const;
	UBOOL IsPlayingSound(USound* S) const;
};

struct ALAudioSoundHandle
{
	USound* Sound;
	INT LoopStart, LoopEnd;
	ALuint ID;
	ALuint Filter;
	FLOAT Rate;
	FLOAT Duration;
	BITFIELD bLoopingSample : 1;
	BITFIELD bIsStereo : 1;

	ALAudioSoundHandle(USound* S)
		: Sound(S)
		, ID(0)
		, Filter(0)
		, LoopStart(0)
		, LoopEnd(0)
		, Rate(0)
		, Duration(0)
		, bLoopingSample(FALSE)
		, bIsStereo(FALSE)
	{}
};
struct ALAudioMusicHandle
{
	UMusic* MusicRef;
	xmp_context xmpcontext;
	UBOOL   IsOgg;
	ALuint	format;
	ALsizei	SampleRate; //no really need to set it here, but dislike having more statics.
	INT		SongSection;
	FString MusicType, MusicTitle;
	OggVorbis_File* OggStream;
	vorbis_info*    vorbisInfo;
	vorbis_comment* vorbisComment;
	memory_ogg_file MemOggFile;

	ALAudioMusicHandle(UMusic* InMusic);
	~ALAudioMusicHandle() noexcept(false);
};
struct ALAudioMusicChannel : protected FThreadingBase
{
	ALAudioMusicHandle* Handle;

	ALAudioMusicChannel(ALAudioMusicHandle* H, BYTE StartSection, FLOAT StartVolume, class UALAudioSubsystem* AudioDevice);
	~ALAudioMusicChannel() noexcept(false);

	inline UBOOL IsInvalid() const
	{
		return bChannelInvalid;
	}
	inline ALint GetBufferError() const
	{
		return BufferError;
	}
	inline FLOAT GetVolume() const
	{
		return Volume;
	}
	void SetVolume(FLOAT NewVolume);

	inline void SetStereoAngles(const ALfloat* Angles) const
	{
		alSourcefv(musicsource, AL_STEREO_ANGLES, Angles);
	}
	inline void SetSeperation(INT Seperation) const
	{
		if (!Handle->IsOgg)
			xmp_set_player(Handle->xmpcontext, XMP_PLAYER_MIX, Seperation);
	}

private:
	UBOOL	bChannelInvalid;
	void*	xmpBuffer;
	BYTE	Section;
	FLOAT	Volume;
	ALuint	musicsource;
	ALuint	musicbuffers[MUSIC_BUFFERS];
	ALint   BufferError;
	char*	OggBufferData;

	void Main() override;
};

#define bound(value,min,max) ((value)<(min)?(min):((value)>(max)?(max):(value)))

#ifdef EFX
// Struct used for querying EFX settings.
struct FEFXEffects
{
	INT Version;
	BYTE ReverbPreset; // Called Ambients in 227.
	FLOAT AirAbsorptionGainHF;
	FLOAT DecayHFRatio;
	FLOAT DecayLFRatio;
	FLOAT DecayTime;
	FLOAT Density;
	FLOAT Diffusion;
	FLOAT EchoDepth;
	FLOAT EchoTime;
	FLOAT Gain;
	FLOAT GainHF;
	FLOAT GainLF;
	FLOAT HFReference;
	FLOAT LFReference;
	FLOAT LateReverbDelay;
	FLOAT LateReverbGain;
	FLOAT RoomRolloffFactor;
	BITFIELD bUserDefined : 1 GCC_ALIGN(4);
	BITFIELD bDecayHFLimit : 1;
	INT Reserved[32] GCC_ALIGN(4);
};
#endif

enum EMTransState : DWORD
{
	TTRANS_None,
	TTRANS_FadeOut,
	TTRANS_FadeIn,
	TTRANS_Crossfade,
};

/*------------------------------------------------------------------------------------
	UALAudioSubsystem.
------------------------------------------------------------------------------------*/

//
// The Generic implementation of UAudioSubsystem.
//

class DLL_EXPORT_CLASS UALAudioSubsystem : public UAudioSubsystem
#if defined(USE_UNEXT)
	, public IHumanHeadAudioSubsystem
#endif
{
	DECLARE_CLASS_AUDIO(UALAudioSubsystem, UAudioSubsystem, CLASS_Config, ALAudio);

	static UALAudioSubsystem* GALAudio;

	// Configuration.
	BITFIELD		Initialized;
	UBOOL			UseDigitalMusic;
	UBOOL			ProbeDevicesOnly;
	UBOOL			AudioStats;
	UBOOL			DetailStats;
	UBOOL			EffectsStats;
#ifdef EFX
	UBOOL			UseReverb;
#endif
	UBOOL			UseAmbientPresets;
//	UBOOL			UseOriginalUnreal;
	UBOOL			bSoundAttenuate;
	UBOOL			UseHardwareChannels;
	UBOOL			UseSpeechVolume;
	INT				MusicPanSeparation;	// XMP default Pan separation
	INT				MusicStereoMix;		// XMP Stereo channel separation
	INT				MusicStereoAngle;
	INT				MusicAmplify;		// XMP Amplification factor: 0=Normal, 1=x2, 2=x4, 3=x8
	INT				Latency;
	INT				EffectsChannels;
	INT				FirstChannel;
	INT				LastChannel;
	BYTE			OutputRateNum;	// General output rate for OpenAL
	BYTE			SampleRateNum;	// SampleRate for XMP Music
	ALsizei			OutputRate;
	ALsizei			SampleRate;
	BYTE			MusicVolume;
	BYTE			SoundVolume;
	BYTE			SpeechVolume;
	BYTE			MusicInterpolation;	// XMP Interpolation type
	BYTE			MusicDsp;			// XMP dsp effects
	DWORD			OutputMode;
	FLOAT			AmbientFactor;
	FLOAT			ReverbFactor;
	FLOAT			DopplerFactor;
	UBOOL			UseAutoSampleRate;

	// for Alut and buffering
	ALsizei size;
	ALsizei length;

	// OpenAL Error
	ALint	error;

	// Device selector
	FStringNoInit PreferredDevice;
	TArray<FString> DetectedDevices;

	//HRTF
	BYTE	UseHRTF;

	// Environment configuration variables
	ALubyte szFnName[128];
	UBOOL ReverbZone;
	UBOOL RaytraceZone;
	UBOOL EffectSet;
	ALfloat speedofsound;
	FLOAT ReverbHFDamp;
	BYTE DelayCompare;
	BYTE fog;
	ALuint	Env;
	FLOAT ViewportVolumeIntensity;

#ifdef EFX
	UBOOL EmulateOldReverb;
	FLOAT OldReverbIntensity;
	FLOAT ReverbIntensity;
#endif

	// CombFilter to EFX
	FLOAT RoomSize;
	FLOAT MaxDelay;
	FLOAT MaxGain;
	FLOAT MinDelay;
	FLOAT AvgDelay;
	FLOAT DelayTime[6];
	FLOAT DelayGain[6];
	FLOAT DHFR1;
	FLOAT T;

	INT zonenumber;
	INT zonecompare;
	INT	zoneset;

	ALuint		sid;
	ALuint		bid;
	ALuint      iEffectSlot;

	//EAXZone stuff
	INT			unset;
	INT			unsetinit;
	UBOOL		bSpecialZone;

	ALfloat     Brightness;
	ALfloat		intenvironmentsize;
	ALfloat		intenvironmentdiffusion;
	ALfloat		obstructionlfratio;
	ALint		obstruction;
	ALint		introom;
	ALfloat		airabsorbtionhf;
	ALfloat		Angles[2];

	//Music
	struct		xmp_frame_info xmpfi;
	struct		xmp_module_info xmpmi;
	ov_callbacks callbacks;
	static std::atomic<UBOOL> EndBuffering;

	// Filter
	UBOOL LoadFilterExtension();
	ALboolean FilterExtensionLoaded;

#ifdef EFX
	// EFX Variables
	ALboolean EffectsExtensionLoaded;

	// EFX Helper
	UBOOL ConditionalLoadEffectsExtension();
	UBOOL QueryEffects(FEFXEffects& Effects, AZoneInfo* Zone, AActor* ViewActor);

	// Effect objects
	LPALGENEFFECTS alGenEffects;
	LPALDELETEEFFECTS alDeleteEffects;
	LPALISEFFECT alIsEffect;
	LPALEFFECTI alEffecti;
	LPALEFFECTIV alEffectiv;
	LPALEFFECTF alEffectf;
	LPALEFFECTFV alEffectfv;
	LPALGETEFFECTI alGetEffecti;
	LPALGETEFFECTIV alGetEffectiv;
	LPALGETEFFECTF alGetEffectf;
	LPALGETEFFECTFV alGetEffectfv;

	// Filter objects
	LPALGENFILTERS alGenFilters;
	LPALDELETEFILTERS alDeleteFilters;
	LPALISFILTER alIsFilter;
	LPALFILTERI alFilteri;
	LPALFILTERIV alFilteriv;
	LPALFILTERF alFilterf;
	LPALFILTERFV alFilterfv;
	LPALGETFILTERI alGetFilteri;
	LPALGETFILTERIV alGetFilteriv;
	LPALGETFILTERF alGetFilterf;
	LPALGETFILTERFV alGetFilterfv;

	// Auxiliary slot object
	LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
	LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
	LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
	LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
	LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
	LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
	LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
	LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
	LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
	LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
	LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

	//OpenALSoft specific extensions.
	bool bOpenALSOFT;
	LPALBUFFERSAMPLESSOFT			alBufferSamplesSOFT;
	LPALBUFFERSUBSAMPLESSOFT		alBufferSubSamplesSOFT;
	LPALISBUFFERFORMATSUPPORTEDSOFT alIsBufferFormatSupportedSOFT;
	LPALGETBUFFERSAMPLESSOFT		alGetBufferSamplesSOFT;

#endif

	// Variables.
	UViewport*		Viewport;
	FTIME			LastTime;
	INT				FreeSlot;
	ALCdevice		*Device;
	ALCcontext		*context_id;
	AZoneInfo*		OldAssignedZone;
	BYTE			Ambient;
	BYTE			bPlayingAmbientSound;
	ALAudioSoundInstance PlayingSounds[MAX_EFFECTS_CHANNELS];

	// Listener state
	FVector LastRenderPos;
	FVector LastViewerPos;

	// Music transition variables
	DWORD TransitionState;
	FLOAT TransitionTime, MusicFadeInTime, MusicFadeOutTime, MusicVolumeModifier;
#if !RUNE_CLASSIC
	ALAudioMusicChannel* CurrentSong, * CrossFadeSong;
	UMusic* PendingSong;
	FLOAT PendingSongVolume;
	BYTE CurrentSongSection, PendingSongSection;
#endif


#if RUNE_CLASSIC
	// Rune Classic Raw Streaming variables.
	TArray<ALuint> RawChunkBuffers; // Buffers in use.
	TArray<ALuint> RawChunkBufferPool; // Pool for buffers not currently in use.
	UBOOL IsRawStreaming;
	FLOAT RawStreamStartTime;
	UBOOL IsRawError;
	ALuint RawSource;

	// Rune Classic Raw Streaming helper.
	void InitRaw();
	void UpdateRaw();
	INT RegainChunkBufferPool();
	UBOOL ResizeRawChunkBufferPool(INT Desired);

#endif

	// Constructor.
	UALAudioSubsystem();
	SC_MODIFIER void StaticConstructor(SC_PARAM);

	// FUnknow interface.
#if defined(USE_UNEXT)
	DWORD STDCALL QueryInterface(const FGuid& RefIID, void** InterfacePtr);
#endif

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();

	// Configuration helpers.
	void SetDefaultValues();

	// UAudioSubsystem interface.
	UBOOL Init();
	void SetViewport(UViewport* Viewport);
	UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar); // Han: Removed default argument for KHG.
	void Update(FPointRegion Region, FCoords& Coords);
	void UnregisterSound(USound* Sound);
#if !RUNE_CLASSIC
	void RegisterMusic(UMusic* Music);
	void UnregisterMusic(UMusic* Music);
	static void StopMusic(UMusic* Music);
#endif
#ifdef _WIN32
	static unsigned __stdcall UpdateBuffer(void *data);
#else
	pthread_attr_t NewThreadAttributes;
	static void* UpdateBuffer(void *data);
#endif
	UBOOL PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch);
	UBOOL PlaySound(AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch, INT _Unknown) // Rune Classic.
	{
		guard(UALAudioSubsystem::PlaySound);
		// Perform some logging to get an idea about the _Unknown parameter.
#if 0 // _Unknown seems to be usually 0. Investigate this later, but remove the logspam for now.
		debugf(NAME_DevAudio, TEXT("PlaySound(Actor=%ls,Id=%i,Sound=%ls,Location=(%f,%f,%f),Volume=%f,Radius=%f,Pitch=%f,_Unknown=%i)"), Actor->GetName(), Id, Sound->GetName(), Location.X, Location.Y, Location.Z, Volume, Radius, Pitch, _Unknown);
#endif
		return PlaySound(Actor, Id, Sound, Location, Volume, Radius, Pitch);
		unguard;
	}
	void NoteDestroy(AActor* Actor);
	void RegisterSound(USound* Sound);
	UBOOL GetLowQualitySetting() { return 0; }
	UViewport* GetViewport();
	void RenderAudioGeometry(FSceneNode* Frame);
	void PostRender(FSceneNode* Frame);
	void Flush();
	void StopSound(INT Index);

	// Magic interface.
	void Lipsynch(ALAudioSoundInstance& Playing);

	// Stop Sound support.
	void StopAllSound(); // Rune, Rune Classic.
	void StopSoundId(INT Id); // Deus Ex, Rune.
	UBOOL StopSound(AActor* Actor, USound *Sound); // Rune Classic.

	// Additional SoundId related interfaces.
	UBOOL SoundIdActive(INT Id);
	UBOOL SoundIDActive(INT Id) { return SoundIdActive(Id); }; // Undying.

	// DeusEx special stuff...
#if defined(INCLUDE_AUDIOSUBSYSTEM_SETINSTANT)
	void SetInstantSoundVolume(BYTE newSoundVolume)    { SoundVolume = newSoundVolume; }; // DEUS_EX CNN - instantly sets the sound volume
	void SetInstantSpeechVolume(BYTE newSpeechVolume)  { SpeechVolume = newSpeechVolume; }; // DEUS_EX CNN - instantly sets the speech volume
	void SetInstantMusicVolume(BYTE newMusicVolume)    { MusicVolume = newMusicVolume; }; // DEUS_EX CNN - instantly sets the music volume
#endif

	// Rune Classic Raw Streaming.
#if defined(RUNE_CLASSIC)
	void RawSamples(FLOAT* Data, INT NumSamples, INT NumChannels, INT SampleRate);
	FLOAT GetRawTimeOffset(FLOAT InOffset);
	void StopRawStream();
	void BeginRawStream(INT NumChannels, INT SampleRate);
#endif

#if ENGINE_VERSION==420 || RUNE_CLASSIC
	void CleanUp()
	{
		debugf(NAME_DevAudio, TEXT("CleanUp()"));
		Super::CleanUp();
	};
#endif

	// Names.
	static void RegisterNames();

	// Utils
	AActor* GetCameraActor(); //Not precise
	FVector GetCameraLocation(); //Not precise
	AZoneInfo* GetCameraZone() const;
	FLOAT SoundPriority( UViewport* Viewport, FVector Location, FLOAT Volume, FLOAT Radius);
	FLOAT AttenuationFactor( ALAudioSoundInstance& Playing);
	FLOAT UpdateTime(); //Returns deltatime

	// Inlines.
	Sample* GetSound(USound* Sound)
	{
		check(Sound);
		if (!Sound->Handle)
			RegisterSound(Sound);
		return (Sample*)Sound->Handle;
	}

	// from alhelper, thanks Chris.
	ALsizei FramesToBytes(ALsizei size, ALenum channels, ALenum type)
	{
		switch (channels)
		{
		case AL_MONO_SOFT:    size *= 1; break;
		case AL_STEREO_SOFT:  size *= 2; break;
		case AL_REAR_SOFT:    size *= 2; break;
		case AL_QUAD_SOFT:    size *= 4; break;
		case AL_5POINT1_SOFT: size *= 6; break;
		case AL_6POINT1_SOFT: size *= 7; break;
		case AL_7POINT1_SOFT: size *= 8; break;
		}
		switch (type)
		{
		case AL_BYTE_SOFT:           size *= sizeof(ALbyte); break;
		case AL_UNSIGNED_BYTE_SOFT:  size *= sizeof(ALubyte); break;
		case AL_SHORT_SOFT:          size *= sizeof(ALshort); break;
		case AL_UNSIGNED_SHORT_SOFT: size *= sizeof(ALushort); break;
		case AL_INT_SOFT:            size *= sizeof(ALint); break;
		case AL_UNSIGNED_INT_SOFT:   size *= sizeof(ALuint); break;
		case AL_FLOAT_SOFT:          size *= sizeof(ALfloat); break;
		case AL_DOUBLE_SOFT:         size *= sizeof(ALdouble); break;
		}
		return size;
	}
	ALsizei BytesToFrames(ALsizei size, ALenum channels, ALenum type)
	{
		return size / FramesToBytes(1, channels, type);
	}
};


/*------------------------------------------------------------------------------------
	Inlines
------------------------------------------------------------------------------------*/

//
// Decode Slot from a sound Id
//
inline INT IdToSlot( INT Id)
{
	return (Id & 0b1110) >> 1;
}

//
// Get head/foot region, if applicable.
//
inline const FPointRegion& GetRegion( AActor* Actor, INT RegionType=0)
{
	if (Actor->bIsPawn)
	{
		if ( RegionType > 0 )
			return reinterpret_cast<APawn*>(Actor)->HeadRegion;
		if ( RegionType < 0 )
			return reinterpret_cast<APawn*>(Actor)->FootRegion;
	}
	return Actor->Region;
}

inline INT ALAudioSoundInstance::GetSlot() const
{
	return IdToSlot(Id);
}
inline UBOOL ALAudioSoundInstance::IsPlayingSound(USound* S) const
{
	guardSlow(ALAudioSoundInstance::IsPlayingSound);
	return (S && Sound && Sound == S->Handle);
	unguardSlow;
}

inline FString FMTRAN_FormatString(BYTE Format)
{
	guard(FMTRANFormatString);
	switch (Format)
	{
		// Original.
		case MTRAN_None:        return TEXT("MTRAN_None");
		case MTRAN_Instant:     return TEXT("MTRAN_Instant");
		case MTRAN_Segue:       return TEXT("MTRAN_Segue");
		case MTRAN_Fade:        return TEXT("MTRAN_Fade");
        case MTRAN_FastFade:    return TEXT("MTRAN_FastFade");
		case MTRAN_SlowFade:    return TEXT("MTRAN_SlowFade");

        // MAX.
		case MTRAN_MAX:			return TEXT("MTRAN_MAX");

		// Unknown.
		default:                return FString::Printf(TEXT("Unknown(%i)"),Format);

	}
	unguard;
}
