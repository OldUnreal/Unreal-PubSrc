/*=============================================================================
	UnTemplate.h: Unreal templates.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Type information.
-----------------------------------------------------------------------------*/

#ifndef _INCL_UNTEMPLATE_H_
#define _INCL_UNTEMPLATE_H_

//
// Type information for initialization.
//
template <class T> struct TTypeInfoBase
{
public:
	typedef const T& ConstInitType;
	static UBOOL NeedsDestructor() {return 1;}
	static UBOOL DefinitelyNeedsDestructor() {return 0;}
	static const T& ToInit( const T& In ) {return In;}
};
template <class T> struct TTypeInfo : public TTypeInfoBase<T>
{
};

template <> struct TTypeInfo<BYTE> : public TTypeInfoBase<BYTE>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<SBYTE> : public TTypeInfoBase<SBYTE>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<ANSICHAR> : public TTypeInfoBase<ANSICHAR>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<INT> : public TTypeInfoBase<INT>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<DWORD> : public TTypeInfoBase<DWORD>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<_WORD> : public TTypeInfoBase<_WORD>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<SWORD> : public TTypeInfoBase<SWORD>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<QWORD> : public TTypeInfoBase<QWORD>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<SQWORD> : public TTypeInfoBase<SQWORD>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<FName> : public TTypeInfoBase<FName>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};
template <> struct TTypeInfo<UObject*> : public TTypeInfoBase<UObject*>
{
public:
	static UBOOL NeedsDestructor() {return 0;}
};

/*-----------------------------------------------------------------------------
	Standard templates.
-----------------------------------------------------------------------------*/

template< class T > inline T Abs( const T A )
{
	return (A>=(T)0) ? A : -A;
}
template< class T > inline T Sgn( const T A )
{
	return (A>0) ? 1 : ((A<0) ? -1 : 0);
}
template< class T > inline T Max( const T A, const T B )
{
	return (A>=B) ? A : B;
}
template< class T > inline T Max( const T A, const T B, const T C )
{
	T X = Max<T>( A, B );
	return (X>=C) ? X : C;
}
template< class T > inline T Max( const T A, const T B, const T C, const T D )
{
	T X = Max<T>( A, B );
	T Y = Max<T>( C, D );
	return (X>=Y) ? X : Y;
}
template< class T > inline T Max3(const T A, const T B, const T C)
{
	return Max(Max(A, B), C);
}
template< class T > inline T Min( const T A, const T B )
{
	return (A<=B) ? A : B;
}
template< class T > inline T Min( const T A, const T B, const T C )
{
	T X = Min<T>( A, B );
	return (X<=C) ? X : C;
}
template< class T > inline T Min( const T A, const T B, const T C, const T D )
{
	T X = Min<T>( A, B );
	T Y = Min<T>( C, D );
	return (X<=Y) ? X : Y;
}
template< class T > inline T Min3(const T A, const T B, const T C)
{
	return Min(Min(A, B), C);
}
template< class T > inline T Square( const T A )
{
	return A*A;
}
template< class T > inline T Cube( const T A )
{
	return A*A*A;
}
template< class T > inline T Quad( const T A )
{
	return A*A*A*A;
}
template< class T > inline T Clamp( const T X, const T Min, const T Max )
{
	return X<Min ? Min : X<Max ? X : Max;
}
template< class T > inline T Align( const T Ptr, uintptr_t Alignment )
{
	return (T)(((uintptr_t)Ptr + Alignment - 1) & ~(Alignment - 1));
}
template< class T > inline void Exchange( T& A, T& B )
{
	const T Temp = A;
	A = B;
	B = Temp;
}
template< class T > T Lerp( T& A, T& B, FLOAT Alpha )
{
	return (T)(A + Alpha * (B - A));
}
/*
template< class T > T* CastUnsafe( UObject* Src )
{
	return Src && Src->GetClass()->IsClassType(T::StaticClass()->CastFlags) ? (T*)Src : NULL;
}
*/
inline DWORD GetTypeHash( const BYTE A )
{
	return A;
}
inline DWORD GetTypeHash( const SBYTE A )
{
	return A;
}
inline DWORD GetTypeHash( const _WORD A )
{
	return A;
}
inline DWORD GetTypeHash( const SWORD A )
{
	return A;
}
inline DWORD GetTypeHash( const INT A )
{
	return A;
}
inline DWORD GetTypeHash( const DWORD A )
{
	return A;
}
inline DWORD GetTypeHash(const QWORD A)
{
	return (DWORD)A ^ ((DWORD)(A >> 16)) ^ ((DWORD)(A >> 32));
}
inline DWORD GetTypeHash(const SQWORD A)
{
	return (DWORD)A ^ ((DWORD)(A >> 16)) ^ ((DWORD)(A >> 32));
}
/*
inline DWORD GetTypeHash( const QWORD A )
{
	return (DWORD)A+((DWORD)(A>>32) * 23);
}
inline DWORD GetTypeHash( const SQWORD A )
{
	return (DWORD)A+((DWORD)(A>>32) * 23);
}
*/
inline DWORD GetTypeHash( const TCHAR* S )
{
	return appStrihash(S);
}
#define ExchangeB(A,B) {UBOOL T=A; A=B; B=T;}

/*----------------------------------------------------------------------------
	Standard macros.
----------------------------------------------------------------------------*/

// Number of elements in an array.
#define ARRAY_COUNT( array ) \
	( int(sizeof(array) / sizeof((array)[0])) )

// Offset of a struct member.
#define STRUCT_OFFSET( struc, member ) \
	( (intptr_t)&((struc*)NULL)->member )

// stijn: the above macro is invalid in standard C++
// the one below is valid as long as struc is a standard layout data type
//#define STRUCT_OFFSET(struc, member) offsetof(struc, member)


/*-----------------------------------------------------------------------------
	Metaprogramming utils.
-----------------------------------------------------------------------------*/

//
// Constant expression bit util.
//
template<QWORD N, DWORD Bit=63> struct TBits
{
	static constexpr DWORD BitCount = (N>>(Bit-1)) ? Bit : TBits<N,Bit-1>::BitCount;
	static constexpr QWORD BitMask = (1ull << BitCount) - 1;
	static constexpr QWORD MostSignificant = 1ull << (BitCount-1);
	static constexpr QWORD CeilLogTwo = (MostSignificant >= N) ? BitCount-1 : BitCount; //Compile time version of appCeilLogTwo
};
template<QWORD N> struct TBits<N,0>
{
	static constexpr DWORD BitCount = 0;
	static constexpr QWORD BitMask = 0;
	static constexpr QWORD MostSignificant = 0;
	static constexpr QWORD CeilLogTwo = 0; //appCeilLogTwo(0)=0
};
static_assert( TBits<20>::BitCount        == 5,        "Error in TBits<20> #BitCount#");
static_assert( TBits<20>::BitMask         == 0b011111, "Error in TBits<20> #BitMask#");
static_assert( TBits<20>::MostSignificant == 0b010000, "Error in TBits<20> #MostSignificant#");
static_assert( TBits<20>::CeilLogTwo      == 5,        "Error in TBits<20> #CeilLogTwo#");
static_assert( TBits<32>::BitCount        == 6,        "Error in TBits<32> #BitCount#");
static_assert( TBits<32>::BitMask         == 0b111111, "Error in TBits<32> #BitMask#");
static_assert( TBits<32>::MostSignificant == 0b100000, "Error in TBits<32> #MostSignificant#");
static_assert( TBits<32>::CeilLogTwo      == 5,        "Error in TBits<32> #CeilLogTwo#");


//
// Raw data util.
//

// stijn: Higor uses this in OpenGLDrv, but this code was not C++-compliant prior to C++17...

#if __cplusplus >= 201703L || _MSVC_LANG >= 201402L
#define USES_TRAWDATA 1

template< typename T > struct TRawData
{
public:
	static void Copy( T& Out, const T& In);
	static void Move( T& To, T& From);
//	static void Swap( T& A, T& B);
	static bool Equals( const T& A, const T& B);

public:
	static constexpr bool Supports16 = !!USES_SSE_INTRINSICS;
	static constexpr bool Supports8  = !!BUILD_64;

	// Precomputed steps.
	static constexpr PTRINT Offset16 = 0;
	static constexpr PTRINT Loops16  = Supports16 ? ((sizeof(T)-Offset16) / 16) : 0;

	static constexpr PTRINT Offset8  = Offset16 + Loops16 * 16;
	static constexpr PTRINT Loops8   = Supports8  ? ((sizeof(T)-Offset8)  / 8) : 0;

	static constexpr PTRINT Offset4  = Offset8  + Loops8 * 8;
	static constexpr PTRINT Loops4   = (sizeof(T) - Offset4) / 4;

	static constexpr PTRINT Offset2  = Offset4  + Loops4 * 4;
	static constexpr PTRINT Loops2   = (sizeof(T) - Offset2) / 2;

	static constexpr PTRINT Offset1  = Offset2  + Loops2 * 2;
	static constexpr PTRINT Loops1   = (sizeof(T) - Offset1);

private:
	// Single unit inners
	template< typename D > static void CopyWithTypeSingle( D& Out, const D& In) { Out = In; }
	template< typename D > static void MoveWithTypeSingle( D& To, D& From)       { To = From; From = (D)0; }
	template< typename D > static bool EqualsWithTypeSingle( const D& A, const D& B) { return A == B; }

#if USES_SSE_INTRINSICS
	// SSE single unit inners
	template<> static void CopyWithTypeSingle<__m128i>( __m128i& Out, const __m128i& In)
	{
		_mm_storeu_si128( &Out, _mm_loadu_si128(&In) );
	}
	template<> static void MoveWithTypeSingle<__m128i>( __m128i& To, __m128i& From)
	{
		_mm_storeu_si128( &To, _mm_loadu_si128(&From) );
		_mm_storeu_si128( &From, _mm_setzero_si128() );
	}
	template<> static bool EqualsWithTypeSingle<__m128i>( const __m128i& A, const __m128i& B)
	{
		return _mm_movemask_ps( _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_loadu_si128(&A),_mm_loadu_si128(&B))) ) == 0b1111;
	}
#endif

	// Loop inners
	template< typename D, DWORD Loops > static void CopyWithType( D& Out, const D& In)
	{
		for ( DWORD i=0; i<Loops; i++)
			CopyWithTypeSingle( (&Out)[i], (&In)[i]);
	}
	template< typename D, DWORD Loops > static void MoveWithType( D& To, D& From)
	{
		for ( DWORD i=0; i<Loops; i++)
			MoveWithTypeSingle( (&To)[i], (&From)[i]);
	}
	template< typename D, DWORD Loops > static bool EqualsWithType( const D& A, const D& B)
	{
		for ( DWORD i=0; i<Loops; i++)
			if ( !EqualsWithTypeSingle((&A)[i],(&B)[i]) )
				return false;
		return true;
	}
};
template<typename T> inline void TRawData<T>::Copy( T& Out, const T& In)
{
#if USES_SSE_INTRINSICS
	CopyWithType<__m128i,Loops16>( *(__m128i*)&Out, *(__m128i*)&In);
#endif
	CopyWithType<QWORD,Loops8>( *(QWORD*)((BYTE*)&Out + Offset8), *(QWORD*)((BYTE*)&In + Offset8) );
	CopyWithType<DWORD,Loops4>( *(DWORD*)((BYTE*)&Out + Offset4), *(DWORD*)((BYTE*)&In + Offset4) );
	CopyWithType<WORD, Loops2>( *(WORD*) ((BYTE*)&Out + Offset2 ),*(WORD*) ((BYTE*)&In + Offset2) );
	CopyWithType<BYTE, Loops1>( *((BYTE*)&Out + Offset1), *((BYTE*)&In + Offset1) );
}
template<typename T> inline void TRawData<T>::Move( T& To, T& From)
{
#if USES_SSE_INTRINSICS
	MoveWithType<__m128i,Loops16>( *(__m128i*)&To, *(__m128i*)&From);
#endif
	MoveWithType<QWORD,Loops8>( *(QWORD*)((BYTE*)&To + Offset8), *(QWORD*)((BYTE*)&From + Offset8) );
	MoveWithType<DWORD,Loops4>( *(DWORD*)((BYTE*)&To + Offset4), *(DWORD*)((BYTE*)&From + Offset4) );
	MoveWithType<WORD, Loops2>( *(WORD*) ((BYTE*)&To + Offset2 ),*(WORD*) ((BYTE*)&From + Offset2) );
	MoveWithType<BYTE, Loops1>( *((BYTE*)&To + Offset1), *((BYTE*)&From + Offset1) );
}
template<typename T> inline bool TRawData<T>::Equals( const T& A, const T& B)
{
	return 
#if USES_SSE_INTRINSICS
	EqualsWithType<__m128i,Loops16>( *(__m128i*)&A, *(__m128i*)&B) &&
#endif
	EqualsWithType<QWORD,Loops8>( *(QWORD*)((BYTE*)&A + Offset8), *(QWORD*)((BYTE*)&B + Offset8) ) &&
	EqualsWithType<DWORD,Loops4>( *(DWORD*)((BYTE*)&A + Offset4), *(DWORD*)((BYTE*)&B + Offset4) ) &&
	EqualsWithType<WORD, Loops2>( *(WORD*) ((BYTE*)&A + Offset2 ),*(WORD*) ((BYTE*)&B + Offset2) ) &&
	EqualsWithType<BYTE, Loops1>( *((BYTE*)&A + Offset1), *((BYTE*)&B + Offset1) );
}

#endif

/*-----------------------------------------------------------------------------
	Allocators.
-----------------------------------------------------------------------------*/

template <class T> class TAllocator
{};

/*-----------------------------------------------------------------------------
	Dynamic array template.
-----------------------------------------------------------------------------*/

//
// Base dynamic array.
//
class CORE_API FArray
{
public:
	void* GetData()
	{
		return Data;
	}
	const void* GetData() const
	{
		return Data;
	}
	UBOOL IsValidIndex( INT i ) const
	{
		return i>=0 && i<ArrayNum;
	}
	inline INT Num() const
	{
		//checkSlow(ArrayNum>=0);
		//checkSlow(ArrayMax>=ArrayNum);
		return ArrayNum;
	}
	inline INT Max() const
	{
		return ArrayMax;
	}
	void InsertZeroed( INT Index, INT Count, INT ElementSize )
	{
		guardSlow(FArray::InsertZeroed);
		// gam ---
		verifySlow(ElementSize>0);
		// sjs rem'd -trips all over- checkSlow(Count>0);
		verifyfSlow(Index>=0 && Index <= ArrayNum, TEXT("OutOfArrayBounds %i/%i"), Index, ArrayNum);
		verifyfSlow(ArrayNum>=0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		// --- gam
		Insert( Index, Count, ElementSize );
		appMemzero( (BYTE*)Data+Index*ElementSize, Count*ElementSize );
		unguardSlow;
	}
	void Insert( INT Index, INT Count, INT ElementSize )
	{
		guardSlow(FArray::Insert);
		verifyfSlow(Count >= 0, TEXT("InsertInvalid %i"), Count);
		// gam ---
		verifySlow(ElementSize>0);
		// sjs rem'd -trips all over- checkSlow(Count>0);
		// --- gam
		verifyfSlow(Index >= 0 && Index <= ArrayNum, TEXT("OutOfArrayBounds %i/%i"), Index, ArrayNum);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);

		INT OldNum = ArrayNum;
		if( (ArrayNum+=Count)>ArrayMax )
		{
			ArrayMax = ArrayNum + 3*ArrayNum/8 + 32;
			Realloc( ElementSize );
		}
		appMemmove
		(
			(BYTE*)Data + (Index+Count )*ElementSize,
			(BYTE*)Data + (Index       )*ElementSize,
			              (OldNum-Index)*ElementSize
		);

		unguardSlow;
	}
	INT AddNoCheck (INT Count)
	{
		INT Index = ArrayNum;
		ArrayNum += Count;
		return Index;
	}
	INT Add( INT Count, INT ElementSize )
	{
		guardSlow(FArray::Add);
		// gam ---
		// sjs rem'd -trips all over- checkSlow(Count>0);
		verifySlow(ElementSize>0);
		// --- gam
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);

		INT Index = ArrayNum;
		if( (ArrayNum+=Count)>ArrayMax )
		{
			ArrayMax = ArrayNum + 3*ArrayNum/8 + 32;
			Realloc( ElementSize );
		}

		return Index;
		unguardSlow;
	}
	INT AddZeroed( INT ElementSize, INT n=1 )
	{
		guardSlow(FArray::AddZeroed);
		// gam ---
		verifySlow(ElementSize>0);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		// --- gam

		INT Index = Add( n, ElementSize );
		appMemzero( (BYTE*)Data+Index*ElementSize, n*ElementSize );
		return Index;
		unguardSlow;
	}
	void Shrink( INT ElementSize )
	{
		guardSlow(FArray::Shrink);
		verifySlow(ElementSize>0); // gam
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		if( ArrayMax != ArrayNum )
		{
			ArrayMax = ArrayNum;
			Realloc( ElementSize );
		}
		unguardSlow;
	}
	// sjs ---
	void SetSize( INT Count, INT ElementSize )
	{
		guardSlow(FArray::SetSize);
		// gam ---
		verifyfSlow(Count >= 0, TEXT("CountInvalid %i"), Count);
		verifySlow(ElementSize>0);
		// --- gam
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		ArrayNum = ArrayMax = Count;
		Realloc( ElementSize );
		unguardSlow;
	}
	// --- sjs
	void Empty( INT ElementSize, INT Slack=0 )
	{
		guardSlow(FArray::Empty);
		// gam ---
		verifySlow(ElementSize>0);
		verifyfSlow(Slack >= 0, TEXT("Invalid Slack %i"), Slack);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		// --- gam
		ArrayNum = 0;
		ArrayMax = Slack;
		Realloc(ElementSize);
		unguardSlow;
	}
	FArray()
	:	Data	( NULL )
	,	ArrayNum( 0 )
	,	ArrayMax( 0 )
	{}
	FArray( ENoInit )
	{}
	~FArray()
	{
		guardSlow(FArray::~FArray);
		EmptyFArray();
		unguardSlow;
	}
	inline void EmptyFArray()
	{
		guardSlow(FArray::EmptyFArray);
		if (Data)
			appFree(Data);
		Data = NULL;
		ArrayNum = ArrayMax = 0;
		unguardSlow;
	}
	void CountBytes( FArchive& Ar, INT ElementSize )
	{
		guardSlow(FArray::CountBytes);
		Ar.CountBytes( ArrayNum*ElementSize, ArrayMax*ElementSize );
		unguardSlow;
	}
	void Remove( INT Index, INT Count, INT ElementSize );
	void RemoveNoRealloc( INT Index, INT Count, INT ElementSize );

	inline void ExchangeFArray(FArray* Ar)
	{
		guardSlow(FArray::ExchangeFArray);
		void* TmpPtr = Data;
		INT TmpNum = ArrayNum;
		INT TmpMax = ArrayMax;
		Data = Ar->Data;
		ArrayNum = Ar->ArrayNum;
		ArrayMax = Ar->ArrayMax;
		Ar->Data = TmpPtr;
		Ar->ArrayNum = TmpNum;
		Ar->ArrayMax = TmpMax;
		unguardSlow;
	}
protected:
	void Realloc( INT ElementSize );
	FArray( INT InNum, INT ElementSize )
	:	Data    ( NULL  )
	,	ArrayNum( InNum )
	,	ArrayMax( InNum )
	{
		if(InNum)
			Realloc( ElementSize );
	}
	void* Data;
	INT	  ArrayNum;
	INT	  ArrayMax;
};

//
// Templated dynamic array.
//
template< class T > class TArray : public FArray
{
public:
	typedef T ElementType;
	TArray()
	:	FArray()
	{}
	TArray( INT InNum )
	:	FArray( InNum, sizeof(T) )
	{}
	TArray( const TArray& Other )
	:	FArray( Other.ArrayNum, sizeof(T) )
	{
		guardSlow(TArray::copyctor);
		if( TTypeInfo<T>::NeedsDestructor() )
		{
			ArrayNum=0;
			for( INT i=0; i<Other.ArrayNum; i++ )
				new(*this)T(Other(i));
		}
		else if( sizeof(T)!=1 )
		{
			for( INT i=0; i<ArrayNum; i++ )
				(*this)(i) = Other(i);
		}
		else if ( ArrayNum>0 )
		{
            guardSlow(DoCopy);
			appMemcpy( &(*this)(0), &Other(0), ArrayNum * sizeof(T) );
			unguardSlow;
		}
		unguardSlow;
	}
	TArray( ENoInit )
	: FArray( E_NoInit )
	{}
	~TArray()
	{
		checkSlow(ArrayNum>=0);
		checkSlow(ArrayMax>=ArrayNum);
		Remove( 0, ArrayNum );
	}
	T& operator()( INT i )
	{
		guardSlow(TArray::operator());
		verifyfSlow(i >= 0 && i <= ArrayNum, TEXT("OutOfArrayBounds %i/%i"), i, ArrayNum);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		return ((T*)Data)[i];
		unguardSlow;
	}
	const T& operator()( INT i ) const
	{
		guardSlow(TArray::operator());
		verifyfSlow(i >= 0 && i <= ArrayNum, TEXT("OutOfArrayBounds %i/%i"), i, ArrayNum);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		return ((T*)Data)[i];
		unguardSlow;
	}
	T Pop()
	{
		guardSlow(TArray::Pop);
		checkf(ArrayNum > 0, TEXT("Pop out of bounds %i"), ArrayNum);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		T Result = ((T*)Data)[ArrayNum-1];
		Remove( ArrayNum-1 );
		return Result;
		unguardSlow;
	}
	T& Last( INT c=0 )
	{
		guardSlow(TArray::Last);
		checkf(c < ArrayNum, TEXT("Last out of bounds %i/%i"), c, ArrayNum);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		return ((T*)Data)[ArrayNum-c-1];
		unguardSlow;
	}
	const T& Last( INT c=0 ) const
	{
		guardSlow(TArray::Last);
		checkf(c < ArrayNum, TEXT("Last out of bounds %i/%i"), c, ArrayNum);
		verifyfSlow(ArrayNum >= 0 && ArrayNum <= ArrayMax, TEXT("AllocArrayOutOfBounds %i/%i"), ArrayNum, ArrayMax);
		return ((T*)Data)[ArrayNum-c-1];
		unguardSlow;
	}
	void Shrink()
	{
		guardSlow(TArray::Shrink);
		FArray::Shrink( sizeof(T) );
		unguardSlow;
	}
	UBOOL FindItem( const T& Item, INT& Index ) const
	{
		guardSlow(TArray::FindItem);
		for( Index=0; Index<ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				return 1;
		return 0;
		unguardSlow;
	}
	INT FindItemIndex( const T& Item ) const
	{
		guardSlow(TArray::FindItemIndex);
		for( INT Index=0; Index<ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				return Index;
		return INDEX_NONE;
		unguardSlow;
	}
	friend FArchive& operator<<( FArchive& Ar, TArray& A )
	{
		guard(TArray<<);
		if (Ar.SerializeRefs())
		{
			if (sizeof(T) != 1) // Skip byte arrays as they never contain pointers.
			{
				for (INT i = 0; i < A.ArrayNum; i++)
					Ar << A(i);
			}
		}
		else
		{
			A.CountBytes(Ar);
			if (sizeof(T) == 1)
			{
				// Serialize simple bytes which require no construction or destruction.
				Ar << AR_INDEX(A.ArrayNum);
				if (Ar.IsLoading())
				{
					A.ArrayMax = A.ArrayNum;
					A.Realloc(sizeof(T));
				}
				// gam ---
				if (A.Num() > 0)
					Ar.Serialize(&A(0), A.Num());
				else
					Ar.Serialize(NULL, 0);
				// --- gam
			}
			else if (Ar.IsLoading())
			{
				// Load array.
				INT NewNum = 0; // gam
				Ar << AR_INDEX(NewNum);
				A.Empty(NewNum);
				for (INT i = 0; i < NewNum; i++)
					Ar << *new(A)T;
			}
			else
			{
				// Save array.
				Ar << AR_INDEX(A.ArrayNum);
				for (INT i = 0; i < A.ArrayNum; i++)
					Ar << A(i);
			}
		}
		return Ar;
		unguard;
	}
	void CountBytes( FArchive& Ar )
	{
		guardSlow(TArray::CountBytes);
		FArray::CountBytes( Ar, sizeof(T) );
		unguardSlow;
	}

	// Add, Insert, Remove, Empty interface.
	INT Add( INT n=1 )
	{
		guardSlow(TArray::Add);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		return FArray::Add( n, sizeof(T) );
		unguardSlow;
	}
	// stijn: used in Render - only to be used if you've preallocated enough bytes (e.g., with Reserve)
	INT AddNoCheck (INT n=1)
	{
		return FArray::AddNoCheck(n);
	}
	// sjs ---
	void SetSize( INT n=1 )
	{
		guardSlow(TArray::SetSize);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		FArray::SetSize( n, sizeof(T) );
		unguardSlow;
	}
	// --- sjs
	void Insert( INT Index, INT Count=1 )
	{
		guardSlow(TArray::Insert);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		FArray::Insert( Index, Count, sizeof(T) );
		unguardSlow;
	}
	void InsertZeroed( INT Index, INT Count=1 )
	{
		guardSlow(TArray::InsertZeroed);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		FArray::InsertZeroed( Index, Count, sizeof(T) );
		unguardSlow;
	}
	void Remove( INT Index, INT Count=1 )
	{
		guardSlow(TArray::Remove);
		verifyf(Index>=0 && (Index + Count) <= ArrayNum, TEXT("Remove out of bounds %i + %i / %i"), Index, Count, ArrayNum);
		if( TTypeInfo<T>::NeedsDestructor() )
			for( INT i=Index; i<Index+Count; i++ )
				(&(*this)(i))->~T();
		FArray::Remove( Index, Count, sizeof(T) );
		unguardSlow;
	}
	void RemoveNoRealloc( INT Index, INT Count=1 )
	{
		guardSlow(TArray::Remove);
		verifyf(Index >= 0 && (Index + Count) <= ArrayNum, TEXT("Remove out of bounds %i + %i / %i"), Index, Count, ArrayNum);
		if( TTypeInfo<T>::NeedsDestructor() )
			for( INT i=Index; i<Index+Count; i++ )
				(&(*this)(i))->~T();
		FArray::RemoveNoRealloc( Index, Count, sizeof(T) );
		unguardSlow;
	}
	void Empty( INT Slack=0 )
	{
		guardSlow(TArray::Empty);
		if( TTypeInfo<T>::NeedsDestructor() )
			for( INT i=0; i<ArrayNum; i++ )
				(&(*this)(i))->~T();
		FArray::Empty( sizeof(T), Slack );
		unguardSlow;
	}

	// Functions dependent on Add, Remove.
	TArray& operator+( const TArray& Other )
	{
		guardSlow(TArray::operator=);
		if( this != &Other )
		{
			for( INT i=0; i<Other.ArrayNum; i++ )
				new( *this )T( Other(i) );
		}
		return *this;
		unguardSlow;
	}
	TArray& operator+=( const TArray& Other )
	{
		guardSlow(TArray::operator=);
		if( this != &Other )
		{
			*this = *this + Other;
		}
		return *this;
		unguardSlow;
	}
	TArray& operator=( const TArray& Other )
	{
		guardSlow(TArray::operator=);
		if( this != &Other )
		{
			Empty( Other.ArrayNum );
			for( INT i=0; i<Other.ArrayNum; i++ )
				new( *this )T( Other(i) );
		}
		return *this;
		unguardSlow;
	}
	INT AddItem( const T& Item )
	{
		guardSlow(TArray::AddItem);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		new(*this) T(Item);
		return Num() - 1;
		unguardSlow;
	}
	INT AddZeroed( INT n=1 )
	{
		guardSlow(TArray::AddZeroed);
		return FArray::AddZeroed( sizeof(T), n );
		unguardSlow;
	}
	INT AddUniqueItem( const T& Item )
	{
		guardSlow(TArray::AddUniqueItem);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		for( INT Index=0; Index<ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				return Index;
		return AddItem( Item );
		unguardSlow;
	}
	INT RemoveItem( const T& Item )
	{
		guardSlow(TArray::RemoveItem);
		INT OriginalNum=ArrayNum;
		for( INT Index=0; Index<ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				Remove( Index-- );
		return OriginalNum - ArrayNum;
		unguardSlow;
	}
	INT RemoveItemNoRealloc(const T& Item)
	{
		guardSlow(TArray::RemoveItemNoRealloc);
		INT OriginalNum = ArrayNum;
		for (INT Index = 0; Index < ArrayNum; Index++)
			if ((*this)(Index) == Item)
				RemoveNoRealloc(Index--);
		return OriginalNum - ArrayNum;
		unguardSlow;
	}
	void InsertItem(INT Index, const T& Item)
	{
		guardSlow(TArray::InsertItem);
		Insert(Index);
		(*this)(Index) = Item;
		unguardSlow;
	}
	// stijn: used in Render
	void EmptyNoRealloc()
	{
		ArrayNum = 0;
	}
	// stijn: used in Render
	void Reserve(INT NewArrayMax)
	{
		if (NewArrayMax > ArrayMax)
		{
			ArrayMax = NewArrayMax;
			Realloc(sizeof(T));
		}
	}
	inline void ExchangeArray(TArray<T>* Other)
	{
		ExchangeFArray(Other);
	}

	// Iterator.
	class TIterator
	{
	public:
		TIterator( TArray<T>& InArray ) : Array(InArray), Index(-1) { ++*this;      }
		void operator++()      { ++Index;                                           }
		void RemoveCurrent()   { Array.Remove(Index--); }
		INT GetIndex()   const { return Index;                                      }
		operator UBOOL() const { return Index < Array.Num();                        }
		T& operator*()   const { return Array(Index);                               }
		T* operator->()  const { return &Array(Index);                              }
		T& GetCurrent()  const { return Array( Index );                             }
		T& GetPrev()     const { return Array( Index ? Index-1 : Array.Num()-1 );   }
		T& GetNext()     const { return Array( Index<Array.Num()-1 ? Index+1 : 0 ); }
	private:
		TArray<T>& Array;
		INT Index;
	};
};
template< class T > class TArrayNoInit : public TArray<T>
{
public:
	TArrayNoInit()
	: TArray<T>(E_NoInit)
	{}
	TArrayNoInit& operator=(const TArrayNoInit& Other)
	{
		TArray<T>::operator=(Other);
		return *this;
	}
	inline void ExchangeArray(TArrayNoInit<T>* Other)
	{
		FArray::ExchangeFArray(Other);
	}
	inline void ExchangeArray(TArray<T>* Other)
	{
		FArray::ExchangeFArray(Other);
	}
};
//
// Array operator news.
//
template <class T> void* operator new( size_t Size, TArray<T>& Array )
{
	guardSlow(TArray::operator new);
	INT Index = Array.FArray::Add(1,sizeof(T));
	return &Array(Index);
	unguardSlow;
}
template <class T> void* operator new( size_t Size, TArray<T>& Array, INT Index )
{
	guardSlow(TArray::operator new);
	Array.FArray::Insert(Index,1,sizeof(T));
	return &Array(Index);
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	Transactional array.
-----------------------------------------------------------------------------*/

template< class T > class TTransArray : public TArray<T>
{
public:
	// Constructors.
	TTransArray( UObject* InOwner, INT InNum=0 )
	:	TArray<T>( InNum )
	,	Owner( InOwner )
	{
		checkSlow(Owner);
	}
	TTransArray( UObject* InOwner, const TArray<T>& Other )
	:	TArray<T>( Other )
	,	Owner( InOwner )
	{
		checkSlow(Owner);
	}
	TTransArray& operator=( const TTransArray& Other )
	{
		operator=( (const TArray<T>&)Other );
		return *this;
	}

	// Add, Insert, Remove, Empty interface.
	INT Add( INT Count=1 )
	{
		guardSlow(TTransArray::Add);
		INT Index = TArray<T>::Add( Count );
		if( GUndo )
			GUndo->SaveArray( Owner, this, Index, Count, 1, sizeof(T), SerializeItem, DestructItem );
		return Index;
		unguardSlow;
	}
	void Insert( INT Index, INT Count=1 )
	{
		guardSlow(TTransArray::InsertZeroed);
		FArray::Insert( Index, Count, sizeof(T) );
		if( GUndo )
			GUndo->SaveArray( Owner, this, Index, Count, 1, sizeof(T), SerializeItem, DestructItem );
		unguardSlow;
	}
	void Remove( INT Index, INT Count=1 )
	{
		guardSlow(TTransArray::Remove);
		if( GUndo )
			GUndo->SaveArray( Owner, this, Index, Count, -1, sizeof(T), SerializeItem, DestructItem );
		TArray<T>::Remove( Index, Count );
		unguardSlow;
	}
	void Empty( INT Slack=0 )
	{
		guardSlow(TTransArray::Empty);
		if( GUndo )
			GUndo->SaveArray( Owner, this, 0, this->ArrayNum, -1, sizeof(T), SerializeItem, DestructItem );
		TArray<T>::Empty( Slack );
		unguardSlow;
	}

	// Functions dependent on Add, Remove.
	TTransArray& operator=( const TArray<T>& Other )
	{
		guardSlow(TTransArray::operator=);
		if( this != &Other )
		{
			Empty( Other.Num() );
			for( INT i=0; i<Other.Num(); i++ )
				new( *this )T( Other(i) );
		}
		return *this;
		unguardSlow;
	}
	INT AddItem( const T& Item )
	{
		guardSlow(TTransArray::AddItem);
		checkSlow(!TTypeInfo<T>::DefinitelyNeedsDestructor());
		new(*this) T(Item);
		return TArray<T>::Num() - 1;
		unguardSlow;
	}
	INT AddZeroed( INT n=1 )
	{
		guardSlow(TTransArray::AddZeroed);
		INT Index = Add(n);
		appMemzero( &(*this)(Index), n*sizeof(T) );
		return Index;
		unguardSlow;
	}
	INT AddUniqueItem( const T& Item )
	{
		guardSlow(TTransArray::AddUniqueItem);
		for( INT Index=0; Index<this->ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				return Index;
		return AddItem( Item );
		unguardSlow;
	}
	INT RemoveItem( const T& Item )
	{
		guardSlow(TTransArray::RemoveItem);
		INT OriginalNum=this->ArrayNum;
		for( INT Index=0; Index<this->ArrayNum; Index++ )
			if( (*this)(Index)==Item )
				Remove( Index-- );
		return OriginalNum - this->ArrayNum;
		unguardSlow;
	}

	// TTransArray interface.
	UObject* GetOwner()
	{
		return Owner;
	}
	void ModifyItem( INT Index )
	{
		guardSlow(TTransArray::ModifyItem);
		if( GUndo )
			GUndo->SaveArray( Owner, this, Index, 1, 0, sizeof(T), SerializeItem, DestructItem );
		unguardSlow;
	}
	void ModifyAllItems()
	{
		guardSlow(TTransArray::ModifyAllItems);
		if( GUndo )
			GUndo->SaveArray( Owner, this, 0, this->Num(), 0, sizeof(T), SerializeItem, DestructItem );
		unguardSlow;
	}
	friend FArchive& operator<<( FArchive& Ar, TTransArray& A )
	{
		guard(TTransArray<<);
		if( !Ar.IsTrans() )
			Ar << (TArray<T>&)A;
		return Ar;
		unguard;
	}
	inline void ExchangeArray(TTransArray<T>* Other)
	{
		FArray::ExchangeFArray(Other);
	}
	inline void ExchangeArray(TArray<T>* Other)
	{
		FArray::ExchangeFArray(Other);
	}
protected:
	static void SerializeItem( FArchive& Ar, void* TPtr )
	{
		guardSlow(TArray::SerializeItem);
		Ar << *(T*)TPtr;
		unguardSlow;
	}
	static void DestructItem( void* TPtr )
	{
		guardSlow(TArray::SerializeItem);
		((T*)TPtr)->~T();
		unguardSlow;
	}
	UObject* Owner;
private:

	// Disallow the copy constructor.
	TTransArray( const TArray<T>& Other )
	{}
};

//
// Transactional array operator news.
//
template <class T> void* operator new( size_t Size, TTransArray<T>& Array )
{
	guardSlow(TArray::operator new);
	INT Index = Array.Add();
	return &Array(Index);
	unguardSlow;
}
template <class T> void* operator new( size_t Size, TTransArray<T>& Array, INT Index )
{
	guardSlow(TArray::operator new);
	Array.Insert(Index);
	return &Array(Index);
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	Lazy loading.
-----------------------------------------------------------------------------*/

//
// Lazy loader base class.
//
class FLazyLoader
{
	friend class ULinkerLoad;
	friend class UFontExporter;
protected:
	FArchive*	 SavedAr;
	INT          SavedPos;
public:
	FLazyLoader()
	: SavedAr( NULL )
	, SavedPos( 0 )
	{}
	virtual void Load() _VF_BASE;
	virtual void Unload() _VF_BASE;
};

//
// Lazy-loadable dynamic array.
//
template <class T> class TLazyArray : public TArray<T>, public FLazyLoader
{
public:
	TLazyArray( INT InNum=0 )
	: TArray<T>( InNum )
	, FLazyLoader()
	{}
	~TLazyArray() noexcept(false)
	{
		guard(TLazyArray::~TLazyArray);
		if( SavedAr )
			SavedAr->DetachLazyLoader( this );
		unguard;
	}
#if LOAD_ON_DEMAND /* Breaks because of untimely accesses of operator() !! */
    T& operator()( INT i )
	{
		guardSlow(TArray::operator());
		checkSlow(i>=0);
		checkSlow(i<=ArrayNum);
		checkSlow(ArrayMax>=ArrayNum);
		if( SavedPos>0 )
			Load();
		return ((T*)Data)[i];
		unguardSlow;
	}
	const T& operator()( INT i ) const
	{
		guardSlow(TArray::operator());
		checkSlow(i>=0);
		checkSlow(i<=ArrayNum);
		checkSlow(ArrayMax>=ArrayNum);
		if( SavedPos>0 )
			Load();
		return ((T*)Data)[i];
		unguardSlow;
	}
#endif
	void Load()
	{
		// Make sure this array is loaded.
		guard(TLazyArray::Load);
		if( SavedPos>0 )
		{
			// Lazy load it now.
			INT PushedPos = SavedAr->Tell();
			SavedAr->Seek( SavedPos );
			*SavedAr << (TArray<T>&)*this;
			SavedPos *= -1;
			SavedAr->Seek( PushedPos );
		}
		unguard;
	}
	void DetachLazyLoader()
	{
		if( SavedAr )
			SavedAr->DetachLazyLoader( this );
		SavedPos = 0;
		SavedAr = NULL;
	}
	void Unload()
#if __GNUG__
		;	// Function declaration.
#else
	{
		// Make sure this array is unloaded.
		guard(TLazyArray::Unload);
		if( SavedPos<0 )
		{
			// Unload it now.
			TArray<T>::Empty();
			SavedPos *= -1;
		}
		unguard;
	}
#endif
	friend FArchive& operator<<( FArchive& Ar, TLazyArray& This )
	{
		guard(TLazyArray<<);
		if( Ar.IsLoading() )
		{
			INT SeekPos=0;
			if( Ar.Ver() <= 61 )
			{
				//oldver: Handles dynamic arrays of fixed-length serialized items only.
				Ar.AttachLazyLoader( &This );
				INT SkipCount = 0; // gam
				Ar << AR_INDEX(SkipCount);
				SeekPos = Ar.Tell() + SkipCount*sizeof(T);

			}
			else
			{
				Ar << SeekPos;
				Ar.AttachLazyLoader( &This );
			}
			if( !GLazyLoad )
				This.Load();
			Ar.Seek( SeekPos );
		}
		else if( Ar.IsSaving() && Ar.Ver()>61 )
		{
			// Save out count for skipping over this data.
			INT CountPos = Ar.Tell();
			Ar << CountPos << (TArray<T>&)This;
			INT EndPos = Ar.Tell();
			Ar.Seek( CountPos );
			Ar << EndPos;
			Ar.Seek( EndPos );
		}
		else Ar << (TArray<T>&)This;
		return Ar;
		unguard;
	}
	inline void ExchangeArray(TLazyArray<T>* Other)
	{
		FArray::ExchangeFArray(Other);
	}
	inline void ExchangeArray(TArray<T>* Other)
	{
		FArray::ExchangeFArray(Other);
	}
};
#if __GNUG__
template <class T> void TLazyArray<T>::Unload()
{
	// Make sure this array is unloaded.
	 guard(TLazyArray::Unload);
	 if( SavedPos<0 && GLazyLoad )
	 {
		// Unload it now.
		this->Empty();
		SavedPos *= -1;
	 }
	 unguard;
}
#endif

/*-----------------------------------------------------------------------------
	Dynamic strings.
-----------------------------------------------------------------------------*/

//
// A dynamically sizeable string.
//
class CORE_API FString : protected TArray<TCHAR>
{
public:
	FString()
	: TArray<TCHAR>()
	{}
	FString( const FString& Other )
	: TArray<TCHAR>( Other.ArrayNum )
	{
		if( ArrayNum )
			appMemcpy( &(*this)(0), &Other(0), ArrayNum*sizeof(TCHAR) );
	}
	FString( const TCHAR* In )
	: TArray<TCHAR>((In && *In) ? (appStrlen(In) + 1) : 0)
	{
		if( ArrayNum )
			appMemcpy( &(*this)(0), In, ArrayNum*sizeof(TCHAR) );
	}
	FString(const TCHAR* Start, const TCHAR* End)
		: TArray<TCHAR>((End > Start) ? ((End - Start) + 1) : 0)
	{
		if (ArrayNum)
		{
			appMemcpy(&(*this)(0), Start, (ArrayNum - 1) * sizeof(TCHAR));
			(*this)(ArrayNum - 1) = 0;
		}
	}
	FString ( const TCHAR* In, INT MaxBytes )
	: TArray<TCHAR>( MaxBytes + 1 )
	{
		// stijn: was
		// if ( ArrayNum && In && appStrlen(In) > MaxBytes)
		// before. This didn't really make sense...
		if ( ArrayNum && In && appStrlen(In) >= MaxBytes)
		{
			appMemcpy(&(*this)(0), In, MaxBytes * sizeof(TCHAR));
			(*this)(MaxBytes) = 0;
		}
	}
	FString( ENoInit )
	: TArray<TCHAR>( E_NoInit )
	{}
	FString( const ANSICHAR* AnsiIn );
	FString( const ANSICHAR* AnsiIn , StringEncoding Encoding );
	explicit FString( BYTE   Arg, INT Digits=1 );
	explicit FString( SBYTE  Arg, INT Digits=1 );
	explicit FString( _WORD  Arg, INT Digits=1 );
	explicit FString( SWORD  Arg, INT Digits=1 );
	explicit FString( INT    Arg, INT Digits=1 );
	explicit FString( DWORD  Arg, INT Digits=1 );
	explicit FString( FLOAT  Arg, INT Digits=1, INT RightDigits=0, UBOOL LeadZero=1 );
	explicit FString( DOUBLE Arg, INT Digits=1, INT RightDigits=0, UBOOL LeadZero=1 );
	FString& operator=( const TCHAR* Other )
	{
	    guardSlow(FString:: operator=(const TCHAR*));
		if( (Data == NULL) || (&(*this)(0)!=Other) ) // gam
		{
			// Added check for Other beeing NULL before derencing. --han
			ArrayNum = ArrayMax = (Other && *Other) ? appStrlen(Other) + 1 : 0;
			Realloc( sizeof(TCHAR) );
			if( ArrayNum )
				appMemcpy( &(*this)(0), Other, ArrayNum*sizeof(TCHAR) );
		}
		return *this;
		unguardSlow;
	}
	FString& operator=( const FString& Other )
	{
		guardSlow(FString:: operator=(const FString&));
		if( Data==NULL || this!=&Other )
		{
			checkSlow(Other.Num()>=0);
			//checkSlow(Other.GetData()!=NULL);
			ArrayNum = ArrayMax = Other.Num();
			guardSlow(DoRealloc);
			Realloc( sizeof(TCHAR) );
			unguardSlow;
			if( ArrayNum )
			{
				guardSlow(DoCopy);
				checkSlow(GetData() != NULL);
				appMemcpy( GetData(), Other.GetData(), ArrayNum*sizeof(TCHAR) );
				unguardSlow;
			}
		}
		return *this;
		unguardSlow;
	}
	FString& operator=( const ANSICHAR* AnsiOther);
    TCHAR& operator[]( INT i )
	{
		guardSlow(FString::operator());
		checkSlow(i>=0);
		checkSlow(i<=ArrayNum);
		checkSlow(ArrayMax>=ArrayNum);
		return ((TCHAR*)Data)[i];
		unguardSlow;
	}
	const TCHAR& operator[]( INT i ) const
	{
		guardSlow(FString::operator());
		checkSlow(i>=0);
		checkSlow(i<ArrayNum);
		checkSlow(ArrayMax>=ArrayNum);
		return ((TCHAR*)Data)[i];
		unguardSlow;
	}

	~FString()
	{
		TArray<TCHAR>::Empty();
	}
	void Empty()
	{
		TArray<TCHAR>::Empty();
	}
	void Shrink()
	{
		TArray<TCHAR>::Shrink();
	}
	const TCHAR* operator*() const
	{
		return Num() ? &(*this)(0) : TEXT("");
	}
	operator UBOOL() const
	{
		return Num()!=0;
	}
	TArray<TCHAR>& GetCharArray()
	{
		//warning: Operations on the TArray<CHAR> can be unsafe, such as adding
		// non-terminating 0's or removing the terminating zero.
		return (TArray<TCHAR>&)*this;
	}
	FString& operator+=( const TCHAR* Str )
	{
		if( Str && *Str != '\0' ) // gam
		{
			if( ArrayNum )
			{
				INT Index = ArrayNum-1;
				Add( appStrlen(Str) );
				appStrcpy( &(*this)(Index), Str ); // stijn: mem safety ok
			}
			else if( *Str )
			{
				Add( appStrlen(Str)+1 );
				appStrcpy( &(*this)(0), Str ); // stijn: mem safety ok
			}
		}
		return *this;
	}
	FString& operator ^= (const TCHAR * Str)
	{
		if (Str && *Str != '\0') // gam
		{
			if (ArrayNum)
			{
				INT Index = ArrayNum;
				Add(appStrlen(Str) + 1);
				(*this)(Index - 1) = ' ';
				appStrcpy(&(*this)(Index), Str);
			}
			else if (*Str)
			{
				Add(appStrlen(Str) + 2);
				(*this)(0) = ' ';
				appStrcpy(&(*this)(1), Str);
			}
		}
		else if (ArrayNum)
		{
			Add(1);
			(*this)(ArrayNum - 2) = ' ';
			(*this)(ArrayNum - 1) = '\0';
		}
		else
		{
			Add(2);
			(*this)(0) = ' ';
			(*this)(1) = '\0';
		}
		return *this;
	}
	FString& operator+=(const TCHAR Ch)
	{
		if (Ch)
		{
			if (ArrayNum)
			{
				INT Index = ArrayNum - 1;
				Add(1);
				(*this)(Index) = Ch;
				(*this)(Index + 1) = 0;
			}
			else
			{
				Add(2);
				(*this)(0) = Ch;
				(*this)(1) = 0;
			}
		}
		return *this;
	}
	FString& operator-=(const TCHAR* Str)
	{
		if (Str && *Str && ArrayNum)
		{
			TCHAR* Result = &(*this)(0);
			INT SectionSize = appStrlen(Str);

			while (ArrayNum)
			{
				TCHAR* StrOffset = appStrstr(Result, Str);
				if (!StrOffset)
					break;
				TCHAR* StrEnd = (StrOffset + SectionSize);
				while (*StrEnd)
					*StrOffset++ = *StrEnd++;
				*StrOffset = 0;
				ArrayNum -= SectionSize;
			}
		}
		return *this;
	}
	FString& operator-=(const FString& Str)
	{
		return operator-=(*Str);
	}
	FString& operator+=( const FString& Str )
	{
		return operator+=( *Str );
	}
	FString operator+( const TCHAR* Str ) const
	{
		return FString( *this ) += Str;
	}
	FString operator+( const FString& Str ) const
	{
		return operator+( *Str );
	}
	FString& operator^=(const FString& Str)
	{
		return operator^=(*Str);
	}
	FString operator^(const TCHAR* Str) const
	{
		return FString(*this) ^= Str;
	}
	FString operator^(const FString& Str) const
	{
		return operator^(*Str);
	}
	FString& operator*=( const TCHAR* Str )
	{
		if( ArrayNum>1 && (*this)(ArrayNum-2)!=PATH_SEPARATOR[0] )
			*this += PATH_SEPARATOR;
		return *this += Str;
	}
	FString& operator*=( const FString& Str )
	{
		return operator*=( *Str );
	}
	FString operator*( const TCHAR* Str ) const
	{
		return FString( *this ) *= Str;
	}
	FString operator*( const FString& Str ) const
	{
		return operator*( *Str );
	}
	UBOOL operator<=( const TCHAR* Other ) const
	{
		return !(appStricmp( **this, Other ) > 0);
	}
	UBOOL operator<( const TCHAR* Other ) const
	{
		return appStricmp( **this, Other ) < 0;
	}
	UBOOL operator>=( const TCHAR* Other ) const
	{
		return !(appStricmp( **this, Other ) < 0);
	}
	UBOOL operator>( const TCHAR* Other ) const
	{
		return appStricmp( **this, Other ) > 0;
	}
	UBOOL operator==( const TCHAR* Other ) const
	{
		return appStricmp( **this, Other )==0;
	}
	UBOOL operator==( const FString& Other ) const
	{
		return appStricmp( **this, *Other )==0;
	}
	UBOOL operator!=( const TCHAR* Other ) const
	{
		return appStricmp( **this, Other )!=0;
	}
	UBOOL operator!=( const FString& Other ) const
	{
		return appStricmp( **this, *Other )!=0;
	}
	INT Len() const
	{
		return Num() ? Num()-1 : 0;
	}
	FString Left( INT Count ) const
	{
		return FString( Clamp(Count,0,Len()), **this );
	}
	FString LeftChop( INT Count ) const
	{
		return FString( Clamp(Len()-Count,0,Len()), **this );
	}
	FString Right( INT Count ) const
	{
		return FString( **this + Len()-Clamp(Count,0,Len()) );
	}
	FString Mid( INT Start, INT Count=MAXINT ) const
	{
		DWORD End = (DWORD)Start+(DWORD)Count;
		Start    = Clamp( (DWORD)Start, (DWORD)0,     (DWORD)Len() );
		End      = Clamp( (DWORD)End,   (DWORD)Start, (DWORD)Len() );
		return FString( End-Start, **this + Start );
	}

	// ../System/Unreal.exe -> Unreal
	FString GetFilenameOnly() const;
	// ../System/Unreal.exe -> exe
	const TCHAR* GetFileExtension() const;
	// ../System/Unreal.exe -> Unreal.exe
	const TCHAR* GetFileNameExt() const;
	// ../System/Unreal.exe -> ../System/
	FString GetFilePath() const;

	static FString GetFilenameOnlyStr(const TCHAR* Str);
	static const TCHAR* GetFileExtensionStr(const TCHAR* Str);
	static const TCHAR* GetFileNameExtStr(const TCHAR* Str);
	static FString GetFilePathStr(const TCHAR* Str);

	UBOOL Divide(const TCHAR* Divider, FString& LeftPart, FString& RightPart) const;
	UBOOL Split(const TCHAR* Divider, FString* LeftPart, FString* RightPart, UBOOL bRight = FALSE) const;

	inline INT InStr( const TCHAR* SubStr) const
	{
		const TCHAR* Tmp = appStrstr(**this,SubStr); //Unicode changes for Win32 - Smirftsch
		return Tmp ? (Tmp-**this) : INDEX_NONE;
	}
	inline INT InStr(const FString& SubStr) const
	{
		return InStr(*SubStr);
	}
	inline INT InStrRight(const TCHAR* SubStr) const
	{
		for (INT i = Len() - 1; i >= 0; i--)
		{
			INT j;
			for (j = 0; SubStr[j]; j++)
				if ((*this)(i + j) != SubStr[j])
					break;
			if (!SubStr[j])
				return i;
		}
		return INDEX_NONE;
	}
	inline INT InStrRight(const FString& SubStr) const
	{
		return InStrRight(*SubStr);
	}
	inline INT InStrOffset(const TCHAR* SubStr, INT Start) const
	{
		if (Start < 0)
			Start = 0;
		if (appStrlen(SubStr) > Len() - Start)
			return INDEX_NONE;
		const TCHAR* Tmp = appStrstr(**this + Start, SubStr);
		return Tmp ? (Tmp-**this) : INDEX_NONE;
	}
	inline INT InStrOffset(const FString& SubStr, INT Start) const
	{
		return InStrOffset(*SubStr, Start);
	}

	inline FString Caps() const
	{
		FString New( **this );
		if (ArrayNum)
			appStrupr(&New(0));
		return New;
	}
	inline FString Locs() const
	{
		FString New( **this );
		if (ArrayNum)
			appStrlwr(&New(0));
		return New;
	}

	UBOOL Extract(const TCHAR* LeftDivider, const TCHAR* RightDivider, FString& MidString) const;
	UBOOL AdvExtract(const TCHAR* LeftDivider, const TCHAR* RightDivider, FString& MidString) const;

	FString Reverse() const
	{
		guard(FString::Reverse);
		FString New;
		if (ArrayNum)
		{
			New.SetSize(ArrayNum);
			const TCHAR* Src = **this;
			TCHAR* Output = &New(0);
			for (INT i = Len() - 1; i > -1; i--, ++Output)
				*Output = Src[i];
			*Output = 0;
		}
		return New;
		unguard;
	}
	FString Replace(const TCHAR* Find, const TCHAR* ReplaceWith) const
	{
		guard(FString::Replace);
		FString Result = *this;
		const INT FromLen = appStrlen(Find);
		INT i = Result.InStr(Find);
		while (i != INDEX_NONE)
		{
			Result = Result.Left(i) + ReplaceWith + Result.Mid(i + FromLen);
			i = Result.InStr(Find);
		}
		return Result;
		unguard;
	}
	inline INT Int() const
	{
		guardSlow(FString::Int);
		return appAtoi(**this);
		unguardSlow;
	}
	inline FLOAT Float() const
	{
		guardSlow(FString::Float);
		return appAtof(**this);
		unguardSlow;
	}
	inline UBOOL IsAlpha() const
	{
		guardSlow(FString::IsAlpha);
		const TCHAR* S = **this;
		while (*S)
		{
			if (!appIsAlpha(*S))
				return FALSE;
			++S;
		}
		return TRUE;
		unguardSlow;
	}
/*
	UBOOL IsAlnum() // Crashing Scriptcompiler!?
	{
		guard(FString::IsAlnum);
		for (INT Pos = 0; Pos < Len(); Pos++)
			if (!appIsAlnum((*this)(Pos)))
				return 0;
		return 1;
		unguard;
	}
*/
	inline UBOOL IsNum() const
    {
		guardSlow(FString::IsNum);
		const TCHAR* S = **this;
		if (*S == '+' || *S == '-')
			++S;
		while (*S)
		{
			if (!appIsDigit(*S))
				return FALSE;
			++S;
		}
		return TRUE;
		unguardSlow;
    }
	inline UBOOL IsFloat() const
	{
		guardSlow(FString::IsFloat);
		const TCHAR* S = **this;
		if (*S == '+' || *S == '-')
			++S;
		UBOOL bHadDecimal = FALSE;
		while (*S)
		{
			if (!appIsDigit(*S))
			{
				if (*S == '.')
				{
					if (bHadDecimal)
						return FALSE;
					else bHadDecimal = TRUE;
				}
				else if (*S == 'f' || *S == 'F') // Trailing float inidicator.
					return bHadDecimal && (S[1] == '\x0');
				else if (*S == 'e' || *S == 'E') // Exponent.
				{
					if (S[1] != '+' && S[1] != '-')
						return FALSE;
					S += 2;
				}
				else return FALSE;
			}
			++S;
		}
		return TRUE;
		unguardSlow;
	}
	inline UBOOL IsHex() const
    {
		guardSlow(FString::IsHex);
		const TCHAR* S = **this;
		while (*S)
		{
			if (!appIsHexDigit(*S))
				return FALSE;
			++S;
		}
		return TRUE;
		unguardSlow;
    }
	inline UBOOL IsName(UBOOL AllowDigitStart = FALSE) const
	{
		guardSlow(FString::IsName);
		if (Len() == 0)
			return TRUE;
		const TCHAR* S = **this;
		if (!AllowDigitStart && appIsDigit(*S))
			return FALSE;
		while (*S)
		{
			if (!appIsAlnum(*S) && *S!='_')
				return FALSE;
			++S;
		}
		return TRUE;
		unguardSlow;
	}
	// Tries to preserve the dot and a zero afterwards, but ditches otherwise needless zeros.
	static FString NiceFloat(FLOAT Value)
	{
		guard(FString::NiceFloat);
		FString Text = FString::Printf(TEXT("%g"), Value);
		if (Text.InStr(TEXT(".")) == INDEX_NONE)
			Text += TEXT(".0");
		return Text;
		unguard;
	}
	FString LeftPad( INT ChCount );
	FString RightPad( INT ChCount );
	static FString Printf( const TCHAR* Fmt, ... );
	static FString Printf(const TCHAR* Fmt, va_list Args);
	static FString Chr( TCHAR Ch );
	void AppendPartialStr(const TCHAR* Start, const TCHAR* End)
	{
		guardSlow(FString::AppendPartialStr);
		if (Start && Start != End)
		{
			INT strLen = End - Start;
			INT Index;
			if (ArrayNum)
			{
				Index = ArrayNum - 1;
				Add(strLen);
			}
			else
			{
				Index = 0;
				Add(strLen + 1);
			}
			appMemcpy(&(*this)(Index), Start, strLen * sizeof(TCHAR));
			Last() = 0;
		}
		unguardSlow;
	}
	// Used after converting a multi-byte encoded string to a fixed-width TCHAR string
	void RecalculateLength()
	{
		guardSlow(FString::RecalculateLength);
		const TCHAR* Buf = **this;
		if (Buf && *Buf)
			ArrayNum = appStrlen(Buf) + 1;
		else
			ArrayNum = 0;
		unguardSlow;
	}
	void ShortenString(); // Shorten string to first null terminator.
	void ReplaceColorCodes(UBOOL bToINIFormat); // Replace any color codes in string to or from INI save format.
	CORE_API friend FArchive& operator<<( FArchive& Ar, FString& S );
	FString StripDelimiter() const; // Strip delimiters from this string.

	inline void ExchangeArray(FString* Other)
	{
		ExchangeFArray(Other);
	}

	void SerializeNetString(FArchive& Ar);

	friend struct FStringNoInit;
private:
	FString( INT InCount, const TCHAR* InSrc )
	:	TArray<TCHAR>( InCount ? InCount+1 : 0 )
	{
		if( ArrayNum )
			appStrncpy( &(*this)(0), InSrc, InCount+1 );
	}
};

// stijn: same as FString but without iconv conversion for serialization
class CORE_API FStringRaw : public FString
{
public:
	FStringRaw(const TCHAR* Other)
	{
		if( (Data == NULL) || (&(*this)(0)!=Other) ) // gam
		{
			ArrayNum = ArrayMax = (Other && *Other) ? appStrlen(Other)+1 : 0;
			Realloc( sizeof(TCHAR) );
			if( ArrayNum )
				appMemcpy( &(*this)(0), Other, ArrayNum*sizeof(TCHAR) );
		}
	}
	CORE_API friend FArchive& operator<<(FArchive& Ar, FStringRaw& S);

	inline void ExchangeArray(FStringRaw* Other)
	{
		ExchangeFArray(Other);
	}
	inline void ExchangeArray(FString* Other)
	{
		ExchangeFArray(Other);
	}
};

struct CORE_API FStringNoInit : public FString
{
	FStringNoInit()
	: FString( E_NoInit )
	{}
	FStringNoInit& operator=( const TCHAR* Other )
	{
	    guardSlow(FString:: operator=(const TCHAR*));
		if( (Data == NULL) || (&(*this)(0)!=Other) ) // gam
		{
			ArrayNum = ArrayMax = (Other && *Other) ? appStrlen(Other) + 1 : 0;
			Realloc( sizeof(TCHAR) );
			if( ArrayNum )
				appMemcpy( &(*this)(0), Other, ArrayNum*sizeof(TCHAR) );
		}
		return *this;
		unguardSlow;
	}
	FStringNoInit& operator=( const FString& Other )
	{
		if (Data == NULL || this != &Other)
		{
			ArrayNum = ArrayMax = Other.Num();
			Realloc( sizeof(TCHAR) );
			if( ArrayNum )
				appMemcpy( &(*this)(0), *Other, ArrayNum*sizeof(TCHAR) );
		}
		return *this;
	}
	FStringNoInit& operator=(const FStringNoInit& Other)
	{
		if (Data == NULL || this != &Other)
		{
			ArrayNum = ArrayMax = Other.Num();
			Realloc(sizeof(TCHAR));
			if (ArrayNum)
				appMemcpy(&(*this)(0), *Other, ArrayNum * sizeof(TCHAR));
		}
		return *this;
	}
	inline void ExchangeArray(FStringNoInit* Other)
	{
		ExchangeFArray(Other);
	}
	inline void ExchangeArray(FString* Other)
	{
		ExchangeFArray(Other);
	}
};
inline DWORD GetTypeHash( const FString& S )
{
	return appStrihash(*S);
}
template <> struct TTypeInfo<FString> : public TTypeInfoBase<FString>
{
	typedef const TCHAR* ConstInitType;
	static const TCHAR* ToInit( const FString& In ) {return *In;}
	static UBOOL DefinitelyNeedsDestructor() {return 0;}
};

/*----------------------------------------------------------------------------
	Special archivers.
----------------------------------------------------------------------------*/

//
// String output device.
//
class FStringOutputDevice : public FString, public FOutputDevice
{
public:
	FStringOutputDevice( const TCHAR* InStr=TEXT("") )
	: FString( InStr )
	{}
	void Serialize( const TCHAR* Data, EName Event )
	{
		*this += (TCHAR*)Data;
	}
};

//
// Buffer writer.
//
class FBufferWriter : public FArchive
{
public:
	FBufferWriter( TArray<BYTE>& InBytes )
	: Bytes( InBytes )
	, Pos( 0 )
	{
		ArIsSaving = 1;
	}
	void Serialize( void* InData, INT Length )
	{
		if( Pos+Length>Bytes.Num() )
			Bytes.Add( Pos+Length-Bytes.Num() );
		if( Length == 1 )
			Bytes(Pos) = ((BYTE*)InData)[0];
		else if (Length > 0)
			appMemcpy( &Bytes(Pos), InData, Length );
		Pos += Length;
	}
	INT Tell()
	{
		return Pos;
	}
	void Seek( INT InPos )
	{
		Pos = InPos;
	}
	INT TotalSize()
	{
		return Bytes.Num();
	}
private:
	TArray<BYTE>& Bytes;
	INT Pos;
};

//
// Buffer archiver.
//
class FBufferArchive : public FBufferWriter, public TArray<BYTE>
{
public:
	FBufferArchive()
	: FBufferWriter( (TArray<BYTE>&)*this )
	{}
};

//
// Buffer reader.
//
class CORE_API FBufferReader : public FArchive
{
public:
	FBufferReader( const TArray<BYTE>& InBytes )
	:	Bytes	( InBytes )
	,	Pos 	( 0 )
	{
		ArIsLoading = ArIsTrans = 1;
	}
	void Serialize( void* Data, INT Num )
	{
		checkSlow(Pos>=0);
		checkSlow(Pos+Num<=Bytes.Num());
		if( Num == 1 )
			((BYTE*)Data)[0] = Bytes(Pos);
		else if ( Num > 0)
			appMemcpy( Data, &Bytes(Pos), Num );
		Pos += Num;
	}
	INT Tell()
	{
		return Pos;
	}
	INT TotalSize()
	{
		return Bytes.Num();
	}
	void Seek( INT InPos )
	{
		check(InPos>=0);
		check(InPos<=Bytes.Num());
		Pos = InPos;
	}
	UBOOL AtEnd()
	{
		return Pos>=Bytes.Num();
	}
private:
	const TArray<BYTE>& Bytes;
	INT Pos;
};

/*----------------------------------------------------------------------------
	TMap.
----------------------------------------------------------------------------*/

// TMapBase but for UnrealScript codebase. Written by Marco
class FScriptMapBase
{
public:
	struct USPair
	{
		INT HashNext, HashValue;
		BYTE* Key;
		BYTE* Value;

		USPair(BYTE* InKey, BYTE* InValue, INT iHash)
			: HashValue(iHash), Key(InKey), Value(InValue)
		{}
		~USPair()
		{
			appFree(Key);
			appFree(Value);
		}
	};
	TArray<USPair> Pairs;
	INT* Hash;
	INT HashCount;

	inline USPair* GetBasePair()
	{
		return Pairs.Num() ? (&Pairs(0) - 1) : nullptr;
	}
	inline USPair& GetPair(const INT Index)
	{
		return Pairs(Index - 1);
	}
	inline void RehashRecommended()
	{
		if (Pairs.Num() == 0)
		{
			if (Hash)
				delete[] Hash;
			Hash = NULL;
			HashCount = 0;
		}
		else
		{
			HashCount = 1 << appCeilLogTwo((Pairs.Num() >> 1) + 8);
			Rehash();
		}
	}
	inline void Rehash()
	{
		guardSlow(FScriptMapBase::Rehash);
		HashCount = Max(HashCount, 8);
		INT i, HashMask = (HashCount - 1);
		INT* NewHash = new INT[HashCount];
		appMemzero(NewHash, HashCount*sizeof(INT));
		for (i = 0; i < Pairs.Num(); i++)
		{
			INT iHash = (Pairs(i).HashValue & HashMask);
			Pairs(i).HashNext = NewHash[iHash];
			NewHash[iHash] = i + 1;
		}
		if (Hash)
			delete[] Hash;
		Hash = NewHash;
		unguardSlow;
	}
	inline void Relax()
	{
		guardSlow(FScriptMapBase::Relax);
		while (HashCount > Pairs.Num() * 2 + 8)
			HashCount = HashCount >> 1;
		Rehash();
		unguardSlow;
	}
	inline void* Add(BYTE* InKey, BYTE* InValue, INT iHash)
	{
		guardSlow(FScriptMapBase::Add);
		USPair& Pair = *new(Pairs) USPair(InKey, InValue, iHash);
		if (Hash)
		{
			iHash = (iHash & (HashCount - 1));
			Pair.HashNext = Hash[iHash];
			Hash[iHash] = Pairs.Num();
			if (HashCount * 2 + 8 < Pairs.Num())
			{
				HashCount = HashCount << 1;
				Rehash();
			}
		}
		else Rehash();
		return InValue;
		unguardSlow;
	}

	FScriptMapBase()
		: Hash(NULL), HashCount(0)
	{}
	FScriptMapBase(ENoInit)
		: Pairs(E_NoInit)
	{}
	~FScriptMapBase()
	{
		guardSlow(FScriptMapBase::~TMapBase);
		if (Hash)
			delete[] Hash;
		Hash = NULL;
		HashCount = 0;
		unguardSlow;
	}
	inline INT Num() const
	{
		return Pairs.Num();
	}
	inline INT FindHash(INT iHash) const
	{
		return Hash ? Hash[iHash & (HashCount - 1)] : 0;
	}
	inline void Empty()
	{
		guardSlow(FScriptMapBase::Empty);
		Pairs.Empty();
		if (Hash)
			delete[] Hash;
		Hash = NULL;
		HashCount = 0;
		unguardSlow;
	}
};

template< class TK, class TI > class TScriptMap : protected FScriptMapBase
{
	friend class TIterator;
public:
	TScriptMap()
		: FScriptMapBase()
	{}
	TScriptMap(ENoInit)
		: FScriptMapBase(E_NoInit)
	{}
	~TScriptMap()
	{
		Empty();
	}
	TScriptMap& operator=(const TScriptMap& Other)
	{
		guardSlow(TScriptMap::operator=);
		Empty();
		for (INT i = 0; i < Other.Pairs.Num(); ++i)
		{
			const USPair& P = Other.Pairs(i);
			BYTE* KV = (BYTE*)appMalloc(sizeof(TK), TEXT("SKey"));
			BYTE* VV = (BYTE*)appMalloc(sizeof(TI), TEXT("SValue"));
			new (KV) TK(*((TK*)P.Key));
			new (VV) TI(*((TI*)P.Value));
			new(Pairs) USPair(KV, VV, P.HashValue);
		}
		RehashRecommended();
		return *this;
		unguardSlow;
	}
	inline void Empty()
	{
		guardSlow(TScriptMap::Empty);
		if (Pairs.Num())
		{
			for (INT i = 0; i < Pairs.Num(); i++)
			{
				((TK*)(Pairs(i).Key))->~TK();
				((TI*)(Pairs(i).Value))->~TI();
			}
			Pairs.Empty();
		}
		if (Hash)
			delete[] Hash;
		Hash = NULL;
		HashCount = 0;
		unguardSlow;
	}
	inline TI& Set(const TK& InKey, const TI& InValue)
	{
		guardSlow(TScriptMap::Set);
		INT iHash = GetTypeHash(InKey);
		if (Hash)
		{
			USPair* P = &Pairs(-1);
			for (INT i = Hash[iHash & (HashCount - 1)]; i; i = P[i].HashNext)
				if (*((TK*)P[i].Key) == InKey)
				{
					*((TI*)P[i].Value) = InValue;
					return *(TI*)P[i].Value;
				}
		}

		BYTE* KV = (BYTE*)appMalloc(sizeof(TK), TEXT("SKey"));
		BYTE* VV = (BYTE*)appMalloc(sizeof(TI), TEXT("SValue"));
		new (KV) TK(InKey);
		new (VV) TI(InValue);
		return *((TI*)Add(KV, VV, iHash));
		unguardSlow;
	}
	inline BYTE Remove(const TK& InKey)
	{
		guardSlow(TScriptMap::Remove);
		if (Hash)
		{
			USPair* P = &Pairs(-1);
			for (INT i = Hash[GetTypeHash(InKey) & (HashCount - 1)]; i; i = P[i].HashNext)
			{
				if (*((TK*)P[i].Key) == InKey)
				{
					((TK*)(Pairs(i).Key))->~TK();
					((TI*)(Pairs(i).Value))->~TI();
					Pairs.Remove(i);
					Relax();
					return 1;
				}
			}
		}
		return 0;
		unguardSlow;
	}
	inline TI* Find(const TK& Key)
	{
		guardSlow(TScriptMap::Find);
		if (Hash)
		{
			USPair* P = &Pairs(-1);
			for (INT i = Hash[GetTypeHash(Key) & (HashCount - 1)]; i; i = P[i].HashNext)
				if (*((TK*)P[i].Key) == Key)
					return (TI*)P[i].Value;
		}
		return NULL;
		unguardSlow;
	}
	inline const TI* Find(const TK& Key) const
	{
		guardSlow(TScriptMap::Find);
		if (Hash)
		{
			const USPair* P = &Pairs(-1);
			for (INT i = Hash[GetTypeHash(Key) & (HashCount - 1)]; i; i = P[i].HashNext)
				if (*((TK*)P[i].Key) == Key)
					return (TI*)P[i].Value;
		}
		return NULL;
		unguardSlow;
	}
	inline TI FindRef(const TK& Key)
	{
		guardSlow(TScriptMap::Find);
		if (Hash)
		{
			USPair* P = &Pairs(-1);
			for (INT i = Hash[GetTypeHash(Key) & (HashCount - 1)]; i; i = P[i].HashNext)
				if (*((TK*)P[i].Key) == Key)
					return *(TI*)P[i].Value;
		}
		return NULL;
		unguardSlow;
	}
	inline const TI FindRef(const TK& Key) const
	{
		guardSlow(TScriptMap::Find);
		if (Hash)
		{
			const USPair* P = &Pairs(-1);
			for (INT i = Hash[GetTypeHash(Key) & (HashCount - 1)]; i; i = P[i].HashNext)
				if (*((TK*)P[i].Key) == Key)
					return *(TI*)P[i].Value;
		}
		return NULL;
		unguardSlow;
	}
	friend FArchive& operator<<(FArchive& Ar, TScriptMap& M)
	{
		guardSlow(TScriptMap::Serialize);
		INT Count = M.Pairs.Num();
		Ar << AR_INDEX(Count);
		if (Ar.IsLoading())
		{
			TK* SerKey;
			TI* SerValue;
			if (M.Pairs.Num())
			{
				for (INT i = 0; i < M.Pairs.Num(); i++)
				{
					((TK*)(M.Pairs(i).Key))->~TK();
					((TI*)(M.Pairs(i).Value))->~TI();
				}
			}
			M.Pairs.Empty(Count);
			for (INT i = 0; i < Count; ++i)
			{
				SerKey = new TK;
				SerValue = new TI;
				Ar << *SerKey;
				Ar << *SerValue;
				new(M.Pairs) USPair((BYTE*)SerKey, (BYTE*)SerValue, GetTypeHash(*SerKey));
			}
			M.RehashRecommended();
		}
		else
		{
			for (INT i = 0; i < M.Pairs.Num(); ++i)
			{
				Ar << *((TK*)M.Pairs(i).Key);
				Ar << *((TI*)M.Pairs(i).Value);
			}
		}
		return Ar;
		unguardSlow;
	}
	inline INT Num() const
	{
		return Pairs.Num();
	}
	void Dump() const
	{
		INT i;
		debugf(TEXT("HashCount %i Pairs %i"), HashCount, Pairs.Num());
		for (i = 0; i < HashCount; ++i)
		{
			debugf(TEXT("Hash[%i] = %i"), i, Hash[i]);
		}
		for (i = 0; i < Pairs.Num(); ++i)
		{
			debugf(TEXT("Pairs[%i] = (Hash %i Next %i)"), i, Pairs(i).HashValue, Pairs(i).HashNext);
		}
	}

	class TIterator
	{
	public:
		TIterator(TScriptMap<TK, TI>& InMap) : Pairs(InMap.Pairs), Index(0) {}
		inline void operator++() { ++Index; }
		inline void RemoveCurrent() { Pairs.Remove(Index--); }
		inline operator UBOOL() const { return Index < Pairs.Num(); }
		inline TK& Key() const { return *((TK*)Pairs(Index).Key); }
		inline TI& Value() const { return *((TI*)Pairs(Index).Value); }
	private:
		TArray<USPair>& Pairs;
		INT Index;
	};
};

template< class TK, class TI > class TScriptMapNoInit : public TScriptMap<TK, TI>
{
public:
	TScriptMapNoInit()
		: TScriptMap<TK,TI>(E_NoInit)
	{}
};

//
// Maps unique keys to values.
//
template< class TK, class TI > class TMapBase
{
protected:
	class TPair
	{
	public:
		INT HashNext;
		TK Key;
		TI Value;
		TPair( typename TTypeInfo<TK>::ConstInitType InKey, typename TTypeInfo<TI>::ConstInitType InValue )
		: Key( InKey ), Value( InValue )
		{}
		TPair()
		{}
		friend FArchive& operator<<( FArchive& Ar, TPair& F )
		{
			guardSlow(TMapBase::TPair<<);
			return Ar << F.Key << F.Value;
			unguardSlow;
		}
	};
	void Rehash()
	{
		guardSlow(TMapBase::Rehash);
		checkSlow(!(HashCount&(HashCount-1)));
		checkSlow(HashCount>=8);
		INT* NewHash = new INT[HashCount];
		{for( INT i=0; i<HashCount; i++ )
		{
			NewHash[i] = INDEX_NONE;
		}}
		{for( INT i=0; i<Pairs.Num(); i++ )
		{
			TPair& Pair    = Pairs(i);
			INT    iHash   = (GetTypeHash(Pair.Key) & (HashCount-1));
			Pair.HashNext  = NewHash[iHash];
			NewHash[iHash] = i;
		}}
		if( Hash )
			delete[] Hash;
		Hash = NewHash;
		unguardSlow;
	}
	void Relax()
	{
		guardSlow(TMapBase::Relax);
		while( HashCount>Pairs.Num()*2+8 )
			HashCount /= 2;
		Rehash();
		unguardSlow;
	}
	TI& Add( typename TTypeInfo<TK>::ConstInitType InKey, typename TTypeInfo<TI>::ConstInitType InValue )
	{
		guardSlow(TMapBase::Add);
		TPair& Pair   = *new(Pairs)TPair( InKey, InValue );
		INT    iHash  = (GetTypeHash(Pair.Key) & (HashCount-1));
		Pair.HashNext = Hash[iHash];
		Hash[iHash]   = Pairs.Num()-1;
		if( HashCount*2+8 < Pairs.Num() )
		{
			HashCount *= 2;
			Rehash();
		}
		return Pair.Value;
		unguardSlow;
	}
	TArray<TPair> Pairs;
	INT* Hash;
	INT HashCount;
public:
	TMapBase()
	:	Hash( NULL )
	,	HashCount( 8 )
	{
		guardSlow(TMapBase::TMapBase);
		Rehash();
		unguardSlow;
	}
	TMapBase( const TMapBase& Other )
	:	Pairs( Other.Pairs )
	,	Hash( NULL )
	,	HashCount( Other.HashCount )
	{
		guardSlow(TMapBase::TMapBase copy);
		Rehash();
		unguardSlow;
	}
	~TMapBase()
	{
		guardSlow(TMapBase::~TMapBase);
		if( Hash )
			delete[] Hash;
		Hash = NULL;
		HashCount = 0;
		unguardSlow;
	}
	TMapBase& operator=( const TMapBase& Other )
	{
		guardSlow(TMapBase::operator=);
		Pairs     = Other.Pairs;
		HashCount = Other.HashCount;
		Rehash();
		return *this;
		unguardSlow;
	}
	void Empty()
	{
		guardSlow(TMapBase::Empty);
		checkSlow(!(HashCount&(HashCount-1)));
		Pairs.Empty();
		HashCount = 8;
		Rehash();
		unguardSlow;
	}
	TI& Set( typename TTypeInfo<TK>::ConstInitType InKey, typename TTypeInfo<TI>::ConstInitType InValue )
	{
		guardSlow(TMap::Set);
		for( INT i=Hash[(GetTypeHash(InKey) & (HashCount-1))]; i!=INDEX_NONE; i=Pairs(i).HashNext )
			if( Pairs(i).Key==InKey )
				{Pairs(i).Value=InValue; return Pairs(i).Value;}
		return Add( InKey, InValue );
		unguardSlow;
	}
	INT Remove( typename TTypeInfo<TK>::ConstInitType InKey )
	{
		guardSlow(TMapBase::Remove);
		INT Count=0;
		for( INT i=Pairs.Num()-1; i>=0; i-- )
			if( Pairs(i).Key==InKey )
				{Pairs.Remove(i); Count++;}
		if( Count )
			Relax();
		return Count;
		unguardSlow;
	}
	TI* Find( const TK& Key )
	{
		guardSlow(TMapBase::Find);
		for( INT i=Hash[(GetTypeHash(Key) & (HashCount-1))]; i!=INDEX_NONE; i=Pairs(i).HashNext )
			if( Pairs(i).Key==Key )
				return &Pairs(i).Value;
		return NULL;
		unguardSlow;
	}
	TI FindRef( const TK& Key )
	{
		guardSlow(TMapBase::Find);
		for( INT i=Hash[(GetTypeHash(Key) & (HashCount-1))]; i!=INDEX_NONE; i=Pairs(i).HashNext )
			if( Pairs(i).Key==Key )
				return Pairs(i).Value;
		return NULL;
		unguardSlow;
	}
	const TI FindRef(const TK& Key) const
	{
		guardSlow(TMapBase::Find);
		for (INT i = Hash[(GetTypeHash(Key) & (HashCount - 1))]; i != INDEX_NONE; i = Pairs(i).HashNext)
			if (Pairs(i).Key == Key)
				return Pairs(i).Value;
		return NULL;
		unguardSlow;
	}
	const TI* Find( const TK& Key ) const
	{
		guardSlow(TMapBase::Find);
		for( INT i=Hash[(GetTypeHash(Key) & (HashCount-1))]; i!=INDEX_NONE; i=Pairs(i).HashNext )
			if( Pairs(i).Key==Key )
				return &Pairs(i).Value;
		return NULL;
		unguardSlow;
	}
	friend FArchive& operator<<( FArchive& Ar, TMapBase& M )
	{
		guardSlow(TMapBase<<);
		Ar << M.Pairs;
		if (Ar.IsLoading())
		{
			if (M.Pairs.Num() > M.HashCount)
				M.HashCount = Min(1 << ((INT)appCeilLogTwo(M.Pairs.Num())), 64 * 1024);
			M.Rehash();
		}
		return Ar;
		unguardSlow;
	}
	void Dump( FOutputDevice& Ar )
	{
#if 1
		// Give some more useful statistics. --han
		INT NonEmpty = 0, Worst = 0;
		for( INT i=0; i<HashCount; i++ )
		{
			INT c=0;
			for( INT j=Hash[i]; j!=INDEX_NONE; j=Pairs(j).HashNext )
				c++;
			if ( c>Worst )
				Worst = c;
			if ( c>0 )
			{
				NonEmpty++;
				Ar.Logf( TEXT("   Hash[%i] = %i"), i, c );
			}
		}
		Ar.Logf( TEXT("TMapBase: %i items, worst %i, %i/%i hash slots used."), Pairs.Num(), Worst, NonEmpty, HashCount );
#else
		// oldver.
		Ar.Logf( TEXT("TMapBase: %i items, %i hash slots"), Pairs.Num(), HashCount );
		for( INT i=0; i<HashCount; i++ )
		{
			INT c=0;
			for( INT j=Hash[i]; j!=INDEX_NONE; j=Pairs(j).HashNext )
				c++;
			Ar.Logf( TEXT("   Hash[%i] = %i"), i, c );
		}
#endif
	}
	class TIterator
	{
	public:
		TIterator(TMapBase<TK, TI>& InMap) : MP(InMap), Index(0), bDeleted(FALSE) {}
		~TIterator() { if (bDeleted) MP.Relax(); }
		void operator++() { ++Index; }
		void Increment() { ++Index; }
		void RemoveCurrent() { MP.Pairs.Remove(Index--); bDeleted = TRUE; }
		operator UBOOL() const { return Index < MP.Pairs.Num(); }
		TK& Key() const { return MP.Pairs(Index).Key; }
		TI& Value() const { return MP.Pairs(Index).Value; }
	private:
		TMapBase<TK, TI>& MP;
		INT Index;
		UBOOL bDeleted;
		friend class TUniqueKeyIterator;
	};
	class TUniqueKeyIterator : public TMapBase<TK, TI>::TIterator
	{
	public:
		TUniqueKeyIterator(TMapBase<TK, TI>& InMap)
			: TMapBase<TK, TI>::TIterator(InMap)
		{
			if (*this)
				VisitedKeys.AddUniqueItem(TMapBase<TK, TI>::TIterator::Key());
		}
		void operator++()
		{
			while (1)
			{
				TMapBase<TK, TI>::TIterator::Increment();

				if (!*this)
					return;

				INT OldNum = VisitedKeys.Num();
				VisitedKeys.AddUniqueItem(TMapBase<TK, TI>::TIterator::Key());
				if (OldNum != VisitedKeys.Num())
					return;
			}
		}
	private:
		TArray<TK> VisitedKeys;
	};

	// Map iterator but doesn't allow to modify any elements.
	class TConstIterator
	{
	public:
		TConstIterator(const TMapBase<TK, TI>& InMap) : MP(InMap), Index(0) {}
		~TConstIterator() {}
		void operator++() { ++Index; }
		operator UBOOL() const { return Index < MP.Pairs.Num(); }
		const TK& Key() const { return MP.Pairs(Index).Key; }
		const TI& Value() const { return MP.Pairs(Index).Value; }
	private:
		const TMapBase<TK, TI>& MP;
		INT Index;
	};
	friend class TUniqueKeyIterator;
	friend class TIterator;
	friend class TConstIterator;
};
template< class TK, class TI > class TMap : public TMapBase<TK,TI>
{
public:
	TMap& operator=( const TMap& Other )
	{
		TMapBase<TK,TI>::operator=( Other );
		return *this;
	}

	int Num()
	{
		guardSlow(TMap::Num);
		return this->Pairs.Num();
		unguardSlow;
	}
};
template< class TK, class TI > class TMultiMap : public TMapBase<TK,TI>
{
public:
	TMultiMap& operator=( const TMultiMap& Other )
	{
		TMapBase<TK,TI>::operator=( Other );
		return *this;
	}

	int Num()
	{
		guardSlow(TMap::Num);
		return this->Pairs.Num();
		unguardSlow;
	}

	int Num(TArray<TK>& Keys)
	{
		guardSlow(TMultiMap::Num);
		Keys.Empty();

		for( INT i=0;i < this->Pairs.Num();i++ )
		{
			INT j=Keys.FindItemIndex(this->Pairs(i).Key);
			if (j==INDEX_NONE)
			{
				j=Keys.AddZeroed();
				Keys(j)=this->Pairs(i).Key;
			}
		}
		return Keys.Num();
		unguardSlow;
	}

	inline INT Count(const TK& Key)
	{
		guardSlow(TMultiMap::Count);
		INT Result = 0;
		for (INT i = this->Hash[(GetTypeHash(Key) & (this->HashCount - 1))]; i != INDEX_NONE; i = this->Pairs(i).HashNext)
			if (this->Pairs(i).Key == Key)
				++Result;
		return Result;
		unguardSlow;
	}

	void MultiFind( const TK& Key, TArray<TI>& Values )
	{
		guardSlow(TMap::MultiFind);
		for( INT i=this->Hash[(GetTypeHash(Key) & (this->HashCount-1))]; i!=INDEX_NONE; i=this->Pairs(i).HashNext )
			if( this->Pairs(i).Key==Key )
				new(Values)TI(this->Pairs(i).Value);
		unguardSlow;
	}
	TI& Add( typename TTypeInfo<TK>::ConstInitType InKey, typename TTypeInfo<TI>::ConstInitType InValue )
	{
		return TMapBase<TK,TI>::Add( InKey, InValue );
	}
	TI& AddUnique( typename TTypeInfo<TK>::ConstInitType InKey, typename TTypeInfo<TI>::ConstInitType InValue )
	{
		for( INT i=this->Hash[(GetTypeHash(InKey) & (this->HashCount-1))]; i!=INDEX_NONE; i=this->Pairs(i).HashNext )
			if( this->Pairs(i).Key==InKey && this->Pairs(i).Value==InValue )
				return this->Pairs(i).Value;
		return Add( InKey, InValue );
	}
	INT MultiRemove( const TK& InKey )
	{
		guardSlow(TMap::RemoveMulti);
		INT Count = 0;
		for ( INT i=this->Pairs.Num()-1; i>=0; i-- )
			if ( this->Pairs(i).Key == InKey )
				{this->Pairs.Remove(i); Count++;}
		if ( Count )
			this->Relax();
		return Count;
		unguardSlow;
	}
	INT RemovePair( typename TTypeInfo<TK>::ConstInitType InKey, typename TTypeInfo<TI>::ConstInitType InValue )
	{
		guardSlow(TMap::Remove);
		INT Count=0;
		for( INT i=this->Pairs.Num()-1; i>=0; i-- )
			if( this->Pairs(i).Key==InKey && this->Pairs(i).Value==InValue )
				{this->Pairs.Remove(i); Count++;}
		if( Count )
			this->Relax();
		return Count;
		unguardSlow;
	}
	TI* FindPair( const TK& Key, const TK& Value )
	{
		guardSlow(TMap::Find);
		for( INT i=this->Hash[(GetTypeHash(Key) & (this->HashCount-1))]; i!=INDEX_NONE; i=this->Pairs(i).HashNext )
			if( this->Pairs(i).Key==Key && this->Pairs(i).Value==Value )
				return &this->Pairs(i).Value;
		return NULL;
		unguardSlow;
	}
};

// Single key map, written by Marco
template< class TK > class TSingleMap
{
protected:
	class TValue
	{
	public:
		INT HashNext;
		TK Key;
		TValue(typename TTypeInfo<TK>::ConstInitType InKey)
			: Key(InKey)
		{}
		TValue()
		{}
		friend FArchive& operator<<(FArchive& Ar, TValue& F)
		{
			guardSlow(TSingleMap::TValue << );
			return Ar << F.Key;
			unguardSlow;
		}
	};
	void Rehash()
	{
		guardSlow(TSingleMap::Rehash);
		checkSlow(!(HashCount & (HashCount - 1)));
		checkSlow(HashCount >= 8);
		INT* NewHash = new INT[HashCount];
		{for (INT i = 0; i < HashCount; i++)
		{
			NewHash[i] = INDEX_NONE;
		}}
		{for (INT i = 0; i < Pairs.Num(); i++)
		{
			TValue& Pair = Pairs(i);
			INT    iHash = (GetTypeHash(Pair.Key) & (HashCount - 1));
			Pair.HashNext = NewHash[iHash];
			NewHash[iHash] = i;
		}}
		if (Hash)
			delete[] Hash;
		Hash = NewHash;
		unguardSlow;
	}
	void Relax()
	{
		guardSlow(TSingleMap::Relax);
		while (HashCount > Pairs.Num() * 2 + 8)
			HashCount /= 2;
		Rehash();
		unguardSlow;
	}
	void Add(typename TTypeInfo<TK>::ConstInitType InKey)
	{
		guardSlow(TSingleMap::Add);
		TValue& Pair = *new(Pairs)TValue(InKey);
		INT    iHash = (GetTypeHash(Pair.Key) & (HashCount - 1));
		Pair.HashNext = Hash[iHash];
		Hash[iHash] = Pairs.Num() - 1;
		if (HashCount * 2 + 8 < Pairs.Num())
		{
			HashCount *= 2;
			Rehash();
		}
		unguardSlow;
	}
	TArray<TValue> Pairs;
	INT* Hash;
	INT HashCount;
public:
	TSingleMap()
		: Hash(NULL)
		, HashCount(8)
	{
		guardSlow(TSingleMap::TMapBase);
		Rehash();
		unguardSlow;
	}
	TSingleMap(const TSingleMap& Other)
		: Pairs(Other.Pairs)
		, Hash(NULL)
		, HashCount(Other.HashCount)
	{
		guardSlow(TSingleMap::TMapBase copy);
		Rehash();
		unguardSlow;
	}
	~TSingleMap()
	{
		guardSlow(TSingleMap::~TMapBase);
		if (Hash)
			delete[] Hash;
		Hash = NULL;
		HashCount = 0;
		unguardSlow;
	}
	TSingleMap& operator=(const TSingleMap& Other)
	{
		guardSlow(TSingleMap::operator=);
		Pairs = Other.Pairs;
		HashCount = Other.HashCount;
		Rehash();
		return *this;
		unguardSlow;
	}
	void Empty()
	{
		guardSlow(TSingleMap::Empty);
		checkSlow(!(HashCount & (HashCount - 1)));
		Pairs.Empty();
		HashCount = 8;
		Rehash();
		unguardSlow;
	}
	UBOOL Set(typename TTypeInfo<TK>::ConstInitType InKey)
	{
		guardSlow(TSingleMap::Set);
		for (INT i = Hash[(GetTypeHash(InKey) & (HashCount - 1))]; i != INDEX_NONE; i = Pairs(i).HashNext)
			if (Pairs(i).Key == InKey)
				return 0;
		Add(InKey);
		return 1;
		unguardSlow;
	}
	UBOOL Remove(typename TTypeInfo<TK>::ConstInitType InKey)
	{
		guardSlow(TSingleMap::Remove);
		for (INT i = Hash[(GetTypeHash(InKey) & (HashCount - 1))]; i != INDEX_NONE; i = Pairs(i).HashNext)
			if (Pairs(i).Key == InKey)
			{
				Pairs.Remove(i);
				Relax();
				return 1;
			}
		return 0;
		unguardSlow;
	}
	UBOOL Find(const TK& Key) const
	{
		guardSlow(TSingleMap::Find);
		for (INT i = Hash[(GetTypeHash(Key) & (HashCount - 1))]; i != INDEX_NONE; i = Pairs(i).HashNext)
			if (Pairs(i).Key == Key)
				return 1;
		return 0;
		unguardSlow;
	}
	inline INT Num() const
	{
		guardSlow(TSingleMap::Remove);
		return Pairs.Num();
		unguardSlow;
	}
	friend FArchive& operator<<(FArchive& Ar, TSingleMap& M)
	{
		guardSlow(TSingleMap << );
		Ar << M.Pairs;
		if (Ar.IsLoading())
			M.Rehash();
		return Ar;
		unguardSlow;
	}
	class TIterator
	{
	public:
		TIterator(TSingleMap& InMap) : MP(InMap), Index(0), bDeleted(FALSE) {}
		~TIterator() { if (bDeleted) MP.Relax(); }
		void operator++() { ++Index; }
		void RemoveCurrent() { MP.Pairs.Remove(Index--); bDeleted = TRUE; }
		operator UBOOL() const { return Index < MP.Pairs.Num(); }
		TK& Key() const { return MP.Pairs(Index).Key; }
	private:
		TSingleMap& MP;
		INT Index;
		UBOOL bDeleted;
	};
	friend class TIterator;
};

/*----------------------------------------------------------------------------
	Sorting template.
----------------------------------------------------------------------------*/

//
// Sort elements. The sort is unstable, meaning that the ordering of equal
// items is not necessarily preserved.
//
template<class T> struct TStack
{
	T* Min;
	T* Max;
};
template<class T> void Sort( T* First, INT Num )
{
	guard(Sort);
	if( Num<2 )
		return;
	TStack<T> RecursionStack[32]={{First,First+Num-1}}, Current, Inner;
	for( TStack<T>* StackTop=RecursionStack; StackTop>=RecursionStack; --StackTop )
	{
		Current = *StackTop;
	Loop:
		INT Count = Current.Max - Current.Min + 1;
		if( Count <= 8 )
		{
			// Use simple bubble-sort.
			while( Current.Max > Current.Min )
			{
				T *Max, *Item;
				for( Max=Current.Min, Item=Current.Min+1; Item<=Current.Max; Item++ )
					if( Compare(*Item, *Max) > 0 )
						Max = Item;
				Exchange( *Max, *Current.Max-- );
			}
		}
		else
		{
			// Grab middle element so sort doesn't exhibit worst-cast behaviour with presorted lists.
			Exchange( Current.Min[Count/2], Current.Min[0] );

			// Divide list into two halves, one with items <=Current.Min, the other with items >Current.Max.
			Inner.Min = Current.Min;
			Inner.Max = Current.Max+1;
			for( ; ; )
			{
				while( ++Inner.Min<=Current.Max && Compare(*Inner.Min, *Current.Min) <= 0 );
				while( --Inner.Max> Current.Min && Compare(*Inner.Max, *Current.Min) >= 0 );
				if( Inner.Min>Inner.Max )
					break;
				Exchange( *Inner.Min, *Inner.Max );
			}
			Exchange( *Current.Min, *Inner.Max );

			// Save big half and recurse with small half.
			if( Inner.Max-1-Current.Min >= Current.Max-Inner.Min )
			{
				if( Current.Min+1 < Inner.Max )
				{
					StackTop->Min = Current.Min;
					StackTop->Max = Inner.Max - 1;
					StackTop++;
				}
				if( Current.Max>Inner.Min )
				{
					Current.Min = Inner.Min;
					goto Loop;
				}
			}
			else
			{
				if( Current.Max>Inner.Min )
				{
					StackTop->Min = Inner  .Min;
					StackTop->Max = Current.Max;
					StackTop++;
				}
				if( Current.Min+1<Inner.Max )
				{
					Current.Max = Inner.Max - 1;
					goto Loop;
				}
			}
		}
	}
	unguard;
}

template<class T> void Sort(TArray<T>& Array)
{
	if (Array.Num())
		Sort<T>(&Array(0), Array.Num());
}

/*----------------------------------------------------------------------------
	TDoubleLinkedList.
----------------------------------------------------------------------------*/

//
// Simple double-linked list template.
//
template< class T > class TDoubleLinkedList : public T
{
public:
	TDoubleLinkedList* Next;
	TDoubleLinkedList** PrevLink;
	void Unlink()
	{
		if( Next )
			Next->PrevLink = PrevLink;
		*PrevLink = Next;
	}
	void Link( TDoubleLinkedList*& Before )
	{
		if( Before )
			Before->PrevLink = &Next;
		Next     = Before;
		PrevLink = &Before;
		Before   = this;
	}
};

/*----------------------------------------------------------------------------
	TList.
----------------------------------------------------------------------------*/

//
// Simple single-linked list template.
//
template <class ElementType> class TList
{
public:

	ElementType			Element;
	TList<ElementType>*	Next;

	// Constructor.

	TList(ElementType InElement,TList<ElementType>* InNext = NULL)
	{
		Element = InElement;
		Next = InNext;
	}
};

/*----------------------------------------------------------------------------
	FRainbowPtr.
----------------------------------------------------------------------------*/

//
// A union of pointers of all base types.
//
union CORE_API FRainbowPtr
{
	// All pointers.
	void*  PtrVOID;
	BYTE*  PtrBYTE;
	_WORD* PtrWORD;
	DWORD* PtrDWORD;
	QWORD* PtrQWORD;
	FLOAT* PtrFLOAT;

	// Conversion constructors.
	FRainbowPtr() {}
	FRainbowPtr( void* Ptr ) : PtrVOID(Ptr) {};
};

/*
// char/wchar_t wrapper for Linux - Smirftsch

template<class cwc>
int wrap_vsprintf(cwc* buf, size_t len, const cwc* fmt, va_list args);

// char to vsnprintf
template<>
inline int wrap_vsprintf<char>(char* buf, size_t len, const char* fmt,
    va_list args) {
  return ::vsnprintf(buf, len, fmt, args);
}

// wchar_t to vswprintf
template<>
inline int wrap_vsprintf<wchar_t>(wchar_t* buf, size_t len, const wchar_t* fmt,
    va_list args) {
  return ::vswprintf(buf, len, fmt, args);
}
*/

/*----------------------------------------------------------------------------
	FAnyProperty.
----------------------------------------------------------------------------*/

// Remove windows def.
#ifdef GetObject
#undef GetObject
#endif

enum EAnyPropertyType
{
	ANYTYPE_NULL,
	ANYTYPE_INT,
	ANYTYPE_FLOAT,
	ANYTYPE_BOOL,
	ANYTYPE_STRING,
	ANYTYPE_NAME,
	ANYTYPE_STRUCT,
	ANYTYPE_OBJECT,
	ANYTYPE_MAX,
};
struct CORE_API FAnyProperty
{
private:
	BYTE* Data;
	class UStruct* StructType;
	INT ReferenceCount;
	BYTE DataType;

public:
	FAnyProperty();
	FAnyProperty(const FAnyProperty& A);
	~FAnyProperty();

	void DeleteValue();

	inline void Release()
	{
		if (--ReferenceCount == 0)
			delete this;
	}
	inline void AddRef()
	{
		++ReferenceCount;
	}

	inline UBOOL MatchesType(EAnyPropertyType PropType) const
	{
		return (DataType == PropType);
	}
	inline UBOOL MatchesStructType(UStruct* Struct) const
	{
		return (DataType == ANYTYPE_STRUCT && StructType == Struct);
	}

	// Get:
	inline BYTE GetType() const
	{
		return DataType;
	}
	inline UStruct* GetStructType() const
	{
		return StructType;
	}
	inline BYTE* GetRawData()
	{
		return Data;
	}
	inline const BYTE* GetRawData() const
	{
		return Data;
	}
	inline INT GetInt() const
	{
		return *(INT*)Data;
	}
	inline FLOAT GetFloat() const
	{
		return *(FLOAT*)Data;
	}
	inline UBOOL GetBool() const
	{
		return *(UBOOL*)Data;
	}
	inline const TCHAR* GetString() const
	{
		return **(FString*)Data;
	}
	inline const FString& GetFString() const
	{
		return *(FString*)Data;
	}
	inline class FName* GetName() const
	{
		return (FName*)Data;
	}
	inline class UObject* GetObject() const
	{
		return (UObject*)Data;
	}

	// Set:
	void SetInt(INT Value);
	void SetFloat(FLOAT Value);
	void SetBool(UBOOL Value);
	void SetString(const TCHAR* Value);
	void SetFString(const FString& Value);
	void SetName(FName* Value);
	void SetObject(UObject* Value);
	void SetStruct(BYTE* Value, UStruct* Struct);
	const TCHAR* GetTypeName();

	void ExportValueText(FString& Value);
	static void ExportValueTextEmpty(FString& Value);
	static const TCHAR* ImportValueText(FAnyProperty*& Ref, const TCHAR* Str);

	void Serialize(FArchive& Ar);
	static void SerializeEmpty(FArchive& Ar); // Save empty datatype.
	static UBOOL PeekIsEmpty(FArchive& Ar); // Check if empty datatype on load.

	static UBOOL NetSerialize(FAnyProperty*& Ref, FArchive& Ar, UPackageMap* Map);

	UBOOL CleanupDestroyed();
	UBOOL ClearNetReference(UObject* Other);
	void SerializeRefs(FArchive& Ar);
};


#endif // _INCL_UNTEMPLATE_H_

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
