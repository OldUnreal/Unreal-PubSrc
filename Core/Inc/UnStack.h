/*=============================================================================
	UnStack.h: UnrealScript execution stack definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

class UStruct;

/*-----------------------------------------------------------------------------
	Constants & types.
-----------------------------------------------------------------------------*/

// Sizes.
enum {MAX_STRING_CONST_SIZE		= 256               };
enum {MAX_CONST_SIZE			= 16 *sizeof(TCHAR) };
enum {MAX_FUNC_PARMS			= 16                };

//
// UnrealScript intrinsic return value declaration.
//
#define RESULT_DECL void*const Result
#define RESULT_GET(ptype) (*reinterpret_cast<ptype*>(Result))
#define HAS_RESULT (Result)
#define RESULT_SET_COND(ptype, eval) if(HAS_RESULT) {RESULT_GET(ptype) = (eval);} else {(eval);}
#define RESULT_SET(ptype, val) if(HAS_RESULT) RESULT_GET(ptype) = val;

//
// guardexec mechanism for script debugging.
//
#define unguardexecSlow  unguardfSlow(( TEXT("(%ls @ %ls : %04X)"), Stack.Object->GetFullName(), Stack.Node->GetFullName(), Stack.Code - &Stack.Node->Script(0) ))
#define unguardexec      unguardf    (( TEXT("(%ls @ %ls : %04X)"), Stack.Object->GetFullName(), Stack.Node->GetFullName(), Stack.Code - &Stack.Node->Script(0) ))

//
// State flags.
//
enum EStateFlags
{
	// State flags.
	STATE_Editable		= 0x00000001,	// State should be user-selectable in UnrealEd.
	STATE_Auto			= 0x00000002,	// State is automatic (the default state).
	STATE_Simulated     = 0x00000004,   // State executes on client side.
};

//
// Function flags.
//
enum EFunctionFlags
{
	// Function flags.
	FUNC_Final			= 0x00000001,	// Function is final (prebindable, non-overridable function).
	FUNC_Defined		= 0x00000002,	// Function has been defined (not just declared).
	FUNC_Iterator		= 0x00000004,	// Function is an iterator.
	FUNC_Latent		    = 0x00000008,	// Function is a latent state function.
	FUNC_PreOperator	= 0x00000010,	// Unary operator is a prefix operator.
	FUNC_Singular       = 0x00000020,   // Function cannot be reentered.
	FUNC_Net            = 0x00000040,   // Function is network-replicated.
	FUNC_NetReliable    = 0x00000080,   // Function should be sent reliably on the network.
	FUNC_Simulated		= 0x00000100,	// Function executed on the client side.
	FUNC_Exec		    = 0x00000200,	// Executable from command line.
	FUNC_Native			= 0x00000400,	// Native function.
	FUNC_Event          = 0x00000800,   // Event function.
	FUNC_Operator       = 0x00001000,   // Operator function.
	FUNC_Static         = 0x00002000,   // Static function.
	FUNC_NoExport       = 0x00004000,   // Don't export intrinsic function to C++.
	FUNC_Const          = 0x00008000,   // Function doesn't modify this object.
	FUNC_Invariant      = 0x00010000,   // Return value is purely dependent on parameters; no state dependencies or internal state changes.

	// Combinations of flags.
	FUNC_FuncInherit        = FUNC_Exec | FUNC_Event,
	FUNC_FuncOverrideMatch	= FUNC_Exec | FUNC_Final | FUNC_Latent | FUNC_PreOperator | FUNC_Iterator | FUNC_Static,
	FUNC_NetFuncFlags       = FUNC_Net | FUNC_NetReliable,
};

//
// Evaluatable expression item types.
//
enum EExprToken
{
	// Variable references.
	EX_LocalVariable		= 0x00,	// A local variable.
	EX_InstanceVariable		= 0x01,	// An object variable.
	EX_DefaultVariable		= 0x02,	// Default variable for a concrete object.

	// Tokens.
	EX_Return				= 0x04,	// Return from function.
	EX_Switch				= 0x05,	// Switch.
	EX_Jump					= 0x06,	// Goto a local address in code.
	EX_JumpIfNot			= 0x07,	// Goto if not expression.
	EX_Stop					= 0x08,	// Stop executing state code.
	EX_Assert				= 0x09,	// Assertion.
	EX_Case					= 0x0A,	// Case.
	EX_Nothing				= 0x0B,	// No operation.
	EX_LabelTable			= 0x0C,	// Table of labels.
	EX_GotoLabel			= 0x0D,	// Goto a label.
	EX_EatString            = 0x0E, // Ignore a dynamic string.
	EX_Let					= 0x0F,	// Assign an arbitrary size value to a variable.
	EX_DynArrayElement      = 0x10, // Dynamic array element.!!
	EX_New                  = 0x11, // New object allocation.
	EX_ClassContext         = 0x12, // Class default metaobject context.
	EX_MetaCast             = 0x13, // Metaclass cast.
	EX_LetBool				= 0x14, // Let boolean variable.
	EX_MapKeyElement		= 0x15,	// Map key element.!!
	//
	EX_EndFunctionParms		= 0x16,	// End of function call parameters.
	EX_Self					= 0x17,	// Self object.
	EX_Skip					= 0x18,	// Skippable expression.
	EX_Context				= 0x19,	// Call a function through an object context.
	EX_ArrayElement			= 0x1A,	// Array element.
	EX_VirtualFunction		= 0x1B,	// A function call with parameters.
	EX_FinalFunction		= 0x1C,	// A prebound function call with parameters.
	EX_IntConst				= 0x1D,	// Int constant.
	EX_FloatConst			= 0x1E,	// Floating point constant.
	EX_StringConst			= 0x1F,	// String constant.
	EX_ObjectConst		    = 0x20,	// An object constant.
	EX_NameConst			= 0x21,	// A name constant.
	EX_RotationConst		= 0x22,	// A rotation constant.
	EX_VectorConst			= 0x23,	// A vector constant.
	EX_ByteConst			= 0x24,	// A byte constant.
	EX_IntZero				= 0x25,	// Zero.
	EX_IntOne				= 0x26,	// One.
	EX_True					= 0x27,	// Bool True.
	EX_False				= 0x28,	// Bool False.
	EX_NativeParm           = 0x29, // Native function parameter offset.
	EX_NoObject				= 0x2A,	// NoObject.
	EX_IntConstByte			= 0x2C,	// Int constant that requires 1 byte.
	EX_BoolVariable			= 0x2D,	// A bool variable which requires a bitmask.
	EX_DynamicCast			= 0x2E,	// Safe dynamic class casting.
	EX_Iterator             = 0x2F, // Begin an iterator operation.
	EX_IteratorPop          = 0x30, // Pop an iterator level.
	EX_IteratorNext         = 0x31, // Go to next iteration.
	EX_StructCmpEq          = 0x32,	// Struct binary compare-for-equal.
	EX_StructCmpNe          = 0x33,	// Struct binary compare-for-unequal.
	EX_UnicodeStringConst   = 0x34, // Unicode string constant.
	EX_EatCtrExpr           = 0x35, // Ignore a constructor expression.
	//
	EX_StructMember         = 0x36, // Struct member.
	EX_StructConstruct      = 0x37, // Struct constructor.
	//
	EX_GlobalFunction		= 0x38, // Call non-state version of a function.

	// Native conversions.
	EX_MinConversion		= 0x39,	// Minimum conversion token.
	EX_RotatorToVector		= 0x39,
	EX_ByteToInt			= 0x3A,
	EX_ByteToBool			= 0x3B,
	EX_ByteToFloat			= 0x3C,
	EX_IntToByte			= 0x3D,
	EX_IntToBool			= 0x3E,
	EX_IntToFloat			= 0x3F,
	EX_BoolToByte			= 0x40,
	EX_BoolToInt			= 0x41,
	EX_BoolToFloat			= 0x42,
	EX_FloatToByte			= 0x43,
	EX_FloatToInt			= 0x44,
	EX_FloatToBool			= 0x45,
	//
	EX_ObjectToBool			= 0x47,
	EX_NameToBool			= 0x48,
	EX_StringToByte			= 0x49,
	EX_StringToInt			= 0x4A,
	EX_StringToBool			= 0x4B,
	EX_StringToFloat		= 0x4C,
	EX_StringToVector		= 0x4D,
	EX_StringToRotator		= 0x4E,
	EX_VectorToBool			= 0x4F,
	EX_VectorToRotator		= 0x50,
	EX_RotatorToBool		= 0x51,
	EX_ByteToString			= 0x52,
	EX_IntToString			= 0x53,
	EX_BoolToString			= 0x54,
	EX_FloatToString		= 0x55,
	EX_ObjectToString		= 0x56,
	EX_NameToString			= 0x57,
	EX_VectorToString		= 0x58,
	EX_RotatorToString		= 0x59,
	EX_MaxConversion		= 0x60,	// Maximum conversion token.

	// Natives.
	EX_ExtendedNative		= 0x60,
	EX_FirstNative			= 0x70,
	EX_MallocConstRef		= 0xD1,
	EX_RefIsValid			= 0x0191,
	EX_ArrayOperation		= 0x01F3,
	EX_ArrayIterator		= 0x0203,
	EX_MapIterator			= 0x0204,
	EX_MapHasValue			= 0x0201,
	EX_MapRemoveValue		= 0x0217,
	EX_MapEmptyValue		= 0x021A,
	EX_AnyOperator			= 0x021D,
	EX_TeriaryCondition		= 0x021E, // Evaluate variable.
	EX_DeleteObject			= 0x0288,
	EX_Max					= 0x1000,
};

// Dynamic array operations.
enum ArOpCode
{
	AROP_AddValue,
	AROP_AddUnique,
	AROP_RemoveValue,
	AROP_FindValue,
};

// Any property operations.
enum AnyOpCode
{
	ANYFUNC_GetInt,
	ANYFUNC_GetFloat,
	ANYFUNC_GetBool,
	ANYFUNC_GetString,
	ANYFUNC_GetName,
	ANYFUNC_GetStruct,
	ANYFUNC_GetObject,
	ANYFUNC_SetInt,
	ANYFUNC_SetFloat,
	ANYFUNC_SetBool,
	ANYFUNC_SetString,
	ANYFUNC_SetName,
	ANYFUNC_SetStruct,
	ANYFUNC_SetObject,
	ANYFUNC_Clone,
	ANYFUNC_Copy,
	ANYFUNC_Delete,
	ANYFUNC_GetType,
	ANYFUNC_GetStructType,
	ANYFUNC_IsType,
	ANYFUNC_GetText,
	ANYFUNC_SetText,
};

//
// Latent functions.
//
enum EPollSlowFuncs
{
	EPOLL_Sleep			      = 384,
	EPOLL_FinishAnim	      = 385,
	EPOLL_FinishInterpolation = 302,
};

#define CASE_EXP(cs) case cs: return TEXT(#cs)

inline const TCHAR* GetExprName(BYTE Code)
{
	switch (Code)
	{
		CASE_EXP(EX_LocalVariable);
		CASE_EXP(EX_InstanceVariable);
		CASE_EXP(EX_DefaultVariable);
		CASE_EXP(EX_Return);
		CASE_EXP(EX_Switch);
		CASE_EXP(EX_Jump);
		CASE_EXP(EX_JumpIfNot);
		CASE_EXP(EX_Stop);
		CASE_EXP(EX_Assert);
		CASE_EXP(EX_Case);
		CASE_EXP(EX_Nothing);
		CASE_EXP(EX_LabelTable);
		CASE_EXP(EX_GotoLabel);
		CASE_EXP(EX_EatString);
		CASE_EXP(EX_Let);
		CASE_EXP(EX_DynArrayElement);
		CASE_EXP(EX_MapKeyElement);
		CASE_EXP(EX_New);
		CASE_EXP(EX_ClassContext);
		CASE_EXP(EX_MetaCast);
		CASE_EXP(EX_LetBool);
		CASE_EXP(EX_EndFunctionParms);
		CASE_EXP(EX_Self);
		CASE_EXP(EX_Skip);
		CASE_EXP(EX_Context);
		CASE_EXP(EX_ArrayElement);
		CASE_EXP(EX_VirtualFunction);
		CASE_EXP(EX_FinalFunction);
		CASE_EXP(EX_IntConst);
		CASE_EXP(EX_FloatConst);
		CASE_EXP(EX_StringConst);
		CASE_EXP(EX_ObjectConst);
		CASE_EXP(EX_NameConst);
		CASE_EXP(EX_RotationConst);
		CASE_EXP(EX_VectorConst);
		CASE_EXP(EX_ByteConst);
		CASE_EXP(EX_IntZero);
		CASE_EXP(EX_IntOne);
		CASE_EXP(EX_True);
		CASE_EXP(EX_False);
		CASE_EXP(EX_NativeParm);
		CASE_EXP(EX_NoObject);
		CASE_EXP(EX_IntConstByte);
		CASE_EXP(EX_BoolVariable);
		CASE_EXP(EX_DynamicCast);
		CASE_EXP(EX_Iterator);
		CASE_EXP(EX_IteratorPop);
		CASE_EXP(EX_IteratorNext);
		CASE_EXP(EX_StructCmpEq);
		CASE_EXP(EX_StructCmpNe);
		CASE_EXP(EX_UnicodeStringConst);
		CASE_EXP(EX_EatCtrExpr);
		CASE_EXP(EX_StructMember);
		CASE_EXP(EX_StructConstruct);
		CASE_EXP(EX_GlobalFunction);
		CASE_EXP(EX_RotatorToVector);
		CASE_EXP(EX_ByteToInt);
		CASE_EXP(EX_ByteToBool);
		CASE_EXP(EX_ByteToFloat);
		CASE_EXP(EX_IntToByte);
		CASE_EXP(EX_IntToBool);
		CASE_EXP(EX_IntToFloat);
		CASE_EXP(EX_BoolToByte);
		CASE_EXP(EX_BoolToInt);
		CASE_EXP(EX_BoolToFloat);
		CASE_EXP(EX_FloatToByte);
		CASE_EXP(EX_FloatToInt);
		CASE_EXP(EX_FloatToBool);
		CASE_EXP(EX_ObjectToBool);
		CASE_EXP(EX_NameToBool);
		CASE_EXP(EX_StringToByte);
		CASE_EXP(EX_StringToInt);
		CASE_EXP(EX_StringToBool);
		CASE_EXP(EX_StringToFloat);
		CASE_EXP(EX_StringToVector);
		CASE_EXP(EX_StringToRotator);
		CASE_EXP(EX_VectorToBool);
		CASE_EXP(EX_VectorToRotator);
		CASE_EXP(EX_RotatorToBool);
		CASE_EXP(EX_ByteToString);
		CASE_EXP(EX_IntToString);
		CASE_EXP(EX_BoolToString);
		CASE_EXP(EX_FloatToString);
		CASE_EXP(EX_ObjectToString);
		CASE_EXP(EX_NameToString);
		CASE_EXP(EX_VectorToString);
		CASE_EXP(EX_RotatorToString);
		CASE_EXP(EX_ExtendedNative);
		CASE_EXP(EX_MallocConstRef);
	default:
		static TCHAR MiscToken[3];
		appSprintf(MiscToken, TEXT("%02X"), Code);
		return MiscToken;
	}
}
#undef CASE_EXP

/*-----------------------------------------------------------------------------
	Execution stack helpers.
-----------------------------------------------------------------------------*/

// Information about an const out parameter that needs to be destructed later.
struct FConstRefParm
{
	class UProperty* Property;
	FConstRefParm* Next;
	BYTE Data[sizeof(INT)];
};

//
// Information about script execution at one stack level.
//
struct CORE_API FFrame : public FOutputDevice
{
	// Variables.
	UStruct*		Node;
	UObject*		Object;
	BYTE*			Code;
	BYTE*			Locals;
	FConstRefParm*	ConstRefs;

	// Constructors.
	FFrame( UObject* InObject );
	FFrame( UObject* InObject, UStruct* InNode, INT CodeOffset, void* InLocals );
	~FFrame();

	// Functions.
	void Step( UObject* Context, RESULT_DECL );
	void Serialize( const TCHAR* V, enum EName Event );
	INT ReadInt();
	INT ReadIntInPlace(); // ReadInt but don't advance code pointer.
	UObject* ReadObject();
	UObject* ReadObjectInPlace(); // ReadObject but don't advance code pointer.
	FLOAT ReadFloat();
	INT ReadWord();
	FName ReadName();
	BYTE* GetLocalValue(class UProperty* P);
	BYTE* GetLocalValueIndex(class UProperty* P, INT Index);
	BYTE ReadByte();
	void SkipBytes(const INT Count);
	void SkipByte();
	void VerifyByte(EExprToken Expr);
	void Jump(const INT CodeOffset);

	// Const refs that can be stacked onto this frame.
	void ClearConstRefs();
	BYTE* AllocConstRef(UProperty* P);

	// Dump full UScript call stack to Log.
	static void DumpUScriptStack();
};

#define UNDEFINED_STATECODE ((BYTE*)-1)

//
// Information about script execution at the main stack level.
// This part of an actor's script state is saveable at any time.
//
struct CORE_API FStateFrame : public FFrame
{
	// Variables.
	FFrame* CurrentFrame;
	UState* StateNode;
	QWORD   ProbeMask;
	INT     LatentAction;

	static UObject* GCurrentExecutor;
	static BYTE* GPendingStateCode;

	static inline void SetStateExecutor(UObject* Obj)
	{
		guardSlow(FStateFrame::SetStateExecutor);
		GCurrentExecutor = Obj;
		GPendingStateCode = UNDEFINED_STATECODE;
		unguardSlow;
	}

	// Functions.
	FStateFrame( UObject* InObject );
	~FStateFrame();
	const TCHAR* Describe();
};

// Default Script buffer object!
class alignas(VECTOR_ALIGNMENT) FScriptBuffer
{
	INT Buffer[8]; // ==MAX_CONST_SIZE

public:
	FScriptBuffer()
	{
	}
	inline void* Get()
	{
		return Buffer;
	}
};

/*-----------------------------------------------------------------------------
	Script execution helpers.
-----------------------------------------------------------------------------*/

//
// Base class for UnrealScript iterator lists.
//
struct FIteratorList
{
	FIteratorList* Next;
	FIteratorList() {}
	FIteratorList( FIteratorList* InNext ) : Next( InNext ) {}
	FIteratorList* GetNext() { return (FIteratorList*)Next; }
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
