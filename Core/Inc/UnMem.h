/*=============================================================================
	UnMem.h: FMemStack class, ultra-fast temporary memory allocation
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Enums for specifying memory allocation type.
enum EMemZeroed {MEM_Zeroed=1};
enum EMemOned   {MEM_Oned  =1};

/*-----------------------------------------------------------------------------
	FMemStack.
-----------------------------------------------------------------------------*/

//
// Simple linear-allocation memory stack.
// Items are allocated via PushBytes() or the specialized operator new()s.
// Items are freed en masse by using FMemMark to Pop() them.
//
#if _DEBUG && DO_GUARD_SLOW && 0
class CORE_API FMemStack
{
public:
	// Get bytes.
	BYTE* PushBytes(INT AllocSize)
	{
		// Debug checks.
		guardSlow(FMemStack::PushBytes);

		if (AllocSize <= 0)
			AllocSize = 1;

		struct FTaggedMemory* Result = new FTaggedMemory;
		Result->Data = new BYTE[AllocSize];
		Result->Prev = TopChunk;
		Result->Size = AllocSize;
		Result->Next = NULL;
		TopChunk = Result;

		return Result->Data;
		unguardSlow;
	}

	// Main functions.
	void Init(INT DefaultChunkSize)
	{
		TopChunk = new FTaggedMemory;
		TopChunk->Data = NULL;
		TopChunk->Next = NULL;
		TopChunk->Prev = NULL;
		TopChunk->Size = 0;
	}

	void Exit()
	{
		while (TopChunk)
		{
			FTaggedMemory* Tmp = TopChunk;
			delete[] Tmp->Data;
			TopChunk = Tmp->Prev;
			delete Tmp;
		}
	}

	void Tick() {}
	INT GetByteCount()
	{
		INT Result = 0;
		for (FTaggedMemory* Tmp = TopChunk; Tmp; Tmp = Tmp->Prev)
		{
			Result += Tmp->Size;
		}
		return Result;
	}

	// Friends.
	friend class FMemMark;

	// Types.
	struct FTaggedMemory
	{
		FTaggedMemory* Next;
		FTaggedMemory* Prev;
		BYTE* Data;
		INT Size;
	};

private:
	// Constants.
	enum { MAX_CHUNKS = 1024 };

	// Variables.
	BYTE* Top;							// Not actually used in the debug implementation
	FTaggedMemory* TopChunk;			// Only chunks 0..ActiveChunks-1 are valid.

	// Functions.
	BYTE* AllocateNewChunk(INT MinSize)
	{
		return NULL;
	}

	void FreeChunks(FTaggedMemory* NewTopChunk)
	{
		while (TopChunk && TopChunk != NewTopChunk)
		{
			FTaggedMemory* Tmp = TopChunk;
			delete Tmp->Data;
			TopChunk = Tmp->Prev;
			delete Tmp;
		}
	}
};
#else
class CORE_API FMemStack
{
public:
	// Get bytes.
	FORCEINLINE BYTE* PushBytes( const INT AllocSize )
	{
		// Debug checks.
		guardSlow(FMemStack::PushBytes);

		checkSlow(AllocSize>=0);
		checkSlow(Top<=End);

		// Try to get memory from the current chunk.
#if USE_ALIGNMENT
		BYTE* Result = (BYTE*)(((size_t)Top + (OBJECT_ALIGNMENT - 1)) & ~(OBJECT_ALIGNMENT - 1));
#else
		BYTE* Result = Top;
#endif
		Top = Result + AllocSize;

		// Make sure we didn't overflow.
		if( Top > End )
		{
			// We'd pass the end of the current chunk, so allocate a new one.
#if USE_ALIGNMENT
			AllocateNewChunk(AllocSize + OBJECT_ALIGNMENT);
			Result = (BYTE*)(((size_t)Top + (OBJECT_ALIGNMENT - 1)) & ~(OBJECT_ALIGNMENT - 1));
#else
			AllocateNewChunk(AllocSize);
			Result = Top;
#endif
			Top    = Result + AllocSize;
		}
		return Result;
		unguardSlow;
	}

	// Main functions.
	void Init( INT DefaultChunkSize );
	void Exit();
	void Tick();
	INT GetByteCount();

	// Friends.
	friend class FMemMark;

	// Types.
	struct FTaggedMemory
	{
		FTaggedMemory* Next;
		INT DataSize;
		BYTE Data[1];
	};

private:
	// Constants.
	enum {MAX_CHUNKS=1024};

	// Variables.
	BYTE*			Top;				// Top of current chunk (Top<=End).
	BYTE*			End;				// End of current chunk.
	INT				DefaultChunkSize;	// Maximum chunk size to allocate.
	FTaggedMemory*	TopChunk;			// Only chunks 0..ActiveChunks-1 are valid.

	// Static.
	static FTaggedMemory* UnusedChunks;

	// Functions.
	BYTE* AllocateNewChunk( INT MinSize );
	void FreeChunks( FTaggedMemory* NewTopChunk );
};
#endif

/*-----------------------------------------------------------------------------
	FMemStack templates.
-----------------------------------------------------------------------------*/

// Operator new for typesafe memory stack allocation.
template <class T> inline T* New( FMemStack& Mem, const INT Count=1 )
{
	guardSlow(FMemStack::New);
	return (T*)Mem.PushBytes( Count*sizeof(T) );
	unguardSlow;
}
template <class T> inline T* NewZeroed( FMemStack& Mem, const INT Count=1 )
{
	guardSlow(FMemStack::New);
	BYTE* Result = Mem.PushBytes( Count*sizeof(T) );
	appMemzero( Result, Count*sizeof(T) );
	return (T*)Result;
	unguardSlow;
}
template <class T> inline T* NewOned( FMemStack& Mem, const INT Count=1 )
{
	guardSlow(FMemStack::New);
	BYTE* Result = Mem.PushBytes( Count*sizeof(T) );
	appMemset( Result, 0xff, Count*sizeof(T) );
	return (T*)Result;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FMemStack operator new's.
-----------------------------------------------------------------------------*/

// Operator new for typesafe memory stack allocation.
inline void* operator new( size_t Size, FMemStack& Mem, const INT Count=1 )
{
	// Get uninitialized memory.
	guardSlow(FMemStack::New1);
	return Mem.PushBytes( static_cast<INT>(Size)*Count);
	unguardSlow;
}
inline void* operator new( size_t Size, FMemStack& Mem, EMemZeroed Tag, const INT Count=1 )
{
	// Get zero-filled memory.
	guardSlow(FMemStack::New2);
	BYTE* Result = Mem.PushBytes(static_cast<INT>(Size)*Count);
	appMemzero( Result, static_cast<INT>(Size)*Count );
	return Result;
	unguardSlow;
}
inline void* operator new( size_t Size, FMemStack& Mem, EMemOned Tag, const INT Count=1 )
{
	// Get one-filled memory.
	guardSlow(FMemStack::New3);
	BYTE* Result = Mem.PushBytes(static_cast<INT>(Size)*Count);
	appMemset( Result, 0xff, static_cast<INT>(Size)*Count );
	return Result;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FMemMark.
-----------------------------------------------------------------------------*/

//
// FMemMark marks a top-of-stack position in the memory stack.
// When the marker is constructed or initialized with a particular memory
// stack, it saves the stack's current position. When marker is popped, it
// pops all items that were added to the stack subsequent to initialization.
//
class CORE_API FMemMark
{
public:
	// Constructors.
	FMemMark()
	{}
	FMemMark( FMemStack& InMem )
	{
		guardSlow(FMemMark::FMemMark);
		Mem          = &InMem;
		Top          = Mem->Top;
		SavedChunk   = Mem->TopChunk;
		unguardSlow;
	}

	// FMemMark interface.
	FORCEINLINE void Pop()
	{
		// Check state.
		guardSlow(FMemMark::Pop);

		// Unlock any new chunks that were allocated.
		if( SavedChunk != Mem->TopChunk )
			Mem->FreeChunks( SavedChunk );

		// Restore the memory stack's state.
		Mem->Top = Top;
		unguardSlow;
	}

private:
	// Implementation variables.
	FMemStack* Mem;
	BYTE* Top;
	FMemStack::FTaggedMemory* SavedChunk;
};

// Written by Marco, to let C++ call Pop when mem mark goes out of scope.
class FScopedMemMark : private FMemMark
{
	FScopedMemMark()
	{}
public:
	FScopedMemMark(FMemStack& InMem)
		: FMemMark(InMem)
	{}
	~FScopedMemMark()
	{
		Pop();
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
