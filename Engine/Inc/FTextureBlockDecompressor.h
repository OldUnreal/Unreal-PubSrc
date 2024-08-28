/*=============================================================================
	FTextureBlockDecompressor.h: A simple decompressor for hardware formats.
	Copyright 2020-2020 Sebastian Kaufel. All Rights Reserved.

	Revision history:
	 * Created by Sebastian Kaufel.
=============================================================================*/

#include "FTextureTranscoder.h"

/*-----------------------------------------------------------------------------
	FTextureBlockDecompressor.
-----------------------------------------------------------------------------*/

//
// Block Compression be gone.
//
class FTextureBlockDecompressor : public FTextureTranscoder
{
public:

	// Constructor.
	FTextureBlockDecompressor( const BYTE InMemoryHint=0, const BYTE InSpeedHint=TCSPEED_Default, const BYTE InColorspaceHint=TEXC_Undefined, const BYTE InAlphaHint=TEXA_Undefined )
	: FTextureTranscoder(InMemoryHint,InSpeedHint,InColorspaceHint,InAlphaHint)
	{}

	// FTextureTranscoder interface.
	UBOOL SupportsInputFormat( const BYTE InputFormat );
	UBOOL SupportsOutputFormat( const BYTE OutputFormat );
	UBOOL SupportsTranscoding( const BYTE InputFormat, const BYTE OutputFormat );
	BYTE SuggestedInputFormat( const BYTE OutputFormat );
	BYTE SuggestedOutputFormat( const BYTE InputFormat );
	UBOOL Transcode( const BYTE InputFormat, TArray<BYTE> InputData, const BYTE OutputFormat, TArray<BYTE>& OutputData, const INT Width, const INT Height, const INT Depth=1, FOutputDevice* Error=GError );
};

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
