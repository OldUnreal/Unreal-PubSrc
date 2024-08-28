/*=============================================================================
	UnTex.h: Unreal texture related classes.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

class UBitmap;
class UTexture;
class UPalette;

/*-----------------------------------------------------------------------------
	Constants.
-----------------------------------------------------------------------------*/

enum {NUM_PAL_COLORS=256};	// Number of colors in a standard palette.

// Constants.
enum
{
	EHiColor565_R = 0xf800,
	EHiColor565_G = 0x07e0,
	EHiColor565_B = 0x001f,

	EHiColor555_R = 0x7c00,
	EHiColor555_G = 0x03e0,
	EHiColor555_B = 0x001f,

	ETrueColor_R  = 0x00ff0000,
	ETrueColor_G  = 0x0000ff00,
	ETrueColor_B  = 0x000000ff,
	ETrueColor_A  = 0xFF000000
};

typedef	struct
{
	BYTE rgb[3];
} COLOR24;

typedef	struct
{
	BYTE rgba[4];
} COLOR32;

typedef	struct
{
	_WORD rgba[4];
} COLOR64;

/*-----------------------------------------------------------------------------
	UTexture and FTextureInfo.
-----------------------------------------------------------------------------*/

enum {MAX_TEXTURE_LOD=4};

//
// Base mipmap.
//
struct ENGINE_API FMipmapBase
{
public:
	BYTE*			DataPtr;		// Pointer to data, valid only when locked.
	INT				USize,  VSize;	// Power of two tile dimensions.
	BYTE			UBits,  VBits;	// Power of two tile bits.
	FMipmapBase( BYTE InUBits, BYTE InVBits )
	:	DataPtr		(0)
	,	USize		(1<<InUBits)
	,	VSize		(1<<InVBits)
	,	UBits		(InUBits)
	,	VBits		(InVBits)
	{}
	FMipmapBase()
	:	DataPtr		(0)
	,	USize		(0)
	,	VSize		(0)
	,	UBits		(0)
	,	VBits		(0)
	{}
};

//
// Texture mipmap.
//
enum { MAX_MIPS = 20 };

struct ENGINE_API FMipmap : public FMipmapBase
{
public:
	TLazyArray<BYTE> DataArray; // Data.
	FMipmap()
	{}
	FMipmap( BYTE InUBits, BYTE InVBits )
	:	FMipmapBase( InUBits, InVBits )
	,	DataArray( USize * VSize )
	{}
	FMipmap( BYTE InUBits, BYTE InVBits, INT InSize )
	:	FMipmapBase( InUBits, InVBits )
	,	DataArray( InSize )
	{}
	void Clear()
	{
		guard(FMipmap::Clear);
		appMemzero( &DataArray(0), DataArray.Num() );
		unguard;
	}
	friend FArchive& operator<<( FArchive& Ar, FMipmap& M )
	{
		guard(FMipmap<<);
		Ar << M.DataArray;
		Ar << M.USize << M.VSize << M.UBits << M.VBits;
		return Ar;
		unguard;
	}
	void LoadMip()
	{
		DataArray.Load();
		DataPtr = &DataArray(0);
	}
	void UnloadMip()
	{
		DataArray.Unload();
		DataPtr = nullptr;
	}
};

//
// Texture clearing flags.
//
enum ETextureClear
{
	TCLEAR_Temporal	= 1,	// Clear temporal texture effects.
	TCLEAR_Bitmap   = 2,    // Clear the immediate bitmap.
};

//
// Information about a locked texture. Used for ease of rendering.
//
struct ENGINE_API FTextureInfo
{
	friend class UBitmap;
	friend class UTexture;

	// Variables.
	UTexture*		Texture;				// Optional texture.
	QWORD			CacheID;				// Unique cache ID.
	QWORD			PaletteCacheID;			// Unique cache ID of palette.
	FVector			Pan;					// Panning value relative to texture planes.
	FColor*			MaxColor;				// Maximum color in texture and all its mipmaps.
	BYTE			Format;					// Texture format.
	FLOAT			UScale;					// U Scaling.
	FLOAT			VScale;					// V Scaling.
	INT				USize;					// Base U size.
	INT				VSize;					// Base V size.
	INT				UClamp;					// U clamping value, or 0 if none.
	INT				VClamp;					// V clamping value, or 0 if none.
	INT				NumMips;				// Number of mipmaps.
	INT				LOD;					// Level of detail, 0=highest.
	FColor*			Palette;				// Palette colors.
	BITFIELD		bHighColorQuality:1;	// High color quality hint.
	BITFIELD		bHighTextureQuality:1;	// High color quality hint.
	BITFIELD		bRealtime:1;			// Texture changes in realtime.
	BITFIELD		bParametric:1;			// Texture data need not be stored.
	BITFIELD		bRealtimeChanged:1;		// Changed since last render.
	BITFIELD		bDecompressed:1;		// Currently assigned to decompressed mips.
	FMipmapBase*	Mips[MAX_MIPS];			// Array of NumMips of mipmaps.
	BYTE			UClampMode;				// Clamping mode for textures
	BYTE			VClampMode;
	UINT			RenderTag;				// Replaces bRealtimeChanged to support multiple render devices.
	FCoords2D*		Modifier;				// Coordinate system modifier.

	FTextureInfo()
		: Texture(NULL), Pan(0.f,0.f,0.f), MaxColor(NULL), Format(122/*TEXF_NODATA*/), NumMips(0), LOD(0), Palette(NULL), Modifier(NULL)
	{
		Mips[0] = NULL;
		STAT(++GStatTexture.TextureInfo.Count);
	}
	~FTextureInfo()
	{
		STAT(--GStatTexture.TextureInfo.Count);
	}

	// Functions.
	void Load();
	void Unload();
	void CacheMaxColor();
};

/*-----------------------------------------------------------------------------
	UFont.
-----------------------------------------------------------------------------*/

// Font constants.
enum {NUM_FONT_PAGES=256};
enum {NUM_FONT_CHARS=256};

//
// Information about one font glyph which resides in a texture.
//
struct ENGINE_API FFontCharacter
{
	// Variables.
	INT StartU, StartV;
	INT USize, VSize;

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FFontCharacter& Ch )
	{
		guard(FFontCharacter<<);
		return Ar << Ch.StartU << Ch.StartV << Ch.USize << Ch.VSize;
		unguard;
	}
};

//
// A font page.
//
struct ENGINE_API FFontPage
{
	// Variables.
	UTexture* Texture;
	TArray<FFontCharacter> Characters;

	// Serializer.
	friend FArchive& operator<<(FArchive& Ar, FFontPage& Ch);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

struct png_fileinfo;

// Integrates of libpng into FArchive workflow.
struct ENGINE_API FPortableNetworksGraphicArchive
{
	// Variables.
	FArchive*   Ar;
	png_fileinfo* FileInfo;

	// Static function which mimics FFileManager interface.
	static FPortableNetworksGraphicArchive* CreateFileWriter(const TCHAR* Filename, DWORD WriteFlags = 0, FOutputDevice* Error = GNull, FFileManager* FileManager = GFileManager);

	// Writes image.
	void SerializeImage(BYTE* Data, INT USize, INT VSize); // !! Hardwired to URenderDevice::ReadPixels() for now.

	// Destructor.
	~FPortableNetworksGraphicArchive() noexcept(false);

	// Archive Foo.
	UBOOL IsLoading()   { return ArIsLoading; }
	UBOOL IsSaving()    { return ArIsSaving; }
	UBOOL IsError()     { return ArIsError; }

private:
	// Constructors.
	FPortableNetworksGraphicArchive(FArchive* InAr, png_fileinfo* InFileInfo);

protected:
	// Status variables.
	UBOOL ArIsLoading;
	UBOOL ArIsSaving;
	UBOOL ArIsError;
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
