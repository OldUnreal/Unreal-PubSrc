/*=============================================================================
	UnTexComp.h: Unreal texture decompression routines.
	Copyright 2019-2020 Sebastian Kaufel, Inc. All Rights Reserved.

	Revision history:
	 * Created by Sebastian Kaufel.

	Wishlist:
	 * BC6H decompression.
	 * Routines for ASTC to check for HDR blocks !!
	 * Routines for BC6H/BC7 block information.
	 * PVRTC decompression.
	 * ASTC decompression.

	Known issues:
	 * BC4_S, BC5_S, EAC_R_S and EAC_RG_S decompression is completely untested.

	Notes:
	 * FTextureBlockDecompressor provides an higher level abstraction around
	   these low level routines.

	Example (Decompress ETC1 to RGBA8):

		// Size of this mip level (2x3, 1x16, 128x63, etc.).
		INT USize=?, VSize=?;

		// Compressed input data.
		BYTE* Input = ?;

		BYTE* Output = new BYTE[FTextureBytes(TEXF_RGBA8,USize,VSize)];

		DWORD* Ptr = (DWORD*)Output;

		for ( INT V=0; V<VSize; V+=FTextureBlockHeight(TEXF_ETC1) )
		{
			for ( INT U=0; U<USize; U+=FTextureBlockWidth(TEXF_ETC1) )
			{
				FColor Texels[FTextureBlockWidth(TEXF_ETC1)*FTextureBlockHeight(TEXF_ETC1)];

				// Don't bother with direct copy writing to dest when
				// possible as there are no ETC1 realtime textures.
				FDecompressBlockETC1( Src, Texels );

				for ( INT VCopy=0; VCopy<Min(VSize-V,FTextureBlockHeight(TEXF_ETC1)); VCopy++ )
					for ( INT UCopy=0; UCopy<Min(USize-U,FTextureBlockWidth(TEXF_ETC1)); UCopy++ )
						Ptr[(V+VCopy)*USize+U+UCopy] = GET_COLOR_DWORD(Texels[FTextureBlockWidth(TEXF_ETC1)*VCopy+UCopy]);

				Src += FTextureBlockBytes( TEXF_ETC1 );
			}
		}

=============================================================================*/

/*----------------------------------------------------------------------------
	ETextureCompressionSpeed.
----------------------------------------------------------------------------*/

//
// Texture compression time budget.
//
enum ETextureCompressionSpeed
{
	TCSPEED_Default   = 0x00,
	TCSPEED_UltraFast = 0x01,
	TCSPEED_VeryFast  = 0x02,
	TCSPEED_Fast      = 0x03,
	TCSPEED_Basic     = 0x04,
	TCSPEED_Slow      = 0x05,
	TCSPEED_VerySlow  = 0x06,
	TCSPEED_UltraSlow = 0x07, // Unused, just for completness sake for now.
	TCSPEED_MAX       = 0xff,
};

inline FString FTextureCompressionSpeedString( const BYTE Speed )
{
	guard(FTextureCompressionSpeedString);
	switch ( Speed )
	{
		case TCSPEED_Default:   return TEXT("Default");
		case TCSPEED_UltraFast: return TEXT("UltraFast");
		case TCSPEED_VeryFast:  return TEXT("VeryFast");
		case TCSPEED_Fast:      return TEXT("Fast");
		case TCSPEED_Basic:     return TEXT("Basic");
		case TCSPEED_Slow:      return TEXT("Slow");
		case TCSPEED_VerySlow:  return TEXT("VerySlow");
		case TCSPEED_UltraSlow: return TEXT("UltraSlow");
		case TCSPEED_MAX:       return TEXT("MAX");
		default:                return TEXT("(Unknown)");
	}
	unguard;
}

inline UBOOL GetTEXTURECOMPRESSIONSPEED( const TCHAR* Stream, BYTE& Value )
{
	guard(GetTEXTUREWRAP);
	if ( !Stream || !*Stream )
		return 0;
	FString Str(Stream);
	if ( Str==TEXT("TCSPEED_Default")   || Str==TEXT("Default")   ) { Value = TCSPEED_Default;   return 1; }
	if ( Str==TEXT("TCSPEED_UltraFast") || Str==TEXT("UltraFast") ) { Value = TCSPEED_UltraFast; return 1; }
	if ( Str==TEXT("TCSPEED_VeryFast")  || Str==TEXT("VeryFast")  ) { Value = TCSPEED_VeryFast;  return 1; }
	if ( Str==TEXT("TCSPEED_Fast")      || Str==TEXT("Fast")      ) { Value = TCSPEED_Fast;      return 1; }
	if ( Str==TEXT("TCSPEED_Basic")     || Str==TEXT("Basic")     ) { Value = TCSPEED_Basic;     return 1; }
	if ( Str==TEXT("TCSPEED_Slow")      || Str==TEXT("Slow")      ) { Value = TCSPEED_Slow;      return 1; }
	if ( Str==TEXT("TCSPEED_VerySlow")  || Str==TEXT("VerySlow")  ) { Value = TCSPEED_VerySlow;  return 1; }
	if ( Str==TEXT("TCSPEED_UltraSlow") || Str==TEXT("UltraSlow") ) { Value = TCSPEED_UltraSlow; return 1; }
	if ( appIsDigit(Stream[0]) )
	{
		Value = (BYTE)appAtoi(Stream);
		return 1;
	}
	return 0;
	unguard;
}

inline BYTE appGetTextureCompressionSpeedCommandline()
{
	guard(appGetTextureCompressionSpeedCommandline);
	if ( ParseParam(appCmdLine(),TEXT("ULTRAFASTTC")) ) return TCSPEED_UltraFast;
	if ( ParseParam(appCmdLine(),TEXT("VERYFASTTC" )) ) return TCSPEED_VeryFast;
	if ( ParseParam(appCmdLine(),TEXT("FASTTC"     )) ) return TCSPEED_Fast;
	if ( ParseParam(appCmdLine(),TEXT("BASICTC"    )) ) return TCSPEED_Basic;
	if ( ParseParam(appCmdLine(),TEXT("SLOWTC"     )) ) return TCSPEED_Slow;
	if ( ParseParam(appCmdLine(),TEXT("VERYSLOWTC" )) ) return TCSPEED_VerySlow;
	if ( ParseParam(appCmdLine(),TEXT("ULTRASLOWTC")) ) return TCSPEED_UltraSlow;
	return TCSPEED_Default;
	unguard;
}

/*----------------------------------------------------------------------------
	S3TC decompression routines.
----------------------------------------------------------------------------*/

//
// BC1-BC3 (S3TC).
//
// [DataFormat] https://www.khronos.org/registry/DataFormat/specs/1.2/dataformat.1.2.html#S3TC
// [OpenGL]     https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_texture_compression_s3tc.txt
// [D3D10]      https://docs.microsoft.com/de-de/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression
//
// BC2/BC3 ignore order of color endpoints and always uses the four gradient colors mode.
//
// S3TC is not a bit-exact encoding to a specific format, so multiple versions
// for different formats are needed depending on the use case. You can use
// FRecommendDecompressedFormat to figure out a suitable format.
//
// I could add BC1 RGB only variants for texture export only, but I feel
// like an FTranscode should rather perform this task instead. --han
//

//
// BC1.
//
inline void FDecompressBlockBC1( const BYTE Block[8], FColor Texels[16] )
{
	// Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)Block)>> 0)&31;
	RGB0.G = ((*(DWORD*)Block)>> 5)&63;
	RGB0.R = ((*(DWORD*)Block)>>11)&31;
	RGB1.B = ((*(DWORD*)Block)>>16)&31;
	RGB1.G = ((*(DWORD*)Block)>>21)&63;
	RGB1.R = ((*(DWORD*)Block)>>27)&31;
#else
	FBitReader BitReader( const_cast<BYTE*>(Block), 64 );

	// BGR order.
	RGB0.B = BitReader.ReadBits(5);
	RGB0.G = BitReader.ReadBits(6);
	RGB0.R = BitReader.ReadBits(5);
	RGB1.B = BitReader.ReadBits(5);
	RGB1.G = BitReader.ReadBits(6);
	RGB1.R = BitReader.ReadBits(5);
#endif

	FColor Palette[4];

	Palette[0] = RGB0.ExpandRGBA8();
	Palette[1] = RGB1.ExpandRGBA8();

	// Need to compare in initial BGR order here.
#if __INTEL_BYTE_ORDER__
	if ( GET_COLOR_WORD(Block[0])>GET_COLOR_WORD(Block[2]) )
#else
	if ( Block[1]>Block[3] || (Block[1]==Block[3] && Block[0]>Block[2]) )
#endif
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
		Palette[3] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.
	}
	else
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB1 );
		Palette[3] = FColor(0,0,0,255); // Opaque black.
	}

	for ( INT Index=0; Index<16; Index++ )
#if __INTEL_BYTE_ORDER__
		Texels[Index] = Palette[((*(DWORD*)&Block[4])>>(2*Index))&3];
#else
		Texels[Index] = Palette[BitReader.ReadBits(2)];
#endif
}

inline void FDecompressBlockBC1( const BYTE Block[8], FColor16 Texels[16] )
{
	// Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)Block)>> 0)&31;
	RGB0.G = ((*(DWORD*)Block)>> 5)&63;
	RGB0.R = ((*(DWORD*)Block)>>11)&31;
	RGB1.B = ((*(DWORD*)Block)>>16)&31;
	RGB1.G = ((*(DWORD*)Block)>>21)&63;
	RGB1.R = ((*(DWORD*)Block)>>27)&31;
#else
	FBitReader BitReader( const_cast<BYTE*>(Block), 64 );

	// BGR order.
	RGB0.B = BitReader.ReadBits(5);
	RGB0.G = BitReader.ReadBits(6);
	RGB0.R = BitReader.ReadBits(5);
	RGB1.B = BitReader.ReadBits(5);
	RGB1.G = BitReader.ReadBits(6);
	RGB1.R = BitReader.ReadBits(5);
#endif

	FColor16 Palette[4];

	Palette[0] = RGB0.ExpandRGBA16();
	Palette[1] = RGB1.ExpandRGBA16();

	// Need to compare in initial BGR order here.
#if __INTEL_BYTE_ORDER__
	if ( GET_COLOR_WORD(Block[0])>GET_COLOR_WORD(Block[2]) )
#else
	if ( Block[1]>Block[3] || (Block[1]==Block[3] && Block[0]>Block[2]) )
#endif
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
		Palette[3] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.
	}
	else
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB1 );
		Palette[3] = FColor16(0,0,0,65535); // Opaque black.
	}

	for ( INT Index=0; Index<16; Index++ )
#if __INTEL_BYTE_ORDER__
		Texels[Index] = Palette[((*(DWORD*)&Block[4])>>(2*Index))&3];
#else
		Texels[Index] = Palette[BitReader.ReadBits(2)];
#endif
}

inline void FDecompressBlockBC1( const BYTE Block[8], FColor32F Texels[16] )
{
	// Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)Block)>> 0)&31;
	RGB0.G = ((*(DWORD*)Block)>> 5)&63;
	RGB0.R = ((*(DWORD*)Block)>>11)&31;
	RGB1.B = ((*(DWORD*)Block)>>16)&31;
	RGB1.G = ((*(DWORD*)Block)>>21)&63;
	RGB1.R = ((*(DWORD*)Block)>>27)&31;
#else
	FBitReader BitReader( const_cast<BYTE*>(Block), 64 );

	// BGR order.
	RGB0.B = BitReader.ReadBits(5);
	RGB0.G = BitReader.ReadBits(6);
	RGB0.R = BitReader.ReadBits(5);
	RGB1.B = BitReader.ReadBits(5);
	RGB1.G = BitReader.ReadBits(6);
	RGB1.R = BitReader.ReadBits(5);
#endif

	FColor32F Palette[4];

	Palette[0] = RGB0.ExpandRGBA32F();
	Palette[1] = RGB1.ExpandRGBA32F();

	// Need to compare in initial BGR order here.
#if __INTEL_BYTE_ORDER__
	if ( GET_COLOR_WORD(Block[0])>GET_COLOR_WORD(Block[2]) )
#else
	if ( Block[1]>Block[3] || (Block[1]==Block[3] && Block[0]>Block[2]) )
#endif
	{
		Palette[2] = (2.0*Palette[0]+Palette[1])/3.0;
		Palette[3] = (Palette[0]+2.0*Palette[1])/3.0;

		// Make sure alpha is exactly 1.0. --han
		Palette[2].A = Palette[3].A = 1.0;
	}
	else
	{
		Palette[2] = (Palette[0]+Palette[1])/2.0;
		Palette[3] = FColor32F(0.0,0.0,0.0,1.0); // Opaque black.

		// This shouldn't be required as 0.5 is exactly representable as floating point.
		Palette[2].A = 1.0;
	}

	for ( INT Index=0; Index<16; Index++ )
#if __INTEL_BYTE_ORDER__
		Texels[Index] = Palette[((*(DWORD*)&Block[4])>>(2*Index))&3];
#else
		Texels[Index] = Palette[BitReader.ReadBits(2)];
#endif
}

//
// BC1_PA.
//
inline void FDecompressBlockBC1_PA( const BYTE Block[8], FColor Texels[16] )
{
	// Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)Block)>> 0)&31;
	RGB0.G = ((*(DWORD*)Block)>> 5)&63;
	RGB0.R = ((*(DWORD*)Block)>>11)&31;
	RGB1.B = ((*(DWORD*)Block)>>16)&31;
	RGB1.G = ((*(DWORD*)Block)>>21)&63;
	RGB1.R = ((*(DWORD*)Block)>>27)&31;
#else
	FBitReader BitReader( const_cast<BYTE*>(Block), 64 );

	// BGR order.
	RGB0.B = BitReader.ReadBits(5);
	RGB0.G = BitReader.ReadBits(6);
	RGB0.R = BitReader.ReadBits(5);
	RGB1.B = BitReader.ReadBits(5);
	RGB1.G = BitReader.ReadBits(6);
	RGB1.R = BitReader.ReadBits(5);
#endif

	FColor Palette[4];

	Palette[0] = RGB0.ExpandRGBA8();
	Palette[1] = RGB1.ExpandRGBA8();

	// Need to compare in initial BGR order here.
#if __INTEL_BYTE_ORDER__
	if ( GET_COLOR_WORD(Block[0])>GET_COLOR_WORD(Block[2]) )
#else
	if ( Block[1]>Block[3] || (Block[1]==Block[3] && Block[0]>Block[2]) )
#endif
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
		Palette[3] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.
	}
	else
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB1 );
		Palette[3] = FColor(0,0,0,0); // Clear.
	}

	for ( INT Index=0; Index<16; Index++ )
#if __INTEL_BYTE_ORDER__
		Texels[Index] = Palette[((*(DWORD*)&Block[4])>>(2*Index))&3];
#else
		Texels[Index] = Palette[BitReader.ReadBits(2)];
#endif
}

inline void FDecompressBlockBC1_PA( const BYTE Block[8], FColor16 Texels[16] )
{
	// Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)Block)>> 0)&31;
	RGB0.G = ((*(DWORD*)Block)>> 5)&63;
	RGB0.R = ((*(DWORD*)Block)>>11)&31;
	RGB1.B = ((*(DWORD*)Block)>>16)&31;
	RGB1.G = ((*(DWORD*)Block)>>21)&63;
	RGB1.R = ((*(DWORD*)Block)>>27)&31;
#else
	FBitReader BitReader( const_cast<BYTE*>(Block), 64 );

	// BGR order.
	RGB0.B = BitReader.ReadBits(5);
	RGB0.G = BitReader.ReadBits(6);
	RGB0.R = BitReader.ReadBits(5);
	RGB1.B = BitReader.ReadBits(5);
	RGB1.G = BitReader.ReadBits(6);
	RGB1.R = BitReader.ReadBits(5);
#endif

	FColor16 Palette[4];

	Palette[0] = RGB0.ExpandRGBA16();
	Palette[1] = RGB1.ExpandRGBA16();

	// Need to compare in initial BGR order here.
#if __INTEL_BYTE_ORDER__
	if ( GET_COLOR_WORD(Block[0])>GET_COLOR_WORD(Block[2]) )
#else
	if ( Block[1]>Block[3] || (Block[1]==Block[3] && Block[0]>Block[2]) )
#endif
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
		Palette[3] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.
	}
	else
	{
		Palette[2] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB1 );
		Palette[3] = FColor16(0,0,0,0); // Clear.
	}

	for ( INT Index=0; Index<16; Index++ )
#if __INTEL_BYTE_ORDER__
		Texels[Index] = Palette[((*(DWORD*)&Block[4])>>(2*Index))&3];
#else
		Texels[Index] = Palette[BitReader.ReadBits(2)];
#endif
}

inline void FDecompressBlockBC1_PA( const BYTE Block[8], FColor32F Texels[16] )
{
	// Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)Block)>> 0)&31;
	RGB0.G = ((*(DWORD*)Block)>> 5)&63;
	RGB0.R = ((*(DWORD*)Block)>>11)&31;
	RGB1.B = ((*(DWORD*)Block)>>16)&31;
	RGB1.G = ((*(DWORD*)Block)>>21)&63;
	RGB1.R = ((*(DWORD*)Block)>>27)&31;
#else
	FBitReader BitReader( const_cast<BYTE*>(Block), 64 );

	// BGR order.
	RGB0.B = BitReader.ReadBits(5);
	RGB0.G = BitReader.ReadBits(6);
	RGB0.R = BitReader.ReadBits(5);
	RGB1.B = BitReader.ReadBits(5);
	RGB1.G = BitReader.ReadBits(6);
	RGB1.R = BitReader.ReadBits(5);
#endif

	FColor32F Palette[4];

	Palette[0] = RGB0.ExpandRGBA32F();
	Palette[1] = RGB1.ExpandRGBA32F();

	// Need to compare in initial BGR order here.
#if __INTEL_BYTE_ORDER__
	if ( GET_COLOR_WORD(Block[0])>GET_COLOR_WORD(Block[2]) )
#else
	if ( Block[1]>Block[3] || (Block[1]==Block[3] && Block[0]>Block[2]) )
#endif
	{
		Palette[2] = (2.0*Palette[0]+Palette[1])/3.0;
		Palette[3] = (Palette[0]+2.0*Palette[1])/3.0;

		// Make sure alpha is exactly 1.0. --han
		Palette[2].A = Palette[3].A = 1.0;
	}
	else
	{
		Palette[2] = (Palette[0]+Palette[1])/2.0;
		Palette[3] = FColor32F(0.0,0.0,0.0,0.0); // Clear.

		// This shouldn't be required as 0.5 is exactly representable as floating point.
		Palette[2].A = 1.0; // !! Don't override Palette[3].A here.
	}

	for ( INT Index=0; Index<16; Index++ )
#if __INTEL_BYTE_ORDER__
		Texels[Index] = Palette[((*(DWORD*)&Block[4])>>(2*Index))&3];
#else
		Texels[Index] = Palette[BitReader.ReadBits(2)];
#endif
}

//
// BC2.
//
inline void FDecompressBlockBC2( const BYTE Block[16], FColor Texels[16] )
{
	// Color Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)&Block[8])>> 0)&31;
	RGB0.G = ((*(DWORD*)&Block[8])>> 5)&63;
	RGB0.R = ((*(DWORD*)&Block[8])>>11)&31;
	RGB1.B = ((*(DWORD*)&Block[8])>>16)&31;
	RGB1.G = ((*(DWORD*)&Block[8])>>21)&63;
	RGB1.R = ((*(DWORD*)&Block[8])>>27)&31;
#else
	FBitReader AlphaBitReader( &Block[0], 64 );
	FBitReader ColorBitReader( &Block[8], 64 );

	// BGR order.
	RGB0.B = ColorBitReader.ReadBits(5);
	RGB0.G = ColorBitReader.ReadBits(6);
	RGB0.R = ColorBitReader.ReadBits(5);
	RGB1.B = ColorBitReader.ReadBits(5);
	RGB1.G = ColorBitReader.ReadBits(6);
	RGB1.R = ColorBitReader.ReadBits(5);
#endif

	FColor ColorPalette[4];

	ColorPalette[0] = RGB0.ExpandRGBA8();
	ColorPalette[1] = RGB1.ExpandRGBA8();

	// Endpoint order is ignored according to specs. I haven't verified this behaviour on hardware. --han
	ColorPalette[2] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
	ColorPalette[3] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.

	for ( INT Index=0; Index<16; Index++ )
	{
#if __INTEL_BYTE_ORDER__
		INT ColorPaletteIndex = ((*(DWORD*)&Block[12])>>(2*Index))&3;
		INT AlphaBits         = ((*(QWORD*)&Block[0])>>(4*Index))&15;
#else
		INT ColorPaletteIndex = ColorBitReader.ReadBits(2);
		INT AlphaBits         = AlphaBitReader.ReadBits(4);
#endif

		Texels[Index].R = ColorPalette[ColorPaletteIndex].R;
		Texels[Index].G = ColorPalette[ColorPaletteIndex].G;
		Texels[Index].B = ColorPalette[ColorPaletteIndex].B;
		Texels[Index].A = (AlphaBits*255+7)/15;
	}
}

inline void FDecompressBlockBC2( const BYTE Block[16], FColor16 Texels[16] )
{
	// Color Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)&Block[8])>> 0)&31;
	RGB0.G = ((*(DWORD*)&Block[8])>> 5)&63;
	RGB0.R = ((*(DWORD*)&Block[8])>>11)&31;
	RGB1.B = ((*(DWORD*)&Block[8])>>16)&31;
	RGB1.G = ((*(DWORD*)&Block[8])>>21)&63;
	RGB1.R = ((*(DWORD*)&Block[8])>>27)&31;
#else
	FBitReader AlphaBitReader( &Block[0], 64 );
	FBitReader ColorBitReader( &Block[8], 64 );

	// BGR order.
	RGB0.B = ColorBitReader.ReadBits(5);
	RGB0.G = ColorBitReader.ReadBits(6);
	RGB0.R = ColorBitReader.ReadBits(5);
	RGB1.B = ColorBitReader.ReadBits(5);
	RGB1.G = ColorBitReader.ReadBits(6);
	RGB1.R = ColorBitReader.ReadBits(5);
#endif

	FColor16 ColorPalette[4];

	ColorPalette[0] = RGB0.ExpandRGBA16();
	ColorPalette[1] = RGB1.ExpandRGBA16();

	// Endpoint order is ignored according to specs. I haven't verified this behaviour on hardware. --han
	ColorPalette[2] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
	ColorPalette[3] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.

	for ( INT Index=0; Index<16; Index++ )
	{
#if __INTEL_BYTE_ORDER__
		INT ColorPaletteIndex = ((*(DWORD*)&Block[12])>>(2*Index))&3;
		INT AlphaBits         = ((*(QWORD*)&Block[0])>>(4*Index))&15;
#else
		INT ColorPaletteIndex = ColorBitReader.ReadBits(2);
		INT AlphaBits         = AlphaBitReader.ReadBits(4);
#endif

		Texels[Index].R = ColorPalette[ColorPaletteIndex].R;
		Texels[Index].G = ColorPalette[ColorPaletteIndex].G;
		Texels[Index].B = ColorPalette[ColorPaletteIndex].B;
		Texels[Index].A = (AlphaBits*65535+7)/15;
	}
}

inline void FDecompressBlockBC2( const BYTE Block[16], FColor32F Texels[16] )
{
	// Color Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// BGR order.
	RGB0.B = ((*(DWORD*)&Block[8])>> 0)&31;
	RGB0.G = ((*(DWORD*)&Block[8])>> 5)&63;
	RGB0.R = ((*(DWORD*)&Block[8])>>11)&31;
	RGB1.B = ((*(DWORD*)&Block[8])>>16)&31;
	RGB1.G = ((*(DWORD*)&Block[8])>>21)&63;
	RGB1.R = ((*(DWORD*)&Block[8])>>27)&31;
#else
	FBitReader AlphaBitReader( &Block[0], 64 );
	FBitReader ColorBitReader( &Block[8], 64 );

	// BGR order.
	RGB0.B = ColorBitReader.ReadBits(5);
	RGB0.G = ColorBitReader.ReadBits(6);
	RGB0.R = ColorBitReader.ReadBits(5);
	RGB1.B = ColorBitReader.ReadBits(5);
	RGB1.G = ColorBitReader.ReadBits(6);
	RGB1.R = ColorBitReader.ReadBits(5);
#endif

	FColor32F ColorPalette[4];

	ColorPalette[0] = RGB0.ExpandRGBA32F();
	ColorPalette[1] = RGB1.ExpandRGBA32F();

	// Endpoint order is ignored according to specs. I haven't verified this behaviour on hardware. --han
	ColorPalette[2] = (2.0*ColorPalette[0]+1.0*ColorPalette[1])/3.0;
	ColorPalette[3] = (1.0*ColorPalette[0]+2.0*ColorPalette[1])/3.0;

	for ( INT Index=0; Index<16; Index++ )
	{
#if __INTEL_BYTE_ORDER__
		INT ColorPaletteIndex = ((*(DWORD*)&Block[12])>>(2*Index))&3;
		INT AlphaBits         = ((*(QWORD*)&Block[0])>>(4*Index))&15;
#else
		INT ColorPaletteIndex = ColorBitReader.ReadBits(2);
		INT AlphaBits         = AlphaBitReader.ReadBits(4);
#endif

		Texels[Index].R = ColorPalette[ColorPaletteIndex].R;
		Texels[Index].G = ColorPalette[ColorPaletteIndex].G;
		Texels[Index].B = ColorPalette[ColorPaletteIndex].B;
		Texels[Index].A = AlphaBits/15.0;
	}
}

//
// BC3.
//
inline void FDecompressBlockBC3( const BYTE Block[16], FColor Texels[16] )
{
	// Color Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// Alpha Endpoints.
	const BYTE& A0 = Block[0];
	const BYTE& A1 = Block[1];

	// BGR order.
	RGB0.B = ((*(DWORD*)&Block[8])>> 0)&31;
	RGB0.G = ((*(DWORD*)&Block[8])>> 5)&63;
	RGB0.R = ((*(DWORD*)&Block[8])>>11)&31;
	RGB1.B = ((*(DWORD*)&Block[8])>>16)&31;
	RGB1.G = ((*(DWORD*)&Block[8])>>21)&63;
	RGB1.R = ((*(DWORD*)&Block[8])>>27)&31;
#else
	FBitReader AlphaBitReader( &Block[0], 64 );
	FBitReader ColorBitReader( &Block[8], 64 );

	// Alpha Endpoints.
	BYTE A0 = AlphaBitReader.ReadBits(8);
	BYTE A1 = AlphaBitReader.ReadBits(8);

	// BGR order.
	RGB0.B = ColorBitReader.ReadBits(5);
	RGB0.G = ColorBitReader.ReadBits(6);
	RGB0.R = ColorBitReader.ReadBits(5);
	RGB1.B = ColorBitReader.ReadBits(5);
	RGB1.G = ColorBitReader.ReadBits(6);
	RGB1.R = ColorBitReader.ReadBits(5);
#endif

	BYTE AlphaPalette[8];

	AlphaPalette[0] = A0;
	AlphaPalette[1] = A1;

	if ( A0>A1 )
	{
		AlphaPalette[2] = (6*AlphaPalette[0]+1*AlphaPalette[1]+3)/7; // Integer version of appRound((6*A0+1*A1)/7.0).
		AlphaPalette[3] = (5*AlphaPalette[0]+2*AlphaPalette[1]+3)/7; // Integer version of appRound((5*A0+2*A1)/7.0).
		AlphaPalette[4] = (4*AlphaPalette[0]+3*AlphaPalette[1]+3)/7; // Integer version of appRound((4*A0+3*A1)/7.0).
		AlphaPalette[5] = (3*AlphaPalette[0]+4*AlphaPalette[1]+3)/7; // Integer version of appRound((3*A0+4*A1)/7.0).
		AlphaPalette[6] = (2*AlphaPalette[0]+5*AlphaPalette[1]+3)/7; // Integer version of appRound((2*A0+5*A1)/7.0).
		AlphaPalette[7] = (1*AlphaPalette[0]+6*AlphaPalette[1]+3)/7; // Integer version of appRound((1*A0+6*A1)/7.0).
	}
	else
	{
		AlphaPalette[2] = (4*AlphaPalette[0]+1*AlphaPalette[1]+2)/5; // Integer version of appRound((4*A0+1*A1)/5.0).
		AlphaPalette[3] = (3*AlphaPalette[0]+2*AlphaPalette[1]+2)/5; // Integer version of appRound((3*A0+2*A1)/5.0).
		AlphaPalette[4] = (2*AlphaPalette[0]+3*AlphaPalette[1]+2)/5; // Integer version of appRound((2*A0+3*A1)/5.0).
		AlphaPalette[5] = (1*AlphaPalette[0]+4*AlphaPalette[1]+2)/5; // Integer version of appRound((1*A0+4*A1)/5.0).
		AlphaPalette[6] = 0;
		AlphaPalette[7] = 255;
	}

	FColor ColorPalette[4];

	ColorPalette[0] = RGB0.ExpandRGBA8();
	ColorPalette[1] = RGB1.ExpandRGBA8();

	// Endpoint order is ignored according to specs. I haven't verified this behaviour on hardware. --han
	ColorPalette[2] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
	ColorPalette[3] = FRed5Green6Blue5::ExpandSumRGBA8( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.

	for ( INT Index=0; Index<16; Index++ )
	{
#if __INTEL_BYTE_ORDER__
		INT AlphaPaletteIndex = (((*(QWORD*)&Block[0])>>(16+3*Index))&7);
		INT ColorPaletteIndex = ((*(DWORD*)&Block[12])>>(2*Index))&3;
#else
		INT AlphaPaletteIndex = AlphaBitReader.ReadBits(3);
		INT ColorPaletteIndex = ColorBitReader.ReadBits(2);
#endif

		Texels[Index].R = ColorPalette[ColorPaletteIndex].R;
		Texels[Index].G = ColorPalette[ColorPaletteIndex].G;
		Texels[Index].B = ColorPalette[ColorPaletteIndex].B;
		Texels[Index].A = AlphaPalette[AlphaPaletteIndex];
	}
}

inline void FDecompressBlockBC3( const BYTE Block[16], FColor16 Texels[16] )
{
	// Color Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// Alpha Endpoints.
	const BYTE& A0 = Block[0];
	const BYTE& A1 = Block[1];

	// BGR order.
	RGB0.B = ((*(DWORD*)&Block[8])>> 0)&31;
	RGB0.G = ((*(DWORD*)&Block[8])>> 5)&63;
	RGB0.R = ((*(DWORD*)&Block[8])>>11)&31;
	RGB1.B = ((*(DWORD*)&Block[8])>>16)&31;
	RGB1.G = ((*(DWORD*)&Block[8])>>21)&63;
	RGB1.R = ((*(DWORD*)&Block[8])>>27)&31;
#else
	FBitReader AlphaBitReader( &Block[0], 64 );
	FBitReader ColorBitReader( &Block[8], 64 );

	// Alpha Endpoints.
	BYTE A0 = AlphaBitReader.ReadBits(8);
	BYTE A1 = AlphaBitReader.ReadBits(8);

	// BGR order.
	RGB0.B = ColorBitReader.ReadBits(5);
	RGB0.G = ColorBitReader.ReadBits(6);
	RGB0.R = ColorBitReader.ReadBits(5);
	RGB1.B = ColorBitReader.ReadBits(5);
	RGB1.G = ColorBitReader.ReadBits(6);
	RGB1.R = ColorBitReader.ReadBits(5);
#endif

	_WORD AlphaPalette[8];

	AlphaPalette[0] = (A0*65535+127)/255;
	AlphaPalette[1] = (A1*65535+127)/255;

	if ( A0>A1 )
	{
		AlphaPalette[2] = (6*AlphaPalette[0]+1*AlphaPalette[1]+3)/7;
		AlphaPalette[3] = (5*AlphaPalette[0]+2*AlphaPalette[1]+3)/7;
		AlphaPalette[4] = (4*AlphaPalette[0]+3*AlphaPalette[1]+3)/7;
		AlphaPalette[5] = (3*AlphaPalette[0]+4*AlphaPalette[1]+3)/7;
		AlphaPalette[6] = (2*AlphaPalette[0]+5*AlphaPalette[1]+3)/7;
		AlphaPalette[7] = (1*AlphaPalette[0]+6*AlphaPalette[1]+3)/7;
	}
	else
	{
		AlphaPalette[2] = (4*AlphaPalette[0]+1*AlphaPalette[1]+2)/5;
		AlphaPalette[3] = (3*AlphaPalette[0]+2*AlphaPalette[1]+2)/5;
		AlphaPalette[4] = (2*AlphaPalette[0]+3*AlphaPalette[1]+2)/5;
		AlphaPalette[5] = (1*AlphaPalette[0]+4*AlphaPalette[1]+2)/5;
		AlphaPalette[6] = 0;
		AlphaPalette[7] = 65535;
	}

	FColor16 ColorPalette[4];

	ColorPalette[0] = RGB0.ExpandRGBA16();
	ColorPalette[1] = RGB1.ExpandRGBA16();

	// Endpoint order is ignored according to specs. I haven't verified this behaviour on hardware. --han
	ColorPalette[2] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB0, RGB1 ); // (2*RGB0+RGB1)/3.
	ColorPalette[3] = FRed5Green6Blue5::ExpandSumRGBA16( RGB0, RGB1, RGB1 ); // (RGB0+2*RGB1)/3.

	for ( INT Index=0; Index<16; Index++ )
	{
#if __INTEL_BYTE_ORDER__
		INT AlphaPaletteIndex = (((*(QWORD*)&Block[0])>>(16+3*Index))&7);
		INT ColorPaletteIndex = ((*(DWORD*)&Block[12])>>(2*Index))&3;
#else
		INT AlphaPaletteIndex = AlphaBitReader.ReadBits(3);
		INT ColorPaletteIndex = ColorBitReader.ReadBits(2);
#endif

		Texels[Index].R = ColorPalette[ColorPaletteIndex].R;
		Texels[Index].G = ColorPalette[ColorPaletteIndex].G;
		Texels[Index].B = ColorPalette[ColorPaletteIndex].B;
		Texels[Index].A = AlphaPalette[AlphaPaletteIndex];
	}
}

inline void FDecompressBlockBC3( const BYTE Block[16], FColor32F Texels[16] )
{
	// Color Endpoints.
	FRed5Green6Blue5 RGB0, RGB1;

#if __INTEL_BYTE_ORDER__
	// Alpha Endpoints.
	const BYTE& A0 = Block[0];
	const BYTE& A1 = Block[1];

	// BGR order.
	RGB0.B = ((*(DWORD*)&Block[8])>> 0)&31;
	RGB0.G = ((*(DWORD*)&Block[8])>> 5)&63;
	RGB0.R = ((*(DWORD*)&Block[8])>>11)&31;
	RGB1.B = ((*(DWORD*)&Block[8])>>16)&31;
	RGB1.G = ((*(DWORD*)&Block[8])>>21)&63;
	RGB1.R = ((*(DWORD*)&Block[8])>>27)&31;
#else
	FBitReader AlphaBitReader( &Block[0], 64 );
	FBitReader ColorBitReader( &Block[8], 64 );

	// Alpha Endpoints.
	BYTE A0 = AlphaBitReader.ReadBits(8);
	BYTE A1 = AlphaBitReader.ReadBits(8);

	// BGR order.
	RGB0.B = ColorBitReader.ReadBits(5);
	RGB0.G = ColorBitReader.ReadBits(6);
	RGB0.R = ColorBitReader.ReadBits(5);
	RGB1.B = ColorBitReader.ReadBits(5);
	RGB1.G = ColorBitReader.ReadBits(6);
	RGB1.R = ColorBitReader.ReadBits(5);
#endif

	FLOAT AlphaPalette[8];

	AlphaPalette[0] = A0/255.0;
	AlphaPalette[1] = A1/255.0;

	if ( A0>A1 )
	{
		AlphaPalette[2] = (6.0*AlphaPalette[0]+1.0*AlphaPalette[1])/7.0;
		AlphaPalette[3] = (5.0*AlphaPalette[0]+2.0*AlphaPalette[1])/7.0;
		AlphaPalette[4] = (4.0*AlphaPalette[0]+3.0*AlphaPalette[1])/7.0;
		AlphaPalette[5] = (3.0*AlphaPalette[0]+4.0*AlphaPalette[1])/7.0;
		AlphaPalette[6] = (2.0*AlphaPalette[0]+5.0*AlphaPalette[1])/7.0;
		AlphaPalette[7] = (1.0*AlphaPalette[0]+6.0*AlphaPalette[1])/7.0;
	}
	else
	{
		AlphaPalette[2] = (4.0*AlphaPalette[0]+1.0*AlphaPalette[1])/5.0;
		AlphaPalette[3] = (3.0*AlphaPalette[0]+2.0*AlphaPalette[1])/5.0;
		AlphaPalette[4] = (2.0*AlphaPalette[0]+3.0*AlphaPalette[1])/5.0;
		AlphaPalette[5] = (1.0*AlphaPalette[0]+4.0*AlphaPalette[1])/5.0;
		AlphaPalette[6] = 0.0;
		AlphaPalette[7] = 1.0;
	}

	FColor32F ColorPalette[4];

	ColorPalette[0] = RGB0.ExpandRGBA32F();
	ColorPalette[1] = RGB1.ExpandRGBA32F();

	// Endpoint order is ignored according to specs. I haven't verified this behaviour on hardware. --han
	ColorPalette[2] = (2.0*ColorPalette[0]+ColorPalette[1])/3.0;
	ColorPalette[3] = (ColorPalette[0]+2.0*ColorPalette[1])/3.0;

	for ( INT Index=0; Index<16; Index++ )
	{
#if __INTEL_BYTE_ORDER__
		INT AlphaPaletteIndex = (((*(QWORD*)&Block[0])>>(16+3*Index))&7);
		INT ColorPaletteIndex = ((*(DWORD*)&Block[12])>>(2*Index))&3;
#else
		INT AlphaPaletteIndex = AlphaBitReader.ReadBits(3);
		INT ColorPaletteIndex = ColorBitReader.ReadBits(2);
#endif

		Texels[Index].R = ColorPalette[ColorPaletteIndex].R;
		Texels[Index].G = ColorPalette[ColorPaletteIndex].G;
		Texels[Index].B = ColorPalette[ColorPaletteIndex].B;
		Texels[Index].A = AlphaPalette[AlphaPaletteIndex];
	}
}

/*----------------------------------------------------------------------------
	RGTC decompression routines.
----------------------------------------------------------------------------*/

//
// BC4/BC5 (RGTC).
//
// [DataFormat] https://www.khronos.org/registry/DataFormat/specs/1.2/dataformat.1.2.html#RGTC
// [OpenGL]     https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_texture_compression_rgtc.txt
// [D3D10]      https://docs.microsoft.com/de-de/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression
//
// RGTC is not a bit-exact encoding to a specific format, so multiple versions
// for different formats are needed depending on the use case. You can use
// FRecommendDecompressedFormat to figure out a suitable format.
//
// According to spec encoding of a block with (EP0,EP1) = (-127,-128)
// is considered undefined, as it contains the ambiguity whether this
// remapping of both -127 and -128 to -1.0 should occor before comparing
// the endpoint, to pick the appropreciate mode.
//
// I decided, to decode such an ambigious blocks into a stipple pattern,
// so we have a chance to catch libraries/tools encoding into this mode. --han
//

//
// BC4.
//
inline void FDecompressBlockBC4( const BYTE Block[8], FRed Texels[16] )
{
	BYTE RedPalette[8];

	// Red Endpoints.
	const BYTE& R0 = Block[0];
	const BYTE& R1 = Block[1];

	RedPalette[0] = R0;
	RedPalette[1] = R1;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3)/7;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3)/7;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3)/7;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3)/7;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3)/7;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3)/7;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2)/5;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2)/5;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2)/5;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2)/5;
		RedPalette[6] = 0;
		RedPalette[7] = 255;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
#else
	FBitReader BitReader( &Block[2], 48 );
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[BitReader.ReadBits(3)];
#endif
}

inline void FDecompressBlockBC4( const BYTE Block[8], FRed16 Texels[16] )
{
	_WORD RedPalette[8];

	// Red Endpoints.
	const BYTE& R0 = Block[0];
	const BYTE& R1 = Block[1];

	RedPalette[0] = (R0*65535+127)/255;
	RedPalette[1] = (R1*65535+127)/255;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3)/7;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3)/7;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3)/7;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3)/7;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3)/7;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3)/7;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2)/5;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2)/5;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2)/5;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2)/5;
		RedPalette[6] = 0;
		RedPalette[7] = 65535;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
#else
	FBitReader BitReader( &Block[2], 48 );
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[BitReader.ReadBits(3)];
#endif
}

inline void FDecompressBlockBC4( const BYTE Block[8], FRed32F Texels[16] )
{
	FLOAT RedPalette[8];

	// Red Endpoints.
	const BYTE& R0 = Block[0];
	const BYTE& R1 = Block[1];

	RedPalette[0] = R0/255.0;
	RedPalette[1] = R1/255.0;

	if ( R0>R1 )
	{
		RedPalette[2] = (6.0*RedPalette[0]+1.0*RedPalette[1])/7.0;
		RedPalette[3] = (5.0*RedPalette[0]+2.0*RedPalette[1])/7.0;
		RedPalette[4] = (4.0*RedPalette[0]+3.0*RedPalette[1])/7.0;
		RedPalette[5] = (3.0*RedPalette[0]+4.0*RedPalette[1])/7.0;
		RedPalette[6] = (2.0*RedPalette[0]+5.0*RedPalette[1])/7.0;
		RedPalette[7] = (1.0*RedPalette[0]+6.0*RedPalette[1])/7.0;
	}
	else
	{
		RedPalette[2] = (4.0*RedPalette[0]+1.0*RedPalette[1])/5.0;
		RedPalette[3] = (3.0*RedPalette[0]+2.0*RedPalette[1])/5.0;
		RedPalette[4] = (2.0*RedPalette[0]+3.0*RedPalette[1])/5.0;
		RedPalette[5] = (1.0*RedPalette[0]+4.0*RedPalette[1])/5.0;
		RedPalette[6] = 0.0;
		RedPalette[7] = 1.0;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
#else
	FBitReader BitReader( &Block[2], 48 );
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[BitReader.ReadBits(3)];
#endif
}

//
// BC4_S.
//
inline void FDecompressBlockBC4_S( const BYTE Block[8], FRedS Texels[16] )
{
	// Red Endpoints.
	const SBYTE& R0 = *(SBYTE*)&Block[0];
	const SBYTE& R1 = *(SBYTE*)&Block[1];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( R0==-127 && R1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 127 : 0;
		return;
	}

	SBYTE RedPalette[8];

	RedPalette[0] = R0>-128 ? R0 : -127;
	RedPalette[1] = R1>-128 ? R1 : -127;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3+7*127)/7-127;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3+7*127)/7-127;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3+7*127)/7-127;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3+7*127)/7-127;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3+7*127)/7-127;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3+7*127)/7-127;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2+5*127)/5-127;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2+5*127)/5-127;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2+5*127)/5-127;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2+5*127)/5-127;
		RedPalette[6] = -127;
		RedPalette[7] =  127;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
#else
	FBitReader BitReader( &Block[2], 48 );
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[BitReader.ReadBits(3)];
#endif
}

inline void FDecompressBlockBC4_S( const BYTE Block[8], FRed16S Texels[16] )
{
	// Red Endpoints.
	const SBYTE& R0 = *(SBYTE*)&Block[0];
	const SBYTE& R1 = *(SBYTE*)&Block[1];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( R0==-127 && R1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 32767 : 0;
		return;
	}

	SWORD RedPalette[8];

	RedPalette[0] = R0>-128 ? ((32767*R0+63+32639)/127-257) : -32767;
	RedPalette[1] = R1>-128 ? ((32767*R1+63+32639)/127-257) : -32767;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3+7*32767)/7-32767;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[6] = -32767;
		RedPalette[7] =  32767;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
#else
	FBitReader BitReader( &Block[2], 48 );
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[BitReader.ReadBits(3)];
#endif
}

inline void FDecompressBlockBC4_S( const BYTE Block[8], FRed32F Texels[16] )
{
	// Red Endpoints.
	const SBYTE& R0 = *(SBYTE*)&Block[0];
	const SBYTE& R1 = *(SBYTE*)&Block[1];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( R0==-127 && R1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 1.0 : 0;
		return;
	}

	FLOAT RedPalette[8];

	RedPalette[0] = R0>-128 ? (R0/127.0) : -1.0;
	RedPalette[1] = R1>-128 ? (R1/127.0) : -1.0;

	if ( R0>R1 )
	{
		RedPalette[2] = (6.0*RedPalette[0]+1.0*RedPalette[1])/7.0;
		RedPalette[3] = (5.0*RedPalette[0]+2.0*RedPalette[1])/7.0;
		RedPalette[4] = (4.0*RedPalette[0]+3.0*RedPalette[1])/7.0;
		RedPalette[5] = (3.0*RedPalette[0]+4.0*RedPalette[1])/7.0;
		RedPalette[6] = (2.0*RedPalette[0]+5.0*RedPalette[1])/7.0;
		RedPalette[7] = (1.0*RedPalette[0]+6.0*RedPalette[1])/7.0;
	}
	else
	{
		RedPalette[2] = (4.0*RedPalette[0]+1.0*RedPalette[1])/5.0;
		RedPalette[3] = (3.0*RedPalette[0]+2.0*RedPalette[1])/5.0;
		RedPalette[4] = (2.0*RedPalette[0]+3.0*RedPalette[1])/5.0;
		RedPalette[5] = (1.0*RedPalette[0]+4.0*RedPalette[1])/5.0;
		RedPalette[6] = -1.0;
		RedPalette[7] =  1.0;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
#else
	FBitReader BitReader( &Block[2], 48 );
	for ( INT Index=0; Index<16; Index++ )
		Texels[Index].R = RedPalette[BitReader.ReadBits(3)];
#endif
}

//
// BC5.
//
inline void FDecompressBlockBC5( const BYTE Block[16], FRedGreen Texels[16] )
{
	// Red Endpoints.
	const BYTE& R0 = Block[0];
	const BYTE& R1 = Block[1];

	BYTE RedPalette[8];

	RedPalette[0] = R0;
	RedPalette[1] = R1;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3)/7;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3)/7;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3)/7;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3)/7;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3)/7;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3)/7;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2)/5;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2)/5;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2)/5;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2)/5;
		RedPalette[6] = 0;
		RedPalette[7] = 255;
	}

	// Green Endpoints.
	const BYTE& G0 = Block[8];
	const BYTE& G1 = Block[9];

	BYTE GreenPalette[8];

	GreenPalette[0] = G0;
	GreenPalette[1] = G1;

	if ( G0>G1 )
	{
		GreenPalette[2] = (6*GreenPalette[0]+1*GreenPalette[1]+3)/7;
		GreenPalette[3] = (5*GreenPalette[0]+2*GreenPalette[1]+3)/7;
		GreenPalette[4] = (4*GreenPalette[0]+3*GreenPalette[1]+3)/7;
		GreenPalette[5] = (3*GreenPalette[0]+4*GreenPalette[1]+3)/7;
		GreenPalette[6] = (2*GreenPalette[0]+5*GreenPalette[1]+3)/7;
		GreenPalette[7] = (1*GreenPalette[0]+6*GreenPalette[1]+3)/7;
	}
	else
	{
		GreenPalette[2] = (4*GreenPalette[0]+1*GreenPalette[1]+2)/5;
		GreenPalette[3] = (3*GreenPalette[0]+2*GreenPalette[1]+2)/5;
		GreenPalette[4] = (2*GreenPalette[0]+3*GreenPalette[1]+2)/5;
		GreenPalette[5] = (1*GreenPalette[0]+4*GreenPalette[1]+2)/5;
		GreenPalette[6] = 0;
		GreenPalette[7] = 255;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
		Texels[Index].G = GreenPalette[(((*(QWORD*)&Block[8])>>(16+3*Index))&7)];
	}
#else
	FBitReader RedBitReader( &Block[2], 48 );
	FBitReader GreenBitReader( &Block[10], 48 );
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[RedBitReader.ReadBits(3)];
		Texels[Index].G = GreenPalette[GreenBitReader.ReadBits(3)];
	}
#endif
}

inline void FDecompressBlockBC5( const BYTE Block[16], FRedGreen16 Texels[16] )
{
	// Red Endpoints.
	const BYTE& R0 = Block[0];
	const BYTE& R1 = Block[1];

	_WORD RedPalette[8];

	RedPalette[0] = (R0*65535+127)/255;
	RedPalette[1] = (R1*65535+127)/255;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3)/7;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3)/7;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3)/7;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3)/7;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3)/7;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3)/7;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2)/5;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2)/5;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2)/5;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2)/5;
		RedPalette[6] = 0;
		RedPalette[7] = 65535;
	}

	// Green Endpoints.
	const BYTE& G0 = Block[8];
	const BYTE& G1 = Block[9];

	_WORD GreenPalette[8];

	GreenPalette[0] = (G0*65535+127)/255;
	GreenPalette[1] = (G1*65535+127)/255;

	if ( G0>G1 )
	{
		GreenPalette[2] = (6*GreenPalette[0]+1*GreenPalette[1]+3)/7;
		GreenPalette[3] = (5*GreenPalette[0]+2*GreenPalette[1]+3)/7;
		GreenPalette[4] = (4*GreenPalette[0]+3*GreenPalette[1]+3)/7;
		GreenPalette[5] = (3*GreenPalette[0]+4*GreenPalette[1]+3)/7;
		GreenPalette[6] = (2*GreenPalette[0]+5*GreenPalette[1]+3)/7;
		GreenPalette[7] = (1*GreenPalette[0]+6*GreenPalette[1]+3)/7;
	}
	else
	{
		GreenPalette[2] = (4*GreenPalette[0]+1*GreenPalette[1]+2)/5;
		GreenPalette[3] = (3*GreenPalette[0]+2*GreenPalette[1]+2)/5;
		GreenPalette[4] = (2*GreenPalette[0]+3*GreenPalette[1]+2)/5;
		GreenPalette[5] = (1*GreenPalette[0]+4*GreenPalette[1]+2)/5;
		GreenPalette[6] = 0;
		GreenPalette[7] = 65535;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
		Texels[Index].G = GreenPalette[(((*(QWORD*)&Block[8])>>(16+3*Index))&7)];
	}
#else
	FBitReader RedBitReader( &Block[2], 48 );
	FBitReader GreenBitReader( &Block[10], 48 );
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[RedBitReader.ReadBits(3)];
		Texels[Index].G = GreenPalette[GreenBitReader.ReadBits(3)];
	}
#endif
}

inline void FDecompressBlockBC5( const BYTE Block[16], FRedGreen32F Texels[16] )
{
	// Red Endpoints.
	const BYTE& R0 = Block[0];
	const BYTE& R1 = Block[1];

	FLOAT RedPalette[8];

	RedPalette[0] = R0/255.0;
	RedPalette[1] = R1/255.0;

	if ( R0>R1 )
	{
		RedPalette[2] = (6.0*RedPalette[0]+1.0*RedPalette[1])/7.0;
		RedPalette[3] = (5.0*RedPalette[0]+2.0*RedPalette[1])/7.0;
		RedPalette[4] = (4.0*RedPalette[0]+3.0*RedPalette[1])/7.0;
		RedPalette[5] = (3.0*RedPalette[0]+4.0*RedPalette[1])/7.0;
		RedPalette[6] = (2.0*RedPalette[0]+5.0*RedPalette[1])/7.0;
		RedPalette[7] = (1.0*RedPalette[0]+6.0*RedPalette[1])/7.0;
	}
	else
	{
		RedPalette[2] = (4.0*RedPalette[0]+1.0*RedPalette[1])/5.0;
		RedPalette[3] = (3.0*RedPalette[0]+2.0*RedPalette[1])/5.0;
		RedPalette[4] = (2.0*RedPalette[0]+3.0*RedPalette[1])/5.0;
		RedPalette[5] = (1.0*RedPalette[0]+4.0*RedPalette[1])/5.0;
		RedPalette[6] = 0.0;
		RedPalette[7] = 1.0;
	}

	// Green Endpoints.
	const BYTE& G0 = Block[8];
	const BYTE& G1 = Block[9];

	FLOAT GreenPalette[8];

	GreenPalette[0] = G0/255.0;
	GreenPalette[1] = G1/255.0;

	if ( G0>G1 )
	{
		GreenPalette[2] = (6.0*GreenPalette[0]+1.0*GreenPalette[1])/7.0;
		GreenPalette[3] = (5.0*GreenPalette[0]+2.0*GreenPalette[1])/7.0;
		GreenPalette[4] = (4.0*GreenPalette[0]+3.0*GreenPalette[1])/7.0;
		GreenPalette[5] = (3.0*GreenPalette[0]+4.0*GreenPalette[1])/7.0;
		GreenPalette[6] = (2.0*GreenPalette[0]+5.0*GreenPalette[1])/7.0;
		GreenPalette[7] = (1.0*GreenPalette[0]+6.0*GreenPalette[1])/7.0;
	}
	else
	{
		GreenPalette[2] = (4.0*GreenPalette[0]+1.0*GreenPalette[1])/5.0;
		GreenPalette[3] = (3.0*GreenPalette[0]+2.0*GreenPalette[1])/5.0;
		GreenPalette[4] = (2.0*GreenPalette[0]+3.0*GreenPalette[1])/5.0;
		GreenPalette[5] = (1.0*GreenPalette[0]+4.0*GreenPalette[1])/5.0;
		GreenPalette[6] = 0.0;
		GreenPalette[7] = 1.0;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
		Texels[Index].G = GreenPalette[(((*(QWORD*)&Block[8])>>(16+3*Index))&7)];
	}
#else
	FBitReader RedBitReader( &Block[2], 48 );
	FBitReader GreenBitReader( &Block[10], 48 );
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[RedBitReader.ReadBits(3)];
		Texels[Index].G = GreenPalette[GreenBitReader.ReadBits(3)];
	}
#endif
}

//
// BC5_S.
//
inline void FDecompressBlockBC5_S( const BYTE Block[16], FRedGreenS Texels[16] )
{
	// Red Endpoints.
	const SBYTE& R0 = *(SBYTE*)&Block[0];
	const SBYTE& R1 = *(SBYTE*)&Block[1];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( (R0==-127 && R1==-128) )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 127 : 0;
			Texels[iTexel].G = 0;
		}
		return;
	}

	SBYTE RedPalette[8];

	RedPalette[0] = R0>-128 ? R0 : -127;
	RedPalette[1] = R1>-128 ? R1 : -127;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3+7*127)/7-127;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3+7*127)/7-127;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3+7*127)/7-127;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3+7*127)/7-127;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3+7*127)/7-127;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3+7*127)/7-127;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2+5*127)/5-127;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2+5*127)/5-127;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2+5*127)/5-127;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2+5*127)/5-127;
		RedPalette[6] = -127;
		RedPalette[7] =  127;
	}

	// Green Endpoints.
	const SBYTE& G0 = *(SBYTE*)&Block[8];
	const SBYTE& G1 = *(SBYTE*)&Block[9];

	SBYTE GreenPalette[8];

	GreenPalette[0] = G0>-128 ? G0 : -127;
	GreenPalette[1] = G1>-128 ? G1 : -127;

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( G0==-127 && G1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = 0;
			Texels[iTexel].G = (iTexel%2)^((iTexel%8)/4) ? 127 : 0;
		}
		return;
	}

	if ( G0>G1 )
	{
		GreenPalette[2] = (6*GreenPalette[0]+1*GreenPalette[1]+3+7*127)/7-127;
		GreenPalette[3] = (5*GreenPalette[0]+2*GreenPalette[1]+3+7*127)/7-127;
		GreenPalette[4] = (4*GreenPalette[0]+3*GreenPalette[1]+3+7*127)/7-127;
		GreenPalette[5] = (3*GreenPalette[0]+4*GreenPalette[1]+3+7*127)/7-127;
		GreenPalette[6] = (2*GreenPalette[0]+5*GreenPalette[1]+3+7*127)/7-127;
		GreenPalette[7] = (1*GreenPalette[0]+6*GreenPalette[1]+3+7*127)/7-127;
	}
	else
	{
		GreenPalette[2] = (4*GreenPalette[0]+1*GreenPalette[1]+2+5*127)/5-127;
		GreenPalette[3] = (3*GreenPalette[0]+2*GreenPalette[1]+2+5*127)/5-127;
		GreenPalette[4] = (2*GreenPalette[0]+3*GreenPalette[1]+2+5*127)/5-127;
		GreenPalette[5] = (1*GreenPalette[0]+4*GreenPalette[1]+2+5*127)/5-127;
		GreenPalette[6] = -127;
		GreenPalette[7] =  127;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
		Texels[Index].G = GreenPalette[(((*(QWORD*)&Block[8])>>(16+3*Index))&7)];
	}
#else
	FBitReader RedBitReader( &Block[2], 48 );
	FBitReader GreenBitReader( &Block[10], 48 );
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[RedBitReader.ReadBits(3)];
		Texels[Index].G = GreenPalette[GreenBitReader.ReadBits(3)];
	}
#endif
}

inline void FDecompressBlockBC5_S( const BYTE Block[16], FRedGreen16S Texels[16] )
{
	// Red Endpoints.
	const SBYTE& R0 = *(SBYTE*)&Block[0];
	const SBYTE& R1 = *(SBYTE*)&Block[1];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( (R0==-127 && R1==-128) )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 32767 : 0;
			Texels[iTexel].G = 0;
		}
		return;
	}

	SWORD RedPalette[8];

	RedPalette[0] = R0>-128 ? ((32767*R0+63+32639)/127-257) : -32767;
	RedPalette[1] = R1>-128 ? ((32767*R1+63+32639)/127-257) : -32767;

	if ( R0>R1 )
	{
		RedPalette[2] = (6*RedPalette[0]+1*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[3] = (5*RedPalette[0]+2*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[4] = (4*RedPalette[0]+3*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[5] = (3*RedPalette[0]+4*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[6] = (2*RedPalette[0]+5*RedPalette[1]+3+7*32767)/7-32767;
		RedPalette[7] = (1*RedPalette[0]+6*RedPalette[1]+3+7*32767)/7-32767;
	}
	else
	{
		RedPalette[2] = (4*RedPalette[0]+1*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[3] = (3*RedPalette[0]+2*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[4] = (2*RedPalette[0]+3*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[5] = (1*RedPalette[0]+4*RedPalette[1]+2+5*32767)/5-32767;
		RedPalette[6] = -32767;
		RedPalette[7] =  32767;
	}

	// Green Endpoints.
	const SBYTE& G0 = *(SBYTE*)&Block[8];
	const SBYTE& G1 = *(SBYTE*)&Block[9];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( G0==-127 && G1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = 0;
			Texels[iTexel].G = (iTexel%2)^((iTexel%8)/4) ? 32767 : 0;
		}
		return;
	}

	SWORD GreenPalette[8];

	GreenPalette[0] = G0>-128 ? ((32767*G0+63+32639)/127-257) : -32767;
	GreenPalette[1] = G1>-128 ? ((32767*G1+63+32639)/127-257) : -32767;

	if ( G0>G1 )
	{
		GreenPalette[2] = (6*GreenPalette[0]+1*GreenPalette[1]+3+7*32767)/7-32767;
		GreenPalette[3] = (5*GreenPalette[0]+2*GreenPalette[1]+3+7*32767)/7-32767;
		GreenPalette[4] = (4*GreenPalette[0]+3*GreenPalette[1]+3+7*32767)/7-32767;
		GreenPalette[5] = (3*GreenPalette[0]+4*GreenPalette[1]+3+7*32767)/7-32767;
		GreenPalette[6] = (2*GreenPalette[0]+5*GreenPalette[1]+3+7*32767)/7-32767;
		GreenPalette[7] = (1*GreenPalette[0]+6*GreenPalette[1]+3+7*32767)/7-32767;
	}
	else
	{
		GreenPalette[2] = (4*GreenPalette[0]+1*GreenPalette[1]+2+5*32767)/5-32767;
		GreenPalette[3] = (3*GreenPalette[0]+2*GreenPalette[1]+2+5*32767)/5-32767;
		GreenPalette[4] = (2*GreenPalette[0]+3*GreenPalette[1]+2+5*32767)/5-32767;
		GreenPalette[5] = (1*GreenPalette[0]+4*GreenPalette[1]+2+5*32767)/5-32767;
		GreenPalette[6] = -32767;
		GreenPalette[7] =  32767;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
		Texels[Index].G = GreenPalette[(((*(QWORD*)&Block[8])>>(16+3*Index))&7)];
	}
#else
	FBitReader RedBitReader( &Block[2], 48 );
	FBitReader GreenBitReader( &Block[10], 48 );
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[RedBitReader.ReadBits(3)];
		Texels[Index].G = GreenPalette[GreenBitReader.ReadBits(3)];
	}
#endif
}

inline void FDecompressBlockBC5_S( const BYTE Block[16], FRedGreen32F Texels[16] )
{
	// Red Endpoints.
	const SBYTE& R0 = *(SBYTE*)&Block[0];
	const SBYTE& R1 = *(SBYTE*)&Block[1];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( R0==-127 && R1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 1.0 : 0;
		return;
	}

	FLOAT RedPalette[8];

	RedPalette[0] = R0>-128 ? (R0/127.0) : -1.0;
	RedPalette[1] = R1>-128 ? (R1/127.0) : -1.0;

	if ( R0>R1 )
	{
		RedPalette[2] = (6.0*RedPalette[0]+1.0*RedPalette[1])/7.0;
		RedPalette[3] = (5.0*RedPalette[0]+2.0*RedPalette[1])/7.0;
		RedPalette[4] = (4.0*RedPalette[0]+3.0*RedPalette[1])/7.0;
		RedPalette[5] = (3.0*RedPalette[0]+4.0*RedPalette[1])/7.0;
		RedPalette[6] = (2.0*RedPalette[0]+5.0*RedPalette[1])/7.0;
		RedPalette[7] = (1.0*RedPalette[0]+6.0*RedPalette[1])/7.0;
	}
	else
	{
		RedPalette[2] = (4.0*RedPalette[0]+1.0*RedPalette[1])/5.0;
		RedPalette[3] = (3.0*RedPalette[0]+2.0*RedPalette[1])/5.0;
		RedPalette[4] = (2.0*RedPalette[0]+3.0*RedPalette[1])/5.0;
		RedPalette[5] = (1.0*RedPalette[0]+4.0*RedPalette[1])/5.0;
		RedPalette[6] = -1.0;
		RedPalette[7] =  1.0;
	}

	// Green Endpoints.
	const SBYTE& G0 = *(SBYTE*)&Block[8];
	const SBYTE& G1 = *(SBYTE*)&Block[9];

	// See remark about undefined endpoint combination at the start of RGTC block.
	if ( G0==-127 && G1==-128 )
	{
		// Stipple.
		for ( INT iTexel=0; iTexel<16; iTexel++ )
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 1.0 : 0;
		return;
	}

	FLOAT GreenPalette[8];

	GreenPalette[0] = G0>-128 ? (G0/127.0) : -1.0;
	GreenPalette[1] = G1>-128 ? (G1/127.0) : -1.0;

	if ( G0>G1 )
	{
		GreenPalette[2] = (6.0*GreenPalette[0]+1.0*GreenPalette[1])/7.0;
		GreenPalette[3] = (5.0*GreenPalette[0]+2.0*GreenPalette[1])/7.0;
		GreenPalette[4] = (4.0*GreenPalette[0]+3.0*GreenPalette[1])/7.0;
		GreenPalette[5] = (3.0*GreenPalette[0]+4.0*GreenPalette[1])/7.0;
		GreenPalette[6] = (2.0*GreenPalette[0]+5.0*GreenPalette[1])/7.0;
		GreenPalette[7] = (1.0*GreenPalette[0]+6.0*GreenPalette[1])/7.0;
	}
	else
	{
		GreenPalette[2] = (4.0*GreenPalette[0]+1.0*GreenPalette[1])/5.0;
		GreenPalette[3] = (3.0*GreenPalette[0]+2.0*GreenPalette[1])/5.0;
		GreenPalette[4] = (2.0*GreenPalette[0]+3.0*GreenPalette[1])/5.0;
		GreenPalette[5] = (1.0*GreenPalette[0]+4.0*GreenPalette[1])/5.0;
		GreenPalette[6] = -1.0;
		GreenPalette[7] =  1.0;
	}

#if __INTEL_BYTE_ORDER__
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[(((*(QWORD*)&Block[0])>>(16+3*Index))&7)];
		Texels[Index].G = GreenPalette[(((*(QWORD*)&Block[8])>>(16+3*Index))&7)];
	}
#else
	FBitReader RedBitReader( &Block[2], 48 );
	FBitReader GreenBitReader( &Block[10], 48 );
	for ( INT Index=0; Index<16; Index++ )
	{
		Texels[Index].R = RedPalette[RedBitReader.ReadBits(3)];
		Texels[Index].G = GreenPalette[GreenBitReader.ReadBits(3)];
	}
#endif
}

/*----------------------------------------------------------------------------
	BPTC decompression routines.
----------------------------------------------------------------------------*/

//
// BC6H/BC7 (BPTC).
//
// [DataFormat] https://www.khronos.org/registry/DataFormat/specs/1.2/dataformat.1.2.html#BPTC
// [OpenGL]     https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_texture_compression_bptc.txt
// [D3D11]      https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc7-format
// [D3D11Mode]  https://docs.microsoft.com/en-us/windows/win32/direct3d11/bc7-format-mode-reference
//
// BC6H is a bit-exact encoding for RGB16F, but this has an unfortunate alignment, so RGBA16F is also supported.
// BC7 is a bit-exact encoding for RGBA8, but this has a fortunate alignment, so only RGBA8 is supported.
//
// !! TODO !!
//inline void FDecompressBlockBC6H( BYTE Block[16], QWORD Texels[16] ) {}
//inline void FDecompressBlockBC6H_S( BYTE Block[16], QWORD Texels[16] ) {}
//
#if 0
	#define guardBC7                    guard
	#define unguardBC7                  unguard
	#define unguardBC7f                 unguardf
	#define checkBC7                    check
	#define checkBC7Bits(mode,expected) check(Mode!=mode||(BitReader.GetPosBits()==expected&&!BitReader.IsError()));
	#define guardBC7Bits(s)             guard(s)
	#define unguardBC7Bits              unguardf(( TEXT("(PosBits=%i,Error=%i)"), BitReader.GetPosBits(), BitReader.IsError() ));
#else
	#define guardBC7(label)
	#define unguardBC7
	#define unguardBC7f(format)
	#define checkBC7(cond)
	#define checkBC7Bits(mode,expected)
	#define guardBC7Bits(s)
	#define unguardBC7Bits
#endif

// !! FIX-ME !! Certainly has a bug in mode 5 and/or single partition subsets in general !!
inline void FDecompressBlockBC7( const BYTE Block[16], FColor Texels[16] )
{
	guard(FDecompressBlockBC7);
	FBitReader BitReader( const_cast<BYTE*>(Block), 128 );
	INT Mode=0;
	while ( !BitReader.ReadBit() && Mode<8 )
		Mode++;

	// Reserved Mode. [DataFormat] recommends hardware decoders to return 0 for all texels.
	if ( Mode==8 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel] = FColor(0,0,0,0);
		}
		return;
	}

	const INT ModePartitionSubsets   = 0;
	const INT ModePartitionBits      = 1;
	const INT ModeRotationBits       = 2;
	const INT ModeIndexSelectionBits = 3;
	const INT ModeColorBits          = 4;
	const INT ModeAlphaBits          = 5;
	const INT ModeEndpointPBits      = 6;
	const INT ModeSharedPBits        = 7;
	const INT ModePrimaryIndexBits   = 8;
	const INT ModeSecondaryIndexBits = 9;
	const INT ModeColorEndpoints     = 10;
	const INT ModeAlphaEndpoints     = 11;
	const INT ModeColorLeftShift     = 12;
	const INT ModeColorRightShift    = 13;
	const INT ModeColorPBitLeftShift = 14;
	const INT ModeAlphaLeftShift     = 15;
	const INT ModeAlphaRightShift    = 16;
	const INT ModeAlphaPBitLeftShift = 17;

	const BYTE ModeInfo[8][18] =
	{
		{ 3, 4, 0, 0, 4, 0, 1, 0, 3, 0, 6, 0, 4, 1, 3,-0,-0,-0 },
		{ 2, 6, 0, 0, 6, 0, 0, 1, 3, 0, 4, 0, 2, 5, 1,-0,-0,-0 },
		{ 3, 6, 0, 0, 5, 0, 0, 0, 2, 0, 6, 0, 3, 2,-0,-0,-0,-0 },
		{ 2, 6, 0, 0, 7, 0, 1, 0, 2, 0, 4, 0, 1, 7, 0,-0,-0,-0 },
		{ 1, 0, 2, 1, 5, 6, 0, 0, 2, 3, 2, 2, 3, 2,-0, 2, 4,-0 },
		{ 1, 0, 2, 0, 7, 8, 0, 0, 2, 2, 2, 2, 1, 6,-0, 0, 8,-0 },
		{ 1, 0, 0, 0, 7, 7, 1, 0, 4, 0, 2, 2, 1, 7, 0, 1, 7, 0 },
		{ 2, 6, 0, 0, 5, 5, 1, 0, 2, 0, 4, 4, 3, 3, 2, 3, 3, 2 } 
	};

	guardBC7Bits(TestModeBits);
	checkBC7Bits(0,1);
	checkBC7Bits(1,2);
	checkBC7Bits(2,3);
	checkBC7Bits(3,4);
	checkBC7Bits(4,5);
	checkBC7Bits(5,6);
	checkBC7Bits(6,7);
	checkBC7Bits(7,8);
	unguardBC7Bits;

	INT Partition          = BitReader.ReadBits( ModeInfo[Mode][ModePartitionBits] );
	INT Rotation           = BitReader.ReadBits( ModeInfo[Mode][ModeRotationBits] );
	INT IndexSelection     = ModeInfo[Mode][ModeIndexSelectionBits]>0 ? BitReader.ReadBits(ModeInfo[Mode][ModeIndexSelectionBits]) : 0;

	guardBC7Bits(TestPartRotIndBits);
	checkBC7Bits(0,5);
	checkBC7Bits(1,8);
	checkBC7Bits(2,9);
	checkBC7Bits(3,10);
	checkBC7Bits(4,8);
	checkBC7Bits(5,8);
	checkBC7Bits(6,7);
	checkBC7Bits(7,14);
	unguardBC7Bits;

	INT PartitionSubsets   = ModeInfo[Mode][ModePartitionSubsets];
	INT ColorBits          = ModeInfo[Mode][ModeColorBits];
	INT AlphaBits          = ModeInfo[Mode][ModeAlphaBits];
	INT EndpointPBits      = ModeInfo[Mode][ModeEndpointPBits];
	INT SharedPBits        = ModeInfo[Mode][ModeSharedPBits];
	INT PrimaryIndexBits   = ModeInfo[Mode][ModePrimaryIndexBits];
	INT SecondaryIndexBits = ModeInfo[Mode][ModeSecondaryIndexBits];
	INT ColorEndpoints     = ModeInfo[Mode][ModeColorEndpoints];
	INT AlphaEndpoints     = ModeInfo[Mode][ModeAlphaEndpoints];
	INT ColorLeftShift     = ModeInfo[Mode][ModeColorLeftShift];
	INT ColorRightShift    = ModeInfo[Mode][ModeColorRightShift];
	INT ColorPBitLeftShift = ModeInfo[Mode][ModeColorPBitLeftShift];
	INT AlphaLeftShift     = ModeInfo[Mode][ModeAlphaLeftShift];
	INT AlphaRightShift    = ModeInfo[Mode][ModeAlphaRightShift];
	INT AlphaPBitLeftShift = ModeInfo[Mode][ModeAlphaPBitLeftShift]; // This turns out to be the same as ColorPBitLeftShift in all relevant cases.

	guardBC7(EndpointColorBits);
	checkBC7((ColorBits+ColorLeftShift)==8);
	checkBC7((ColorBits+Max<INT>(0,ColorBits-ColorRightShift)+EndpointPBits+SharedPBits)==8);
	if ( EndpointPBits || SharedPBits )
	{
		checkBC7(ColorPBitLeftShift==(ColorLeftShift-1));
	}
	unguardBC7f(( TEXT("(Mode=%i,ColorBits=%i,ColorLeftShift=%i,ColorRightShift=%i,EndpointPBits=%i,SharedPBits=%i)"), Mode, ColorBits, ColorLeftShift, ColorRightShift, EndpointPBits, SharedPBits ));

	guardBC7(EndpointAlphaBits);
	if ( AlphaBits>0 )
	{
		checkBC7((AlphaBits+AlphaLeftShift)==8);
		checkBC7((AlphaBits+Max<INT>(0,AlphaBits-AlphaRightShift)+EndpointPBits+SharedPBits)==8);
		if ( EndpointPBits || SharedPBits )
		{
			checkBC7(AlphaPBitLeftShift==(AlphaLeftShift-1));
		}
	}
	unguardBC7f(( TEXT("(Mode=%i,AlphaBits=%i,AlphaLeftShift=%i,AlphaRightShift=%i,EndpointPBits=%i,SharedPBits=%i)"), Mode, AlphaBits, AlphaLeftShift, AlphaRightShift, EndpointPBits, SharedPBits ));

	// ColorBits>0 for all modes. Two endpoints per subset.
	BYTE Reds[6], Greens[6], Blues[6], Alphas[6];
	for ( INT iR=0; iR<ColorEndpoints; iR++ ) { BYTE R = BitReader.ReadBits(ColorBits); Reds  [iR] = (R<<ColorLeftShift)|(R>>ColorRightShift); }
	for ( INT iG=0; iG<ColorEndpoints; iG++ ) { BYTE G = BitReader.ReadBits(ColorBits); Greens[iG] = (G<<ColorLeftShift)|(G>>ColorRightShift); }
	for ( INT iB=0; iB<ColorEndpoints; iB++ ) { BYTE B = BitReader.ReadBits(ColorBits); Blues [iB] = (B<<ColorLeftShift)|(B>>ColorRightShift); }
	for ( INT iA=0; iA<AlphaEndpoints; iA++ ) { BYTE A = BitReader.ReadBits(AlphaBits); Alphas[iA] = (A<<AlphaLeftShift)|(A>>AlphaRightShift); }

	guardBC7Bits(TestEndpointBits);
	checkBC7Bits(0,77);
	checkBC7Bits(1,80);
	checkBC7Bits(2,99);
	checkBC7Bits(3,94);
	checkBC7Bits(4,50);
	checkBC7Bits(5,66);
	checkBC7Bits(6,63);
	checkBC7Bits(7,94);
	unguardBC7Bits;

	// P-bit per endpoint.
	if ( EndpointPBits )
	{
		for ( INT iEndpointPBit=0; iEndpointPBit<2*PartitionSubsets; iEndpointPBit++ )
		{
			DWORD EndpointPBit = BitReader.ReadBit();

			DWORD ColorPBit    = EndpointPBit<<ColorPBitLeftShift; // = ColorShiftHigh-1
			DWORD AlphaPbit    = AlphaBits>0 ? (EndpointPBit<<AlphaPBitLeftShift) : 0; // Could skip this check, would only write into unused data. --han

			Reds  [iEndpointPBit] |= ColorPBit;
			Greens[iEndpointPBit] |= ColorPBit;
			Blues [iEndpointPBit] |= ColorPBit;
			Alphas[iEndpointPBit] |= AlphaPbit;
		}
	}

	// P-bit per subset.
	else if ( SharedPBits )
	{
		for ( INT iSharedPBit=0; iSharedPBit<PartitionSubsets; iSharedPBit++ )
		{
			DWORD SharedPBit = BitReader.ReadBit();

			DWORD ColorPBit  = SharedPBit<<ColorPBitLeftShift;
			DWORD AlphaPbit  = AlphaBits>0 ? (SharedPBit<<AlphaPBitLeftShift) : 0; // Could skip this check, would only write into unused data. --han

			Reds  [2*iSharedPBit  ] |= ColorPBit;
			Reds  [2*iSharedPBit+1] |= ColorPBit;
			Greens[2*iSharedPBit  ] |= ColorPBit;
			Greens[2*iSharedPBit+1] |= ColorPBit;
			Blues [2*iSharedPBit  ] |= ColorPBit;
			Blues [2*iSharedPBit+1] |= ColorPBit;
			Alphas[2*iSharedPBit  ] |= AlphaPbit;
			Alphas[2*iSharedPBit+1] |= AlphaPbit;
		}
	}

	guardBC7Bits(TestPartRotIndBits);
	checkBC7Bits(0,83);
	checkBC7Bits(1,82);
	checkBC7Bits(2,99);
	checkBC7Bits(3,98);
	checkBC7Bits(4,50);
	checkBC7Bits(5,66);
	checkBC7Bits(6,65);
	checkBC7Bits(7,98);
	unguardBC7Bits;

	// ModePrimaryIndexBits>0 for all modes. !! Apply the bit safe tricks !! Actually called Anchor Index values.
	BYTE PrimaryIndices[16], SecondaryIndices[16];

	const BYTE TwoSubsetsAnchorIndices[64] =
	{
		15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  2,  8,  2,  2,  8,  8, 15,  2,  8,  2,  2,  8,  8,  2,  2, 
		15, 15,  6,  8,  2,  8, 15, 15,  2,  8,  2,  2,  2, 15, 15,  6,  6,  2,  6,  8, 15, 15,  2,  2, 15, 15, 15, 15, 15,  2,  2, 15
	};

	const BYTE ThreeSubsetsAnchorIndices[2][64] =
	{
		{
			 3,  3, 15, 15,  8,  3, 15, 15,  8,  8,  6,  6,  6,  5,  3,  3,  3,  3,  8, 15,  3,  3,  6, 10,  5,  8,  8,  6,  8,  5, 15, 15,
			 8, 15,  3,  5,  6, 10,  8, 15, 15,  3, 15,  5, 15, 15, 15, 15,  3, 15,  5,  5,  5,  8,  5, 10,  5, 10,  8, 13, 15, 12,  3,  3
		},
		{
			15,  8,  8,  3, 15, 15,  3,  8, 15, 15, 15, 15, 15, 15, 15,  8, 15,  8, 15,  3, 15,  8, 15,  8,  3, 15,  6, 10, 15, 15, 10,  8,
			15,  3, 15, 10, 10,  8,  9, 10,  6, 15,  8, 15,  3,  6,  6,  8, 15,  3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  3, 15, 15,  8
		}
	};

	INT iPrimaryIndex=0, iSecondaryIndex=0;

	if ( PartitionSubsets==1 )
	{
		guardBC7(SinglePartionSubset)
		checkBC7(Mode==4||Mode==5||Mode==6);

		PrimaryIndices[iPrimaryIndex++] = BitReader.ReadBits( PrimaryIndexBits-1 );
		for ( (void)iPrimaryIndex; iPrimaryIndex<16; iPrimaryIndex++ )
			PrimaryIndices[iPrimaryIndex] = BitReader.ReadBits( PrimaryIndexBits );

		// Secondary indices only occur for a single subset.
		if ( SecondaryIndexBits>0 )
		{
			checkBC7(Mode==4||Mode==5);

			SecondaryIndices[iSecondaryIndex++] = BitReader.ReadBits( SecondaryIndexBits-1 );
			for ( (void)iSecondaryIndex; iSecondaryIndex<16; iSecondaryIndex++ )
				SecondaryIndices[iSecondaryIndex] = BitReader.ReadBits( SecondaryIndexBits );

			check(iSecondaryIndex==16);
		}

		checkBC7(iPrimaryIndex==16);
		unguardBC7f(( TEXT("(iPrimaryIndex=%i,iSecondaryIndex=%i)"), iPrimaryIndex, iSecondaryIndex ));
	}
	else if ( PartitionSubsets==2 )
	{
		guardBC7(TwoPartionSubsets)
		checkBC7(Mode==1||Mode==3||Mode==7);

		const INT AnchorIndex = TwoSubsetsAnchorIndices[Partition];

		PrimaryIndices[iPrimaryIndex++] = BitReader.ReadBits( PrimaryIndexBits-1 );
		for ( (void)iPrimaryIndex; iPrimaryIndex<AnchorIndex; iPrimaryIndex++ )
			PrimaryIndices[iPrimaryIndex] = BitReader.ReadBits( PrimaryIndexBits );

		PrimaryIndices[iPrimaryIndex++] = BitReader.ReadBits( PrimaryIndexBits-1 );
		for ( (void)iPrimaryIndex; iPrimaryIndex<16; iPrimaryIndex++ )
			PrimaryIndices[iPrimaryIndex] = BitReader.ReadBits( PrimaryIndexBits );

		checkBC7(iPrimaryIndex==16);
		unguardBC7f(( TEXT("(iPrimaryIndex=%i)"), iPrimaryIndex ));
	}
	else
	{
		guardBC7(ThreePartionSubsets)
		checkBC7(Mode==0||Mode==2);

		INT AnchorIndices[2] = { ThreeSubsetsAnchorIndices[0][Partition], ThreeSubsetsAnchorIndices[1][Partition] };

		if ( AnchorIndices[0]>AnchorIndices[1] )
		{
			Exchange<INT>( AnchorIndices[0], AnchorIndices[1] );
		}

		PrimaryIndices[iPrimaryIndex++] = BitReader.ReadBits( PrimaryIndexBits-1 );
		for ( (void)iPrimaryIndex; iPrimaryIndex<AnchorIndices[0]; iPrimaryIndex++ )
			PrimaryIndices[iPrimaryIndex] = BitReader.ReadBits( PrimaryIndexBits );

		PrimaryIndices[iPrimaryIndex++] = BitReader.ReadBits( PrimaryIndexBits-1 );
		for ( (void)iPrimaryIndex; iPrimaryIndex<AnchorIndices[1]; iPrimaryIndex++ )
			PrimaryIndices[iPrimaryIndex] = BitReader.ReadBits( PrimaryIndexBits );

		PrimaryIndices[iPrimaryIndex++] = BitReader.ReadBits( PrimaryIndexBits-1 );
		for ( (void)iPrimaryIndex; iPrimaryIndex<16; iPrimaryIndex++ )
			PrimaryIndices[iPrimaryIndex] = BitReader.ReadBits( PrimaryIndexBits );

		checkBC7(iPrimaryIndex==16);
		unguardBC7f(( TEXT("(iPrimaryIndex=%i)"), iPrimaryIndex ));
	}

	guardBC7Bits(TestAllTheBits);
	checkBC7Bits(0,128);
	checkBC7Bits(1,128);
	checkBC7Bits(2,128);
	checkBC7Bits(3,128);
	checkBC7Bits(4,128);
	checkBC7Bits(5,128);
	checkBC7Bits(6,128);
	checkBC7Bits(7,128);
	unguardBC7Bits;

	const BYTE SubsetPartitions[2][64][16] =
	{
		{
			{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 }, { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
			{ 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 }, { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1 }, { 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 },
			{ 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1 }, { 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1 },
			{ 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
			{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
			{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1 }, { 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 }, { 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
			{ 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0 }, { 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1 },
			{ 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0 },
			{ 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0 }, { 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0 },
			{ 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0 }, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
			{ 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0 }, { 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0 },
			{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }, { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1 },
			{ 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0 }, { 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0 },
			{ 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 }, { 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0 },
			{ 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1 }, { 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1 },
			{ 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0 }, { 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0 },
			{ 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0 },
			{ 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 }, { 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1 },
			{ 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1 }, { 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
			{ 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0 }, { 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0 },
			{ 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1 }, { 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1 },
			{ 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0 }, { 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0 },
			{ 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1 }, { 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1 },
			{ 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 }, { 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1 },
			{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 }, { 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
			{ 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 }, { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1 }
		},
		{
			{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 1, 2, 2, 2, 2 }, { 0, 0, 0, 1, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 2, 1 },
			{ 0, 0, 0, 0, 2, 0, 0, 1, 2, 2, 1, 1, 2, 2, 1, 1 }, { 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 1, 1, 0, 1, 1, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2 }, { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0, 2, 2 },
			{ 0, 0, 2, 2, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, { 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 }, { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2 },
			{ 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 }, { 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2 },
			{ 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2 }, { 0, 1, 2, 2, 0, 1, 2, 2, 0, 1, 2, 2, 0, 1, 2, 2 },
			{ 0, 0, 1, 1, 0, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 2 }, { 0, 0, 1, 1, 2, 0, 0, 1, 2, 2, 0, 0, 2, 2, 2, 0 },
			{ 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 2, 1, 1, 2, 2 }, { 0, 1, 1, 1, 0, 0, 1, 1, 2, 0, 0, 1, 2, 2, 0, 0 },
			{ 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2 }, { 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2, 1, 1, 1, 1 },
			{ 0, 1, 1, 1, 0, 1, 1, 1, 0, 2, 2, 2, 0, 2, 2, 2 }, { 0, 0, 0, 1, 0, 0, 0, 1, 2, 2, 2, 1, 2, 2, 2, 1 },
			{ 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 2, 2, 0, 1, 2, 2 }, { 0, 0, 0, 0, 1, 1, 0, 0, 2, 2, 1, 0, 2, 2, 1, 0 },
			{ 0, 1, 2, 2, 0, 1, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 1, 2, 0, 0, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2 },
			{ 0, 1, 1, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 1, 1, 0 }, { 0, 0, 0, 0, 0, 1, 1, 0, 1, 2, 2, 1, 1, 2, 2, 1 },
			{ 0, 0, 2, 2, 1, 1, 0, 2, 1, 1, 0, 2, 0, 0, 2, 2 }, { 0, 1, 1, 0, 0, 1, 1, 0, 2, 0, 0, 2, 2, 2, 2, 2 },
			{ 0, 0, 1, 1, 0, 1, 2, 2, 0, 1, 2, 2, 0, 0, 1, 1 }, { 0, 0, 0, 0, 2, 0, 0, 0, 2, 2, 1, 1, 2, 2, 2, 1 },
			{ 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 2, 2 }, { 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 1, 2, 0, 0, 1, 1 },
			{ 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 2, 2, 0, 2, 2, 2 }, { 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0 },
			{ 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0 }, { 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 },
			{ 0, 1, 2, 0, 2, 0, 1, 2, 1, 2, 0, 1, 0, 1, 2, 0 }, { 0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2, 0, 0, 1, 1 },
			{ 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0, 1, 1 }, { 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2 },
			{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1 }, { 0, 0, 2, 2, 1, 1, 2, 2, 0, 0, 2, 2, 1, 1, 2, 2 },
			{ 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0, 1, 1 }, { 0, 2, 2, 0, 1, 2, 2, 1, 0, 2, 2, 0, 1, 2, 2, 1 },
			{ 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 0, 1 }, { 0, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 },
			{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2 }, { 0, 2, 2, 2, 0, 1, 1, 1, 0, 2, 2, 2, 0, 1, 1, 1 },
			{ 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 2, 1, 1, 1, 2 }, { 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2 },
			{ 0, 2, 2, 2, 0, 1, 1, 1, 0, 1, 1, 1, 0, 2, 2, 2 }, { 0, 0, 0, 2, 1, 1, 1, 2, 1, 1, 1, 2, 0, 0, 0, 2 },
			{ 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2 }, { 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 1, 2 },
			{ 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2 }, { 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2 },
			{ 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 2, 2 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2 },
			{ 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1 }, { 0, 2, 2, 2, 1, 2, 2, 2, 0, 2, 2, 2, 1, 2, 2, 2 },
			{ 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }, { 0, 1, 1, 1, 2, 0, 1, 1, 2, 2, 0, 1, 2, 2, 2, 0 }
		}
	};

	const INT InterpolationFactors[3][16] =
	{
		{ 0, 21, 43, 64, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // For 2 bit indices.
		{ 0,  9, 18, 27, 37, 46, 55, 64, -1, -1, -1, -1, -1, -1, -1, -1 }, // For 3 bit indices.
		{ 0,  4,  9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64 }  // For 4 bit indices.
	};

	for ( INT iTexel=0; iTexel<16; iTexel++ )
	{
		FColor Color;

		INT PixelSubset = PartitionSubsets>1 ? SubsetPartitions[PartitionSubsets-2][Partition][iTexel] : 0;

		INT EndpointIndices[2] = { 2*PixelSubset, 2*PixelSubset+1 };

		INT PrimaryWeight   = InterpolationFactors[PrimaryIndexBits-2][PrimaryIndices[iTexel]];
		INT SecondaryWeight = SecondaryIndexBits>0 ? InterpolationFactors[SecondaryIndexBits-2][SecondaryIndices[iTexel]] : 0;

		INT ColorWeight = !IndexSelection ? PrimaryWeight : SecondaryWeight;
		INT AlphaWeight = (SecondaryIndexBits==0 || IndexSelection) ? PrimaryWeight : SecondaryWeight;

		Color.R = ((64-ColorWeight)*Reds  [EndpointIndices[0]] + ColorWeight*Reds  [EndpointIndices[1]] + 32) >> 6;
		Color.G = ((64-ColorWeight)*Greens[EndpointIndices[0]] + ColorWeight*Greens[EndpointIndices[1]] + 32) >> 6;
		Color.B = ((64-ColorWeight)*Blues [EndpointIndices[0]] + ColorWeight*Blues [EndpointIndices[1]] + 32) >> 6;
		Color.A = AlphaBits>0 ? (((64-AlphaWeight)*Alphas[EndpointIndices[0]] + AlphaWeight*Alphas[EndpointIndices[1]] + 32) >> 6) : 255;

		switch ( Rotation )
		{
			case 1: Exchange(Color.A,Color.R); break;
			case 2: Exchange(Color.A,Color.G); break;
			case 3: Exchange(Color.A,Color.B); break;
		}

		Texels[iTexel] = Color;
	}

	unguard;
}

/*----------------------------------------------------------------------------
	ETC1/ETC2/EAC decompression routines.
----------------------------------------------------------------------------*/

//
// ETC1/ETC2/EAC.
//
// [DataFormat] https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.html#ETC1
//
// DataFormat spec is written as big endian format, which initially caused me some confusion. So be warned. --han
//
// ETC1 is a bit-exact encoding for RGB8, but a this has an unfortunate alignment, we just support RGBA8 instead.
// ETC2(/ETC2_PA?)/ETC2_RGB_EAC_A is a bit exact encoding for RGBA8.
// The EAC formats are a bit exact encoding for 11 bit values, but also predefine a way for 16 bit expansion.
// Only int16 and fp32 will be supplied, however 11 bit versions can be derived by omiting the bit expansion
// in the int16 versions.
//

// ETC1 shared between modes.
#define ETC1_EXTRACT_PIXEL_INDEX_MSBS(block)               (256*block[4]+block[5])
#define ETC1_EXTRACT_PIXEL_INDEX_LSBS(block)               (256*block[6]+block[7])
#define ETC1_PIXEL_MSB(msbs,index)                         ((msbs>>(index))&1)
#define ETC1_PIXEL_LSB(lsbs,index)                         ((lsbs>>(index))&1)
#define ETC1_PIXEL_INDEX(msbs,lsbs,index)                  ((ETC1_PIXEL_MSB(msbs,index)<<1)|ETC1_PIXEL_LSB(lsbs,index))
#define ETC1_EXTRACT_FLIP_BIT(block)                       ((block[3]>>0)&1)
#define ETC1_EXTRACT_DIFF_BIT(block)                       ((block[3]>>1)&1)
//#define ETC1_EXTRACT_TABLE_CODEWORD(block,sub_block)       ((block[3]>>(2+3*(1-sub_block)))&7)
#define ETC1_EXTRACT_TABLE_CODEWORD(block,sub_block)       DWORD((block[3]>>(2+3*(1-sub_block)))&7)

// ETC1 differential mode.
#define ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_R(block)      ((block[0]>>3)&31)
#define ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_G(block)      ((block[1]>>3)&31)
#define ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_B(block)      ((block[2]>>3)&31)
#define ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_R(block)     ((block[0]>>0)&7)
#define ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_G(block)     ((block[1]>>0)&7)
#define ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_B(block)     ((block[2]>>0)&7)

// ETC1 individual mode.
#define ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_R(block)  ((block[0]>>4)&15)
#define ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_G(block)  ((block[1]>>4)&15)
#define ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_B(block)  ((block[2]>>4)&15)
#define ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_R(block) ((block[0]>>0)&15)
#define ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_G(block) ((block[1]>>0)&15)
#define ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_B(block) ((block[2]>>0)&15)

// ETC1 bit expansion.
#define ETC1_EXTEND_SIGN_BIT_3_TO_5(bits)                  ((((bits)&4)<<2)|(((bits)&4)<<1)|(bits))
#define ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(bits)             (((bits)<<4)|((bits)<<0))
#define ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(bits)             (((bits)<<3)|((bits)>>2))

// ETC2 T mode.
#define ETC2_EXTRACT_T_FIRST_BASE_COLOR_R(block)           (((block[0]>>1)&12)|(block[0]&3))
#define ETC2_EXTRACT_T_FIRST_BASE_COLOR_G(block)           ((block[1]>>4)&15)
#define ETC2_EXTRACT_T_FIRST_BASE_COLOR_B(block)           ((block[1]>>0)&15)
#define ETC2_EXTRACT_T_SECOND_BASE_COLOR_R(block)          ((block[2]>>4)&15)
#define ETC2_EXTRACT_T_SECOND_BASE_COLOR_G(block)          ((block[2]>>0)&15)
#define ETC2_EXTRACT_T_SECOND_BASE_COLOR_B(block)          ((block[3]>>4)&15)
#define ETC2_EXTRACT_T_DISTANCE(block)                     (((block[3]>>1)&6)|((block[3]>>0)&1))

// ETC2 H mode.
#define ETC2_EXTRACT_H_FIRST_BASE_COLOR_R(block)           ((block[0]>>3)&15)
#define ETC2_EXTRACT_H_FIRST_BASE_COLOR_G(block)           (((block[0]<<1)&14)|((block[1]>>4)&1))
#define ETC2_EXTRACT_H_FIRST_BASE_COLOR_B(block)           ((block[1]&8)|((block[1]<<1)&6)|((block[2]>>7)&1))
#define ETC2_EXTRACT_H_SECOND_BASE_COLOR_R(block)          ((block[2]>>3)&15)
#define ETC2_EXTRACT_H_SECOND_BASE_COLOR_G(block)          (((block[2]<<1)&14)|((block[3]>>7)&1))
#define ETC2_EXTRACT_H_SECOND_BASE_COLOR_B(block)          ((block[3]>>3)&15)
#define ETC2_EXTRACT_H_DISTANCE_A(block)                   ((block[3]>>2)&1)
#define ETC2_EXTRACT_H_DISTANCE_B(block)                   ((block[3]>>0)&1)

// ETC2 planar mode. !! There is most likely a bug in here.
#define ETC1_EXTRACT_PLANAR_BASE_COLOR_R(block)            ((block[0]>>1)&63)
#define ETC1_EXTRACT_PLANAR_BASE_COLOR_G(block)            (((block[0]<<6)&64)|((block[1]>>1)&63))
#define ETC1_EXTRACT_PLANAR_BASE_COLOR_B(block)            (((block[1]<<5)&32)|((block[2]>>0)&24)|((block[2]<<1)&6)|((block[3]>>7)&1))
#define ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_R(block)      (((block[3]>>1)&62)|((block[3]>>0)&1))
#define ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_G(block)      ((block[4]>>1)&127)
#define ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_B(block)      (((block[4]<<5)&32)|((block[5]>>3)&31))
#define ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_R(block)        (((block[5]<<3)&56)|((block[6]>>5)&7))
#define ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_G(block)        (((block[6]<<2)&124)|((block[7]>>6)&3))
#define ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_B(block)        ((block[7]>>0)&63)

// ETC2 punchthrough.
#define ETC2_PUNCHTHROUGH_EXTRACT_OPAQUE_BIT(block)        ((block[3]>>1)&1)

// ETC2 bit expansion.
#define ETC2_EXTEND_SIGN_BIT_3_TO_8(bits)                  ((((bits)&4)<<5)|(((bits)&4)<<4)|(((bits)&4)<<3)|(((bits)&4)<<2)|(((bits)&4)<<1)|(bits))
#define ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(bits)             (((bits)<<2)|((bits)>>4))
#define ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(bits)             (((bits)<<1)|((bits)>>6))

// EAC.
#define EAC_EXTRACT_BASE_CODEWORD(block)                   ((block)[0])
#define EAC_EXTRACT_MULTIPLIER(block)                      (((block)[1]>>4)&15)
#define EAC_EXTRACT_TABLE_INDEX(block)                     (((block)[1]>>0)&15)
#define EAC_EXTRACT_PIXEL_INDICES(block)                   (256*(256*(256*(256*(256*(QWORD((block)[2]))+QWORD((block)[3]))+QWORD((block)[4]))+QWORD((block)[5]))+QWORD((block)[6]))+QWORD((block)[7]))
#define EAC_PIXEL_INDEX(indices,pixel)                     (((indices)>>(QWORD(45-3*(pixel))))&7)

// EAC bit expansion.
#define EAC_EXTEND_11_TO_16_UNSIGNED_BITS(bits)            (((bits)<<5)|((bits)>>6))
#define EAC_EXTEND_11_TO_16_SIGNED_BITS(bits)              (((bits)>=0) ? (((bits)<<5)|((bits)>>5)) : (-(((-(bits))<<5) + ((-(bits))>>5))))

inline void FDecompressBlockETC1( const BYTE Block[8], FColor Texels[16] )
{
	const DWORD PixelIndexMSBs    = ETC1_EXTRACT_PIXEL_INDEX_MSBS(Block);
	const DWORD PixelIndexLSBs    = ETC1_EXTRACT_PIXEL_INDEX_LSBS(Block);
	const DWORD FlipBit           = ETC1_EXTRACT_FLIP_BIT(Block);
	const DWORD DiffBit           = ETC1_EXTRACT_DIFF_BIT(Block);
	const DWORD TableCodewords[2] = { ETC1_EXTRACT_TABLE_CODEWORD(Block,0), ETC1_EXTRACT_TABLE_CODEWORD(Block,1) };

	FColor BaseColors[2];

	// Individual mode.
	if ( DiffBit==0 )
	{
		BaseColors[0] = FColor
		(
			ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_R(Block)),
			ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_G(Block)),
			ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_B(Block)),
			255
		);
		BaseColors[1] = FColor
		(
			ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_R(Block)),
			ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_G(Block)),
			ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_B(Block)),
			255
		);
	}
	// Differential mode (and T and H and Plane mode in ETC2).
	else
	{
		BaseColors[0] = FColor
		(
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_R(Block)),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_G(Block)),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_B(Block)),
			255
		);
		BaseColors[1] = FColor
		(
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS((ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_R(Block)+ETC1_EXTEND_SIGN_BIT_3_TO_5(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_R(Block)))&31),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS((ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_G(Block)+ETC1_EXTEND_SIGN_BIT_3_TO_5(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_G(Block)))&31),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS((ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_B(Block)+ETC1_EXTEND_SIGN_BIT_3_TO_5(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_B(Block)))&31),
			255
		);
	}

	// This table is already remapped so it can be directly accessed by PixelIndex.
	const INT ModifierTable[8][4] =
	{
		{  2,   8,  -2,   -8 },
		{  5,  17,  -5,  -17 },
		{  9,  29,  -9,  -29 },
		{ 13,  42, -13,  -42 },
		{ 18,  60, -18,  -60 },
		{ 24,  80, -24,  -80 },
		{ 33, 106, -33, -106 },
		{ 47, 183, -47, -183 }
	};

	// ETC1 is column major, but we need to output row major.
	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			INT SubBlock;
			if ( i<2 && j<2 )
				SubBlock = 0;
			else if ( i>1 && j>1 )
				SubBlock = 1;
			else if ( i>1 )
				SubBlock = 1-FlipBit;
			else
				SubBlock = FlipBit;

			DWORD PixelIndex = ETC1_PIXEL_INDEX(PixelIndexMSBs,PixelIndexLSBs,j+4*i);
			INT   Modifier   = ModifierTable[TableCodewords[SubBlock]][PixelIndex];

			FColor& Result = Texels[i+4*j];
			Result.R = Clamp<INT>( INT(BaseColors[SubBlock].R)+Modifier, 0, 255 );
			Result.G = Clamp<INT>( INT(BaseColors[SubBlock].G)+Modifier, 0, 255 );
			Result.B = Clamp<INT>( INT(BaseColors[SubBlock].B)+Modifier, 0, 255 );
			Result.A = 255;
		}
	}
}

inline void FDecompressBlockETC2( const BYTE Block[8], FColor Texels[16] )
{
	const DWORD DiffBit        = ETC1_EXTRACT_DIFF_BIT(Block);
	const DWORD PixelIndexMSBs = ETC1_EXTRACT_PIXEL_INDEX_MSBS(Block);
	const DWORD PixelIndexLSBs = ETC1_EXTRACT_PIXEL_INDEX_LSBS(Block);

	const FRedGreenBlue ModeBase
	(
		ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_R(Block),
		ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_G(Block),
		ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_B(Block)
	);
	const FRedGreenBlueS ModeDelta
	(
		ETC2_EXTEND_SIGN_BIT_3_TO_8(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_R(Block)),
		ETC2_EXTEND_SIGN_BIT_3_TO_8(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_G(Block)),
		ETC2_EXTEND_SIGN_BIT_3_TO_8(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_B(Block))
	);
	const FRedGreenBlue32S ModeSum
	(
		ModeBase.R+(INT)ModeDelta.R,
		ModeBase.G+(INT)ModeDelta.G,
		ModeBase.B+(INT)ModeDelta.B
	);

	// T and H mode.
	if ( DiffBit && ((ModeSum.R&(~31))|(ModeSum.G&(~31))) )
	{
		FColor PaintColors[4];

		const BYTE DistanceTable[8] = 
		{
			3,
			6,
			11,
			16,
			23,
			32,
			41,
			64
		};

		// T mode.
		if ( DiffBit && (ModeSum.R&(~31)) )
		{
			const FColor BaseColors[2] =
			{
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_FIRST_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_FIRST_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_FIRST_BASE_COLOR_B(Block)),
					255
				),
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_SECOND_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_SECOND_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_SECOND_BASE_COLOR_B(Block)),
					255
				)
			};

			const INT Distance = DistanceTable[ETC2_EXTRACT_T_DISTANCE(Block)];

			PaintColors[0] = BaseColors[0];
			PaintColors[1] = FColor
			(
				Clamp<INT>(BaseColors[1].R+Distance,0,255),
				Clamp<INT>(BaseColors[1].G+Distance,0,255),
				Clamp<INT>(BaseColors[1].B+Distance,0,255),
				255
			);
			PaintColors[2] = BaseColors[1];
			PaintColors[3] = FColor
			(
				Clamp<INT>(BaseColors[1].R-Distance,0,255),
				Clamp<INT>(BaseColors[1].G-Distance,0,255),
				Clamp<INT>(BaseColors[1].B-Distance,0,255),
				255
			);
		}
		// H mode.
		else
		{
			const FColor BaseColors[2] =
			{
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_FIRST_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_FIRST_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_FIRST_BASE_COLOR_B(Block)),
					255
				),
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_SECOND_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_SECOND_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_SECOND_BASE_COLOR_B(Block)),
					255
				)
			};

			const INT DistanceIndex = (ETC2_EXTRACT_H_DISTANCE_A(Block)<<2)|(ETC2_EXTRACT_H_DISTANCE_B(Block)<<1)|(((BaseColors[0].R<<16)|(BaseColors[0].G<<8)|(BaseColors[0].B<<0))>=((BaseColors[1].R<<16)|(BaseColors[1].G<<8)|(BaseColors[1].B<<0)));
			const INT Distance      = DistanceTable[DistanceIndex];

			PaintColors[0] = FColor
			(
				Clamp<INT>(BaseColors[0].R+Distance,0,255),
				Clamp<INT>(BaseColors[0].G+Distance,0,255),
				Clamp<INT>(BaseColors[0].B+Distance,0,255),
				255
			);
			PaintColors[1] = FColor
			(
				Clamp<INT>(BaseColors[0].R-Distance,0,255),
				Clamp<INT>(BaseColors[0].G-Distance,0,255),
				Clamp<INT>(BaseColors[0].B-Distance,0,255),
				255
			);
			PaintColors[2] = FColor
			(
				Clamp<INT>(BaseColors[1].R+Distance,0,255),
				Clamp<INT>(BaseColors[1].G+Distance,0,255),
				Clamp<INT>(BaseColors[1].B+Distance,0,255),
				255
			);
			PaintColors[3] = FColor
			(
				Clamp<INT>(BaseColors[1].R-Distance,0,255),
				Clamp<INT>(BaseColors[1].G-Distance,0,255),
				Clamp<INT>(BaseColors[1].B-Distance,0,255),
				255
			);
		}

		for ( INT i=0; i<4; i++ )
		{
			for ( INT j=0; j<4; j++ )
			{
				DWORD PixelIndex = ETC1_PIXEL_INDEX(PixelIndexMSBs,PixelIndexLSBs,j+4*i);

				Texels[i+4*j] = PaintColors[PixelIndex];
			}
		}
	}
	// Planar mode.
	else if ( DiffBit && (ModeSum.B&(~31)) )
	{
		const FRedGreenBlue BaseColor
		(
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_BASE_COLOR_R(Block)),
			ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_BASE_COLOR_G(Block)),
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_BASE_COLOR_B(Block))
		);

		const FRedGreenBlue HorizontalColor
		(
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_R(Block)),
			ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_G(Block)),
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_B(Block))
		);

		const FRedGreenBlue VerticalColor
		(
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_R(Block)),
			ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_G(Block)),
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_B(Block))
		);

		for ( INT x=0; x<4; x++ )
		{
			for ( INT y=0; y<4; y++ )
			{
				Texels[4*y+x] = FColor
				(
					Clamp<INT>( (x*(HorizontalColor.R-BaseColor.R)+y*(VerticalColor.R-BaseColor.R)+4*BaseColor.R+2)/4, 0, 255 ),
					Clamp<INT>( (x*(HorizontalColor.G-BaseColor.G)+y*(VerticalColor.G-BaseColor.G)+4*BaseColor.G+2)/4, 0, 255 ),
					Clamp<INT>( (x*(HorizontalColor.B-BaseColor.B)+y*(VerticalColor.B-BaseColor.B)+4*BaseColor.B+2)/4, 0, 255 ),
					255
				);
			}
		}
	}

	// Individual and differential mode.
	else
	{
		const DWORD FlipBit           = ETC1_EXTRACT_FLIP_BIT(Block);
		const DWORD TableCodewords[2] = { ETC1_EXTRACT_TABLE_CODEWORD(Block,0), ETC1_EXTRACT_TABLE_CODEWORD(Block,1) };

		FColor BaseColors[2];

		// Differential mode.
		if ( DiffBit )
		{
			BaseColors[0] = FColor
			(
				ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeBase.R),
				ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeBase.G),
				ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeBase.B),
				255
			);

			// Broken.
			BaseColors[1] = FColor // Note that ModeSum is guranteed to not have bits set outside of 31.
			(
				ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeSum.R),
				ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeSum.G),
				ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeSum.B),
				255
			);
		}
		// Individual mode.
		else
		{
			BaseColors[0] = FColor
			(
				ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_R(Block)),
				ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_G(Block)),
				ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_FIRST_BASE_COLOR_B(Block)),
				255
			);
			BaseColors[1] = FColor
			(
				ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_R(Block)),
				ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_G(Block)),
				ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_INDIVIDUAL_SECOND_BASE_COLOR_B(Block)),
				255
			);
		}

		// This table is already remapped so it can be directly accessed by PixelIndex.
		const INT ModifierTable[8][4] =
		{
			{  2,   8,  -2,   -8 },
			{  5,  17,  -5,  -17 },
			{  9,  29,  -9,  -29 },
			{ 13,  42, -13,  -42 },
			{ 18,  60, -18,  -60 },
			{ 24,  80, -24,  -80 },
			{ 33, 106, -33, -106 },
			{ 47, 183, -47, -183 }
		};

		for ( INT i=0; i<4; i++ )
		{
			for ( INT j=0; j<4; j++ )
			{
				INT SubBlock;
				if ( i<2 && j<2 )
					SubBlock = 0;
				else if ( i>1 && j>1 )
					SubBlock = 1;
				else if ( i>1 )
					SubBlock = 1-FlipBit;
				else
					SubBlock = FlipBit;

				DWORD PixelIndex = ETC1_PIXEL_INDEX(PixelIndexMSBs,PixelIndexLSBs,j+4*i);
				INT   Modifier   = ModifierTable[TableCodewords[SubBlock]][PixelIndex];

				Texels[i+4*j] = FColor
				(
					Clamp<INT>( INT(BaseColors[SubBlock].R)+Modifier, 0, 255 ),
					Clamp<INT>( INT(BaseColors[SubBlock].G)+Modifier, 0, 255 ),
					Clamp<INT>( INT(BaseColors[SubBlock].B)+Modifier, 0, 255 ),
					255
				);
			}
		}
	}
}

inline void FDecompressBlockETC2_PA( const BYTE Block[8], FColor Texels[16] )
{
	const DWORD OpaqueBit      = ETC2_PUNCHTHROUGH_EXTRACT_OPAQUE_BIT(Block);
	const DWORD PixelIndexMSBs = ETC1_EXTRACT_PIXEL_INDEX_MSBS(Block);
	const DWORD PixelIndexLSBs = ETC1_EXTRACT_PIXEL_INDEX_LSBS(Block);

	const FRedGreenBlue ModeBase
	(
		ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_R(Block),
		ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_G(Block),
		ETC1_EXTRACT_DIFFERENTIAL_BASE_COLOR_B(Block)
	);
	const FRedGreenBlueS ModeDelta
	(
		ETC2_EXTEND_SIGN_BIT_3_TO_8(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_R(Block)),
		ETC2_EXTEND_SIGN_BIT_3_TO_8(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_G(Block)),
		ETC2_EXTEND_SIGN_BIT_3_TO_8(ETC1_EXTRACT_DIFFERENTIAL_COLOR_DELTA_B(Block))
	);
	const FRedGreenBlue32S ModeSum
	(
		ModeBase.R+(INT)ModeDelta.R,
		ModeBase.G+(INT)ModeDelta.G,
		ModeBase.B+(INT)ModeDelta.B
	);

	// T and H mode.
	if ( ((ModeSum.R&(~31))|(ModeSum.G&(~31))) )
	{
		FColor PaintColors[4];

		const BYTE DistanceTable[8] = 
		{
			3,
			6,
			11,
			16,
			23,
			32,
			41,
			64
		};

		// T mode.
		if ( ModeSum.R&(~31) )
		{
			const FColor BaseColors[2] =
			{
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_FIRST_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_FIRST_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_FIRST_BASE_COLOR_B(Block)),
					255
				),
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_SECOND_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_SECOND_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_T_SECOND_BASE_COLOR_B(Block)),
					255
				)
			};

			const INT Distance = DistanceTable[ETC2_EXTRACT_T_DISTANCE(Block)];

			PaintColors[0] = BaseColors[0];
			PaintColors[1] = FColor
			(
				Clamp<INT>(BaseColors[1].R+Distance,0,255),
				Clamp<INT>(BaseColors[1].G+Distance,0,255),
				Clamp<INT>(BaseColors[1].B+Distance,0,255),
				255
			);
			PaintColors[2] = BaseColors[1];
			PaintColors[3] = FColor
			(
				Clamp<INT>(BaseColors[1].R-Distance,0,255),
				Clamp<INT>(BaseColors[1].G-Distance,0,255),
				Clamp<INT>(BaseColors[1].B-Distance,0,255),
				255
			);
		}
		// H mode.
		else
		{
			const FColor BaseColors[2] =
			{
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_FIRST_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_FIRST_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_FIRST_BASE_COLOR_B(Block)),
					255
				),
				FColor
				(
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_SECOND_BASE_COLOR_R(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_SECOND_BASE_COLOR_G(Block)),
					ETC1_EXTEND_4_TO_8_UNSIGNED_BITS(ETC2_EXTRACT_H_SECOND_BASE_COLOR_B(Block)),
					255
				)
			};

			const INT DistanceIndex = (ETC2_EXTRACT_H_DISTANCE_A(Block)<<2)|(ETC2_EXTRACT_H_DISTANCE_B(Block)<<1)|(((BaseColors[0].R<<16)|(BaseColors[0].G<<8)|(BaseColors[0].B<<0))>=((BaseColors[1].R<<16)|(BaseColors[1].G<<8)|(BaseColors[1].B<<0)));
			const INT Distance      = DistanceTable[DistanceIndex];

			PaintColors[0] = FColor
			(
				Clamp<INT>(BaseColors[0].R+Distance,0,255),
				Clamp<INT>(BaseColors[0].G+Distance,0,255),
				Clamp<INT>(BaseColors[0].B+Distance,0,255),
				255
			);
			PaintColors[1] = FColor
			(
				Clamp<INT>(BaseColors[0].R-Distance,0,255),
				Clamp<INT>(BaseColors[0].G-Distance,0,255),
				Clamp<INT>(BaseColors[0].B-Distance,0,255),
				255
			);
			PaintColors[2] = FColor
			(
				Clamp<INT>(BaseColors[1].R+Distance,0,255),
				Clamp<INT>(BaseColors[1].G+Distance,0,255),
				Clamp<INT>(BaseColors[1].B+Distance,0,255),
				255
			);
			PaintColors[3] = FColor
			(
				Clamp<INT>(BaseColors[1].R-Distance,0,255),
				Clamp<INT>(BaseColors[1].G-Distance,0,255),
				Clamp<INT>(BaseColors[1].B-Distance,0,255),
				255
			);
		}

		for ( INT i=0; i<4; i++ )
		{
			for ( INT j=0; j<4; j++ )
			{
				DWORD PixelIndex = ETC1_PIXEL_INDEX(PixelIndexMSBs,PixelIndexLSBs,j+4*i);

				if ( !OpaqueBit && PixelIndex==2 )				
				{
					Texels[i+4*j] = FColor(0,0,0,0);
				}
				else
				{
					Texels[i+4*j] = PaintColors[PixelIndex];
				}
			}
		}
	}
	// Planar mode.
	else if ( ModeSum.B&(~31) )
	{
		const FRedGreenBlue BaseColor
		(
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_BASE_COLOR_R(Block)),
			ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_BASE_COLOR_G(Block)),
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_BASE_COLOR_B(Block))
		);

		const FRedGreenBlue HorizontalColor
		(
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_R(Block)),
			ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_G(Block)),
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_HORIZONTAL_COLOR_B(Block))
		);

		const FRedGreenBlue VerticalColor
		(
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_R(Block)),
			ETC2_EXTEND_7_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_G(Block)),
			ETC2_EXTEND_6_TO_8_UNSIGNED_BITS(ETC1_EXTRACT_PLANAR_VERTICAL_COLOR_B(Block))
		);

		for ( INT x=0; x<4; x++ )
		{
			for ( INT y=0; y<4; y++ )
			{
				Texels[4*y+x] = FColor
				(
					Clamp<INT>( (x*(HorizontalColor.R-BaseColor.R)+y*(VerticalColor.R-BaseColor.R)+4*BaseColor.R+2)/4, 0, 255 ),
					Clamp<INT>( (x*(HorizontalColor.G-BaseColor.G)+y*(VerticalColor.G-BaseColor.G)+4*BaseColor.G+2)/4, 0, 255 ),
					Clamp<INT>( (x*(HorizontalColor.B-BaseColor.B)+y*(VerticalColor.B-BaseColor.B)+4*BaseColor.B+2)/4, 0, 255 ),
					255
				);
			}
		}
	}
	// Differential mode.
	else
	{
		const DWORD FlipBit           = ETC1_EXTRACT_FLIP_BIT(Block);
		const DWORD TableCodewords[2] = { ETC1_EXTRACT_TABLE_CODEWORD(Block,0), ETC1_EXTRACT_TABLE_CODEWORD(Block,1) };

		FColor BaseColors[2];

		BaseColors[0] = FColor
		(
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeBase.R),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeBase.G),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeBase.B),
			255
		);

		// Broken.
		BaseColors[1] = FColor // Note that ModeSum is guranteed to not have bits set outside of 31.
		(
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeSum.R),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeSum.G),
			ETC1_EXTEND_5_TO_8_UNSIGNED_BITS(ModeSum.B),
			255
		);

		// This table is already remapped so it can be directly accessed by PixelIndex.
		const INT ModifierTable[2][8][4] =
		{
			{
				{  0,   8,   0,   -8 },
				{  0,  17,   0,  -17 },
				{  0,  29,   0,  -29 },
				{  0,  42,   0,  -42 },
				{  0,  60,   0,  -60 },
				{  0,  80,   0,  -80 },
				{  0, 106,   0, -106 },
				{  0, 183,   0, -183 }
			},
			{
				{  2,   8,  -2,   -8 },
				{  5,  17,  -5,  -17 },
				{  9,  29,  -9,  -29 },
				{ 13,  42, -13,  -42 },
				{ 18,  60, -18,  -60 },
				{ 24,  80, -24,  -80 },
				{ 33, 106, -33, -106 },
				{ 47, 183, -47, -183 }
			}
		};

		for ( INT i=0; i<4; i++ )
		{
			for ( INT j=0; j<4; j++ )
			{
				INT SubBlock;
				if ( i<2 && j<2 )
					SubBlock = 0;
				else if ( i>1 && j>1 )
					SubBlock = 1;
				else if ( i>1 )
					SubBlock = 1-FlipBit;
				else
					SubBlock = FlipBit;

				DWORD PixelIndex = ETC1_PIXEL_INDEX(PixelIndexMSBs,PixelIndexLSBs,j+4*i);

				if ( !OpaqueBit && PixelIndex==2 )				
				{
					Texels[i+4*j] = FColor(0,0,0,0);
				}
				else
				{
					INT Modifier = ModifierTable[OpaqueBit][TableCodewords[SubBlock]][PixelIndex];

					Texels[i+4*j] = FColor
					(
						Clamp<INT>( INT(BaseColors[SubBlock].R)+Modifier, 0, 255 ),
						Clamp<INT>( INT(BaseColors[SubBlock].G)+Modifier, 0, 255 ),
						Clamp<INT>( INT(BaseColors[SubBlock].B)+Modifier, 0, 255 ),
						255
					);
				}
			}
		}
	}
}

inline void FDecompressBlockETC2_RGB_EAC_A( const BYTE Block[16], FColor Texels[16] )
{
	//
	// Alpha data preceeds the seperate color data. Color data is encoded in exactly
	// the same way as for plain ETC2. Decode color first, as FDecompressBlockETC2
	// will set alpha to 255.
	//
	FDecompressBlockETC2( Block+8, Texels );

	// Decode alpha.
	const INT   BaseCodeword = EAC_EXTRACT_BASE_CODEWORD(Block);
	const INT   Multiplier   = EAC_EXTRACT_MULTIPLIER(Block);
	const DWORD TableIndex   = EAC_EXTRACT_TABLE_INDEX(Block);
	const QWORD PixelIndices = EAC_EXTRACT_PIXEL_INDICES(Block);

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD PixelIndex = EAC_PIXEL_INDEX(PixelIndices,j+4*i);
			const INT   Modifier   = ModifierTable[TableIndex][PixelIndex];

			Texels[i+4*j].A = Clamp<INT>( BaseCodeword+Modifier*Multiplier, 0, 255 );
		}
	}
}

inline void FDecompressBlockEAC_R( const BYTE Block[8], FRed16 Texels[16] )
{
	const INT   RedBaseCodeword = EAC_EXTRACT_BASE_CODEWORD(Block);
	const INT   RedMultiplier   = EAC_EXTRACT_MULTIPLIER(Block);
	const DWORD RedTableIndex   = EAC_EXTRACT_TABLE_INDEX(Block);
	const QWORD RedPixelIndices = EAC_EXTRACT_PIXEL_INDICES(Block);

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier   = ModifierTable[RedTableIndex][RedPixelIndex];

			const DWORD Red11         = Clamp<INT>( RedBaseCodeword*8+4+RedModifier*RedMultiplier*8, 0, 2047 );

			Texels[i+4*j].R = EAC_EXTEND_11_TO_16_UNSIGNED_BITS(Red11);
		}
	}
}
inline void FDecompressBlockEAC_R( const BYTE Block[8], FRed32F Texels[16] )
{
	const INT   RedBaseCodeword = EAC_EXTRACT_BASE_CODEWORD(Block);
	const INT   RedMultiplier   = EAC_EXTRACT_MULTIPLIER(Block);
	const DWORD RedTableIndex   = EAC_EXTRACT_TABLE_INDEX(Block);
	const QWORD RedPixelIndices = EAC_EXTRACT_PIXEL_INDICES(Block);

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier   = ModifierTable[RedTableIndex][RedPixelIndex];

			const DWORD Red11         = Clamp<INT>( RedBaseCodeword*8+4+RedModifier*RedMultiplier*8, 0, 2047 );

			Texels[i+4*j].R = Red11/2047.0;
		}
	}
}

inline void FDecompressBlockEAC_R_S( const BYTE Block[8], FRed16S Texels[16] )
{
	const INT   RedBaseCodeword   = (SBYTE)EAC_EXTRACT_BASE_CODEWORD(Block);
	const INT   RedMultiplier     = EAC_EXTRACT_MULTIPLIER(Block);
	const DWORD RedTableIndex     = EAC_EXTRACT_TABLE_INDEX(Block);
	const QWORD RedPixelIndices   = EAC_EXTRACT_PIXEL_INDICES(Block);

	//
	// -128 is not allowed to occur as value for the BaseCodeword, however a decoder
	// is required to map it to -127.
	//
	// It may make sense to violate the spec, and color code here, to spot encoder
	// bugs. --han
	//
#if 0
	// Conform.
	if ( RedBaseCodeword==-128 )
	{
		RedBaseCodeword = -127;
	}
#else
	// Stipple.
	if ( RedBaseCodeword==-128 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 32767 : 0;
		}
		return;
	}
#endif

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex   = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier     = ModifierTable[RedTableIndex][RedPixelIndex];

			INT Red11;
			if ( RedModifier==0 )
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier,   -1023, 1023 );
			}
			else
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier  *RedMultiplier  *8, -1023, 1023 );
			}

			Texels[i+4*j].R = EAC_EXTEND_11_TO_16_SIGNED_BITS(Red11);
		}
	}
}
inline void FDecompressBlockEAC_R_S( const BYTE Block[8], FRed32F Texels[16] )
{
	// Consists of two EAC_R_S channels. Red data preceeds green data.
	const INT   RedBaseCodeword   = (SBYTE)EAC_EXTRACT_BASE_CODEWORD(Block);
	const INT   RedMultiplier     = EAC_EXTRACT_MULTIPLIER(Block);
	const DWORD RedTableIndex     = EAC_EXTRACT_TABLE_INDEX(Block);
	const QWORD RedPixelIndices   = EAC_EXTRACT_PIXEL_INDICES(Block);

	//
	// -128 is not allowed to occur as value for the BaseCodeword, however a decoder
	// is required to map it to -127.
	//
	// It may make sense to violate the spec, and color code here, to spot encoder
	// bugs. --han
	//
#if 0
	// Conform.
	if ( RedBaseCodeword==-128 )
	{
		RedBaseCodeword = -127;
	}
#else
	// Stipple.
	if ( RedBaseCodeword==-128 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 1.0 : 0.0;
		}
		return;
	}
#endif

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex   = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier     = ModifierTable[RedTableIndex][RedPixelIndex];

			DWORD Red11;
			if ( RedModifier==0 )
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier,   -1023, 1023 );
			}
			else
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier  *RedMultiplier  *8, -1023, 1023 );
			}

			Texels[i+4*j].R = Red11/1023.0;
		}
	}
}

inline void FDecompressBlockEAC_RG( const BYTE Block[16], FRedGreen16 Texels[16] )
{
	const BYTE* RedBlock   = Block+0;
	const BYTE* GreenBlock = Block+8;

	// Consists of two EAC_R channels. Red data preceeds green data.
	const INT   RedBaseCodeword   = EAC_EXTRACT_BASE_CODEWORD(RedBlock);
	const INT   RedMultiplier     = EAC_EXTRACT_MULTIPLIER(RedBlock);
	const DWORD RedTableIndex     = EAC_EXTRACT_TABLE_INDEX(RedBlock);
	const QWORD RedPixelIndices   = EAC_EXTRACT_PIXEL_INDICES(RedBlock);

	const INT   GreenBaseCodeword = EAC_EXTRACT_BASE_CODEWORD(GreenBlock);
	const INT   GreenMultiplier   = EAC_EXTRACT_MULTIPLIER(GreenBlock);
	const DWORD GreenTableIndex   = EAC_EXTRACT_TABLE_INDEX(GreenBlock);
	const QWORD GreenPixelIndices = EAC_EXTRACT_PIXEL_INDICES(GreenBlock);

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex   = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier     = ModifierTable[RedTableIndex][RedPixelIndex];

			const DWORD GreenPixelIndex = EAC_PIXEL_INDEX(GreenPixelIndices,j+4*i);
			const INT   GreenModifier   = ModifierTable[GreenTableIndex][GreenPixelIndex];

			const DWORD Red11           = Clamp<INT>( RedBaseCodeword  *8+4+RedModifier  *RedMultiplier  *8, 0, 2047 );
			const DWORD Green11         = Clamp<INT>( GreenBaseCodeword*8+4+GreenModifier*GreenMultiplier*8, 0, 2047 );

			Texels[i+4*j].R = EAC_EXTEND_11_TO_16_UNSIGNED_BITS(Red11);
			Texels[i+4*j].G = EAC_EXTEND_11_TO_16_UNSIGNED_BITS(Green11);
		}
	}
}
inline void FDecompressBlockEAC_RG( const BYTE Block[16], FRedGreen32F Texels[16] )
{
	const BYTE* RedBlock   = Block+0;
	const BYTE* GreenBlock = Block+8;

	// Consists of two EAC_R channels. Red data preceeds green data.
	const INT   RedBaseCodeword   = EAC_EXTRACT_BASE_CODEWORD(RedBlock);
	const INT   RedMultiplier     = EAC_EXTRACT_MULTIPLIER(RedBlock);
	const DWORD RedTableIndex     = EAC_EXTRACT_TABLE_INDEX(RedBlock);
	const QWORD RedPixelIndices   = EAC_EXTRACT_PIXEL_INDICES(RedBlock);

	const INT   GreenBaseCodeword = EAC_EXTRACT_BASE_CODEWORD(GreenBlock);
	const INT   GreenMultiplier   = EAC_EXTRACT_MULTIPLIER(GreenBlock);
	const DWORD GreenTableIndex   = EAC_EXTRACT_TABLE_INDEX(GreenBlock);
	const QWORD GreenPixelIndices = EAC_EXTRACT_PIXEL_INDICES(GreenBlock);

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex   = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier     = ModifierTable[RedTableIndex][RedPixelIndex];

			const DWORD GreenPixelIndex = EAC_PIXEL_INDEX(GreenPixelIndices,j+4*i);
			const INT   GreenModifier   = ModifierTable[GreenTableIndex][GreenPixelIndex];

			const DWORD Red11           = Clamp<INT>( RedBaseCodeword  *8+4+RedModifier  *RedMultiplier  *8, 0, 2047 );
			const DWORD Green11         = Clamp<INT>( GreenBaseCodeword*8+4+GreenModifier*GreenMultiplier*8, 0, 2047 );

			Texels[i+4*j].R = Red11/2047.0;
			Texels[i+4*j].G = Green11/2047.0;
		}
	}
}

inline void FDecompressBlockEAC_RG_S( const BYTE Block[16], FRedGreen16S Texels[16] )
{
	const BYTE* RedBlock   = Block+0;
	const BYTE* GreenBlock = Block+8;

	// Consists of two EAC_R_S channels. Red data preceeds green data.
	const INT   RedBaseCodeword   = (SBYTE)EAC_EXTRACT_BASE_CODEWORD(RedBlock);
	const INT   RedMultiplier     = EAC_EXTRACT_MULTIPLIER(RedBlock);
	const DWORD RedTableIndex     = EAC_EXTRACT_TABLE_INDEX(RedBlock);
	const QWORD RedPixelIndices   = EAC_EXTRACT_PIXEL_INDICES(RedBlock);

	const INT   GreenBaseCodeword = (SBYTE)EAC_EXTRACT_BASE_CODEWORD(GreenBlock);
	const INT   GreenMultiplier   = EAC_EXTRACT_MULTIPLIER(GreenBlock);
	const DWORD GreenTableIndex   = EAC_EXTRACT_TABLE_INDEX(GreenBlock);
	const QWORD GreenPixelIndices = EAC_EXTRACT_PIXEL_INDICES(GreenBlock);

	//
	// -128 is not allowed to occur as value for the BaseCodeword, however a decoder
	// is required to map it to -127.
	//
	// It may make sense to violate the spec, and color code here, to spot encoder
	// bugs. --han
	//
#if 0
	// Conform.
	if ( RedBaseCodeword==-128 )
	{
		RedBaseCodeword = -127;
	}
	if ( GreenBaseCodeword==-128 )
	{
		GreenBaseCodeword = -127;
	}
#else
	// Stipple.
	if ( RedBaseCodeword==-128 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 32767 : 0;
			Texels[iTexel].G = 0;
		}
		return;
	}
	if ( GreenBaseCodeword==-128 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = 0;
			Texels[iTexel].G = (iTexel%2)^((iTexel%8)/4) ? 32767 : 0;
		}
		return;
	}
#endif

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex   = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier     = ModifierTable[RedTableIndex][RedPixelIndex];

			INT Red11;
			if ( RedModifier==0 )
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier,   -1023, 1023 );
			}
			else
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier  *RedMultiplier  *8, -1023, 1023 );
			}

			const DWORD GreenPixelIndex = EAC_PIXEL_INDEX(GreenPixelIndices,j+4*i);
			const INT   GreenModifier   = ModifierTable[GreenTableIndex][GreenPixelIndex];

			INT Green11;
			if ( GreenModifier==0 )
			{
				Green11 = Clamp<INT>( GreenBaseCodeword*8+GreenModifier, -1023, 1023 );
			}
			else
			{
				Green11 = Clamp<INT>( GreenBaseCodeword*8+GreenModifier*GreenMultiplier*8, -1023, 1023 );
			}

			Texels[i+4*j].R = EAC_EXTEND_11_TO_16_SIGNED_BITS(Red11);
			Texels[i+4*j].G = EAC_EXTEND_11_TO_16_SIGNED_BITS(Green11);
		}
	}
}
inline void FDecompressBlockEAC_RG_S( const BYTE Block[16], FRedGreen32F Texels[16] )
{
	const BYTE* RedBlock   = Block+0;
	const BYTE* GreenBlock = Block+8;

	// Consists of two EAC_R_S channels. Red data preceeds green data.
	const INT   RedBaseCodeword   = (SBYTE)EAC_EXTRACT_BASE_CODEWORD(RedBlock);
	const INT   RedMultiplier     = EAC_EXTRACT_MULTIPLIER(RedBlock);
	const DWORD RedTableIndex     = EAC_EXTRACT_TABLE_INDEX(RedBlock);
	const QWORD RedPixelIndices   = EAC_EXTRACT_PIXEL_INDICES(RedBlock);

	const INT   GreenBaseCodeword = (SBYTE)EAC_EXTRACT_BASE_CODEWORD(GreenBlock);
	const INT   GreenMultiplier   = EAC_EXTRACT_MULTIPLIER(GreenBlock);
	const DWORD GreenTableIndex   = EAC_EXTRACT_TABLE_INDEX(GreenBlock);
	const QWORD GreenPixelIndices = EAC_EXTRACT_PIXEL_INDICES(GreenBlock);

	//
	// -128 is not allowed to occur as value for the BaseCodeword, however a decoder
	// is required to map it to -127.
	//
	// It may make sense to violate the spec, and color code here, to spot encoder
	// bugs. --han
	//
#if 0
	// Conform.
	if ( RedBaseCodeword==-128 )
	{
		RedBaseCodeword = -127;
	}
	if ( GreenBaseCodeword==-128 )
	{
		GreenBaseCodeword = -127;
	}
#else
	// Stipple.
	if ( RedBaseCodeword==-128 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = (iTexel%2)^((iTexel%8)/4) ? 1.0 : 0.0;
			Texels[iTexel].G = 0.0;
		}
		return;
	}
	if ( GreenBaseCodeword==-128 )
	{
		for ( INT iTexel=0; iTexel<16; iTexel++ )
		{
			Texels[iTexel].R = 0.0;
			Texels[iTexel].G = (iTexel%2)^((iTexel%8)/4) ? 1.0 : 0.0;
		}
		return;
	}
#endif

	const INT ModifierTable[16][8] =
	{
		{ -3, -6,  -9, -15, 2, 5, 8, 14 },
		{ -3, -7, -10, -13, 2, 6, 9, 12 },
		{ -2, -5,  -8, -13, 1, 4, 7, 12 },
		{ -2, -4,  -6, -13, 1, 3, 5, 12 },
		{ -3, -6,  -8, -12, 2, 5, 7, 11 },
		{ -3, -7,  -9, -11, 2, 6, 8, 10 },
		{ -4, -7,  -8, -11, 3, 6, 7, 10 },
		{ -3, -5,  -8, -11, 2, 4, 7, 10 },
		{ -2, -6,  -8, -10, 1, 5, 7,  9 },
		{ -2, -5,  -8, -10, 1, 4, 7,  9 },
		{ -2, -4,  -8, -10, 1, 3, 7,  9 },
		{ -2, -5,  -7, -10, 1, 4, 6,  9 },
		{ -3, -4,  -7, -10, 2, 3, 6,  9 },
		{ -1, -2,  -3, -10, 0, 1, 2,  9 },
		{ -4, -6,  -8,  -9, 3, 5, 7,  8 },
		{ -3, -5,  -7,  -9, 2, 4, 6,  8 }
	};

	for ( INT i=0; i<4; i++ )
	{
		for ( INT j=0; j<4; j++ )
		{
			const DWORD RedPixelIndex   = EAC_PIXEL_INDEX(RedPixelIndices,j+4*i);
			const INT   RedModifier     = ModifierTable[RedTableIndex][RedPixelIndex];

			DWORD Red11;
			if ( RedModifier==0 )
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier,   -1023, 1023 );
			}
			else
			{
				Red11 = Clamp<INT>( RedBaseCodeword  *8+RedModifier  *RedMultiplier  *8, -1023, 1023 );
			}

			const DWORD GreenPixelIndex = EAC_PIXEL_INDEX(GreenPixelIndices,j+4*i);
			const INT   GreenModifier   = ModifierTable[GreenTableIndex][GreenPixelIndex];

			DWORD Green11;
			if ( GreenModifier==0 )
			{
				Green11 = Clamp<INT>( GreenBaseCodeword*8+GreenModifier, -1023, 1023 );
			}
			else
			{
				Green11 = Clamp<INT>( GreenBaseCodeword*8+GreenModifier*GreenMultiplier*8, -1023, 1023 );
			}

			Texels[i+4*j].R = Red11/1023.0;
			Texels[i+4*j].G = Green11/1023.0;
		}
	}
}

/*----------------------------------------------------------------------------
	ASTC decompression routines.
----------------------------------------------------------------------------*/

//
// PROBABLY-FIX-ME-LATER.
//

/*----------------------------------------------------------------------------
	PVTRC decompression routines.
----------------------------------------------------------------------------*/

//
// MAYBE-FIX-ME.
//

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
