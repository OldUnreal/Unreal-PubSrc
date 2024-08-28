/*=============================================================================
	UnMath.h: Unreal math routines
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Defintions.
-----------------------------------------------------------------------------*/

#if MACOSXPPC
#include <ppc_intrinsics.h>
#endif

// Forward declarations.
class  FVector;
class  FPlane;
class  FCoords;
class  FRotator;
class  FScale;
class  FGlobalMath;
class  FMatrix;
class  FQuat;

#if !DEFINED_appRound
inline INT appRound(FLOAT Value)
{
	return (INT)floor(Value + 0.5);
}
#endif

// Fixed point conversion.
inline	INT Fix		(INT A)			{return A<<16;};
inline	INT Fix		(FLOAT A)		{return appRound(A*65536.f);};
inline	INT Unfix	(INT A)			{return A>>16;};

// Constants.
#undef  PI
#define PI 					(3.1415926535897932f)
#define SMALL_NUMBER		(1.e-8f)
#define KINDA_SMALL_NUMBER	(1.e-4f)

// Magic numbers for numerical precision.
#define DELTA			(0.00001f)
#define SLERP_DELTA		(0.0001f)

#if VECTOR_ALIGNMENT==16 && OLDUNREAL_AMD64
#define VECTOR_SSE_MATHS 1
#endif

/*-----------------------------------------------------------------------------
	Math functions.
-----------------------------------------------------------------------------*/

inline DOUBLE appExp(DOUBLE Value)
{
	return exp(Value);
}
inline DOUBLE appLoge(DOUBLE Value)
{
	return log(Value);
}
inline DOUBLE appFmod(DOUBLE A, DOUBLE B)
{
	return fmod(A, B);
}
inline DOUBLE appSin(DOUBLE Value)
{
	return sin(Value);
}
inline DOUBLE appCos(DOUBLE Value)
{
	return cos(Value);
}
inline DOUBLE appAcos(DOUBLE Value)
{
	return acos(Value);
}
inline DOUBLE appTan(DOUBLE Value)
{
	return tan(Value);
}
inline DOUBLE appAtan(DOUBLE Value)
{
	return atan(Value);
}
inline DOUBLE appAtan2(DOUBLE Y, DOUBLE X)
{
	return atan2(Y, X);
}
inline DOUBLE appSinDeg(DOUBLE Value)
{
	return sin(Value * PI / 180);
}
inline DOUBLE appCosDeg(DOUBLE Value)
{
	return  cos(Value * PI / 180.0);
}
inline DOUBLE appAcosDeg(DOUBLE Value)
{
	return  cos(Value * PI / 180.0);
}
inline DOUBLE appTanDeg(DOUBLE Value)
{
	return tan(Value * PI / 180.0);
}
inline DOUBLE appAtanDeg(DOUBLE Value)
{
	return atan(Value * PI / 180.0);
}
inline DOUBLE appAtan2Deg(DOUBLE Y, DOUBLE X)
{
	return atan2(Y, X) * 180 / PI;
}

#if MACOSXPPC
inline DOUBLE appSqrt(DOUBLE Value)
{
	return(__fres(__frsqrte(Value)));
}
#elif 1
#define appSqrt sqrt
#else
// Source: https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
// Marco: Futher benchmarking seams to show C sqrt is just faster.
double inline __declspec (naked) __fastcall appSqrt(double n)
{
	_asm fld qword ptr[esp + 4]
	_asm fsqrt
	_asm ret 8
}
#endif

inline DOUBLE appPow(DOUBLE A, DOUBLE B)
{
	return pow(A, B);
}
inline UBOOL appIsNan(DOUBLE Value)
{
#if _MSC_VER
	return _isnan(Value) == 1;
#else
	return isnan(Value) == 1;
#endif
}
inline INT appRand()
{
	return rand();
}
inline FLOAT appFrand()
{
	return rand() / (FLOAT)RAND_MAX;
}
inline FLOAT appRandRange(FLOAT Min, FLOAT Max)
{
	return Min + (Max - Min) * appFrand();
}
inline INT appRandRange(INT Min, INT Max)
{
	INT Range = Max - Min;
#if RAND_MAX==0x7fff // Windows.
	INT R = Range > 0 ? (((Range > RAND_MAX) ? ((appRand() << 15) | appRand()) : appRand()) % Range) : 0;
#else
	INT R = Range > 0 ? (appRand() % Range) : 0;
#endif
	return R + Min;
}

#if !DEFINED_appFloor
inline INT appFloor(FLOAT f)
{
#if MACOSXPPC
	// Sanjay Patel from Apple says this gives a tiny performance boost. --ryan.
	float zero = f - f;
	if (f < zero)
	{
		// negative values need to handled properly
		return (INT)floor(f);
	}
	else
	{
		// positive values just get rounded down to the nearest integer
		return (INT)(f);
	}
#elif __PSX2_EE__
	register int r;
	__asm__ __volatile__(
		"cvt.w.s %1,%1 \n\t"
		"mfc1 %0,%1    \n\t"
		:"=r"(r)
		: "$f"(f)
	);
	return r;
#else
    //
    // stijn: check if the source number can be represented as a 32-bit integer.
    //
    // Note 1: MSVC6 generates a _ftol call to perform the float-to-integral conversion and then discards the upper 32-bit half of the result.
    // MSVC2019 generates a CVTTSS2SI opcode to perform the conversion (on x86). This opcode generates a floating point exception when the source number is too big to be represented as a float.
    // If we mask the exception, we get a return value of -1.#IND.
    //
    // Note 2: MAXINT cannot be represented as a IEEE-754 float, so we check for 2147483520.f instead...
    //

	return (f > 2147483520.f) ? MAXINT : (INT)floor(f);
#endif
}
#endif

#if !DEFINED_appCeil
inline INT appCeil(FLOAT Value)
{
	return (INT)ceil(Value);
}
#endif

/*-----------------------------------------------------------------------------
	Global functions.
-----------------------------------------------------------------------------*/

inline FLOAT Splerp( FLOAT F )
{
	FLOAT S = Square(F);
	return (1.0/16.0)*S*S - (1.0/2.0)*S + 1;
}

inline INT ReduceAngleX( INT Angle )
{
	Angle &= 65535;
	return (Angle<32768 ? Angle : (Angle-65535));
}

//
// Snap a value to the nearest grid multiple.
//
inline FLOAT FSnap( FLOAT Location, FLOAT Grid )
{
	if( Grid==0.f )	return Location;
	else			return appFloor((Location + 0.5f*Grid)/Grid)*Grid;
}

//
// Internal sheer adjusting function so it snaps nicely at 0 and 45 degrees.
//
inline FLOAT FSheerSnap (FLOAT Sheer)
{
	if		(Sheer < -0.65f) return Sheer + 0.15f;
	else if (Sheer > +0.65f) return Sheer - 0.15f;
	else if (Sheer < -0.55f) return -0.50f;
	else if (Sheer > +0.55f) return 0.50f;
	else if (Sheer < -0.05f) return Sheer + 0.05f;
	else if (Sheer > +0.05f) return Sheer - 0.05f;
	else					 return 0.f;
}

//
// Find the closest power of 2 that is >= N.
//
inline DWORD FNextPowerOfTwo( DWORD N )
{
	if (N<=0L		) return 0L;
	if (N<=1L		) return 1L;
	if (N<=2L		) return 2L;
	if (N<=4L		) return 4L;
	if (N<=8L		) return 8L;
	if (N<=16L	    ) return 16L;
	if (N<=32L	    ) return 32L;
	if (N<=64L 	    ) return 64L;
	if (N<=128L     ) return 128L;
	if (N<=256L     ) return 256L;
	if (N<=512L     ) return 512L;
	if (N<=1024L    ) return 1024L;
	if (N<=2048L    ) return 2048L;
	if (N<=4096L    ) return 4096L;
	if (N<=8192L    ) return 8192L;
	if (N<=16384L   ) return 16384L;
	if (N<=32768L   ) return 32768L;
	if (N<=65536L   ) return 65536L;
	else			  return 0;
}

//
// Add to a word angle, constraining it within a min (not to cross)
// and a max (not to cross).  Accounts for funkyness of word angles.
// Assumes that angle is initially in the desired range.
//
inline _WORD FAddAngleConfined( INT Angle, INT Delta, INT MinThresh, INT MaxThresh )
{
	if( Delta < 0 )
	{
		if( Delta<=-0x10000L || Delta<=-(INT)((_WORD)(Angle-MinThresh)))
			return (_WORD)MinThresh;
	}
	else if( Delta > 0 )
	{
		if( Delta>=0x10000L || Delta>=(INT)((_WORD)(MaxThresh-Angle)))
			return (_WORD)MaxThresh;
	}
	return (_WORD)(Angle+Delta);
}

//
// Eliminate all fractional precision from an angle.
//
inline INT ReduceAngle( INT Angle );

//
// Fast 32-bit float evaluations.
// Warning: likely not portable, and useful on Pentium class processors only.
//

inline UBOOL IsSmallerPositiveFloat(float F1,float F2)
{
#if 1
	return (F1 < F2);
#else
	return ( (*(DWORD*)&F1) < (*(DWORD*)&F2));
#endif
}

inline FLOAT MinPositiveFloat(float F1, float F2)
{
#if 1
	//#define MinPositiveFloat(f1,f2) __fsel(f2-f1,f1,f2)
	if ( F1 < F2 ) return F1; else return F2;
#else
	if ( (*(DWORD*)&F1) < (*(DWORD*)&F2)) return F1; else return F2;
#endif
}

inline UBOOL u_isnan(double Value)
{
#if _MSC_VER
	return _isnan(Value) == 1;
#else
	return isnan(Value) == 1;
#endif
}

//
// Warning: 0 and -0 have different binary representations.
//

inline UBOOL approximatelyEqualFloat(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

inline UBOOL EqualPositiveFloat(float F1, float F2)
{
#if 1
    return approximatelyEqualFloat(F1,F2, 0.0005);
#else
	return ( *(DWORD*)&F1 == *(DWORD*)&F2 );
#endif
}

inline UBOOL IsNegativeFloat(float F1)
{
#if 1
	return (F1 < 0.0f);
#else
	return ( (*(DWORD*)&F1) >= (DWORD)0x80000000 ); // Detects sign bit.
#endif
}

inline FLOAT MaxPositiveFloat(float F1, float F2)
{
#if 1
    //#define MaxPositiveFloat(f1,f2) __fsel(f1-f2,f1,f2)
	if ( F1 < F2 ) return F2; else return F1;
#else
	if ( (*(DWORD*)&F1) < (*(DWORD*)&F2)) return F2; else return F1;
#endif
}

// Clamp F0 between F1 and F2, all positive assumed.
inline FLOAT ClampPositiveFloat(float F0, float F1, float F2)
{
#if 1
	if		( F0 < F1 ) return F1;
	else if ( F0 > F2 ) return F2;
	else return F0;
#else
	if      ( (*(DWORD*)&F0) < (*(DWORD*)&F1)) return F1;
	else if ( (*(DWORD*)&F0) > (*(DWORD*)&F2)) return F2;
	else return F0;
#endif
}

// Clamp any float F0 between zero and positive float Range
#if 1
#define ClipFloatFromZero(F0,Range)\
{\
	if ( F0 < 0.0f ) F0 = 0.0f;\
	else if	( F0 > Range) F0 = Range;\
}
#else
#define ClipFloatFromZero(F0,Range)\
{\
	if ( (*(DWORD*)&F0) >= (DWORD)0x80000000) F0 = 0.f;\
	else if	( (*(DWORD*)&F0) > (*(DWORD*)&Range)) F0 = Range;\
}
#endif


/*------------------------------------------------------------------------------------
Approximate math functions.
------------------------------------------------------------------------------------*/

//
// Famous Fast inverse square root.
//
// A bit on the history of it:
// https://www.beyond3d.com/content/articles/8/ (tldr author unknown)
//
// A bit more detailed view, constant derivation and error bounds are
// discussed here:
// https://www.lomont.org/Math/Papers/2003/InvSqrt.pdf
//
// Nomenclature: Nn, where n is the number of newton iterations.
//
// max rel. err.
//   n=0: (unknown) Large.
//   n=1: 0.175124  Rather large for resonable work, but should be enough for rejection work.
//   n=2: 0.000465  Quite good.
//   n=3: (unknown) Should be enough for almost everything.
//
// So use n=2 as a about safe default.
//

inline FLOAT appFastInvSqrtN0(FLOAT X)
{
	INT I = *(INT*)&X;

	// Trick.
	I = 0x5f375a86 - (I >> 1);
	X = *(FLOAT*)&I;

	// No Newton iteration.
	return X;
}
inline FLOAT appFastInvSqrtN1(FLOAT X)
{
	FLOAT H = 0.5f*X; INT I = *(INT*)&X;

	// Trick.
	I = 0x5f375a86 - (I >> 1);
	X = *(FLOAT*)&I;

	// Single Newton iteration.
	X = X*(1.5f - H*X*X);
	return X;
}
inline FLOAT appFastInvSqrtN2(FLOAT X)
{
	FLOAT H = 0.5f*X; INT I = *(INT*)&X;

	// Trick.
	I = 0x5f375a86 - (I >> 1);
	X = *(FLOAT*)&I;

	// Two Newton iterations.
	X = X*(1.5f - H*X*X);
	X = X*(1.5f - H*X*X);
	return X;
}
inline FLOAT appFastInvSqrtN3(FLOAT X)
{
	FLOAT H = 0.5f*X; INT I = *(INT*)&X;

	// Trick.
	I = 0x5f375a86 - (I >> 1);
	X = *(FLOAT*)&I;

	// Three Newton iterations.
	X = X*(1.5f - H*X*X);
	X = X*(1.5f - H*X*X);
	X = X*(1.5f - H*X*X);
	return X;
}

#if MACOSXPPC
#define appFastInvSqrt(x) __frsqrte(x)
#else
#define appFastInvSqrt(x) appFastInvSqrtN2(x)
#endif

// Conveniance.
inline FLOAT appFastSqrt(FLOAT X) { return 1.f / appFastInvSqrt(X); }
inline FLOAT appFastSqrtN0(FLOAT X) { return 1.f / appFastInvSqrtN0(X); }
inline FLOAT appFastSqrtN1(FLOAT X) { return 1.f / appFastInvSqrtN1(X); }
inline FLOAT appFastSqrtN2(FLOAT X) { return 1.f / appFastInvSqrtN2(X); }
inline FLOAT appFastSqrtN3(FLOAT X) { return 1.f / appFastInvSqrtN3(X); }

/*-----------------------------------------------------------------------------
	FVector.
-----------------------------------------------------------------------------*/

// Information associated with a floating point vector, describing its
// status as a point in a rendering context.
enum EVectorFlags
{
	FVF_OutXMin		= 0x04,	// Outcode rejection, off left hand side of screen.
	FVF_OutXMax		= 0x08,	// Outcode rejection, off right hand side of screen.
	FVF_OutYMin		= 0x10,	// Outcode rejection, off top of screen.
	FVF_OutYMax		= 0x20,	// Outcode rejection, off bottom of screen.
	FVF_OutNear     = 0x40, // Near clipping plane.
	FVF_OutFar      = 0x80, // Far clipping plane.
	FVF_OutReject   = (FVF_OutXMin | FVF_OutXMax | FVF_OutYMin | FVF_OutYMax), // Outcode rejectable.
	FVF_OutSkip		= (FVF_OutXMin | FVF_OutXMax | FVF_OutYMin | FVF_OutYMax), // Outcode clippable.
};

#if VECTOR_SSE_MATHS
#define MM_3D_MASK         _mm_set_epi32(0x00000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF)
#define _mm_pshufd_ps(v,i) _mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128(v), i))

inline __m128 _mm_coords_sum_ps(__m128 x)
{
	__m128 w = _mm_shuffle_ps(x, x, 0b10110001); //x,y,z,w >> y,x,w,z
	//	__m128 w = _mm_pshufd_ps( v, 0b10110001); /*SSE2 version*/
	x = _mm_add_ps(x, w); // x+y,-,z+w,-
	w = _mm_movehl_ps(w, x); // >> z+w,-,-,-
	w = _mm_add_ss(w, x); // x+y+z+w,-,-,-
	return w;
}
inline __m128 _mm_coords_sum_xy_ps(__m128 x)
{
	__m128 w = _mm_shuffle_ps(x, x, 0b10110001); //x,y,z,w >> y,x,w,z
	//	__m128 w = _mm_pshufd_ps( v, 0b10110001); /*SSE2 version*/
	x = _mm_add_ss(x, w); // x+y,-,-,-
	return x;
}
#endif

class alignas(VECTOR_ALIGNMENT) FVector
{
public:
	// Variables.
	union
	{
		struct { FLOAT X, Y, Z; };
		struct { FLOAT R, G, B; };
	};

#if VECTOR_SSE_MATHS
	// Load FVector to XMM safely (we can specify whether we want to zero the unused float)
	template<UBOOL bZeroW = 1>
	static inline __m128 _mm(const FVector& V)
	{
		__m128 mm = _mm_loadu_ps(&V.X);  //X,Y,Z,?
		if (bZeroW)
			mm = _mm_and_ps(mm, _mm_castsi128_ps(MM_3D_MASK)); //X,Y,Z,0
		return mm;
	}
#endif

	// Constructors.
	FVector()
//	:	X(0.0f), Y(0.0f), Z(0.0f)
	{}

	FVector( FLOAT InX, FLOAT InY, FLOAT InZ )
	:	X(InX), Y(InY), Z(InZ)
	{}

	FVector(const class FVector2D& InV, FLOAT InZ);

	// Binary math operators.
	inline FVector operator^( const FVector& V ) const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		__m128 mm_xyz_t = _mm<false>(*this);
		__m128 mm_xyz_v = _mm<false>(V);
		__m128 mm_yzx_t = _mm_pshufd_ps(mm_xyz_t, 0b11001001); //PSHUFD is larger, but doesn't require a previous copy (via MOVAPS)
		__m128 mm_zxy_v = _mm_pshufd_ps(mm_xyz_v, 0b11010010);
		__m128 mm_zxy_t = _mm_pshufd_ps(mm_xyz_t, 0b11010010);
		__m128 mm_yzx_v = _mm_pshufd_ps(mm_xyz_v, 0b11001001);
		__m128 mm_cross = _mm_sub_ps
		(_mm_mul_ps(mm_yzx_t, mm_zxy_v)
			, _mm_mul_ps(mm_zxy_t, mm_yzx_v));
		_mm_storeu_ps(&Result.X, mm_cross);
		return Result;
#else
		return FVector
		(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X
		);
#endif
	}
	inline FLOAT operator|( const FVector& V ) const
	{
#if VECTOR_SSE_MATHS
		FLOAT Result;
		__m128 mm_mul = _mm_mul_ps(_mm<false>(V), _mm<false>(*this));
		__m128 mm_mul3d = _mm_and_ps(mm_mul, _mm_castsi128_ps(MM_3D_MASK));
		__m128 mm_dot = _mm_coords_sum_ps(mm_mul3d);
		_mm_store_ss(&Result, mm_dot);
		return Result;
#else
		return X*V.X + Y*V.Y + Z*V.Z;
#endif
	}
	friend FVector operator*( FLOAT Scale, const FVector& V )
	{
		return FVector( V.X * Scale, V.Y * Scale, V.Z * Scale );
	}
	inline FVector operator+( const FVector& V ) const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		_mm_storeu_ps(&Result.X, _mm_add_ps(_mm<false>(*this), _mm<false>(V)));
		return Result;
#else
		return FVector( X + V.X, Y + V.Y, Z + V.Z );
#endif
	}
	inline FVector operator-( const FVector& V ) const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		_mm_storeu_ps(&Result.X, _mm_sub_ps(_mm<false>(*this), _mm<false>(V)));
		return Result;
#else
		return FVector( X - V.X, Y - V.Y, Z - V.Z );
#endif
	}
	inline FVector operator*( FLOAT Scale ) const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		_mm_storeu_ps(&Result.X, _mm_mul_ps(_mm<false>(*this), _mm_load1_ps(&Scale)));
		return Result;
#else
		return FVector( X * Scale, Y * Scale, Z * Scale );
#endif
	}
	FVector operator/( FLOAT Scale ) const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		FLOAT RV = 1.f / Scale;
		_mm_storeu_ps(&Result.X, _mm_mul_ps(_mm<false>(*this), _mm_load1_ps(&RV)));
		return Result;
#else
		#if MACOSXPPC
		FLOAT RScale = __fres(Scale);
		#else
		FLOAT RScale = 1.f/Scale;
		#endif
		return FVector( X * RScale, Y * RScale, Z * RScale );
#endif
	}
	inline FVector operator*( const FVector& V ) const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		_mm_storeu_ps(&Result.X, _mm_mul_ps(_mm<false>(*this), _mm<false>(V)));
		return Result;
#else
		return FVector( X * V.X, Y * V.Y, Z * V.Z );
#endif
	}

	inline void DivMultiDbl(DOUBLE V)
	{
		V = 1.0 / V;
		X *= V;
		Y *= V;
		Z *= V;
	}

	// Binary comparison operators.
	UBOOL operator==( const FVector& V ) const
	{
		return (X == V.X) && (Y == V.Y) && (Z == V.Z);
	}
	UBOOL operator!=( const FVector& V ) const
	{
		return (X != V.X) || (Y != V.Y) || (Z != V.Z);
	}

	// Unary operators.
	inline FVector operator-() const
	{
#if VECTOR_SSE_MATHS
		//0x80000000, 0x80000000, 0x80000000, 0x00000000
		constexpr __m128 MM_XOR_SIGN{ -0.f, -0.f, -0.f,  0.f };

		FVector Result;
		_mm_storeu_ps(&Result.X, _mm_xor_ps(_mm<false>(*this), MM_XOR_SIGN));
		return Result;
#else
		return FVector( -X, -Y, -Z );
#endif
	}

	// Assignment operators.
#if VECTOR_SSE_MATHS
	inline FVector& operator=(const FVector& V)
	{
		_mm_storeu_ps(&X, _mm<false>(V));
		return *this;
	}
#endif
	inline FVector operator+=( const FVector& V )
	{
#if VECTOR_SSE_MATHS
		_mm_storeu_ps(&X, _mm_add_ps(_mm<false>(*this), _mm<false>(V)));
#else
		X += V.X; Y += V.Y; Z += V.Z;
#endif
		return *this;
	}
	inline FVector operator-=( const FVector& V )
	{
#if VECTOR_SSE_MATHS
		_mm_storeu_ps(&X, _mm_sub_ps(_mm<false>(*this), _mm<false>(V)));
#else
		X -= V.X; Y -= V.Y; Z -= V.Z;
#endif
		return *this;
	}
	inline FVector operator*=( FLOAT Scale )
	{
#if VECTOR_SSE_MATHS
		_mm_storeu_ps(&X, _mm_mul_ps(_mm<false>(*this), _mm_load1_ps(&Scale)));
#else
		X *= Scale; Y *= Scale; Z *= Scale;
#endif
		return *this;
	}
	FVector operator/=( FLOAT V )
	{
		#if MACOSXPPC
		FLOAT RV = __fres(V);
		#else
		FLOAT RV = 1.f / V;
		#endif
#if VECTOR_SSE_MATHS
		_mm_storeu_ps(&X, _mm_mul_ps(_mm<false>(*this), _mm_load1_ps(&RV)));
#else
		X *= RV; Y *= RV; Z *= RV;
#endif
		return *this;
	}
	FVector operator*=( const FVector& V )
	{
#if VECTOR_SSE_MATHS
		_mm_storeu_ps(&X, _mm_mul_ps(_mm<false>(*this), _mm<false>(V)));
#else
		X *= V.X; Y *= V.Y; Z *= V.Z;
#endif
		return *this;
	}
	FVector operator/=( const FVector& V )
	{
#if VECTOR_SSE_MATHS
		_mm_storeu_ps(&X, _mm_div_ps(_mm<false>(*this), _mm<false>(V)));
#else
		X /= V.X; Y /= V.Y; Z /= V.Z;
#endif
		return *this;
	}

	// Simple functions.
	inline FLOAT Size() const
	{
#if VECTOR_SSE_MATHS
		FLOAT Result;
		__m128 mm_this = _mm(*this);
		__m128 mm_mul = _mm_mul_ps(mm_this, mm_this);
		__m128 mm_dot = _mm_coords_sum_ps(mm_mul);
		_mm_store_ss(&Result, _mm_sqrt_ss(mm_dot));
		return Result;
#else
		return appSqrt( X*X + Y*Y + Z*Z );
#endif
	}
	inline FLOAT SizeSquared() const
	{
#if VECTOR_SSE_MATHS
		FLOAT Result;
		__m128 mm_this = _mm(*this);
		__m128 mm_mul = _mm_mul_ps(mm_this, mm_this);
		__m128 mm_dot = _mm_coords_sum_ps(mm_mul);
		_mm_store_ss(&Result, mm_dot);
		return Result;
#else
		return X*X + Y*Y + Z*Z;
#endif
	}
	inline DOUBLE SizeSquaredD() const
	{
		return X * X + Y * Y + Z * Z;
	}
	inline FLOAT Size2D() const
	{
#if VECTOR_SSE_MATHS
		FLOAT Result;
		__m128 mm_this = _mm<false>(*this);
		__m128 mm_mul = _mm_mul_ps(mm_this, mm_this);
		__m128 mm_dot = _mm_coords_sum_xy_ps(mm_mul);
		_mm_store_ss(&Result, _mm_sqrt_ss(mm_dot));
		return Result;
#else
		return appSqrt( X*X + Y*Y );
#endif
	}
	inline FLOAT SizeSquared2D() const
	{
#if VECTOR_SSE_MATHS
		FLOAT Result;
		__m128 mm_this = _mm<false>(*this);
		__m128 mm_mul = _mm_mul_ps(mm_this, mm_this);
		__m128 mm_dot = _mm_coords_sum_xy_ps(mm_mul);
		_mm_store_ss(&Result, mm_dot);
		return Result;
#else
		return X*X + Y*Y;
#endif
	}
	inline UBOOL IsNearlyZero() const
	{
		return
				Abs(X)<KINDA_SMALL_NUMBER
			&&	Abs(Y)<KINDA_SMALL_NUMBER
			&&	Abs(Z)<KINDA_SMALL_NUMBER;
	}
	inline UBOOL IsZero() const
	{
		return X==0.f && Y==0.f && Z==0.f;
	}
	inline UBOOL IsOne() const
	{
		return X == 1.f && Y == 1.f && Z == 1.f;
	}
	inline UBOOL MoreOrEqualZero() const
	{
		return X >= 0.f && Y >= 0.f && Z >= 0.f;
	}
	inline UBOOL IsNan() const
	{
		return u_isnan(X) || u_isnan(Y) || u_isnan(Z);
	}
	inline UBOOL IsFinite() const
	{
		//A finite value is any floating - point value that is neither infinite nor NaN(Not - A - Number).
		return isfinite(X) && isfinite(Y) && isfinite(Z);
	}
	inline UBOOL Normalize()
	{
		FLOAT SquareSum = X*X+Y*Y+Z*Z;
		if( SquareSum >= SMALL_NUMBER )
		{
			FLOAT Scale = appFastInvSqrt(SquareSum);
			X *= Scale; Y *= Scale; Z *= Scale;
			return 1;
		}
		return 0;
	}
	inline UBOOL NormalizeSlow()
	{
		DOUBLE Scale = X * X + Y * Y + Z * Z;
		if (Scale >= SMALL_NUMBER)
		{
			Scale = 1.0 / appSqrt(Scale);
			X *= Scale; Y *= Scale; Z *= Scale;
			return TRUE;
		}
		return FALSE;
	}
	inline FVector Projection() const
	{
#if VECTOR_SSE_MATHS
		FVector Result;
		const FLOAT One = 1.f;
		__m128 mm_one = _mm_load_ss(&One);    //1,0,0,0
		__m128 mm_this = _mm<false>(*this);    //X,Y,Z,?
		__m128 mm_div = _mm_shuffle_ps(mm_this, mm_one, 0b00001010); //Z,Z,1,1
		_mm_store_ps(&Result.X, _mm_div_ps(mm_this, mm_div)); //X*RZ, Y*RZ, 1, ? 
		return Result;
#else
		FLOAT RZ = 1.f/Z;
		return FVector( X*RZ, Y*RZ, 1 );
#endif
	}
	inline FVector UnsafeNormal() const
	{
		FLOAT Scale = appFastInvSqrt(X*X+Y*Y+Z*Z);
		return FVector( X*Scale, Y*Scale, Z*Scale );
	}
	inline FVector NormalApprox() const // Safe but with slight inaccurancy!
	{
		FLOAT Scale = appFastInvSqrtN0(X * X + Y * Y + Z * Z);
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}
	FVector GridSnap( const FVector& Grid )
	{
		return FVector( FSnap(X, Grid.X),FSnap(Y, Grid.Y),FSnap(Z, Grid.Z) );
	}
	FVector BoundToCube( FLOAT Radius )
	{
		return FVector
		(
			Clamp(X,-Radius,Radius),
			Clamp(Y,-Radius,Radius),
			Clamp(Z,-Radius,Radius)
		);
	}
	void AddBounded( const FVector& V, FLOAT Radius=MAXSWORD )
	{
		*this = (*this + V).BoundToCube(Radius);
	}
	FLOAT& Component( INT Index )
	{
		return (&X)[Index];
	}
	inline FVector Lerp(const FVector& Dest, FLOAT a) const
	{
		return *this + ((Dest - *this) * a);
	}
	inline FVector Slerp(const FVector& Dest, FLOAT a) const
	{
		a = 3.f * a * a - 2.f * a * a * a;
		return *this + ((Dest - *this) * a);
	}
	inline FLOAT SizeFast() const
	{
		return 1.f / appFastInvSqrt(X*X + Y*Y + Z*Z);
	}
	inline FLOAT SizeFastN0() const
	{
		return 1.f / appFastInvSqrtN0(X*X + Y*Y + Z*Z);
	}
	inline FLOAT SizeFastN1() const
	{
		return 1.f / appFastInvSqrtN1(X*X + Y*Y + Z*Z);
	}
	inline FLOAT SizeFastN2() const
	{
		return 1.f / appFastInvSqrtN2(X*X + Y*Y + Z*Z);
	}
	inline FLOAT SizeFastN3() const
	{
		return 1.f / appFastInvSqrtN3(X*X + Y*Y + Z*Z);
	}
	inline FLOAT Size2DFast() const
	{
		return 1.f / appFastInvSqrt(X*X + Y*Y);
	}
	inline FLOAT Size2DFastN0() const
	{
		return 1.f / appFastInvSqrtN0(X*X + Y*Y);
	}
	inline FLOAT Size2DFastN1() const
	{
		return 1.f / appFastInvSqrtN1(X*X + Y*Y);
	}
	inline FLOAT Size2DFastN2() const
	{
		return 1.f / appFastInvSqrtN2(X*X + Y*Y);
	}
	inline FLOAT Size2DFastN3() const
	{
		return 1.f / appFastInvSqrtN3(X*X + Y*Y);
	}
	inline QWORD GetIdentifier() const
	{
		return *reinterpret_cast<const QWORD*>(&X);
	}

	// Return a boolean that is based on the vector's direction.
	// When      V==(0,0,0) Booleanize(0)=1.
	// Otherwise Booleanize(V) <-> !Booleanize(!B).
	inline UBOOL Booleanize()
	{
		return
			X >  0.f ? 1 :
			X <  0.f ? 0 :
			Y >  0.f ? 1 :
			Y <  0.f ? 0 :
			Z >= 0.f ? 1 : 0;
	}

	// Transformation.
	FVector TransformVectorBy( const FCoords& Coords ) const;
	inline FVector TransformPointBy( const FCoords& Coords ) const;
	FVector MirrorByVector( const FVector& MirrorNormal ) const;
	FVector MirrorByPlane( const FPlane& MirrorPlane ) const;
	FVector PivotTransform(const FCoords& Coords) const;

	// Complicated functions.
	FRotator Rotation() const;
	
	//
	// Find good arbitrary axis vectors to represent U and V axes of a plane
	// given just the normal.
	//
	inline void FindBestAxisVectors(FVector& Axis1, FVector& Axis2) const
	{
		FLOAT NX = Abs(X);
		FLOAT NY = Abs(Y);
		FLOAT NZ = Abs(Z);

		// Find best basis vectors.
		if (NZ > NX && NZ > NY)	Axis1 = FVector(1, 0, 0);
		else					Axis1 = FVector(0, 0, 1);

		Axis1 = (Axis1 - *this * (Axis1 | *this)).SafeNormalSlow();
		Axis2 = Axis1 ^ *this;
	}

	inline FVector SafeNormal() const
	{
		FLOAT Scale = X * X + Y * Y + Z * Z;
		if (Scale < SMALL_NUMBER)
			return FVector(0.f, 0.f, 0.f);

		Scale = appFastInvSqrt(Scale);
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}
	inline FVector SafeNormal2D() const
	{
		FLOAT Scale = X * X + Y * Y;
		if (Scale < SMALL_NUMBER)
			return FVector(0.f, 0.f, 0.f);

		Scale = appFastInvSqrt(Scale);
		return FVector(X * Scale, Y * Scale, 0.f);
	}

	// Higher precision normalize.
	inline FVector SafeNormal2DSlow() const
	{
		DOUBLE Scale = X * X + Y * Y;
		if (Scale < SMALL_NUMBER)
			return FVector(0.f, 0.f, 0.f);

		Scale = 1.0 / appSqrt(Scale);
		return FVector(X * Scale, Y * Scale, 0.f);
	}

	// Higher precision normalize.
	inline FVector SafeNormalSlow() const
	{
		DOUBLE Scale = X * X + Y * Y + Z * Z;
		if (Scale < SMALL_NUMBER)
			return FVector(0.f, 0.f, 0.f);

		Scale = 1.0 / appSqrt(Scale);
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	// Higher precision normalize.
	inline FVector UnsafeNormalSlow() const
	{
		DOUBLE Scale = 1.0 / appSqrt(X * X + Y * Y + Z * Z);
		return FVector(X * Scale, Y * Scale, Z * Scale);
	}

	// Euclidean distance between two points.
	inline FLOAT Distance(const FVector& Other) const
	{
#if VECTOR_SSE_MATHS
		return (*this - Other).Size();
#else
		const FLOAT dx = (X - Other.X);
		const FLOAT dy = (Y - Other.Y);
		const FLOAT dz = (Z - Other.Z);
		return appSqrt(dx * dx + dy * dy + dz * dz);
#endif
	}

	// Squared distance between two points.
	inline FLOAT DistSquared(const FVector& Other) const
	{
#if VECTOR_SSE_MATHS
		return (*this - Other).SizeSquared();
#else
		const FLOAT dx = (X - Other.X);
		const FLOAT dy = (Y - Other.Y);
		const FLOAT dz = (Z - Other.Z);
		return (dx * dx + dy * dy + dz * dz);
#endif
	}

	inline FLOAT GetMax() const
	{
		return Max(Abs(X), Max(Abs(Y), Abs(Z)));
	}
	inline FLOAT GetMin() const
	{
		return Min(Abs(X), Min(Abs(Y), Abs(Z)));
	}

	inline UBOOL approximatelyEqual(const FVector& V, const FLOAT Epsilon = 0.00005f) const
	{
		return ((*this - V).GetMax() < Epsilon);
	}

	// Triple product of three vectors.
	inline FLOAT Triple(const FVector& InY, const FVector& InZ) const
	{
#if VECTOR_SSE_MATHS
		return *this | (InY ^ InZ);
#else
		return
			( (X * (InY.Y * InZ.Z - InY.Z * InZ.Y))
			+ (Y * (InY.Z * InZ.X - InY.X * InZ.Z))
			+ (Z * (InY.X * InZ.Y - InY.Y * InZ.X)));
#endif
	}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FVector& V )
	{
		return Ar << V.X << V.Y << V.Z;
	}

	// Returns a string description.
	FString String() const
	{
		guardSlow(FVector::String);
		return FString::Printf(TEXT("(X=%f,Y=%f,Z=%f)"), X, Y, Z);
		unguardSlow;
	}

	// Write compressed networked vector
	inline void SerializeCompressed(FArchive& Ar)
	{
		INT vX(appRound(X)), vY(appRound(Y)), vZ(appRound(Z));
		DWORD Bits = Clamp<DWORD>(appCeilLogTwo(1 + Max(Max(Abs(vX), Abs(vY)), Abs(vZ))), 1, 16) - 1;
		Ar.SerializeInt(Bits, 16);
		INT   Bias = 1 << (Bits + 1);
		DWORD Max = 1 << (Bits + 2);
		DWORD DX(vX + Bias), DY(vY + Bias), DZ(vZ + Bias);
		Ar.SerializeInt(DX, Max);
		Ar.SerializeInt(DY, Max);
		Ar.SerializeInt(DZ, Max);
		if (Ar.IsLoading())
		{
			X = (INT)DX - Bias;
			Y = (INT)DY - Bias;
			Z = (INT)DZ - Bias;
		}
	}
	// Serialize vector as a packed rotation Yaw/Pitch angle.
	void SerializeCompressedNormal(FArchive& Ar);
};

typedef FVector FVectorNormal;

class FVector2D
{
public:
	FLOAT X, Y;

	FVector2D()
	{}
	FVector2D(FLOAT InX, FLOAT InY)
		: X(InX), Y(InY)
	{}
	FVector2D(const FVector& V)
		: X(V.X), Y(V.Y)
	{}

	inline FVector Vector() const
	{
		return FVector(X, Y, 0.f);
	}
	inline void operator+=(const FVector2D& V)
	{
		X += V.X;
		Y += V.Y;
	}
	inline void operator-=(const FVector2D& V)
	{
		X -= V.X;
		Y -= V.Y;
	}
	inline void operator*=(const FVector2D& V)
	{
		X *= V.X;
		Y *= V.Y;
	}
	inline void operator/=(const FVector2D& V)
	{
		X /= V.X;
		Y /= V.Y;
	}
	inline void operator*=(FLOAT V)
	{
		X *= V;
		Y *= V;
	}
	inline void operator/=(FLOAT V)
	{
		X /= V;
		Y /= V;
	}
	friend FVector2D operator*(const FLOAT Scale, const FVector2D& V)
	{
		return FVector2D(V.X * Scale, V.Y * Scale);
	}
	inline FVector2D operator+(const FVector2D& V) const
	{
		return FVector2D(X + V.X, Y + V.Y);
	}
	inline FVector2D operator-(const FVector2D& V) const
	{
		return FVector2D(X - V.X, Y - V.Y);
	}
	inline FVector2D operator*(const FLOAT Scale) const
	{
		return FVector2D(X * Scale, Y * Scale);
	}
	inline FVector2D operator/(const FLOAT Scale) const
	{
		const FLOAT RScale = 1.f / Scale;
		return FVector2D(X * RScale, Y * RScale);
	}
	inline FVector2D operator*(const FVector2D& V) const
	{
		return FVector2D(X * V.X, Y * V.Y);
	}
	inline FLOAT operator|(const FVector2D& V) const
	{
		return X * V.X + Y * V.Y;
	}
	inline FLOAT operator^(const FVector2D& V) const
	{
		return (X * V.Y - Y * V.X);
	}

	inline FLOAT Size() const
	{
		return appSqrt(X * X + Y * Y);
	}
	inline FLOAT SizeSquared() const
	{
		return (X * X + Y * Y);
	}
	inline DOUBLE SizeSquaredDbl() const
	{
		return (Square<DOUBLE>(X) + Square<DOUBLE>(Y));
	}
	inline FVector2D GridSnap(const FVector2D& Grid) const
	{
		return FVector2D(FSnap(X, Grid.X), FSnap(Y, Grid.Y));
	}
	inline FLOAT Distance(const FVector2D& V) const
	{
		return appSqrt(Square(X - V.X) + Square(Y - V.Y));
	}
	inline FLOAT DistSquared(const FVector2D& V) const
	{
		return (Square(X - V.X) + Square(Y - V.Y));
	}
	inline FVector2D CrossAxis() const
	{
		return FVector2D(Y, -X);
	}
	inline FVector2D Flip() const
	{
		return FVector2D(-X, -Y);
	}
	inline FLOAT Triple(const FVector2D& InY, const FVector2D& InZ) const
	{
		return ((X * (InY.Y - InZ.Y))
				+ (Y * (InZ.X - InY.X))
				+ (InY.X * InZ.Y - InY.Y * InZ.X));
	}

	inline UBOOL Normalize()
	{
		DOUBLE Scale = X * X + Y * Y;
		if (Scale >= SMALL_NUMBER)
		{
			Scale = 1.0 / appSqrt(Scale);
			X *= Scale; Y *= Scale;
			return TRUE;
		}
		return FALSE;
	}
	inline FVector2D SafeNormal() const
	{
		DOUBLE Scale = X * X + Y * Y;
		if (Scale >= SMALL_NUMBER)
		{
			Scale = 1.0 / appSqrt(Scale);
			return FVector2D(X * Scale, Y * Scale);
		}
		return FVector2D(0.f, 0.f);
	}
	inline FVector2D UnsafeNormal() const
	{
		const DOUBLE Scale = 1.0 / appSqrt(X * X + Y * Y);
		return FVector2D(X * Scale, Y * Scale);
	}
};

inline FVector::FVector(const FVector2D& InV, FLOAT InZ)
	:X(InV.X), Y(InV.Y), Z(InZ)
{}

struct FPackedInt
{
	INT Value;
	INT MaxValue;
};

/*-----------------------------------------------------------------------------
	FPlane.
-----------------------------------------------------------------------------*/

class alignas(VECTOR_ALIGNMENT) FPlane : public FVector
{
public:
	// Variables.
	FLOAT W;

#if VECTOR_SSE_MATHS
	// Load FPlane to XMM
	static inline __m128 _mm(const FPlane& P)
	{
		return _mm_loadu_ps(&P.X);
	}
#endif

	// Constructors.
	FPlane()
	{}
	inline FPlane( const FPlane& P )
	:	FVector(P)
	,	W(P.W)
	{}
	inline FPlane( const FVector& V )
	:	FVector(V)
	,	W(0)
	{}
	inline FPlane( FLOAT InX, FLOAT InY, FLOAT InZ, FLOAT InW )
	:	FVector(InX,InY,InZ)
	,	W(InW)
	{}
	inline FPlane( const FVector& InNormal, FLOAT InW )
	:	FVector(InNormal), W(InW)
	{}
	inline FPlane( const FVector& InBase, const FVector &InNormal )
	:	FVector(InNormal)
	,	W(InBase | InNormal)
	{}
	inline FPlane( const FVector& A, const FVector& B, const FVector& C )
	:	FVector( ((B-A)^(C-A)).SafeNormalSlow() )
	{
		W = (A | FVector(*this));
	}

	//
	// Convert byte hue-saturation-brightness to floating point red-green-blue.
	//
	static inline FPlane GetHSV(BYTE H, BYTE S, BYTE V)
	{
#if 0
		return FromHSV(H, S, V);
#else
		FLOAT Brightness = V * (1.4f / 255.f);
		Brightness *= 0.7f / (0.01f + appSqrt(Brightness));
		Brightness = Clamp(Brightness, 0.f, 1.f);
		FVector Hue = (H < 86) ? FVector((85 - H) / 85.f, (H - 0) / 85.f, 0) : (H < 171) ? FVector(0, (170 - H) / 85.f, (H - 85) / 85.f) : FVector((H - 170) / 85.f, 0, (255 - H) / 84.f);
		return FPlane((Hue + S / 255.f * (FVector(1, 1, 1) - Hue)) * Brightness, 0);
#endif
	}

	// Same as GetHSV but with precise math.
	static inline FPlane FromHSV(BYTE H, BYTE S, BYTE V)
	{
		const FLOAT fS = 1.f - (FLOAT(S) / 255.f);
		const FLOAT fV = FLOAT(V) / 255.f;
		const FLOAT fH = FLOAT(H) / (255.f / 6.f);
		const FLOAT C = fV * fS;
		const FLOAT X = C * (1.f - Abs<FLOAT>(appFmod(fH, 2.f) - 1.f));
		FPlane Result(0.f, 0.f, 0.f, 0.f);

		if (fH < 1.f)
		{
			Result.X = C;
			Result.Y = X;
		}
		else if (fH < 2.f)
		{
			Result.X = X;
			Result.Y = C;
		}
		else if (fH < 3.f)
		{
			Result.Y = C;
			Result.Z = X;
		}
		else if (fH < 4.f)
		{
			Result.Y = X;
			Result.Z = C;
		}
		else if (fH < 5.f)
		{
			Result.X = X;
			Result.Z = C;
		}
		else
		{
			Result.X = C;
			Result.Z = X;
		}
		const FLOAT m = fV - C;
		if (m > 0.f)
		{
			Result.X += m;
			Result.Y += m;
			Result.Z += m;
			Result.ClampToColor();
		}
		return Result;
	}

	// Operators
	inline FPlane operator-() const
	{
		return FPlane(-X, -Y, -Z, -W);
	}
	friend FPlane operator*(FLOAT Scale, const FPlane& V)
	{
		return FPlane(V.X * Scale, V.Y * Scale, V.Z * Scale, V.W * Scale);
	}
	inline FPlane operator+(const FPlane& V) const
	{
		return FPlane(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
	}
	inline FPlane operator-(const FPlane& V) const
	{
		return FPlane(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
	}
	inline FPlane operator*(FLOAT Scale) const
	{
		return FPlane(X * Scale, Y * Scale, Z * Scale, W * Scale);
	}
	FPlane operator/(FLOAT Scale) const
	{
#if MACOSXPPC
		FLOAT RScale = __fres(Scale);
#else
		FLOAT RScale = 1.f / Scale;
#endif
		return FPlane(X * RScale, Y * RScale, Z * RScale, W * RScale);
	}
	inline FPlane operator*(const FPlane& V) const
	{
		return FPlane(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
	}

	// Functions.
	inline FLOAT PlaneDot( const FVector &P ) const
	{
#if VECTOR_SSE_MATHS
		// Used to fill the W coordinate with -1
		constexpr __m128 MM_PLANEDOT_W{ 0.f, 0.f, 0.f, -1.f };

		FLOAT Result;
		__m128 mm_x_y_z_w = _mm_or_ps(_mm(P), MM_PLANEDOT_W); //PX,PY,PZ,-1
		mm_x_y_z_w = _mm_mul_ps(mm_x_y_z_w, _mm(*this)); //X*PX, Y*PY, Z*PZ, -W   (x,y,z,w)
		_mm_store_ss(&Result, _mm_coords_sum_ps(mm_x_y_z_w));
		return Result;
#else
		return X*P.X + Y*P.Y + Z*P.Z - W;
#endif
	}
	FPlane Flip() const
	{
#if VECTOR_SSE_MATHS
		FPlane Result;
		_mm_storeu_ps(&Result.X, _mm_sub_ps(_mm_setzero_ps(), _mm(*this)));
		return Result;
#else
		return FPlane(-X,-Y,-Z,-W);
#endif
	}
	FPlane TransformPlaneByOrtho( const FCoords &Coords ) const;
	FPlane TransformPlaneByOrthoAdj( const FCoords &Coords, const FCoords &CoordsAdj, bool bFlip ) const;
	UBOOL operator==( const FPlane& V ) const
	{
	    FLOAT Epsilon = 0.00005f;
		return
		(
           approximatelyEqualFloat(X, V.X, Epsilon)
        && approximatelyEqualFloat(Y, V.Y, Epsilon)
        && approximatelyEqualFloat(Z, V.Z, Epsilon)
        && approximatelyEqualFloat(W, V.W, Epsilon)
        );
	}
	UBOOL operator!=( const FPlane& V ) const
	{
	    FLOAT Epsilon = 0.00005f;
		return
		(
           !approximatelyEqualFloat(X, V.X, Epsilon)
        || !approximatelyEqualFloat(Y, V.Y, Epsilon)
        || !approximatelyEqualFloat(Z, V.Z, Epsilon)
        || !approximatelyEqualFloat(W, V.W, Epsilon)
        );
	}
	inline void ClampToColor()
	{
		X = Clamp(X, 0.f, 1.f);
		Y = Clamp(Y, 0.f, 1.f);
		Z = Clamp(Z, 0.f, 1.f);
		W = Clamp(W, 0.f, 1.f);
	}
	inline FPlane Lerp(const FPlane& Dest, const FLOAT Alpha) const
	{
		return *this + (Dest - *this) * Alpha;
	}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FPlane &P )
	{
		return Ar << (FVector&)P << P.W;
	}
};

/*-----------------------------------------------------------------------------
	FCoords.
-----------------------------------------------------------------------------*/

//
// A coordinate system matrix.
//
class alignas(VECTOR_ALIGNMENT) FCoords
{
public:
	FVector	Origin;
	FVector	XAxis;
	FVector YAxis;
	FVector ZAxis;

	// Constructors.
	FCoords() {}
	FCoords( const FVector &InOrigin )
	:	Origin(InOrigin), XAxis(1,0,0), YAxis(0,1,0), ZAxis(0,0,1) {}
	FCoords( const FVector &InOrigin, const FVector &InX, const FVector &InY, const FVector &InZ )
	:	Origin(InOrigin), XAxis(InX), YAxis(InY), ZAxis(InZ) {}
	FCoords(const FVector& Org, const FQuat& Q);

	// Return this coordinate system's transpose.
	// If the coordinate system is orthogonal, this is equivalent to its inverse.
	inline FCoords Transpose() const
	{
		return FCoords
		(
			-Origin.TransformVectorBy(*this),
			FVector(XAxis.X, YAxis.X, ZAxis.X),
			FVector(XAxis.Y, YAxis.Y, ZAxis.Y),
			FVector(XAxis.Z, YAxis.Z, ZAxis.Z)
		);
	}

	// Mirror the coordinates about a normal vector.
	inline FCoords MirrorByVector(const FVector& MirrorNormal) const
	{
		return FCoords
		(
			Origin.MirrorByVector(MirrorNormal),
			XAxis.MirrorByVector(MirrorNormal),
			YAxis.MirrorByVector(MirrorNormal),
			ZAxis.MirrorByVector(MirrorNormal)
		);
	}

	// Mirror the coordinates about a plane.
	inline FCoords MirrorByPlane(const FPlane& Plane) const
	{
		return FCoords
		(
			Origin.MirrorByPlane(Plane),
			XAxis.MirrorByVector(Plane),
			YAxis.MirrorByVector(Plane),
			ZAxis.MirrorByVector(Plane)
		);
	}

	// Determinant of all 3 axes.
	inline FLOAT Determinant() const
	{
		return XAxis.Triple(YAxis, ZAxis);
	}

	// Coordinate system inverse.
	inline FCoords Inverse() const
	{
		FLOAT RDet = 1.f / XAxis.Triple(YAxis, ZAxis);
		return FCoords
		(-Origin.TransformVectorBy(*this)
			, RDet * FVector
				 ((YAxis.Y * ZAxis.Z - YAxis.Z * ZAxis.Y)
				, (ZAxis.Y * XAxis.Z - ZAxis.Z * XAxis.Y)
				, (XAxis.Y * YAxis.Z - XAxis.Z * YAxis.Y))
			, RDet * FVector
				 ((YAxis.Z * ZAxis.X - ZAxis.Z * YAxis.X)
				, (ZAxis.Z * XAxis.X - XAxis.Z * ZAxis.X)
				, (XAxis.Z * YAxis.X - XAxis.X * YAxis.Z))
			, RDet * FVector
				 ((YAxis.X * ZAxis.Y - YAxis.Y * ZAxis.X)
				, (ZAxis.X * XAxis.Y - ZAxis.Y * XAxis.X)
				, (XAxis.X * YAxis.Y - XAxis.Y * YAxis.X))
		);
	}

	// Combine two 'pivot' transforms.
	inline FCoords ApplyPivot(const FCoords& CoordsB) const
	{
		// Fast solution assuming orthogonal coordinate system
		FCoords Temp;
		Temp.Origin = CoordsB.Origin + Origin.TransformVectorBy(CoordsB);
		Temp.XAxis = CoordsB.XAxis.TransformVectorBy(*this);
		Temp.YAxis = CoordsB.YAxis.TransformVectorBy(*this);
		Temp.ZAxis = CoordsB.ZAxis.TransformVectorBy(*this);
		return Temp;
	}

	// Invert pivot transformation
	inline FCoords PivotInverse() const
	{
		FCoords Temp;
		FLOAT RDet = 1.f / XAxis.Triple(YAxis, ZAxis);

		Temp.XAxis = RDet * FVector
			( (YAxis.Y * ZAxis.Z - YAxis.Z * ZAxis.Y)
			, (ZAxis.Y * XAxis.Z - ZAxis.Z * XAxis.Y)
			, (XAxis.Y * YAxis.Z - XAxis.Z * YAxis.Y));

		Temp.YAxis = RDet * FVector
			( (YAxis.Z * ZAxis.X - ZAxis.Z * YAxis.X)
			, (ZAxis.Z * XAxis.X - XAxis.Z * ZAxis.X)
			, (XAxis.Z * YAxis.X - XAxis.X * YAxis.Z));

		Temp.ZAxis = RDet * FVector
			( (YAxis.X * ZAxis.Y - YAxis.Y * ZAxis.X)
			, (ZAxis.X * XAxis.Y - ZAxis.Y * XAxis.X)
			, (XAxis.X * YAxis.Y - XAxis.Y * YAxis.X));

		FVector TOrigin = -Origin;
		Temp.Origin = TOrigin.TransformVectorBy(Temp);
		return Temp;
	}

	// Convert this orthogonal coordinate system to a rotation.
	FRotator OrthoRotation() const;

	// Operators.
	FCoords& operator*=	(const FCoords   &TransformCoords);
	FCoords	 operator*	(const FCoords   &TransformCoords) const;
	FCoords& operator*=	(const FVector   &Point);
	FCoords  operator*	(const FVector   &Point) const;
	FCoords& operator*=	(const FRotator  &Rot);
	FCoords  operator*	(const FRotator  &Rot) const;
	FCoords& operator*=	(const FScale    &Scale);
	FCoords  operator*	(const FScale    &Scale) const;
	FCoords& operator/=	(const FVector   &Point);
	FCoords  operator/	(const FVector   &Point) const;
	FCoords& operator/=	(const FRotator  &Rot);
	FCoords  operator/	(const FRotator  &Rot) const;
	FCoords& operator/=	(const FScale    &Scale);
	FCoords  operator/	(const FScale    &Scale) const;

	inline FCoords TransposeAdjoint() const
	{
		FCoords C;
		C.XAxis.X = YAxis.Y * ZAxis.Z - YAxis.Z * ZAxis.Y;
		C.XAxis.Y = YAxis.Z * ZAxis.X - YAxis.X * ZAxis.Z;
		C.XAxis.Z = YAxis.X * ZAxis.Y - YAxis.Y * ZAxis.X;
		C.YAxis.X = ZAxis.Y * XAxis.Z - ZAxis.Z * XAxis.Y;
		C.YAxis.Y = ZAxis.Z * XAxis.X - ZAxis.X * XAxis.Z;
		C.YAxis.Z = ZAxis.X * XAxis.Y - ZAxis.Y * XAxis.X;
		C.ZAxis.X = XAxis.Y * YAxis.Z - XAxis.Z * YAxis.Y;
		C.ZAxis.Y = XAxis.Z * YAxis.X - XAxis.X * YAxis.Z;
		C.ZAxis.Z = XAxis.X * YAxis.Y - XAxis.Y * YAxis.X;
		return C;
	}
	inline FCoords TransposeAdjointMirror() const
	{
		FCoords C;
		C.XAxis.X = YAxis.Y * ZAxis.Z - YAxis.Z * ZAxis.Y;
		C.XAxis.Y = YAxis.Z * ZAxis.X - YAxis.X * ZAxis.Z;
		C.XAxis.Z = YAxis.X * ZAxis.Y - YAxis.Y * ZAxis.X;
		C.YAxis.X = ZAxis.Y * XAxis.Z - ZAxis.Z * XAxis.Y;
		C.YAxis.Y = ZAxis.Z * XAxis.X - ZAxis.X * XAxis.Z;
		C.YAxis.Z = ZAxis.X * XAxis.Y - ZAxis.Y * XAxis.X;
		C.ZAxis.X = XAxis.Y * YAxis.Z - XAxis.Z * YAxis.Y;
		C.ZAxis.Y = XAxis.Z * YAxis.X - XAxis.X * YAxis.Z;
		C.ZAxis.Z = XAxis.X * YAxis.Y - XAxis.Y * YAxis.X;
		if (Determinant() > 0.f)
			C.Mirror();
		return C;
	}
	inline void Mirror()
	{
		XAxis = -XAxis;
		YAxis = -YAxis;
		ZAxis = -ZAxis;
	}

	// Scale all axes without modifing the origin.
	inline void ScaleAxes(const FVector& Scalar)
	{
		XAxis *= Scalar;
		YAxis *= Scalar;
		ZAxis *= Scalar;
	}

	// Project 2D UV map to 3D coordinates system.
	static FCoords ProjectFrom2DCoords(const FVector& PointA, const FVector2D& UVA, const FVector& PointB, const FVector2D& UVB, const FVector& PointC, const FVector2D& UVC);

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FCoords& F )
	{
		return Ar << F.Origin << F.XAxis << F.YAxis << F.ZAxis;
	}

	// Returns a string description.
	FString String() const
	{
		guardSlow(FCoords::String);
		return FString::Printf(TEXT("(Origin=%s,XAxis=%s,YAxis=%s,ZAxis=%s)"), *Origin.String(), *XAxis.String(), *YAxis.String(), *ZAxis.String());
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	FCoords2D. Written by Marco - Mainly to be used by Texture modifiers.
-----------------------------------------------------------------------------*/

class FCoords2D
{
public:
	FVector2D Origin, XAxis, YAxis;

	FCoords2D()
	{}
	FCoords2D(const FVector2D& Org, const FVector2D& X, const FVector2D& Y)
		: Origin(Org), XAxis(X), YAxis(Y)
	{}
	FCoords2D(INT)
		: Origin(0.f, 0.f), XAxis(1.f, 0.f), YAxis(0.f, 1.f)
	{}

	inline void TransformPointUV(FLOAT& X, FLOAT& Y, FLOAT UMulti, FLOAT VMulti) const
	{
		X += (Origin.X * UMulti);
		Y += (Origin.Y * VMulti);
		FLOAT OrgX = X;
		X = (X * XAxis.X) + (Y * XAxis.Y);
		Y = (OrgX * YAxis.X) + (Y * YAxis.Y);
	}
	inline void TransformPoint(FLOAT& X, FLOAT& Y) const
	{
		X += Origin.X;
		Y += Origin.Y;
		FLOAT OrgX = X;
		X = (X * XAxis.X) + (Y * XAxis.Y);
		Y = (OrgX * YAxis.X) + (Y * YAxis.Y);
	}
	inline void TransformVector(FLOAT& X, FLOAT& Y) const
	{
		FLOAT OrgX = X;
		X = (X * XAxis.X) + (Y * XAxis.Y);
		Y = (OrgX * YAxis.X) + (Y * YAxis.Y);
	}

	inline void operator*=(const FCoords2D& TransformCoords)
	{
		TransformCoords.TransformPoint(Origin.X, Origin.Y);
		TransformCoords.TransformVector(XAxis.X, XAxis.Y);
		TransformCoords.TransformVector(YAxis.X, YAxis.Y);
	}
	inline void ApplyOffset(const FVector2D& V)
	{
		Origin += V;
	}
	inline void ApplyScale2D(const FVector2D& V)
	{
		Origin *= V;
		XAxis /= V.X;
		YAxis /= V.Y;
	}
	inline void ApplyScale(FLOAT V)
	{
		Origin *= V;
		FLOAT Neq = 1.f / V;
		XAxis *= Neq;
		YAxis *= Neq;
	}
	inline void ApplyRotation(INT Ang);
};

/*-----------------------------------------------------------------------------
	FSphere.
-----------------------------------------------------------------------------*/

class CORE_API FSphere : public FPlane
{
public:
	// Constructors.
	FSphere()
	{}
	FSphere( INT )
	:	FPlane(0,0,0,0)
	{}
	FSphere( FVector V, FLOAT W )
	:	FPlane( V, W )
	{}
	FSphere( const FVector* Pts, INT Count );
	friend FArchive& operator<<( FArchive& Ar, FSphere& S )
	{
		guardSlow(FSphere<<);
		if( Ar.Ver()<=61 )//oldver
			Ar << (FVector&)S;
		else
			Ar << (FPlane&)S;
		return Ar;
		unguardSlow
	}
	inline FSphere TransformSphere( const FCoords& C ) const
	{
		return FSphere(TransformPointBy(C),appSqrt(Max(C.XAxis|C.XAxis,Max(C.YAxis|C.YAxis,C.ZAxis|C.ZAxis))) * W);
	}
};

/*-----------------------------------------------------------------------------
	FScale.
-----------------------------------------------------------------------------*/

// An axis along which sheering is performed.
enum ESheerAxis
{
	SHEER_None = 0,
	SHEER_XY   = 1,
	SHEER_XZ   = 2,
	SHEER_YX   = 3,
	SHEER_YZ   = 4,
	SHEER_ZX   = 5,
	SHEER_ZY   = 6,
};

//
// Scaling and sheering info associated with a brush.  This is
// easily-manipulated information which is built into a transformation
// matrix later.
//
class CORE_API FScale
{
public:
	// Variables.
	FVector		Scale;
	FLOAT		SheerRate;
	BYTE		SheerAxis; // From ESheerAxis

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FScale &S )
	{
		return Ar << S.Scale << S.SheerRate << S.SheerAxis;
	}

	// Constructors.
	FScale() {}
	FScale( const FVector &InScale, FLOAT InSheerRate, ESheerAxis InSheerAxis )
	:	Scale(InScale), SheerRate(InSheerRate), SheerAxis(InSheerAxis) {}
	FScale(const FVector& InScale)
		: Scale(InScale), SheerRate(0.f), SheerAxis(SHEER_None) {}

	// Operators.
	UBOOL operator==( const FScale &S ) const
	{
		return Scale==S.Scale && SheerRate==S.SheerRate && SheerAxis==S.SheerAxis;
	}

	// Functions.
	FLOAT Orientation()
	{
		return Sgn(Scale.X * Scale.Y * Scale.Z);
	}
};
/*-----------------------------------------------------------------------------
	FModelCoords.
-----------------------------------------------------------------------------*/

//
// A model coordinate system, describing both the covariant and contravariant
// transformation matrices to transform points and normals by.
//
class CORE_API FModelCoords
{
public:
	// Variables.
	FCoords PointXform;		// Coordinates to transform points by  (covariant).
	FCoords VectorXform;	// Coordinates to transform normals by (contravariant).

	// Constructors.
	FModelCoords()
	{}
	FModelCoords( const FCoords& InCovariant, const FCoords& InContravariant )
	:	PointXform(InCovariant), VectorXform(InContravariant)
	{}

	// Functions.
	FModelCoords Inverse()
	{
		return FModelCoords( VectorXform.Transpose(), PointXform.Transpose() );
	}
};

/*-----------------------------------------------------------------------------
	FRotator.
-----------------------------------------------------------------------------*/

//
// Rotation.
//
class FRotator
{
public:
	// Variables.
	INT Pitch; // Looking up and down (0=Straight Ahead, +Up, -Down).
	INT Yaw;   // Rotating around (running in circles), 0=East, +North, -South.
	INT Roll;  // Rotation about axis of screen, 0=Straight, +Clockwise, -CCW.

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FRotator& R )
	{
		return Ar << R.Pitch << R.Yaw << R.Roll;
	}

	// Constructors.
	FRotator() {}
	FRotator( INT InPitch, INT InYaw, INT InRoll )
	:	Pitch(InPitch), Yaw(InYaw), Roll(InRoll) {}

	// Binary arithmetic operators.
	FRotator operator+( const FRotator &R ) const
	{
		return FRotator( Pitch+R.Pitch, Yaw+R.Yaw, Roll+R.Roll );
	}
	FRotator operator-( const FRotator &R ) const
	{
		return FRotator( Pitch-R.Pitch, Yaw-R.Yaw, Roll-R.Roll );
	}
	FRotator operator*( FLOAT Scale ) const
	{
		return FRotator( appRound(Pitch*Scale), appRound(Yaw*Scale), appRound(Roll*Scale) );
	}
	friend FRotator operator*( FLOAT Scale, const FRotator &R )
	{
		return FRotator( appRound(R.Pitch*Scale), appRound(R.Yaw*Scale), appRound(R.Roll*Scale) );
	}
	FRotator operator/(FLOAT Scale) const
	{
		if (Scale == 0.f)
			return FRotator(0, 0, 0);
		Scale = 1.f / Scale;
		return FRotator(appRound(Pitch * Scale), appRound(Yaw * Scale), appRound(Roll * Scale));
	}
	friend FRotator operator/(FLOAT Scale, const FRotator& R)
	{
		if (Scale == 0.f)
			return FRotator(0, 0, 0);
		Scale = 1.f / Scale;
		return FRotator(appRound(R.Pitch * Scale), appRound(R.Yaw * Scale), appRound(R.Roll * Scale));
	}
	FRotator operator*= (FLOAT Scale)
	{
		Pitch = appRound(Pitch*Scale); Yaw = appRound(Yaw*Scale); Roll = appRound(Roll*Scale);
		return *this;
	}
	FRotator operator/= (FLOAT Scale)
	{
		if (Scale == 0)
			Pitch = Yaw = Roll = 0;
		else
		{
			Scale = 1.f / Scale;
			Pitch = appRound(Pitch * Scale); Yaw = appRound(Yaw * Scale); Roll = appRound(Roll * Scale);
		}
		return *this;
	}
	// Binary comparison operators.
	UBOOL operator==( const FRotator &R ) const
	{
		return Pitch==R.Pitch && Yaw==R.Yaw && Roll==R.Roll;
	}
	UBOOL operator!=( const FRotator &V ) const
	{
		return Pitch!=V.Pitch || Yaw!=V.Yaw || Roll!=V.Roll;
	}
	// Assignment operators.
	FRotator operator+=( const FRotator &R )
	{
		Pitch += R.Pitch; Yaw += R.Yaw; Roll += R.Roll;
		return *this;
	}
	FRotator operator-=( const FRotator &R )
	{
		Pitch -= R.Pitch; Yaw -= R.Yaw; Roll -= R.Roll;
		return *this;
	}
	// Functions.
	UBOOL IsZero() const
	{
		return ((Pitch&65535)==0) && ((Yaw&65535)==0) && ((Roll&65535)==0);
	}
	FRotator Add( INT DeltaPitch, INT DeltaYaw, INT DeltaRoll )
	{
		Yaw   += DeltaYaw;
		Pitch += DeltaPitch;
		Roll  += DeltaRoll;
		return *this;
	}
	FRotator AddBounded( INT DeltaPitch, INT DeltaYaw, INT DeltaRoll )
	{
		Yaw  += DeltaYaw;
		Pitch = FAddAngleConfined(Pitch,DeltaPitch,192*0x100,64*0x100);
		Roll  = FAddAngleConfined(Roll, DeltaRoll, 192*0x100,64*0x100);
		return *this;
	}
	FRotator GridSnap( const FRotator &RotGrid )
	{
		return FRotator
		(
			appRound(FSnap(Pitch,RotGrid.Pitch)),
			appRound(FSnap(Yaw,  RotGrid.Yaw)),
			appRound(FSnap(Roll, RotGrid.Roll))
		);
	}
	inline FRotator ShortenRotation() const
	{
		return FRotator(ReduceAngleX(Pitch), ReduceAngleX(Yaw), ReduceAngleX(Roll));
	}
	FVector Vector() const;

	// Write compressed networked rotator
	inline void SerializeCompressed(FArchive& Ar)
	{
		BYTE BPitch = (Pitch >> 8), BYaw = (Yaw >> 8), BRoll = (Roll >> 8);

		BYTE B = (BPitch != 0);
		Ar.SerializeBits(&B, 1);
		if (B)
			Ar << BPitch;
		else BPitch = 0;

		B = (BYaw != 0);
		Ar.SerializeBits(&B, 1);
		if (B)
			Ar << BYaw;
		else BYaw = 0;

		B = (BRoll != 0);
		Ar.SerializeBits(&B, 1);
		if (B)
			Ar << BRoll;
		else BRoll = 0;

		if (Ar.IsLoading())
		{
			Pitch = BPitch << 8;
			Yaw = BYaw << 8;
			Roll = BRoll << 8;
		}
	}
};

inline FRotator GetDirectionalRot(const FRotator& R, const FPlane& HitNormal)
{
	FPlane X,Y,Z;
	X = R.Vector();
	X = X - (X.PlaneDot(HitNormal)) * HitNormal;
	if( X.Size()<0.001f ) // Surface directly coplanar to the rotation
		return (-HitNormal).Rotation();
	X.Normalize();
	Y = (X ^ HitNormal);
	Y.Normalize();
	Z = HitNormal;
	return (FCoords( FVector(0,0,0), -Z, -Y, X ).OrthoRotation());
}

/*-----------------------------------------------------------------------------
	Bounds.
-----------------------------------------------------------------------------*/

//
// A rectangular minimum bounding volume.
//
class CORE_API FBox
{
public:
	// Variables.
	FVector Min;
	FVector Max;
	BYTE IsValid;

	// Constructors.
	FBox() {}
	FBox(INT) : Min(0,0,0), Max(0,0,0), IsValid(0) {}
	FBox( const FVector& InMin, const FVector& InMax ) : Min(InMin), Max(InMax), IsValid(1) {}
	FBox( const FVector* Points, INT Count );

	// Accessors.
	FVector& GetExtrema( int i )
	{
		return (&Min)[i];
	}
	const FVector& GetExtrema( int i ) const
	{
		return (&Min)[i];
	}
	inline FVector GetExtent() const
	{
		return 0.5f * (Max - Min);
	}
	inline FVector GetCentroid() const
	{
		return Min + 0.5f * (Max - Min);
	}
	inline void GetExtentNCentroid(FVector& Center, FVector& Extent) const
	{
		Extent = GetExtent();
		Center = Min + Extent;
	}
	inline FSphere GetSphereBounds() const
	{
		FVector Cent, Ext;
		GetExtentNCentroid(Cent, Ext);
		return FSphere(Cent, Ext.Size());
	}

	// Functions.
	inline FBox& operator+=( const FVector &Other )
	{
		if( IsValid )
		{
			Min.X = ::Min( Min.X, Other.X );
			Min.Y = ::Min( Min.Y, Other.Y );
			Min.Z = ::Min( Min.Z, Other.Z );

			Max.X = ::Max( Max.X, Other.X );
			Max.Y = ::Max( Max.Y, Other.Y );
			Max.Z = ::Max( Max.Z, Other.Z );
		}
		else
		{
			Min = Max = Other;
			IsValid = 1;
		}
		return *this;
	}
	FBox operator+( const FVector& Other ) const
	{
		return FBox(*this) += Other;
	}
	inline FBox& operator+=( const FBox& Other )
	{
		if( IsValid && Other.IsValid )
		{
			Min.X = ::Min( Min.X, Other.Min.X );
			Min.Y = ::Min( Min.Y, Other.Min.Y );
			Min.Z = ::Min( Min.Z, Other.Min.Z );

			Max.X = ::Max( Max.X, Other.Max.X );
			Max.Y = ::Max( Max.Y, Other.Max.Y );
			Max.Z = ::Max( Max.Z, Other.Max.Z );
		}
		else *this = Other;
		return *this;
	}
	FBox operator+( const FBox& Other ) const
	{
		return FBox(*this) += Other;
	}
	FBox TransformBy( const FCoords& Coords ) const
	{
		FBox NewBox(0);
		for( int i=0; i<2; i++ )
			for( int j=0; j<2; j++ )
				for( int k=0; k<2; k++ )
					NewBox += FVector( GetExtrema(i).X, GetExtrema(j).Y, GetExtrema(k).Z ).TransformPointBy( Coords );
		return NewBox;
	}
	FBox ExpandBy( FLOAT W ) const
	{
		return FBox( Min - FVector(W,W,W), Max + FVector(W,W,W) );
	}

	inline void ExpandByBox(const FBox& Other)
	{
		Min.X = ::Min(Min.X, Other.Min.X);
		Min.Y = ::Min(Min.Y, Other.Min.Y);
		Min.Z = ::Min(Min.Z, Other.Min.Z);

		Max.X = ::Max(Max.X, Other.Max.X);
		Max.Y = ::Max(Max.Y, Other.Max.Y);
		Max.Z = ::Max(Max.Z, Other.Max.Z);
	}

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, FBox& Bound )
	{
		return Ar << Bound.Min << Bound.Max << Bound.IsValid;
	}
};
typedef FBox FBoundingBox;

//
// A combination of bounding box and sphere.
//
struct FBoundingVolume : public FBox
{
	FPlane Sphere;
};

// Marco: 2D version of BoundingBox

struct F2DBox
{
	FVector2D Min, Max;
	UBOOL IsValid;

	F2DBox()
	{}
	F2DBox(INT)
		: Min(0, 0), Max(0, 0), IsValid(FALSE)
	{}
	F2DBox(const FVector2D& Base)
		: Min(Base), Max(Base), IsValid(TRUE)
	{}
	F2DBox(const FVector2D& MinPt, const FVector2D& MaxPt)
		: Min(MinPt), Max(MaxPt), IsValid(TRUE)
	{}
	inline F2DBox& operator+=(const FVector2D& Other)
	{
		if (IsValid)
		{
			Min.X = ::Min(Min.X, Other.X);
			Min.Y = ::Min(Min.Y, Other.Y);

			Max.X = ::Max(Max.X, Other.X);
			Max.Y = ::Max(Max.Y, Other.Y);
		}
		else
		{
			Min = Max = Other;
			IsValid = TRUE;
		}
		return *this;
	}
	F2DBox operator+(const FVector2D& Other) const
	{
		return F2DBox(*this) += Other;
	}
	inline F2DBox& operator+=(const F2DBox& Other)
	{
		if (IsValid && Other.IsValid)
		{
			Min.X = ::Min(Min.X, Other.Min.X);
			Min.Y = ::Min(Min.Y, Other.Min.Y);

			Max.X = ::Max(Max.X, Other.Max.X);
			Max.Y = ::Max(Max.Y, Other.Max.Y);
		}
		else *this = Other;
		return *this;
	}
	F2DBox operator+(const F2DBox& Other) const
	{
		return F2DBox(*this) += Other;
	}
	inline void ExpandBy(FLOAT Value)
	{
		Min.X -= Value;
		Min.Y -= Value;
		Max.X += Value;
		Max.Y += Value;
	}
	inline UBOOL BoundsOverlap(const F2DBox& Other) const
	{
		return ((Min.X <= Other.Min.X && Max.X >= Other.Min.X) || (Other.Min.X <= Min.X && Other.Max.X >= Min.X)) &&
			((Min.Y <= Other.Min.Y && Max.Y >= Other.Min.Y) || (Other.Min.Y <= Min.Y && Other.Max.Y >= Min.Y));
	}
	inline UBOOL PointOverlaps(const FVector2D Point) const
	{
		return ((Min.X <= Point.X && Max.X >= Point.X) && (Min.Y <= Point.Y && Max.Y >= Point.Y));
	}
};

/*-----------------------------------------------------------------------------
	FGlobalMath.
-----------------------------------------------------------------------------*/

//
// Global mathematics info.
//
class CORE_API FGlobalMath
{
public:
	// Constants.
	enum {ANGLE_SHIFT 	= 2};		// Bits to right-shift to get lookup value.
	enum {ANGLE_BITS	= 14};		// Number of valid bits in angles.
	enum {NUM_ANGLES 	= 16384}; 	// Number of angles that are in lookup table.
	enum {NUM_SQRTS		= 16384};	// Number of square roots in lookup table.
	enum {ANGLE_MASK    =  (((1<<ANGLE_BITS)-1)<<(16-ANGLE_BITS))};

	// Class constants.
	const FVector  	WorldMin;
	const FVector  	WorldMax;
	const FCoords  	UnitCoords;
	const FScale   	UnitScale;
	const FCoords	ViewCoords;

	// Basic math functions.
	FLOAT Sqrt( int i )
	{
		return SqrtFLOAT[i];
	}
	inline FLOAT SinTab( int i )
	{
		return TrigFLOAT[((i>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	inline FLOAT CosTab( int i )
	{
		return TrigFLOAT[(((i+16384)>>ANGLE_SHIFT)&(NUM_ANGLES-1))];
	}
	FLOAT SinFloat( FLOAT F )
	{
		return SinTab((INT)((F*65536.f)/(2.f*PI)));
	}
	FLOAT CosFloat( FLOAT F )
	{
		return CosTab((INT)((F*65536.f)/(2.f*PI)));
	}

	// Constructor.
	FGlobalMath();

private:
	// Tables.
	FLOAT  TrigFLOAT		[NUM_ANGLES];
	FLOAT  SqrtFLOAT		[NUM_SQRTS];
};

inline INT ReduceAngle( INT Angle )
{
	return Angle & FGlobalMath::ANGLE_MASK;
};

/*-----------------------------------------------------------------------------
	Floating point constants.
-----------------------------------------------------------------------------*/

//
// Lengths of normalized vectors (These are half their maximum values
// to assure that dot products with normalized vectors don't overflow).
//
#define FLOAT_NORMAL_THRESH				(0.0001f)

//
// Magic numbers for numerical precision.
//
#define THRESH_POINT_ON_PLANE			(0.10f)		/* Thickness of plane for front/back/inside test */
#define THRESH_POINT_ON_SIDE			(0.20f)		/* Thickness of polygon side's side-plane for point-inside/outside/on side test */
#define THRESH_POINTS_ARE_SAME			(0.002f)	/* Two points are same if within this distance */
#define THRESH_POINTS_ARE_NEAR			(0.015f)	/* Two points are near if within this distance and can be combined if imprecise math is ok */
#define THRESH_NORMALS_ARE_SAME			(0.00002f)	/* Two normal points are same if within this distance */
													/* Making this too large results in incorrect CSG classification and disaster */
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)	/* Two vectors are near if within this distance and can be combined if imprecise math is ok */
													/* Making this too large results in lighting problems due to inaccurate texture coordinates */
#define THRESH_SPLIT_POLY_WITH_PLANE	(0.25f)		/* A plane splits a polygon in half */
#define THRESH_SPLIT_POLY_PRECISELY		(0.01f)		/* A plane exactly splits a polygon */
#define THRESH_ZERO_NORM_SQUARED		(0.0001f)	/* Size of a unit normal that is considered "zero", squared */
#define THRESH_VECTORS_ARE_PARALLEL		(0.02f)		/* Vectors are parallel if dot product varies less than this */



/*-----------------------------------------------------------------------------
	FVector transformation.
-----------------------------------------------------------------------------*/

//
// Transformations in optimized assembler format.
// An adaption of Michael Abrash' optimal transformation code.
//
#if ASM
inline void ASMTransformPoint(const FCoords &Coords, const FVector &InVector, FVector &OutVector)
{
	// FCoords is a structure of 4 vectors: Origin, X, Y, Z
	//				 	  x  y  z
	// FVector	Origin;   0  4  8
	// FVector	XAxis;   12 16 20
	// FVector  YAxis;   24 28 32
	// FVector  ZAxis;   36 40 44
	//
	//	task:	Temp = ( InVector - Coords.Origin );
	//			Outvector.X = (Temp | Coords.XAxis);
	//			Outvector.Y = (Temp | Coords.YAxis);
	//			Outvector.Z = (Temp | Coords.ZAxis);
	//
	// About 33 cycles on a Pentium.
	//
	__asm
	{
		mov     esi,[InVector]
		mov     edx,[Coords]
		mov     edi,[OutVector]

		// get source
		fld     dword ptr [esi+0]
		fld     dword ptr [esi+4]
		fld     dword ptr [esi+8] // z y x
		fxch    st(2)     // xyz

		// subtract origin
		fsub    dword ptr [edx + 0]  // xyz
		fxch    st(1)
		fsub	dword ptr [edx + 4]  // yxz
		fxch    st(2)
		fsub	dword ptr [edx + 8]  // zxy
		fxch    st(1)        // X Z Y

		// triplicate X for  transforming
		fld     st(0)	// X X   Z Y
        fmul    dword ptr [edx+12]     // Xx X Z Y
        fld     st(1)   // X Xx X  Z Y
        fmul    dword ptr [edx+24]   // Xy Xx X  Z Y
		fxch    st(2)
		fmul    dword ptr [edx+36]  // Xz Xx Xy  Z  Y
		fxch    st(4)     // Y  Xx Xy  Z  Xz

		fld     st(0)			// Y Y    Xx Xy Z Xz
		fmul    dword ptr [edx+16]
		fld     st(1) 			// Y Yx Y    Xx Xy Z Xz
        fmul    dword ptr [edx+28]
		fxch    st(2)			// Y  Yx Yy   Xx Xy Z Xz
		fmul    dword ptr [edx+40]	 // Yz Yx Yy   Xx Xy Z Xz
		fxch    st(1)			// Yx Yz Yy   Xx Xy Z Xz

        faddp   st(3),st(0)	  // Yz Yy  XxYx   Xy Z  Xz
        faddp   st(5),st(0)   // Yy  XxYx   Xy Z  XzYz
        faddp   st(2),st(0)   // XxYx  XyYy Z  XzYz
		fxch    st(2)         // Z     XyYy XxYx XzYz

		fld     st(0)         //  Z  Z     XyYy XxYx XzYz
		fmul    dword ptr [edx+20]
		fld     st(1)         //  Z  Zx Z  XyYy XxYx XzYz
        fmul    dword ptr [edx+32]
		fxch    st(2)         //  Z  Zx Zy
		fmul    dword ptr [edx+44]	  //  Zz Zx Zy XyYy XxYx XzYz
		fxch    st(1)         //  Zx Zz Zy XyYy XxYx XzYz

		faddp   st(4),st(0)   //  Zz Zy XyYy  XxYxZx  XzYz
		faddp   st(4),st(0)	  //  Zy XyYy     XxYxZx  XzYzZz
		faddp   st(1),st(0)   //  XyYyZy      XxYxZx  XzYzZz
		fxch    st(1)		  //  Xx+Xx+Zx   Xy+Yy+Zy  Xz+Yz+Zz

		fstp    dword ptr [edi+0]
        fstp    dword ptr [edi+4]
        fstp    dword ptr [edi+8]
	}
}
#elif ASMLINUX
inline void ASMTransformPoint(const FCoords &Coords, const FVector &InVector, FVector &OutVector)
{
	__asm__ __volatile__ (
		// Get source.
		"flds	0(%%esi)     \n\t"   //# x
		"flds	4(%%esi)     \n\t"   //# y x
		"flds	8(%%esi)     \n\t"   //# z y x
		"fxch	%%st(2)      \n\t"

		// Subtract origin.
		"fsubs	0(%1)        \n\t"
		"fxch	%%st(1)      \n\t"
		"fsubs	4(%1)        \n\t"
		"fxch	%%st(2)      \n\t"
		"fsubs	8(%1)        \n\t"
		"fxch	%%st(1)      \n\t"

		// Triplicate X for transforming.
		"fld		%%st(0)  \n\t"
		"fmuls	12(%1)       \n\t"
		"fld		%%st(1)  \n\t"
		"fmuls	24(%1)       \n\t"
		"fxch	%%st(2)      \n\t"
		"fmuls	36(%1)       \n\t"
		"fxch	%%st(4)      \n\t"

		"fld		%%st(0)  \n\t"
		"fmuls	16(%1)       \n\t"
		"fld		%%st(1)  \n\t"
		"fmuls	28(%1)       \n\t"
		"fxch	%%st(2)      \n\t"
		"fmuls	40(%1)       \n\t"
		"fxch	%%st(1)      \n\t"

		"faddp	%%st(0),%%st(3) \n\t"
		"faddp	%%st(0),%%st(5) \n\t"
		"faddp	%%st(0),%%st(2) \n\t"
		"fxch	%%st(2)         \n\t"

		"fld		%%st(0)  \n\t"
		"fmuls	20(%1)       \n\t"
		"fld		%%st(1)  \n\t"
		"fmuls	32(%1)       \n\t"
		"fxch	%%st(2)      \n\t"
		"fmuls	44(%1)       \n\t"
		"fxch	%%st(1)      \n\t"

		"faddp	%%st(0),%%st(4) \n\t"
		"faddp	%%st(0),%%st(4) \n\t"
		"faddp	%%st(0),%%st(1) \n\t"
		"fxch	%%st(1)         \n\t"

		"fstps	0(%%edi)     \n\t"
		"fstps	4(%%edi)     \n\t"
		"fstps	8(%%edi)     \n\t"
	:
	:	"S" (&InVector),
		"q" (&Coords),
		"D" (&OutVector)
	: "memory"
	);
}
#endif

#if ASM
inline void ASMTransformVector(const FCoords &Coords, const FVector &InVector, FVector &OutVector)
{
	__asm
	{
		mov     esi,[InVector]
		mov     edx,[Coords]
		mov     edi,[OutVector]

		// get source
		fld     dword ptr [esi+0]
		fld     dword ptr [esi+4]
		fxch    st(1)
		fld     dword ptr [esi+8] // z x y
		fxch    st(1)             // x z y

		// triplicate X for  transforming
		fld     st(0)	// X X   Z Y
        fmul    dword ptr [edx+12]     // Xx X Z Y
        fld     st(1)   // X Xx X  Z Y
        fmul    dword ptr [edx+24]   // Xy Xx X  Z Y
		fxch    st(2)
		fmul    dword ptr [edx+36]  // Xz Xx Xy  Z  Y
		fxch    st(4)     // Y  Xx Xy  Z  Xz

		fld     st(0)			// Y Y    Xx Xy Z Xz
		fmul    dword ptr [edx+16]
		fld     st(1) 			// Y Yx Y    Xx Xy Z Xz
        fmul    dword ptr [edx+28]
		fxch    st(2)			// Y  Yx Yy   Xx Xy Z Xz
		fmul    dword ptr [edx+40]	 // Yz Yx Yy   Xx Xy Z Xz
		fxch    st(1)			// Yx Yz Yy   Xx Xy Z Xz

        faddp   st(3),st(0)	  // Yz Yy  XxYx   Xy Z  Xz
        faddp   st(5),st(0)   // Yy  XxYx   Xy Z  XzYz
        faddp   st(2),st(0)   // XxYx  XyYy Z  XzYz
		fxch    st(2)         // Z     XyYy XxYx XzYz

		fld     st(0)         //  Z  Z     XyYy XxYx XzYz
		fmul    dword ptr [edx+20]
		fld     st(1)         //  Z  Zx Z  XyYy XxYx XzYz
        fmul    dword ptr [edx+32]
		fxch    st(2)         //  Z  Zx Zy
		fmul    dword ptr [edx+44]	  //  Zz Zx Zy XyYy XxYx XzYz
		fxch    st(1)         //  Zx Zz Zy XyYy XxYx XzYz

		faddp   st(4),st(0)   //  Zz Zy XyYy  XxYxZx  XzYz
		faddp   st(4),st(0)	  //  Zy XyYy     XxYxZx  XzYzZz
		faddp   st(1),st(0)   //  XyYyZy      XxYxZx  XzYzZz
		fxch    st(1)		  //  Xx+Xx+Zx   Xy+Yy+Zy  Xz+Yz+Zz

		fstp    dword ptr [edi+0]
        fstp    dword ptr [edi+4]
        fstp    dword ptr [edi+8]
	}
}
#endif

#if ASMLINUX
inline void ASMTransformVector(const FCoords &Coords, const FVector &InVector, FVector &OutVector)
{
	asm volatile(
		// Get source.
		"flds	0(%%esi)    \n\t"
		"flds	4(%%esi)    \n\t"
		"fxch	%%st(1)     \n\t"
		"flds	8(%%esi)    \n\t"
		"fxch	%%st(1)     \n\t"

		// Triplicate X for transforming.
		"fld		%%st(0)      \n\t"
		"fmuls	12(%1)           \n\t"
		"fld		%%st(1)      \n\t"
		"fmuls	24(%1)           \n\t"
		"fxch	%%st(2)          \n\t"
		"fmuls	36(%1)           \n\t"
		"fxch	%%st(4)          \n\t"

		"fld		%%st(0)      \n\t"
		"fmuls	16(%1)           \n\t"
		"fld		%%st(1)      \n\t"
		"fmuls	28(%1)           \n\t"
		"fxch	%%st(2)          \n\t"
		"fmuls	40(%1)           \n\t"
		"fxch	%%st(1)          \n\t"

		"faddp	%%st(0),%%st(3)  \n\t"
		"faddp	%%st(0),%%st(5)  \n\t"
		"faddp	%%st(0),%%st(2)  \n\t"
		"fxch	%%st(2)          \n\t"

		"fld		%%st(0)      \n\t"
		"fmuls	20(%1)           \n\t"
		"fld		%%st(1)      \n\t"
		"fmuls	32(%1)           \n\t"
		"fxch	%%st(2)          \n\t"
		"fmuls	44(%1)           \n\t"
		"fxch	%%st(1)          \n\t"

		"faddp	%%st(0),%%st(4)  \n\t"
		"faddp	%%st(0),%%st(4)  \n\t"
		"faddp	%%st(0),%%st(1)  \n\t"
		"fxch	%%st(1)          \n\t"

		"fstps	0(%%edi)         \n\t"
		"fstps	4(%%edi)         \n\t"
		"fstps	8(%%edi)         \n\t"
	:
	: "S" (&InVector),
	  "q" (&Coords),
	  "D" (&OutVector)
	: "memory"
	);
}
#endif

//
// Transform a point by a coordinate system, moving
// it by the coordinate system's origin if nonzero.
//
inline FVector FVector::TransformPointBy( const FCoords &Coords ) const
{
#if ASM
	FVector Temp;
	ASMTransformPoint( Coords, *this, Temp);
	return Temp;
#elif 0 //ASMLINUX
	static FVector Temp;
	ASMTransformPoint( Coords, *this, Temp);
	return Temp;
#elif __PSX2_EE__
	FVector Result;
	FVector UnitVector = FVector(1.f, 1.f, 1.f);
	asm volatile (
		"lqc2		vf1, 0x00(%0)  \n\t"
		"lqc2		vf3, 0x00(%1)  \n\t"
		"lqc2		vf4, 0x00(%2)  \n\t"
		"lqc2		vf5, 0x00(%3)  \n\t"
		"lqc2		vf6, 0x00(%4)  \n\t"
	:
	: "r" (&UnitVector),
	  "r" (&Coords.Origin), "r" (&Coords.XAxis),
	  "r" (&Coords.YAxis), "r" (&Coords.ZAxis)
	);
	asm volatile (
		/* Register allocation:
		 * vf0: 0, 0, 0, 1 constant vector
		 * vf1: 1, 1, 1, 1 unit vector
		 * vf3: Frame origin
		 * vf4: XAxis
		 * vf5: YAxis
		 * vf6: ZAxis
		 */

		/* Load 128bit values. */
		"lqc2		vf2,  0x00(%1)          \n\t"

		/* Get subtracted vector */
		"vsub.xyz	vf7xyz, vf2xyz, vf3xyz	\n\t"	/* vf6 <- Temp */

		/* Perform dot products */
		"vmul.xyz	vf8xyz, vf7xyz, vf4xyz	\n\t"	/* vf8 <- Temp | XAxis */
		"vaddax.x	ACCx, vf0x,	vf8x		\n\t"	/* ACCx = vf8.x */
		"vmadday.x	ACCx, vf1x, vf8y		\n\t"	/* ACCx = vf8.x + vf8.y */
		"vmaddaz.x	ACCx, vf1x, vf8z		\n\t"	/* ACCx = vf8.x + vf8.y + vf8.z */

		"vmul.xyz	vf8xyz, vf7xyz, vf5xyz	\n\t"	/* vf8 <- Temp | YAxis */
		"vaddax.y	ACCy, vf0y,	vf8x		\n\t"	/* ACCy = vf8.x */
		"vmadday.y	ACCy, vf1y, vf8y		\n\t"	/* ACCy = vf8.x + vf8.y */
		"vmaddaz.y	ACCy, vf1y, vf8z		\n\t"	/* ACCy = vf8.x + vf8.y + vf8.z */

		"vmul.xyz	vf8xyz, vf7xyz, vf6xyz	\n\t"	/* vf8 <- Temp | ZAxis */
		"vaddax.z	ACCz, vf0z,	vf8x		\n\t"	/* ACCy = vf8.x */
		"vmadday.z	ACCz, vf1z, vf8y		\n\t"	/* ACCy = vf8.x + vf8.y */
		"vmaddaz.z	ACCz, vf1z, vf8z		\n\t"	/* ACCy = vf8.x + vf8.y + vf8.z */

		"vmadd		vf9, vf0, vf0			\n\t"	/* vf9 = ACC + 0 * 0 */
		"sqc2		vf9, 0x00(%0)			\n\t"	/* Result.Point <- ACC */
	:
	: "r" (&Result), "r" (this)
	: "memory"
	);
	return Result;
#elif MACOSX
	// (This was Westlake's code, guess they'd know better... --ryan.)
	FLOAT TempX = X - Coords.Origin.X;
	FLOAT TempY = Y - Coords.Origin.Y;
	FLOAT TempZ = Z - Coords.Origin.Z;

	const FVector *Axis;
	Axis = &Coords.XAxis;
	FLOAT PointX = TempX*Axis->X + TempY*Axis->Y + TempZ*Axis->Z;
	Axis = &Coords.YAxis;
	FLOAT PointY = TempX*Axis->X + TempY*Axis->Y + TempZ*Axis->Z;
	Axis = &Coords.ZAxis;
	FLOAT PointZ = TempX*Axis->X + TempY*Axis->Y + TempZ*Axis->Z;
	return( FVector(PointX,PointY,PointZ) );
#else
#if VECTOR_SSE_MATHS
	FVector Temp = *this - Coords.Origin;
	return Temp.TransformVectorBy(Coords);
#else
	FVector Temp = *this - Coords.Origin;
	return FVector(	Temp | Coords.XAxis, Temp | Coords.YAxis, Temp | Coords.ZAxis );
#endif
#endif
}

//
// Transform a directional vector by a coordinate system.
// Ignore's the coordinate system's origin.
//
inline FVector FVector::TransformVectorBy( const FCoords &Coords ) const
{
#if ASM
	FVector Temp;
	ASMTransformVector( Coords, *this, Temp);
	return Temp;
#elif 0 //ASMLINUX
	FVector Temp;
	ASMTransformVector( Coords, *this, Temp);
	return Temp;
#elif __PSX2_EE__
	FVector Result;
	FVector UnitVector = FVector(1.f, 1.f, 1.f);
	asm volatile (
		"lqc2		vf1, 0x00(%0)  \n\t"
		"lqc2		vf4, 0x00(%1)  \n\t"
		"lqc2		vf5, 0x00(%2)  \n\t"
		"lqc2		vf6, 0x00(%3)  \n\t"
	:
	: "r" (&UnitVector), "r" (&Coords.XAxis),
	  "r" (&Coords.YAxis), "r" (&Coords.ZAxis)
	);
	asm volatile (
		/* Register allocation:
		 * vf0: 0, 0, 0, 1 constant vector
		 * vf1: 1, 1, 1, 1 unit vector
		 * vf4: XAxis
		 * vf5: YAxis
		 * vf6: ZAxis
		 */

		/* Load 128bit values. */
		"lqc2		vf7,  0x00(%1)          \n\t"

		/* Perform dot products */
		"vmul.xyz	vf8xyz, vf7xyz, vf4xyz	\n\t"	/* vf8 <- Temp | XAxis */
		"vaddax.x	ACCx, vf0x,	vf8x		\n\t"	/* ACCx = vf8.x */
		"vmadday.x	ACCx, vf1x, vf8y		\n\t"	/* ACCx = vf8.x + vf8.y */
		"vmaddaz.x	ACCx, vf1x, vf8z		\n\t"	/* ACCx = vf8.x + vf8.y + vf8.z */

		"vmul.xyz	vf8xyz, vf7xyz, vf5xyz	\n\t"	/* vf8 <- Temp | YAxis */
		"vaddax.y	ACCy, vf0y,	vf8x		\n\t"	/* ACCy = vf8.x */
		"vmadday.y	ACCy, vf1y, vf8y		\n\t"	/* ACCy = vf8.x + vf8.y */
		"vmaddaz.y	ACCy, vf1y, vf8z		\n\t"	/* ACCy = vf8.x + vf8.y + vf8.z */

		"vmul.xyz	vf8xyz, vf7xyz, vf6xyz	\n\t"	/* vf8 <- Temp | ZAxis */
		"vaddax.z	ACCz, vf0z,	vf8x		\n\t"	/* ACCy = vf8.x */
		"vmadday.z	ACCz, vf1z, vf8y		\n\t"	/* ACCy = vf8.x + vf8.y */
		"vmaddaz.z	ACCz, vf1z, vf8z		\n\t"	/* ACCy = vf8.x + vf8.y + vf8.z */

		"vmadd		vf9, vf0, vf0			\n\t"	/* vf9 = ACC + 0 * 0 */
		"sqc2		vf9, 0x00(%0)			\n\t"	/* Result.Point <- ACC */
	:
	: "r" (&Result), "r" (this)
	: "memory"
	);
	return Result;
#elif MACOSX || MACOSXPPC
	// (This was Westlake's code, guess they'd know better... --ryan.)
	const FVector *Axis;
	Axis = &Coords.XAxis;
	FLOAT PointX = X*Axis->X + Y*Axis->Y + Z*Axis->Z;
	Axis = &Coords.YAxis;
	FLOAT PointY = X*Axis->X + Y*Axis->Y + Z*Axis->Z;
	Axis = &Coords.ZAxis;
	FLOAT PointZ = X*Axis->X + Y*Axis->Y + Z*Axis->Z;
	return( FVector(PointX,PointY,PointZ) );
#else
#if VECTOR_SSE_MATHS
	FVector Result;
	// Instead of calculating dot products individually, we'll do it in a single
	// operation by transposing the results and then adding all 3 vectors
	__m128 m_this = _mm<false>(*this);
	__m128 m_Xl = _mm_mul_ps(m_this, _mm<false>(Coords.XAxis)); //Xx,Xy,Xz
	__m128 m_Yl = _mm_mul_ps(m_this, _mm<false>(Coords.YAxis)); //Yx,Yy,Yz
	__m128 m_Zl = _mm_mul_ps(m_this, _mm<false>(Coords.ZAxis)); //Zx,Zy,Zz
	//Target:
	// Xx,Yx,Zx
	// Xy,Yy,Zy
	// Xz,Yz,Zz
	__m128 m_tmp1 = _mm_unpacklo_ps(m_Xl, m_Yl); //Xx,Yx,Xy,Yy
	__m128 m_tmp2 = _mm_unpackhi_ps(m_Xl, m_Yl); //Xz,Yz
	__m128 m_DOT = _mm_shuffle_ps(m_tmp2, m_Zl, 0b11100100); //Xz,Yz,Zz
	m_tmp2 = _mm_movehl_ps(m_tmp2, m_tmp1); //Xy,Yy (when first parameter matches result, only one instruction is generated)
	m_tmp1 = _mm_movelh_ps(m_tmp1, m_Zl); //Xx,Yx,Zx,Zy (same as above)
	m_DOT = _mm_add_ps(m_DOT, m_tmp1); // Xx+Xz, Yx+Yz, Zx+Zz
	m_tmp2 = _mm_shuffle_ps(m_tmp2, m_tmp1, 0b10110100); // Xy,Yy,Zy,Zx
	m_DOT = _mm_add_ps(m_DOT, m_tmp2); // Xx+Xy+Xz, Yx+Yy+Yz, Zx+Zy+Zz

	_mm_storeu_ps(&Result.X, m_DOT);
	return Result;
#else
	return FVector(	*this | Coords.XAxis, *this | Coords.YAxis, *this | Coords.ZAxis );
#endif
#endif
}


// Apply 'pivot' transform: First rotate, then add the translation.
// TODO: convert to assembly !
inline FVector FVector::PivotTransform(const FCoords& Coords) const
{
	return Coords.Origin + TransformVectorBy(Coords);
}

//
// Mirror a vector about a normal vector.
//
inline FVector FVector::MirrorByVector( const FVector& MirrorNormal ) const
{
	return *this - MirrorNormal * (2.f * (*this | MirrorNormal));
}

//
// Mirror a vector about a plane.
//
inline FVector FVector::MirrorByPlane( const FPlane& Plane ) const
{
	return *this - static_cast<FVector>(Plane) * (2.f * Plane.PlaneDot(*this) );
}

/*-----------------------------------------------------------------------------
	FVector friends.
-----------------------------------------------------------------------------*/

//
// Compare two points and see if they're the same, using a threshold.
// Returns 1=yes, 0=no.  Uses fast distance approximation.
//
inline UBOOL FPointsAreSame( const FVector &P, const FVector &Q )
{
	return P.approximatelyEqual(Q, THRESH_POINTS_ARE_SAME);
}

//
// Compare two points and see if they're the same, using a threshold.
// Returns 1=yes, 0=no.  Uses fast distance approximation.
//
inline UBOOL FPointsAreNear( const FVector &Point1, const FVector &Point2, FLOAT Dist )
{
	return Point1.approximatelyEqual(Point2, Dist);
}

//
// Calculate the signed distance (in the direction of the normal) between
// a point and a plane.
//
inline FLOAT FPointPlaneDist
(
	const FVector &Point,
	const FVector &PlaneBase,
	const FVector &PlaneNormal
)
{
	return (Point - PlaneBase) | PlaneNormal;
}

//
// See if two normal vectors (or plane normals) are nearly parallel.
//
inline UBOOL FParallel( const FVector &Normal1, const FVector &Normal2 )
{
	FLOAT NormalDot = Normal1 | Normal2;
	return (Abs(NormalDot - 1.f) <= THRESH_VECTORS_ARE_PARALLEL);
}

//
// See if two planes are coplanar.
//
inline UBOOL FCoplanar( const FVector &Base1, const FVector &Normal1, const FVector &Base2, const FVector &Normal2 )
{
	if      (!FParallel(Normal1,Normal2)) return FALSE;
	else if (FPointPlaneDist (Base2,Base1,Normal1) > THRESH_POINT_ON_PLANE) return FALSE;
	else    return TRUE;
}

/*-----------------------------------------------------------------------------
	FPlane implementation.
-----------------------------------------------------------------------------*/

//
// Transform a point by a coordinate system, moving
// it by the coordinate system's origin if nonzero.
//
inline FPlane FPlane::TransformPlaneByOrtho( const FCoords &Coords ) const
{
	FVector Normal( *this | Coords.XAxis, *this | Coords.YAxis, *this | Coords.ZAxis );
	return FPlane( Normal, W - (Coords.Origin.TransformVectorBy(Coords) | Normal) );
}
//
// Same as above but in addition normalize the plane.
//
inline FPlane FPlane::TransformPlaneByOrthoAdj( const FCoords &Coords, const FCoords &CoordsAdj, bool bFlip ) const
{
	FVector Normal(TransformVectorBy(CoordsAdj).SafeNormalSlow());
	if( bFlip )
		Normal = -Normal;
	return FPlane((*this * W).TransformPointBy(Coords),Normal);
}

/*-----------------------------------------------------------------------------
	FCoords operators.
-----------------------------------------------------------------------------*/

//
// Transform this coordinate system by another coordinate system.
//
inline FCoords& FCoords::operator*=( const FCoords& TransformCoords )
{
	//!! Proper solution:
	//Origin = Origin.TransformPointBy( TransformCoords.Inverse().Transpose() );
	// Fast solution assuming orthogonal coordinate system:
	Origin = Origin.TransformPointBy ( TransformCoords );
	XAxis  = XAxis .TransformVectorBy( TransformCoords );
	YAxis  = YAxis .TransformVectorBy( TransformCoords );
	ZAxis  = ZAxis .TransformVectorBy( TransformCoords );
	return *this;
}
inline FCoords FCoords::operator*( const FCoords &TransformCoords ) const
{
	return FCoords(*this) *= TransformCoords;
}

//
// Transform this coordinate system by a pitch-yaw-roll rotation.
//
inline FCoords& FCoords::operator*=( const FRotator &Rot )
{
	FLOAT	SR	= GMath.SinTab(Rot.Roll),
			SP	= GMath.SinTab(Rot.Pitch),
			SY	= GMath.SinTab(Rot.Yaw),
			CR	= GMath.CosTab(Rot.Roll),
			CP	= GMath.CosTab(Rot.Pitch),
			CY	= GMath.CosTab(Rot.Yaw);

	*this *= FCoords(	FVector(0,0,0),
						FVector(CP * CY,					CP * SY,				SP),
						FVector(SR * SP * CY - CR * SY,		SR * SP * SY + CR * CY,	-SR * CP),
						FVector(-(CR * SP * CY + SR * SY),	CY * SR - CR * SP * SY,	CR * CP));
	return *this;
}
inline FCoords FCoords::operator*( const FRotator &Rot ) const
{
	return FCoords(*this) *= Rot;
}

inline FCoords& FCoords::operator*=( const FVector &Point )
{
	Origin -= Point;
	return *this;
}
inline FCoords FCoords::operator*( const FVector &Point ) const
{
	return FCoords(*this) *= Point;
}

//
// Detransform this coordinate system by a pitch-yaw-roll rotation.
//
inline FCoords& FCoords::operator/=( const FRotator &Rot )
{
	FLOAT	SR	= GMath.SinTab(Rot.Roll),
			SP	= GMath.SinTab(Rot.Pitch),
			SY	= GMath.SinTab(Rot.Yaw),
			CR	= GMath.CosTab(Rot.Roll),
			CP	= GMath.CosTab(Rot.Pitch),
			CY	= GMath.CosTab(Rot.Yaw);

	// Transpose rotation angle.
	*this *= FCoords(	FVector(0,0,0),
						FVector(CP * CY,	SR * SP * CY - CR * SY,	-(CR * SP * CY + SR * SY)),
						FVector(CP * SY,	SR * SP * SY + CR * CY,	CY * SR - CR * SP * SY),
						FVector(SP,			-SR * CP,				CR * CP));
	return *this;
}
inline FCoords FCoords::operator/( const FRotator &Rot ) const
{
	return FCoords(*this) /= Rot;
}

inline FCoords& FCoords::operator/=( const FVector &Point )
{
	Origin += Point;
	return *this;
}
inline FCoords FCoords::operator/( const FVector &Point ) const
{
	return FCoords(*this) /= Point;
}

//
// Transform this coordinate system by a scale.
// Note: Will return coordinate system of opposite handedness if
// Scale.X*Scale.Y*Scale.Z is negative.
//
inline FCoords& FCoords::operator*=( const FScale &Scale )
{
	// Apply sheering.
	if (Scale.SheerAxis != SHEER_None)
	{
		FLOAT   Sheer = FSheerSnap(Scale.SheerRate);
		FCoords TempCoords = GMath.UnitCoords;
		switch (Scale.SheerAxis)
		{
		case SHEER_XY:
			TempCoords.XAxis.Y = Sheer;
			break;
		case SHEER_XZ:
			TempCoords.XAxis.Z = Sheer;
			break;
		case SHEER_YX:
			TempCoords.YAxis.X = Sheer;
			break;
		case SHEER_YZ:
			TempCoords.YAxis.Z = Sheer;
			break;
		case SHEER_ZX:
			TempCoords.ZAxis.X = Sheer;
			break;
		case SHEER_ZY:
			TempCoords.ZAxis.Y = Sheer;
			break;
		default:
			break;
		}
		*this *= TempCoords;
	}

	// Apply scaling.
	XAxis    *= Scale.Scale;
	YAxis    *= Scale.Scale;
	ZAxis    *= Scale.Scale;
	Origin.X /= Scale.Scale.X;
	Origin.Y /= Scale.Scale.Y;
	Origin.Z /= Scale.Scale.Z;

	return *this;
}
inline FCoords FCoords::operator*( const FScale &Scale ) const
{
	return FCoords(*this) *= Scale;
}

//
// Detransform a coordinate system by a scale.
//
inline FCoords& FCoords::operator/=( const FScale &Scale )
{
	// Deapply scaling.
	XAxis    /= Scale.Scale;
	YAxis    /= Scale.Scale;
	ZAxis    /= Scale.Scale;
	Origin.X *= Scale.Scale.X;
	Origin.Y *= Scale.Scale.Y;
	Origin.Z *= Scale.Scale.Z;

	// Deapply sheering.
	if (Scale.SheerAxis != SHEER_None)
	{
		FCoords TempCoords(GMath.UnitCoords);
		FLOAT Sheer = FSheerSnap(Scale.SheerRate);
		switch (Scale.SheerAxis)
		{
		case SHEER_XY:
			TempCoords.XAxis.Y = -Sheer;
			break;
		case SHEER_XZ:
			TempCoords.XAxis.Z = -Sheer;
			break;
		case SHEER_YX:
			TempCoords.YAxis.X = -Sheer;
			break;
		case SHEER_YZ:
			TempCoords.YAxis.Z = -Sheer;
			break;
		case SHEER_ZX:
			TempCoords.ZAxis.X = -Sheer;
			break;
		case SHEER_ZY:
			TempCoords.ZAxis.Y = -Sheer;
			break;
		default: // SHEER_NONE
			break;
		}
		*this *= TempCoords;
	}

	return *this;
}
inline FCoords FCoords::operator/( const FScale &Scale ) const
{
	return FCoords(*this) /= Scale;
}

inline void FCoords2D::ApplyRotation(INT Ang)
{
	FLOAT SY = GMath.SinTab(Ang), CY = GMath.CosTab(Ang);

	// https://en.wikipedia.org/wiki/Rotation_matrix
	*this *= FCoords2D(FVector2D(0.f, 0.f),
		FVector2D(CY, SY),
		FVector2D(-SY, CY));
}

/*-----------------------------------------------------------------------------
	Random numbers.
-----------------------------------------------------------------------------*/

//
// Compute pushout of a box from a plane.
//
inline FLOAT FBoxPushOut( FVector Normal, FVector Size )
{
	return Abs(Normal.X*Size.X) + Abs(Normal.Y*Size.Y) + Abs(Normal.Z*Size.Z);
}

//
// Return a uniformly distributed random unit vector.
//
inline FVector VRand()
{
	FVector Result(0.f,0.f,0.f);
	do
	{
		// Check random vectors in the unit sphere so result is statistically uniform.
		Result.X = appFrand()*2 - 1;
		Result.Y = appFrand()*2 - 1;
		Result.Z = appFrand()*2 - 1;
	} while( Result.SizeSquared() > 1.f );
	return Result.UnsafeNormalSlow();
}

#if 1 //Math added by Legend on 4/12/2000
//
// RandomSpreadVector
// By Paul Du Bois, Infinite Machine
//
// Return a random unit vector within a cone of spread_degrees.
// (Distribution is such that there is no bunching near the axis.)
//
// - Create an FRotator with pitch < spread_degrees/2, yaw = 0, roll = random.
//   The tricky bit is getting the probability distribution of pitch
//   correct.  If it's flat, particles will tend to cluster around the pole.
//
// - For a given pitch phi, the probability p(phi) should be proportional to
//   the surface area of the slice of sphere defined by that pitch and a
//   random roll.  This is 2 pi (r sin phi) (r d phi) =~ sin phi
//
// - To map a flat distribution (FRand) to our new p(phi), we first find the
//   CDF (cumulative distribution fn) which is basically the integral of
//   p(phi) over its domain, normalized to have a total area of 1.  Smaller
//   spreads are modeled by limiting the domain of p(phi) and normalizing
//   the CDF for the smaller domain.  The CDF is always monotonically
//   non-decreasing and has range [0,1].
//
// - The insight is that the inverse of the CDF is exactly what we need to
//   convert a flat distribution to our p(phi).  aCDF: [0,1] -> angle.
//   (insert handwaving argument)
//
// - CDF(x) = integral(0->x) sin phi d phi
//          = -cos(x) - -cos(0)
//          = K * (1 - cos(x)) (K is normalizing factor to make CDF(domain_max)=1)
//        K = 1/max_cos_val   max_cos_val = (1 - cos(max pitch))
//
// - aCDF(y) = acos(1-y/K) = acos(1-y*max_cos_val)
//
inline FVector RandomSpreadVector(FLOAT spread_degrees)
{
    FLOAT max_pitch = Clamp(spread_degrees * (PI / 180.0f / 2.0f),0.0f,180.0f);
    FLOAT K = 1.0f - appCos(max_pitch);
    FLOAT pitch = appAcos(1.0f - appFrand()*K);  // this is the aCDF
    FLOAT rand_roll = appFrand() * (2.0f * PI);
    FLOAT radius = appSin(pitch);
    return FVector(appCos(pitch),radius*appSin(rand_roll),radius*appCos(rand_roll));
}
#endif

/*-----------------------------------------------------------------------------
	Advanced geometry.
-----------------------------------------------------------------------------*/

//
// Find the intersection of an infinite line (defined by two points) and
// a plane.  Assumes that the line and plane do indeed intersect; you must
// make sure they're not parallel before calling.
//
inline FVector FLinePlaneIntersection
(
	const FVector &Point1,
	const FVector &Point2,
	const FVector &PlaneOrigin,
	const FVector &PlaneNormal
)
{
	return
		Point1
	+	(Point2-Point1)
	*	(((PlaneOrigin - Point1)|PlaneNormal) / ((Point2 - Point1)|PlaneNormal));
}
inline FVector FLinePlaneIntersection
(
	const FVector &Point1,
	const FVector &Point2,
	const FPlane  &Plane
)
{
	return
		Point1
	+	(Point2-Point1)
	*	((Plane.W - (Point1|Plane))/((Point2 - Point1)|Plane));
}

/*-----------------------------------------------------------------------------
	FPlane functions.
-----------------------------------------------------------------------------*/

//
// Compute intersection point of three planes.
// Return 1 if valid, 0 if infinite.
//
inline UBOOL FIntersectPlanes3( FVector& I, const FPlane& P1, const FPlane& P2, const FPlane& P3 )
{
	guard(FIntersectPlanes3);

	// Compute determinant, the triple product P1|(P2^P3)==(P1^P2)|P3.
	FLOAT Det = (P1 ^ P2) | P3;
	if( Square(Det) < Square(0.001f) )
	{
		// Degenerate.
		I = FVector(0,0,0);
		return 0;
	}
	else
	{
		// Compute the intersection point, guaranteed valid if determinant is nonzero.
		I = (P1.W*(P2^P3) + P2.W*(P3^P1) + P3.W*(P1^P2)) / Det;
	}
	return 1;
	unguard;
}

//
// Compute intersection point and direction of line joining two planes.
// Return 1 if valid, 0 if infinite.
//
inline UBOOL FIntersectPlanes2( FVector& I, FVector& D, const FPlane& P1, const FPlane& P2 )
{
	guardSlow(FIntersectPlanes2);

	// Compute line direction, perpendicular to both plane normals.
	D = P1 ^ P2;
	DOUBLE DD = D.SizeSquaredD();
	if (DD < SMALL_NUMBER)
	{
		// Parallel or nearly parallel planes.
		//D = I = FVector(0,0,0);
		return FALSE;
	}
	else
	{
		// Compute intersection.
		I = (P1.W*(P2^D) + P2.W*(D^P1));
		I.DivMultiDbl(DD);
		D.DivMultiDbl(appSqrt(DD));
		return TRUE;
	}
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FRotator functions.
-----------------------------------------------------------------------------*/

constexpr DOUBLE vToUERadians = (DOUBLE)MAXWORD / (2.0 * PI);
//
// Convert a rotation into a vector facing in its direction.
//
inline FVector FRotator::Vector() const
{
	guardSlow(FRotator::Vector);
	FLOAT CP	= GMath.CosTab(Pitch);
	return FVector(CP*GMath.CosTab(Yaw),CP*GMath.SinTab(Yaw),GMath.SinTab(Pitch));
	unguardSlow;
}

inline void FVector::SerializeCompressedNormal(FArchive& Ar)
{
	DWORD Yaw, Pitch;
	if (Ar.IsLoading())
	{
		Ar.SerializeInt(Yaw, 2);
		if (Yaw == 0)
		{
			X = 0.f;
			Y = 0.f;
			Z = 0.f;
		}
		else
		{
			Ar.SerializeInt(Yaw, 256);
			Ar.SerializeInt(Pitch, 128);
			Yaw = Yaw << 8;
			Pitch = Pitch << 9;
			FLOAT CP = GMath.CosTab(Pitch);
			X = CP * GMath.CosTab(Yaw);
			Y = CP * GMath.SinTab(Yaw);
			Z = GMath.SinTab(Pitch);
		}
	}
	else
	{
		if (SizeSquared() < SLERP_DELTA)
			Yaw = 0;
		else Yaw = 1;
		Ar.SerializeInt(Yaw, 2);

		if (Yaw)
		{
			Yaw = (appFloor(appAtan2(Y, X) * vToUERadians)) & 65535;
			Pitch = (appFloor(appAtan2(Z, Size2D()) * vToUERadians)) & 65024;
			Yaw = Yaw >> 8;
			Pitch = Pitch >> 9;
			Ar.SerializeInt(Yaw, 256);
			Ar.SerializeInt(Pitch, 128);
		}
	}
}

// Return the FRotator corresponding to the direction that the vector
// is pointing in.  Sets Yaw and Pitch to the proper numbers, and sets
// roll to zero because the roll can't be determined from a vector.
inline FRotator FVector::Rotation() const
{
	FRotator R;

	// Find yaw.
	R.Yaw = appFloor(appAtan2(Y, X) * vToUERadians);

	// Find pitch.
	R.Pitch = appFloor(appAtan2(Z, Size2D()) * vToUERadians);

	// Find roll.
	R.Roll = 0;

	return R;
}

/*-----------------------------------------------------------------------------
	FMatrix.
-----------------------------------------------------------------------------*/
// Floating point 4 x 4  (4 x 3)  KNI-friendly matrix
class FMatrix
{
public:

	// Variables.
	FPlane XPlane; // each plane [x,y,z,w] is a *column* in the matrix.
	FPlane YPlane;
	FPlane ZPlane;
	FPlane WPlane;

	inline FLOAT& M(INT i,INT j) {return ((FLOAT*)&XPlane)[i*4+j];}
	inline const FLOAT& M(INT i,INT j) const {return ((FLOAT*)&XPlane)[i*4+j];}

	// Constructors.
	FMatrix()
	{}
	FMatrix( FPlane InX, FPlane InY, FPlane InZ )
	:	XPlane(InX), YPlane(InY), ZPlane(InZ), WPlane(0,0,0,0)
	{}
	FMatrix( FPlane InX, FPlane InY, FPlane InZ, FPlane InW )
	:	XPlane(InX), YPlane(InY), ZPlane(InZ), WPlane(InW)
	{}


	// Regular transform
	FVector TransformFVector(const FVector &V) const
	{
		FVector FV;

		FV.X = V.X * M(0,0) + V.Y * M(0,1) + V.Z * M(0,2) + M(0,3);
		FV.Y = V.X * M(1,0) + V.Y * M(1,1) + V.Z * M(1,2) + M(1,3);
		FV.Z = V.X * M(2,0) + V.Y * M(2,1) + V.Z * M(2,2) + M(2,3);

		return FV;
	}

	// Homogeneous transform
	FPlane TransformFPlane(const FPlane &P) const
	{
		FPlane FP;

		FP.X = P.X * M(0,0) + P.Y * M(0,1) + P.Z * M(0,2) + M(0,3);
		FP.Y = P.X * M(1,0) + P.Y * M(1,1) + P.Z * M(1,2) + M(1,3);
		FP.Z = P.X * M(2,0) + P.Y * M(2,1) + P.Z * M(2,2) + M(2,3);
		FP.W = P.X * M(3,0) + P.Y * M(3,1) + P.Z * M(3,2) + M(3,3);

		return FP;
	}

	FQuat FMatrixToFQuat();

	// Combine transforms binary operation MxN
	friend FMatrix FMatrixFromFCoords(const FCoords& FC);
	friend FCoords FCoordsFromFMatrix(const FMatrix& FM);

	inline FMatrix CombineTransforms(const FMatrix& N) const
	{
		FMatrix Q;
		for (int i = 0; i < 4; i++) // row
			for (int j = 0; j < 3; j++) // column
				Q.M(i, j) = M(i, 0) * N.M(0, j) + M(i, 1) * N.M(1, j) + M(i, 2) * N.M(2, j) + M(i, 3) * N.M(3, j);
		return (Q);
	}
};

// Conversions for Unreal1 coordinate system class.

inline FMatrix FMatrixFromFCoords(const FCoords& FC)
{
	FMatrix M;
	M.XPlane = FPlane( FC.XAxis.X, FC.XAxis.Y, FC.XAxis.Z, FC.Origin.X );
	M.YPlane = FPlane( FC.YAxis.X, FC.YAxis.Y, FC.YAxis.Z, FC.Origin.Y );
	M.ZPlane = FPlane( FC.ZAxis.X, FC.ZAxis.Y, FC.ZAxis.Z, FC.Origin.Z );
	M.WPlane = FPlane( 0.f,        0.f,        0.f,        1.f         );
	return M;
}

inline FCoords FCoordsFromFMatrix(const FMatrix& FM)
{
	FCoords C;
	C.Origin = FVector( FM.XPlane.W, FM.YPlane.W, FM.ZPlane.W );
	C.XAxis  = FVector( FM.XPlane.X, FM.XPlane.Y, FM.XPlane.Z );
	C.YAxis  = FVector( FM.YPlane.X, FM.YPlane.Y, FM.YPlane.Z );
	C.ZAxis  = FVector( FM.ZPlane.X, FM.ZPlane.Y, FM.ZPlane.Z );
	return C;
}

// Convert this orthogonal coordinate system to a rotation.
inline FRotator FCoords::OrthoRotation() const
{
	constexpr DOUBLE Factor = 32768.0 / PI;
	FRotator R
	(
		appFloor(appAtan2(XAxis.Z, XAxis.Size2D()) * Factor),
		appFloor(appAtan2(XAxis.Y, XAxis.X) * Factor),
		0
	);
	const FVector S = (GMath.UnitCoords / R).YAxis;
	R.Roll = appFloor(appAtan2(ZAxis | S, YAxis | S) * Factor);
	return R;
}

inline FCoords FCoords::ProjectFrom2DCoords(const FVector& PointA, const FVector2D& UVA, const FVector& PointB, const FVector2D& UVB, const FVector& PointC, const FVector2D& UVC)
{
	guardSlow(FCoords::ProjectFrom2DCoords);
	const FVector PN = ((PointA - PointB) ^ (PointC - PointA)).SafeNormalSlow();
	FVector2D UV0 = UVA;
	FVector2D UV1 = UVB;
	FVector2D UV2 = UVC;

	if ((UV0.X == UV1.X) || (UV2.X == UV1.X) || (UV2.X == UV0.X) || (UV0.Y == UV1.Y) || (UV2.Y == UV1.Y) || (UV2.Y == UV0.Y))
	{
		UV1.X += 0.004173f;
		UV1.Y += 0.004173f;
		UV2.X += 0.004173f;
		UV2.Y += 0.004173f;
	}

	FCoords Result;
	FCoords Coords(FVector(0, 0, 0), (PointB - PointA), (PointC - PointA), PN);
	Coords = Coords.Inverse();
	Result.XAxis = FVector(UV1.X - UV0.X, UV2.X - UV0.X, 0.0f).TransformVectorBy(Coords);
	Result.YAxis = FVector(UV1.Y - UV0.Y, UV2.Y - UV0.Y, 0.0f).TransformVectorBy(Coords);
	Result.ZAxis = PN;
	Coords = FCoords(FVector(0, 0, 0), Result.XAxis, Result.YAxis, FVector(PN.X, PN.Y, PN.Z)).Inverse();
	Result.Origin = -FVector(UV0.X - (Result.XAxis | PointA), UV0.Y - (Result.YAxis | PointA), 0.0f).TransformVectorBy(Coords);
	return Result;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FQuat.
-----------------------------------------------------------------------------*/

// floating point quaternion.
class FQuat
{
public:
	// Variables.
	FLOAT X, Y, Z, W;
	// X,Y,Z, W also doubles as the Axis/Angle format.

	// Constructors.
	FQuat()
	{}

	FQuat(FLOAT InX, FLOAT InY, FLOAT InZ, FLOAT InA)
		: X(InX), Y(InY), Z(InZ), W(InA)
	{}

	FQuat(const FRotator& R)
	{
#if 1 // Marco: seams we must use FCoords as middle man.
		*this = FQuat(GMath.UnitCoords / R);
#else
		FLOAT cosp = appCos(R.Pitch * 0.0000479369);
		FLOAT cosy = appCos(R.Yaw * 0.0000479369);
		FLOAT cosr = appCos(R.Roll * 0.0000479369);
		FLOAT sinp = appSin(R.Pitch * 0.0000479369);
		FLOAT siny = appSin(R.Yaw * 0.0000479369);
		FLOAT sinr = appSin(R.Roll * 0.0000479369);

		W = cosp * cosy * cosr + sinp * siny * sinr;
		X = sinp * cosy * cosr + cosp * siny * sinr;
		Y = cosp * siny * cosr - sinp * cosy * sinr;
		Z = cosp * cosy * sinr - sinp * siny * cosr;

		FLOAT L = appSqrt(appPow(W, 2) + appPow(X, 2) + appPow(Y, 2) + appPow(Z, 2));
		W /= L;
		X /= L;
		Y /= L;
		Z /= L;
#endif
	}
	inline FRotator GetRotator() const
	{
#if 1 // Marco: seams we must use FCoords as middle man.
		return GetCoords().OrthoRotation();
#else
		float sinPitchR, cosPitchR, sinYawR, cosYawR, sinRollR, cosRollR;
		FRotator Result;

		sinPitchR = 2.0 * (X * W - Y * Z);
		cosPitchR = appSqrt(1 - sinPitchR * sinPitchR);
		Result.Pitch = appFloor(appAtan2(sinPitchR, cosPitchR) * 10430.37835);

		if (cosPitchR == 0)
		{    // Argh no! Gimbal Lock!
			sinYawR = 2.0 * (X * Y - Z * W);
			cosYawR = 2.0 * (Y * Z + X * W);
			Result.Yaw = appFloor(appAtan2(sinYawR, cosYawR) * 10430.37835);
			Result.Roll = 0;
			// Yaw seems to be 32768 off if Pitch is 49152. This is a quick
			// fix as I'm too lazy to search for the error.
			if (sinPitchR < 0)
				Result.Yaw += 32768;
		}
		else
		{
			sinYawR = 2.0 * (X * Z + Y * Y) / cosPitchR;
			cosYawR = (1.0 - 2.0 * X * X - 2.0 * Y * Y) / cosPitchR;
			Result.Yaw = appFloor(appAtan2(sinYawR, cosYawR) * 10430.37835);
			sinRollR = 2.0 * (X * Y + Z * W) / cosPitchR;
			cosRollR = (1.0 - 2.0 * X * X - 2.0 * Z * Z) / cosPitchR;
			Result.Roll = appFloor(appAtan2(sinRollR, cosRollR) * 10430.37835);
		}
		return Result;
#endif
	}

	FQuat(const FCoords& C)
	{
		// Trace.
		FLOAT Trace = C.XAxis.X + C.YAxis.Y + C.ZAxis.Z + 1.0f;
		// Calculate directly for positive trace.
		if (Trace > 0.f)
		{
			FLOAT S = 0.5f / appSqrt(Trace);
			W = 0.25f / S;
			X = (C.ZAxis.Y - C.YAxis.Z) * S;
			Y = (C.XAxis.Z - C.ZAxis.X) * S;
			Z = (C.YAxis.X - C.XAxis.Y) * S;
			return;
		}
		// Or determine the major diagonal element.
		if ((C.XAxis.X > C.YAxis.Y) && (C.XAxis.X > C.ZAxis.Z))
		{
			FLOAT SZ = 0.5f / appSqrt(1.0f + C.XAxis.X - C.YAxis.Y - C.ZAxis.Z);
			X = 0.5f * SZ;
			Y = (C.XAxis.Y + C.YAxis.X) * SZ;
			Z = (C.XAxis.Z + C.ZAxis.X) * SZ;
			W = (C.YAxis.Z + C.ZAxis.Y) * SZ;
		}
		else if (C.YAxis.Y > C.ZAxis.Z)
		{
			FLOAT SZ = 0.5f / appSqrt(1.0f + C.YAxis.Y - C.XAxis.X - C.ZAxis.Z);
			X = (C.XAxis.Y + C.YAxis.X) * SZ;
			Y = 0.5f * SZ;
			Z = (C.YAxis.Z + C.ZAxis.Y) * SZ;
			W = (C.XAxis.Z + C.ZAxis.X) * SZ;
		}
		else
		{
			FLOAT SZ = 0.5f / appSqrt(1.0f + C.ZAxis.Z - C.XAxis.X - C.YAxis.Y);
			X = (C.XAxis.Z + C.ZAxis.X) * SZ;
			Y = (C.YAxis.Z + C.ZAxis.Y) * SZ;
			Z = 0.5f * SZ;
			W = (C.XAxis.Y + C.YAxis.X) * SZ;
		}
	}
	inline FCoords GetCoords(const FVector& InOrigin = FVector(0, 0, 0)) const
	{
		FLOAT wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
		FCoords Result;

		x2 = X + X;  y2 = Y + Y;  z2 = Z + Z;
		xx = X * x2; xy = X * y2; xz = X * z2;
		yy = Y * y2; yz = Y * z2; zz = Z * z2;
		wx = W * x2; wy = W * y2; wz = W * z2;

		Result.XAxis.X = 1.0f - (yy + zz);
		Result.XAxis.Y = xy - wz;
		Result.XAxis.Z = xz + wy;

		Result.YAxis.X = xy + wz;
		Result.YAxis.Y = 1.0f - (xx + zz);
		Result.YAxis.Z = yz - wx;

		Result.ZAxis.X = xz - wy;
		Result.ZAxis.Y = yz + wx;
		Result.ZAxis.Z = 1.0f - (xx + yy);

		Result.Origin = InOrigin;
		return Result;
	}

	// Binary operators.
	FQuat operator+(const FQuat& Q) const
	{
		return FQuat(X + Q.X, Y + Q.Y, Z + Q.Z, W + Q.W);
	}
	FQuat operator+=(const FQuat& Q) const
	{
		return *this + Q;
	}

	FQuat operator-(const FQuat& Q) const
	{
		return FQuat(X - Q.X, Y - Q.Y, Z - Q.Z, W - Q.W);
	}
	FQuat operator-=(const FQuat& Q) const
	{
		return *this - Q;
	}

	FQuat operator*(const FQuat& Q) const
	{
		return FQuat(
			W*Q.X + X*Q.W + Y*Q.Z - Z*Q.Y,
			W*Q.Y + Y*Q.W + Z*Q.X - X*Q.Z,
			W*Q.Z + Z*Q.W + X*Q.Y - Y*Q.X,
			W*Q.W - X*Q.X - Y*Q.Y - Z*Q.Z
			);
	}
	FQuat operator*=(const FQuat& B) const
	{
		return *this * B;
	}

	FQuat operator*(const FLOAT Scale) const
	{
		return FQuat(Scale*X, Scale*Y, Scale*Z, Scale*W);
	}

	FQuat operator/(const FLOAT Scale) const
	{
		FLOAT Mul = 1.f / Scale;
		return FQuat(X * Mul, Y * Mul, Z * Mul, W * Mul);
	}

	// Unary operators.
	FQuat operator-() const
	{
		return FQuat(X, Y, Z, -W);
	}

	// Misc operators
	UBOOL operator!=(const FQuat& Q) const
	{
		return X != Q.X || Y != Q.Y || Z != Q.Z || W != Q.W;
	}

	inline FQuat Normalize() const
	{
		FLOAT SquareSum = (FLOAT)(X*X + Y*Y + Z*Z + W*W);
		if (SquareSum >= DELTA)
		{
			FLOAT Scale = appFastInvSqrt(SquareSum);
			return FQuat(X * Scale, Y * Scale, Z * Scale, W * Scale);
		}
		else return FQuat(0.f, 0.f, 0.1f, 0.f);
	}

	// Inverse quat
	inline FQuat Inverse() const
	{
		return FQuat(-X, -Y, -Z, W);
	}

	// Serializer.
	friend FArchive& operator<<(FArchive& Ar, FQuat& F)
	{
		return Ar << F.X << F.Y << F.Z << F.W;
	}

	FMatrix FQuatToFMatrix();

	// Warning : assumes normalized quaternions.
	inline FQuat FQuatToAngAxis() const
	{
		FLOAT scale = (FLOAT)appSin(W);
		FQuat A;

		if (scale >= DELTA)
		{
			A.X = Z / scale;
			A.Y = Y / scale;
			A.Z = Z / scale;
			A.W = (2.0f * appAcos(W));
			// Degrees: A.W = ((FLOAT)appAcos(W) * 360.0f) * INV_PI;
		}
		else
		{
			A.X = 0.0f;
			A.Y = 0.0f;
			A.Z = 1.0f;
			A.W = 0.0f;
		}

		return A;
	};

	//
	// Angle-Axis to Quaternion. No normalized axis assumed.
	//
	inline FQuat AngAxisToFQuat() const
	{
		FLOAT scale = X*X + Y*Y + Z*Z;
		FQuat Q;

		if (scale >= DELTA)
		{
			FLOAT invscale = appFastInvSqrt(scale);
			Q.X = X * invscale;
			Q.Y = Y * invscale;
			Q.Z = Z * invscale;
			Q.W = appCos(W * 0.5f); //Radians assumed.
		}
		else
		{
			Q.X = 0.0f;
			Q.Y = 0.0f;
			Q.Z = 1.0f;
			Q.W = 0.0f;
		}
		return Q;
	}

	inline BYTE IsZero()
	{
		return *((QWORD*)this) == 0;
	}

	inline FLOAT SizeSq() const
	{
		return appPow(W, 2) + appPow(X, 2) + appPow(Y, 2) + appPow(Z, 2);
	}
	inline FLOAT Size() const
	{
		return appSqrt(appPow(W, 2) + appPow(X, 2) + appPow(Y, 2) + appPow(Z, 2));
	}

	FQuat SlerpAligned(const FQuat& q, FLOAT a) const;
	FQuat Slerp(const FQuat& q, FLOAT a) const;
};

inline FCoords::FCoords(const FVector& Org, const FQuat& Q)
	: Origin(Org)
{
	FLOAT wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = Q.X + Q.X;  y2 = Q.Y + Q.Y;  z2 = Q.Z + Q.Z;
	xx = Q.X * x2;   xy = Q.X * y2;   xz = Q.X * z2;
	yy = Q.Y * y2;   yz = Q.Y * z2;   zz = Q.Z * z2;
	wx = Q.W * x2;   wy = Q.W * y2;   wz = Q.W * z2;

	XAxis = FVector(1.0f - (yy + zz), xy - wz, xz + wy);
	YAxis = FVector(xy + wz, 1.0f - (xx + zz), yz - wx);
	ZAxis = FVector(xz - wy, yz + wx, 1.0f - (xx + yy));
}

// Dot product of axes to get cos of angle  #Warning some people use .W component here too !
inline FLOAT FQuatDot(const FQuat& Q1, const FQuat& Q2)
{
	return(Q1.X*Q2.X + Q1.Y*Q2.Y + Q1.Z*Q2.Z);
};

// Error measure (angle) between two quaternions, ranged [0..1]
inline FLOAT FQuatError(FQuat& Q1, FQuat& Q2)
{
	// Returns the hypersphere-angle between two quaternions; alignment shouldn't matter, though
	// normalized input is expected.
	FLOAT cosom = Q1.X*Q2.X + Q1.Y*Q2.Y + Q1.Z*Q2.Z + Q1.W*Q2.W;
	return (Abs(cosom) < 0.9999999f) ? appAcos(cosom)*(1.f / PI) : 0.0f;
}

// Ensure quat1 points to same side of the hypersphere as quat2
inline void AlignFQuatWith(FQuat &quat1, const FQuat &quat2)
{
	FLOAT Minus = Square(quat1.X - quat2.X) + Square(quat1.Y - quat2.Y) + Square(quat1.Z - quat2.Z) + Square(quat1.W - quat2.W);
	FLOAT Plus = Square(quat1.X + quat2.X) + Square(quat1.Y + quat2.Y) + Square(quat1.Z + quat2.Z) + Square(quat1.W + quat2.W);

	if (Minus > Plus)
	{
		quat1.X = -quat1.X;
		quat1.Y = -quat1.Y;
		quat1.Z = -quat1.Z;
		quat1.W = -quat1.W;
	}
}

// No-frills spherical interpolation. Assumes aligned quaternions, and the output is not normalized.
inline FQuat SlerpQuat(const FQuat &quat1, const FQuat &quat2, float slerp)
{
	FQuat result;
	float omega, cosom, sininv, scale0, scale1;

	// Get cosine of angle betweel quats.
	cosom = quat1.X * quat2.X +
		quat1.Y * quat2.Y +
		quat1.Z * quat2.Z +
		quat1.W * quat2.W;

	if (cosom < 0.99999999f)
	{
		omega = appAcos(cosom);
		sininv = 1.f / appSin(omega);
		scale0 = appSin((1.f - slerp) * omega) * sininv;
		scale1 = appSin(slerp * omega) * sininv;

		result.X = scale0 * quat1.X + scale1 * quat2.X;
		result.Y = scale0 * quat1.Y + scale1 * quat2.Y;
		result.Z = scale0 * quat1.Z + scale1 * quat2.Z;
		result.W = scale0 * quat1.W + scale1 * quat2.W;
		return result.Normalize();
	}
	else
	{
		return quat1;
	}
}

inline FQuat FQuat::SlerpAligned(const FQuat& q, FLOAT a) const
{
	FQuat result = *this;
	const FQuat qa = q;
	AlignFQuatWith(result, qa);
	return SlerpQuat(result, qa, a);
}
inline FQuat FQuat::Slerp(const FQuat& q, FLOAT a) const
{
	return SlerpQuat(*this, q, a);
}

// Convert quaternion to transformation (rotation) matrix.
// Optimized code based on the Feb 98 GDMAG article by Nick Bobick.
inline FMatrix FQuat::FQuatToFMatrix()
{
	FMatrix M;
	FLOAT wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	x2 = X + X;  y2 = Y + Y;  z2 = Z + Z;
	xx = X * x2;   xy = X * y2;   xz = X * z2;
	yy = Y * y2;   yz = Y * z2;   zz = Z * z2;
	wx = W * x2;   wy = W * y2;   wz = W * z2;

	M.XPlane.X = 1.0f - (yy + zz);
	M.XPlane.Y = xy - wz;
	M.XPlane.Z = xz + wy;
	M.XPlane.W = 0.0f;

	M.YPlane.X = xy + wz;
	M.YPlane.Y = 1.0f - (xx + zz);
	M.YPlane.Z = yz - wx;
	M.YPlane.W = 0.0f;

	M.ZPlane.X = xz - wy;
	M.ZPlane.Y = yz + wx;
	M.ZPlane.Z = 1.0f - (xx + yy);
	M.ZPlane.W = 0.0f;

	M.WPlane.X = 0.0f;
	M.WPlane.Y = 0.0f;
	M.WPlane.Z = 0.0f;
	M.WPlane.W = 1.0f;

	return M;
};

// Matrix-to-Quaternion code.
inline FQuat FMatrix::FMatrixToFQuat()
{
	FQuat Q;

	INT nxt[3] = { 1, 2, 0 };

	FLOAT tr = M(0, 0) + M(1, 1) + M(2, 2);

	// Check the diagonal

	if (tr > 0.f)
	{
		FLOAT s = appSqrt(tr + 1.0f);
		Q.W = s / 2.0f;
		s = 0.5f / s;

		Q.X = (M(1, 2) - M(2, 1)) * s;
		Q.Y = (M(2, 0) - M(0, 2)) * s;
		Q.Z = (M(0, 1) - M(1, 0)) * s;
	}
	else
	{
		// diagonal is negative
		FLOAT  q[4];

		INT i = 0;

		if (M(1, 1) > M(0, 0)) i = 1;
		if (M(2, 2) > M(i, i)) i = 2;

		INT j = nxt[i];
		INT k = nxt[j];

		FLOAT s = appSqrt((M(i, i) - (M(j, j) + M(k, k))) + 1.0f);

		q[i] = s * 0.5f;

		//if (s != 0.0f) s = 0.5f / s;
		if (s > DELTA) s = 0.5f / s;

		q[3] = (M(j, k) - M(k, j)) * s;
		q[j] = (M(i, j) + M(j, i)) * s;
		q[k] = (M(i, k) + M(k, i)) * s;

		Q.X = q[0];
		Q.Y = q[1];
		Q.Z = q[2];
		Q.W = q[3];
	}

	return Q;
};

//
// Taylor series expansion of arccos.
//
#define ATP_PI (3.1415926535897932f)
inline FLOAT appAcosTaylorP1(FLOAT X)
{
	return (ATP_PI / 2.f) - X;
}
inline FLOAT appAcosTaylorP3(FLOAT X)
{
	//return (PI/2)-X-(1.f/6.f)*X*X*X;
	return (ATP_PI / 2.f) - X*(1.f + (1.f / 6.f)*X*X);
}
inline FLOAT appAcosTaylorP5(FLOAT X)
{
	//return (PI/2)-X-(1.f/6.f)*X*X*X-(3.f/40.f)*X*X*X*X*X;
	return (ATP_PI / 2.f) - X*(1.f + X*X*((1.f / 6.f) + X*X*(3.f / 40.f)));
}
inline FLOAT appAcosTaylorP7(FLOAT X)
{
	//return (PI/2)-X-(1.f/6.f)*X*X*X-(3.f/40.f)*X*X*X*X*X-(5.f/112.f)*X*X*X*X*X*X*X;
	return (ATP_PI / 2.f) - X*(1.f + X*X*((1.f / 6.f) + X*X*((3.f / 40.f) + X*X*(5.f / 112.f))));
}
inline FLOAT appAcosTaylorP9(FLOAT X)
{
	return (ATP_PI / 2.f) - X - (1.f / 6.f)*X*X*X - (3.f / 40.f)*X*X*X*X*X - (5.f / 112.f)*X*X*X*X*X*X*X - (35.f / 1152.f)*X*X*X*X*X*X*X*X*X;
}

//
// A truecolor value.
//
#define GET_COLOR_DWORD(color) (*(DWORD*)&(color))
struct alignas(4) FColor
{
	// Variables.
	// This is always RGBA, regardless of byte order!  --ryan.
	BYTE R GCC_PACK(1);
	BYTE G GCC_PACK(1);
	BYTE B GCC_PACK(1);
	BYTE A GCC_PACK(1);

	// Constructors.
	FColor() {}
	FColor(BYTE InR, BYTE InG, BYTE InB)
		: R(InR), G(InG), B(InB) {}
	FColor(BYTE InR, BYTE InG, BYTE InB, BYTE InA)
		: R(InR), G(InG), B(InB), A(InA) {}
	FColor(const FPlane& P)
		: R(Clamp(appFloor(P.X * 256), 0, 255))
		, G(Clamp(appFloor(P.Y * 256), 0, 255))
		, B(Clamp(appFloor(P.Z * 256), 0, 255))
		, A(Clamp(appFloor(P.W * 256), 0, 255))
	{}
	FColor(const FVector& V)
		: R(Clamp(appFloor(V.X * 256), 0, 255))
		, G(Clamp(appFloor(V.Y * 256), 0, 255))
		, B(Clamp(appFloor(V.Z * 256), 0, 255))
		, A(255)
	{}

	// Serializer.
	friend FArchive& operator<< (FArchive& Ar, FColor& Color)
	{
		return Ar << Color.R << Color.G << Color.B << Color.A;
	}

	// Operators.
	inline UBOOL operator==(const FColor& C) const
	{
		//return D==C.D;
		return GET_COLOR_DWORD(*this) == GET_COLOR_DWORD(C);
	}
	inline UBOOL operator!=(const FColor& C) const
	{
		return GET_COLOR_DWORD(*this) != GET_COLOR_DWORD(C);
	}
	inline INT Brightness() const
	{
		return (2 * (INT)R + 3 * (INT)G + 1 * (INT)B) >> 3;
	}
	inline FLOAT FBrightness() const
	{
		return (2.f * R + 3.f * G + 1.f * B) / (6.f * 256.f);
	}
	inline DWORD TrueColor() const
	{
		DWORD D = *(DWORD*)this;
#ifndef __INTEL_BYTE_ORDER__
		D = INTEL_ORDER32(D);
#endif
		return ((D & 0xff) << 16) + (D & 0xff00) + ((D & 0xff0000) >> 16);
	}
	inline _WORD HiColor565() const
	{
		DWORD D = GET_COLOR_DWORD(*this);
#ifndef __INTEL_BYTE_ORDER__
		D = INTEL_ORDER32(D);
#endif
		return ((D & 0xf8) << 8) + ((D & 0xfC00) >> 5) + ((D & 0xf80000) >> 19);
	}
	inline _WORD HiColor555() const
	{
		DWORD D = GET_COLOR_DWORD(*this);
#ifndef __INTEL_BYTE_ORDER__
		D = INTEL_ORDER32(D);
#endif
		return ((D & 0xf8) << 7) + ((D & 0xf800) >> 6) + ((D & 0xf80000) >> 19);
	}
	inline FPlane Plane() const
	{
		return FPlane(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
	}
	inline FVector Vector() const
	{
		return FVector(R / 255.f, G / 255.f, B / 255.f);
	}
	inline FColor Brighten(INT Amount)
	{
		return FColor(Plane() * (1.f - Amount / 24.f));
	}
	inline BYTE IsZero() const
	{
		return (GET_COLOR_DWORD(*this) & 0x00ffffff) == 0;
	}

	/*
	Convert color to HSL:
	Output: X = Hue,	Y = Saturation,	Z = Luminance (Range 0-1)
	*/
	inline FVector GetHSL() const
	{
		guardSlow(FColor::GetHSL);
		FVector HSL(0, 0, 0);
		const FVector ColorVec = Vector();
		const FLOAT cMax = Max(ColorVec.X, Max(ColorVec.Y, ColorVec.Z));
		const FLOAT cMin = Min(ColorVec.X, Min(ColorVec.Y, ColorVec.Z));
		const FLOAT C = cMax - cMin;
		FLOAT SatMod = 1.f;

		{
			FLOAT Hue = 0.f;
			if (C == 0) Hue = 0;
			else if (cMax == ColorVec.X) Hue = appFmod(((ColorVec.Y - ColorVec.Z) / C), 6.f);
			else if (cMax == ColorVec.Y) Hue = (ColorVec.Z - ColorVec.X) / C + 2;
			else Hue = (ColorVec.X - ColorVec.Y) / C + 4;
			Hue *= 60.f;
			if (Hue < 0) Hue += 360.f;
			Hue /= 360.f;

			// Marco: Saturation adjustment based on hue (due to Unreals inaccurate Hue wheel).
			// Note: It is impossible to get some true colors (like yellow) with Unreals Hue color wheel!
			constexpr FLOAT SaturationHack = 0.707155f;
			SatMod = 1.f - ((1.f + appSin(Hue * (PI * 6.f) - (PI / 2.f))) * 0.5f * SaturationHack);

			HSL.X = Hue;
		}
		{
			FLOAT Saturation = 0.f;
			if (cMax == 0)
				Saturation = 1.f;
			else
				Saturation = 1.f - ((C / cMax) * SatMod);
			HSL.Y = Saturation;
			HSL.Z = cMax;
		}
		return HSL;
		unguardSlow;
	}
	// Same as GetHSL but without saturation color correction.
	inline FVector GetHSLReal() const
	{
		guardSlow(FColor::GetHSLReal);
		FVector HSL(0, 0, 0);
		const FVector ColorVec = Vector();
		const FLOAT cMax = Max(ColorVec.X, Max(ColorVec.Y, ColorVec.Z));
		const FLOAT cMin = Min(ColorVec.X, Min(ColorVec.Y, ColorVec.Z));
		const FLOAT C = cMax - cMin;
		FLOAT SatMod = 1.f;

		{
			FLOAT Hue = 0.f;
			if (C == 0) Hue = 0;
			else if (cMax == ColorVec.X) Hue = appFmod(((ColorVec.Y - ColorVec.Z) / C), 6.f);
			else if (cMax == ColorVec.Y) Hue = (ColorVec.Z - ColorVec.X) / C + 2;
			else Hue = (ColorVec.X - ColorVec.Y) / C + 4;
			Hue *= 60.f;
			if (Hue < 0) Hue += 360.f;
			Hue /= 360.f;
			HSL.X = Hue;
		}
		{
			FLOAT Saturation = 0.f;
			if (cMax == 0)
				Saturation = 1.f;
			else
				Saturation = 1.f - ((C / cMax) * SatMod);
			HSL.Y = Saturation;
			HSL.Z = cMax;
		}
		return HSL;
		unguardSlow;
	}
};

inline void SerializeCompressedInt(INT& Value, FArchive& Ar)
{
	if (Ar.IsLoading())
	{
		DWORD Len;
		Ar.SerializeInt(Len, 4);

		switch (Len)
		{
		case 0:
			Ar.SerializeInt(Len, 128);
			Value = Len;
			break;
		case 1:
			Ar.SerializeInt(Len, 4096);
			Value = Len;
			break;
		case 2:
			Ar.SerializeInt(Len, 65536);
			Value = Len;
			break;
		default:
			Ar << Value;
		}
	}
	else
	{
		DWORD Len;
		if (Value >= 0 && Value < 65536)
		{
			if (Value < 128)
			{
				Len = 0;
				Ar.SerializeInt(Len, 4);
				Len = Value;
				Ar.SerializeInt(Len, 128);
			}
			else if (Value < 4096)
			{
				Len = 1;
				Ar.SerializeInt(Len, 4);
				Len = Value;
				Ar.SerializeInt(Len, 4096);
			}
			else
			{
				Len = 2;
				Ar.SerializeInt(Len, 4);
				Len = Value;
				Ar.SerializeInt(Len, 65536);
			}
		}
		else
		{
			Len = 3;
			Ar.SerializeInt(Len, 4);
			Ar << Value;
		}
	}
}

// Interpolated function
struct FInterpCurvePoint
{
	FLOAT InVal;
	FLOAT OutVal;

	UBOOL operator==(const FInterpCurvePoint& Other)
	{
		return (InVal == Other.InVal && OutVal == Other.OutVal);
	}
	friend FArchive& operator<<(FArchive& Ar, FInterpCurvePoint& Point)
	{
		return Ar << Point.InVal << Point.OutVal;
	}
};
struct FInterpCurve
{
	TArray<FInterpCurvePoint> Points;

	FInterpCurve()
		: Points(E_NoInit)
	{}
	inline FLOAT Eval(FLOAT InValue) const
	{
		guardSlow(FInterpCurve::Eval);
		const INT NumPts = Points.Num();
		if (!NumPts)
			return 0.f;
		else if (NumPts == 1 || (Points(0).InVal > InValue))
			return Points(0).OutVal;

		const FInterpCurvePoint* C = &Points(0);
		for (INT i = 1; i < NumPts; ++i)
		{
			if (C[i].InVal >= InValue)
			{
				FLOAT A = (InValue - C[i - 1].InVal) / (C[i].InVal - C[i - 1].InVal);
				return (C[i].OutVal * A) + (C[i - 1].OutVal * (1.f - A));
			}
		}
		return Points.Last().OutVal;
		unguardSlow;
	}

	friend FArchive& operator<<(FArchive& Ar, FInterpCurve& Curve)
	{
		return Ar << Curve.Points;
	}
	void operator=(const FInterpCurve& Other)
	{
		Points = Other.Points;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
