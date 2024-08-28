/*=============================================================================
	UnAudio.h: Unreal base audio.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
		* Wave modification code by Erik de Neve
=============================================================================*/

class USound;
class UMusic;
class UAudioSubsystem;

/*-----------------------------------------------------------------------------
	Special Sony VAG Includes.
-----------------------------------------------------------------------------*/

#if HAVE_VAG
	#include "ENCVAG.H"
#endif

/*-----------------------------------------------------------------------------
	UAudioSubsystem.
-----------------------------------------------------------------------------*/

//
// UAudioSubsystem is the abstract base class of
// the game's audio subsystem.
//
class ENGINE_API UAudioSubsystem : public USubsystem
{
	DECLARE_ABSTRACT_CLASS(UAudioSubsystem,USubsystem,CLASS_Config,Engine)

	// UAudioSubsystem interface.
	virtual UBOOL Init() _VF_BASE_RET(0);
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog ) _VF_BASE_RET(0);
	virtual UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch ) _VF_BASE_RET(0);
	virtual UBOOL GetLowQualitySetting() _VF_BASE_RET(0);
	virtual UViewport* GetViewport() _VF_BASE_RET(NULL);
	virtual void SetViewport( UViewport* Viewport ) _VF_BASE;
	virtual void Update( FPointRegion Region, FCoords& Listener ) _VF_BASE;
	virtual void RegisterMusic( UMusic* Music ) _VF_BASE;
	virtual void RegisterSound( USound* Music ) _VF_BASE;
	virtual void UnregisterSound( USound* Sound ) _VF_BASE;
	virtual void UnregisterMusic( UMusic* Music ) _VF_BASE;
	virtual void NoteDestroy( AActor* Actor ) _VF_BASE;
	virtual void RenderAudioGeometry( FSceneNode* Frame ) _VF_BASE;
	virtual void PostRender( FSceneNode* Frame ) _VF_BASE;
	virtual void Flush() _VF_BASE;
	virtual void StopSoundId(INT Id) _VF_BASE;
	virtual void SetInstantSoundVolume(BYTE newSoundVolume) _VF_BASE;
	virtual void SetInstantSpeechVolume(BYTE newSpeechVolume) _VF_BASE;
	virtual void SetInstantMusicVolume(BYTE newMusicVolume) _VF_BASE;
	virtual UBOOL ResolveDuration(USound* Sound) { return 0; }
	virtual void CleanUp() {};
};

/*-----------------------------------------------------------------------------
	USound.
-----------------------------------------------------------------------------*/

#define LIPSynchFreq 32 // Frequency of the LipSync data samples (samples per second).

//
// Sound data.
//
class ENGINE_API FSoundData : public TLazyArray<BYTE>
{
private:
	TArray<BYTE> Decompressed, Source, LipSynch;
	BYTE CompressFlags;

public:
	USound* Owner;
	void Load();
	FLOAT GetPeriod();

	// Compress/Decompress to OGG
	BYTE Compress(BYTE Quality);
	BYTE Decompress();
	BYTE IsCompressed();
	TArray<BYTE>* GetAudio(BYTE bDecompressed = 1);
	TArray<BYTE>* GetLipSynch();
	FLOAT GetLipVolume(FLOAT Time); // Returns volume in 0-1 scale, Time is audio time in range 0 - Duration
	void MakeUncompressed();

	FSoundData( USound* InOwner )
	: CompressFlags(0), Owner( InOwner )
	{}
};

//
// A sound effect.
//
class ENGINE_API USound : public UObject
{
	DECLARE_CLASS(USound,UObject,(CLASS_SafeReplace | CLASS_RuntimeStatic),Engine)

	// Variables.
	FSoundData	Data;
	FName		FileType;
	INT			OriginalSize;
	FLOAT       Duration;
	void*		Handle;
	static UAudioSubsystem* Audio;

	// Constructor.
	USound()
	: Data( this )
	{
		Duration = -1.f;
	}

	// Duration.
	FLOAT GetDuration()
	{
		if (Duration < 0.f)
		{
			if (!Audio || !Audio->ResolveDuration(this))
				Duration = Data.GetPeriod();
		}
		return Duration;
	};

	void PS2Convert();

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();
	void PostLoad();
};

/*-----------------------------------------------------------------------------
	UMusic.
-----------------------------------------------------------------------------*/

//
// A song.
//
class ENGINE_API UMusic : public UObject
{
	DECLARE_CLASS(UMusic,UObject,(CLASS_SafeReplace | CLASS_RuntimeStatic),Engine)

	// Variables.
	TLazyArray<BYTE>	Data;
	FName				FileType;
	INT					OriginalSize;
	void*				Handle;
	static UAudioSubsystem* Audio;

	// Constructor.
	UMusic()
	{}

	// UObject implementation.
	void Serialize( FArchive& Ar );
	void Destroy();
	void PostLoad();
};

/*-----------------------------------------------------------------------------
	FWaveModInfo.
-----------------------------------------------------------------------------*/

//  Macros to convert 4 bytes to a Riff-style ID DWORD.
//  Todo: make these endian independent !!!

#define MAKEFOURCC(ch0, ch1, ch2, ch3)\
    ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |\
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define mmioFOURCC(ch0, ch1, ch2, ch3)\
    MAKEFOURCC(ch0, ch1, ch2, ch3)

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,2)
#endif

// Main Riff-Wave header.
struct FRiffWaveHeader
{
	DWORD	rID;			// Contains 'RIFF'
	DWORD	ChunkLen;		// Remaining length of the entire riff chunk (= file).
	DWORD	wID;			// Form type. Contains 'WAVE' for .wav files.
};

// General chunk header format.
struct FRiffChunkOld
{
	DWORD	ChunkID;		  // General data chunk ID like 'data', or 'fmt '
	DWORD	ChunkLen;		  // Length of the rest of this chunk in bytes.
};

// ChunkID: 'fmt ' ("WaveFormatEx" structure )
struct FFormatChunk
{
    _WORD   wFormatTag;        // Format type: 1 = PCM
    _WORD   nChannels;         // Number of channels (i.e. mono, stereo...).
    DWORD   nSamplesPerSec;    // Sample rate. 44100 or 22050 or 11025  Hz.
    DWORD   nAvgBytesPerSec;   // For buffer estimation  = sample rate * BlockAlign.
    _WORD   nBlockAlign;       // Block size of data = Channels times BYTES per sample.
    _WORD   wBitsPerSample;    // Number of bits per sample of mono data.
    _WORD   cbSize;            // The count in bytes of the size of extra information (after cbSize).
};

// ChunkID: 'smpl'
struct FSampleChunk
{
	DWORD   dwManufacturer;
	DWORD   dwProduct;
	DWORD   dwSamplePeriod;
	DWORD   dwMIDIUnityNote;
	DWORD   dwMIDIPitchFraction;
	DWORD	dwSMPTEFormat;
	DWORD   dwSMPTEOffset;		//
	DWORD   cSampleLoops;		// Number of tSampleLoop structures following this chunk
	DWORD   cbSamplerData;		//
};

struct FSampleLoop				// Immediately following cbSamplerData in the SMPL chunk.
{
	DWORD	dwIdentifier;		//
	DWORD	dwType;				//
	DWORD	dwStart;			// Startpoint of the loop in samples
	DWORD	dwEnd;				// Endpoint of the loop in samples
	DWORD	dwFraction;			// Fractional sample adjustment
	DWORD	dwPlayCount;		// Play count
};

// stijn: I am not proud of this but this is the cleanest solution I could think
// of. All of the structs above are aligned on 16-bit boundaries, which means
// that the DWORD pointers in the FWaveModInfo struct potentially point to
// unaligned memory. In optimized builds for ARM (an probably PPC), gcc assumes
// that all pointer derefs are aligned, so it generates optimized load/store
// instructions that trap on unaligned accesses. With this indirection, we tell
// the compiler that the DWORD pointers point to memory with 16-bit alignment so
// it should not generate instructions that cannot tolerate misalignment.
//
// NOTE: Unlike the original code, this solution _IS_ C compliant.

struct FUnalignedDWORD
{
	DWORD   dwUnaligned;
};

struct FUnalignedDWORDPtr
{
	FUnalignedDWORD* Ptr;

	DWORD operator*()
	{
		return Ptr->dwUnaligned;
	}

	void Set(DWORD In)
	{
		Ptr->dwUnaligned = In;
	}
	
	void operator=(DWORD* In)
	{
		Ptr = reinterpret_cast<FUnalignedDWORD*>(In);
	}
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

//
// Structure for in-memory interpretation and modification of WAVE sound structures.
//
class ENGINE_API FWaveModInfo
{
public:

	// Pointers to variables in the in-memory WAVE file.
	FUnalignedDWORDPtr pSamplesPerSec;
	FUnalignedDWORDPtr pAvgBytesPerSec;
	_WORD* pBlockAlign;
	_WORD* pBitsPerSample;
	_WORD* pChannels;

	DWORD  OldBitsPerSample;

	FUnalignedDWORDPtr pWaveDataSize;
	FUnalignedDWORDPtr pMasterSize;
	BYTE*  SampleDataStart;
	BYTE*  SampleDataEnd;
	DWORD  SampleDataSize;
	BYTE*  WaveDataEnd;

	INT	   SampleLoopsNum;
	FSampleLoop*  pSampleLoop;

	DWORD  NewDataSize;
	UBOOL  NoiseGate;

	// Constructor.
	FWaveModInfo()
	{
		NoiseGate   = false;
		SampleLoopsNum = 0;
	}

	// 16-bit padding.
	DWORD Pad16Bit( DWORD InDW )
	{
		return ((InDW + 1)& ~1);
	}

	// Read headers and load all info pointers in WaveModInfo.
	// Returns 0 if invalid data encountered.
	// UBOOL ReadWaveInfo( TArray<BYTE>& WavData );
	UBOOL ReadWaveInfo( TArray<BYTE>& WavData );

	// Handle RESIZING and updating of all variables needed for the new size:
	// notably the (possibly multiple) loop structures.
	UBOOL UpdateWaveData( TArray<BYTE>& WavData);

	// Wave size and/or bitdepth reduction.
	void Reduce16to8();
	void HalveData();
	void HalveReduce16to8();

	// Filters.
	void NoiseGateFilter();
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
