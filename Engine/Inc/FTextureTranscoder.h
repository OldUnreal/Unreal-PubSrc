/*=============================================================================
	FTextureTranscoder.h: Texture Transcoder interface definition Header.
	Copyright 2019-2020 Sebastian Kaufel. All Rights Reserved.

	Revision history:
	 * Created by Sebastian Kaufel.
=============================================================================*/

// Add pragma once, as subclasses will include this file.
#pragma once

/*-----------------------------------------------------------------------------
	FTextureTranscoder.
-----------------------------------------------------------------------------*/

// Guard macro for the FTextureTranscoder::Transcode interface.
#define guardTranscode(s) guard(s)
#define unguardTranscode  unguardf(( TEXT("(InputFormat=%ls,InputBytes=%i,OutputFormat=%ls,OutputBytes=%i,Width=%i,Height=%i,Depth=%i"), *FTextureFormatString(InputFormat), InputData.Num(), *FTextureFormatString(OutputFormat), OutputData.Num(), Width, Height, Depth ));

//
// Baseclass for FTextureBlockCompressor and FTextureBlockDecompressor
// for future FTexturePalettizer and FTextureDepalettizer and
// probably some uncompressed format convertion class.
//
// !! FIX-ME !! Figure out AlphaHint/Masked stuff and how to deal with
// Palette specific issues.
//
// !! Lossless (and channellossless interfaces...)
// Like shuffling RGBA into BGRA is lossless in any way. while RGBA into RG just losses channels, not precission.
// May differentiate between lossless and bit-identical. 8 bit -> 16 bit is lossless, but not bit identical.
// Maybe add an additional cost interface, which could rank compression as super high, while ranking
// shuffling as virtually free, while bit depth convertion beeing sth. in between.
//
class FTextureTranscoder
{
protected:

	// Variables.
	BYTE MemoryHint;
	BYTE SpeedHint;
	BYTE ColorspaceHint;
	BYTE AlphaHint;

public:

	// Constructor.
	FTextureTranscoder( const BYTE InMemoryHint=0, const BYTE InSpeedHint=TCSPEED_Default, const BYTE InColorspaceHint=TEXC_Undefined, const BYTE InAlphaHint=TEXA_Undefined )
	: MemoryHint(InMemoryHint)
	, SpeedHint(InSpeedHint)
	, ColorspaceHint(InColorspaceHint)
	, AlphaHint(InAlphaHint)
	{}

	// Whether transcoding from InputFormat is supported in general.
	virtual UBOOL SupportsInputFormat( const BYTE InputFormat )=0;

	// Whether transcoding to OutputFormat is supported in general.
	virtual UBOOL SupportsOutputFormat( const BYTE OutputFormat )=0;

	// Whether transcoding InputFormat to OutputFormat is supported.
	virtual UBOOL SupportsTranscoding( const BYTE InputFormat, const BYTE OutputFormat )=0;

	// Suggested input format for compression (may depend on Hints). Will return NODATA if OutputFormat is not supported.
	virtual BYTE SuggestedInputFormat( const BYTE OutputFormat )=0;

	// Suggested output format for decompression (may depend on Hints). Will return NODATA if InputFormat is not supported.
	virtual BYTE SuggestedOutputFormat( const BYTE InputFormat )=0;

	//
	// Transcode.
	//
	// Guaranteed to be save in case InputData refers to the same object as OutputData.
	// Will not modify InputData or OutputData in case of a failure.
	//
	// Supply GWarn as Error if you just want some verbose output (in case of UnrealEd)
	// or abort execution (in case of UBuildCommandlet).
	//
	// Code is FTextureTranscoder::Transcode provides:
	//  * Checks with SupportsTranscoding interface.
	//  * Verifies Width, Height and Depth are non zero and positive.
	//  * Verifies InputData is of correct size.
	//
	// !! Maybe localize.
	// !! Maybe consider integer overflow.
	//
	virtual UBOOL Transcode( const BYTE InputFormat, TArray<BYTE> InputData, const BYTE OutputFormat, TArray<BYTE>& OutputData, const INT Width, const INT Height, const INT Depth=1, FOutputDevice* Error=GError )
	{
		guardTranscode(FTextureTranscoder::Transcode);
		if ( InputFormat==OutputFormat )
		{
			Error->Logf( TEXT("InputFormat and OutputFormat are both %ls."), *FTextureFormatString(InputFormat) );
			return 0;
		}
		if ( FIsMetaFormat(InputFormat) )
		{
			Error->Logf( TEXT("InputFormat is MetaFormat %ls."), *FTextureFormatString(InputFormat) );
			return 0;
		}
		if ( FIsMetaFormat(OutputFormat) )
		{
			Error->Logf( TEXT("OutputFormat is MetaFormat %ls."), *FTextureFormatString(OutputFormat) );
			return 0;
		}
		if ( !SupportsTranscoding(InputFormat,OutputFormat) )
		{
			Error->Logf( TEXT("Transcoding from %ls to %ls is not supported."), *FTextureFormatString(InputFormat), *FTextureFormatString(OutputFormat) );
			return 0;
		}
		if ( Width<1 )
		{
			Error->Logf( TEXT("Width needs to be non zero and positive, but is %i."), Width );
			return 0;
		}
		if ( Height<1 )
		{
			Error->Logf( TEXT("Height needs to be non zero and positive, but is %i."), Height );
			return 0;
		}
		if ( Depth<1 )
		{
			Error->Logf( TEXT("Depth/Layers needs to be non zero and positive, but is %i."), Depth );
			return 0;
		}

		INT ExpectedInputBytes = FTextureBytes( InputFormat, Width, Height, Depth );
		if ( ExpectedInputBytes!=InputData.Num() )
		{
			Error->Logf( TEXT("Expected %i bytes as InputData size but got %i bytes."), ExpectedInputBytes, InputData.Num() );
			return 0;
		}

		check(ExpectedInputBytes>0);
		return 1;
		unguardTranscode;
	}

	// ?.
	virtual void SetMemoryHint( const BYTE InMemoryHint )
	{
		MemoryHint = InMemoryHint;
	}

	// ETextureTranscodingSpeed.
	virtual void SetSpeedHint( const BYTE InSpeedHint )
	{
		SpeedHint = InSpeedHint;
	}

	// ETextureColorspace.
	virtual void SetColorspaceHint( const BYTE InColorspaceHint )
	{
		ColorspaceHint = InColorspaceHint;
	}

	// ETextureAlphaspace.
	virtual void SetAlphaHint( const BYTE InAlphaHint )
	{ 
		AlphaHint = InAlphaHint;
	}
};

/*-----------------------------------------------------------------------------
	Daisy chaining.
-----------------------------------------------------------------------------*/

// !! TODO !! Create helpers to be able to figure out how to get from
// one format to another using multiple TextureTranscoders.

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
