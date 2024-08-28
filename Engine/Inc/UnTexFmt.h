/*=============================================================================
	UnTexFmt.h: Unreal texture type and format definitions and routines.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
	Copyright 2015-2020 Sebastian Kaufel, Inc. All Rights Reserved.

	Revision history:
	 * Created by Sebastian Kaufel based on ETextureFormat enum from UnTex.h

	TODO:
	 * Come up with better naming scheme as GetTEXTURETYPE/GetTEXTUREFORMAT,
	   and eventually allow it to parse parts of a stream, and not on a whole
	   word basis only.
	 * OpenGL only allows several (or all?) of the Block Compressed formats
	   only to be used on 2D & 3D or 3D only. See if this is a thing for
	   other APIs as well, and see if we need some helper here to verify
		 whether it's a valid format for the ETextureType.
	 * Proper PVRTC support for FRecommendDecompressedFormat.
	 * Unit testing could be a thing here.#
	 * Add helper for calculating mip sizes.
	 * Helper for calculating strides?
=============================================================================*/

/*----------------------------------------------------------------------------
	Texture type.
----------------------------------------------------------------------------*/

//
// A dimensioness of a texture.
//
enum ETextureType
{
	TEXT_2D           = 0x00,
	TEXT_3D           = 0x01,
	TEXT_1D           = 0x02,
	TEXT_CubeMap      = 0x03,
	TEXT_Array1D      = 0x04,
	TEXT_Array2D      = 0x05,
	TEXT_ArrayCubeMap = 0x06,
	TEXT_MAX		      = 0xff,
};

#if 0
// UnrealScript.
var(Info) const editconst enum ETextureType
{
	TEXT_2D,
	TEXT_3D,
	TEXT_1D,
	TEXT_CubeMap,
	TEXT_Array1D,
	TEXT_Array2D,
	TEXT_ArrayCubeMap,
} Type;
#endif

inline const TCHAR* FTextureTypeString( BYTE Format )
{
	guard(FTextureTypeString);
	switch ( Format )
	{
		case TEXT_2D:           return TEXT("2D");
		case TEXT_3D:           return TEXT("3D");
		case TEXT_1D:           return TEXT("1D");
		case TEXT_CubeMap:      return TEXT("CubeMap");
		case TEXT_Array1D:      return TEXT("Array1D");
		case TEXT_Array2D:      return TEXT("Array2D");
		case TEXT_ArrayCubeMap: return TEXT("ArrayCubeMap");

		// MAX.
		case TEXT_MAX:			    return TEXT("MAX");

		// Unknown.
		default:
		{
			TCHAR* Res = appStaticString1024();
			appSprintf(Res, TEXT("0x%02X"), Format);
			return Res;
		}
	}
	unguard;
}

// !! FIX-ME !! Find a less horrible name.
inline UBOOL GetTEXTURETYPE( const TCHAR* Stream, BYTE& OutValue )
{
	guard(GetTEXTURETYPE);
	if ( !Stream || !*Stream )
		return 0;
	FString Str(Stream);

	// Original.
	if ( Str==TEXT("TEXT_2D")           || Str==TEXT("2D")           ) { OutValue = TEXT_2D;           return 1; }
	if ( Str==TEXT("TEXT_3D")           || Str==TEXT("3D")           ) { OutValue = TEXT_3D;           return 1; }
	if ( Str==TEXT("TEXT_1D")           || Str==TEXT("1D")           ) { OutValue = TEXT_1D;           return 1; }
	if ( Str==TEXT("TEXT_CubeMap")      || Str==TEXT("CubeMap")      ) { OutValue = TEXT_CubeMap;      return 1; }
	if ( Str==TEXT("TEXT_Array1D")      || Str==TEXT("Array1D")      ) { OutValue = TEXT_Array1D;      return 1; }
	if ( Str==TEXT("TEXT_Array2D")      || Str==TEXT("Array2D")      ) { OutValue = TEXT_Array2D;      return 1; }
	if ( Str==TEXT("TEXT_ArrayCubeMap") || Str==TEXT("ArrayCubeMap") ) { OutValue = TEXT_ArrayCubeMap; return 1; }

	// MAX. Could be used to reset a outer option I guess.
	if ( Str==TEXT("TEXT_MAX")          || Str==TEXT("MAX")          ) { OutValue = TEXT_MAX;          return 1; }

	// Number
	if ( appIsDigit(Stream[0]) )
	{
		OutValue = (BYTE)appAtoi(Stream);
		return 1;
	}
	return 0;
	unguard;
}

/*----------------------------------------------------------------------------
	Routines for calculating mipmap count and sizes.
----------------------------------------------------------------------------*/

//
// These rules follow the rounding down convention, which is
// the umbiquous standard for (hardware based) rendering.
//
// Rounding down vs. rounding up is sketched in covered in rough detail
// inside:
//
//   [2003] (Guthe, Heckbert) Non-Power-of-Two Mipmap Creation
//   http://download.nvidia.com/developer/Papers/2005/NP2_Mipmapping/NP2_Mipmap_Creation.pdf
//

//
// Calculates maximum mipmap level.
//
inline INT FMipmapMaxLevel( INT Width, INT Height=1, INT Depth=1 )
{
	return Max<INT>( appFloorLogTwo(Width), appFloorLogTwo(Height), appFloorLogTwo(Depth) );
}

//
// Calculates number of mipmap levels.
//
inline INT FMipmapNumLevels( INT Width, INT Height=1, INT Depth=1 )
{
	return FMipmapMaxLevel(Width,Height,Depth)+1;
}

//
// Calculates extends for a given mip level. You may still have to align to
// block sizes for storage afterwards.
//
// Level=0 is the highest resolution mipmap level.
//
// Examples:
//
//   Calculate storage size for an image of format Format
//   and size USize x VSize at mipmap level Level:
//
//     FTextureBytes( Format, FMipmapExtend(USize,Level), FMipmapExtend(VSize,Level) )
//
//   Calculate block aliogned width for a given Format and width USize at a given level Level.
//
//     FTextureBlockAlignedWidth( Format, FMipmapExtend(Level) )
//
inline INT FMipmapExtend( INT Width, INT Level )
{
	// !! FIX-ME !! IMPLEMENT !!
	return 0;
}

/*----------------------------------------------------------------------------
	Texture formats.
----------------------------------------------------------------------------*/

inline UBOOL bCompressedFormat(BYTE Format)
{
	if (
		Format == TEXF_BC1 ||
		Format == TEXF_BC2 ||
		Format == TEXF_BC3 ||
		Format == TEXF_BC4 ||
		Format == TEXF_BC5 ||
		Format == TEXF_BC7 ||
		Format == TEXF_ETC1 ||
		Format == TEXF_MONO
		)
		return 1;
	else return 0;
}
// Defines for legacy names.
#if 1
	#define TEXF_RGBA7          TEXF_BGRA8_LM
	#define TEXF_DXT1           TEXF_BC1
	#define TEXF_DXT3           TEXF_BC2
	#define TEXF_DXT5           TEXF_BC3
// Defines for Unreal 227 names.
    // #define TEXF_RGBA8          TEXF_BGRA8 //DO NOT USE (anymore)! All TEXF_RGBA8 should be replaced by now, since actually really TEXF_BGRA8. Maybe should rename all RGBA8_ to RGBA8 now.
	#define TEXF_RGTC_R         TEXF_BC4
	#define TEXF_RGTC_R_SIGNED  TEXF_BC4_S
	#define TEXF_RGTC_RG        TEXF_BC5
	#define TEXF_RGTC_RG_SIGNED TEXF_BC5_S
	#define TEXF_BPTC_RGBA      TEXF_BC7
	#define TEXF_BPTC_RGB_SF    TEXF_BC6H_S
	#define TEXF_BPTC_RGB_UF    TEXF_BC6H
#endif

inline FString FTextureFormatString(BYTE Format)
{
	guard(FTextureFormatString);
	switch (Format)
	{
		// Original.
		case TEXF_P8:                return TEXT("P8");
		case TEXF_BGRA8_LM:          return TEXT("BGRA8_LM");
		case TEXF_R5G6B5:            return TEXT("R5G6B5");
		case TEXF_BC1:               return TEXT("BC1/DXT1");
		case TEXF_RGB8:              return TEXT("RGB8");
		case TEXF_BGRA8:             return TEXT("BGRA8");

		// S3TC (continued).
		case TEXF_BC2:               return TEXT("BC2/DXT3");
		case TEXF_BC3:               return TEXT("BC3/DXT5");

		// RGTC.
		case TEXF_BC4:               return TEXT("BC4");
		case TEXF_BC4_S:             return TEXT("BC4_S");
		case TEXF_BC5:               return TEXT("BC5");
		case TEXF_BC5_S:             return TEXT("BC5_S");

		// BPTC.
		case TEXF_BC7:               return TEXT("BC7");
		case TEXF_BC6H_S:            return TEXT("BC6H_S");
		case TEXF_BC6H:              return TEXT("BC6H");

		// Normalized RGBA.
		case TEXF_RGBA16:            return TEXT("RGBA16");
		case TEXF_RGBA16_S:          return TEXT("RGBA16_S");
		case TEXF_RGBA32:            return TEXT("RGBA32");
		case TEXF_RGBA32_S:          return TEXT("RGBA32_S");

		// Monochrome texture (fonts mainly)
        case TEXF_MONO:              return TEXT("MONO");

		// Meta.
		case TEXF_NODATA:            return TEXT("NODATA");
		case TEXF_UNCOMPRESSED:      return TEXT("UNCOMPRESSED");
		case TEXF_UNCOMPRESSED_LOW:  return TEXT("UNCOMPRESSED_LOW");
		case TEXF_UNCOMPRESSED_HIGH: return TEXT("UNCOMPRESSED_HIGH");
		case TEXF_COMPRESSED:        return TEXT("COMPRESSED");
		case TEXF_COMPRESSED_LOW:    return TEXT("COMPRESSED_LOW");
		case TEXF_COMPRESSED_HIGH:   return TEXT("COMPRESSED_HIGH");

		// S3TC (continued).
		case TEXF_BC1_PA:            return TEXT("BC1_PA");

		// Normalized RGBA (continued).
		case TEXF_R8:                return TEXT("R8");
		case TEXF_R8_S:              return TEXT("R8_S");
		case TEXF_R16:               return TEXT("R16");
		case TEXF_R16_S:             return TEXT("R16_S");
		case TEXF_R32:               return TEXT("R32");
		case TEXF_R32_S:             return TEXT("R32_S");
		case TEXF_RG8:               return TEXT("RG8");
		case TEXF_RG8_S:             return TEXT("RG8_S");
		case TEXF_RG16:              return TEXT("RG16");
		case TEXF_RG16_S:            return TEXT("RG16_S");
		case TEXF_RG32:              return TEXT("RG32");
		case TEXF_RG32_S:            return TEXT("RG32_S");
		//
		case TEXF_RGB8_S:            return TEXT("RGB8_S");
		case TEXF_RGB16_:            return TEXT("RGB16");
		case TEXF_RGB16_S:           return TEXT("RGB16_S");
		case TEXF_RGB32:             return TEXT("RGB32");
		case TEXF_RGB32_S:           return TEXT("RGB32_S");
		case TEXF_RGBA8_:            return TEXT("RGBA8");
		case TEXF_RGBA8_S:           return TEXT("RGBA8_S");

		// Floating point RGBA.
		case TEXF_R16_F:             return TEXT("R16_F");
		case TEXF_R32_F:             return TEXT("R32_F");
		case TEXF_RG16_F:            return TEXT("RG16_F");
		case TEXF_RG32_F:            return TEXT("RG32_F");
		case TEXF_RGB16_F:           return TEXT("RGB16_F");
		case TEXF_RGB32_F:           return TEXT("RGB32_F");
		case TEXF_RGBA16_F:          return TEXT("RGBA16_F");
		case TEXF_RGBA32_F:          return TEXT("RGBA32_F");

		// ETC1/ETC2/EAC.
		case TEXF_ETC1:              return TEXT("ETC1");
		case TEXF_ETC2:              return TEXT("ETC2");
		case TEXF_ETC2_PA:           return TEXT("ETC2_PA");
		case TEXF_ETC2_RGB_EAC_A:    return TEXT("ETC2_RGB_EAC_A");
		case TEXF_EAC_R:             return TEXT("EAC_R");
		case TEXF_EAC_R_S:           return TEXT("EAC_R_S");
		case TEXF_EAC_RG:            return TEXT("EAC_RG");
		case TEXF_EAC_RG_S:          return TEXT("EAC_RG_S");

		// ASTC.
		case TEXF_ASTC_4x4:          return TEXT("ASTC_4x4");
		case TEXF_ASTC_5x4:          return TEXT("ASTC_5x4");
		case TEXF_ASTC_5x5:          return TEXT("ASTC_5x5");
		case TEXF_ASTC_6x5:          return TEXT("ASTC_6x5");
		case TEXF_ASTC_6x6:          return TEXT("ASTC_6x6");
		case TEXF_ASTC_8x5:          return TEXT("ASTC_8x5");
		case TEXF_ASTC_8x6:          return TEXT("ASTC_8x6");
		case TEXF_ASTC_8x8:          return TEXT("ASTC_8x8");
		case TEXF_ASTC_10x5:         return TEXT("ASTC_10x5");
		case TEXF_ASTC_10x6:         return TEXT("ASTC_10x6");
		case TEXF_ASTC_10x8:         return TEXT("ASTC_10x8");
		case TEXF_ASTC_10x10:        return TEXT("ASTC_10x10");
		case TEXF_ASTC_12x10:        return TEXT("ASTC_12x10");
		case TEXF_ASTC_12x12:        return TEXT("ASTC_12x12");
		case TEXF_ASTC_3x3x3:        return TEXT("ASTC_3x3x3");
		case TEXF_ASTC_4x3x3:        return TEXT("ASTC_4x3x3");
		case TEXF_ASTC_4x4x3:        return TEXT("ASTC_4x4x3");
		case TEXF_ASTC_4x4x4:        return TEXT("ASTC_4x4x4");
		case TEXF_ASTC_5x4x4:        return TEXT("ASTC_5x4x4");
		case TEXF_ASTC_5x5x4:        return TEXT("ASTC_5x5x4");
		case TEXF_ASTC_5x5x5:        return TEXT("ASTC_5x5x5");
		case TEXF_ASTC_6x5x5:        return TEXT("ASTC_6x5x5");
		case TEXF_ASTC_6x6x5:        return TEXT("ASTC_6x6x5");
		case TEXF_ASTC_6x6x6:        return TEXT("ASTC_6x6x6");

		case TEXF_PVRTC1_2BPP:       return TEXT("PVRTC1_2BPP");
		case TEXF_PVRTC1_4BPP:       return TEXT("PVRTC1_4BPP");
		case TEXF_PVRTC2_2BPP:       return TEXT("PVRTC2_2BPP");
		case TEXF_PVRTC2_4BPP:       return TEXT("PVRTC2_4BPP");

		// RGBA (Integral).
		case TEXF_R8_UI:             return TEXT("R8_UI");
		case TEXF_R8_I:              return TEXT("R8_I");
		case TEXF_R16_UI:            return TEXT("R16_UI");
		case TEXF_R16_I:             return TEXT("R16_I");
		case TEXF_R32_UI:            return TEXT("R32_UI");
		case TEXF_R32_I:             return TEXT("R32_I");
		case TEXF_RG8_UI:            return TEXT("RG8_UI");
		case TEXF_RG8_I:             return TEXT("RG8_I");
		case TEXF_RG16_UI:           return TEXT("RG16_UI");
		case TEXF_RG16_I:            return TEXT("RG16_I");
		case TEXF_RG32_UI:           return TEXT("RG32_UI");
		case TEXF_RG32_I:            return TEXT("RG32_I");
		case TEXF_RGB8_UI:           return TEXT("RGB8_UI");
		case TEXF_RGB8_I:            return TEXT("RGB8_I");
		case TEXF_RGB16_UI:          return TEXT("RGB16_UI");
		case TEXF_RGB16_I:           return TEXT("RGB16_I");
		case TEXF_RGB32_UI:          return TEXT("RGB32_UI");
		case TEXF_RGB32_I:           return TEXT("RGB32_I");
		case TEXF_RGBA8_UI:          return TEXT("RGBA8_UI");
		case TEXF_RGBA8_I:           return TEXT("RGBA8_I");
		case TEXF_RGBA16_UI:         return TEXT("RGBA16_UI");
		case TEXF_RGBA16_I:          return TEXT("RGBA16_I:");
		case TEXF_RGBA32_UI:         return TEXT("RGBA32_UI");
		case TEXF_RGBA32_I:          return TEXT("RGBA32_I");

		// Special.
		case TEXF_ARGB8:             return TEXT("ARGB8");
		case TEXF_ABGR8:             return TEXT("ABGR8");
		case TEXF_RGB10A2:           return TEXT("RGB10A2");
		case TEXF_RGB10A2_UI:        return TEXT("RGB10A2_UI");
		case TEXF_RGB10A2_LM:        return TEXT("RGB10A2_LM");
		case TEXF_RGB9E5:            return TEXT("RGB9E5");
		case TEXF_P8_RGB9E5:         return TEXT("P8_RGB9E5");
		case TEXF_R1:                return TEXT("R1");
#if 0 // not defined in 227
		case TEXF_RGB10A2_S:         return TEXT("RGB10A2_S");
		case TEXF_RGB10A2_I:         return TEXT("RGB10A2_I");
		case TEXF_R11G11B10_F:       return TEXT("R11G11B10_F");

		// Normalized BGR.
		case TEXF_B5G6R5:            return TEXT("B5G6R5");
		case TEXF_BGR8:              return TEXT("BGR8");

		// Double precission floating point RGBA.
		case TEXF_R64_F:             return TEXT("R64_F");
		case TEXF_RG64_F:            return TEXT("RG64_F");
		case TEXF_RGB64_F:           return TEXT("RGB64_F");
		case TEXF_RGBA64_F:          return TEXT("RGBA64_F");
#endif

		// MAX.
		case TEXF_MAX:			     return TEXT("MAX");

		// Unknown.
		default:                     return FString::Printf(TEXT("0x%02X"), Format);
	}
	unguard;
}

inline const TCHAR* GetTextureFormatString(BYTE TexFormat)
{
#define FORMAT_CASE(nm) case nm: return TEXT(#nm)
	switch (TexFormat)
	{
		FORMAT_CASE(TEXF_P8);
//		FORMAT_CASE(TEXF_RGBA7);
//		FORMAT_CASE(TEXF_DXT1);
		FORMAT_CASE(TEXF_RGB8);
//		FORMAT_CASE(TEXF_RGBA8);
//		FORMAT_CASE(TEXF_DXT3);
//		FORMAT_CASE(TEXF_DXT5);
//		FORMAT_CASE(TEXF_RGTC_R);
//		FORMAT_CASE(TEXF_RGTC_R_SIGNED);
//		FORMAT_CASE(TEXF_RGTC_RG);
//		FORMAT_CASE(TEXF_RGTC_RG_SIGNED);
//		FORMAT_CASE(TEXF_BPTC_RGBA);
//		FORMAT_CASE(TEXF_BPTC_RGB_SF);
//		FORMAT_CASE(TEXF_BPTC_RGB_UF);
		FORMAT_CASE(TEXF_RGBA16);
		FORMAT_CASE(TEXF_RGBA16_S);
		FORMAT_CASE(TEXF_RGBA32);
		FORMAT_CASE(TEXF_RGBA32_S);
		FORMAT_CASE(TEXF_MONO);
        FORMAT_CASE(TEXF_BGRA8_LM);
        FORMAT_CASE(TEXF_R5G6B5);
        FORMAT_CASE(TEXF_BC1);
        FORMAT_CASE(TEXF_BGRA8);
        FORMAT_CASE(TEXF_BC2);
        FORMAT_CASE(TEXF_BC3);
        FORMAT_CASE(TEXF_BC4);
        FORMAT_CASE(TEXF_BC4_S);
        FORMAT_CASE(TEXF_BC5);
        FORMAT_CASE(TEXF_BC5_S);
        FORMAT_CASE(TEXF_BC7);
        FORMAT_CASE(TEXF_BC6H_S);
        FORMAT_CASE(TEXF_BC6H);
        FORMAT_CASE(TEXF_UNCOMPRESSED);
        FORMAT_CASE(TEXF_UNCOMPRESSED_LOW);
        FORMAT_CASE(TEXF_UNCOMPRESSED_HIGH);
        FORMAT_CASE(TEXF_COMPRESSED);
        FORMAT_CASE(TEXF_COMPRESSED_LOW);
        FORMAT_CASE(TEXF_COMPRESSED_HIGH);
        FORMAT_CASE(TEXF_BC1_PA);
        FORMAT_CASE(TEXF_R8);
        FORMAT_CASE(TEXF_R8_S);
        FORMAT_CASE(TEXF_R16);
        FORMAT_CASE(TEXF_R16_S);
        FORMAT_CASE(TEXF_R32);
        FORMAT_CASE(TEXF_R32_S);
        FORMAT_CASE(TEXF_RG8);
        FORMAT_CASE(TEXF_RG8_S);
        FORMAT_CASE(TEXF_RG16);
        FORMAT_CASE(TEXF_RG16_S);
        FORMAT_CASE(TEXF_RG32);
        FORMAT_CASE(TEXF_RG32_S);
        FORMAT_CASE(TEXF_RGB8_S);
        FORMAT_CASE(TEXF_RGB16_);
        FORMAT_CASE(TEXF_RGB16_S);
        FORMAT_CASE(TEXF_RGB32);
        FORMAT_CASE(TEXF_RGB32_S);
//      FORMAT_CASE(TEXF_RGBA8);
        FORMAT_CASE(TEXF_RGBA8_);
        FORMAT_CASE(TEXF_RGBA8_S);
        FORMAT_CASE(TEXF_R16_F);
        FORMAT_CASE(TEXF_R32_F);
        FORMAT_CASE(TEXF_RG16_F);
        FORMAT_CASE(TEXF_RG32_F);
        FORMAT_CASE(TEXF_RGB16_F);
        FORMAT_CASE(TEXF_RGB32_F);
        FORMAT_CASE(TEXF_RGBA16_F);
        FORMAT_CASE(TEXF_RGBA32_F);
        FORMAT_CASE(TEXF_ETC1);
        FORMAT_CASE(TEXF_ETC2);
        FORMAT_CASE(TEXF_ETC2_PA);
        FORMAT_CASE(TEXF_ETC2_RGB_EAC_A);
        FORMAT_CASE(TEXF_EAC_R);
        FORMAT_CASE(TEXF_EAC_R_S);
        FORMAT_CASE(TEXF_EAC_RG);
        FORMAT_CASE(TEXF_EAC_RG_S);
        FORMAT_CASE(TEXF_ASTC_4x4);
        FORMAT_CASE(TEXF_ASTC_5x4);
        FORMAT_CASE(TEXF_ASTC_5x5);
        FORMAT_CASE(TEXF_ASTC_6x5);
        FORMAT_CASE(TEXF_ASTC_6x6);
        FORMAT_CASE(TEXF_ASTC_8x5);
        FORMAT_CASE(TEXF_ASTC_8x6);
        FORMAT_CASE(TEXF_ASTC_8x8);
        FORMAT_CASE(TEXF_ASTC_10x5);
        FORMAT_CASE(TEXF_ASTC_10x6);
        FORMAT_CASE(TEXF_ASTC_10x8);
        FORMAT_CASE(TEXF_ASTC_10x10);
        FORMAT_CASE(TEXF_ASTC_12x10);
        FORMAT_CASE(TEXF_ASTC_12x12);
        FORMAT_CASE(TEXF_ASTC_3x3x3);
        FORMAT_CASE(TEXF_ASTC_4x3x3);
        FORMAT_CASE(TEXF_ASTC_4x4x3);
        FORMAT_CASE(TEXF_ASTC_4x4x4);
        FORMAT_CASE(TEXF_ASTC_5x4x4);
        FORMAT_CASE(TEXF_ASTC_5x5x4);
        FORMAT_CASE(TEXF_ASTC_5x5x5);
        FORMAT_CASE(TEXF_ASTC_6x5x5);
        FORMAT_CASE(TEXF_ASTC_6x6x5);
        FORMAT_CASE(TEXF_ASTC_6x6x6);
        FORMAT_CASE(TEXF_PVRTC1_2BPP);
        FORMAT_CASE(TEXF_PVRTC1_4BPP);
        FORMAT_CASE(TEXF_PVRTC2_2BPP);
        FORMAT_CASE(TEXF_PVRTC2_4BPP);
        FORMAT_CASE(TEXF_R8_UI);
        FORMAT_CASE(TEXF_R8_I);
        FORMAT_CASE(TEXF_R16_UI);
        FORMAT_CASE(TEXF_R16_I);
        FORMAT_CASE(TEXF_R32_UI);
        FORMAT_CASE(TEXF_R32_I);
        FORMAT_CASE(TEXF_RG8_UI);
        FORMAT_CASE(TEXF_RG8_I);
        FORMAT_CASE(TEXF_RG16_UI);
        FORMAT_CASE(TEXF_RG16_I);
        FORMAT_CASE(TEXF_RG32_UI);
        FORMAT_CASE(TEXF_RG32_I);
        FORMAT_CASE(TEXF_RGB8_UI);
        FORMAT_CASE(TEXF_RGB8_I);
        FORMAT_CASE(TEXF_RGB16_UI);
        FORMAT_CASE(TEXF_RGB16_I);
        FORMAT_CASE(TEXF_RGB32_UI);
        FORMAT_CASE(TEXF_RGB32_I);
        FORMAT_CASE(TEXF_RGBA8_UI);
        FORMAT_CASE(TEXF_RGBA8_I);
        FORMAT_CASE(TEXF_RGBA16_UI);
        FORMAT_CASE(TEXF_RGBA16_I);
        FORMAT_CASE(TEXF_RGBA32_UI);
        FORMAT_CASE(TEXF_RGBA32_I);
        FORMAT_CASE(TEXF_ARGB8);
        FORMAT_CASE(TEXF_ABGR8);
        FORMAT_CASE(TEXF_RGB10A2);
        FORMAT_CASE(TEXF_RGB10A2_UI);
        FORMAT_CASE(TEXF_RGB10A2_LM);
        FORMAT_CASE(TEXF_RGB9E5);
        FORMAT_CASE(TEXF_P8_RGB9E5);
        FORMAT_CASE(TEXF_R1);
        FORMAT_CASE(TEXF_NODATA);
		FORMAT_CASE(TEXF_MAX);

	default:
		return TEXT("Unknown");
	}
#undef FORMAT_CASE
}

// !! FIX-ME !! Find a less horrible name.
inline UBOOL GetTEXTUREFORMAT( const TCHAR* Stream, BYTE& OutValue )
{
	guard(GetTEXTUREFORMAT);
	if ( !Stream || !*Stream )
		return 0;
	FString Str(Stream);

	// Original.
	if ( Str==TEXT("TEXF_P8")                || Str==TEXT("P8")                ) { OutValue = TEXF_P8;                return 1; }
	if ( Str==TEXT("TEXF_BGRA8_LM")          || Str==TEXT("BGRA8_LM")          ) { OutValue = TEXF_BGRA8_LM;          return 1; }
	if ( Str==TEXT("TEXF_R5G6B5")            || Str==TEXT("R5G6B5")            ) { OutValue = TEXF_R5G6B5;            return 1; }
	if ( Str==TEXT("TEXF_BC1")               || Str==TEXT("BC1")               ) { OutValue = TEXF_BC1;               return 1; }
	if ( Str==TEXT("TEXF_RGB8")              || Str==TEXT("RGB8")              ) { OutValue = TEXF_RGB8;              return 1; }
	if ( Str==TEXT("TEXF_BGRA8")             || Str==TEXT("BGRA8")             ) { OutValue = TEXF_BGRA8;             return 1; }

	// S3TC (continued).
	if ( Str==TEXT("TEXF_BC2")               || Str==TEXT("BC2")               ) { OutValue = TEXF_BC2;               return 1; }
	if ( Str==TEXT("TEXF_BC3")               || Str==TEXT("BC3")               ) { OutValue = TEXF_BC3;               return 1; }

	// RGTC.
	if ( Str==TEXT("TEXF_BC4")               || Str==TEXT("BC4")               ) { OutValue = TEXF_BC4;               return 1; }
	if ( Str==TEXT("TEXF_BC4_S")             || Str==TEXT("BC4_S")             ) { OutValue = TEXF_BC4_S;             return 1; }
	if ( Str==TEXT("TEXF_BC5")               || Str==TEXT("BC5")               ) { OutValue = TEXF_BC5;               return 1; }
	if ( Str==TEXT("TEXF_BC5_S")             || Str==TEXT("BC5_S")             ) { OutValue = TEXF_BC5_S;             return 1; }

	// BPTC.
	if ( Str==TEXT("TEXF_BC7")               || Str==TEXT("BC7")               ) { OutValue = TEXF_BC7;               return 1; }
	if ( Str==TEXT("TEXF_BC6H")              || Str==TEXT("BC6H")              ) { OutValue = TEXF_BC6H;              return 1; }
	if ( Str==TEXT("TEXF_BC6H_S")            || Str==TEXT("BC6H_S")            ) { OutValue = TEXF_BC6H_S;            return 1; }

	// Normalized RGBA.
	if ( Str==TEXT("TEXF_RGBA16")            || Str==TEXT("RGBA16")            ) { OutValue = TEXF_RGBA16;            return 1; }
	if ( Str==TEXT("TEXF_RGBA16_S")          || Str==TEXT("RGBA16_S")          ) { OutValue = TEXF_RGBA16_S;          return 1; }
	if ( Str==TEXT("TEXF_RGBA32")            || Str==TEXT("RGBA32")            ) { OutValue = TEXF_RGBA32;            return 1; }
	if ( Str==TEXT("TEXF_RGBA32_S")          || Str==TEXT("RGBA32_S")          ) { OutValue = TEXF_RGBA32_S;          return 1; }

	// Meta.
	if ( Str==TEXT("TEXF_NODATA")            || Str==TEXT("NODATA")            ) { OutValue = TEXF_NODATA;            return 1; }
	if ( Str==TEXT("TEXF_UNCOMPRESSED")      || Str==TEXT("UNCOMPRESSED")      ) { OutValue = TEXF_UNCOMPRESSED;      return 1; }
	if ( Str==TEXT("TEXF_UNCOMPRESSED_LOW")  || Str==TEXT("UNCOMPRESSED_LOW")  ) { OutValue = TEXF_UNCOMPRESSED_LOW;  return 1; }
	if ( Str==TEXT("TEXF_UNCOMPRESSED_HIGH") || Str==TEXT("UNCOMPRESSED_HIGH") ) { OutValue = TEXF_UNCOMPRESSED_HIGH; return 1; }
	if ( Str==TEXT("TEXF_COMPRESSED")        || Str==TEXT("COMPRESSED")        ) { OutValue = TEXF_COMPRESSED;        return 1; }
	if ( Str==TEXT("TEXF_COMPRESSED_LOW")    || Str==TEXT("COMPRESSED_LOW")    ) { OutValue = TEXF_COMPRESSED_LOW;    return 1; }
	if ( Str==TEXT("TEXF_COMPRESSED_HIGH")   || Str==TEXT("COMPRESSED_HIGH")   ) { OutValue = TEXF_COMPRESSED_HIGH;   return 1; }

	// S3TC (continued).
	if ( Str==TEXT("TEXF_BC1_PA")            || Str==TEXT("BC1_PA")            ) { OutValue = TEXF_BC1_PA;            return 1; }

	// Normalized RGBA (continued).
	if ( Str==TEXT("TEXF_R8")                || Str==TEXT("R8")                ) { OutValue = TEXF_R8;                return 1; }
	if ( Str==TEXT("TEXF_R8_S")              || Str==TEXT("R8_S")              ) { OutValue = TEXF_R8_S;              return 1; }
	if ( Str==TEXT("TEXF_R16")               || Str==TEXT("R16")               ) { OutValue = TEXF_R16;               return 1; }
	if ( Str==TEXT("TEXF_R16_S")             || Str==TEXT("R16_S")             ) { OutValue = TEXF_R16_S;             return 1; }
	if ( Str==TEXT("TEXF_R32")               || Str==TEXT("R32")               ) { OutValue = TEXF_R32;               return 1; }
	if ( Str==TEXT("TEXF_R32_S")             || Str==TEXT("R32_S")             ) { OutValue = TEXF_R32_S;             return 1; }
	if ( Str==TEXT("TEXF_RG8")               || Str==TEXT("RG8")               ) { OutValue = TEXF_RG8;               return 1; }
	if ( Str==TEXT("TEXF_RG8_S")             || Str==TEXT("RG8_S")             ) { OutValue = TEXF_RG8_S;             return 1; }
	if ( Str==TEXT("TEXF_RG16")              || Str==TEXT("RG16")              ) { OutValue = TEXF_RG16;              return 1; }
	if ( Str==TEXT("TEXF_RG16_S")            || Str==TEXT("RG16_S")            ) { OutValue = TEXF_RG16_S;            return 1; }
	if ( Str==TEXT("TEXF_RG32")              || Str==TEXT("RG32")              ) { OutValue = TEXF_RG32;              return 1; }
	if ( Str==TEXT("TEXF_RG32_S")            || Str==TEXT("RG32_S")            ) { OutValue = TEXF_RG32_S;            return 1; }
	//
	if ( Str==TEXT("TEXF_RGB8_S")            || Str==TEXT("RGB8_S")            ) { OutValue = TEXF_RGB8_S;            return 1; }
	if ( Str==TEXT("TEXF_RGB16")             || Str==TEXT("RGB16")             ) { OutValue = TEXF_RGB16_;            return 1; }
	if ( Str==TEXT("TEXF_RGB16_S")           || Str==TEXT("RGB16_S")           ) { OutValue = TEXF_RGB16_S;           return 1; }
	if ( Str==TEXT("TEXF_RGB32")             || Str==TEXT("RGB32")             ) { OutValue = TEXF_RGB32;             return 1; }
	if ( Str==TEXT("TEXF_RGB32_S")           || Str==TEXT("RGB32_S")           ) { OutValue = TEXF_RGB32_S;           return 1; }
	if ( Str==TEXT("TEXF_RGBA8")             || Str==TEXT("RGBA8")             ) { OutValue = TEXF_RGBA8_;            return 1; }
	if ( Str==TEXT("TEXF_RGBA8_S")           || Str==TEXT("RGBA8_S")           ) { OutValue = TEXF_RGBA8_S;           return 1; }

	// Floating point RGBA.
	if ( Str==TEXT("TEXF_R16_F")             || Str==TEXT("R16_F")             ) { OutValue = TEXF_R16_F;             return 1; }
	if ( Str==TEXT("TEXF_R32_F")             || Str==TEXT("R32_F")             ) { OutValue = TEXF_R32_F;             return 1; }
	if ( Str==TEXT("TEXF_RG16_F")            || Str==TEXT("RG16_F")            ) { OutValue = TEXF_RG16_F;            return 1; }
	if ( Str==TEXT("TEXF_RG32_F")            || Str==TEXT("RG32_F")            ) { OutValue = TEXF_RG32_F;            return 1; }
	if ( Str==TEXT("TEXF_RGB16_F")           || Str==TEXT("RGB16_F")           ) { OutValue = TEXF_RGB16_F;           return 1; }
	if ( Str==TEXT("TEXF_RGB32_F")           || Str==TEXT("RGB32_F")           ) { OutValue = TEXF_RGB32_F;           return 1; }
	if ( Str==TEXT("TEXF_RGBA16_F")          || Str==TEXT("RGBA16_F")          ) { OutValue = TEXF_RGBA16_F;          return 1; }
	if ( Str==TEXT("TEXF_RGBA32_F")          || Str==TEXT("RGBA32_F")          ) { OutValue = TEXF_RGBA32_F;          return 1; }

	// ETC1/ETC2/EAC.
	if ( Str==TEXT("TEXF_ETC1")              || Str==TEXT("ETC1")              ) { OutValue = TEXF_ETC1;              return 1; }
	if ( Str==TEXT("TEXF_ETC2")              || Str==TEXT("ETC2")              ) { OutValue = TEXF_ETC2;              return 1; }
	if ( Str==TEXT("TEXF_ETC2_PA")           || Str==TEXT("ETC2_PA")           ) { OutValue = TEXF_ETC2_PA;           return 1; }
	if ( Str==TEXT("TEXF_ETC2_RGB_EAC_A")    || Str==TEXT("ETC2_RGB_EAC_A")    ) { OutValue = TEXF_ETC2_RGB_EAC_A;    return 1; }
	if ( Str==TEXT("TEXF_EAC_R")             || Str==TEXT("EAC_R")             ) { OutValue = TEXF_EAC_R;             return 1; }
	if ( Str==TEXT("TEXF_EAC_R_S")           || Str==TEXT("EAC_R_S")           ) { OutValue = TEXF_EAC_R_S;           return 1; }
	if ( Str==TEXT("TEXF_EAC_RG")            || Str==TEXT("EAC_RG")            ) { OutValue = TEXF_EAC_RG;            return 1; }
	if ( Str==TEXT("TEXF_EAC_RG_S")          || Str==TEXT("EAC_RG_S")          ) { OutValue = TEXF_EAC_RG_S;          return 1; }

	// ASTC.
	if ( Str==TEXT("TEXF_ASTC_4x4")          || Str==TEXT("ASTC_4x4")          ) { OutValue = TEXF_ASTC_4x4;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_5x4")          || Str==TEXT("ASTC_5x4")          ) { OutValue = TEXF_ASTC_5x4;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_5x5")          || Str==TEXT("ASTC_5x5")          ) { OutValue = TEXF_ASTC_5x5;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_6x5")          || Str==TEXT("ASTC_6x5")          ) { OutValue = TEXF_ASTC_6x5;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_6x6")          || Str==TEXT("ASTC_6x6")          ) { OutValue = TEXF_ASTC_6x6;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_8x5")          || Str==TEXT("ASTC_8x5")          ) { OutValue = TEXF_ASTC_8x5;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_8x6")          || Str==TEXT("ASTC_8x6")          ) { OutValue = TEXF_ASTC_8x6;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_8x8")          || Str==TEXT("ASTC_8x8")          ) { OutValue = TEXF_ASTC_8x8;          return 1; }
	if ( Str==TEXT("TEXF_ASTC_10x5")         || Str==TEXT("ASTC_10x5")         ) { OutValue = TEXF_ASTC_10x5;         return 1; }
	if ( Str==TEXT("TEXF_ASTC_10x6")         || Str==TEXT("ASTC_10x6")         ) { OutValue = TEXF_ASTC_10x6;         return 1; }
	if ( Str==TEXT("TEXF_ASTC_10x8")         || Str==TEXT("ASTC_10x8")         ) { OutValue = TEXF_ASTC_10x8;         return 1; }
	if ( Str==TEXT("TEXF_ASTC_10x10")        || Str==TEXT("ASTC_10x10")        ) { OutValue = TEXF_ASTC_10x10;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_12x10")        || Str==TEXT("ASTC_12x10")        ) { OutValue = TEXF_ASTC_12x10;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_12x12")        || Str==TEXT("ASTC_12x12")        ) { OutValue = TEXF_ASTC_12x12;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_3x3x3")        || Str==TEXT("ASTC_3x3x3")        ) { OutValue = TEXF_ASTC_3x3x3;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_4x3x3")        || Str==TEXT("ASTC_4x3x3")        ) { OutValue = TEXF_ASTC_4x3x3;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_4x4x3")        || Str==TEXT("ASTC_4x4x3")        ) { OutValue = TEXF_ASTC_4x4x3;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_4x4x4")        || Str==TEXT("ASTC_4x4x4")        ) { OutValue = TEXF_ASTC_4x4x4;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_5x4x4")        || Str==TEXT("ASTC_5x4x4")        ) { OutValue = TEXF_ASTC_5x4x4;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_5x5x4")        || Str==TEXT("ASTC_5x5x4")        ) { OutValue = TEXF_ASTC_5x5x4;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_5x5x5")        || Str==TEXT("ASTC_5x5x5")        ) { OutValue = TEXF_ASTC_5x5x5;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_6x5x5")        || Str==TEXT("ASTC_6x5x5")        ) { OutValue = TEXF_ASTC_6x5x5;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_6x6x5")        || Str==TEXT("ASTC_6x6x5")        ) { OutValue = TEXF_ASTC_6x6x5;        return 1; }
	if ( Str==TEXT("TEXF_ASTC_6x6x6")        || Str==TEXT("ASTC_6x6x6")        ) { OutValue = TEXF_ASTC_6x6x6;        return 1; }

	// PVRTC.
	if ( Str==TEXT("TEXF_PVRTC1_2BPP")       || Str==TEXT("PVRTC1_2BPP")       ) { OutValue = TEXF_PVRTC1_2BPP;       return 1; }
	if ( Str==TEXT("TEXF_PVRTC1_4BPP")       || Str==TEXT("PVRTC1_4BPP")       ) { OutValue = TEXF_PVRTC1_4BPP;       return 1; }
	if ( Str==TEXT("TEXF_PVRTC2_2BPP")       || Str==TEXT("PVRTC2_2BPP")       ) { OutValue = TEXF_PVRTC2_2BPP;       return 1; }
	if ( Str==TEXT("TEXF_PVRTC2_4BPP")       || Str==TEXT("PVRTC2_4BPP")       ) { OutValue = TEXF_PVRTC2_4BPP;       return 1; }

	// RGBA (Integral).
	if ( Str==TEXT("TEXF_R8_UI")             || Str==TEXT("R8_UI")             ) { OutValue = TEXF_R8_UI;             return 1; }
	if ( Str==TEXT("TEXF_R8_I")              || Str==TEXT("R8_I")              ) { OutValue = TEXF_R8_I;              return 1; }
	if ( Str==TEXT("TEXF_R16_UI")            || Str==TEXT("R16_UI")            ) { OutValue = TEXF_R16_UI;            return 1; }
	if ( Str==TEXT("TEXF_R16_I")             || Str==TEXT("R16_I")             ) { OutValue = TEXF_R16_I;             return 1; }
	if ( Str==TEXT("TEXF_R32_UI")            || Str==TEXT("R32_UI")            ) { OutValue = TEXF_R32_UI;            return 1; }
	if ( Str==TEXT("TEXF_R32_I")             || Str==TEXT("R32_I")             ) { OutValue = TEXF_R32_I;             return 1; }
	if ( Str==TEXT("TEXF_RG8_UI")            || Str==TEXT("RG8_UI")            ) { OutValue = TEXF_RG8_UI;            return 1; }
	if ( Str==TEXT("TEXF_RG8_I")             || Str==TEXT("RG8_I")             ) { OutValue = TEXF_RG8_I;             return 1; }
	if ( Str==TEXT("TEXF_RG16_UI")           || Str==TEXT("RG16_UI")           ) { OutValue = TEXF_RG16_UI;           return 1; }
	if ( Str==TEXT("TEXF_RG16_I")            || Str==TEXT("RG16_I")            ) { OutValue = TEXF_RG16_I;            return 1; }
	if ( Str==TEXT("TEXF_RG32_UI")           || Str==TEXT("RG32_UI")           ) { OutValue = TEXF_RG32_UI;           return 1; }
	if ( Str==TEXT("TEXF_RG32_I")            || Str==TEXT("RG32_I")            ) { OutValue = TEXF_RG32_I;            return 1; }
	if ( Str==TEXT("TEXF_RGB8_UI")           || Str==TEXT("RGB8_UI")           ) { OutValue = TEXF_RGB8_UI;           return 1; }
	if ( Str==TEXT("TEXF_RGB8_I")            || Str==TEXT("RGB8_I")            ) { OutValue = TEXF_RGB8_I;            return 1; }
	if ( Str==TEXT("TEXF_RGB16_UI")          || Str==TEXT("RGB16_UI")          ) { OutValue = TEXF_RGB16_UI;          return 1; }
	if ( Str==TEXT("TEXF_RGB16_I")           || Str==TEXT("RGB16_I")           ) { OutValue = TEXF_RGB16_I;           return 1; }
	if ( Str==TEXT("TEXF_RGB32_UI")          || Str==TEXT("RGB32_UI")          ) { OutValue = TEXF_RGB32_UI;          return 1; }
	if ( Str==TEXT("TEXF_RGB32_I")           || Str==TEXT("RGB32_I")           ) { OutValue = TEXF_RGB32_I;           return 1; }
	if ( Str==TEXT("TEXF_RGBA8_UI")          || Str==TEXT("RGBA8_UI")          ) { OutValue = TEXF_RGBA8_UI;          return 1; }
	if ( Str==TEXT("TEXF_RGBA8_I")           || Str==TEXT("RGBA8_I")           ) { OutValue = TEXF_RGBA8_I;           return 1; }
	if ( Str==TEXT("TEXF_RGBA16_UI")         || Str==TEXT("RGBA16_UI")         ) { OutValue = TEXF_RGBA16_UI;         return 1; }
	if ( Str==TEXT("TEXF_RGBA16_I")          || Str==TEXT("RGBA16_I")          ) { OutValue = TEXF_RGBA16_I;          return 1; }
	if ( Str==TEXT("TEXF_RGBA32_UI")         || Str==TEXT("RGBA32_UI")         ) { OutValue = TEXF_RGBA32_UI;         return 1; }
	if ( Str==TEXT("TEXF_RGBA32_I")          || Str==TEXT("RGBA32_I")          ) { OutValue = TEXF_RGBA32_I;          return 1; }

	// Special.
	if ( Str==TEXT("TEXF_ARGB8")             || Str==TEXT("ARGB8")             ) { OutValue = TEXF_ARGB8;             return 1; }
	if ( Str==TEXT("TEXF_ABGR8")             || Str==TEXT("ABGR8")             ) { OutValue = TEXF_ABGR8;             return 1; }
	if ( Str==TEXT("TEXF_RGB10A2")           || Str==TEXT("RGB10A2")           ) { OutValue = TEXF_RGB10A2;           return 1; }
	if ( Str==TEXT("TEXF_RGB10A2_UI")        || Str==TEXT("RGB10A2_UI")        ) { OutValue = TEXF_RGB10A2_UI;        return 1; }
	if ( Str==TEXT("TEXF_RGB10A2_LM")        || Str==TEXT("RGB10A2_LM")        ) { OutValue = TEXF_RGB10A2_LM;        return 1; }
	if ( Str==TEXT("TEXF_RGB9E5")            || Str==TEXT("RGB9E5")            ) { OutValue = TEXF_RGB9E5;            return 1; }
	if ( Str==TEXT("TEXF_P8_RGB9E5")         || Str==TEXT("P8_RGB9E5")         ) { OutValue = TEXF_P8_RGB9E5;         return 1; }
	if ( Str==TEXT("TEXF_R1")                || Str==TEXT("R1")                ) { OutValue = TEXF_R1;                return 1; }

#if 0
	// For legacy names.
	if ( Str==TEXT("TEXF_RGBA7")             || Str==TEXT("RGBA7")             ) { OutValue = TEXF_BGRA8_LM;          return 1; }
	if ( Str==TEXT("TEXF_DXT1")              || Str==TEXT("DXT1")              ) { OutValue = TEXF_BC1;               return 1; }
	if ( Str==TEXT("TEXF_DXT1_PA")           || Str==TEXT("DXT1_PA")           ) { OutValue = TEXF_BC1_PA;            return 1; }
	if ( Str==TEXT("TEXF_DXT3")              || Str==TEXT("DXT3")              ) { OutValue = TEXF_BC2;               return 1; }
	if ( Str==TEXT("TEXF_DXT5")              || Str==TEXT("DXT5")              ) { OutValue = TEXF_BC3;               return 1; }
#endif

#if 0
	// For Unreal 227 names.
	if ( Str==TEXT("TEXF_RGTC_R")            || Str==TEXT("RGTC_R")            ) { OutValue = TEXF_BC4;               return 1; }
	if ( Str==TEXT("TEXF_RGTC_R_SIGNED")     || Str==TEXT("RGTC_R_SIGNED")     ) { OutValue = TEXF_BC4_S;             return 1; }
	if ( Str==TEXT("TEXF_RGTC_RG")           || Str==TEXT("RGTC_RG")           ) { OutValue = TEXF_BC5;               return 1; }
	if ( Str==TEXT("TEXF_RGTC_RG_SIGNED")    || Str==TEXT("RGTC_RG_SIGNED")    ) { OutValue = TEXF_BC5_S;             return 1; }
	if ( Str==TEXT("TEXF_BPTC_RGBA")         || Str==TEXT("BPTC_RGBA")         ) { OutValue = TEXF_BC7;               return 1; }
	if ( Str==TEXT("TEXF_BPTC_RGB_SF")       || Str==TEXT("BPTC_RGB_SF")       ) { OutValue = TEXF_BC6H_S;            return 1; }
	if ( Str==TEXT("TEXF_BPTC_RGB_UF")       || Str==TEXT("BPTC_RGB_UF")       ) { OutValue = TEXF_BC6H;              return 1; }
#endif

	// MAX. Could be used to reset a outer option I guess.
	if ( Str==TEXT("TEXF_MAX")               || Str==TEXT("MAX")               ) { OutValue = TEXF_MAX;               return 1; }

	// Number
	if ( appIsDigit(Stream[0]) )
	{
		OutValue = (BYTE)appAtoi(Stream);
		return 1;
	}
	return 0;
	unguard;
}

/*----------------------------------------------------------------------------
	Routines encoding format specific data.
----------------------------------------------------------------------------*/

inline UBOOL FIsKnownFormat( BYTE Format )
{
	return Format<=TEXF_MAX;
}

inline UBOOL FIsS3TCFormat( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_BC1:
		case TEXF_BC1_PA:
		case TEXF_BC2:
		case TEXF_BC3:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIsRGTCFormat( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_BC4:
		case TEXF_BC4_S:
		case TEXF_BC5:
		case TEXF_BC5_S:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIsBPTCFormat( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_BC6H:
		case TEXF_BC6H_S:
		case TEXF_BC7:
			return 1;

		default:
			return 0;
	}
}

// Don't like the name ~. Scheme doesn't really match TEXF_ETC2_RGB_EAC_A.
inline UBOOL FIsETCFormat( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_ETC1:
		case TEXF_ETC2:
		case TEXF_ETC2_PA:
		case TEXF_ETC2_RGB_EAC_A:
		case TEXF_EAC_R:
		case TEXF_EAC_R_S:
		case TEXF_EAC_RG:
		case TEXF_EAC_RG_S:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIs2DASTCFormat( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_ASTC_4x4:
		case TEXF_ASTC_5x4:
		case TEXF_ASTC_5x5:
		case TEXF_ASTC_6x5:
		case TEXF_ASTC_6x6:
		case TEXF_ASTC_8x5:
		case TEXF_ASTC_8x6:
		case TEXF_ASTC_8x8:
		case TEXF_ASTC_10x5:
		case TEXF_ASTC_10x6:
		case TEXF_ASTC_10x8:
		case TEXF_ASTC_10x10:
		case TEXF_ASTC_12x10:
		case TEXF_ASTC_12x12:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIs3DASTCFormat( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_ASTC_3x3x3:
		case TEXF_ASTC_4x3x3:
		case TEXF_ASTC_4x4x3:
		case TEXF_ASTC_4x4x4:
		case TEXF_ASTC_5x4x4:
		case TEXF_ASTC_5x5x4:
		case TEXF_ASTC_5x5x5:
		case TEXF_ASTC_6x5x5:
		case TEXF_ASTC_6x6x5:
		case TEXF_ASTC_6x6x6:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIsASTCFormat( BYTE Format )
{
	return FIs2DASTCFormat(Format) || FIs3DASTCFormat(Format);
}

inline UBOOL FIsPVRTC1Format( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_PVRTC1_2BPP:
		case TEXF_PVRTC1_4BPP:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIsPVRTC2Format( BYTE Format )
{
	switch ( Format )
	{
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			return 1;

		default:
			return 0;
	}
}

inline UBOOL FIsPVRTCFormat( BYTE Format )
{
	return FIsPVRTC1Format(Format) || FIsPVRTC2Format(Format);
}

// !! This needs a proper concept.
inline UBOOL FIsCompressedFormat( BYTE Format )
{
	// Edge cases.
	switch ( Format )
	{
		// It kinda is but not.
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return -1;

		// It kinda is, but not at the same time.
		case TEXF_P8:
		case TEXF_P8_RGB9E5:
			return 0;

		// It kinda is a 8x1 block, but afaik some image formats may be able to handle it, etc.
		// Act for now as if it is, and add some transcoder for it later..
		case TEXF_R1:
			return 1;
	}

	return FIsS3TCFormat(Format) || FIsRGTCFormat(Format) || FIsBPTCFormat(Format) || FIsETCFormat(Format) || FIsASTCFormat(Format) || FIsPVRTCFormat(Format);
}

inline UBOOL FIsPalettizedFormat( BYTE Format )
{
	return Format==TEXF_P8 || Format==TEXF_P8_RGB9E5;
}

inline UBOOL FIsMetaFormat( BYTE Format )
{
	switch ( Format )
	{
		// Meta.
		case TEXF_NODATA:
		case TEXF_UNCOMPRESSED:
		case TEXF_UNCOMPRESSED_LOW:
		case TEXF_UNCOMPRESSED_HIGH:
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return 1;

		default:
			return 0;
	}
}

// Does PVRTC allow HDR?
inline UBOOL FIsHighDynamicRangeFormat( BYTE Format )
{
	// ASTC.
	if ( FIsASTCFormat(Format) )
		return 1;

	switch ( Format )
	{
		// Uncompressed.
		case TEXF_R16_F:
		case TEXF_R32_F:
		case TEXF_RG16_F:
		case TEXF_RG32_F:
		case TEXF_RGB16_F:
		case TEXF_RGB32_F:
		case TEXF_RGBA16_F:
		case TEXF_RGBA32_F:
			return 1;

		// BPTC.
		case TEXF_BC6H:
		case TEXF_BC6H_S:
			return 1;

		// Special.
		case TEXF_RGB9E5:
		case TEXF_P8_RGB9E5:
			return 1;

		default:
			return 0;
			break;
	}
}

// Returns true if mind blown.
inline UBOOL FHasFormatHighDynamicRangeAlphaChannel( BYTE Format )
{
	// ASTC.
	if ( FIsASTCFormat(Format) )
		return 1;

	switch ( Format )
	{
		// Uncompressed.
		case TEXF_RGBA16_F:
		case TEXF_RGBA32_F:
			return 1;

		default:
			return 0;
			break;
	}
}

inline UBOOL FHasFormatRedChannel( BYTE Format )
{
	switch ( Format )
	{
		// At least once NoData as a non edge case.
		case TEXF_NODATA:
			return 0;

		// Edge cases as usual.
		case TEXF_UNCOMPRESSED:
		case TEXF_UNCOMPRESSED_LOW:
		case TEXF_UNCOMPRESSED_HIGH:
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return -1;

		// All textures types do otherwise, kinda.
		default:
			return 1;
	}
}

// Do white listing here, as this can be easier extended.
inline UBOOL FHasFormatGreenChannel( BYTE Format )
{
	switch ( Format )
	{
		// Original.
		case TEXF_P8:
		case TEXF_BGRA8_LM:
		case TEXF_R5G6B5:
		case TEXF_BC1:
		case TEXF_RGB8:
		case TEXF_BGRA8:
			return 1;

		// S3TC (continued).
		case TEXF_BC2:
		case TEXF_BC3:
			return 1;

		// RGTC.
		case TEXF_BC5:
		case TEXF_BC5_S:
			return 1;

		// BPTC.
		case TEXF_BC7:
		case TEXF_BC6H_S:
		case TEXF_BC6H:
			return 1;

		// Normalized RGBA.
		case TEXF_RGBA16:
		case TEXF_RGBA16_S:
		case TEXF_RGBA32:
		case TEXF_RGBA32_S:
			return 1;

		// Meta. Edge cases as usual.
		case TEXF_NODATA:
			return 0;
		case TEXF_UNCOMPRESSED:
		case TEXF_UNCOMPRESSED_LOW:
		case TEXF_UNCOMPRESSED_HIGH:
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return -1;

		// S3TC (continued).
		case TEXF_BC1_PA:
			return 1;

		// Normalized RGBA (continued).
		case TEXF_RG8:
		case TEXF_RG8_S:
		case TEXF_RG16:
		case TEXF_RG16_S:
		case TEXF_RG32:
		case TEXF_RG32_S:
		//
		case TEXF_RGB8_S:
		case TEXF_RGB16_:
		case TEXF_RGB16_S:
		case TEXF_RGB32:
		case TEXF_RGB32_S:
		case TEXF_RGBA8_:
		case TEXF_RGBA8_S:
			return 1;

		// Floating point RGBA.
		case TEXF_RG16_F:
		case TEXF_RG32_F:
		case TEXF_RGB16_F:
		case TEXF_RGB32_F:
		case TEXF_RGBA16_F:
		case TEXF_RGBA32_F:
			return 1;

		// ETC1/ETC2/EAC.
		case TEXF_ETC1:
		case TEXF_ETC2:
		case TEXF_ETC2_PA:
		case TEXF_ETC2_RGB_EAC_A:
		case TEXF_EAC_RG:
		case TEXF_EAC_RG_S:
			return 1;

		// ASTC.
		case TEXF_ASTC_4x4:
		case TEXF_ASTC_5x4:
		case TEXF_ASTC_5x5:
		case TEXF_ASTC_6x5:
		case TEXF_ASTC_6x6:
		case TEXF_ASTC_8x5:
		case TEXF_ASTC_8x6:
		case TEXF_ASTC_8x8:
		case TEXF_ASTC_10x5:
		case TEXF_ASTC_10x6:
		case TEXF_ASTC_10x8:
		case TEXF_ASTC_10x10:
		case TEXF_ASTC_12x10:
		case TEXF_ASTC_12x12:
		case TEXF_ASTC_3x3x3:
		case TEXF_ASTC_4x3x3:
		case TEXF_ASTC_4x4x3:
		case TEXF_ASTC_4x4x4:
		case TEXF_ASTC_5x4x4:
		case TEXF_ASTC_5x5x4:
		case TEXF_ASTC_5x5x5:
		case TEXF_ASTC_6x5x5:
		case TEXF_ASTC_6x6x5:
		case TEXF_ASTC_6x6x6:
			return 1;

		// PVRTC.
		case TEXF_PVRTC1_2BPP:
		case TEXF_PVRTC1_4BPP:
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			return 1;

		// RGBA (Integral).
		case TEXF_RG8_UI:
		case TEXF_RG8_I:
		case TEXF_RG16_UI:
		case TEXF_RG16_I:
		case TEXF_RG32_UI:
		case TEXF_RG32_I:
		case TEXF_RGB8_UI:
		case TEXF_RGB8_I:
		case TEXF_RGB16_UI:
		case TEXF_RGB16_I:
		case TEXF_RGB32_UI:
		case TEXF_RGB32_I:
		case TEXF_RGBA8_UI:
		case TEXF_RGBA8_I:
		case TEXF_RGBA16_UI:
		case TEXF_RGBA16_I:
		case TEXF_RGBA32_UI:
		case TEXF_RGBA32_I:
			return 1;

		// Special.
		case TEXF_ARGB8:
		case TEXF_ABGR8:
		case TEXF_RGB10A2:
		case TEXF_RGB10A2_UI:
		case TEXF_RGB10A2_LM:
		case TEXF_RGB9E5:
		case TEXF_P8_RGB9E5:
			return 1;

		// I guess no?
		default:
			return 0;
			break;
	}
}

inline UBOOL FHasFormatBlueChannel( BYTE Format )
{
	switch ( Format )
	{
		// Original.
		case TEXF_P8:
		case TEXF_BGRA8_LM:
		case TEXF_R5G6B5:
		case TEXF_BC1:
		case TEXF_RGB8:
		case TEXF_BGRA8:
			return 1;

		// S3TC (continued).
		case TEXF_BC2:
		case TEXF_BC3:
			return 1;

		// BPTC.
		case TEXF_BC7:
		case TEXF_BC6H_S:
		case TEXF_BC6H:
			return 1;

		// Normalized RGBA.
		case TEXF_RGBA16:
		case TEXF_RGBA16_S:
		case TEXF_RGBA32:
		case TEXF_RGBA32_S:
			return 1;

		// Meta. Edge cases as usual.
		case TEXF_NODATA:
			return 0;
		case TEXF_UNCOMPRESSED:
		case TEXF_UNCOMPRESSED_LOW:
		case TEXF_UNCOMPRESSED_HIGH:
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return -1;

		// S3TC (continued).
		case TEXF_BC1_PA:
			return 1;

		// Normalized RGBA (continued).
		//
		case TEXF_RGB8_S:
		case TEXF_RGB16_:
		case TEXF_RGB16_S:
		case TEXF_RGB32:
		case TEXF_RGB32_S:
		case TEXF_RGBA8_:
		case TEXF_RGBA8_S:
			return 1;

		// Floating point RGBA.
		case TEXF_RGB16_F:
		case TEXF_RGB32_F:
		case TEXF_RGBA16_F:
		case TEXF_RGBA32_F:
			return 1;

		// ETC1/ETC2/EAC.
		case TEXF_ETC1:
		case TEXF_ETC2:
		case TEXF_ETC2_PA:
		case TEXF_ETC2_RGB_EAC_A:
			return 1;

		// ASTC.
		case TEXF_ASTC_4x4:
		case TEXF_ASTC_5x4:
		case TEXF_ASTC_5x5:
		case TEXF_ASTC_6x5:
		case TEXF_ASTC_6x6:
		case TEXF_ASTC_8x5:
		case TEXF_ASTC_8x6:
		case TEXF_ASTC_8x8:
		case TEXF_ASTC_10x5:
		case TEXF_ASTC_10x6:
		case TEXF_ASTC_10x8:
		case TEXF_ASTC_10x10:
		case TEXF_ASTC_12x10:
		case TEXF_ASTC_12x12:
		case TEXF_ASTC_3x3x3:
		case TEXF_ASTC_4x3x3:
		case TEXF_ASTC_4x4x3:
		case TEXF_ASTC_4x4x4:
		case TEXF_ASTC_5x4x4:
		case TEXF_ASTC_5x5x4:
		case TEXF_ASTC_5x5x5:
		case TEXF_ASTC_6x5x5:
		case TEXF_ASTC_6x6x5:
		case TEXF_ASTC_6x6x6:
			return 1;

		// PVRTC.
		case TEXF_PVRTC1_2BPP:
		case TEXF_PVRTC1_4BPP:
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			return 1;

		// RGBA (Integral).
		case TEXF_RGB8_UI:
		case TEXF_RGB8_I:
		case TEXF_RGB16_UI:
		case TEXF_RGB16_I:
		case TEXF_RGB32_UI:
		case TEXF_RGB32_I:
		case TEXF_RGBA8_UI:
		case TEXF_RGBA8_I:
		case TEXF_RGBA16_UI:
		case TEXF_RGBA16_I:
		case TEXF_RGBA32_UI:
		case TEXF_RGBA32_I:
			return 1;

		// Special.
		case TEXF_ARGB8:
		case TEXF_ABGR8:
		case TEXF_RGB10A2:
		case TEXF_RGB10A2_UI:
		case TEXF_RGB10A2_LM:
		case TEXF_RGB9E5:
		case TEXF_P8_RGB9E5:
			return 1;

		// I guess no?
		default:
			return 0;
			break;
	}
}

inline UBOOL FHasFormatAlphaChannel( BYTE Format )
{
	switch ( Format )
	{
		// Original.
		case TEXF_P8:
		case TEXF_BGRA8_LM:
		case TEXF_BGRA8:
			return 1;

		// S3TC (continued).
		case TEXF_BC2:
		case TEXF_BC3:
			return 1;

		// BPTC.
		case TEXF_BC7:
			return 1;

		// Normalized RGBA.
		case TEXF_RGBA16:
		case TEXF_RGBA16_S:
		case TEXF_RGBA32:
		case TEXF_RGBA32_S:
			return 1;

		// Meta. Edge cases as usual.
		case TEXF_NODATA:
			return 0;
		case TEXF_UNCOMPRESSED:
		case TEXF_UNCOMPRESSED_LOW:
		case TEXF_UNCOMPRESSED_HIGH:
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return -1;

		// S3TC (continued).
		case TEXF_BC1_PA:
			return 1;

		// Normalized RGBA (continued).
		case TEXF_RGBA8_:
		case TEXF_RGBA8_S:
			return 1;

		// Floating point RGBA.
		case TEXF_RGBA16_F:
		case TEXF_RGBA32_F:
			return 1;

		// ETC1/ETC2/EAC.
		case TEXF_ETC2_PA:
		case TEXF_ETC2_RGB_EAC_A:
			return 1;

		// ASTC.
		case TEXF_ASTC_4x4:
		case TEXF_ASTC_5x4:
		case TEXF_ASTC_5x5:
		case TEXF_ASTC_6x5:
		case TEXF_ASTC_6x6:
		case TEXF_ASTC_8x5:
		case TEXF_ASTC_8x6:
		case TEXF_ASTC_8x8:
		case TEXF_ASTC_10x5:
		case TEXF_ASTC_10x6:
		case TEXF_ASTC_10x8:
		case TEXF_ASTC_10x10:
		case TEXF_ASTC_12x10:
		case TEXF_ASTC_12x12:
		case TEXF_ASTC_3x3x3:
		case TEXF_ASTC_4x3x3:
		case TEXF_ASTC_4x4x3:
		case TEXF_ASTC_4x4x4:
		case TEXF_ASTC_5x4x4:
		case TEXF_ASTC_5x5x4:
		case TEXF_ASTC_5x5x5:
		case TEXF_ASTC_6x5x5:
		case TEXF_ASTC_6x6x5:
		case TEXF_ASTC_6x6x6:
			return 1;

		// PVRTC.
		case TEXF_PVRTC1_2BPP:
		case TEXF_PVRTC1_4BPP:
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			return 1;

		// RGBA (Integral).
		case TEXF_RGBA8_UI:
		case TEXF_RGBA8_I:
		case TEXF_RGBA16_UI:
		case TEXF_RGBA16_I:
		case TEXF_RGBA32_UI:
		case TEXF_RGBA32_I:
			return 1;

		// Special.
		case TEXF_ARGB8:
		case TEXF_ABGR8:
		case TEXF_RGB10A2:
		case TEXF_RGB10A2_UI:
		case TEXF_RGB10A2_LM:
			return 1;

		// I guess no?
		default:
			return 0;
			break;
	}
}

inline UBOOL FIsRedFormat( BYTE Format )
{
	return FHasFormatRedChannel(Format) && !FHasFormatGreenChannel(Format) && !FHasFormatBlueChannel(Format) && !FHasFormatAlphaChannel(Format);
}
inline UBOOL FIsRedGreenFormat( BYTE Format )
{
	return FHasFormatRedChannel(Format) && FHasFormatGreenChannel(Format) && !FHasFormatBlueChannel(Format) && !FHasFormatAlphaChannel(Format);
}
inline UBOOL FIsRedGreenBlueFormat( BYTE Format )
{
	return FHasFormatRedChannel(Format) && FHasFormatGreenChannel(Format) && FHasFormatBlueChannel(Format) && !FHasFormatAlphaChannel(Format);
}
inline UBOOL FIsRGBAFormat( BYTE Format )
{
	return FHasFormatRedChannel(Format) && FHasFormatGreenChannel(Format) && FHasFormatBlueChannel(Format) && FHasFormatAlphaChannel(Format);
}

inline UBOOL FIsFormatSigned( BYTE Format )
{
	switch ( Format )
	{
		// RGTC.
		case TEXF_BC4_S:
		case TEXF_BC5_S:
			return 1;

		// BPTC.
		case TEXF_BC6H_S:
			return 1;

		// RGBA.
		case TEXF_RGBA16_S:
		case TEXF_RGBA32_S:
			return 1;

		// Meta. I hate edge cases.
		case TEXF_NODATA:
			return 0;
		case TEXF_UNCOMPRESSED:
		case TEXF_UNCOMPRESSED_LOW:
		case TEXF_UNCOMPRESSED_HIGH:
		case TEXF_COMPRESSED:
		case TEXF_COMPRESSED_LOW:
		case TEXF_COMPRESSED_HIGH:
			return -1;

			// RGBA.
		case TEXF_R8_S:
		case TEXF_R16_S:
		case TEXF_R32_S:
		case TEXF_RG8_S:
		case TEXF_RG16_S:
		case TEXF_RG32_S:
		case TEXF_RGB8_S:
		case TEXF_RGB16_S:
		case TEXF_RGB32_S:
		case TEXF_RGBA8_S:
			return 1;

		// RGBA FLOAT.
		case TEXF_R16_F:
		case TEXF_R32_F:
		case TEXF_RG16_F:
		case TEXF_RG32_F:
		case TEXF_RGB16_F:
		case TEXF_RGB32_F:
		case TEXF_RGBA16_F:
		case TEXF_RGBA32_F:
			return 1;

		// ETC1/ETC2/EAC.
		case TEXF_EAC_R_S:
		case TEXF_EAC_RG_S:
			return 1;

		// ASTC.
		case TEXF_ASTC_4x4: //
		case TEXF_ASTC_5x4: // ASTC spec is tough,
		case TEXF_ASTC_5x5: // but if I understand
		case TEXF_ASTC_6x5: // it correctly, it can
		case TEXF_ASTC_6x6: // not contain signed
		case TEXF_ASTC_8x5: // data. --han
		case TEXF_ASTC_8x6: //
		case TEXF_ASTC_8x8:
		case TEXF_ASTC_10x5:
		case TEXF_ASTC_10x6:
		case TEXF_ASTC_10x8:
		case TEXF_ASTC_10x10:
		case TEXF_ASTC_12x10:
		case TEXF_ASTC_12x12:
		case TEXF_ASTC_3x3x3:
		case TEXF_ASTC_4x3x3:
		case TEXF_ASTC_4x4x3:
		case TEXF_ASTC_4x4x4:
		case TEXF_ASTC_5x4x4:
		case TEXF_ASTC_5x5x4:
		case TEXF_ASTC_5x5x5:
		case TEXF_ASTC_6x5x5:
		case TEXF_ASTC_6x6x5:
		case TEXF_ASTC_6x6x6:
			return 0;

		// PVRTC.
		case TEXF_PVRTC1_2BPP:
		case TEXF_PVRTC1_4BPP:
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			return -9999; // I have no clue.

		// I guess no?
		default:
			return 0;
			break;
	}
}

//
// Special four channel orders.
//
inline UBOOL FIsFormatBGRAOrdered( BYTE Format )
{
	switch ( Format )
	{
		// Uncompressed.
		case TEXF_BGRA8:
		case TEXF_BGRA8_LM:
			return 1;
			break;

		default:
			return 0;
			break;
	}
}
inline UBOOL FIsFormatARGBOrdered( BYTE Format )
{
	return Format==TEXF_ARGB8;
}
inline UBOOL FIsFormatABGROrdered( BYTE Format )
{
	return Format==TEXF_ABGR8;
}
// Do some FIsFormatRGBAOrdered?

inline INT FTextureBlockWidth( BYTE Format )
{
	switch ( Format )
	{
		// S3TC.
		case TEXF_BC1:            return  4;
		case TEXF_BC1_PA:         return  4;
		case TEXF_BC2:            return  4;
		case TEXF_BC3:            return  4;

		// RGTC.
		case TEXF_BC4:            return  4;
		case TEXF_BC4_S:          return  4;
		case TEXF_BC5:            return  4;
		case TEXF_BC5_S:          return  4;

		// BPTC.
		case TEXF_BC6H:           return  4;
		case TEXF_BC6H_S:         return  4;
		case TEXF_BC7:            return  4;

		// ETC1/ETC2/EAC.
		case TEXF_ETC1:           return  4;
		case TEXF_ETC2:           return  4;
		case TEXF_ETC2_PA:        return  4;
		case TEXF_ETC2_RGB_EAC_A: return  4;
		case TEXF_EAC_R:          return  4;
		case TEXF_EAC_R_S:        return  4;
		case TEXF_EAC_RG:         return  4;
		case TEXF_EAC_RG_S:       return  4;

		// ASTC.
		case TEXF_ASTC_4x4:       return  4;
		case TEXF_ASTC_5x4:       return  5;
		case TEXF_ASTC_5x5:       return  5;
		case TEXF_ASTC_6x5:       return  6;
		case TEXF_ASTC_6x6:       return  6;
		case TEXF_ASTC_8x5:       return  8;
		case TEXF_ASTC_8x6:       return  8;
		case TEXF_ASTC_8x8:       return  8;
		case TEXF_ASTC_10x5:      return 10;
		case TEXF_ASTC_10x6:      return 10;
		case TEXF_ASTC_10x8:      return 10;
		case TEXF_ASTC_10x10:     return 10;
		case TEXF_ASTC_12x10:     return 12;
		case TEXF_ASTC_12x12:     return 12;
		case TEXF_ASTC_3x3x3:     return  3;
		case TEXF_ASTC_4x3x3:     return  4;
		case TEXF_ASTC_4x4x3:     return  4;
		case TEXF_ASTC_4x4x4:     return  4;
		case TEXF_ASTC_5x4x4:     return  5;
		case TEXF_ASTC_5x5x4:     return  5;
		case TEXF_ASTC_5x5x5:     return  5;
		case TEXF_ASTC_6x5x5:     return  6;
		case TEXF_ASTC_6x6x5:     return  6;
		case TEXF_ASTC_6x6x6:     return  6;

		// PVRTC.
		case TEXF_PVRTC1_2BPP: // !! FIX-ME !!
		case TEXF_PVRTC1_4BPP:
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			check(0);
			return -1;

		// Straight Wacker.
		case TEXF_R1:             return  8;

		// !! Edge cases, can't think of how to handle them best. --han
		case TEXF_NODATA:
		case TEXF_UNCOMPRESSED:
		case TEXF_COMPRESSED:
			return 0;
			break;

		// Uncompressed.
		default:
			return 1;
			break;
	}
}

//
// For currently defined texture formats always the same as Width.
// However there are texture formats, such as ASTC, in the wild,
// which do offer anistropic block sizes. --han
//
inline INT FTextureBlockHeight( BYTE Format )
{
	//
	// Currently only ASTC Block sizes aren't guaranteed to be isotropic on the plane,
	// but suddenly a wild R1 appears.
	//
	switch ( Format )
	{
		// ASTC.
		case TEXF_ASTC_4x4:   return  4;
		case TEXF_ASTC_5x4:   return  4;
		case TEXF_ASTC_5x5:   return  5;
		case TEXF_ASTC_6x5:   return  5;
		case TEXF_ASTC_6x6:   return  6;
		case TEXF_ASTC_8x5:   return  5;
		case TEXF_ASTC_8x6:   return  6;
		case TEXF_ASTC_8x8:   return  8;
		case TEXF_ASTC_10x5:  return  5;
		case TEXF_ASTC_10x6:  return  6;
		case TEXF_ASTC_10x8:  return  8;
		case TEXF_ASTC_10x10: return 10;
		case TEXF_ASTC_12x10: return 10;
		case TEXF_ASTC_12x12: return 12;
		case TEXF_ASTC_3x3x3: return  3;
		case TEXF_ASTC_4x3x3: return  3;
		case TEXF_ASTC_4x4x3: return  4;
		case TEXF_ASTC_4x4x4: return  4;
		case TEXF_ASTC_5x4x4: return  4;
		case TEXF_ASTC_5x5x4: return  5;
		case TEXF_ASTC_5x5x5: return  5;
		case TEXF_ASTC_6x5x5: return  5;
		case TEXF_ASTC_6x6x5: return  6;
		case TEXF_ASTC_6x6x6: return  6;

		// Straight Wacker.
		case TEXF_R1:         return  1;
	}

	// Remaining formats are isotropic in plane.
	return FTextureBlockWidth( Format );
}

inline INT FTextureBlockDepth( BYTE Format )
{
	// Currently only ASTC offers true 3D block compression.
	switch ( Format )
	{
		// 3D ASTC.
		case TEXF_ASTC_3x3x3: return 3;
		case TEXF_ASTC_4x3x3: return 3;
		case TEXF_ASTC_4x4x3: return 3;
		case TEXF_ASTC_4x4x4: return 4;
		case TEXF_ASTC_5x4x4: return 4;
		case TEXF_ASTC_5x5x4: return 4;
		case TEXF_ASTC_5x5x5: return 5;
		case TEXF_ASTC_6x5x5: return 5;
		case TEXF_ASTC_6x6x5: return 5;
		case TEXF_ASTC_6x6x6: return 6;
	}

	return FIsMetaFormat(Format) ? 0 : 1; // !! I don't return 0 here for Width/Height.
}

// Storage requirement in bytes for a BlockWidth x BlockHeight sized chunk.
inline INT FTextureBlockBytes( BYTE Format )
{
	// Fun...
	switch ( Format )
	{
		// Original.
		case TEXF_P8:                return  1;
		case TEXF_BGRA8_LM:          return  4;
		case TEXF_R5G6B5:            return  2;
		case TEXF_BC1:               return  8;
		case TEXF_RGB8:              return  3;
		case TEXF_BGRA8:             return  4;

		// S3TC (continued).
		case TEXF_BC2:               return 16;
		case TEXF_BC3:               return 16;

		// RGTC.
		case TEXF_BC4:               return  8;
		case TEXF_BC4_S:             return  8;
		case TEXF_BC5:               return 16;
		case TEXF_BC5_S:             return 16;

		// BPTC.
		case TEXF_BC7:               return 16;
		case TEXF_BC6H_S:            return 16;
		case TEXF_BC6H:              return 16;

		// RGBA.
		case TEXF_RGBA16:            return  8;
		case TEXF_RGBA16_S:          return  8;
		case TEXF_RGBA32:            return 16;
		case TEXF_RGBA32_S:          return 16;

		// Meta.
		case TEXF_NODATA:            return  0; // Technically yes.
		case TEXF_UNCOMPRESSED:      return -1; // !! aligns with values for error or unknown format.
		case TEXF_UNCOMPRESSED_LOW:  return -1; // !! aligns with values for error or unknown format.
		case TEXF_UNCOMPRESSED_HIGH: return -1; // !! aligns with values for error or unknown format.
		case TEXF_COMPRESSED:        return -1; // !! I have no idea what to do here, but -1
		case TEXF_COMPRESSED_LOW:    return -1; // !! I have no idea what to do here, but -1
		case TEXF_COMPRESSED_HIGH:   return -1; // !! I have no idea what to do here, but -1

		// S3TC (continued).
		case TEXF_BC1_PA:            return  8;

		// Normalized RGBA (continued).
		case TEXF_R8:                return  1;
		case TEXF_R8_S:              return  1;
		case TEXF_R16:               return  2;
		case TEXF_R16_S:             return  2;
		case TEXF_R32:               return  4;
		case TEXF_R32_S:             return  4;
		case TEXF_RG8:               return  2;
		case TEXF_RG8_S:             return  2;
		case TEXF_RG16:              return  4;
		case TEXF_RG16_S:            return  4;
		case TEXF_RG32:              return  8;
		case TEXF_RG32_S:            return  8;
		//
		case TEXF_RGB8_S:            return  3;
		case TEXF_RGB16_:            return  6;
		case TEXF_RGB16_S:           return  6;
		case TEXF_RGB32:             return 12;
		case TEXF_RGB32_S:           return 12;
		case TEXF_RGBA8_:            return  4;
		case TEXF_RGBA8_S:           return  4;

		// Floating point RGBA.
		case TEXF_R16_F:             return  2;
		case TEXF_R32_F:             return  4;
		case TEXF_RG16_F:            return  4;
		case TEXF_RG32_F:            return  8;
		case TEXF_RGB16_F:           return  6;
		case TEXF_RGB32_F:           return 12;
		case TEXF_RGBA16_F:          return  8;
		case TEXF_RGBA32_F:          return 16;

		// ETC1/ETC2/EAC.
		case TEXF_ETC1:              return  8;
		case TEXF_ETC2:              return  8;
		case TEXF_ETC2_PA:           return  8;
		case TEXF_ETC2_RGB_EAC_A:    return 16;
		case TEXF_EAC_R:             return  8;
		case TEXF_EAC_R_S:           return  8;
		case TEXF_EAC_RG:            return 16;
		case TEXF_EAC_RG_S:          return 16;

		// ASTC.
		case TEXF_ASTC_4x4:          return 16;
		case TEXF_ASTC_5x4:          return 16;
		case TEXF_ASTC_5x5:          return 16;
		case TEXF_ASTC_6x5:          return 16;
		case TEXF_ASTC_6x6:          return 16;
		case TEXF_ASTC_8x5:          return 16;
		case TEXF_ASTC_8x6:          return 16;
		case TEXF_ASTC_8x8:          return 16;
		case TEXF_ASTC_10x5:         return 16;
		case TEXF_ASTC_10x6:         return 16;
		case TEXF_ASTC_10x8:         return 16;
		case TEXF_ASTC_10x10:        return 16;
		case TEXF_ASTC_12x10:        return 16;
		case TEXF_ASTC_12x12:        return 16;
		case TEXF_ASTC_3x3x3:        return 16;
		case TEXF_ASTC_4x3x3:        return 16;
		case TEXF_ASTC_4x4x3:        return 16;
		case TEXF_ASTC_4x4x4:        return 16;
		case TEXF_ASTC_5x4x4:        return 16;
		case TEXF_ASTC_5x5x4:        return 16;
		case TEXF_ASTC_5x5x5:        return 16;
		case TEXF_ASTC_6x5x5:        return 16;
		case TEXF_ASTC_6x6x5:        return 16;
		case TEXF_ASTC_6x6x6:        return 16;

		// PVRTC.
		case TEXF_PVRTC1_2BPP: // !! FIX-ME !!
		case TEXF_PVRTC1_4BPP:
		case TEXF_PVRTC2_2BPP:
		case TEXF_PVRTC2_4BPP:
			check(0);
			return -1;

		// RGBA (Integral).
		case TEXF_R8_UI:             return  1;
		case TEXF_R8_I:              return  1;
		case TEXF_R16_UI:            return  2;
		case TEXF_R16_I:             return  2;
		case TEXF_R32_UI:            return  4;
		case TEXF_R32_I:             return  4;
		case TEXF_RG8_UI:            return  2;
		case TEXF_RG8_I:             return  2;
		case TEXF_RG16_UI:           return  4;
		case TEXF_RG16_I:            return  4;
		case TEXF_RG32_UI:           return  8;
		case TEXF_RG32_I:            return  8;
		case TEXF_RGB8_UI:           return  3;
		case TEXF_RGB8_I:            return  3;
		case TEXF_RGB16_UI:          return  6;
		case TEXF_RGB16_I:           return  6;
		case TEXF_RGB32_UI:          return 12;
		case TEXF_RGB32_I:           return 12;
		case TEXF_RGBA8_UI:          return  4;
		case TEXF_RGBA8_I:           return  4;
		case TEXF_RGBA16_UI:         return  8;
		case TEXF_RGBA16_I:          return  8;
		case TEXF_RGBA32_UI:         return 16;
		case TEXF_RGBA32_I:          return 16;

		// Special.
		case TEXF_ARGB8:             return  4;
		case TEXF_ABGR8:             return  4;
		case TEXF_RGB10A2:           return  4;
		case TEXF_RGB10A2_UI:        return  4;
		case TEXF_RGB10A2_LM:        return  4;
		case TEXF_RGB9E5:            return  4;
		case TEXF_P8_RGB9E5:         return  1;
		case TEXF_R1:                return  1;

		// Unknown.
		default:                     return -1;
	}
}

//
// Some compression formats such as BC7 are a bit exact encoding
// for a specific format, while others such as BC1-5 are not
// defined in this way, so it can be decompressed to different
// formats with various size and quality tradeoffs.
//
// One example is that RGBA8 is enough for BC1, however for
// BC3 the alpha channel can encode data at a significant
// higher precessions as 8 bit, so for exporting the
// texture data RGBA16 is recommended, while for fallback
// rendering support RGBA8 is.
//
// Treat P8 as a compressed format here, and return
// the data type stored inside the Palette.
//
// Export=1 selects desired format for export, which may have
// an alignment (due to dropping of alpha channel) which
// may be unsuitable for rendering use.
//
// Export=0 will return a format which is recommended
// as a fallback in case hardware decompression is not
// available. This may include noticable quality loss,
// for example when normals are stored in BC5 format.
//
// Green/Blue/Alpha=1 will assume the specific channel
// contains relevant data.
//
// Green/Blue/Alpha=0 will assume the specific channel
// won't contain relevant data and may choose a format
// without alpha.
//
// HDR=1 will assume that HDR might be present in mixed
// LDR/HDR formats such as the ASTC ones.
//
// HDR=0 assume LDR only.
//
// ASTC decoding depends on whether it is treated as sRGB or
// non sRGB format, which may influence our choice of formats.
//
// I'm planing on adding helper to UnTexComp.h to analyze ASTC
// blocks for whatever chunks and/or data is included. --han
//
// !! I want some FIsFormatNormalized thing... but ASTC falls
// !! out of the box here again... kinda.
//
inline BYTE FRecommendDecompressedFormat( BYTE Format, UBOOL Export=1, UBOOL Green=1, UBOOL Blue=1, UBOOL Alpha=1, UBOOL Signed=1, UBOOL HDR=1, UBOOL sRGB=1 ) // Do I need a signed for ASTC here as well?!?!?!
{
	guard(FRecommendDecompressedFormat);

	// Handle a few edgy cases.
	switch ( Format )
	{
		// We kinda do handle it here, but I don't want to rely
		// on FIsCompressedFormat in this edge case.
		case TEXF_P8:
			break;

		// Again, not rely on FIsCompressedFormat behaviour. Note that
		// some image formats may actually support 1 bit per pixel.
		case TEXF_R1:
			return TEXF_R8;

		// This is so meta. One day somebody will look at these lines
		// after spending three days desperatly chasing a bug. --han
		case TEXF_COMPRESSED:
			return TEXF_UNCOMPRESSED;
		case TEXF_COMPRESSED_LOW:
			return TEXF_UNCOMPRESSED_LOW;
		case TEXF_COMPRESSED_HIGH:
			return TEXF_UNCOMPRESSED_HIGH;

		// Filter out all unknown, other uncompressed and meta formats.
		default:
			if ( !FIsCompressedFormat(Format) || FIsMetaFormat(Format) || !FIsKnownFormat(Format) )
				return Format;
	}

	// Downgrade if possible.
	Green  = Green  && FHasFormatGreenChannel(Format);
	Blue   = Blue   && FHasFormatBlueChannel(Format);
	Alpha  = Alpha  && FHasFormatAlphaChannel(Format);
	Signed = Signed && FIsFormatSigned(Format);
	HDR    = HDR    && FIsHighDynamicRangeFormat(Format);

	// Any sandwiched optional channels will be ignored (e.g. Green not required but Blue and/or Alpha present, etc.).
	INT RequiredChannels = Alpha ? 4 : Blue ? 3 : Green ? 2 : 1;

	// Precalculate desired Formats for specific precissions, signs, etc. regardless of we may need them for the current Format.
	BYTE Format8    = RequiredChannels==4 ? TEXF_RGBA8_   : RequiredChannels==3 ? (Export ? TEXF_RGB8    : TEXF_RGBA8_  ) : RequiredChannels==2 ? TEXF_RG8    : TEXF_R8;
	BYTE Format8_S  = RequiredChannels==4 ? TEXF_RGBA8_S  : RequiredChannels==3 ? (Export ? TEXF_RGB8_S  : TEXF_RGBA8_S ) : RequiredChannels==2 ? TEXF_RG8_S  : TEXF_R8_S;
	BYTE Format16   = RequiredChannels==4 ? TEXF_RGBA16   : RequiredChannels==3 ? (Export ? TEXF_RGB16_  : TEXF_RGBA16  ) : RequiredChannels==2 ? TEXF_RG16   : TEXF_R16;
	BYTE Format16_S = RequiredChannels==4 ? TEXF_RGBA16_S : RequiredChannels==3 ? (Export ? TEXF_RGB16_S : TEXF_RGBA16_S) : RequiredChannels==2 ? TEXF_RG16_S : TEXF_R16_S;
	BYTE Format16_F = RequiredChannels==4 ? TEXF_RGBA16_F : RequiredChannels==3 ? (Export ? TEXF_RGB16_F : TEXF_RGBA16_F) : RequiredChannels==2 ? TEXF_RG16_F : TEXF_R16_F;

	// The new World, where you can hide everything from HDR data to cats.
	if ( FIsASTCFormat(Format) )
	{
		return HDR ? Format16_F : Export ? Format16 : Format8;
	}

	// !! Can has HDR? Can has depths? Can has Blue?
	if ( FIsPVRTCFormat(Format) )
	{
		return TEXF_RGBA8_;
	}

	//
	// The old World, where everything is (half way) in order.
	//
	switch ( Format )
	{
		case TEXF_P8:             return                                Format8;    // Data is stored at the same precission inside the Palette.
		case TEXF_BC1:            return Export          ? Format16   : Format8;    // Non bit exact encoding. Format16 is probably overkill.
		case TEXF_BC1_PA:         return Export          ? Format16   : Format8;    // Non bit exact encoding. Format16 is probably overkill.
		case TEXF_BC2:            return Export          ? Format16   : Format8;    // Non bit exact encoding. Format16 is probably overkill.
		case TEXF_BC3:            return Export          ? Format16   : Format8;    // Non bit exact encoding. Format16 is justified in any case for Alpha.
		case TEXF_BC4:            return Export          ? Format16   : Format8;    // Non bit exact encoding?
		case TEXF_BC4_S:          return Export          ? Format16_S : Format8_S;  // Non bit exact encoding?
		case TEXF_BC5:            return Export          ? Format16   : Format8;    // Non bit exact encoding? Can contain normals, so Format16 at runtime could be considered.
		case TEXF_BC5_S:          return Export          ? Format16_S : Format8_S;  // Non bit exact encoding?
		case TEXF_BC6H:           return                                Format16_F; // Bit exact encoding.
		case TEXF_BC6H_S:         return                                Format16_F; // Bit exact encoding.
		case TEXF_BC7:            return                                Format8;    // Bit exact encoding.
		case TEXF_ETC1:           return                                Format8;    // It sounds like a bit exact 8 bit encoding in Khronos spec?
		case TEXF_ETC2:           return                                Format8;    // It sounds like a bit exact 8 bit encoding in Khronos spec?
		case TEXF_ETC2_PA:        return                                Format8;    // It sounds like a bit exact 8 bit encoding in Khronos spec?
		case TEXF_ETC2_RGB_EAC_A: return Export && Alpha ? Format16   : Format8;    // Alpha has 11 bits, but without alpha it seems like bit exact for 8 bit?
		case TEXF_EAC_R:          return Export          ? Format16   : Format8;    // Has exact 11 bit representation, but extends to 16 bit in well defined way.
		case TEXF_EAC_R_S:        return Export          ? Format16_S : Format8_S;  // Has exact 11 bit representation, but extends to 16 bit in well defined way.
		case TEXF_EAC_RG:         return Export          ? Format16   : Format8;    // Has exact 11 bit representation, but extends to 16 bit in well defined way.
		case TEXF_EAC_RG_S:       return Export          ? Format16_S : Format8_S;  // Has exact 11 bit representation, but extends to 16 bit in well defined way.
	}

	//
	// We filter out unknown and non compresed formats at the top.
	// Reaching this point implies that we missed handling a
	// compressed texture format.
	//
	checkSlow(0);
	return Format;
	unguard;
}

/*----------------------------------------------------------------------------
	Routines making use of format specific encoding.
----------------------------------------------------------------------------*/

//
// Consult PixelStorei documentation. Is pointer alignment even an issue?
//
inline INT FTextureUnpackAlignment( BYTE Format, const void* Data=NULL )
{
	// P8 may be 1 byte per pixel, however this corresponds
	// with a RGBA8 entry inside the Palette.
	if ( Format==TEXF_P8 )
		return FTextureUnpackAlignment( TEXF_RGBA8_, Data );

	INT Bits = (PTRINT(Data)&7) | (FTextureBlockBytes(Format)&7);

	return (Bits&1) ? 1 : (Bits&2) ? 2 : (Bits&4) ? 4 : 8;
}

//
// Return type is floating point, there are texture formats, such as ASTC,
// in the wild which have non integer bpp. However this function should
// never be used for anything more than just display purposes either way,
// so it doesn't really hurt.
//
// FTextureBytes can be used to calculate the actual required storage sizes.
//
inline FLOAT FBitsPerPixel( BYTE Format )
{
	return 8.0f*FTextureBlockBytes(Format)/(FTextureBlockWidth(Format)*FTextureBlockHeight(Format)*FTextureBlockDepth(Format));
}

//
// Alignes texture dimensions to format specific block sizes.
// This is often called Pitch.
//
// Examples:
//  * BC1 11x3 texture would be aligned to 12x4
//  * R8  11x3 texture would be aligned to 11x3
//
inline INT FTextureBlockAlignedWidth( BYTE Format, INT Width )
{
	INT BlockWidth = FTextureBlockWidth( Format );
	return ((Width+BlockWidth-1)&~(BlockWidth-1));
}
inline INT FTextureBlockAlignedHeight( BYTE Format, INT Height )
{
	INT BlockHeight = FTextureBlockHeight( Format );
	return ((Height+BlockHeight-1)&~(BlockHeight-1));
}
inline INT FTextureBlockAlignedDepth( BYTE Format, INT Depth )
{
	INT BlockDepth = FTextureBlockDepth( Format );
	return ((Depth+BlockDepth-1)&~(BlockDepth-1));
}

//
// Calculates specific storage size for a particular texture.
//
// Result is 64 bit, as otherwise 2 GB textures would be max,
// which is uncomfortable close to what one might want to be doing.
//
inline SQWORD FTextureBytes( BYTE Format, INT Width, INT Height=1, INT Depth=1 )
{
	if ( FIsMetaFormat(Format) )
		return 0;

	SQWORD Result = FTextureBlockBytes(Format);

	Result *= FTextureBlockAlignedWidth(Format,Width)/FTextureBlockWidth(Format);
	Result *= FTextureBlockAlignedHeight(Format,Height)/FTextureBlockHeight(Format);
	Result *= FTextureBlockAlignedDepth(Format,Depth)/FTextureBlockDepth(Format);

	return Result;
}

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
