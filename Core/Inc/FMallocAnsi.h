/*=============================================================================
	FMallocAnsi.h: ANSI memory allocator.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if __LINUX_X86__ || __LINUX_ARM__
# include <malloc.h> // for memalign
#endif

//
// ANSI C memory allocator.
//
class FMallocAnsi : public FMalloc
{
public:
	// FMalloc interface.
	void* Malloc( size_t Size, const TCHAR* Tag )
	{
		guard(FMallocAnsi::Malloc);
		check(Size>0);
#if _MSC_VER
		void* Ptr = _aligned_malloc( Size, VECTOR_ALIGNMENT );
#elif MACOSX
		// stijn: macOS guarantees 16-byte alignment for malloc
		checkSlow(VECTOR_ALIGNMENT <= 16);
		void* Ptr = malloc(Size);
#else
		void* Ptr = memalign ( VECTOR_ALIGNMENT, Size );
#endif
		check(Ptr);
		return Ptr;
		unguard;
	}
	void* Realloc( void* Ptr, size_t NewSize, const TCHAR* Tag )
	{
		guard(FMallocAnsi::Realloc);
		check(NewSize>=0);
		void* Result;
		if( Ptr && NewSize )
		{			
#if _MSC_VER
			Result = _aligned_realloc(Ptr, NewSize, VECTOR_ALIGNMENT);
#elif MACOSX
			Result = realloc(Ptr, NewSize);
#else
            // stijn: Neither C11 nor POSIX support aligned realloc
			Result = realloc( Ptr, NewSize );

			if (reinterpret_cast<PTRINT>(Result) & (VECTOR_ALIGNMENT-1))
			{
				void* Tmp = memalign(VECTOR_ALIGNMENT, NewSize);
				// copy over the old contents
				if (Tmp && Result)
				{
					memcpy(Tmp, Result, NewSize);
					free(Result);
					Result = Tmp;
				}
			}
#endif
		}
		else if( NewSize )
		{
#if _MSC_VER
			Result = _aligned_malloc(NewSize, VECTOR_ALIGNMENT);
#elif MACOSX
			Result = malloc(NewSize);
#else
			Result = memalign( VECTOR_ALIGNMENT, NewSize );
#endif
		}
		else
		{
			if( Ptr )
			{
#if _MSC_VER
				_aligned_free(Ptr);
#else
				free(Ptr);
#endif
			}
			Result = NULL;
		}
		return Result;
		unguardf(( TEXT("%08X %i %ls"), reinterpret_cast<PTRINT>(Ptr), NewSize, Tag ));
	}
	void Free( void* Ptr )
	{
		guard(FMallocAnsi::Free);
#if _MSC_VER
		_aligned_free(Ptr);
#else
		free(Ptr);
#endif
		unguard;
	}
	void DumpAllocs()
	{
		guard(FMallocAnsi::DumpAllocs);
		debugf( NAME_Exit, TEXT("Allocation checking disabled") );
		unguard;
	}
	void HeapCheck()
	{
		guard(FMallocAnsi::HeapCheck);
#if _MSC_VER
		INT Result = _heapchk();
		check(Result!=_HEAPBADBEGIN);
		check(Result!=_HEAPBADNODE);
		check(Result!=_HEAPBADPTR);
		check(Result!=_HEAPEMPTY);
		check(Result==_HEAPOK);
#endif
		unguard;
	}
	void Init()
	{
		guard(FMallocAnsi::Init);
		unguard;
	}
	void Exit()
	{
		guard(FMallocAnsi::Exit);
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
