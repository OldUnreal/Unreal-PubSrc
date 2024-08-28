/*=============================================================================
	FMallocDebug.h: Debug memory allocator.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

// Tags.
enum {MEM_PreTag =0xf0ed1cee};
enum {MEM_PostTag=0xdeadf00f};
enum {MEM_Tag    =0xfe      };
enum {MEM_WipeTag=0xcd      };

// Debug memory allocator.
class FMallocDebug : public FMalloc
{
private:
	// Structure for memory debugging.
	struct FMemDebugBase
	{
		TCHAR*		Tag;
		size_t		Size;
		INT			RefCount;
		INT			PreTag;
	};
	typedef TDoubleLinkedList<FMemDebugBase> FMemDebug;

	// Variables.
	FThreadLock MemMutex;
	FMemDebug* GFirstDebug;
	UBOOL MemInited;

	inline void* InternalMalloc(size_t Size, const TCHAR* Tag)
	{
		guard(InternalMalloc);
		STAT(++GMemStats.AllocCount.Count);
		checkSlow(MemInited);
		check((size_t)Size > 0);
		FMemDebug* Ptr = NULL;
		guard(CallMalloc);
		Ptr = (FMemDebug*)malloc(sizeof(FMemDebug) + Size + sizeof(size_t));
		check(Ptr);
		unguard;
		TCHAR* DupTag = NULL;
		guard(DupTag);
		DupTag = (TCHAR*)malloc((appStrlen(Tag) + 1) * sizeof(TCHAR));
		appStrcpy(DupTag, Tag);
		unguard;
		guard(SetPtr);
		Ptr->Tag = DupTag;
		Ptr->RefCount = 1;
		Ptr->Size = Size;
		Ptr->Next = GFirstDebug;
		Ptr->PrevLink = &GFirstDebug;
		Ptr->PreTag = MEM_PreTag;
		unguard;
		guard(SetPost);
		*(uintptr_t*)((BYTE*)Ptr + sizeof(FMemDebug) + Size) = MEM_PostTag;
		unguard;
		guard(FillMem);
		appMemset(Ptr + 1, MEM_Tag, Size);
		unguard;
		guard(DoPrevLink);
		if (GFirstDebug)
		{
			check(GIsCriticalError || GFirstDebug->PrevLink == &GFirstDebug);
			GFirstDebug->PrevLink = &Ptr->Next;
		}
		GFirstDebug = Ptr;
		unguard;
		return Ptr + 1;
		unguard;
	}
	inline void InternalFree(void* InPtr)
	{
		guard(InternalFree);
		checkSlow(MemInited);
		if (!InPtr)
			return;

		STAT(--GMemStats.AllocCount.Count);
		FMemDebug* Ptr = (FMemDebug*)InPtr - 1;
		check(GIsCriticalError || Ptr->Size > 0);
		check(GIsCriticalError || Ptr->RefCount == 1);
		check(GIsCriticalError || Ptr->PreTag == MEM_PreTag);
		check(GIsCriticalError || *(uintptr_t*)((BYTE*)InPtr + Ptr->Size) == MEM_PostTag);
		appMemset(InPtr, MEM_WipeTag, Ptr->Size);
		Ptr->Size = 0;
		Ptr->RefCount = 0;

		check(GIsCriticalError || Ptr->PrevLink);
		check(GIsCriticalError || *Ptr->PrevLink == Ptr);
		*Ptr->PrevLink = Ptr->Next;
		if (Ptr->Next)
			Ptr->Next->PrevLink = Ptr->PrevLink;

		free(Ptr->Tag);
		free(Ptr);
		unguard;
	}
public:
	// FMalloc interface.
	FMallocDebug()
	:	GFirstDebug	( NULL )
	,	MemInited	( 0 )
	{}
	void* Malloc(size_t Size, const TCHAR* Tag)
	{
		guard(FMallocDebug::Malloc);
		FScopeThread Scope(MemMutex);
		STAT(++GMemStats.AllocCounter.Calls);
		STAT(FStatTimerScope MScope(GMemStats.AllocTime));
		return InternalMalloc(Size, Tag);
		unguardf(( TEXT("%i %ls"), Size, Tag ));
	}
	void* Realloc(void* InPtr, size_t NewSize, const TCHAR* Tag)
	{
		guard(FMallocDebug::Realloc);
		FScopeThread Scope(MemMutex);
		STAT(++GMemStats.AllocCounter.Calls);
		STAT(FStatTimerScope MScope(GMemStats.AllocTime));
		checkSlow(MemInited);
		if( InPtr && NewSize )
		{
			check(GIsCriticalError||((FMemDebug*)InPtr-1)->RefCount==1);
			check(GIsCriticalError||((FMemDebug*)InPtr-1)->Size>0);
			//check(GIsCriticalError||((FMemDebug*)InPtr-1)->Size<=_msize((FMemDebug*)InPtr-1));
			void* Result = InternalMalloc( NewSize, Tag );
			appMemcpy( Result, InPtr, Min(((FMemDebug*)InPtr-1)->Size,NewSize) );
			InternalFree( InPtr );
			return Result;
		}
		else if( NewSize )
		{
			return InternalMalloc( NewSize, Tag );
		}
		else
		{
			if( InPtr )
				InternalFree( InPtr );
			return NULL;
		}
		unguardf((TEXT("%08X %i %ls"), (uintptr_t)InPtr, NewSize, Tag));
	}
	void Free( void* InPtr )
	{
		guard(FMallocDebug::Free);
		if (InPtr)
		{
			FScopeThread Scope(MemMutex);
			STAT(++GMemStats.AllocCounter.Calls);
			STAT(FStatTimerScope MScope(GMemStats.AllocTime));
			InternalFree(InPtr);
		}
		unguard;
	}
	void DumpAllocs()
	{
		guard(FMallocDebug::DumpAllocs);
		INT Count=0;
		INT Chunks=0;
		debugf( TEXT("Unfreed memory:") );
		for( FMemDebug* Ptr=GFirstDebug; Ptr; Ptr=Ptr->Next )
		{
			TCHAR Temp[256];
			appStrncpy(Temp, (TCHAR*)(Ptr + 1), Min((size_t)255, Ptr->Size));
			//debugf( TEXT("   % 10i %ls <%ls>"), Ptr->Size, Ptr->Tag, Temp );
			Count += Ptr->Size;
			Chunks++;
		}
		debugf( TEXT("End of list: %i Bytes still allocated"), Count );
		debugf( TEXT("             %i Chunks allocated"), Chunks );
		unguard;
	}
	void HeapCheck()
	{
		guard(FMallocDebug::HeapCheck);
		for( FMemDebug** Link = &GFirstDebug; *Link; Link=&(*Link)->Next )
			check(GIsCriticalError||*(*Link)->PrevLink==*Link);
		unguard;
	}
	void Init()
	{
		guard(FMallocDebug::Init);
		check(!MemInited);
		MemInited=1;
		unguard;
	}
	void Exit()
	{
		guard(FMallocDebug::Exit);
		check(MemInited);
		MemInited=0;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
