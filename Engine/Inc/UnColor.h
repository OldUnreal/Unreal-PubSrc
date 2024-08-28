/*=============================================================================
	UnColor.h: Unreal color format definitions and routines.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
	Copyright 2015-2020 Sebastian Kaufel. All Rights Reserved.

	Revision history:
	 * Created by Sebastian Kaufel based on FColor from UnTex.h.
=============================================================================*/

/*
!! Add some fucking Parse method to Templates.

UBOOL GetFCOLOR( const TCHAR* Stream, FColor& Value )
{
	guard(GetFCOLOR);
	if ( !Stream || !*Stream )
		return 0;
	FColor Temp(0,0,0,0);
	INT Components=0;
	Components += Parse( Stream, TEXT("R="), Temp.R );
	Components += Parse( Stream, TEXT("G="), Temp.G );
	Components += Parse( Stream, TEXT("B="), Temp.B );
	Components += Parse( Stream, TEXT("A="), Temp.A );
	if ( Components>0 )
		Value = Temp;	
	return Components>0;
	unguard;
}
*/

/*-----------------------------------------------------------------------------
	Forward declarations.
-----------------------------------------------------------------------------*/
/*
class FColor;
class FColor16F;
class FColor32F;
class FRed5Green6Blue5;
class FRedGreenBlue6Alpha1;
class FRedGreenBlue10Alpha1;
*/

/*-----------------------------------------------------------------------------
	Constants.
-----------------------------------------------------------------------------*/
/*
// Masks for BGRA order.
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
};
*/

class FColor32F;

// If I use both CT and FLOAT constructors, this just gives me hell with ambiguity.
enum EC_NormalizedColor
{
	E_NormalizedColor,
};

// Used for construction (unsigned) integer normal maps
enum EC_NormalMap
{
	E_NormalMap,
};

/*-----------------------------------------------------------------------------
	Macros.
-----------------------------------------------------------------------------*/

#define GET_COLOR_BYTE(color)  ( *(BYTE*)&(color))
#define GET_COLOR_WORD(color)  (*(_WORD*)&(color))
#define GET_COLOR_DWORD(color) (*(DWORD*)&(color))
#define GET_COLOR_QWORD(color) (*(QWORD*)&(color))

/*----------------------------------------------------------------------------
	Colorspaces and Alphaspace.
----------------------------------------------------------------------------*/

//
// Texture Colorspaces.
//

// Maybe Grey!
// TEXC_Modulated.
// TEXA_SignedDistanceField.
enum ETextureColorspace
{
	// Cleaned up.
	TEXC_Undefined    = 0x00, // Unspecified (typically about sRGB).
	TEXC_Linear       = 0x01, // Linear data (unspecified).
	TEXC_RGB          = 0x02, // Linear RGB data (unspecified primaries).
	TEXC_sRGB         = 0x03, // sRGB with proper piecewise gamma transfer function.
	TEXC_sRGB22       = 0x04, // sRGB gamut, but with approximated Gamma=2.2 transfer function, popular in DCC tools.
	TEXC_sRGB20       = 0x05, // sRGB gamut, but with approximated Gamma=2.0 transfer function, popular as a rough approximation.
	TEXC_AppleRGB     = 0x06, // The game textures in Unreal are a wild mix of different
	TEXC_AdobeRGB     = 0x07, // colorspace including Apple RGB and Adobe RGB.
	// 0x08-0x0F resered.
	TEXC_Normals      = 0x10, // Normals as stored normalized unsigned D3D format.
	// 0x11-0x1F reserved.
	TEXC_CIEXYZ       = 0x20, // Do I need to specify an illuminant? (if so: D65).
	TEXC_CIELAB       = 0x21, // D65 Illuminant.
	TEXC_CIELUV       = 0x22, // D65 Illuminant.
	TEXC_DIN99        = 0x23, // D65 Illuminant.
	// 0x24-0x39 reserved.

	// 0x80-0x100 reserved for user internal purposes?

	TEXC_MAX		      = 0xff,
};

// Whether the Colorspace is loosely RGB color based or not (like CIEXYZ).
inline UBOOL FIsRedGreenBlueColorspace( const BYTE Colorspace )
{
	switch ( Colorspace )
	{
		case TEXC_RGB:
		case TEXC_sRGB:
		case TEXC_sRGB22:
		case TEXC_AppleRGB:
		case TEXC_AdobeRGB:
			return 1;
	
		default:
			return 0;
	}
}

// I think this demonstrates a slight awkwardness of naming it Colorspace. --han
inline UBOOL FIsColorColorspace( const BYTE Colorspace )
{
	switch ( Colorspace )
	{
		case TEXC_CIEXYZ:
		case TEXC_CIELAB:
		case TEXC_CIELUV:
		case TEXC_DIN99:
			return 1;
	
		default:
			return FIsRedGreenBlueColorspace(Colorspace);
	}
}

//
// Texture Alphaspace.
//
enum ETextureAlphaspace
{
	TEXA_Undefined = 0x00, // Unspecified 


	// TEXA_SignedDistanceField
	// TEXA_AlphaTested = 0x01, // AlphaThreshold at 0.5.
	// TEXA_RenormalizedAlphaTested = 0x02, // Mips calculate different..

};

inline UBOOL GetFCOLOR( const TCHAR* Stream, FColor& Value )
{
	guard(GetFCOLOR);
	if ( !Stream || !*Stream )
		return 0;
	FColor Temp(0,0,0,0);
	INT Components=0;
	Components += Parse( Stream, TEXT("R="), Temp.R );
	Components += Parse( Stream, TEXT("G="), Temp.G );
	Components += Parse( Stream, TEXT("B="), Temp.B );
	Components += Parse( Stream, TEXT("A="), Temp.A );
	if ( Components>0 )
		Value = Temp;	
	return Components>0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Color Templates.
-----------------------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif
//
// A full integer color template.
//
template<typename CT, SQWORD TYPE_MIN, SQWORD TYPE_MAX, typename CLAMP_TYPE, BYTE F> class TColor
{
public:
	// Variables.
	union
	{
		CT R;
		CT X;
		CT S;
	};
	union
	{
		CT G;
		CT Y;
		CT T;
	};
	union
	{
		CT B;
		CT Z;
		CT P;
	};
	union
	{
		CT A;
		CT W;
		CT Q;
	};

	// Constructors.
	TColor()
	{}
	TColor( CT InR, CT InG=0, CT InB=0, CT InA=TYPE_MAX )
	:	R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
	{}
	// Input is assumed to be in [0,1] range for unsigned and, [-1,1] for signed types. For signed types -1 will map to TYPE_MIN+1, while 0 will be preserved.
	TColor( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	: R(Clamp<CLAMP_TYPE>(appRound(InR*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, G(Clamp<CLAMP_TYPE>(appRound(InG*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, B(Clamp<CLAMP_TYPE>(appRound(InB*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, A(Clamp<CLAMP_TYPE>(appRound(InA*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, TColor& C )
	{
		return Ar << C.R << C.G << C.B << C.A;
	}

	// Operators.
	UBOOL operator==( const TColor &C ) const
	{
		return R==C.R && G==C.G && B==C.B && A==C.A;
	}
	UBOOL operator!=( const TColor& C ) const
	{
		return R!=C.R || G!=C.G || B!=C.B || A!=C.A;
	}

	// Routines.
	FPlane Plane() const
	{
		return FPlane( R/DOUBLE(TYPE_MAX), G/DOUBLE(TYPE_MAX), B/DOUBLE(TYPE_MAX), A/DOUBLE(TYPE_MAX) );
	}
	FPlane PlaneClamp() const
	{
		return FPlane( (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0) );
	}

	// Channel inversion routines.
	void InvertRed() // !! FIX-ME !! Only valid for unsigned.
	{
		R = ComponentMax()-R;
	}
	void InvertGreen()
	{
		G = ComponentMax()-G;
	}
	void InvertBlue()
	{
		B = ComponentMax()-B;
	}
	void InvertAlpha()
	{
		A = ComponentMax()-A;
	}

	// Component access.
	CT Red() const
	{
		return R;
	}
	CT Green() const
	{
		return G;
	}
	CT Blue() const
	{
		return B;
	}
	CT Alpha() const
	{
		return A;
	}

	// Pink masking.
	void PinkMask()
	{
		if ( R>(ComponentMax()-(ComponentMax()-ComponentMin())/255) && G<(ComponentMin()+(ComponentMax()-ComponentMin())/255) && B>(ComponentMax()-(ComponentMax()-ComponentMin())/255) )
			*this = TColor(0,0,0,0);
	}

	// Type interface.
	static INT Colors()
	{
		return 4;
	}
	static INT ColorBytes()
	{
		return sizeof(CT)*Colors();
	}
	static BYTE Format()
	{
		return F;
	}
	static SQWORD ComponentMin()
	{
		return TYPE_MIN;
	}
	static SQWORD ComponentMax()
	{
		return TYPE_MAX;
	}

	// Returns a string description.
	FString String() const
	{
		guard(TColor::String);
		return FString::Printf( TEXT("(R=%i,G=%i,B=%i,A=%i)"), R, G, B, A );
		unguard;
	}
};

//
// A Red integer color template.
//
template<typename CT, SQWORD TYPE_MIN, SQWORD TYPE_MAX, typename CLAMP_TYPE, BYTE F> class TRed
{
public:
	// Variables.
	union
	{
		CT R;  // Colors.
		CT X;  // Coordinates.
		CT S;  // Texture Coordinates.
	};

	// Constructors.
	TRed()
	{}
	TRed( CT InR )
	: R(InR)
	{}
	// Input is assumed to be in [0,1] range for unsigned and, [-1,1] for signed types. For signed types -1 will map to TYPE_MIN+1, while 0 will be preserved.
	TRed( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(Clamp<CLAMP_TYPE>(appRound(InR*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, TRed& C )
	{
		return Ar << C.R << C.G;
	}

	// Operators.
	UBOOL operator==( const TRed &C ) const
	{
		return R==C.R;
	}
	UBOOL operator!=( const TRed& C ) const
	{
		return R!=C.R;
	}

	// Routines.
	FPlane Plane() const
	{
		return FPlane( R/DOUBLE(TYPE_MAX), 0.f, 0.f, 1.f );
	}
	FPlane PlaneClamp() const
	{
		return FPlane( (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), 0.f, 0.f, 1.f );
	}

	// Channel inversion routines.
	void InvertRed() // !! FIX-ME !! Only valid for unsigned.
	{
		R = ComponentMax()-R;
	}
	void InvertGreen()
	{
	}
	void InvertBlue()
	{
	}
	void InvertAlpha()
	{
	}

	// Component access.
	CT Red() const
	{
		return R;
	}
	CT Green() const
	{
		return 0;
	}
	CT Blue() const
	{
		return 0;
	}
	CT Alpha() const
	{
		return TYPE_MAX;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 1;
	}
	static INT ColorBytes()
	{
		return sizeof(CT)*Colors();
	}
	static BYTE Format()
	{
		return F;
	}
	static SQWORD ComponentMin()
	{
		return TYPE_MIN;
	}
	static SQWORD ComponentMax()
	{
		return TYPE_MAX;
	}

	// Returns a string description.
	FString String() const
	{
		guard(TRed::String);
		return FString::Printf( TEXT("(R=%i)"), R );
		unguard;
	}
};

//
// A RG integer color template.
//
template<typename CT, SQWORD TYPE_MIN, SQWORD TYPE_MAX, typename CLAMP_TYPE, BYTE F> class TRedGreen
{
public:
	// Variables.
	union
	{
		CT R;
		CT X;
		CT S;
	};
	union
	{
		CT G;
		CT Y;
		CT T;
	};

	// Constructors.
	TRedGreen()
	{}
	TRedGreen( CT InR, CT InG=0 )
	: R(InR)
	, G(InG)
	{}
	// Input is assumed to be in [0,1] range for unsigned and, [-1,1] for signed types. For signed types -1 will map to TYPE_MIN+1, while 0 will be preserved.
	TRedGreen( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(Clamp<CLAMP_TYPE>(appRound(InR*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, G(Clamp<CLAMP_TYPE>(appRound(InG*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	{}
	TRedGreen( EC_NormalMap, FLOAT InX, FLOAT InY )
	:	R(Clamp<CLAMP_TYPE>(appRound(CT(0)<CT(-1) ? ((InX+1.0)/2.0)*TYPE_MAX : InX*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, G(Clamp<CLAMP_TYPE>(appRound(CT(0)<CT(-1) ? ((InY+1.0)/2.0)*TYPE_MAX : InY*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, TRedGreen& C )
	{
		return Ar << C.R << C.G;
	}

	// Operators.
	UBOOL operator==( const TRedGreen &C ) const
	{
		return R==C.R && G==C.G;
	}
	UBOOL operator!=( const TRedGreen& C ) const
	{
		return R!=C.R || G!=C.G;
	}

	// Routines.
	FPlane Plane() const
	{
		return FPlane( R/DOUBLE(TYPE_MAX), G/DOUBLE(TYPE_MAX), 0.f, 1.f );
	}
	FPlane PlaneClamp() const
	{
		return FPlane( (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), 0.f, 1.f );
	}

	// Channel inversion routines.
	void InvertRed() // !! FIX-ME !! Only valid for unsigned.
	{
		R = ComponentMax()-R;
	}
	void InvertGreen()
	{
		G = ComponentMax()-G;
	}
	void InvertBlue()
	{
	}
	void InvertAlpha()
	{
	}

	// Component access.
	CT Red() const
	{
		return R;
	}
	CT Green() const
	{
		return G;
	}
	CT Blue() const
	{
		return 0;
	}
	CT Alpha() const
	{
		return TYPE_MAX;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 2;
	}
	static INT ColorBytes()
	{
		return sizeof(CT)*Colors();
	}
	static BYTE Format()
	{
		return F;
	}
	static SQWORD ComponentMin()
	{
		return TYPE_MIN;
	}
	static SQWORD ComponentMax()
	{
		return TYPE_MAX;
	}

	// Returns a string description.
	FString String() const
	{
		guard(TRedGreen::String);
		return FString::Printf( TEXT("(R=%i,G=%i)"), R, G );
		unguard;
	}
};

//
// A RGB integer color template.
//
// More or less just a template for easier variable access.
//
template<typename CT, SQWORD TYPE_MIN, SQWORD TYPE_MAX, typename CLAMP_TYPE, BYTE F> class TRedGreenBlue
{
public:
	// Variables.
	union
	{
		CT R;
		CT X;
		CT S;
	};
	union
	{
		CT G;
		CT Y;
		CT T;
	};
	union
	{
		CT B;
		CT Z;
		CT P;
	};

	// Constructors.
	TRedGreenBlue()
	{}
	TRedGreenBlue( CT InR, CT InG=0, CT InB=0, CT InA=TYPE_MAX )
	: R(InR)
	, G(InG)
	, B(InB)
	{}
	// Input is assumed to be in [0,1] range for unsigned and, [-1,1] for signed types. For signed types -1 will map to TYPE_MIN+1, while 0 will be preserved.
	TRedGreenBlue( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(Clamp<CLAMP_TYPE>(appRound(InR*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, G(Clamp<CLAMP_TYPE>(appRound(InG*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	, B(Clamp<CLAMP_TYPE>(appRound(InB*TYPE_MAX),TYPE_MIN,TYPE_MAX))
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, TRedGreenBlue& C )
	{
		return Ar << C.R << C.G << C.B;
	}

	// Operators.
	UBOOL operator==( const TRedGreenBlue &C ) const
	{
		return R==C.R && G==C.G && B==C.B;
	}
	UBOOL operator!=( const TRedGreenBlue& C ) const
	{
		return R!=C.R || G!=C.G || B!=C.B;
	}

	// Routines.
	FPlane Plane() const
	{
		return FPlane( R/DOUBLE(TYPE_MAX), G/DOUBLE(TYPE_MAX), B/DOUBLE(TYPE_MAX), 1.0 );
	}
	FPlane PlaneClamp() const
	{
		return FPlane( (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), (DOUBLE)Max(R/DOUBLE(TYPE_MAX),-1.0), 1.0 );
	}

	// Channel inversion routines.
	void InvertRed() // !! FIX-ME !! Only valid for unsigned.
	{
		R = ComponentMax()-R;
	}
	void InvertGreen()
	{
		G = ComponentMax()-G;
	}
	void InvertBlue()
	{
		B = ComponentMax()-B;
	}
	void InvertAlpha()
	{
	}

	// Component access.
	CT Red() const
	{
		return R;
	}
	CT Green() const
	{
		return G;
	}
	CT Blue() const
	{
		return B;
	}
	CT Alpha() const
	{
		return TYPE_MAX;
	}

	// Pink masking.
	void PinkMask()
	{
		if ( R>(ComponentMax()-(ComponentMax()-ComponentMin())/255) && G<(ComponentMin()+(ComponentMax()-ComponentMin())/255) && B>(ComponentMax()-(ComponentMax()-ComponentMin())/255) )
			*this = TRedGreenBlue(0,0,0);
	}

	// Type interface.
	static INT Colors()
	{
		return 3;
	}
	static INT ColorBytes()
	{
		return sizeof(CT)*Colors();
	}
	static BYTE Format()
	{
		return F;
	}
	static SQWORD ComponentMin()
	{
		return TYPE_MIN;
	}
	static SQWORD ComponentMax()
	{
		return TYPE_MAX;
	}

	// Returns a string description.
	FString String() const
	{
		guard(TRedGreenBlue::String);
		return FString::Printf( TEXT("(R=%i,G=%i,B=%i)"), R, G, B );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	Templated Colors.
-----------------------------------------------------------------------------*/

typedef TRed<BYTE,  -(SQWORD)         0,        255, INT,    TEXF_R8   > FRed;
typedef TRed<SBYTE, -(SQWORD)       128,        127, INT,    TEXF_R8_S > FRedS;
typedef TRed<_WORD, -(SQWORD)         0,      65535, INT,    TEXF_R16  > FRed16;
typedef TRed<SWORD, -(SQWORD)     32768,      32767, INT,    TEXF_R16_S> FRed16S;
typedef TRed<DWORD, -(SQWORD)         0, 4294967295, SQWORD, TEXF_R32  > FRed32;
typedef TRed<INT,   -(SQWORD)2147483648, 2147483647, SQWORD, TEXF_R32_S> FRed32S;

typedef TRedGreen<BYTE,  -(SQWORD)         0,        255, INT,    TEXF_RG8   > FRedGreen;
typedef TRedGreen<SBYTE, -(SQWORD)       128,        127, INT,    TEXF_RG8_S > FRedGreenS;
typedef TRedGreen<_WORD, -(SQWORD)         0,      65535, INT,    TEXF_RG16  > FRedGreen16;
typedef TRedGreen<SWORD, -(SQWORD)     32768,      32767, INT,    TEXF_RG16_S> FRedGreen16S;
typedef TRedGreen<DWORD, -(SQWORD)         0, 4294967295, SQWORD, TEXF_RG32  > FRedGreen32;
typedef TRedGreen<INT,   -(SQWORD)2147483648, 2147483647, SQWORD, TEXF_RG32_S> FRedGreen32S;

typedef TRedGreenBlue<BYTE,  -(SQWORD)         0,        255, INT,    TEXF_RGB8   > FRedGreenBlue;
typedef TRedGreenBlue<SBYTE, -(SQWORD)       128,        127, INT,    TEXF_RGB8_S > FRedGreenBlueS;
typedef TRedGreenBlue<_WORD, -(SQWORD)         0,      65535, INT,    TEXF_RGB16_ > FRedGreenBlue16;
typedef TRedGreenBlue<SWORD, -(SQWORD)     32768,      32767, INT,    TEXF_RGB16_S> FRedGreenBlue16S;
typedef TRedGreenBlue<DWORD, -(SQWORD)         0, 4294967295, SQWORD, TEXF_RGB32  > FRedGreenBlue32;
typedef TRedGreenBlue<INT,   -(SQWORD)2147483648, 2147483647, SQWORD, TEXF_RGB32_S> FRedGreenBlue32S;

typedef TColor<BYTE,  -(SQWORD)         0,        255, INT,    TEXF_RGBA8_  > _FColor;
typedef TColor<SBYTE, -(SQWORD)       128,        127, INT,    TEXF_RGBA8_S > FColorS;
typedef TColor<_WORD, -(SQWORD)         0,      65535, INT,    TEXF_RGBA16  > FColor16;
typedef TColor<SWORD, -(SQWORD)     32768,      32767, INT,    TEXF_RGBA16_S> FColor16S;
typedef TColor<DWORD, -(SQWORD)         0, 4294967295, SQWORD, TEXF_RGBA32  > FColor32;
typedef TColor<INT,   -(SQWORD)2147483648, 2147483647, SQWORD, TEXF_RGBA32_S> FColor32S;


/*------------------------------------------------------------------------------------
	FColor32F.
------------------------------------------------------------------------------------*/

// A floating point color value.
class FColor32F
{
public:
	// Variables.
	union
	{
		FLOAT R;
		FLOAT X;
	};
	union
	{
		FLOAT G;
		FLOAT Y;
	};
	union
	{
		FLOAT B;
		FLOAT Z;
	};
	union
	{
		FLOAT A;
		FLOAT _A;
	};

	// Constructors.
	FColor32F()
	{}
	FColor32F( FLOAT InR, FLOAT InG, FLOAT InB, FLOAT InA )
	:	R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
	{}
	FColor32F( const FColor& InC )
	:	R(InC.R/255.0f)
	,	G(InC.G/255.0f)
	,	B(InC.B/255.0f)
	,	A(InC.A/255.0f)
	{}
	FColor32F( const FColor16& InC )
	:	R(InC.R/65535.0f)
	,	G(InC.G/65535.0f)
	,	B(InC.B/65535.0f)
	,	A(InC.A/65535.0f)
	{}
	FColor32F( const FPlane& InP )
	:	R(InP.X)
	,	G(InP.Y)
	,	B(InP.Z)
	,	A(InP.W)
	{}
	FColor32F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
	{}

	// Serializer.
	friend FArchive& operator<< (FArchive& Ar, FColor32F& Color )
	{
		return Ar << Color.R << Color.G << Color.B << Color.A;
	}

	// Operators.
	UBOOL operator==( const FColor32F &C ) const
	{
		return R==C.R && G==C.G && B==C.B && A==C.A;
	}
	UBOOL operator!=( const FColor32F& C ) const
	{
		return R!=C.R || G!=C.G || B!=C.B || A!=C.A;
	}
	FLOAT FBrightness() const
	{
		return 0.2126f*R+0.7152f*G+0.0722f*B;
	}
	FLOAT FMin() const
	{
		return Min<FLOAT>(R,G,B);
	}
	FLOAT FMax() const
	{
		return Max<FLOAT>(R,G,B);
	}

	// More Operates snagged from FVector.
	FLOAT operator|( const FColor32F& V ) const
	{
		return R*V.R + G*V.G + B*V.B + A*V.A;
	}
	friend FColor32F operator*( FLOAT Scale, const FColor32F& V )
	{
		return FColor32F( V.R*Scale, V.G*Scale, V.B*Scale, V.A*Scale );
	}
	FColor32F operator+( const FColor32F& V ) const
	{
		return FColor32F( R+V.R, G+V.G, B+V.B, A+V.A );
	}
	FColor32F operator-( const FColor32F& V ) const
	{
		return FColor32F( R-V.R, G-V.G, B-V.B, A-V.A );
	}
	FColor32F operator*( FLOAT Scale ) const
	{
		return FColor32F( R*Scale, G*Scale, B*Scale, A*Scale );
	}
	FColor32F operator/( FLOAT Scale ) const
	{
		FLOAT RScale = 1.0/Scale;
		return FColor32F( R*RScale, G*RScale, B*RScale, A*RScale );
	}
	FColor32F operator*( const FColor32F& V ) const
	{
		return FColor32F( R*V.R, G*V.G, B*V.B, A*V.A );
	}

	// Unary operators.
	FColor32F operator-() const
	{
		return FColor32F( -R, -G, -B, -A );
	}

	// Assignment operators.
	FColor32F operator+=( const FColor32F& V )
	{
		R += V.R;
		G += V.G;
		B += V.B;
		A += V.A;
		return *this;
	}
	FColor32F operator-=( const FColor32F& V )
	{
		R -= V.R;
		G -= V.G;
		B -= V.B;
		A -= V.A;
		return *this;
	}
	FColor32F operator*=( FLOAT Scale )
	{
		R *= Scale;
		G *= Scale;
		B *= Scale;
		A *= Scale;
		return *this;
	}
	FColor32F operator/=( FLOAT V )
	{
		FLOAT RV = 1.0/V;
		R *= RV;
		G *= RV;
		B *= RV;
		A *= RV;
		return *this;
	}
	FColor32F operator*=( const FColor32F& V )
	{
		R *= V.R;
		G *= V.G;
		B *= V.B;
		A *= V.A;
		return *this;
	}
	FColor32F operator/=( const FColor32F& V )
	{
		R /= V.R;
		G /= V.G;
		B /= V.B;
		A /= V.A;
		return *this;
	}

	// Misc.
	friend FColor32F Max( const FColor32F& A, const FColor32F& B )
	{
		return FColor32F
		(
			Max<FLOAT>( A.R, B.R ),
			Max<FLOAT>( A.G, B.G ),
			Max<FLOAT>( A.B, B.B ),
			Max<FLOAT>( A.A, B.A )
		);
	}
	friend FColor32F Min( const FColor32F& A, const FColor32F& B )
	{
		return FColor32F
		(
			Min<FLOAT>( A.R, B.R ),
			Min<FLOAT>( A.G, B.G ),
			Min<FLOAT>( A.B, B.B ),
			Min<FLOAT>( A.A, B.A )
		);
	}
	friend FColor32F Clamp( const FColor32F& Color, const FColor32F& Min, const FColor32F& Max )
	{
		return FColor32F
		(
			Clamp<FLOAT>( Color.R, Min.R, Max.R ),
			Clamp<FLOAT>( Color.G, Min.G, Max.G ),
			Clamp<FLOAT>( Color.B, Min.B, Max.B ),
			Clamp<FLOAT>( Color.A, Min.A, Max.A )
		);
	}

	FPlane& Plane() const
	{
		return *(FPlane*)this;
	}
	
	FColor ToFColor()
	{
		FColor Result(
			Clamp<INT>(appRound(R * 255), 0, 255),
			Clamp<INT>(appRound(G * 255), 0, 255),
			Clamp<INT>(appRound(B * 255), 0, 255),
			Clamp<INT>(appRound(A * 255), 0, 255));
		return Result;
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R;
	}
	void InvertGreen()
	{
		G = 1.0f-G;
	}
	void InvertBlue()
	{
		B = 1.0f-B;
	}
	void InvertAlpha()
	{
		A = 1.0f-A;
	}

	// Component access.
	FLOAT Red() const
	{
		return R;
	}
	FLOAT Green() const
	{
		return G;
	}
	FLOAT Blue() const
	{
		return B;
	}
	FLOAT Alpha() const
	{
		return A;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 4;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_RGBA32_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FColor32F::String);
		return FString::Printf( TEXT("(R=%f,G=%f,B=%f,A=%f)"), R, G, B, A );
		unguard;
	}
};

/*------------------------------------------------------------------------------------
	FRedGreenBlue32F.
------------------------------------------------------------------------------------*/

// A floating point rgb value.
class FRedGreenBlue32F
{
public:
	// Variables.
	union
	{
		FLOAT R;
		FLOAT X;
	};
	union
	{
		FLOAT G;
		FLOAT Y;
	};
	union
	{
		FLOAT B;
		FLOAT Z;
	};

	// Constructors.
	FRedGreenBlue32F()
	{}
	FRedGreenBlue32F( BYTE InR, BYTE InG, BYTE InB )
	:	R(InR/255.f)
	,	G(InG/255.f)
	,	B(InB/255.f)
	{}
	FRedGreenBlue32F( BYTE InR, BYTE InG, BYTE InB, BYTE InA )
	:	R(InR/255.f)
	,	G(InG/255.f)
	,	B(InB/255.f)
	{}
	FRedGreenBlue32F( _WORD InR, _WORD InG, _WORD InB )
	:	R(InR/65535.f)
	,	G(InG/65535.f)
	,	B(InB/65535.f)
	{}
	FRedGreenBlue32F( _WORD InR, _WORD InG, _WORD InB, _WORD InA )
	:	R(InR/65535.f)
	,	G(InG/65535.f)
	,	B(InB/65535.f)
	{}
	FRedGreenBlue32F( FLOAT InR, FLOAT InG, FLOAT InB )
	:	R(InR)
	, G(InG)
	, B(InB)
	{}
	FRedGreenBlue32F( FLOAT InR, FLOAT InG, FLOAT InB, FLOAT InA )
	:	R(InR)
	, G(InG)
	, B(InB)
	{}
	FRedGreenBlue32F( const FColor& InC )
	:	R(InC.R/255.f)
	,	G(InC.G/255.f)
	,	B(InC.B/255.f)
	{}
	FRedGreenBlue32F( const FColor16& InC )
	:	R(InC.R/65535.f)
	,	G(InC.G/65535.f)
	,	B(InC.B/65535.f)
	{}
	FRedGreenBlue32F( const FPlane& InP )
	:	R(InP.X)
	,	G(InP.Y)
	,	B(InP.Z)
	{}
	FRedGreenBlue32F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	, G(InG)
	, B(InB)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRedGreenBlue32F& Color )
	{
		return Ar << Color.R << Color.G << Color.B;
	}

	FPlane Plane() const
	{
		return FPlane(R,G,B,1.0f);
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R;
	}
	void InvertGreen()
	{
		G = 1.0f-G;
	}
	void InvertBlue()
	{
		B = 1.0f-B;
	}
	void InvertAlpha()
	{
	}

	// Component access.
	FLOAT Red() const
	{
		return R;
	}
	FLOAT Green() const
	{
		return G;
	}
	FLOAT Blue() const
	{
		return B;
	}
	FLOAT Alpha() const
	{
		return 1.0f;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 3;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_RGB32_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRedGreenBlue32F::String);
		return FString::Printf( TEXT("(R=%f,G=%f,B=%f)"), R, G, B );
		unguard;
	}
};

/*------------------------------------------------------------------------------------
	FRedGreen32F.
------------------------------------------------------------------------------------*/

// A floating point rg value.
class FRedGreen32F
{
public:
	// Variables.
	union
	{
		FLOAT R;
		FLOAT X;
	};
	union
	{
		FLOAT G;
		FLOAT Y;
	};

	// Constructors.
	FRedGreen32F()
	{}
	FRedGreen32F( BYTE InR, BYTE InG, BYTE InB )
	:	R(InR/255.f)
	,	G(InG/255.f)
	{}
	FRedGreen32F( BYTE InR, BYTE InG, BYTE InB, BYTE InA )
	:	R(InR/255.f)
	,	G(InG/255.f)
	{}
	FRedGreen32F( _WORD InR, _WORD InG, _WORD InB )
	:	R(InR/65535.f)
	,	G(InG/65535.f)
	{}
	FRedGreen32F( _WORD InR, _WORD InG, _WORD InB, _WORD InA )
	:	R(InR/65535.f)
	,	G(InG/65535.f)
	{}
	FRedGreen32F( FLOAT InR, FLOAT InG, FLOAT InB )
	:	R(InR)
	, G(InG)
	{}
	FRedGreen32F( FLOAT InR, FLOAT InG, FLOAT InB, FLOAT InA )
	:	R(InR)
	, G(InG)
	{}
	FRedGreen32F( const FColor& InC )
	:	R(InC.R/255.f)
	,	G(InC.G/255.f)
	{}
	FRedGreen32F( const FColor16& InC )
	:	R(InC.R/65535.f)
	,	G(InC.G/65535.f)
	{}
	FRedGreen32F( const FPlane& InP )
	:	R(InP.X)
	,	G(InP.Y)
	{}
	FRedGreen32F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	, G(InG)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRedGreen32F& Color )
	{
		return Ar << Color.R << Color.G;
	}

	FPlane Plane() const
	{
		return FPlane(R,G,0.0f,1.0f);
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R;
	}
	void InvertGreen()
	{
		G = 1.0f-G;
	}
	void InvertBlue()
	{
	}
	void InvertAlpha()
	{
	}

	// Component access.
	FLOAT Red() const
	{
		return R;
	}
	FLOAT Green() const
	{
		return G;
	}
	FLOAT Blue() const
	{
		return 0.0f;
	}
	FLOAT Alpha() const
	{
		return 1.0f;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 2;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_RG32_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRedGreen32F::String);
		return FString::Printf( TEXT("(R=%f,G=%f)"), R, G );
		unguard;
	}
};

/*------------------------------------------------------------------------------------
	FRed32F.
------------------------------------------------------------------------------------*/

// A floating point red value.
class FRed32F
{
public:
	// Variables.
	union
	{
		FLOAT R;
		FLOAT X;
	};

	// Constructors.
	FRed32F()
	{}
	FRed32F( BYTE InR, BYTE InG, BYTE InB )
	:	R(InR/255.f)
	{}
	FRed32F( BYTE InR, BYTE InG, BYTE InB, BYTE InA )
	:	R(InR/255.f)
	{}
	FRed32F( _WORD InR, _WORD InG, _WORD InB )
	:	R(InR/65535.f)
	{}
	FRed32F( _WORD InR, _WORD InG, _WORD InB, _WORD InA )
	:	R(InR/65535.f)
	{}
	FRed32F( FLOAT InR, FLOAT InG, FLOAT InB )
	:	R(InR)
	{}
	FRed32F( FLOAT InR, FLOAT InG, FLOAT InB, FLOAT InA )
	:	R(InR)
	{}
	FRed32F( const FColor& InC )
	:	R(InC.R/255.f)
	{}
	FRed32F( const FColor16& InC )
	:	R(InC.R/65535.f)
	{}
	FRed32F( const FPlane& InP )
	:	R(InP.X)
	{}
	FRed32F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRed32F& Color )
	{
		return Ar << Color.R;
	}

	FPlane Plane() const
	{
		return FPlane(R,0.0f,0.0f,1.0f);
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R;
	}
	void InvertGreen()
	{
	}
	void InvertBlue()
	{
	}
	void InvertAlpha()
	{
	}

	// Component access.
	FLOAT Red() const
	{
		return R;
	}
	FLOAT Green() const
	{
		return 0.0f;
	}
	FLOAT Blue() const
	{
		return 0.0f;
	}
	FLOAT Alpha() const
	{
		return 1.0f;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 1;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_R32_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRed32F::String);
		return FString::Printf( TEXT("(R=%f)"), R );
		unguard;
	}
};

/*------------------------------------------------------------------------------------
	FColor16F (stub).
------------------------------------------------------------------------------------*/

//
// A half precission floating point value.
//
// !! FIX-ME !! Support opcodes for convertion.
//
struct FHalf
{
	// Constructors.
	FHalf()
	{}
	FHalf(FLOAT InValue);

	// Access.
	FLOAT Float() const;

	// Manipulation.
	//void RemoveDenormals(); // Sets denormal values to zero.
	//void ClampPositive();   // Value = Max(0,FP16MAX).

	// Serializer.
	friend FArchive& operator<<(FArchive& Ar, FHalf& Half)
	{
		return Ar << Half.Value;
	}

	//static void StaticInit();
private:
	_WORD Value;
};

// A half floating point color value.
class FColor16F
{
public:
	// Variables.
	union
	{
		FHalf R;
		FHalf X;
	};
	union
	{
		FHalf G;
		FHalf Y;
	};
	union
	{
		FHalf B;
		FHalf Z;
	};
	union
	{
		FHalf A;
		FHalf _A;
	};

	// Constructors.
	FColor16F()
	{}
	FColor16F( FLOAT InR, FLOAT InG, FLOAT InB )
	:	R(InR)
	, G(InG)
	, B(InB)
	, A(1.f)
	{}
	FColor16F( FLOAT InR, FLOAT InG, FLOAT InB, FLOAT InA )
	:	R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
	{}
	FColor16F( const FPlane& InP )
	:	R(InP.X)
	,	G(InP.Y)
	,	B(InP.Z)
	,	A(InP.W)
	{}
	FColor16F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	, G(InG)
	, B(InB)
	, A(InA)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FColor16F& Color )
	{
		return Ar << Color.R << Color.G << Color.B << Color.A;
	}

	FPlane Plane() const
	{
		return *(FPlane*)this;
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R.Float();
	}
	void InvertGreen()
	{
		G = 1.0f-G.Float();
	}
	void InvertBlue()
	{
		B = 1.0f-B.Float();
	}
	void InvertAlpha()
	{
		A = 1.0f-A.Float();
	}

	// Component access.
	FHalf Red() const
	{
		return R;
	}
	FHalf Green() const
	{
		return G;
	}
	FHalf Blue() const
	{
		return B;
	}
	FHalf Alpha() const
	{
		return A;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 4;
	}
	static INT ColorBytes()
	{
		return sizeof(FHalf)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_RGBA16_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FColor16F::String);
		return FString::Printf( TEXT("(R=%f,G=%f,B=%f,A=%f)"), R.Float(), G.Float(), B.Float(), A.Float() );
		unguard;
	}
};

// A half floating point rgb value.
class FRedGreenBlue16F
{
public:
	// Variables.
	union
	{
		FHalf R;
		FHalf X;
	};
	union
	{
		FHalf G;
		FHalf Y;
	};
	union
	{
		FHalf B;
		FHalf Z;
	};

	// Constructors.
	FRedGreenBlue16F()
	{}
	FRedGreenBlue16F( const FPlane& InP )
	:	R(InP.X)
	,	G(InP.Y)
	,	B(InP.Z)
	{}
	FRedGreenBlue16F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	, G(InG)
	, B(InB)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRedGreenBlue16F& RedGreenBlue )
	{
		return Ar << RedGreenBlue.R << RedGreenBlue.G << RedGreenBlue.B;
	}

	FPlane Plane() const
	{
		return FPlane(R.Float(),G.Float(),B.Float(),1.0f);
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R.Float();
	}
	void InvertGreen()
	{
		G = 1.0f-G.Float();
	}
	void InvertBlue()
	{
		B = 1.0f-B.Float();
	}
	void InvertAlpha()
	{
	}

	// Component access.
	FHalf Red() const
	{
		return R;
	}
	FHalf Green() const
	{
		return G;
	}
	FHalf Blue() const
	{
		return B;
	}
	FHalf Alpha() const
	{
		return 1.0f;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 3;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_RGB16_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRedGreenBlue16F::String);
		return FString::Printf( TEXT("(R=%f,G=%f,B=%f)"), R.Float(), G.Float(), B.Float() );
		unguard;
	}
};

// A half floating point rg value.
class FRedGreen16F
{
public:
	// Variables.
	union
	{
		FHalf R;
		FHalf X;
	};
	union
	{
		FHalf G;
		FHalf Y;
	};

	// Constructors.
	FRedGreen16F()
	{}
	FRedGreen16F( const FPlane& InP )
	:	R(InP.X)
	,	G(InP.Y)
	{}
	FRedGreen16F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	, G(InG)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRedGreen16F& RedGreen )
	{
		return Ar << RedGreen.R << RedGreen.G;
	}

	FPlane Plane() const
	{
		return FPlane(R.Float(),G.Float(),0.0f,1.0f);
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R.Float();
	}
	void InvertGreen()
	{
		G = 1.0f-G.Float();
	}
	void InvertBlue()
	{
	}
	void InvertAlpha()
	{
	}

	// Component access.
	FHalf Red() const
	{
		return R;
	}
	FHalf Green() const
	{
		return G;
	}
	FHalf Blue() const
	{
		return 0.0f;
	}
	FHalf Alpha() const
	{
		return 1.0f;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 2;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_RG16_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRedGreen16F::String);
		return FString::Printf( TEXT("(R=%f,G=%f)"), R.Float(), G.Float() );
		unguard;
	}
};

// A half floating point red value.
class FRed16F
{
public:
	// Variables.
	union
	{
		FHalf R;
		FHalf X;
	};

	// Constructors.
	FRed16F()
	{}
	FRed16F( const FPlane& InP )
	:	R(InP.X)
	{}
	FRed16F( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(InR)
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRed16F& Red )
	{
		return Ar << Red.R;
	}

	FPlane Plane() const
	{
		return FPlane(R.Float(),0.0f,0.0f,1.0f);
	}

	// Channel inversion routines. Assume color to be normalized in [0,1].
	void InvertRed()
	{
		R = 1.0f-R.Float();
	}
	void InvertGreen()
	{
	}
	void InvertBlue()
	{
	}
	void InvertAlpha()
	{
	}

	// Component access.
	FHalf Red() const
	{
		return R;
	}
	FHalf Green() const
	{
		return 0.0f;
	}
	FHalf Blue() const
	{
		return 0.0f;
	}
	FHalf Alpha() const
	{
		return 1.0f;
	}

	// Pink masking.
	void PinkMask()
	{
	}

	// Type interface.
	static INT Colors()
	{
		return 1;
	}
	static INT ColorBytes()
	{
		return sizeof(FLOAT)*Colors();
	}
	static BYTE Format()
	{
		return TEXF_R16_F;
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRed16F::String);
		return FString::Printf( TEXT("(R=%f)"), R.Float() );
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	Odd ducklings.
-----------------------------------------------------------------------------*/

//
// A RGB565 Color.
//
class FRed5Green6Blue5
{
public:
	// Variables.
	union
	{
		BITFIELD R:5;
		BITFIELD X:5;
		BITFIELD S:5;
	};
	union
	{
		BITFIELD G:6;
		BITFIELD Y:6;
		BITFIELD T:6;
	};
	union
	{
		BITFIELD B:5;
		BITFIELD Z:5;
		BITFIELD P:5;
	};

	// Constructors.
	FRed5Green6Blue5()
	{}
	FRed5Green6Blue5( INT InR, INT InG=0, INT InB=0, INT InA=0 )
	: R(Clamp<INT>(InR,RedMin(),RedMax()))
	, G(Clamp<INT>(InG,GreenMin(),GreenMax()))
	, B(Clamp<INT>(InB,BlueMin(),BlueMax()))
	{}
	// Input is assumed to be in [0,1] range for unsigned and, [-1,1] for signed types. For signed types -1 will map to TYPE_MIN+1, while 0 will be preserved.
	FRed5Green6Blue5( EC_NormalizedColor, FLOAT InR, FLOAT InG=0.0, FLOAT InB=0.0, FLOAT InA=1.0 )
	:	R(Clamp<INT>(appRound(InR*RedMax()),RedMin(),RedMax()))
	, G(Clamp<INT>(appRound(InG*GreenMax()),GreenMin(),GreenMax()))
	, B(Clamp<INT>(appRound(InB*BlueMax()),BlueMin(),BlueMax()))
	{}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRed5Green6Blue5& C )
	{
		return Ar << GET_COLOR_WORD(C);
	}

	// Operators.
	UBOOL operator==( const FRed5Green6Blue5& C ) const
	{
		return R==C.R && G==C.G && B==C.B;
	}
	UBOOL operator!=( const FRed5Green6Blue5& C ) const
	{
		return R!=C.R || G!=C.G || B!=C.B;
	}

	// Routines.
	FColor32F Color() const
	{
		return FColor32F( R/DOUBLE(RedMax()), G/DOUBLE(GreenMax()), B/DOUBLE(BlueMax()), 1.0 );
	}
	FPlane Plane() const
	{
		return FPlane( R/DOUBLE(RedMax()), G/DOUBLE(GreenMax()), B/DOUBLE(BlueMax()), 1.0 );
	}

	//
	// Expansion, mostly intended for texture decompression routines,
	// for formats which won't specify a bit precise encoding, such as BC1-BC5.
	//
	// Numeric verfication for the integer routines is found inside
	// Nerf 300a/Help/Development/RGB565Expand.ods
	// 
	FColor ExpandRGBA8() const
	{
		// Integer math version of above.
		return FColor
		(
			(R * 255 + 15) / 31, // Integer version of appRound(255.0*R/31.0).
			(G * 255 + 31) / 63, // Integer version of appRound(255.0*R/63.0).
			(B * 255 + 15) / 31, // Integer version of appRound(255.0*R/31.0).
			255
		);
	}
	FColor16 ExpandRGBA16() const
	{
		// Integer math version of above.
		return FColor16
		(
			(R * 65535 + 15) / 31,
			(G * 65535 + 31) / 63,
			(B * 65535 + 15) / 31,
			65535
		);
	}
	FColor32F ExpandRGBA32F() const
	{
		return FColor32F(R / 32.0, G / 63.0, B / 31.0, 1.0);
	}
	static FColor ExpandSumRGBA8(FRed5Green6Blue5 RGB0, FRed5Green6Blue5 RGB1)
	{
		return FColor
		(
			((RGB0.R + RGB1.R) * 255 + 31) / 62,  // Integer version of appRound(255.0*((R0+R1)/2.0)/31.0) = appRound(255.0*(R0+R1)/62.0).
			((RGB0.G + RGB1.G) * 255 + 63) / 126, // Integer version of appRound(255.0*((G0+G1)/2.0)/63.0) = appRound(255.0*(G0+G1)/126.0).
			((RGB0.B + RGB1.B) * 255 + 31) / 62,  // Integer version of appRound(255.0*((B0+B1)/2.0)/31.0) = appRound(255.0*(B0+B1)/62.0).
			255
		);
	}
	static FColor16 ExpandSumRGBA16(FRed5Green6Blue5 RGB0, FRed5Green6Blue5 RGB1)
	{
		return FColor16
		(
			((RGB0.R + RGB1.R) * 65535 + 31) / 62,
			((RGB0.G + RGB1.G) * 65535 + 63) / 126,
			((RGB0.B + RGB1.B) * 65535 + 31) / 62,
			65535
		);
	}
	static FColor ExpandSumRGBA8(FRed5Green6Blue5 RGB0, FRed5Green6Blue5 RGB1, FRed5Green6Blue5 RGB2)
	{
		return FColor
		(
			((RGB0.R + RGB1.R + RGB2.R) * 255 + 46) / 93,  // Integer version of appRound(255.0*((R0+R1+R2)/3.0)/31.0) = appRound(255.0*(R0+R1+R2)/93.0).
			((RGB0.G + RGB1.G + RGB2.G) * 255 + 94) / 189, // Integer version of appRound(255.0*((G0+G1+G2)/3.0)/63.0) = appRound(255.0*(G0+G1+G2)/189.0).
			((RGB0.B + RGB1.B + RGB2.B) * 255 + 46) / 93,  // Integer version of appRound(255.0*((B0+B1+B2)/3.0)/31.0) = appRound(255.0*(B0+B1+B2)/93.0).
			255
		);
	}
	static FColor16 ExpandSumRGBA16(FRed5Green6Blue5 RGB0, FRed5Green6Blue5 RGB1, FRed5Green6Blue5 RGB2)
	{
		return FColor16
		(
			((RGB0.R + RGB1.R + RGB2.R) * 65535 + 46) / 93,
			((RGB0.G + RGB1.G + RGB2.G) * 65535 + 94) / 189,
			((RGB0.B + RGB1.B + RGB2.B) * 65535 + 46) / 93,
			255
		);
	}

	// Channel inversion routines.
	void InvertRed()
	{
		R = RedMax()-R;
	}
	void InvertGreen()
	{
		G = GreenMax()-G;
	}
	void InvertBlue()
	{
		B = BlueMax()-B;
	}
	void InvertAlpha()
	{
	}

	// Component access.
	INT Red() const
	{
		return R;
	}
	INT Green() const
	{
		return G;
	}
	INT Blue() const
	{
		return B;
	}

	// Type interface.
	static INT Colors()
	{
		return 3;
	}
	static INT ColorBytes()
	{
		return 2;
	}
	static BYTE Format()
	{
		return TEXF_R5G6B5;
	}
	static INT RedMin()
	{
		return 0;
	}
	static INT RedMax()
	{
		return 31;
	}
	static INT GreenMin()
	{
		return 0;
	}
	static INT GreenMax()
	{
		return 63;
	}
	static INT BlueMin()
	{
		return 0;
	}
	static INT BlueMax()
	{
		return 31;
	}

	FRed5Green6Blue5 BGR() const
	{
		return FRed5Green6Blue5(B,G,R);
	}

	// Returns a string description.
	FString String() const
	{
		guard(FRed5Green6Blue5::String);
		return FString::Printf( TEXT("(R=%i,G=%i,B=%i)"), R, G, B );
		unguard;
	}
};

//
// A RGB5A1 Color.
//
class FRedGreenBlue5Alpha1
{
public:
	BITFIELD R:5;
	BITFIELD G:5;
	BITFIELD B:5;
	BITFIELD A:1;
};

//
// A RGB10A2 Color.
//
//
// A RGB10A2 Color.
//
struct FRedGreenBlue10Alpha2
{
	FRedGreenBlue10Alpha2()
	{}
	FRedGreenBlue10Alpha2(BITFIELD InR, BITFIELD InG, BITFIELD InB, BITFIELD InA = 0)
		: R(InR), G(InG), B(InB), A(InA)
	{}
	FRedGreenBlue10Alpha2(const FVector& InV, BITFIELD InA = 0)
		: R(Clamp<INT>(appFloor(InV.X * 1023.f), 0, 1023))
		, G(Clamp<INT>(appFloor(InV.Y * 1023.f), 0, 1023))
		, B(Clamp<INT>(appFloor(InV.Z * 1023.f), 0, 1023))
		, A(InA)
	{}

	BITFIELD R : 10;
	BITFIELD G : 10;
	BITFIELD B : 10;
	BITFIELD A : 2;
};

/*----------------------------------------------------------------------------
	FGlobalColors.
----------------------------------------------------------------------------*/

//
// Predefined set of frequently used colors.
//
struct FGlobalColors
{
	// Opaque Colors.
	static FColor32F Black()      { return FColor32F( 0.0f, 0.0f, 0.0f, 1.0f ); } 
	static FColor32F Grey()       { return FColor32F( 0.5f, 0.5f, 0.5f, 1.0f ); }
	static FColor32F White()      { return FColor32F( 1.0f, 1.0f, 1.0f, 1.0f ); }
	static FColor32F Red()        { return FColor32F( 1.0f, 0.0f, 0.0f, 1.0f ); }
	static FColor32F Green()      { return FColor32F( 0.0f, 1.0f, 0.0f, 1.0f ); }
	static FColor32F Blue()       { return FColor32F( 0.0f, 0.0f, 1.0f, 1.0f ); }

	// Clear Colors.
	static FColor32F Clear()      { return FColor32F( 0.0f, 0.0f, 0.0f, 0.0f ); } // Name used by Porter Duff Paper.
	static FColor32F ClearBlack() { return FColor32F( 0.0f, 0.0f, 0.0f, 0.0f ); } 
	static FColor32F ClearGrey()  { return FColor32F( 0.5f, 0.5f, 0.5f, 0.0f ); }
	static FColor32F ClearWhite() { return FColor32F( 1.0f, 1.0f, 1.0f, 0.0f ); }
	static FColor32F ClearRed()   { return FColor32F( 1.0f, 0.0f, 0.0f, 0.0f ); }
	static FColor32F ClearGreen() { return FColor32F( 0.0f, 1.0f, 0.0f, 0.0f ); }
	static FColor32F ClearBlue()  { return FColor32F( 0.0f, 0.0f, 1.0f, 0.0f ); }
};

/*----------------------------------------------------------------------------
	Color helper functions.
----------------------------------------------------------------------------*/

inline FColor32F FSaturate( const FColor32F& Color, const UBOOL Alpha=1 )
{
	return FColor32F
	(
		Clamp(Color.R,0.f,1.f),
		Clamp(Color.G,0.f,1.f),
		Clamp(Color.B,0.f,1.f),
		Alpha ? Clamp(Color.A,0.f,1.f) : Color.A
	);
}

// Clamps value between 0 and 1 by scaling values above 1. LEAVES ALPHA CHANNEL UNTOUCHED.
inline FColor32F FSaturateScale( const FColor32F& Color )
{
	//debugf( TEXT("FSaturateScale( %ls )"), *Color.String() );
	FLOAT T = Max(Max(Color.R,Color.G),Max(Color.B,1.f));
	return FColor32F
	(
		Max(0.f,Color.R)/T,
		Max(0.f,Color.G)/T,
		Max(0.f,Color.B)/T,
		Color.A
	);
}

//
// !! FIX-ME: ecRGB variants.
//

/*----------------------------------------------------------------------------
	Gamma transfer functions.
----------------------------------------------------------------------------*/

inline FColor32F GammaCompress( const FColor32F& Color, const FLOAT Gamma )
{
	FColor32F Result; FLOAT Exp=1.f/Gamma;
	Result.R = appPow(Color.R,Exp);
	Result.G = appPow(Color.G,Exp);
	Result.B = appPow(Color.B,Exp);
	Result.A = Color.A;
	return Result;
}
inline FColor32F GammaDecompress( const FColor32F& Color, const FLOAT Gamma )
{
	FColor32F Result;
	Result.R = appPow(Color.R,Gamma);
	Result.G = appPow(Color.G,Gamma);
	Result.B = appPow(Color.B,Gamma);
	Result.A = Color.A;;
	return Result;
}

//
// Piecewise gamma transfer functions for sRGB colorspace.
//
// https://www.w3.org/Graphics/Color/sRGB.html
// https://www.opengl.org/registry/specs/EXT/texture_sRGB.txt
//
// OpenGL Extension actually uses slightly modified step values,
// for the conversation which address the missmatch of the two 
// piecewise defined segments found in the W3 document.
//
// TODO:
//  * Verify if step value is on the correct side (minor importance)
//  * Check which values are used inside IEC 61966-2-1.
//
inline FColor32F GammaCompress_sRGB( const FColor32F Color )
{
	FColor32F Result;
	Result.R = Color.R<=0.0031308f ? 12.92f*Color.R : 1.055f*appPow(Color.R,0.41666f)-0.055f;
	Result.G = Color.G<=0.0031308f ? 12.92f*Color.G : 1.055f*appPow(Color.G,0.41666f)-0.055f;
	Result.B = Color.B<=0.0031308f ? 12.92f*Color.B : 1.055f*appPow(Color.B,0.41666f)-0.055f;
	Result.A = Color.A;
	return Result;
}
inline FColor32F GammaDecompress_sRGB( const FColor32F Color )
{
	FColor32F Result;
	Result.R = Color.R<=0.04045f ? Color.R/12.92f : appPow((Color.R+0.055f)/1.055f,2.4f);
	Result.G = Color.G<=0.04045f ? Color.G/12.92f : appPow((Color.G+0.055f)/1.055f,2.4f);
	Result.B = Color.B<=0.04045f ? Color.B/12.92f : appPow((Color.B+0.055f)/1.055f,2.4f);
	Result.A = Color.A;
	return Result;
}

/*-----------------------------------------------------------------------------
	Colorspace conversation routines.
-----------------------------------------------------------------------------*/

// Serious shit to goes here. --han

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
