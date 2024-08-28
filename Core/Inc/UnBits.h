/*=============================================================================
	UnBits.h: Unreal bitstream manipulation classes.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	FBitWriter.
-----------------------------------------------------------------------------*/

//
// Writes bitstreams.
//
struct CORE_API FBitWriter : public FArchive
{
	friend struct FBitWriterMark;
public:
	FBitWriter( INT InMaxBits, UBOOL bUTPack=FALSE );
	void SerializeBits( void* Src, INT LengthBits );
	void SerializeInt( DWORD& Value, DWORD Max );
	void WriteInt( DWORD Result, DWORD Max );
	void WriteBit( BYTE In );
	void Serialize( void* Src, INT LengthBytes );
	BYTE* GetData();
	INT GetNumBytes();
	INT GetNumBits();
	void SetOverflowed();

	INT Tell()
	{
		return GetNumBits();
	}
protected:
	TArray<BYTE> Buffer;
	INT   Num;
	INT   Max;
	UBOOL bUTPacking;
};

//
// For pushing and popping FBitWriter positions.
//
struct CORE_API FBitWriterMark
{
public:
	FBitWriterMark()
	:	Num         ( 0 )
	{}
	FBitWriterMark( FBitWriter& Writer )
	:	Overflowed	( Writer.ArIsError )
	,	Num			( Writer.Num )
	{}
	INT GetNumBits()
	{
		return Num;
	}
	void Pop( FBitWriter& Writer );
private:
	UBOOL			Overflowed;
	INT				Num;
};

/*-----------------------------------------------------------------------------
	FBitReader.
-----------------------------------------------------------------------------*/

//
// Reads bitstreams.
//
struct CORE_API FBitReader : public FArchive
{
	friend struct FBitReaderMark;
public:
	FBitReader( BYTE* Src=NULL, INT CountBits=0, UBOOL bUTPack=FALSE );
	void SetData( FBitReader& Src, INT CountBits );
	void SerializeBits( void* Dest, INT LengthBits );
	void SerializeInt( DWORD& Value, DWORD Max );
	DWORD ReadInt( DWORD Max );
	BYTE ReadBit();
	void Serialize( void* Dest, INT LengthBytes );
	BYTE* GetData();
	UBOOL AtEnd();
	void SetOverflowed();
	INT GetNumBytes();
	INT GetNumBits();
	INT GetPosBits();

	INT Tell()
	{
		return GetPosBits();
	}

	DWORD ReadBits(INT NumBits)
	{
		guardSlow(FBitReader::ReadBits);
		checkSlow(NumBits >= 0);
		checkSlow(NumBits <= 32);

		DWORD Result = 0;
		for (INT Bit = 0; Bit < NumBits; Bit++)
			Result |= DWORD(ReadBit()) << Bit;

		return Result;
		unguardSlow;
	}

private:
	TArray<BYTE> Buffer;
	INT   Num;
	INT   Pos;
	UBOOL bUTPacking;
};

//
// For pushing and popping FBitReader positions.
//
struct FBitReaderMark
{
public:
	FBitReaderMark() = delete;
	FBitReaderMark(FBitReader& Reader)
		: Pos(Reader.Pos)
	{}
	inline void Pop(FBitReader& Reader) const
	{
		Reader.ArIsError = (Pos <= Reader.Num);
		Reader.Pos = Pos;
	}
private:
	INT				Pos;
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
