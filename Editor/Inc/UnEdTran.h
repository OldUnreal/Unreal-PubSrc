/*=============================================================================
	UnEdTran.h: Unreal transaction tracking system
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	UTransactor.
-----------------------------------------------------------------------------*/

//
// Object responsible for tracking transactions for undo/redo.
//
class EDITOR_API UTransactor : public UObject
{
	DECLARE_ABSTRACT_CLASS(UTransactor,UObject,CLASS_Transient,Editor)

	// UTransactor interface.
	virtual void Reset( const TCHAR* Action )=0;
	virtual void Begin( const TCHAR* SessionName )=0;
	virtual void End()=0;
	virtual void Continue()=0;
	virtual UBOOL bActionsDone() = 0;
	virtual UBOOL CanUndo( FString* Str=NULL )=0;
	virtual UBOOL CanRedo( FString* Str=NULL )=0;
	virtual UBOOL Undo()=0;
	virtual UBOOL Redo()=0;
	virtual FTransactionBase* CreateInternalTransaction()=0;
};

/*-----------------------------------------------------------------------------
	A single transaction.
-----------------------------------------------------------------------------*/

//
// A single transaction, representing a set of serialized, undoable changes to a set of objects.
//
//warning: The undo buffer cannot be made persistent because of its dependence on offsets
// of arrays from their owning UObjects.
//
//warning: Transactions which rely on Preload calls cannot be garbage collected
// since references to objects point to the most recent version of the object, not
// the ordinally correct version which was referred to at the time of serialization.
// Therefore, Preload-sensitive transactions may only be performed using
// a temporary UTransactor::CreateInternalTransaction transaction, not a
// garbage-collectable UTransactor::Begin transaction.
//
//warning: UObject::Serialize implicitly assumes that class properties do not change
// in between transaction resets.
//
class FTransaction : public FTransactionBase
{
public:
	// Record of an object.
	class FObjectRecord
	{
	public:
		// Variables.
		TArray<BYTE>	Data;
		UObject*		Object;
		FArray*			Array;
		INT				Index;
		INT				Count;
		INT				Oper;
		INT				ElementSize;
		STRUCT_AR		Serializer;
		STRUCT_DTOR		Destructor;
		UBOOL			Restored;

		// Constructors.
		FObjectRecord();
		FObjectRecord( FTransaction* Owner, UObject* InObject, FArray* InArray, INT InIndex, INT InCount, INT InOper, INT InElementSize, STRUCT_AR InSerializer, STRUCT_DTOR InDestructor );
		// Functions.
		void SerializeContents( FArchive& Ar, INT InOper );
		void Restore( FTransaction* Owner );
		friend FArchive& operator<<( FArchive& Ar, FObjectRecord& R );

		// Transfers data from an array.
		class FReader : public FArchive
		{
		public:
			FReader( FTransaction* InOwner, TArray<BYTE>& InBytes );
		private:
			void Serialize( void* Data, INT Num );
			FArchive& operator<<( class FName& N );
			FArchive& operator<<( class UObject*& Res );
			void Preload( UObject* Object );
			
			FTransaction* Owner;
			TArray<BYTE>& Bytes;
			INT Offset;
		};

		// Transfers data to an array.
		class FWriter : public FArchive
		{
		public:
			FWriter( TArray<BYTE>& InBytes );
		private:
			void Serialize( void* Data, INT Num );
			FArchive& operator<<( class FName& N );
			FArchive& operator<<( class UObject*& Res );
			TArray<BYTE>& Bytes;
		};
	};

	// Transaction variables.
	TArray<FObjectRecord>	Records;
	FString					Title;
	UBOOL					Flip;
	INT						Inc;

	// Constructor.
	FTransaction( const TCHAR* InTitle=NULL, UBOOL InFlip=0 );

	// FTransactionBase interface.
	void SaveObject( UObject* Object );
	void SaveArray( UObject* Object, FArray* Array, INT Index, INT Count, INT Oper, INT ElementSize, STRUCT_AR Serializer, STRUCT_DTOR Destructor );
	void Apply();
	
	// FTransaction interface.
	size_t DataSize();
	const TCHAR* GetTitle();
	friend FArchive& operator<<( FArchive& Ar, FTransaction& T );

	// Transaction friends.
	friend class FObjectRecord;
	friend class FObjectRecord::FReader;
	friend class FObjectRecord::FWriter;
};

/*-----------------------------------------------------------------------------
	Transaction tracking system.
-----------------------------------------------------------------------------*/

//
// Transaction tracking system, manages the undo and redo buffer.
//
class EDITOR_API UTransBuffer : public UTransactor
{
	DECLARE_CLASS(UTransBuffer,UObject,CLASS_Transient,Editor)
	NO_DEFAULT_CONSTRUCTOR(UTransBuffer)

	// Variables.
	TArray<FTransaction>	UndoBuffer;
	INT						UndoCount;
	FString					ResetReason;
	INT						ActiveCount;
	size_t					MaxMemory;
	UBOOL					Overflow;
	INT						ResetCount;

	// Constructor.
	UTransBuffer( size_t InMaxMemory );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	// UTransactor interface.
	void Reset( const TCHAR* Reason );
	void Begin( const TCHAR* SessionName );
	void End();
	void Continue();
	UBOOL bActionsDone();
	UBOOL CanUndo( FString* Str=NULL );
	UBOOL CanRedo( FString* Str=NULL );
	UBOOL Undo();
	UBOOL Redo();
	FTransactionBase* CreateInternalTransaction();

	// Functions.
	void FinishDo();
	size_t UndoDataSize();
	void CheckState();
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
