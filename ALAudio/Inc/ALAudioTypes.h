/*=============================================================================
ALAudioSubsystem.cpp: Unreal OpenAL Audio interface object.
Copyright 2015 Oldunreal.

Revision history:
* Old initial version by Joseph I. Valenzuela (based on AudioSubsystem.cpp)
* Heavily modified and completely rewritten by Jochen 'Smirftsch' Görnitz
* Porting and porting help for other UEngine1 games, code cleanups, efx refactoring and lipsynch by Sebastian 'Hanfling' Kaufel
=============================================================================*/

/*------------------------------------------------------------------------------------
	Constants
------------------------------------------------------------------------------------*/

#define AUDIO_TOTALCHANNELS	32
#define AUDIO_TOTALVOICES	256

#define AUDIO_MINPAN		0
#define AUDIO_MIDPAN		16384
#define AUDIO_MAXPAN		32767
#define AUDIO_SURPAN		32768

#define AUDIO_MINVOLUME		0
#define AUDIO_MAXVOLUME		256

#define AUDIO_DISABLED		0
#define AUDIO_ENABLED		1

#define AUDIO_MINSAMPLE8	-128
#define AUDIO_MAXSAMPLE8	127

#define AUDIO_MINSAMPLE16	-32768
#define AUDIO_MAXSAMPLE16	32767

/*------------------------------------------------------------------------------------
	Flags
------------------------------------------------------------------------------------*/

#define AUDIO_MONO			0
#define AUDIO_STEREO		1

#define AUDIO_8BIT			0
#define AUDIO_16BIT			2
	
#define AUDIO_SHOLD			0
#define AUDIO_COSINE		4

#define AUDIO_2DAUDIO		0
#define AUDIO_3DAUDIO		8

#define SAMPLE_8BIT 		1
#define SAMPLE_16BIT		2
#define SAMPLE_BIDILOOP		4
#define SAMPLE_LOOPED		8
#define SAMPLE_MONO			16
#define SAMPLE_STEREO		32

#define MEM_SEEK_CUR		0
#define MEM_SEEK_ABS		1

#define VOICE_AUTO			0
#define VOICE_DISABLED		0
#define VOICE_ENABLED		1
#define VOICE_ACTIVE		2
#define VOICE_FINISHED		4

struct Sample
{
	DWORD	Size;			// Size of the rest of structure.
	_WORD	Panning;		// Panning position.
	_WORD	Volume;			// Volume level.
	_WORD	Type;			// Loop or sample type.
	DWORD	Length;			// Length in samples.
	DWORD	LoopStart;		// Start of loop in samples.
	DWORD	LoopEnd;		// End of loop in samples.
	DWORD	SamplesPerSec;		// Samples per second.
	void*	Data;			// Data.
};
