/*=============================================================================
ALAudioSubsystem.cpp: Unreal OpenAL Audio interface object.
Copyright 2015 Oldunreal.

Revision history:
* Old initial version by Joseph I. Valenzuela (based on AudioSubsystem.cpp)
* Heavily modified and completely rewritten by Jochen 'Smirftsch' Görnitz
* Porting and porting help for other UEngine1 games, code cleanups, efx refactoring and lipsynch by Sebastian 'Hanfling' Kaufel
=============================================================================*/

/*------------------------------------------------------------------------------------
Audio includes.
------------------------------------------------------------------------------------*/

#ifdef _WIN32
#include <comutil.h>
#else
#include <pthread.h>
#include <errno.h>
#endif
#include "ALAudio.h"
#include "WaveFormat.h"
#include "OggSupport.h"
#define EFX

#ifdef EFX
#pragma warning(push)
#pragma warning(disable:4305) // double to float truncation
EFXEAXREVERBPROPERTIES g_ReverbPreset[] = {
	REVERB_PRESET_GENERIC,
	REVERB_PRESET_PADDEDCELL,
	REVERB_PRESET_ROOM,
	REVERB_PRESET_BATHROOM,
	REVERB_PRESET_LIVINGROOM,
	REVERB_PRESET_STONEROOM,
	REVERB_PRESET_AUDITORIUM,
	REVERB_PRESET_CONCERTHALL,
	REVERB_PRESET_CAVE,
	REVERB_PRESET_ARENA,
	REVERB_PRESET_HANGAR,
	REVERB_PRESET_CARPETTEDHALLWAY,
	REVERB_PRESET_HALLWAY,
	REVERB_PRESET_STONECORRIDOR,
	REVERB_PRESET_ALLEY,
	REVERB_PRESET_FOREST,
	REVERB_PRESET_CITY,
	REVERB_PRESET_MOUNTAINS,
	REVERB_PRESET_QUARRY,
	REVERB_PRESET_PLAIN,
	REVERB_PRESET_PARKINGLOT,
	REVERB_PRESET_SEWERPIPE,
	REVERB_PRESET_UNDERWATER,
	REVERB_PRESET_DRUGGED,
	REVERB_PRESET_DIZZY,
	REVERB_PRESET_PSYCHOTIC,
	REVERB_PRESET_CASTLE_SMALLROOM,
	REVERB_PRESET_CASTLE_SHORTPASSAGE,
	REVERB_PRESET_CASTLE_MEDIUMROOM,
	REVERB_PRESET_CASTLE_LONGPASSAGE,
	REVERB_PRESET_CASTLE_LARGEROOM,
	REVERB_PRESET_CASTLE_HALL,
	REVERB_PRESET_CASTLE_CUPBOARD,
	REVERB_PRESET_CASTLE_COURTYARD,
	REVERB_PRESET_CASTLE_ALCOVE,
	REVERB_PRESET_FACTORY_ALCOVE,
	REVERB_PRESET_FACTORY_SHORTPASSAGE,
	REVERB_PRESET_FACTORY_MEDIUMROOM,
	REVERB_PRESET_FACTORY_LONGPASSAGE,
	REVERB_PRESET_FACTORY_LARGEROOM,
	REVERB_PRESET_FACTORY_HALL,
	REVERB_PRESET_FACTORY_CUPBOARD,
	REVERB_PRESET_FACTORY_COURTYARD,
	REVERB_PRESET_FACTORY_SMALLROOM,
	REVERB_PRESET_ICEPALACE_ALCOVE,
	REVERB_PRESET_ICEPALACE_SHORTPASSAGE,
	REVERB_PRESET_ICEPALACE_MEDIUMROOM,
	REVERB_PRESET_ICEPALACE_LONGPASSAGE,
	REVERB_PRESET_ICEPALACE_LARGEROOM,
	REVERB_PRESET_ICEPALACE_HALL,
	REVERB_PRESET_ICEPALACE_CUPBOARD,
	REVERB_PRESET_ICEPALACE_COURTYARD,
	REVERB_PRESET_ICEPALACE_SMALLROOM,
	REVERB_PRESET_SPACESTATION_ALCOVE,
	REVERB_PRESET_SPACESTATION_MEDIUMROOM,
	REVERB_PRESET_SPACESTATION_SHORTPASSAGE,
	REVERB_PRESET_SPACESTATION_LONGPASSAGE,
	REVERB_PRESET_SPACESTATION_LARGEROOM,
	REVERB_PRESET_SPACESTATION_HALL,
	REVERB_PRESET_SPACESTATION_CUPBOARD,
	REVERB_PRESET_SPACESTATION_SMALLROOM,
	REVERB_PRESET_WOODEN_ALCOVE,
	REVERB_PRESET_WOODEN_SHORTPASSAGE,
	REVERB_PRESET_WOODEN_MEDIUMROOM,
	REVERB_PRESET_WOODEN_LONGPASSAGE,
	REVERB_PRESET_WOODEN_LARGEROOM,
	REVERB_PRESET_WOODEN_HALL,
	REVERB_PRESET_WOODEN_CUPBOARD,
	REVERB_PRESET_WOODEN_SMALLROOM,
	REVERB_PRESET_WOODEN_COURTYARD,
	REVERB_PRESET_SPORT_EMPTYSTADIUM,
	REVERB_PRESET_SPORT_SQUASHCOURT,
	REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL,
	REVERB_PRESET_SPORT_LARGESWIMMINGPOOL,
	REVERB_PRESET_SPORT_GYMNASIUM,
	REVERB_PRESET_SPORT_FULLSTADIUM,
	REVERB_PRESET_SPORT_STADIUMTANNOY,
	REVERB_PRESET_PREFAB_WORKSHOP,
	REVERB_PRESET_PREFAB_SCHOOLROOM,
	REVERB_PRESET_PREFAB_PRACTISEROOM,
	REVERB_PRESET_PREFAB_OUTHOUSE,
	REVERB_PRESET_PREFAB_CARAVAN,
	REVERB_PRESET_DOME_TOMB,
	REVERB_PRESET_PIPE_SMALL,
	REVERB_PRESET_DOME_SAINTPAULS,
	REVERB_PRESET_PIPE_LONGTHIN,
	REVERB_PRESET_PIPE_LARGE,
	REVERB_PRESET_PIPE_RESONANT,
	REVERB_PRESET_OUTDOORS_BACKYARD,
	REVERB_PRESET_OUTDOORS_ROLLINGPLAINS,
	REVERB_PRESET_OUTDOORS_DEEPCANYON,
	REVERB_PRESET_OUTDOORS_CREEK,
	REVERB_PRESET_OUTDOORS_VALLEY,
	REVERB_PRESET_MOOD_HEAVEN,
	REVERB_PRESET_MOOD_HELL,
	REVERB_PRESET_MOOD_MEMORY,
	REVERB_PRESET_DRIVING_COMMENTATOR,
	REVERB_PRESET_DRIVING_PITGARAGE,
	REVERB_PRESET_DRIVING_INCAR_RACER,
	REVERB_PRESET_DRIVING_INCAR_SPORTS,
	REVERB_PRESET_DRIVING_INCAR_LUXURY,
	REVERB_PRESET_DRIVING_FULLGRANDSTAND,
	REVERB_PRESET_DRIVING_EMPTYGRANDSTAND,
	REVERB_PRESET_DRIVING_TUNNEL,
	REVERB_PRESET_CITY_STREETS,
	REVERB_PRESET_CITY_SUBWAY,
	REVERB_PRESET_CITY_MUSEUM,
	REVERB_PRESET_CITY_LIBRARY,
	REVERB_PRESET_CITY_UNDERPASS,
	REVERB_PRESET_CITY_ABANDONED,
	REVERB_PRESET_DUSTYROOM,
	REVERB_PRESET_CHAPEL,
	REVERB_PRESET_SMALLWATERROOM,
	REVERB_PRESET_UNDERSLIME,
	REVERB_PRESET_NONE
};
#pragma warning(pop)

EFXEAXREVERBPROPERTIES *GetEFXReverb(DWORD enumVal)
{
	//range check
	if (enumVal >= ARRAY_COUNT(g_ReverbPreset))
		enumVal = ARRAY_COUNT(g_ReverbPreset) - 1; // Han: Use REVERB_PRESET_NONE now to be more consistent.
	return &g_ReverbPreset[enumVal];
}
#endif

//static const TCHAR *g_pSection = TEXT("ALAudio.ALAudioSubsystem");

UALAudioSubsystem* UALAudioSubsystem::GALAudio = NULL;

/*------------------------------------------------------------------------------------
Internals and helpers. han: maybe we should put them in class scope.
------------------------------------------------------------------------------------*/

static FLOAT DeltaTime = 0.f; //Ugly

inline bool sourceIsPlaying(ALuint sid)
{
	ALint state;
	alGetSourcei(sid, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}
inline void TransformCoordinates(ALfloat Dest[3], const FVector &Source)
{
	Dest[0] = Source.X;
	Dest[1] = Source.Y;
	Dest[2] = -Source.Z;
}

static bool FillSingleBuffer(ALAudioMusicHandle *MusicHandle, char* OggBufferData)
{
	DWORD size = 0;
	INT   section;
	DWORD result;

	while (size < MUSIC_BUFFER_SIZE)
	{
		result = ov_read(MusicHandle->OggStream, OggBufferData + size, MUSIC_BUFFER_SIZE - size, 0, 2, 1, &section);

		INT CurrentTime = ov_time_tell(MusicHandle->OggStream);
		INT TotalTime = ov_time_total(MusicHandle->OggStream, -1);
		//debugf(TEXT("%i %i"),CurrentTime,TotalTime);
		if (CurrentTime == TotalTime) //simple loop.
			ov_time_seek(MusicHandle->OggStream, 0);

		if (result > 0)
			size += result;
		else break;
	}

	if (size == 0)
		return 0;

	return 1;
}
static bool FillSingleSoundBuffer(OggVorbis_File *OggStream, char OggBufferData[SOUND_BUFFER_SIZE])
{
	DWORD size = 0;
	INT   section;
	DWORD result;

	while (size < SOUND_BUFFER_SIZE)
	{
		result = ov_read(OggStream, OggBufferData + size, SOUND_BUFFER_SIZE - size, 0, 2, 1, &section);

		if (result > 0)
			size += result;
		else break;
	}

	if (size == 0)
		return 0;

	return 1;
}

// Prints aligned HelpLine assuming 25 chars is enough.
void PrintAlignedHelpLine(FOutputDevice& Ar, const TCHAR* Cmd, const TCHAR* Help = TEXT(""))
{
	Ar.Logf(TEXT("%25ls - %ls"), Cmd, Help);
}

// Calls UnrealScript Event.
UBOOL ProcessScript(UObject* Object, FName Event, void* Parms = NULL, UBOOL bChecked = 0)
{
	check(Object);
	UFunction* Function = bChecked ? Object->FindFunctionChecked(Event) : Object->FindFunction(Event);
	if (Function == NULL)
		return 0;
	Object->ProcessEvent(Function, Parms);
	return 1;
}

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(_MSC_VER)
void TimerBegin()
{
	// This will increase the precision of the kernel interrupt
	// timer. Although this will slightly increase resource usage
	// this will also increase the precision of sleep calls and
	// this will in turn increase the stability of the framerate
	timeBeginPeriod(1);
}
void TimerEnd()
{
	// Restore the kernel timer config
	timeEndPeriod(1);
}
#endif


/*------------------------------------------------------------------------------------
UALAudioSubsystem.
------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UALAudioSubsystem);

UALAudioSubsystem::UALAudioSubsystem()
{
	guard(UALAudioSubsystem::UALAudioSubsystem);

	UpdateTime();
	OldAssignedZone = NULL;
	Initialized = 0;

#if defined(EFX)
	EffectsExtensionLoaded = false;
#endif
	FilterExtensionLoaded = false;

	unguard;
};

/*------------------------------------------------------------------------------------
OpenAL device selection
------------------------------------------------------------------------------------*/

void UALAudioSubsystem::StaticConstructor(SC_PARAM)
{
	guard(UALAudioSubsystem::StaticConstructor);

	RegisterNames();

	// Generate frequencies enum.
	UEnum* OutputRates = new(SC_GETCLASS, TEXT("OutputRates"))UEnum(NULL);
	new(OutputRates->Names)FName(TEXT("8000Hz"));
	new(OutputRates->Names)FName(TEXT("11025Hz"));
	new(OutputRates->Names)FName(TEXT("16000Hz"));
	new(OutputRates->Names)FName(TEXT("22050Hz"));
	new(OutputRates->Names)FName(TEXT("32000Hz"));
	new(OutputRates->Names)FName(TEXT("44100Hz"));
	new(OutputRates->Names)FName(TEXT("48000Hz"));
	new(OutputRates->Names)FName(TEXT("96000Hz"));
	new(OutputRates->Names)FName(TEXT("192000Hz"));

#if !RUNE_CLASSIC
	// Enum for libxmp sampling rate.
	UEnum* SampleRates = new(SC_GETCLASS, TEXT("SampleRates"))UEnum(NULL);
	new(SampleRates->Names)FName(TEXT("8000Hz"));
	new(SampleRates->Names)FName(TEXT("11025Hz"));
	new(SampleRates->Names)FName(TEXT("16000Hz"));
	new(SampleRates->Names)FName(TEXT("22050Hz"));
	new(SampleRates->Names)FName(TEXT("32000Hz"));
	new(SampleRates->Names)FName(TEXT("44100Hz"));
	new(SampleRates->Names)FName(TEXT("48000Hz"));

	// Generate list of interpolation types for libxmp.
	UEnum* MusicInterpolationTypes = new(SC_GETCLASS, TEXT("MusicInterpolationTypes"))UEnum(NULL);
	new(MusicInterpolationTypes->Names)FName(TEXT("NEAREST")); // Nearest neighbor.
	new(MusicInterpolationTypes->Names)FName(TEXT("LINEAR")); // Linear.
	new(MusicInterpolationTypes->Names)FName(TEXT("SPLINE")); // Cubic spline.

	//Generate list of DSP Filtereffect for libxmp
	UEnum* MusicDspTypes = new(SC_GETCLASS, TEXT("DSP Filtereffect"))UEnum(NULL);
	new (MusicDspTypes->Names)FName(TEXT("DSP_LOWPASS"));
	new (MusicDspTypes->Names)FName(TEXT("DSP_ALL"));
#endif // !RUNE_CLASSIC

	// Device selection and enumeration
	new(SC_GETCLASS, TEXT("PreferredDevice"), RF_Public) UStrProperty(CPP_PROPERTY(PreferredDevice), TEXT("Audio"), CPF_Config);
	UArrayProperty* A = new(SC_GETCLASS, TEXT("DetectedDevices"), RF_Public) UArrayProperty(CPP_PROPERTY(DetectedDevices), TEXT("Audio"), CPF_Config);
	A->Inner = new(A, TEXT("StrProperty0"), RF_Public) UStrProperty;

	// General options.
#if !RUNE_CLASSIC
	new(SC_GETCLASS, TEXT("SampleRate"), RF_Public)UByteProperty(CPP_PROPERTY(SampleRateNum), TEXT("Audio"), CPF_Config, SampleRates); // Should be moved to XMP specific options.
#endif
	new(SC_GETCLASS, TEXT("SoundVolume"), RF_Public)UByteProperty(CPP_PROPERTY(SoundVolume), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("SpeechVolume"), RF_Public)UByteProperty(CPP_PROPERTY(SpeechVolume), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("MusicVolume"), RF_Public)UByteProperty(CPP_PROPERTY(MusicVolume), TEXT("Audio"), CPF_Config);
#if ENGINE_VERSION!=1100
	new(SC_GETCLASS, TEXT("UseSpeechVolume"), RF_Public)UBoolProperty(CPP_PROPERTY(UseSpeechVolume), TEXT("Audio"), CPF_Config);
#endif
	new(SC_GETCLASS, TEXT("EffectsChannels"), RF_Public)UIntProperty(CPP_PROPERTY(EffectsChannels), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("UseDigitalMusic"), RF_Public)UBoolProperty(CPP_PROPERTY(UseDigitalMusic), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("AmbientFactor"), RF_Public)UFloatProperty(CPP_PROPERTY(AmbientFactor), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("bSoundAttenuate"), RF_Public)UBoolProperty(CPP_PROPERTY(bSoundAttenuate), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("DopplerFactor"), RF_Public)UFloatProperty(CPP_PROPERTY(DopplerFactor), TEXT("Audio"), CPF_Config);
	//new(SC_GETCLASS, TEXT("UseOriginalUnreal"), RF_Public)UBoolProperty(CPP_PROPERTY(UseOriginalUnreal), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("OutputRate"), RF_Public)UByteProperty(CPP_PROPERTY(OutputRateNum), TEXT("Audio"), CPF_Config, OutputRates);
	new(SC_GETCLASS, TEXT("UseAutoSampleRate"), RF_Public)UBoolProperty(CPP_PROPERTY(UseAutoSampleRate), TEXT("Audio"), CPF_Config);

	// Misc.
	new(SC_GETCLASS, TEXT("ProbeDevicesOnly"), RF_Public)UBoolProperty(CPP_PROPERTY(ProbeDevicesOnly), TEXT("Audio"), CPF_Config);

#if !RUNE_CLASSIC
	// XMP specific options.
	new(SC_GETCLASS, TEXT("MusicInterpolation"), RF_Public)UByteProperty(CPP_PROPERTY(MusicInterpolation), TEXT("Audio"), CPF_Config, MusicInterpolationTypes);
	new(SC_GETCLASS, TEXT("MusicDsp"), RF_Public)UByteProperty(CPP_PROPERTY(MusicDsp), TEXT("Audio"), CPF_Config, MusicDspTypes);
	new(SC_GETCLASS, TEXT("MusicPanSeparation"), RF_Public)UIntProperty(CPP_PROPERTY(MusicPanSeparation), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("MusicStereoMix"), RF_Public)UIntProperty(CPP_PROPERTY(MusicStereoMix), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("MusicStereoAngle"), RF_Public)UIntProperty(CPP_PROPERTY(MusicStereoAngle), TEXT("Audio"), CPF_Config);
#endif // !RUNE_CLASSIC
	new(SC_GETCLASS, TEXT("MusicAmplify"), RF_Public)UIntProperty(CPP_PROPERTY(MusicAmplify), TEXT("Audio"), CPF_Config);

	// EFX specific options.
#if defined(EFX)
	new(SC_GETCLASS, TEXT("EmulateOldReverb"), RF_Public)UBoolProperty(CPP_PROPERTY(EmulateOldReverb), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("OldReverbIntensity"), RF_Public)UFloatProperty(CPP_PROPERTY(OldReverbIntensity), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("UseReverb"), RF_Public)UBoolProperty(CPP_PROPERTY(UseReverb), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("ReverbIntensity"), RF_Public)UFloatProperty(CPP_PROPERTY(ReverbIntensity), TEXT("Audio"), CPF_Config);
#endif
	new(SC_GETCLASS, TEXT("DetailStats"), RF_Public)UBoolProperty(CPP_PROPERTY(DetailStats), TEXT("Audio"), CPF_Config);
	new(SC_GETCLASS, TEXT("ViewportVolumeIntensity"), RF_Public)UFloatProperty(CPP_PROPERTY(ViewportVolumeIntensity), TEXT("Audio"), CPF_Config);

	// HRTF specific options.
#if defined(HRTF)
	UEnum* HRTFRequest = new(SC_GETCLASS, TEXT("HRTFRequest"))UEnum(NULL);
	new(HRTFRequest->Names)FName(TEXT("Autodetect"));
	new(HRTFRequest->Names)FName(TEXT("Enable"));
	new(HRTFRequest->Names)FName(TEXT("Disable"));

	new(SC_GETCLASS, TEXT("UseHRTF"), RF_Public)UByteProperty(CPP_PROPERTY(UseHRTF), TEXT("Audio"), CPF_Config, HRTFRequest);
#endif

#if !defined(LEGACY_STATIC_CONSTRUCTOR)
	SetDefaultValues();
#endif

	unguard;
}

void UALAudioSubsystem::SetDefaultValues()
{
	// General options.
	OutputRateNum = 5; // 44100Hz
	SampleRateNum = 5; // 44100Hz
	SoundVolume = 200;
	SpeechVolume = 200;
	MusicVolume = 120;
	EffectsChannels = 64;
	AmbientFactor = 0.7f;
	DopplerFactor = 0.01f;
	bSoundAttenuate = 1;
	UseDigitalMusic = 1;
	UseSpeechVolume = 1;
	UseAutoSampleRate = 1;
	MusicStereoAngle = 30;

	//HRTF control.
	UseHRTF = 0;

#if !RUNE_CLASSIC
	// XMP specific options.
	MusicInterpolation = 2; // Cubic spline.
	MusicDsp = 1; // DSP_ALL.
	MusicPanSeparation = 50;
	MusicStereoMix = 70;
	MusicAmplify = 0; // Normal.
#endif

	// Misc.
	ProbeDevicesOnly = 0;

	// EFX specific options.
#if defined(EFX)
	// Drasticaly reduce old reverb intensity for dx.
#if ENGINE_VERSION==1100
	OldReverbIntensity = 0.2f;
#else
	OldReverbIntensity = 1.0f;
	ReverbIntensity = 1.0f;
#endif
	ViewportVolumeIntensity = 1.0f;
	UseReverb = 1;
	EmulateOldReverb = 1;
#endif
}

/*------------------------------------------------------------------------------------
FUnknown Interface.
------------------------------------------------------------------------------------*/

#if defined(USE_UNEXT)
DWORD STDCALL UALAudioSubsystem::QueryInterface(const FGuid& RefIID, void** InterfacePtr)
{
	if (RefIID == IID_IHumanHeadAudioSubsystem)
	{
		*(IHumanHeadAudioSubsystem**)InterfacePtr = this;
		return 1;
	}
	return 0;
}
#endif

/*------------------------------------------------------------------------------------
UObject Interface.
------------------------------------------------------------------------------------*/

void UALAudioSubsystem::PostEditChange()
{
	guard(UALAudioSubsystem::PostEditChange);

	// General options.
	OutputRateNum = Clamp(OutputRateNum, (BYTE)0, (BYTE)8);
	SampleRateNum = Clamp(SampleRateNum, (BYTE)0, (BYTE)6);
	MusicVolume = Clamp(MusicVolume, (BYTE)0, (BYTE)255);
	SoundVolume = Clamp(SoundVolume, (BYTE)0, (BYTE)255);
	SpeechVolume = Clamp(SpeechVolume, (BYTE)0, (BYTE)255);
	EffectsChannels = Clamp(EffectsChannels, 1, MAX_EFFECTS_CHANNELS);
	AmbientFactor = Clamp(AmbientFactor, 0.f, 10.f);
	DopplerFactor = Clamp(DopplerFactor, 0.f, 10.f);

#if !RUNE_CLASSIC
	// XMP specific options.
	MusicDsp = Clamp(MusicDsp, (BYTE)0, (BYTE)3);
	MusicInterpolation = Clamp(MusicInterpolation, (BYTE)0, (BYTE)2);
	MusicPanSeparation = Clamp(MusicPanSeparation, 0, 100);
	MusicStereoMix = Clamp(MusicStereoMix, 0, 100);
#endif
	MusicAmplify = Clamp(MusicAmplify, 0, 3);

#if defined(EFX)
	// EFX specific options.
	OldReverbIntensity = Clamp(OldReverbIntensity, 0.f, 10.f);
	ReverbIntensity = Clamp(ReverbIntensity, 0.f, 10.f);

	// Make sure Effects Extension is loaded if someone enabled Reverb.
	if (!ConditionalLoadEffectsExtension())
		EffectsExtensionLoaded = false;
#endif

	// Load filter
	if (!LoadFilterExtension())
	{
		FilterExtensionLoaded = false;
		bSoundAttenuate = 0;
	}

	ViewportVolumeIntensity = Clamp(ViewportVolumeIntensity, 0.f, 5.f);

	MusicStereoAngle = Clamp(MusicStereoAngle, (INT) 0, (INT)360); // 360°

	Angles[0] = MusicStereoAngle * PI / 180;
	Angles[1] = -Angles[0];

	unguard;
}

/*------------------------------------------------------------------------------------
UAudioSubsystem Interface.
------------------------------------------------------------------------------------*/

UBOOL UALAudioSubsystem::Init()
{
	guard(UALAudioSubsystem::Init);

	GALAudio = this;

	// Enumerate OpenAL devices
	guard(ProbeDevices);
	const ALCchar* deviceList = NULL;
	try
	{
		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE)
			deviceList = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER); // No idea why it fails specifically here is using a library with invalid instruction set, but better catching it to show at least a proper error message.
		else
			deviceList = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	}
	catch (...)
	{
		appErrorf(TEXT("Your CPU doesn't support whatever enhanced instruction set (SSE2, SSE3, SSE4, AVX, ...) was used to build the OpenAL library with. Try a different OpenAL32.dll/libopenal.so."));
	}

	DetectedDevices.Empty();
	while (deviceList && *deviceList)
	{
		new(DetectedDevices) FString(appFromAnsi(deviceList));
		deviceList += strlen(deviceList) + 1;
	}
	SaveConfig();
	unguard;


	if (ProbeDevicesOnly)
		return false;

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(_MSC_VER)
	TimerBegin();
#endif

	/*
	debugf(TEXT("UseReverb %i"), UseReverb);
	debugf(TEXT("OldReverbIntensity %f"), OldReverbIntensity);
	debugf(TEXT("EmulateOldReverb %i"), EmulateOldReverb);
	debugf(TEXT("MusicAmplify %i"), MusicAmplify);
	debugf(TEXT("ProbeDevicesOnly %i"), ProbeDevicesOnly);
	debugf(TEXT("MusicStereoMix %i"), MusicStereoMix);
	debugf(TEXT("MusicPanSeparation %i"), MusicPanSeparation);
	debugf(TEXT("MusicDsp %i"), MusicDsp);
	debugf(TEXT("MusicInterpolation %i"), MusicInterpolation);
	debugf(TEXT("DopplerFactor %f"), DopplerFactor);
	debugf(TEXT("bSoundAttenuate %i"), bSoundAttenuate);
	debugf(TEXT("AmbientFactor %f"), AmbientFactor);
	debugf(TEXT("UseDigitalMusic %i"), UseDigitalMusic);
	debugf(TEXT("EffectsChannels %i"), EffectsChannels);
	debugf(TEXT("UseSpeechVolume %i"), UseSpeechVolume);
	debugf(TEXT("MusicVolume %i"), MusicVolume);
	debugf(TEXT("SpeechVolume %i"), SpeechVolume);
	debugf(TEXT("SoundVolume %i"), SoundVolume);
	debugf(TEXT("SampleRate %i"), SampleRate);
	debugf(TEXT("OutputRate %i"), OutputRate);
	debugf(TEXT("ALDevice %i"), ALDevice);
	*/

	// Validate ini settings...
	PostEditChange();

	// I want to get rid of this maintaining indices by hand. --han
#define ATTRLIST_FREQUENCY 1
#define ATTRLIST_REFRESH   3
#define ATTRLIST_HRTF      5

	ALCint AttrList[32];
	appMemzero(AttrList, sizeof(AttrList));

	// Set specifies.
	AttrList[0] = ALC_FREQUENCY;
	AttrList[2] = ALC_REFRESH;
#if defined (HRTF)
	AttrList[4] = ALC_HRTF_SOFT;
#endif

	// Set default values.
	AttrList[ATTRLIST_FREQUENCY] = 44100;
	AttrList[ATTRLIST_REFRESH] = 60;
#if defined (HRTF)
	AttrList[ATTRLIST_HRTF] = ALC_DONT_CARE_SOFT;
#endif

	// Set requested output rate for OpenAL.
	INT Rates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000, 192000 };
	AttrList[ATTRLIST_FREQUENCY] = OutputRate = Rates[OutputRateNum];
	OutputRate = Rates[OutputRateNum];

#if defined (HRTF)

	/*
	Specifying the value for ALC_HRTF_SOFT of ALC_FALSE will request no HRTF mixing. The default
	value of ALC_DONT_CARE_SOFT will allow the AL to determine for itself
	whether HRTF should be used or not (depending on the detected device port
	or form factor, format, etc).
	*/
	switch (UseHRTF)
	{
		// Force enable.
	case 1:
		AttrList[ATTRLIST_HRTF] = ALC_TRUE;
		AudioLog(NAME_Init, TEXT("ALAudio: Trying to enable HRTF extension"));
		break;

		// Force disable.
	case 2:
		AttrList[ATTRLIST_HRTF] = ALC_FALSE;
		break;

		// Let ALSoft do whatever it pleases.
	default:
		AttrList[ATTRLIST_HRTF] = ALC_DONT_CARE_SOFT;
		AudioLog(NAME_Init, TEXT("ALAudio: Trying to autodetect HRTF. Note: Autodetection may only work with USB headphones."));
		break;
	}
#endif

#if !RUNE_CLASSIC
	// SampleRate for XMP
	INT XMPRates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000 };
	SampleRate = XMPRates[SampleRateNum];
#endif

	// Stereo Angle
	Angles[0] = MusicStereoAngle * PI / 180;
	Angles[1] = -Angles[0];

	/*------------------------------------------------------------------------------------
	Start of OpenAL init
	------------------------------------------------------------------------------------*/

	// Initialize OpenAL Audio library.
	Device = nullptr;

	// Select preferred device.
	if (PreferredDevice != TEXT(""))
	{
		AudioLog(NAME_DevAudio, TEXT("ALAudio: Trying to use preferred device: %ls"), *PreferredDevice);
		Device = alcOpenDevice(appToAnsi(*PreferredDevice));
	}

	if (!Device)
	{
		AudioLog(NAME_DevAudio, TEXT("ALAudio: Trying to use default device"));
		Device = alcOpenDevice(NULL);
	}

	if (!Device)
	{
		GWarn->Logf(TEXT("ALAudio: Failed to Initialize Open AL device (alcOpenDevice)"));
		return false;
	}

	AudioLog(NAME_Init, TEXT("ALAudio: We are using OpenAL device: %ls "), appFromAnsi(alcGetString(Device, ALC_DEVICE_SPECIFIER)));
	AudioLog(NAME_Init, TEXT("ALAudio: OutputRateNum %i(%i Hz) SampleRateNum %i (%i Hz)"), OutputRateNum, OutputRate, SampleRateNum, SampleRate);

	context_id = alcCreateContext(Device, AttrList);
	// context_id = alcCreateContext(Device,NULL); // for testing -  NULL is perfectly valid

	if (context_id == NULL) {
		GWarn->Logf(TEXT("ALAudio: Failed to create context (alcCreateContext)"));
		return false;
	}
	alcMakeContextCurrent(context_id);
	if (alcGetError(Device) != ALC_NO_ERROR)
	{
		GWarn->Logf(TEXT("ALAudio: Failed to make context current (alcMakeContextCurrent)"));
		alcDestroyContext(context_id);
		context_id = NULL;
		alcCloseDevice(Device);
		return false;
	}

	ALsizei Freq = ALC_FALSE;
	alcGetIntegerv(Device, ALC_FREQUENCY, 1, &Freq);
	if (OutputRate != Freq)
	{
		AudioLog(NAME_Init, TEXT("ALAudio: Can't set OutputRate of %i, using system (OS) defaults of %i.\r\n")
			TEXT("Some backends don't support setting custom rates for specific applications (MMDevAPI or ALSA without dmix).\r\n")
			TEXT("In this case setting something higher than what the system is outputting is unnecessary since it's just going to be downsampled for output anyway,\r\n")
			TEXT("and setting it lower is just going to make it get upsampled after OpenAL Soft has already applied its filters and effects.\r\n")
			TEXT("Adjust your system settings instead."), OutputRate, Freq);
		OutputRate = Freq;
	}

	ALsizei MonoSources = ALC_FALSE, StereoSources = ALC_FALSE;
	alcGetIntegerv(Device, ALC_MONO_SOURCES, 1, &MonoSources);
	alcGetIntegerv(Device, ALC_STEREO_SOURCES, 1, &StereoSources);
	AudioLog(NAME_Init, TEXT("ALAudio: Audio hardware supports %i mono (usually sound) and %i stereo (usually music) sources."), MonoSources, StereoSources);
	if (EffectsChannels > MonoSources)
	{
		AudioLog(NAME_Init, TEXT("ALAudio: Reducing EffectsChannels to %i sources."), MonoSources);
		EffectsChannels = MonoSources;
		// While providing usually 1 stereo source, hardware devices will use 2 voices for a stereo sound with 1 voice for mono.
		// OpenALSoft lets you play whatever as long as you have a source to play it.
	}

#if !RUNE_CLASSIC
	if (UseAutoSampleRate)
	{
		UBOOL Matched = 0;

		// Search for output rate in xmp sample rate array. This is probably the best thing to happen.
		for (INT i = 0; !Matched && i<ARRAY_COUNT(XMPRates); i++)
		{
			if (XMPRates[i] == OutputRate)
			{
				SampleRate = XMPRates[i];
				Matched = 1;
			}
		}

		// The current ALAudio sample rate is not supported by libxmp, so try finding a good match (e.g. 48000 to 96000).
		for (INT i = ARRAY_COUNT(XMPRates) - 1; !Matched && i >= 0; i--)
		{
			if ((OutputRate % XMPRates[i]) == 0)
			{
				SampleRate = XMPRates[i];
				Matched = 1;
			}
		}

		if (Matched)
			AudioLog(NAME_Init, TEXT("ALAudio: auto matched SampleRate (%i Hz) to OutputRate (%i Hz)."), SampleRate, OutputRate);
		else
			AudioLog(NAME_Init, TEXT("ALAudio: failed to automatch SampleRate to OutputRate (%i Hz)."), OutputRate);
	}
#endif

	//OpenALSoft specific extensions.
	if (alcIsExtensionPresent(Device, "ALC_SOFT_HRTF") != ALC_TRUE)
	{
		GWarn->Logf(TEXT("ALAudio: No OpenALSoft extension ALC_SOFT_HRTF present!"));
		bOpenALSOFT = false;
	}
	else
	{
		debugf(NAME_Init, TEXT("ALAudio: OpenALSoft extension ALC_SOFT_HRTF found."));
		bOpenALSOFT = true;
	}

#if defined (HRTF)
	ALsizei hrtf = ALC_HRTF_DISABLED_SOFT;
	if (bOpenALSOFT)
		alcGetIntegerv(Device, ALC_HRTF_STATUS_SOFT, 1, &hrtf);

	switch (hrtf)
	{
	case ALC_HRTF_DISABLED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled."));
		break;
	case ALC_HRTF_ENABLED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is enabled."));
		break;
	case ALC_HRTF_DENIED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled because it's not allowed on the	device. This may be caused by invalid resource permissions, or other user configuration that disallows HRTF."));
		break;
	case ALC_HRTF_REQUIRED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is enabled because it must be used on the device. This may be caused by a device that can only use HRTF, or other user configuration (like alsoft.ini or .alsoftrc) that forces HRTF to be used."));
		break;
	case ALC_HRTF_HEADPHONES_DETECTED_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is enabled because the device reported headphones."));
		break;
	case ALC_HRTF_UNSUPPORTED_FORMAT_SOFT:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled because the device does not support it with the current	format. Typically this is caused by non - stereo output or an incompatible output frequency."));
		break;
	default:
		AudioLog(NAME_Init, TEXT("ALAudio: HRTF is disabled."));
	}
	AudioLog(NAME_DevAudio, TEXT("ALAudio: Initialized with rate %i"), OutputRate);
#endif

#ifdef ALURE
	/*
	if (alureInitDevice(NULL,Attrlist) == AL_FALSE)
	GWarn->Logf(TEXT("ALAudio: alureInitDevice failed %ls"),appFromAnsi(alureGetErrorString()));
	else AudioLog(NAME_Init, TEXT("ALAudio: ALURE successfully initialized using %ls."),appFromAnsi(alcGetString(Device, ALC_DEVICE_SPECIFIER)));
	*/
#else
	alutInitWithoutContext(NULL, NULL);
	if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
	{
		GWarn->Logf(TEXT("ALAudio: alutInitWithoutContext failed %ls"), (char *)alutGetErrorString(error));
	}
#endif

	AudioLog(NAME_DevAudio, TEXT("ALAudio: Using OutputRate of %i Hz for audio output"), OutputRate);
#if !RUNE_CLASSIC
	AudioLog(NAME_DevAudio, TEXT("ALAudio: Using SampleRate of %i Hz for libxmp"), SampleRate);
#endif

	/*------------------------------------------------------------------------------------
	End of OpenAL init
	------------------------------------------------------------------------------------*/

	alGetError(); //reset error state.

	// Match OpenAL distance to games distance. See ALAudioBuild.h for derivation of DEFAULT_UNREALUNITS_PER_METER.
    alSpeedOfSound(343.3f / (1.f / DEFAULT_UNREALUNITS_PER_METER));
    if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: Couldn't set speed of sound with error: %ls"), appFromAnsi(alGetString(error)));

	alListenerf(AL_METERS_PER_UNIT, 1.f / DEFAULT_UNREALUNITS_PER_METER);
	if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: Couldn't set distance units with error: %ls"), appFromAnsi(alGetString(error)));

	// set orientation
	ALfloat zup[] = { 0.0, -1.0, 0.0, 0.0, 0.0, -1.0 };
	alListenerfv(AL_ORIENTATION, zup);
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	// initialize transition
#if !RUNE_CLASSIC
	CurrentSong = CrossFadeSong = nullptr;
	PendingSong = nullptr;
#endif
	TransitionState = EMTransState::TTRANS_None;

	// OGG Helpers
	callbacks.read_func = MEM_readOgg;
	callbacks.seek_func = MEM_seekOgg;
	callbacks.close_func = MEM_closeOgg;
	callbacks.tell_func = MEM_tellOgg;

	// Initialized!
	USound::Audio = this;
#if !RUNE_CLASSIC
	UMusic::Audio = this;
#endif
	Initialized = 1;

#ifdef EFX
	// Make sure Effects Extension is loaded if someone enabled Reverb.
	if (!ConditionalLoadEffectsExtension())
		EffectsExtensionLoaded = false;
#endif

	// Load filter
	if (!LoadFilterExtension())
	{
		FilterExtensionLoaded = false;
		bSoundAttenuate = 0;
	}

	AudioLog(NAME_Init, TEXT("ALAudio: Version %ls found"), appFromAnsi(alGetString(AL_VERSION)));
	AudioLog(NAME_Init, TEXT("ALAudio subsystem initialized."));

	// Set master volume carefully to avoid overmodulation.
	alListenerf(AL_GAIN, 1.0f);
    if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: Couldn't set AL_GAIN parameters with error: %ls"), appFromAnsi(alGetString(error)));

		StopAllSound();

	//alListenerf(AL_GAIN,static_cast<ALfloat>(GIsEditor ? 255 : SoundVolume) / AUDIO_MAXVOLUME);

#if RUNE_CLASSIC
	IsRawStreaming = 0;
	RawStreamStartTime = -1.f;
#endif

#if ENGINE_VERSION==227
	UEngine::ImplementCredits(TEXT("Audio Engine: Open AL by Creative Technology"), TEXT("www.openal.org"), TEXT("UnrealI.OpenALLogo"));
#endif

	return true;
	unguard;
}

/*------------------------------------------------------------------------------------
ConditionalLoadEffectsExtension()

Loads OpenAL Effects Extension if
- Initialized set to true. (returns false otherwise)
- Reverb is enabled. (returns true otherwise)
- Not already loaded. (returns true otherwise)
- Device is not NULL. (crashes otherwise)

Returns true if loading was successful. false otherwise.
------------------------------------------------------------------------------------*/

UBOOL UALAudioSubsystem::LoadFilterExtension()
{
	// Load Filter Object Management Functions.
	alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
	alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
	alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
	if (alGenFilters == NULL || alDeleteFilters == NULL || alIsFilter == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Filter Object Management Functions."));
		return 0;
	}

	// Load Filter Object Property Functions.
	alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
	alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
	alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
	alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
	if (alFilteri == NULL || alFilteriv == NULL || alFilterf == NULL || alFilterfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Filter Object Property Functions."));
		return 0;
	}

	// Load Filter Object Query Property Functions.
	alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
	alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
	alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
	alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
	if (alGetFilteri == NULL || alGetFilteriv == NULL || alGetFilterf == NULL || alGetFilterfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Filter Object Query Property Functions."));
		return 0;
	}
	FilterExtensionLoaded = true;
	return 1;
}

#ifdef EFX
UBOOL UALAudioSubsystem::ConditionalLoadEffectsExtension()
{
	// Don't premature run this code.
	if (!Initialized || !UseReverb)
		return 0;

	// Check for need to load the extension.
	if (EffectsExtensionLoaded)
		return 1;


	// TODO: Cleanup init code.
	check(Device);

	// initialize iEffectSlot.
	iEffectSlot = 0;

	// Check if effects extension is available.
	if (alcIsExtensionPresent(Device, (ALCchar*)ALC_EXT_EFX_NAME) != ALC_TRUE)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Reverb effects not available due to lack of OpenAL Effects Extension. Try updating Soundcard drivers and OpenAL."));
		return 0;
	}

	// Query version number of effects extension.
	ALint alc_efx_major_version, alc_efx_minor_version;

	alcGetError(Device);
	alcGetIntegerv(Device, ALC_EFX_MAJOR_VERSION, 1, &alc_efx_major_version);
	alcGetIntegerv(Device, ALC_EFX_MINOR_VERSION, 1, &alc_efx_minor_version);

	if (alcGetError(Device) != ALC_NO_ERROR)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to query OpenAL Effects Extension version. Try updating Soundcard drivers and OpenAL."));
		return 0;
	}

	debugf(NAME_Init, TEXT("OpenAL Effects extension version %i.%i found."), alc_efx_major_version, alc_efx_minor_version);

	// This should *really* never happen.
	if (alc_efx_major_version < 1)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("OpenAL Effects extension major version is less than 1. Explosion imminent."));
		return 0;
	}

	// Load Auxiliary Effect Slot Object Management Functions.
	alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
	alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
	alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
	if (alGenAuxiliaryEffectSlots == NULL || alDeleteAuxiliaryEffectSlots == NULL || alIsAuxiliaryEffectSlot == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Slot Object Management Functions."));
		return 0;
	}

	// Load Auxiliary Effect Slot Object Property Functions.
	alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
	alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
	alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
	alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
	if (alAuxiliaryEffectSloti == NULL || alAuxiliaryEffectSlotiv == NULL || alAuxiliaryEffectSlotf == NULL || alAuxiliaryEffectSlotfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Slot Object Property Functions."));
		return 0;
	}

	// Load Auxiliary Effect Slot Object Query Property Functions.
	alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
	alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
	alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
	alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
	if (alGetAuxiliaryEffectSloti == NULL || alGetAuxiliaryEffectSlotiv == NULL || alGetAuxiliaryEffectSlotf == NULL || alGetAuxiliaryEffectSlotfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Slot Object Query Property Functions."));
		return 0;
	}

	// Load Effect Object Management Functions.
	alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
	alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
	if (alGenEffects == NULL || alDeleteEffects == NULL || alIsEffect == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Object Management Functions."));
		return 0;
	}

	// Load Effect Object Property Functions.
	alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
	alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
	alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
	alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
	if (alEffecti == NULL || alEffectiv == NULL || alEffectf == NULL || alEffectfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Object Property Functions."));
		return 0;
	}

	// Load Effect Object Query Property Functions.
	alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
	alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
	alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
	alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
	if (alGetEffecti == NULL || alGetEffectiv == NULL || alGetEffectf == NULL || alGetEffectfv == NULL)
	{
		GWarn->Logf(NAME_DevAudio, TEXT("Failed to load Auxiliary Effect Object Query Property Functions."));
		return 0;
	}
	// Create auxiliary effect slot.
	alGetError();
	alGenAuxiliaryEffectSlots(1, &iEffectSlot);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		GWarn->Logf(TEXT("ALAudio: alGenAuxiliary create error: %ls"), appFromAnsi(alGetString(error)));
		return 0;
	}
	else
		AudioLog(NAME_Init, TEXT("ALAudio: EFX initialized."));

	debugf(NAME_DevAudio, TEXT("Successfully loaded OpenAL Effects Extension functions."));

	// Everything's fine.
	EffectsExtensionLoaded = true;
	return 1;
}
#endif

void UALAudioSubsystem::SetViewport(UViewport* InViewport)
{
	guard(UALAudioSubsystem::SetViewport);
#if RUNE_CLASSIC
	AudioLog(NAME_DevAudio, TEXT("ALAudio: SetViewport(InViewport=%ls) -- START"), *InViewport->GetFullNameSafe());
#endif
	//debugf(TEXT("SetViewport!"));

	OldAssignedZone = NULL;
	// Stop playing sounds.
	guard(StopSounds);
	for (INT i = 0; i<EffectsChannels; i++)
		StopSound(i);
	unguard;

	guard(StopMusic);
#if !RUNE_CLASSIC
	if (CurrentSong)
	{
		delete CurrentSong;
		CurrentSong = nullptr;
	}
	if (CrossFadeSong)
	{
		delete CrossFadeSong;
		CrossFadeSong = nullptr;
	}
	PendingSong = nullptr;
#endif
	TransitionState = EMTransState::TTRANS_None;
	unguard;

	if (Viewport)
	{
#if !RUNE_CLASSIC
		guard(SetViewportUnregisterMusic);
		for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
			if (MusicIt->Handle)
				UnregisterMusic(*MusicIt);
		unguard;
#endif
		guard(SetViewportRegisterSounds);
		for (TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt)
			if (SoundIt->Handle)
				UnregisterSound(*SoundIt);
		unguard;
	}

	Viewport = InViewport;
	if (Viewport)
	{
		// Determine startup parameters.
		guard(ModifyViewportActor);
		// ALAudio once again wrote in random memory. --han
		//check(Viewport->Actor);
		if (Viewport->Actor && Viewport->Actor->Song && Viewport->Actor->Transition == MTRAN_None)
			Viewport->Actor->Transition = MTRAN_Instant;
		unguard;

		guard(RegisterSounds);
		for (TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt)
			if (!SoundIt->Handle)
				RegisterSound(*SoundIt);
		unguard;
	}
#if RUNE_CLASSIC
	AudioLog(NAME_DevAudio, TEXT("ALAudio: SetViewport() -- END"));
#endif
	unguard;
}

UViewport* UALAudioSubsystem::GetViewport()
{
	return Viewport;
}

ALAudioMusicHandle::ALAudioMusicHandle(UMusic* InMusic)
	: MusicRef(InMusic)
	, xmpcontext(nullptr)
	, IsOgg(0)
	, SongSection(0)
	, SampleRate(0)
	, format(AL_FORMAT_STEREO16)
	, OggStream(nullptr)
{
	MemOggFile.filePtr = nullptr;
}
ALAudioMusicHandle::~ALAudioMusicHandle() noexcept(false)
{
	if (OggStream)
		delete OggStream;
	if (MemOggFile.filePtr)
		delete MemOggFile.filePtr;
	if (xmpcontext)
		xmp_free_context(xmpcontext);
}

ALAudioMusicChannel::ALAudioMusicChannel(ALAudioMusicHandle* H, BYTE StartSection, FLOAT StartVolume, UALAudioSubsystem* AudioDevice)
	: FThreadingBase(TEXT("MusicThread")), Handle(H), bChannelInvalid(TRUE), xmpBuffer(nullptr), Section(StartSection), Volume(StartVolume), BufferError(0), OggBufferData(nullptr)
{
	guard(ALAudioMusicChannel::ALAudioMusicChannel);
	ALint error;
	alGenSources(1, &musicsource);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		warnf(TEXT("ALAudio: alGenSources failed %ls"), appFromAnsi(alGetString(error)));
		return;
	}

	alGenBuffers(MUSIC_BUFFERS, musicbuffers);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		warnf(TEXT("ALAudio: alGenBuffers for music failed with: %ls"), appFromAnsi(alGetString(error)));
		return;
	}

	if (!H->IsOgg)
	{
		xmpBuffer = appMalloc(MUSIC_BUFFER_SIZE, TEXT("libxmp buffer"));
		if (!xmpBuffer)
		{
			warnf(TEXT("ALAudio: can't allocate Music buffer!"));
			return;
		}
	}

	if (H->IsOgg)
	{
		AudioLog(TEXT("ALAudio: playing OGG %ls"), *H->MusicTitle);

		/* Fill OpenAL buffers */
		OggBufferData = reinterpret_cast<char*>(appMalloc(sizeof(char) * MUSIC_BUFFER_SIZE, TEXT("OggBuffer")));
		INT i;
		for (i = 0; i < MUSIC_BUFFERS; i++)
		{
			if (!FillSingleBuffer(H, OggBufferData))
			{
				warnf(TEXT("ALAudio: OGG Buffer Num:%i FillSingleBuffer failed"), i);
				return;
			}

			alBufferData(musicbuffers[i], H->format, (ALvoid*)OggBufferData, MUSIC_BUFFER_SIZE, (ALsizei)H->vorbisInfo->rate);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				//GWarn->Logf(TEXT("ALAudio: OGG Buffer Num:%i alBufferData failed %ls"),i,appFromAnsi(alGetString(error)));
				continue;
			}
		}

		alSourceQueueBuffers(musicsource, MUSIC_BUFFERS, musicbuffers);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			warnf(TEXT("ALAudio: OGG alSourceQueueBuffers failed %ls"), appFromAnsi(alGetString(error)));
			return;
		}

		alSourcePlay(musicsource);
		if (alGetError() != AL_NO_ERROR)
		{
			warnf(TEXT("ALAudio: OGG alSourcePlay failed %ls"), appFromAnsi(alGetString(error)));
			return;
		}
	}
	else if (xmp_start_player(H->xmpcontext, AudioDevice->SampleRate, 0) == 0)
	{
		H->SampleRate = AudioDevice->SampleRate;
		if (StartSection != 0)
			xmp_set_position(H->xmpcontext, StartSection);

		AudioLog(NAME_DevMusic, TEXT("Initial Music Volume: %i"), xmp_get_player(H->xmpcontext, XMP_PLAYER_VOLUME));

		xmp_set_player(H->xmpcontext, XMP_PLAYER_AMP, AudioDevice->MusicAmplify);
		xmp_set_player(H->xmpcontext, XMP_PLAYER_MIX, AudioDevice->MusicStereoMix);
		xmp_set_player(H->xmpcontext, XMP_PLAYER_INTERP, AudioDevice->MusicInterpolation);

		AudioLog(NAME_DevMusic, TEXT("MusicDsp:           %i"), AudioDevice->MusicDsp);
		AudioLog(NAME_DevMusic, TEXT("MusicAmplify:       %i"), AudioDevice->MusicAmplify);
		AudioLog(NAME_DevMusic, TEXT("MusicInterpolation: %i"), AudioDevice->MusicInterpolation);

		if (AudioDevice->MusicDsp == 0)
			xmp_set_player(H->xmpcontext, XMP_PLAYER_DSP, XMP_DSP_LOWPASS);
		else
			xmp_set_player(H->xmpcontext, XMP_PLAYER_DSP, XMP_DSP_ALL);

		//xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_VOLUME, 200);//arbitrary value, must be between 0 and 200

		// Fill OpenAL buffers
		for (INT i = 0; i < MUSIC_BUFFERS; i++)
		{
#if PlayBuffer
			if (xmp_play_buffer(H->xmpcontext, xmpBuffer, MUSIC_BUFFER_SIZE, 1) != 0)
#else
			if (xmp_play_frame(H->xmpcontext) != 0)
#endif
			{
				warnf(TEXT("ALAudio: xmp_play_buffer error"));
				return;
			}

#if PlayBuffer
			alBufferData(musicbuffers[i], H->format, xmpBuffer, MUSIC_BUFFER_SIZE, H->SampleRate);
#else
			xmp_get_frame_info(MusicHandle->xmpcontext, &xmpfi);
			alBufferData(MusicHandle->musicbuffers[i], MusicHandle->format, xmpfi.buffer, xmpfi.buffer_size, MusicHandle->SampleRate);
			//debugf(TEXT("Buffering %i %3d/%3d %3d/%3d with size %i"),i, xmpfi.pos, xmpmi.mod->len, xmpfi.row, xmpfi.num_rows,xmpfi.buffer_size);
#endif

			if ((error = alGetError()) != AL_NO_ERROR)
			{
				warnf(TEXT("ALAudio: libxmp Buffer Num:%i alBufferData failed %ls"), i, appFromAnsi(alGetString(error)));
				continue;
			}
		}
		alSourceQueueBuffers(musicsource, MUSIC_BUFFERS, musicbuffers);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			warnf(TEXT("ALAudio: libxmp alSourceQueueBuffers failed %ls"), appFromAnsi(alGetString(error)));
			return;
		}
		alSourcePlay(musicsource);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			warnf(TEXT("ALAudio: libxmp alSourcePlay failed %ls"), appFromAnsi(alGetString(error)));
			return;
		}
	}
	else
	{
		warnf(TEXT("ALAudio: xmp_start_player failed to play %ls"), *H->MusicTitle);
		return;
	}

	// Music volume
	alSource3f(musicsource, AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(musicsource, AL_VELOCITY, 0.0, 0.0, 0.0);
	alSource3f(musicsource, AL_DIRECTION, 0.0, 0.0, 0.0);
	alSourcef(musicsource, AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(musicsource, AL_SOURCE_RELATIVE, AL_FALSE);
	alSourcef(musicsource, AL_GAIN, StartVolume);
	alSourcef(musicsource, AL_MAX_GAIN, 1.f);
	alSourcefv(musicsource, AL_STEREO_ANGLES, AudioDevice->Angles); // The AL_STEREO_ANGLES tag can be used with alSourcefv() and two angles. The angles are specified anticlockwise relative to the real front, so a normal 60degree front stage is specified with alSourcefv(sid,AL_STEREO_ANGLES,+M_PI/6,-M_PI/6).

	bChannelInvalid = FALSE;
	RunThread();
	unguard;
}
ALAudioMusicChannel::~ALAudioMusicChannel() noexcept(false)
{
	guard(ALAudioMusicChannel::~ALAudioMusicChannel);
	if (!bChannelInvalid)
	{
		ALint error;
		ShutdownThread();
		alSourceStop(musicsource);

		if (musicsource)
			alDeleteSources(1, &musicsource);

		alDeleteBuffers(MUSIC_BUFFERS, musicbuffers);
		if ((error = alGetError()) != AL_NO_ERROR)
			GWarn->Logf(NAME_DevMusic, TEXT("ALAudio: UnregisterMusic alDeleteBuffers error: %ls"), appFromAnsi(alGetString(error)));
	}
	if (xmpBuffer)
		appFree(xmpBuffer);
	if (OggBufferData)
		appFree(OggBufferData);
	unguard;
}
void ALAudioMusicChannel::SetVolume(FLOAT NewVolume)
{
	guard(ALAudioMusicChannel::SetVolume);
	Volume = NewVolume;
	alSourcef(musicsource, AL_GAIN, NewVolume);
	unguard;
}
void ALAudioMusicChannel::Main()
{
	guard(ALAudioMusicChannel::Main);
#if !PlayBuffer
	struct		xmp_frame_info uxmpfi;
#endif

	ALint processed = 0;

	//AudioLog(NAME_DevMusic,(TEXT("ALAudio: Start UpdateBuffer for %ls"),*MusicHandle->MusicTitle));
	while (!bRequestExit && !BufferError)
	{
		alGetSourcei(musicsource, AL_BUFFERS_PROCESSED, &processed);

		/*
		ALint qeued;
		alGetSourcei(MusicHandle->musicsource, AL_BUFFERS_QUEUED, &qeued);
		debugf(TEXT("AL_BUFFERS_PROCESSED %i"),processed);
		*/

		if (Handle->IsOgg)
		{
			while (processed--)
			{
				if (!FillSingleBuffer(Handle, OggBufferData))
					bRequestExit = TRUE;

				ALuint buffer = 0;
				alSourceUnqueueBuffers(musicsource, 1, &buffer);
				alBufferData(buffer, Handle->format, OggBufferData, MUSIC_BUFFER_SIZE, (ALsizei)Handle->vorbisInfo->rate);
				//debugf(TEXT("AL buffer - processed: %d - error: %d - musicsource: %d - buffer: %d"), processed, MusicHandle->BufferError, MusicHandle->musicsource, buffer);
				alSourceQueueBuffers(musicsource, 1, &buffer);
				if (!sourceIsPlaying(musicsource))
				{
					alSourcePlay(musicsource);
					if (!sourceIsPlaying(musicsource))
						BufferError = AL_INVALID_VALUE;
				}
				if (bRequestExit || BufferError)
					break;
			}
		}
		else
		{
			while (processed--)
			{
				//debugf(TEXT("AL_BUFFERS_PROCESSED %i AL_BUFFERS_QUEUED %i"),processed, qeued);
#if PlayBuffer
				if (xmp_play_buffer(Handle->xmpcontext, xmpBuffer, MUSIC_BUFFER_SIZE, -1) != 0)
					bRequestExit = TRUE;
#else
				xmp_play_frame(MusicHandle->xmpcontext);
				xmp_get_frame_info(MusicHandle->xmpcontext, &uxmpfi);
#endif
				ALuint buffer = 0;
				alSourceUnqueueBuffers(musicsource, 1, &buffer);
#if PlayBuffer
				alBufferData(buffer, Handle->format, xmpBuffer, MUSIC_BUFFER_SIZE, Handle->SampleRate);
				//debugf(TEXT("AL buffer - processed: %d - error: %d - musicsource: %d - buffer: %d"), processed, MusicHandle->BufferError, MusicHandle->musicsource, buffer);
#else
				alBufferData(buffer, MusicHandle->format, uxmpfi.buffer, uxmpfi.buffer_size, MusicHandle->SampleRate);
#endif
				alSourceQueueBuffers(musicsource, 1, &buffer);
				if (!sourceIsPlaying(musicsource))
				{
					alSourcePlay(musicsource);
					if (!sourceIsPlaying(musicsource))
						BufferError = AL_INVALID_VALUE;
				}

				if (bRequestExit || BufferError)
					break;
			}
		}
#ifdef _WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
	}

	if (Handle)
	{
		//AudioLog(NAME_DevMusic,(TEXT("ALAudio: End UpdateBuffer for %ls"),*MusicHandle->MusicTitle));
		alSourceStop(musicsource);
	}
	unguard;
}

#if !RUNE_CLASSIC
void UALAudioSubsystem::RegisterMusic(UMusic* Music)
{
	guard(UALAudioSubsystem::RegisterMusic);
	AudioLog(NAME_DevMusic, TEXT("RegisterMusic %ls"), Music->GetFullName());

	if (!Music->Handle)
	{
		AudioLog(NAME_DevMusic,TEXT("RegisterMusic found no MusicHandle for %ls"),Music->GetName());
		alGetError(); // clear possible previous errors (just to be certain)
		ALAudioMusicHandle* MusicHandle = new ALAudioMusicHandle(Music);
		Music->Data.Load();
		if (!Music->Data.Num() || Music->Data.Num() <= 512)
		{
			warnf(TEXT("ALAudio: Bad music length (%i bytes) on %ls"), Music->Data.Num(), Music->GetFullName());
			Music->Data.Unload();
			delete MusicHandle;
			return;
		}

		MusicHandle->IsOgg = IsOggFormat(&Music->Data(0), Music->Data.Num());
		if (MusicHandle->IsOgg)
		{
			MusicHandle->OggStream = new OggVorbis_File;
			MusicHandle->MemOggFile.filePtr = new BYTE[Music->Data.Num()];
			appMemcpy(MusicHandle->MemOggFile.filePtr, &Music->Data(0), Music->Data.Num());
			MusicHandle->MemOggFile.curPtr = MusicHandle->MemOggFile.filePtr;
			MusicHandle->MemOggFile.fileSize = Music->Data.Num();
			INT Result = ov_open_callbacks((void *)&MusicHandle->MemOggFile, MusicHandle->OggStream, NULL, -1, callbacks);
			if (Result < 0)
			{
				GWarn->Logf(TEXT("ALAudio: OGG load module from memory error in %ls"), *Music->GetFullNameSafe());
				Music->Data.Unload();
				delete MusicHandle;
				return;
			}
		}
		else
		{
			MusicHandle->xmpcontext = xmp_create_context();
			xmp_set_player(MusicHandle->xmpcontext, XMP_PLAYER_DEFPAN, MusicPanSeparation); //must be set before loading the module!
			if (xmp_load_module_from_memory(MusicHandle->xmpcontext, &(Music->Data(0)), Music->Data.Num()) < 0)
			{
				GWarn->Logf(TEXT("ALAudio: xmp_load_module_from_memory error in %ls"), *Music->GetFullNameSafe());
				GWarn->Logf(TEXT("ALAudio: check track format!"));
				Music->Data.Unload();
				if (MusicHandle)
					delete MusicHandle;
				return;
			}
		}
		Music->Data.Unload();

		// Data into Handle
		if (MusicHandle->IsOgg)
		{
			MusicHandle->vorbisInfo = ov_info(MusicHandle->OggStream, -1);
			MusicHandle->vorbisComment = ov_comment(MusicHandle->OggStream, -1);

			if (MusicHandle->vorbisInfo->channels == 1)
				MusicHandle->format = AL_FORMAT_MONO16;
			else MusicHandle->format = AL_FORMAT_STEREO16;

			MusicHandle->MusicTitle = Music->GetName();
			for (int i = 0; i < MusicHandle->vorbisComment->comments && MusicHandle->MusicTitle.Len() < 128; i++)
				MusicHandle->MusicTitle ^= appFromAnsi(MusicHandle->vorbisComment->user_comments[i]);
			MusicHandle->MusicType = FString::Printf(TEXT("OGG Version:%i Channels:%i Rate:%i"), MusicHandle->vorbisInfo->version, MusicHandle->vorbisInfo->channels, MusicHandle->vorbisInfo->rate);
		}
		else
		{
			xmp_get_module_info(MusicHandle->xmpcontext, &xmpmi);
			MusicHandle->MusicType = appFromAnsi(xmpmi.mod->type);
			MusicHandle->MusicTitle = FString::Printf(TEXT("%ls %ls"), Music->GetName(), appFromAnsi(xmpmi.mod->name));
		}
		if (MusicHandle->MusicTitle.Len() > 128)
			MusicHandle->MusicTitle = MusicHandle->MusicTitle.Left(128);
		Music->Handle = MusicHandle;
		AudioLog(NAME_DevMusic,TEXT("RegisterMusic successfully created MusicHandle for %ls - %ls (%ls)"),Music->GetName(), *MusicHandle->MusicTitle, *MusicHandle->MusicType);
	}
	else
		AudioLog(NAME_DevMusic, TEXT("RegisterMusic already existing handle for %ls"), Music->GetName());
	unguard;
}
#endif

#if !RUNE_CLASSIC
void UALAudioSubsystem::UnregisterMusic(UMusic* Music)
{
	guard(UALAudioSubsystem::UnregisterMusic);

	if (Music && Music->Handle)
	{
		ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)Music->Handle;

		//debugf(TEXT("ALAudio: UnregisterMusic %ls"),*Music->GetName());
		AudioLog(NAME_DevMusic, TEXT("ALAudio: Unregister music: %ls"), *Music->GetFullNameSafe());

		if (CurrentSong && CurrentSong->Handle == MusicHandle)
		{
			delete CurrentSong;
			CurrentSong = nullptr;
		}
		if (CrossFadeSong && CrossFadeSong->Handle == MusicHandle)
		{
			delete CrossFadeSong;
			CrossFadeSong = nullptr;
		}
		Music->Handle = NULL;
		delete MusicHandle;
	}
	else if (Music)
		AudioLog(NAME_DevMusic, TEXT("ALAudio: No MusicHandle to unregister for music: %ls"), *Music->GetFullNameSafe());
	unguard;
}
#endif

void UALAudioSubsystem::RegisterSound(USound* Sound)
{
	guard(UALAudioSubsystem::RegisterSound);
	checkSlow(Sound);

	if (Sound && !Sound->Handle)
	{
		ALenum  format = AL_FORMAT_STEREO16; //preset.

		// Set the handle to avoid reentrance.
		Sound->Handle = (void*)-1;

		// Load the data.
#if ENGINE_VERSION==227
		TArray<BYTE>* Data = Sound->Data.GetAudio();
#else
		Sound->Data.Load();
		TArray<BYTE>* Data = &Sound->Data;
#endif

		// Han: Might be a good idea to check if anything was actually loaded instead of assuming that it was.
		if (Data->Num() <= 0)
		{
			debugf(NAME_DevAudio, TEXT("Can't register sound %ls, because it has no Data."), *Sound->GetFullNameSafe());

			// Just in case (well, is this even safe to do in this case?).
			Sound->Data.Unload();

			Sound->Handle = NULL;
			return;
		}

		ALuint buffer = 0;
		ALuint filter = 0;
		UBOOL IsOgg = 0;
		alGetError(); //clear error buffer.
		alGenBuffers(1, &buffer);

		if (FilterExtensionLoaded)
			alGenFilters(1, &filter);

		if ((error = alGetError()) != AL_NO_ERROR)
		{
			GWarn->Logf(TEXT("ALAudio: alGenBuffers for sound failed with: %ls"), appFromAnsi(alGetString(error)));
			Sound->Data.Unload();
			Sound->Handle = NULL;
			if (alIsBuffer(buffer))
				alDeleteBuffers(1, &buffer);
			if (alIsFilter)
				alDeleteFilters(1, &filter);
			return;
		}


#if 0 // Marco: already decompressed in engine.
		IsOgg = IsOggFormat(&Sound->Data(0), Sound->Data.Num());
		if (IsOgg)
		{
			//appMsgf(TEXT("IsOgg"));
			OggVorbis_File* OggStream;
			OggStream = new OggVorbis_File;
			memory_ogg_file MemOggFile;

			MemOggFile.filePtr = new BYTE[Sound->Data.Num()];
			appMemcpy(MemOggFile.filePtr, &Sound->Data(0), Sound->Data.Num());
			MemOggFile.curPtr = MemOggFile.filePtr;
			MemOggFile.fileSize = Sound->Data.Num();
			INT Result = ov_open_callbacks((void *)&MemOggFile, OggStream, NULL, -1, callbacks);
			if (Result < 0)
			{
				GWarn->Logf(TEXT("ALAudio: OGG sound load from memory error"));
				Sound->Data.Unload();
				Sound->Handle = NULL;
				delete MemOggFile.filePtr;
				delete OggStream;
				if (alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);
				if (alIsFilter)
					alDeleteFilters(1, &filter);
				return;
			}
			vorbis_info*    vorbisInfo;
			vorbisInfo = ov_info(OggStream, -1);

			vorbis_comment* vorbisComment;
			vorbisComment = ov_comment(OggStream, -1);

			/*
			FString MusicType,MusicTitle=TEXT("");
			for(int i = 0; i < vorbisComment->comments; i++)
			MusicTitle += FString::Printf(TEXT(" %ls"),appFromAnsi(vorbisComment->user_comments[i]));

			MusicType = FString::Printf(TEXT("OGG Version:%i Channels:%i Rate:%i"),vorbisInfo->version,vorbisInfo->channels,vorbisInfo->rate);
			appMsgf(TEXT("%ls %ls"),*MusicTitle, *MusicType);
			*/
			if (vorbisInfo->channels == 1)
				format = AL_FORMAT_MONO16;
			else
				format = AL_FORMAT_STEREO16;

			char OggBufferData[SOUND_BUFFER_SIZE];
			Sound->Data.Empty();
			INT DecodedSize = 0;
			while (FillSingleSoundBuffer(OggStream, OggBufferData))
			{
				Sound->Data.Add(SOUND_BUFFER_SIZE);
				appMemcpy(&Sound->Data(DecodedSize), OggBufferData, SOUND_BUFFER_SIZE);
				DecodedSize += SOUND_BUFFER_SIZE;
			}
			delete MemOggFile.filePtr;
			delete OggStream;
			//appMsgf(TEXT("DecodedSize %i Sound->Data.Num( %i %f"),DecodedSize, Sound->Data.Num(), freq);
			alBufferData(buffer, format, &Sound->Data(0), Sound->Data.Num(), (ALsizei)vorbisInfo->rate);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: OGG sound alBufferData failed %ls"), appFromAnsi(alGetString(error)));
				Sound->Data.Unload();
				Sound->Handle = NULL;
				if (alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);
				if (alIsFilter)
					alDeleteFilters(1, &filter);
				return;
			}
		}
		else
#endif
		{
#ifdef ALURE
			if (alureBufferDataFromMemory(&(*Data)(0), Data->Num(), buffer) == AL_FALSE)
			{
				GWarn->Logf(TEXT("ALAudio: alureBufferDataFromMemory failed: %ls"), appFromAnsi(alureGetErrorString()));

				/*
				alureStream *stream;
				stream = alureCreateStreamFromMemory(&Sound->Data(0),Sound->Data.Num(), 250000, 3, NULL);
				if(!stream)
				GWarn->Logf(TEXT("ALAudio: alureCreateStreamFromMemory failed: %ls"),appFromAnsi(alureGetErrorString()));
				*/

				Sound->Data.Unload();
				Sound->Handle = NULL;
				if (alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);
				if (alIsFilter)
					alDeleteFilters(1, &filter);
				return;
			}
#else
			ALvoid	*sounddata;
			ALfloat freq;
			sounddata = alutLoadMemoryFromFileImage(&Sound->Data(0), Sound->Data.Num(), &format, &size, &freq);
			if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: alutLoadMemoryFromFileImage failed %ls in %ls"), appFromAnsi(alutGetErrorString(error)), Sound->GetName());
				if (error == ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA)
				{
					Sound->Handle = NULL;
					if (alIsBuffer(buffer))
						alDeleteBuffers(1, &buffer);
					if (alIsFilter)
						alDeleteFilters(1, &filter);
					return;
				}
				// FIXME! Hack to get wav files which are unsupported by ALUT played anyway.
				// may cause problems with sound output, but seems to work.
				alBufferData(buffer, format, &Sound->Data(0), Sound->Data.Num(), (ALsizei)freq);
				if ((error = alGetError()) != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: alBufferData failed %ls"), appFromAnsi(alGetString(error)));
					Sound->Handle = NULL;
					if (alIsBuffer(buffer))
						alDeleteBuffers(1, &buffer);
					if (alIsFilter)
						alDeleteFilters(1, &filter);
					return;
				}
			}
			else
			{
				alBufferData(buffer, format, sounddata, size, (ALsizei)freq);
				if ((error = alGetError()) != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: alBufferData failed %ls"), appFromAnsi(alGetString(error)));
					Sound->Handle = NULL;
					if (alIsBuffer(buffer))
						alDeleteBuffers(1, &buffer);
					if (alIsFilter)
						alDeleteFilters(1, &filter);
					return;
				}
			}
#endif
		}

		if (buffer == AL_NONE)
		{
			GWarn->Logf(TEXT("ALAudio: Invalid sound format in %ls"), *Sound->GetFullNameSafe());
			Sound->Data.Unload();
			Sound->Handle = NULL;
			if (alIsBuffer(buffer))
				alDeleteBuffers(1, &buffer);
			if (alIsFilter)
				alDeleteFilters(1, &filter);
			return;
		}

		// Data into handle
		ALAudioSoundHandle* AudioHandle = new ALAudioSoundHandle(Sound);
		AudioHandle->ID = buffer;
		AudioHandle->Filter = filter;
		//debugf(TEXT("Sound %ls"),Sound->GetName());

		if (GetSampleLoop(&(*Data)(0), &(*Data)(Data->Num() - 1), &AudioHandle->LoopStart, &AudioHandle->LoopEnd))
			AudioHandle->bLoopingSample = TRUE;

		// Get sound rate and duration
		ALint bufferSize, frequency, bitsPerSample, channels;
		alGetBufferi(buffer, AL_SIZE, &bufferSize);
		alGetBufferi(buffer, AL_FREQUENCY, &frequency);
		alGetBufferi(buffer, AL_CHANNELS, &channels);
		alGetBufferi(buffer, AL_BITS, &bitsPerSample);
		AudioHandle->Rate = Max( (FLOAT)(frequency*channels*(bitsPerSample/8)), 1.f); //Prevent inf
		AudioHandle->Duration = (FLOAT)bufferSize/AudioHandle->Rate;
		AudioHandle->bIsStereo = (channels > 1);

		Sound->Handle = AudioHandle;

		// Unload the data.
		Sound->Data.Unload();
	}
	unguard;
}

void UALAudioSubsystem::UnregisterSound(USound* Sound)
{
	guard(UALAudioSubsystem::UnregisterSound);
	check(Sound);
	if (Sound->Handle)
	{
		ALAudioSoundHandle* AudioHandle = reinterpret_cast<ALAudioSoundHandle*>(Sound->Handle);

		AudioLog(NAME_DevSound, TEXT("ALAudio: Unregister sound: %ls"), *Sound->GetFullNameSafe());

		// Stop this sound.
		for (INT i = 0; i < EffectsChannels; i++)
			if (PlayingSounds[i].Sound == AudioHandle)
				StopSound(i);

		if (AudioHandle)
		{
			alDeleteBuffers(1, &AudioHandle->ID);
			if (AudioHandle->Filter)
				alDeleteFilters(1, &AudioHandle->Filter);
			delete AudioHandle;
		}
		Sound->Handle = NULL;
	}
	unguard;
}

void UALAudioSubsystem::Flush()
{
	guard(UALAudioSubsystem::Flush);
	AudioLog(TEXT("ALAudio: Flushing channels"));

#if !RUNE_CLASSIC
	for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
		if (MusicIt->Handle)
			UnregisterMusic(*MusicIt);
#endif

	for (TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt)
		if (SoundIt->Handle)
			UnregisterSound(*SoundIt);

	if (Viewport && Viewport->Actor)
	{
		// Determine startup parameters.
		if (Viewport->Actor->Song && Viewport->Actor->Transition == MTRAN_None)
			Viewport->Actor->Transition = MTRAN_Instant;
	}
	unguard;
}

// TODO: Check which debugf/AudioLog should be Ar.Logf().
UBOOL UALAudioSubsystem::Exec(const TCHAR* Cmd, FOutputDevice& Ar)
{
	guard(UALAudioSubsystem::Exec);
	const TCHAR* Str = Cmd;
	int order = 0;
	//debugf( NAME_DevMusic, TEXT("executing: %ls"), Str);

	if (ParseCommand(&Str, TEXT("AudioHelp")))
	{
		PrintAlignedHelpLine(Ar, TEXT("ALAudioSubsystem commands are:"));
		PrintAlignedHelpLine(Ar, TEXT("AStat Audio/Detail"), TEXT("Render on screen audio status."));
		PrintAlignedHelpLine(Ar, TEXT("MusicOrder <0-255>"), TEXT("Change music section(IT/S3M/MIDI/etc. only), 255 = stop music"));
		PrintAlignedHelpLine(Ar, TEXT("AudioFlush"), TEXT("Flush music and soundchannels."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicOffset"), TEXT("Return currently playing music offset (in S)."));
		PrintAlignedHelpLine(Ar, TEXT("SetMusicOffset <Time>"), TEXT("Change currently playing music play offset time (in S)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicLen <MusicName>"), TEXT("Returns length of selected song (in S)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicType"), TEXT("Returns the type of song (STREAM/TRACKER)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicInfo <MusicName>"), TEXT("Returns format info about the song (OGG/IT/S3M/MIDI/etc)."));
		PrintAlignedHelpLine(Ar, TEXT("GetMusicTitle <MusicName>"), TEXT("Returns the Title of a song."));
		PrintAlignedHelpLine(Ar, TEXT("MusicStop"), TEXT("Stops currently playing music."));
		PrintAlignedHelpLine(Ar, TEXT("MusicStart- Starts previously stopped music."));
		PrintAlignedHelpLine(Ar, TEXT("MusicRestart"), TEXT("Restarts currently playing music."));
		PrintAlignedHelpLine(Ar, TEXT("MusicRestore"), TEXT("Restores music from level."));
		PrintAlignedHelpLine(Ar, TEXT("MaxEffectsChannels"), TEXT("Returns number of maximum supported effect channels."));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("AudioFlush")))
	{
		Flush();
		return 1;
	}
	if (ParseCommand(&Str, TEXT("ASTAT")))
	{
		if (ParseCommand(&Str, TEXT("Audio")))
		{
			AudioStats ^= 1;
			if (AudioStats)
				Ar.Logf(TEXT("ALAudio: Audio stats enabled."));
			else Ar.Logf(TEXT("ALAudio: Audio stats disabled."));
			return 1;
		}
		if (ParseCommand(&Str, TEXT("Detail")))
		{
			DetailStats ^= 1;
			if (DetailStats)
			{
				AudioStats = 1;
				Ar.Logf(TEXT("ALAudio: Audio detail stats enabled."));
			}
			else
				Ar.Logf(TEXT("ALAudio: Audio detail stats disabled."));
			return 1;
		}
		if (ParseCommand(&Str, TEXT("Effects")))
		{
			EffectsStats ^= 1;
			if (EffectsStats)
			{
				AudioStats = 1;
				Ar.Logf(TEXT("ALAudio: Audio effects stats enabled."));
			}
			else
				Ar.Logf(TEXT("ALAudio: Audio effects stats disabled."));
			return 1;
		}
	}
#if !RUNE_CLASSIC
	if (Parse(Str, TEXT("MusicOrder"), order))
	{
		if (!CurrentSong)
			return 1;

		if (order == 255)
		{
			Ar.Logf(TEXT("ALAudio: Changing music order to %i (stop)."), order);
			check(Viewport);
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->SongSection = 255;
			Viewport->Actor->Transition = MTRAN_FastFade;
			return 1;
		}
		else
		{
			if (CurrentSong->Handle->IsOgg)
			{
				Ar.Logf(TEXT("ALAudio: Not possible in OGG Music"));
			}
			else
			{
				Ar.Logf(TEXT("ALAudio: Changing music order to %i."), order);
				xmp_set_position(CurrentSong->Handle->xmpcontext, order);
			}
		}
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicOffset")))
	{
		if (!CurrentSong)
			return 1;
		INT Result = 0;
		if (CurrentSong->Handle->IsOgg)
		{
			Result = ov_time_tell(CurrentSong->Handle->OggStream);
		}
		else
		{
			xmp_get_frame_info(CurrentSong->Handle->xmpcontext, &xmpfi);
			Result = xmpfi.time / 1000;
		}
		Ar.Logf(TEXT("%i"), Result);
		return 1;
	}
	if (Parse(Str, TEXT("SetMusicOffset"), order))
	{
		if (!CurrentSong)
			return 1;
		if (CurrentSong->Handle->IsOgg)
		{
			ov_time_seek(CurrentSong->Handle->OggStream, order);
		}
		else
		{
			INT Result = order * 1000;
			xmp_seek_time(CurrentSong->Handle->xmpcontext, Result);
		}
		Ar.Logf(TEXT("ALAudio: SetMusicOffset to %i."), order);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicLen")))
	{
		UMusic* Music = (appStrlen(Str) <= 1 ? (CurrentSong ? CurrentSong->Handle->MusicRef : nullptr) : FindObject<UMusic>(ANY_PACKAGE, Str));
		INT Result = -1;
		if (!Music)
		{
			Ar.Logf(TEXT("%i"), Result);
			return 1;
		}
		if (!Music->Handle)
		{
			Ar.Logf(TEXT("%i"), Result);
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}

		ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)Music->Handle;
		if (MusicHandle->IsOgg)
		{
			Result = ov_time_total(MusicHandle->OggStream, -1);
		}
		else
		{
			xmp_get_frame_info(MusicHandle->xmpcontext, &xmpfi);
			Result = xmpfi.total_time / 1000;
		}
		Ar.Logf(TEXT("%i"), Result);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicInfo")))
	{
		UMusic* Music = (appStrlen(Str) <= 1 ? (CurrentSong ? CurrentSong->Handle->MusicRef : nullptr) : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)Music->Handle;
		Ar.Logf(TEXT("%ls"), *MusicHandle->MusicType);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicType")))
	{
		UMusic* Music = (appStrlen(Str) <= 1 ? (CurrentSong ? CurrentSong->Handle->MusicRef : nullptr) : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)Music->Handle;

		if (MusicHandle->IsOgg)
		{
			//debugf(TEXT("GetMusicType STREAM"));
			Ar.Logf(TEXT("STREAM"));
		}
		else
		{
			//debugf(TEXT("GetMusicType TRACKER"));
			Ar.Logf(TEXT("TRACKER"));
		}

		return 1;
	}
	if (ParseCommand(&Str, TEXT("GetMusicTitle")))
	{
		UMusic* Music = (appStrlen(Str) <= 1 ? (CurrentSong ? CurrentSong->Handle->MusicRef : nullptr) : FindObject<UMusic>(ANY_PACKAGE, Str));
		if (!Music)
			return 1;
		if (!Music->Handle)
		{
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
		}
		ALAudioMusicHandle* MusicHandle = (ALAudioMusicHandle*)Music->Handle;
		Ar.Logf(TEXT("%ls"), *MusicHandle->MusicTitle);

		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicStop")))
	{
		if (!CurrentSong)
			return 1;

		//Fade to stop...
		check(Viewport);
		check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
		Viewport->Actor->SongSection = 255;
		Viewport->Actor->Transition = MTRAN_FastFade;
		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicStart")))
	{
		if (appStrlen(Str) <= 1)
		{
			check(Viewport);
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			if (Viewport->Actor->Song)
			{
				Viewport->Actor->SongSection = 0;
				Viewport->Actor->Transition = MTRAN_FastFade;
				return 1;
			}
			else return 1;
		}
		else
			AudioLog(TEXT("Trying to load %ls"), Str);

		UMusic* Music = FindObject<UMusic>(ANY_PACKAGE, Str);

		if (!Music)
		{
			UPackage* Pkg = Cast<UPackage>(LoadPackage(NULL, Str, LOAD_Forgiving));
			if (!Pkg)
				return 1;
			Music = FindObject<UMusic>(ANY_PACKAGE, Str);
		}
		if (Music)
		{
			//debugf(TEXT("Loaded Music: %ls"),Music->GetName());
			RegisterMusic(Music);
			if (!Music || !Music->Handle)
				return 1;
			check(Viewport);
			check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
			Viewport->Actor->Song = Music;
			Viewport->Actor->SongSection = 0;
			Viewport->Actor->Transition = MTRAN_Segue;
		}
		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicRestart")))
	{
		if (!CurrentSong)
			return 1;

		AudioLog(TEXT("Restarting %ls"), *CurrentSong->Handle->MusicTitle);
		check(Viewport);
		check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
		Viewport->Actor->SongSection = 0;
		Viewport->Actor->Transition = MTRAN_Instant;
		return 1;
	}
	if (ParseCommand(&Str, TEXT("MusicRestore")))
	{
		check(Viewport);
		check(Viewport->Actor); // Put these check up front all Viewport->Actor usage. --han
		if (!Viewport->Actor->Level || !Viewport->Actor->Level->Song)
			return 1;

		if (CurrentSong)
			UnregisterMusic(CurrentSong->Handle->MusicRef);

		AudioLog(TEXT("Trying to restore %ls"), Viewport->Actor->Level->Song->GetName());
		Viewport->Actor->Song = Viewport->Actor->Level->Song;
		Viewport->Actor->SongSection = Viewport->Actor->Level->SongSection;
		Viewport->Actor->Transition = MTRAN_Instant;
		return 1;
	}
#endif // !RUNE_CLASSIC
	if (ParseCommand(&Str, TEXT("MaxEffectsChannels")))
	{
		Ar.Logf(TEXT("%i"), MAX_EFFECTS_CHANNELS);
		return 1;
	}
	// HAN: Out of OpenGLDrv.
	if (ParseCommand(&Str, TEXT("AudioBuild")))
	{
		Ar.Logf(TEXT("ALAudio built: %ls"), appFromAnsi(__DATE__ " " __TIME__));
		return 1;
	}
	// Command out of X-Com: Enforcer, but there is no reason to not always include it.
	if (ParseCommand(&Str, TEXT("STOPALLSOUNDS")))
	{
		Ar.Logf(TEXT("Sounds stopped"));
		StopAllSound();
		return 1;
	}

	// This seems to be a 420 thing.
#if ENGINE_VERSION>420 || RUNE_CLASSIC
	// Pause and Unpause Audio.
	if (ParseCommand(&Str, TEXT("PAUSEAUDIO")))
	{
		Ar.Logf(TEXT("Audio Paused!! (not implemented)"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("UNPAUSEAUDIO")))
	{
		Ar.Logf(TEXT("Audio Unpaused!! (not implemented)"));
		return 1;
	}
#endif

	// X-Com: Enforcer.
#if defined(XCOM_ENFORCER)
	// Bink related stuff.
	if (ParseCommand(&Str, TEXT("SetBinkAudio")))
	{
		BinkSetSoundSystem(BinkOpenWaveOut, 0);
		Ar.Logf(TEXT("Bink is using Wave Out!"));
		return 1;
	}

	// CD Playback related.
	if (ParseCommand(&Str, TEXT("CDTRACK")))
	{
		INT TrackNum = appAtoi(Str);
		Ar.Logf(TEXT("CD Track %i (not implemented)"), TrackNum);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("STOPCD")))
	{
		Ar.Logf(TEXT("Stopping CD! (not implemented)"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("STOPTEST")))
	{
		Ar.Logf(TEXT("Stopping CD Test! (not implemented)"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("LOOPTRACK")))
	{
		INT TrackNum = appAtoi(Str);
		Ar.Logf(TEXT("Loop CD Track %i (not implemented)"), TrackNum);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDTEST")))
	{
		INT TrackNum = appAtoi(Str);
		Ar.Logf(TEXT("CD Test!! (not implemented)"));
		// Output will look like this:
		//Ar.Logf(TEXT("CurrentCDTrack = %i"), INDEX_NONE);
		//Ar.Logf(TEXT("Current Track = %i"), INDEX_NONE);
		//Ar.Logf(TEXT("Num Tracks = %i"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDPREV")))
	{
		Ar.Logf(TEXT("Previous CD Track %i (not implemented)"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDWANK")))
	{
		Ar.Logf(TEXT("Wanking CD Track %i (not implemented)"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDNEXT")))
	{
		Ar.Logf(TEXT("Next CD Track %i (not implemented)"), INDEX_NONE);
		return 1;
	}
	if (ParseCommand(&Str, TEXT("LoopCDTrack")))
	{
		Ar.Logf(TEXT("Loop CD Track !! (not implemented)"));
		if (0)
			Ar.Logf(TEXT("CD Track was looped!!"));
		return 1;
	}
	if (ParseCommand(&Str, TEXT("CDVOLUME")))
	{
		FLOAT Volume = appAtof(Str);
		Ar.Logf(TEXT("CD Volume %f (not implemented)"), Volume);
		return 1;
	}
#endif

	return FALSE;
	unguard;
}

UBOOL UALAudioSubsystem::PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch)
{
	INT Index = INDEX_NONE; // Put it up front so I can unguardf it. --han
	guard(UALAudioSubsystem::PlaySound);

	if ( !Sound || !Viewport || !Viewport->Actor)
		return 0;

    AActor* PlayingActor = NULL;

    if (Actor && !Actor->IsPendingKill())
        PlayingActor = Actor;

	// Special case: stop playing a sound. (see: Engine.Decoration.Timer)
	if ( Volume == 0 )
	{
		if ( IdToSlot(Id) != SLOT_None )
			StopSoundId(Id);
		return 0;
	}

#if ENGINE_VERSION==227
	ULevel* ViewLevel = GetCameraActor()->XLevel;
	if (PlayingActor && PlayingActor->XLevel != ViewLevel) // Filter source originated from another level.
		return 0;
#endif

	// Compute priority.
	FLOAT Priority = SoundPriority(Viewport, Location, Volume, Radius);
	FLOAT BestPriority = Priority;

	guard(FindChannel);
	if (PlayingActor)
	{
		for (INT i = 0; i < EffectsChannels; i++)
		{
			if ((PlayingSounds[i].Actor == PlayingActor) && (PlayingSounds[i].SoundFlags & SF_Loop) && (Sound != (USound*)-1) && PlayingSounds[i].IsPlayingSound(Sound)) // never play already looping sound from same actor twice!
				return 0;
			if ((IdToSlot(Id) != SLOT_None) && (PlayingSounds[i].Actor == PlayingActor) && (PlayingSounds[i].Id & ~1) == (Id & ~1))
			{
				if (Id & 1)
					return 0;
				else
				{
					Index = i;
					break;
				}
			}
			else if (PlayingSounds[i].Priority <= BestPriority)
			{
				Index = i;
				BestPriority = PlayingSounds[i].Priority;
			}
		}
	}
	else
	{
		for (INT i = 0; i < EffectsChannels; i++)
		{
			if (PlayingSounds[i].Priority <= BestPriority)
			{
				Index = i;
				BestPriority = PlayingSounds[i].Priority;
			}
		}
	}
	unguard;

	// If no sound, or its priority is overruled, stop it.
	if (Index == -1)
		return 0;

	// If already playing, stop it.
	StopSound(Index);

	if (Sound == (USound*)-1 || Sound->Handle == (void*)-1)
		return 0;

	ALAudioSoundHandle* AudioHandle = NULL;
	guard(Handle);
	AudioHandle = (ALAudioSoundHandle*)Sound->Handle;
	if (!AudioHandle)
	{
		RegisterSound(Sound);
		AudioHandle = (ALAudioSoundHandle*)Sound->Handle;
		if (!AudioHandle)
			return 0;
		check(AudioHandle != (void*)-1);
	}
	unguard;

	bid = AudioHandle->ID;

	guard(Sources);
	alGenSources(1, &sid);
	alSourcei(sid, AL_BUFFER, bid);
	unguard;

	DWORD SoundFlags = 0;
	guard(SoundFlags);
	INT Slot = IdToSlot(Id);
	if ( bPlayingAmbientSound )
		SoundFlags |= SF_AmbientSound;
	if ( AudioHandle->bLoopingSample )
		SoundFlags |= SF_LoopingSource;
	if ( (Slot == SLOT_Interface) || (PlayingActor && (PlayingActor == Viewport->Actor || PlayingActor == Viewport->Actor->Weapon)) )
		SoundFlags |= SF_No3D;
	if ( Slot == SLOT_Interface )
		SoundFlags |= SF_NoFilter;
	if ( UseSpeechVolume && (Slot == SLOT_Talk) )
		SoundFlags |= SF_Speech;
	if ( PlayingActor )
	{
		AZoneInfo* Zone = GetRegion( PlayingActor, Slot==SLOT_Talk||Slot==SLOT_Pain).Zone;
		if ( Zone && Zone->bWaterZone )
			SoundFlags |= SF_WaterEmission;
	}
	unguard;


#ifdef EFX
	guard(EFX);
	// Set the EFX effect slot on the Aux0 line on the source
	if (EffectsExtensionLoaded && alIsSource(sid) && EffectSet)//TODO: Move to abstraction
	{
		alSource3i(sid, AL_AUXILIARY_SEND_FILTER, iEffectSlot, 0, AL_FILTER_NULL); // EFX implementation doesnt match original reverb fully (yet?). Smirftsch
		// Check error status
		if ((error = alGetError()) != AL_NO_ERROR)
			GWarn->Logf(TEXT("ALAudio: EFX set slot to source error: %ls"), appFromAnsi(alGetString(error)));
	}
	unguard;
#endif
	//check(Index<EffectsChannels);

	guard(UpdatePlayingSound);
	ALAudioSoundInstance& Playing = PlayingSounds[Index];

	// Default state
	Playing.Sound = AudioHandle;
	Playing.Actor = PlayingActor;
	Playing.Priority = Priority;
	Playing.Id = Id;
	Playing.SourceID = sid;
	Playing.Volume = Volume;
	Playing.Location = (PlayingActor ? PlayingActor->Location : Location);
	Playing.SoundFlags = SoundFlags;
	Playing.CameraLocation = GetCameraLocation();
	Playing.Radius = Radius;
#if ENGINE_VERSION==227
	Playing.Level = ViewLevel;
#endif

	// Modify state with necessary API calls.
	Playing.Init();
	Playing.SetRadius(Radius);
	Playing.SetPitch(Pitch);
	Playing.SetDopplerFactor(1.0);
	Playing.Spatialize();
	Playing.UpdateVolume();

	if ( FilterExtensionLoaded )
		Playing.UpdateAttenuation( AttenuationFactor(Playing)); //No DeltaTime: instant transition.
	unguard;

	if ((error = alGetError()) != AL_NO_ERROR)
		GWarn->Logf(TEXT("ALAudio: PlaySound alSourcef setting error: %ls"), appFromAnsi(alGetString(error)));

	guard(Play);
	alSourcePlay(sid);
	unguard;

	return 1;
	unguardf((TEXT("(Actor=%ls,Id=%i,Sound=%ls,Location=(%f,%f,%f),Volume=%f,Radius=%f,Pitch=%f)|(Index=%i)"), *Actor->GetFullNameSafe(), Id, *Sound->GetFullNameSafe(), Location.X, Location.Y, Location.Z, Volume, Radius, Pitch, Index));
}

void UALAudioSubsystem::RenderAudioGeometry(FSceneNode* Frame)
{
	// Do nothing.
}

#ifdef EFX
//
// Retrieves EFX Effects.
//
// Basic control flow:
//   Initializes Info with default values (Out of old ALAudio.EFXZone)
//   Returns false if Zone==NULL.
//   Tries to call QueryEffects() UnrealScript event on Zone.
//   (Non    227): If calling fails returns false, return true otherwise.
//   (Unreal 227): If no preset in AZoneInfo, try calling.
//
UBOOL UALAudioSubsystem::QueryEffects(FEFXEffects& Effects, AZoneInfo* Zone, AActor* ViewActor)
{
	guard(UALAudioSubsystem::QueryEffects);

	struct eventQueryEffects_Parms
	{
		FEFXEffects Effects;
		AActor* ViewActor;
	} Parms;

	// Set ViewActor for Event.
	Parms.ViewActor = ViewActor;

	// Set default values (Taken out of old ALAudio.EFXZone).
	Parms.Effects.Version = 1;
	Parms.Effects.ReverbPreset = ARRAY_COUNT(g_ReverbPreset) - 1; // RP_None
	Parms.Effects.AirAbsorptionGainHF = 0.994f;
	Parms.Effects.DecayHFRatio = 0.83f;
	Parms.Effects.DecayLFRatio = 1.0;
	Parms.Effects.DecayTime = 1.49f;
	Parms.Effects.Density = 1.0;
	Parms.Effects.Diffusion = 1.0;
	Parms.Effects.EchoDepth = 0.0;
	Parms.Effects.EchoTime = 0.25;
	Parms.Effects.Gain = 0.32f;
	Parms.Effects.GainHF = 0.89f;
	Parms.Effects.GainLF = 0.0;
	Parms.Effects.HFReference = 5000.0;
	Parms.Effects.LFReference = 250.0;
	Parms.Effects.LateReverbDelay = 0.011f;
	Parms.Effects.LateReverbGain = 1.26f;
	Parms.Effects.RoomRolloffFactor = 0.0;
	Parms.Effects.bUserDefined = 0;
	Parms.Effects.bDecayHFLimit = 1;

#if ENGINE_VERSION==227
	if (Zone->EFXAmbients != ARRAY_COUNT(g_ReverbPreset) - 1) // If none check for userdefined method.
	{
		Effects = Parms.Effects; // Ugly struct copy.
		Effects.ReverbPreset = Zone->EFXAmbients;
		return 1;
	}
#endif

	UBOOL Called = Zone ? ProcessScript(Zone, ALAUDIO_QueryEffects, &Parms) : 0; // Call UnrealScript event if Zone is not NULL.

	// Clamp.
	if (Parms.Effects.bUserDefined)
	{
		Parms.Effects.AirAbsorptionGainHF = Clamp(Parms.Effects.AirAbsorptionGainHF, 0.892f, 1.f);
		Parms.Effects.DecayHFRatio = Clamp(Parms.Effects.DecayHFRatio, 0.1f, 20.f);
		Parms.Effects.DecayLFRatio = Clamp(Parms.Effects.DecayLFRatio, 0.1f, 20.f);
		Parms.Effects.DecayTime = Clamp(Parms.Effects.DecayTime, 0.1f, 20.f);
		Parms.Effects.Density = Clamp(Parms.Effects.Density, 0.f, 1.f);
		Parms.Effects.Diffusion = Clamp(Parms.Effects.Diffusion, 0.f, 1.f);
		Parms.Effects.EchoDepth = Clamp(Parms.Effects.EchoDepth, 0.f, 1.f);
		Parms.Effects.EchoTime = Clamp(Parms.Effects.EchoTime, 0.075f, 0.25f);
		Parms.Effects.Gain = Clamp(Parms.Effects.Gain, 0.f, 1.f);
		Parms.Effects.GainHF = Clamp(Parms.Effects.GainHF, 0.f, 1.f);
		Parms.Effects.GainLF = Clamp(Parms.Effects.GainLF, 0.f, 1.f);
		Parms.Effects.HFReference = Clamp(Parms.Effects.HFReference, 1000.f, 20000.f);
		Parms.Effects.LFReference = Clamp(Parms.Effects.LFReference, 20.f, 1000.f);
		Parms.Effects.LateReverbDelay = Clamp(Parms.Effects.LateReverbDelay, 0.f, 0.1f);
		Parms.Effects.LateReverbGain = Clamp(Parms.Effects.LateReverbGain, 0.1f, 10.f);
		Parms.Effects.RoomRolloffFactor = Clamp(Parms.Effects.RoomRolloffFactor, 0.0f, 10.f);
		Parms.Effects.bDecayHFLimit = Clamp(Parms.Effects.bDecayHFLimit, (BITFIELD)AL_FALSE, (BITFIELD)AL_TRUE);
	}

	Effects = Parms.Effects; // Ugly struct copy.

	if (Called)
		return 1;

	// We got EFX data out of the Zone, everything is fine.
	return 0;

	unguardf((TEXT("(Zone=%ls, ViewActor=%ls)"), Zone ? *Zone->GetFullNameSafe() : TEXT("None"), ViewActor ? *ViewActor->GetFullNameSafe() : TEXT("None")));
}
#endif

void UALAudioSubsystem::StopSound(INT Index)
{
	guard(UALAudioSubsystem::StopSound);

	PlayingSounds[Index].Stop();
	PlayingSounds[Index].Sound = NULL;
	PlayingSounds[Index].Actor = NULL;
	PlayingSounds[Index].Id = 0;
	PlayingSounds[Index].Priority = 0.0f;
	PlayingSounds[Index].Volume = 0.0f;
	PlayingSounds[Index].Radius = 0.0f;
	PlayingSounds[Index].SoundFlags = 0;
	Brightness = 1; // Some ambient sounds, like beams use that to scale volume based on their current brightness.
	unguard;
}

inline void GetTransitTime(BYTE T, FLOAT& OutTime, FLOAT& InTime, UBOOL& CrossFading)
{
	switch (T)
	{
	case MTRAN_Instant:
		InTime = 0.1f;
		OutTime = 0.1f;
		break;
	case MTRAN_Segue:
		CrossFading = TRUE;
		InTime = 7.f;
		OutTime = 3.f;
		break;
	case MTRAN_Fade:
		InTime = 3.f;
		OutTime = 3.f;
		break;
	case MTRAN_FastFade:
		InTime = 1.f;
		OutTime = 1.f;
		break;
	case MTRAN_SlowFade:
		InTime = 5.f;
		OutTime = 5.f;
		break;
	default:
		InTime = 0.f;
		OutTime = 0.f;
	}
}

void UALAudioSubsystem::Update( FPointRegion Region, FCoords& Coords)
{
	guard(UALAudioSubsystem::Update);
	if (!Viewport || !Viewport->Actor)
		return;

	DeltaTime = UpdateTime();

	// Update listener
	alDopplerFactor(DopplerFactor);
	AActor* ViewActor = GetCameraActor();
	ALfloat ListenerPosition[3], ListenerVelocity[3], ListenerDirection[6];

	TransformCoordinates(ListenerPosition, Coords.Origin); //using the head location instead of the player location
	TransformCoordinates(ListenerVelocity, ViewActor->Velocity);
	TransformCoordinates(ListenerDirection, Coords.ZAxis);
	TransformCoordinates(&ListenerDirection[3], -Coords.YAxis);
	alListenerfv(AL_ORIENTATION, ListenerDirection);
	alListenerfv(AL_POSITION, ListenerPosition);
	alListenerfv(AL_VELOCITY, ListenerVelocity);
	LastRenderPos = Coords.Origin;
	LastViewerPos = ViewActor->Location;

	AWeapon* Weapon = ViewActor->bIsPawn ? ((APawn*)ViewActor)->Weapon : NULL;
	UBOOL Realtime = Viewport->IsRealtime() && Viewport->Actor->Level->Pauser == TEXT("");
	ULevel* ViewLevel = ViewActor->XLevel;

	for (INT i = 0; i < EffectsChannels; i++)
	{
		ALAudioSoundInstance& Playing = PlayingSounds[i];
		FLOAT OldPitch = Playing.Pitch;

#if ENGINE_VERSION==227
		if (Playing.Actor)
			Playing.Level = Playing.Actor->XLevel;
		if (Playing.Level != ViewLevel)
		{
			// Audio source is in different level!
			StopSound(i);
			continue;
		}
#endif

#ifdef USE_LIPSYNCH
		if ( IdToSlot(Playing.Id) == SLOT_Talk )
		{
			// Handle lipsynch.
			UBOOL TalkingSlot = (Playing.Id & 15) == (SLOT_Talk * 2) || (Playing.Id & 15) == (SLOT_Talk * 2 + 1); // Ignore interruptable flag.
			if (bOpenALSOFT && TalkingSlot && Playing.Actor && Playing.Actor->bIsPawn && ((APawn*)Playing.Actor)->bIsSpeaking && Playing.LipsynchTime<appSecondsAudio())
			{
				// Do the magic!
				Lipsynch(Playing);

				// Update timer.
				Playing.LipsynchTime = appSecondsAudio() + 1.0f / LIPSYNCH_FREQUENCY;
			}

		}
#endif

		if ( Playing.SoundFlags & SF_AmbientSound )
		{
			if ( !Playing.Actor || Playing.Actor->bDeleteMe )
			{
				StopSound(1);
				continue;
			}

			if (LastRenderPos.DistSquared(Playing.Actor->Location) > Square(Playing.Actor->WorldSoundRadius())
				|| !Playing.IsPlayingSound(Playing.Actor->AmbientSound)
				|| !Realtime)
			{
				// Ambient sound went out of range.
				StopSound(i);
				continue;
			}
			else
			{
				// Update basic sound properties.
				FLOAT Brightness = 2.0 * (AmbientFactor*((FLOAT)Playing.Actor->SoundVolume) / 255.0);
				if (Playing.Actor->LightType != LT_None)
				{
					FPlane Color;
					Brightness *= Playing.Actor->LightBrightness / 255.0;
					Viewport->GetOuterUClient()->Engine->Render->GlobalLighting((Viewport->Actor->ShowFlags & SHOW_PlayerCtrl) != 0, Playing.Actor, Brightness, Color);
				}
				Playing.Volume = Brightness;
				Playing.SetRadius( Playing.Actor->WorldSoundRadius() );
				Playing.SetPitch( Playing.Actor->SoundPitch / 64.0);
			}
		}
		else if ( Playing.SoundFlags & SF_LoopingSource )
		{
			// Ambient sound went out of range.
			if (LastRenderPos.DistSquared(Playing.Location) > Square(Playing.Radius))
			{
				StopSound(i);
				continue;
			}
			// Special hack for looping Weapon Sounds like GESBioRifle to prevent infinite loops. Check if Fire/AltFire is still pressed, otherwise stop the loop.
#if RUNE || RUNE_CLASSIC
			else if (Weapon && !Viewport->Actor->bFire && !Viewport->Actor->bAltFire)
				StopSound(i);
#else
			else if (Weapon && !Viewport->Actor->bFire && !Viewport->Actor->bAltFire &&
				(Playing.IsPlayingSound(Weapon->CockingSound) ||
				Playing.IsPlayingSound(Weapon->FireSound) ||
				Playing.IsPlayingSound(Weapon->AltFireSound)))
				Playing.SoundFlags &= (~SF_LoopingSource);
#endif
		}

		if ( Playing.IsPlaying() )
		{
			Playing.ProcessLoop();
			Playing.UpdateActor();
			Playing.Priority = SoundPriority( Viewport, Playing.Location, Playing.Volume, Playing.Radius);
			Playing.Spatialize();
			Playing.UpdateVolume();
			if ( FilterExtensionLoaded )
				Playing.UpdateAttenuation( AttenuationFactor(Playing), DeltaTime);
		}
		else
			StopSound(i);
	}

	// See if any new ambient sounds need to be started.
	if (Realtime)
	{
		guardSlow(StartAmbience);
		for ( INT i=0 ; i<ViewLevel->Actors.Num() ; i++)
		{
			AActor* Actor = ViewLevel->Actors(i);
			if (Actor && Actor->AmbientSound && LastRenderPos.DistSquared(Actor->Location) <= Square(Actor->WorldSoundRadius()))
			{
				INT Id = Actor->GetIndex() * 16 + SLOT_Ambient * 2;
				INT j;
				for ( j=0 ; j<EffectsChannels ; j++)
				{
					ALAudioSoundInstance& Playing = PlayingSounds[j];

					if (!Playing.Actor)
                        continue;

					if ( (Playing.Actor == Actor) && (Playing.Id == Id) )
					{
						if (!Playing.IsPlayingSound(Actor->AmbientSound))
						{
							if ( Playing.SoundFlags & SF_AmbientSound ) // Replacing automatic ambient sound
							{
								StopSound(j);
								j = EffectsChannels;
							}
							else // Ambient sound being overriden by script PlaySound with SLOT_Ambient
							{
								//TODO: Move to abstraction
								FLOAT SoundTime;
								alGetSourcef( Playing.SourceID, AL_SAMPLE_OFFSET, &SoundTime);
								SoundTime /= Playing.Sound->Rate;
								// Ambient sound override about to end, play next sound now
								if ( SoundTime + DeltaTime > Playing.Sound->Duration )
								{
									StopSound(j);
									j = EffectsChannels;
								}
							}
						}
						break;
					}
				}

				if (j == EffectsChannels)
				{
					bPlayingAmbientSound = TRUE;
					PlaySound(Actor, Id, Actor->AmbientSound, Actor->Location, AmbientFactor*((FLOAT)Actor->SoundVolume / 255.0), Actor->WorldSoundRadius(), Actor->SoundPitch / 64.0);
					bPlayingAmbientSound = FALSE;
				}
			}
		}
		unguardSlow;
	}

	//Music Code
#if !RUNE_CLASSIC
	if (UseDigitalMusic)
	{
		ALfloat FMusicVolume = ((FLOAT)MusicVolume / 255.f);
		if (TransitionState)
		{
			TransitionTime += DeltaTime;
			if (TransitionState == EMTransState::TTRANS_Crossfade)
			{
				if (TransitionTime >= MusicFadeInTime)
				{
					if (CrossFadeSong)
					{
						delete CurrentSong;
						CurrentSong = CrossFadeSong;
						CrossFadeSong = nullptr;
						MusicVolumeModifier = PendingSongVolume;
					}
					CurrentSong->SetVolume(FMusicVolume * MusicVolumeModifier);
					TransitionState = EMTransState::TTRANS_None;
				}
				else
				{
					if (TransitionTime >= MusicFadeOutTime)
					{
						if (CrossFadeSong)
						{
							delete CurrentSong;
							CurrentSong = CrossFadeSong;
							CrossFadeSong = nullptr;
							MusicVolumeModifier = PendingSongVolume;
						}
						CurrentSong->SetVolume((TransitionTime / MusicFadeInTime) * FMusicVolume * MusicVolumeModifier);
					}
					else
					{
						CurrentSong->SetVolume((1.f - (TransitionTime / MusicFadeOutTime)) * FMusicVolume * MusicVolumeModifier);
						CrossFadeSong->SetVolume((TransitionTime / MusicFadeInTime) * FMusicVolume * PendingSongVolume);
					}
				}
			}
			else if (TransitionState == EMTransState::TTRANS_FadeOut)
			{
				if (TransitionTime >= MusicFadeOutTime)
				{
					if (PendingSong)
					{
						TransitionTime -= MusicFadeOutTime;
						TransitionState = EMTransState::TTRANS_FadeIn;

						if (CurrentSong)
							delete CurrentSong;
						CurrentSong = new ALAudioMusicChannel(reinterpret_cast<ALAudioMusicHandle*>(PendingSong->Handle), PendingSongSection, 0.f, this);
						if (CurrentSong->IsInvalid())
						{
							delete CurrentSong;
							CurrentSong = nullptr;
						}

						CurrentSongSection = PendingSongSection;
						MusicVolumeModifier = PendingSongVolume;
						PendingSong = nullptr;
					}
					else if (CurrentSong)
					{
						delete CurrentSong;
						CurrentSong = nullptr;
						TransitionState = EMTransState::TTRANS_None;
					}
				}
				else if (CurrentSong)
					CurrentSong->SetVolume((1.f - (TransitionTime / MusicFadeOutTime)) * FMusicVolume * MusicVolumeModifier);
			}
			else if (!CurrentSong)
			{
				TransitionState = EMTransState::TTRANS_None;
				if (CrossFadeSong)
				{
					delete CrossFadeSong;
					CrossFadeSong = nullptr;
				}
			}
			else if (TransitionTime >= MusicFadeInTime)
			{
				CurrentSong->SetVolume(FMusicVolume * MusicVolumeModifier);
				TransitionState = EMTransState::TTRANS_None;
				if (CrossFadeSong)
				{
					delete CrossFadeSong;
					CrossFadeSong = nullptr;
				}
			}
			else CurrentSong->SetVolume((TransitionTime / MusicFadeInTime) * FMusicVolume * MusicVolumeModifier);

			if (CurrentSong)
			{
				// Stereo Angle
				Angles[0] = MusicStereoAngle * PI / 180;
				Angles[1] = -Angles[0];
				CurrentSong->SetStereoAngles(Angles);
				CurrentSong->SetSeperation(MusicStereoMix); //update stereo mix

				if(CrossFadeSong)
					CrossFadeSong->SetStereoAngles(Angles);
			}
		}
		else if (CurrentSong)
		{
			if (CurrentSong->GetBufferError() != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: Musicbuffer error: %ls"), appFromAnsi(alGetString(CurrentSong->GetBufferError()))); // catch error here since can't print out from bufferthread directly.
				delete CurrentSong;
				CurrentSong = nullptr;
			}
			else
			{
				CurrentSong->SetVolume(FMusicVolume * MusicVolumeModifier);

				// Stereo Angle
				Angles[0] = MusicStereoAngle * PI / 180;
				Angles[1] = -Angles[0];
				CurrentSong->SetStereoAngles(Angles);
				CurrentSong->SetSeperation(MusicStereoMix); //update stereo mix
			}
		}

		if (Viewport->Actor->Transition != MTRAN_None && TransitionState != EMTransState::TTRANS_Crossfade) // Can't interrupt crossfade cleanly, so wait it out...
		{
			PendingSong = Viewport->Actor->Song;
			if (Viewport->Actor->SongSection == 255)
				PendingSong = nullptr;
			if (PendingSong)
			{
				if (!PendingSong->Handle)
				{
					RegisterMusic(PendingSong);
					if (!PendingSong->Handle)
						PendingSong = nullptr;
				}
			}
			PendingSongSection = Viewport->Actor->SongSection;
			PendingSongVolume = (FLOAT)Viewport->Actor->SongVolume / 255.f;

			FLOAT FadeOutTime, FadeInTime;
			UBOOL bCrossFade = FALSE;
			GetTransitTime(Viewport->Actor->Transition, FadeOutTime, FadeInTime, bCrossFade);

			if (TransitionState)
			{
				if (TransitionState == EMTransState::TTRANS_FadeIn)
				{
					TransitionState = EMTransState::TTRANS_FadeOut;
					TransitionTime = (1.f - (TransitionTime / MusicFadeInTime)) * FadeOutTime;
					MusicFadeOutTime = FadeOutTime;
					MusicFadeInTime = FadeInTime;
				}
				else MusicFadeInTime = FadeInTime;
			}
			else if (CurrentSong)
			{
				TransitionState = EMTransState::TTRANS_FadeOut;
				TransitionTime = 0.f;
				MusicFadeOutTime = FadeOutTime;
				MusicFadeInTime = FadeInTime;

				if (bCrossFade && PendingSong && PendingSong != CurrentSong->Handle->MusicRef)
				{
					TransitionState = EMTransState::TTRANS_Crossfade;
					if (CrossFadeSong)
						delete CrossFadeSong;
					CrossFadeSong = new ALAudioMusicChannel(reinterpret_cast<ALAudioMusicHandle*>(PendingSong->Handle), PendingSongSection, 0.f, this);
					if (CrossFadeSong->IsInvalid())
					{
						delete CrossFadeSong;
						CrossFadeSong = nullptr;
					}
				}
			}
			else if (PendingSong)
			{
				TransitionState = EMTransState::TTRANS_FadeIn;
				TransitionTime = 0.f;
				MusicFadeInTime = FadeInTime;

				MusicVolumeModifier = PendingSongVolume;
				CurrentSong = new ALAudioMusicChannel(reinterpret_cast<ALAudioMusicHandle*>(PendingSong->Handle), PendingSongSection, 0.f, this);
				if (CurrentSong->IsInvalid())
				{
					delete CurrentSong;
					CurrentSong = nullptr;
				}
			}
			Viewport->Actor->Transition = MTRAN_None;
		}
	}
#endif // !RUNE_CLASSIC.

#if RUNE_CLASSIC
	// Update certain raw things like MusicVolume.
	UpdateRaw();
#endif

	/*------------------------------------------------------------------------------------
	Reverb implementation for Zoneinfo
	------------------------------------------------------------------------------------*/
#ifdef EFX
	if (UseReverb && EffectsExtensionLoaded)
	{
		guard(UseReverb);

		alGetError();

#if ENGINE_VERSION==227
		Region = Viewport->Actor->CameraRegion;
#endif

		//Region = (Viewport->Actor->ViewTaPrget ? Viewport->Actor->ViewTarget->Region : Viewport->Actor->HeadRegion);
		if (!Region.Zone)
			Region = Viewport->Actor->ViewTarget ? (Viewport->Actor->ViewTarget->bIsPawn && !Viewport->Actor->bBehindView) ? ((APawn*)Viewport->Actor->ViewTarget)->HeadRegion : Viewport->Actor->ViewTarget->Region : (
#if ENGINE_VERSION!=005 && ENGINE_VERSION!=107 //  NOT RUNE
			Viewport->Actor->bBehindView ? Viewport->Actor->Region :
#endif
			Viewport->Actor->HeadRegion);

		if (!Region.Zone || OldAssignedZone == Region.Zone || SoundVolume == 0)
			goto EFXDone;
		FLOAT MasterGain = (FLOAT)(Region.Zone->MasterGain / 255.f);

		OldAssignedZone = Region.Zone;
		ReverbZone = Region.Zone->bReverbZone;
		RaytraceZone = Region.Zone->bRaytraceReverb;
		speedofsound = Region.Zone->SpeedOfSound;
		ReverbHFDamp = Clamp(Region.Zone->CutoffHz, 0, OutputRate);//max samp rate
		zonenumber = Region.ZoneNumber;
		Ambient = ARRAY_COUNT(g_ReverbPreset) - 1;

		FEFXEffects Effects;
		UBOOL RetrievedEffects = QueryEffects(Effects, Region.Zone, Viewport->Actor);

		if (RetrievedEffects)
			Ambient = Effects.ReverbPreset;

		ALuint iEffect = 0;

		//debugf(TEXT("OldReverbVolume %f,(FLOAT)(Region.Zone->MasterGain/255.0f) %f, OldReverbIntensity %f AmbientFactor %f , DopplerFactor %f"),OldReverbVolume,(FLOAT)(Region.Zone->MasterGain/255.0f),OldReverbIntensity,AmbientFactor,DopplerFactor);

		bSpecialZone = (RetrievedEffects && !Viewport->Actor->bShowMenu);

		if (EffectSet) // Zone has changed and Effect is in place , so remove all EFX effects to be ready for new effect.
		{
			//debugf(TEXT("Zonechange, removing old EFX effect"));
			alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
			EffectSet = 0;
		}
		if (ReverbZone || bSpecialZone) // maybe needs to be adjusted more to the original reverb model - Smirftsch
		{
			// Create EFX objects...
			//debugf(TEXT("setting ReverbZone"));
			EFXEAXREVERBPROPERTIES efxReverb;

			alGenEffects(1, &iEffect);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: alGenEffects create error: %ls"), appFromAnsi(alGetString(error)));
				goto Error;
			}

			//debugf(TEXT("alIsAuxiliaryEffectSlot %i"),alIsAuxiliaryEffectSlot(iEffectSlot));

			// Send automatic adjustments
			alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE);
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: alAuxiliaryEffectSloti create error: %ls"), appFromAnsi(alGetString(error)));
				goto Error;
			}

			// Change some arbitrary reverb parameter
			if (Effects.bUserDefined)
			{
				efxReverb.flAirAbsorptionGainHF = Effects.AirAbsorptionGainHF;
				efxReverb.flDecayHFRatio = Effects.DecayHFRatio;
				efxReverb.flDecayLFRatio = Effects.DecayLFRatio;
				efxReverb.flDecayTime = Effects.DecayTime;
				efxReverb.flDensity = Effects.Density;
				efxReverb.flDiffusion = Effects.Diffusion;
				efxReverb.flEchoDepth = Effects.EchoDepth;
				efxReverb.flEchoTime = Effects.EchoTime;
				efxReverb.flGain = Effects.Gain;
				efxReverb.flGainHF = Effects.GainHF;
				efxReverb.flHFReference = Effects.HFReference;
				efxReverb.flLateReverbDelay = Effects.LateReverbDelay;
				efxReverb.flLateReverbGain = Effects.LateReverbGain;
				efxReverb.flLFReference = Effects.LFReference;
				efxReverb.flRoomRolloffFactor = Effects.RoomRolloffFactor;
				efxReverb.iDecayHFLimit = Effects.bDecayHFLimit;
				/*
				// yet unused?
				efxReverb.flModulationDepth		=	Effects.lModulationDepth;
				efxReverb.flModulationTime		=	Effects.lModulationTime;
				efxReverb.flReflectionsDelay	=	Effects.lReflectionsDelay;
				efxReverb.flReflectionsGain		=	Effects.lReflectionsGain;
				efxReverb.flReflectionsPan[3]	=	Effects.lReflectionsPan[3];
				efxReverb.flLateReverbPan[3]	=	Effects.lLateReverbPan[3];
				*/
			}
			else if (EmulateOldReverb && (Ambient == (ARRAY_COUNT(g_ReverbPreset) - 1)))
			{
				MaxDelay = 0.f;
				MaxGain = 0.f;
				MinDelay = 0.f;
				AvgDelay = 0.f;
				T = 0.f;

				//Calculate reverb time from six comb filters
				INT i = 0;

				for (i = 0; i<ARRAY_COUNT(Region.Zone->Delay); i++)
				{
					DelayTime[i] = Clamp(Region.Zone->Delay[i] / 500.0f, 0.001f, 0.340f);
					DelayGain[i] = Clamp(Region.Zone->Gain[i] / 255.0f, 0.001f, 0.999f);
					if ((DelayTime[i] * DelayGain[i])>(MaxDelay*MaxGain))
					{
						MaxDelay = DelayTime[i];
						MaxGain = DelayGain[i];
						//Reverb->Time=(float)((-60.0f*MaxDelay)/(20.0f*log10(MaxGain)));
					}
					if (DelayTime[i]<MinDelay)
						MinDelay = DelayTime[i];
					AvgDelay += (DelayTime[i] / 6.0f);
					T += DelayTime[i];
				}
				RoomSize = 3.0f*(AvgDelay - MinDelay) / (MaxDelay - MinDelay);

				if (RoomSize<0.4f)
				{
					efxReverb = *GetEFXReverb(110);
					//debugf(TEXT("Ambient: 110"));
				}
				else if (RoomSize<0.8f)
				{
					efxReverb = *GetEFXReverb(26);
					//debugf(TEXT("Ambient: 26"));
				}
				else if (RoomSize<1.2f)
				{
					efxReverb = *GetEFXReverb(44);
					//debugf(TEXT("Ambient: 44"));
				}
				else if (RoomSize<1.6f)
				{
					efxReverb = *GetEFXReverb(27);
					//debugf(TEXT("Ambient: 27"));
				}
				else if (RoomSize<2.0f)
				{
					efxReverb = *GetEFXReverb(45);
					//debugf(TEXT("Ambient: 45"));
				}
				else if (RoomSize<2.4f)
				{
					efxReverb = *GetEFXReverb(28);
					//debugf(TEXT("Ambient: 28"));
				}
				else if (RoomSize<4.f)
				{
					efxReverb = *GetEFXReverb(29);
					//debugf(TEXT("Ambient: 29"));
				}
				else if (RoomSize<6.0f)
				{
					efxReverb = *GetEFXReverb(21);
					//debugf(TEXT("Ambient: 21"));
				}
				else if (RoomSize<8.0f)
				{
					efxReverb = *GetEFXReverb(90);
					//debugf(TEXT("Ambient: 90"));
				}
				else
				{
					efxReverb = *GetEFXReverb(18);
					//debugf(TEXT("Ambient: 18"));
				}
				//debugf(TEXT("OpenAL: Roomsize: %f MasterGain %f, efxReverb.flGain %f, OldReverbIntensity %f"),RoomSize, MasterGain, efxReverb.flGain, OldReverbIntensity);
				efxReverb.flGain = Clamp((MasterGain * efxReverb.flGain * OldReverbIntensity), 0.001f, 0.999f);
			}
			else
			{
			    efxReverb = *GetEFXReverb(Ambient);
			    //debugf(TEXT("Ambient: %i"),Ambient);
			    //debugf(TEXT("OpenAL: Ambient %i MasterGain %f, efxReverb.flGain %f, ReverbIntensity %f"),Ambient, MasterGain, efxReverb.flGain, ReverbIntensity);
				efxReverb.flGain = Clamp((MasterGain * efxReverb.flGain * ReverbIntensity), 0.001f, 0.999f);
			}
			//debugf(TEXT("Region.Zone %ls ambients %i"), *Region.Zone->GetFullNameSafe(), Ambient);
			alEffecti(iEffect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
			alEffectf(iEffect, AL_REVERB_DENSITY, efxReverb.flDensity);
			alEffectf(iEffect, AL_REVERB_GAIN, efxReverb.flGain);
			alEffectf(iEffect, AL_REVERB_GAINHF, efxReverb.flGainHF);
			alEffectf(iEffect, AL_REVERB_DECAY_TIME, efxReverb.flDecayTime);
			alEffectf(iEffect, AL_REVERB_DECAY_HFRATIO, efxReverb.flDecayHFRatio);
			alEffectf(iEffect, AL_REVERB_REFLECTIONS_GAIN, efxReverb.flReflectionsGain);
			alEffectf(iEffect, AL_REVERB_REFLECTIONS_DELAY, efxReverb.flReflectionsDelay);
			alEffectf(iEffect, AL_REVERB_LATE_REVERB_GAIN, efxReverb.flLateReverbGain);
			alEffectf(iEffect, AL_REVERB_LATE_REVERB_DELAY, efxReverb.flLateReverbDelay);
			alEffectf(iEffect, AL_REVERB_AIR_ABSORPTION_GAINHF, efxReverb.flAirAbsorptionGainHF);
			alEffectf(iEffect, AL_REVERB_ROOM_ROLLOFF_FACTOR, efxReverb.flRoomRolloffFactor);
			alEffecti(iEffect, AL_REVERB_DECAY_HFLIMIT, efxReverb.iDecayHFLimit);

            /*
			debugf(TEXT("AL_REVERB_DENSITY%f"),efxReverb.flDensity);
			debugf(TEXT("AL_REVERB_GAIN %f"),efxReverb.flGain);
			debugf(TEXT("AL_REVERB_GAINHF %f"),efxReverb.flGainHF);
			debugf(TEXT("AL_REVERB_DECAY_TIME %f"),efxReverb.flDecayTime);
			debugf(TEXT("AL_REVERB_DECAY_HFRATIO %f"),efxReverb.flDecayHFRatio);
			debugf(TEXT("AL_REVERB_REFLECTIONS_GAIN %f"),efxReverb.flReflectionsGain);
			debugf(TEXT("AL_REVERB_REFLECTIONS_DELAY %f"),efxReverb.flReflectionsDelay);
			debugf(TEXT("AL_REVERB_LATE_REVERB_GAIN %f"),efxReverb.flLateReverbGain);
			debugf(TEXT("AL_REVERB_LATE_REVERB_DELAY %f"),efxReverb.flLateReverbDelay);
			debugf(TEXT("AL_REVERB_AIR_ABSORPTION_GAINHF %f"),efxReverb.flAirAbsorptionGainHF);
			debugf(TEXT("AL_REVERB_ROOM_ROLLOFF_FACTOR %f"),efxReverb.flRoomRolloffFactor);
			debugf(TEXT("AL_REVERB_DECAY_HFLIMIT %f"),efxReverb.iDecayHFLimit);
			*/

			// Check error status
			if ((error = alGetError()) != AL_NO_ERROR)
			{
				GWarn->Logf(TEXT("ALAudio: EFX reverb parameter change error: %ls"), appFromAnsi(alGetString(error)));
				goto Error;
			}

			// Re-load effect into slot
			if (alIsAuxiliaryEffectSlot(iEffectSlot))
			{
				alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_EFFECT, iEffect);
				// Check error status
				if ((error = alGetError()) != AL_NO_ERROR)
				{
					GWarn->Logf(TEXT("ALAudio: alAuxiliaryEffectSloti effectslot error: %ls"), appFromAnsi(alGetString(error)));
					goto Error;
				}
			}
			EffectSet = 1; // Effect has been set;

			if (alIsEffect(iEffect))
				alDeleteEffects(1, &iEffect);

			if (!EffectSet)
			{
			Error:
				GWarn->Logf(TEXT("ALAudio: Error in EFX, unsetting any effects."));
				if (alIsAuxiliaryEffectSlot(iEffectSlot))
					alAuxiliaryEffectSloti(iEffectSlot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
				if (alIsEffect(iEffect))
					alDeleteEffects(1, &iEffect);
				EffectSet = 0;
			}
		}
		unguard;
	}
EFXDone:;
#endif
	unguard;
}
void UALAudioSubsystem::PostRender(FSceneNode* Frame)
{
	guard(UALAudioSubsystem::PostRender);

	Frame->Viewport->Canvas->DrawColor = FColor(255, 255, 255);
	if (AudioStats)
	{
		Frame->Viewport->Canvas->CurX = 0;
		Frame->Viewport->Canvas->CurY = 16;
		Frame->Viewport->Canvas->WrappedPrintf
			(Frame->Viewport->Canvas->SmallFont, 0, TEXT("ALAudioSubsystem Statistics:"));
		INT i = 0;
		for (i = 0; i<EffectsChannels; i++)
		{
		    FString SoundName;
		    if (PlayingSounds[i].Sound)
                SoundName = PlayingSounds[i].Sound->Sound->GetFullNameSafe(); // Better safe than sorry

            FString ActorName;
            if (PlayingSounds[i].Actor)
                ActorName=PlayingSounds[i].Actor->GetFullNameSafe(); //Could be very well without Actor...

			if (PlayingSounds[i].SourceID)
			{

				// Current Sound.
				Frame->Viewport->Canvas->CurX = 10;
				Frame->Viewport->Canvas->CurY = 24 + 8 * i;
				if (DetailStats)
					Frame->Viewport->Canvas->WrappedPrintf
					(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Channel %i: %ls - Vol: %05.2f Pitch: %05.2f Radius: %07.2f Priority: %05.2f Actor:%ls"),
					i, *SoundName, PlayingSounds[i].Volume,
					PlayingSounds[i].Pitch, PlayingSounds[i].Radius, PlayingSounds[i].Priority, *ActorName);
				else Frame->Viewport->Canvas->WrappedPrintf
					(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Channel %i: %ls"),
					i, *SoundName);
			}
			else
			{

				Frame->Viewport->Canvas->CurX = 10;
				Frame->Viewport->Canvas->CurY = 24 + 8 * i;
				if (DetailStats)
					Frame->Viewport->Canvas->WrappedPrintf
					(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Channel %i: None ..."), i);
				else Frame->Viewport->Canvas->WrappedPrintf
					(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Channel %i: None"), i);
			}
		}
#if !RUNE_CLASSIC
		if (CurrentSong)
		{
			INT Offset = 0, MusicLen = 0, Volume = MusicVolume;
			if (CurrentSong->Handle->IsOgg)
			{
				Offset = ov_time_tell(CurrentSong->Handle->OggStream);
				MusicLen = ov_time_total(CurrentSong->Handle->OggStream, -1);
			}
			else
			{
				xmp_get_frame_info(CurrentSong->Handle->xmpcontext, &xmpfi);
				Offset = xmpfi.time / 1000;
				MusicLen = xmpfi.total_time / 1000;
				Volume = xmpfi.volume;
			}

			Frame->Viewport->Canvas->CurX = 10;
			Frame->Viewport->Canvas->CurY = 24 + 8 * (i + 1);
			if (DetailStats)
			{
				Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->SmallFont, 0, TEXT("MusicChannel 0: Title: %ls - Vol: %i"), *CurrentSong->Handle->MusicTitle, Volume);
				Frame->Viewport->Canvas->CurY = 24 + 8 * (i + 2);
				Frame->Viewport->Canvas->CurX = 10;
				Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->SmallFont, 0, TEXT("Offset: %i TotalLength : %i Type : %ls"), Offset, MusicLen, *CurrentSong->Handle->MusicType);
			}
			else Frame->Viewport->Canvas->WrappedPrintf(Frame->Viewport->Canvas->SmallFont, 0, TEXT("MusicChannel 0: Title: %ls"), *CurrentSong->Handle->MusicTitle);
		}
#endif // !RUNE_CLASSIC
	}
	unguard;
}

/*-----------------------------------------------------------------------------
Destroy & Shutdown.
-----------------------------------------------------------------------------*/

void UALAudioSubsystem::NoteDestroy(AActor* Actor)
{
	guard(UALAudioSubsystem::NoteDestroy);
	check(Actor);
	check(Actor->IsValid());

	// Stop referencing actor.
	for (INT i = 0; i<EffectsChannels; i++)
	{
		if (PlayingSounds[i].Actor == Actor)
		{
			if ( PlayingSounds[i].SoundFlags & SF_Loop )
			{
				// Stop ambient sound when actor dies.
				StopSound(i);
			}
			else
			{
				// Unbind regular sounds from actors.
				PlayingSounds[i].Actor = NULL;
			}
		}
	}

	unguard;
}

void UALAudioSubsystem::Destroy()
{
	guard(UALAudioSubsystem::Destroy);

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(_MSC_VER)
	TimerEnd();
#endif

	if (Initialized)
	{
		// Unregister all music
#if !RUNE_CLASSIC
		for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
			if (MusicIt->Handle)
				UnregisterMusic(*MusicIt);
#endif

		// Unhook.
		EndBuffering = TRUE; //Make sure to have no thread left buffering!!
#if __GNUG__
		// stijn: @Smirftsch, I do not understand why you had this pthread_exit
		// call here. This is totally unsafe. It shuts down the main game
		// thread, but leaves other threads running. On my Linux machine, I
		// still had two pulseaudio threads running in the background while my
		// main game thread was gone. This turned my ut-bin into a zombie
		// process.

//		pthread_exit(NULL);
#endif
		USound::Audio = NULL;
#if !RUNE_CLASSIC
		UMusic::Audio = NULL;
#endif

		// Shut down viewport.
		SetViewport(NULL);

		AudioLog(NAME_Exit, TEXT("ALAudio: subsystem shut down."));

#ifdef EFX
		// Remove alDeleteAuxiliaryEffectSlot
		if (EffectsExtensionLoaded && alIsAuxiliaryEffectSlot(iEffectSlot))
			alDeleteAuxiliaryEffectSlots(1, &iEffectSlot);
#endif

		// Shutdown ALUT / ALURE
#ifdef ALURE
		alureShutdownDevice();
#else
		alutExit();
#endif

		// Shutdown soundsystem.
		if (context_id)
		{
			alcMakeContextCurrent(NULL);
			Device = alcGetContextsDevice(context_id);
			if (Device)
			{
				alcDestroyContext(context_id);
				alcCloseDevice(Device);
			}
			context_id = NULL;
		}
		AudioLog(NAME_Exit, TEXT("ALAudio: Audio subsystem shut down."));

		// han: Set Initialized to zero?
	}
	Super::Destroy();

	if (GALAudio == this)
		GALAudio = nullptr;

#ifdef EFX
	unguardf((TEXT("(Initialized=%i,EffectsExtensionLoaded=%i)"), Initialized, EffectsExtensionLoaded));
#else
	unguardf((TEXT("(Initialized=%i)"), Initialized));
#endif
}

void UALAudioSubsystem::ShutdownAfterError()
{
	guard(UALAudioSubsystem::ShutdownAfterError);

#if ENGINE_VERSION!=227 && ENGINE_VERSION != 469 && defined(_MSC_VER)
	TimerEnd();
#endif

	// Unregister all music
#if !RUNE_CLASSIC && !UNREAL_TOURNAMENT_UTPG
	for (TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt)
		if (MusicIt->Handle)
			UnregisterMusic(*MusicIt);
#endif

	// Unhook.
	EndBuffering = TRUE; //Make sure to have no thread left buffering!!
	USound::Audio = NULL;
#if !RUNE_CLASSIC
	UMusic::Audio = NULL;
#endif

	// Safely shut down.
	AudioLog(NAME_Exit, TEXT("ALAudio subsystem shut down."));

	try
	{

#ifdef EFX
		// Remove alDeleteAuxiliaryEffectSlot
		if (EffectsExtensionLoaded && alIsAuxiliaryEffectSlot(iEffectSlot))
			alDeleteAuxiliaryEffectSlots(1, &iEffectSlot);
#endif

		if (context_id)
		{
			ALCcontext* Context = alcGetCurrentContext();
			Device = alcGetContextsDevice(Context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(Context);
			alcCloseDevice(Device);
			context_id = NULL;
		}

		// Shutdown ALUT / ALURE
#ifdef ALURE
		alureShutdownDevice();
#else
		alutExit();
#endif
	}
	catch(...)
	{

	}

	AudioLog(NAME_Exit, TEXT("UALAudioSubsystem::ShutdownAfterError"));
	Super::ShutdownAfterError();

#ifdef EFX
	unguardf((TEXT("(Initialized=%i,EffectsExtensionLoaded=%i)"), Initialized, EffectsExtensionLoaded));
#else
	unguardf((TEXT("(Initialized=%i)"), Initialized));
#endif
}

//
// Stop Sound support.
//
void UALAudioSubsystem::StopAllSound() // Rune.
{
	guard(UALAudioSubsystem::StopAllSound);
	check(ARRAY_COUNT(PlayingSounds) >= EffectsChannels); // More sanity checks. --han

	for (INT i = 0; i<EffectsChannels; i++)
		StopSound(i);

	unguard;
}

//
// I don't exactly know how this is supposed to work, but I implemented a scheme which sounds resonable for me. --han
//
//  * Stop everything if neither Actor nor Sound is specified.
//  * If just Actor is set, stop all sounds associated with this Actor.
//  * If just Sound is set, stop it for all Actors.
//  * If both Actor and Sound is set, require both to match.
//
UBOOL UALAudioSubsystem::StopSound(AActor* Actor, USound *Sound) // Rune Classic.
{
	guard(UALAudioSubsystem::StopSound);
	check(ARRAY_COUNT(PlayingSounds) >= EffectsChannels); // More sanity checks. --han
	debugf(NAME_DevAudio, TEXT("StopSound(Actor=%ls,Sound=%ls)"), Actor->GetName(), Sound->GetName());

	INT Count = 0;
	for (INT i = 0; i<EffectsChannels; i++)
		if ((Actor == NULL || Actor == PlayingSounds[i].Actor) && (Sound == NULL || PlayingSounds[i].IsPlayingSound(Sound)))
			StopSound(i), Count++;

	return Count;
	unguardf((TEXT("(Actor=%ls,Sound=%ls)"), *Actor->GetFullNameSafe(), *Sound->GetFullNameSafe()));
}
void UALAudioSubsystem::StopSoundId(INT Id) // DeusEx, Rune and Undying, NOT Rune Classic.
{
	guard(UALAudioSubsystem::StopSoundId);
	INT Index = INDEX_NONE;

	for (INT i = 0; i<EffectsChannels; i++)
	{
#if defined(UNDYING) // Undying ignores force play flag.
		if (PlayingSounds[i].Id == Id)
#else
		if ((PlayingSounds[i].Id&~1) == (Id&~1))
#endif
		{
			Index = i;
			break;
		}
	}
	if (Index != INDEX_NONE)
		StopSound(Index);

	unguardf((TEXT("(Id=%i)"), Id));
}

//
// Additional SoundId related interfaces.
//
UBOOL UALAudioSubsystem::SoundIdActive(INT Id) // Undying.
{
	guard(UALAudioSubsystem::SoundIdActive);
	check(sizeof(PlayingSounds) <= EffectsChannels); // More sanity checks. --han

	for (INT i = 0; i<EffectsChannels; i++)
#if defined(UNDYING) // Undying ignores force play flag.
		if (PlayingSounds[i].Id == Id)
#else
		if ((PlayingSounds[i].Id&~1) == (Id&~1))
#endif
			return 1;
	return 0;

	unguardf((TEXT("(Id=%i)"), Id));
}

// Non unicode verification.
#if defined(LEGACY_YELL_IF_UNICODE) && (defined(_UNICODE) || defined(UNICODE) )
#error "Your active configuration is set to UNICODE!"
#endif

/*-----------------------------------------------------------------------------

	Subsystem utils.

-----------------------------------------------------------------------------*/

AActor* UALAudioSubsystem::GetCameraActor()
{
	guardSlow(UALAudioSubsystem::GetCameraActor);
	check(Viewport);
	check(Viewport->Actor);
#if RUNE || RUNE_CLASSIC //RUNE
	return Viewport->Actor;
#elif ENGINE_VERSION==227
	return (Viewport->Actor->CalcCameraActor ? Viewport->Actor->CalcCameraActor : Viewport->Actor);
#else
	return (Viewport->Actor->ViewTarget ? Viewport->Actor->ViewTarget : Viewport->Actor);
#endif
	unguardSlow;
}

FVector UALAudioSubsystem::GetCameraLocation()
{
	guardSlow(UALAudioSubsystem::GetCameraLocation);
#if RUNE || RUNE_CLASSIC //RUNE
	check(Viewport);
	check(Viewport->Actor);
	return Viewport->Actor->ViewLocation;
#elif ENGINE_VERSION==227
	check(Viewport);
	check(Viewport->Actor);
	return (Viewport->Actor->CalcCameraActor ? Viewport->Actor->CalcCameraLocation : Viewport->Actor->Location);
#else
	return LastRenderPos + (GetCameraActor()->Location - LastViewerPos);
#endif
	unguardSlow;
}

AZoneInfo* UALAudioSubsystem::GetCameraZone() const
{
	guardSlow(UALAudioSubsystem::GetCameraZone);
#if ENGINE_VERSION==227
	return (Viewport && Viewport->Actor) ? Viewport->Actor->CameraRegion.Zone : nullptr;
#else
	return (Viewport && Viewport->Actor) ? (Viewport->Actor->ViewTarget ? Viewport->Actor->ViewTarget->Region.Zone : Viewport->Actor->HeadRegion.Zone) : nullptr;
#endif
	unguardSlow;
}

FLOAT UALAudioSubsystem::SoundPriority(UViewport* Viewport, FVector Location, FLOAT Volume, FLOAT Radius)
{
	guard(UALAudioSubsystem::SoundPriority);
	return Volume * (1.0f - (Location - GetCameraLocation()).Size() / Radius);
	unguard;
}

FLOAT UALAudioSubsystem::AttenuationFactor( ALAudioSoundInstance& Playing)
{
	guard(UALAudioSubsystem::AttenuationFactor);

	if ( !Playing.Actor )
		return Playing.AttenuationFactor;

	if ( !bSoundAttenuate || (Playing.SoundFlags & SF_NoFilter) )
		return 1.0;

	AActor* Listener = (Viewport && Viewport->Actor) ? GetCameraActor() : NULL;

	//Attenuate by visibility
	FLOAT Factor = 1.0;
	if ( Listener && (Playing.Actor->Region.ZoneNumber != 0) )
	{
		if ( Playing.Actor->IsBrush() )
		{
			//TODO
		}
		else if ((!Listener->GetLevel()->FastLineCheck(Playing.Location, GetCameraLocation(), TRACE_AllColliding | TRACE_LightBlocking, Listener)))//!FastLineCheck( Listener->GetLevel()->Model, GetCameraLocation(), Playing.Location))//( Listener && Listener->GetLevel()->FastLineCheck(Playing.Location, GetCameraLocation(), TRACE_AllColliding, Listener))
		{
			if (Playing.Actor->VisibilityRadius > 0.f)
			{
				Factor = Clamp(Playing.Actor->VisibilityRadius, 0.f, 1.0f); //give the mapper the opportunity to change this factor.
			}
			else Factor = 0.5f;
			//debugf(TEXT("Attenuating"));
		}
	}

	// Use Water zone's ViewFog to see how much sound must be attenuated
	// Designed like this to not affect DM-Pyramid's NitrogenZone.
	// Use Head region for listener and pawns emitting talk/pain sounds.

	// Attenuate by emitter water zone
	if ( Playing.SoundFlags & SF_WaterEmission )
	{
		INT Slot = Playing.GetSlot();
		AZoneInfo* Zone = GetRegion( Playing.Actor, Slot==SLOT_Talk||Slot==SLOT_Pain ).Zone;
		if ( Zone )
			Factor /= 1.0f + Max(Zone->ViewFog.X + Zone->ViewFog.Y + Zone->ViewFog.Z, 0.f) * 1.5;
	}

	//Attenuate by listener water zone
	if ( Listener )
	{
		AZoneInfo* Zone = GetRegion( Listener, 1).Zone;
		if ( Zone && Zone->bWaterZone )
			Factor /= 1.0f + Max(Zone->ViewFog.X + Zone->ViewFog.Y + Zone->ViewFog.Z, 0.f) * 3.0;
	}

	return Factor;

	unguard;
}

FLOAT UALAudioSubsystem::UpdateTime()
{
#if ENGINE_VERSION==227
	return ALevelInfo::CurrentLevel ? ALevelInfo::CurrentLevel->LastRealDeltaTime : (1.f / 60.f);
#else
	FLOAT DeltaTime;
	FTIME NewTime;
#if defined(LEGACY_TIME) || defined(UNREAL_TOURNAMENT)
	NewTime = appSecondsAudio();
#else
	// Han: Either use DOUBLE for FTime games too or make consequent use of FTime (TransitionStartTime, etc.)
	NewTime = appSeconds();
#endif
	DeltaTime = Clamp( (FLOAT)(NewTime-LastTime), 0.f, 0.4f);
	LastTime = NewTime;
	return DeltaTime;
#endif
}


/*-----------------------------------------------------------------------------
Rune Classic Raw Streaming.

Todo:
* Figure out which alSource* stuff needs to be set!
-----------------------------------------------------------------------------*/

#if RUNE_CLASSIC

#if 1
#define rawf debugf
#else
#define rawf
#endif

#define DESIRED_RAW_POOL_SIZE 8 // Could actually be sth. like 32 until we figure out how to not get so much data at once.

void UALAudioSubsystem::InitRaw()
{
	guard(UALAudioSubsystem::ResizeRawChunkBufferPool);
	IsRawStreaming = IsRawError = 0;
	RawStreamStartTime = 0.f;
	ALuint RawSource = 0;
	unguard;
}

void UALAudioSubsystem::UpdateRaw()
{
	guard(UALAudioSubsystem::UpdateRaw);
	if (!IsRawStreaming)
		return;

	// Update MusicVolume.
	alSourcef(RawSource, AL_GAIN, MusicVolume / 255.f);
	unguard;
}

UBOOL UALAudioSubsystem::ResizeRawChunkBufferPool(INT Desired)
{
	guard(UALAudioSubsystem::ResizeRawChunkBufferPool);
	INT OldPoolCount = RawChunkBufferPool.Num();

	// Clear Error.
	alGetError();

	// Enlarge.
	if (RawChunkBufferPool.Num()<Desired)
	{
		guard(Enlarge);
		ALuint* Buffers = (ALuint*)appAlloca(Desired*sizeof(ALuint*));
		INT NumBuffers = Desired - RawChunkBufferPool.Num();
		alGenBuffers(NumBuffers, &Buffers[RawChunkBufferPool.Num()]);
		if (alGetError() != AL_NO_ERROR)
		{
			debugf(NAME_DevMusic, TEXT("Failed to create buffer for raw stream pool. "));
			return 0;
		}
		for (INT i = RawChunkBufferPool.Num(); i<Desired; i++)
			RawChunkBufferPool.AddItem(Buffers[i]);
		rawf(NAME_DevMusic, TEXT("Generated %i chunk buffers."), NumBuffers);
		unguard;
	}
	// Shrink.
	else if (RawChunkBufferPool.Num()>Desired)
	{
		guard(Shrink);
		alDeleteBuffers(RawChunkBufferPool.Num() - Desired, &RawChunkBufferPool(Desired));
		RawChunkBufferPool.Remove(Desired, RawChunkBufferPool.Num() - Desired);
		if (alGetError() != AL_NO_ERROR)
		{
			debugf(NAME_DevMusic, TEXT("Encountered error while shrinking raw stream pool. "));
			return 0;
		}
		rawf(NAME_DevMusic, TEXT("Deleted %i chunk buffers."), OldPoolCount - RawChunkBufferPool.Num());
		unguard;
	}
	return 1;
	unguardf((TEXT("(Desired=%i)"), Desired));
}

INT UALAudioSubsystem::RegainChunkBufferPool()
{
	guard(UALAudioSubsystem::RegainChunkBufferPool);
	INT OldPoolSize = RawChunkBufferPool.Num();
	ALint BuffersProcessed;
	alGetSourcei(RawSource, AL_BUFFERS_PROCESSED, &BuffersProcessed);
	if (BuffersProcessed>0)
	{
		check(BuffersProcessed <= RawChunkBuffers.Num());
		for (INT i = 0; i<BuffersProcessed; i++)
			RawChunkBufferPool.AddItem(RawChunkBuffers(i));

		// Unqueue.
		alSourceUnqueueBuffers(RawSource, BuffersProcessed, &RawChunkBuffers(0));
		RawChunkBuffers.Remove(0, BuffersProcessed);
	}
	rawf(NAME_DevMusic, TEXT("Regained %i chunk buffers."), RawChunkBufferPool.Num() - OldPoolSize);
	return BuffersProcessed;
	unguard;
}

void UALAudioSubsystem::RawSamples(FLOAT* Data, INT NumSamples, INT NumChannels, INT SampleRate)
{
	guard(UALAudioSubsystem::RawSamples);
	rawf(NAME_DevMusic, TEXT("RawSamples(Data=0x%08x,NumSamples=%i,NumChannels=%i,SampleRate=%i)"), Data, NumSamples, NumChannels, SampleRate);
	check(Data);
	check(NumSamples>0);
	check(SampleRate>0);
	if (IsRawError)
		return;
	if (NumChannels<1 || NumChannels>2)
	{
		debugf(NAME_DevMusic, TEXT("Only mono and stereo raw stream is supported (NumChannels=%i). "), NumChannels);
		return;
	}

	// Clear Error.
	alGetError();

	//new(RawChunkBuffers) FRawChunk( Data, NumSamples, NumChannels, SampleRate );

	// Get a chunk.
	ALuint ChunkBuffer = 0;
	if (RawChunkBufferPool.Num() || RegainChunkBufferPool()>0 || ResizeRawChunkBufferPool(DESIRED_RAW_POOL_SIZE))
		ChunkBuffer = RawChunkBufferPool.Pop();
	else
		return;
	RawChunkBuffers.AddItem(ChunkBuffer);

	// Clamp.
	guard(Buffer);
#if 1
	// Okay now finally works, but keep the stuff below in case we need that if float format is not preset.
	FLOAT* ResampledData = new FLOAT[NumSamples];
	for (INT i = 0; i<NumSamples; i++)
		ResampledData[i] = Clamp(Data[i], -1.f, +1.f);
	// Buffer.
	alBufferData(ChunkBuffer, NumChannels == 2 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32, Data, NumSamples*sizeof(FLOAT), SampleRate);
	delete ResampledData;
#else
	// Sample to SWORD and buffer. Smirftsch always says he likes the SWORD type...
	SWORD* ResampledData = new SWORD[NumSamples/**NumChannels*/]; //
	for (INT i = 0; i<NumSamples/**NumChannels*/; i++)
		ResampledData[i] = Clamp(Data[i], -1.f, +1.f)*32767.0;
	alBufferData(ChunkBuffer, NumChannels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, ResampledData, NumSamples*sizeof(SWORD)/*NumChannels*/, SampleRate);
	delete ResampledData;
#endif
	if (alGetError() != AL_NO_ERROR)
	{
		debugf(NAME_DevMusic, TEXT("Failed to buffer data for raw stream. "));
		return;
	}
	alSourceQueueBuffers(RawSource, 1, &ChunkBuffer);
	if (alGetError() != AL_NO_ERROR)
	{
		debugf(NAME_DevMusic, TEXT("Failed to create queue buffer for raw stream. "));
		return;
	}
	unguard;

	// Start (again) if needed.
	ALint SourceState = 0;
	alGetSourcei(RawSource, AL_SOURCE_STATE, &SourceState);
	if (SourceState != AL_PLAYING)
		alSourcePlay(RawSource);
	unguard;
}
// FIX-ME: Figure out what needs to be returned here so we don't get a shitload of Chunks at once, or copy them asside and buffer them once a threshold is reached or Update() is called.
FLOAT UALAudioSubsystem::GetRawTimeOffset(FLOAT InOffset) // Looks like this is used to determine whether we need new samples.
{
	guard(UALAudioSubsystem::GetRawTimeOffset);
	rawf(NAME_DevMusic, TEXT("GetRawTimeOffset(InOffset=%f)"), InOffset);
	//static FLOAT Bla=0.f;
	if (IsRawStreaming)
		return (appSecondsAudio() - RawStreamStartTime) + InOffset;
	//return Latency/1000.f+InOffset;
	//return (Bla-=100.f);
	else
		return 0.f;
	unguard;
}
void UALAudioSubsystem::StopRawStream()
{
	guard(UALAudioSubsystem::StopRawStream);
	rawf(NAME_DevMusic, TEXT("(RawChunkBuffers#=%i,RawChunkBufferPool#=%i,IsRawError=%i)"), RawChunkBuffers.Num(), RawChunkBufferPool.Num(), IsRawError);
	if (IsRawError)
		return;

	debugf(NAME_DevMusic, TEXT("Stopping raw stream."));
	check(IsRawStreaming);

	// Delete source first, otherwise unqueueing fails and thus deletion. --han
	alDeleteSources(1, &RawSource);
	RawSource = 0;

	// Unqueue all buffers and add to pool.
	/*if ( RawChunkBuffers.Num() )
	{
	alSourceUnqueueBuffers( RawSource, RawChunkBuffers.Num(), &RawChunkBuffers(0) );
	if ( alGetError()!=AL_NO_ERROR )
	{
	debugf( NAME_DevMusic, TEXT("Failed to unqueue remaining raw chunks. ") );

	// Just delete now.
	alDeleteBuffers( RawChunkBuffers.Num(), &RawChunkBuffers(0) );
	RawChunkBuffers.Empty();
	}
	else
	{
	while ( RawChunkBuffers.Num() )
	RawChunkBufferPool.AddItem(RawChunkBuffers.Pop());
	}
	}*/

	// Resize Pool to desired size.
	if (!ResizeRawChunkBufferPool(DESIRED_RAW_POOL_SIZE))
		debugf(NAME_DevMusic, TEXT("Failed to resize raw queue buffer pool to desired size."));

	// Exit if not RawStreaming.
	if (!IsRawStreaming)
		return;

	IsRawStreaming = 0;
	unguard;
}

void UALAudioSubsystem::BeginRawStream(INT NumChannels, INT SampleRate)
{
	guard(UALAudioSubsystem::BeginRawStream);
	if (IsRawError)
	{
		debugf(NAME_DevMusic, TEXT("Cannot begin raw stream due to by prior raw streaming errors."));
		return;
	}

	// Check for AL_EXT_float32 presense. TODO: Move this check somewhere else and cache the result. Or even do some OpenGLDrv style.
	UBOOL SUPPORTS_AL_EXT_float32 = alIsExtensionPresent("AL_EXT_float32"); // Playback seems to be all fucked up, but I have not yet an idea why.
	if (!SUPPORTS_AL_EXT_float32)
	{
		// In fact I could just resample to a required to exist OpenAL 1.1 format, but...
		debugf(NAME_DevMusic, TEXT("Cannot begin raw stream: AL_EXT_float32 extension is missing."));
		return;
	}

	debugf(NAME_DevMusic, TEXT("Beginning raw stream (%i:%i)"), NumChannels, SampleRate);
	check(!IsRawStreaming);
	check(RawChunkBuffers.Num() == 0);

	// Clear error.
	alGetError();

	// Generate source.
	alGenSources(1, &RawSource);
	if (alGetError() != AL_NO_ERROR)
	{
		debugf(NAME_DevMusic, TEXT("Failed to create source for raw stream. "));
		return;
	}

	// Enlarge or shrink Pool if required.
	if (!ResizeRawChunkBufferPool(DESIRED_RAW_POOL_SIZE))
	{
		IsRawError = 1;
		return;
	}

	RawStreamStartTime = appSecondsAudio();
	IsRawStreaming = 1;

	// A bit odd, but it sets MusicVolume.
	UpdateRaw();
	unguard;
}

#endif // RUNE_CLASSIC.
std::atomic<UBOOL>				UALAudioSubsystem::EndBuffering(FALSE);

/*-----------------------------------------------------------------------------
The End.
-----------------------------------------------------------------------------*/
