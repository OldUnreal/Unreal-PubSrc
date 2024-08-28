/*=============================================================================
	UnScript.h: UnrealScript execution engine.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Native functions.
-----------------------------------------------------------------------------*/

//
// Native function table.
//
typedef void (UObject::*Native)( FFrame& TheStack, RESULT_DECL );
extern CORE_API Native GNatives[];
BYTE CORE_API GRegisterNative( INT iNative, const Native& Func );

//
// Registering a native function.
//
#if _MSC_VER
	#define IMPLEMENT_FUNCTION(cls,num,func) \
		extern "C" DLL_EXPORT Native int##cls##func = (Native)&cls::func; \
		static BYTE cls##func##Temp = GRegisterNative( num, int##cls##func );
	#define IMPLEMENT_STRUCT_REP(strname,func) extern "C" DLL_EXPORT StructNetSer netserialize##strname = (StructNetSer)&func
#else
	#define IMPLEMENT_FUNCTION(cls,num,func) \
		DLL_EXPORT { Native int##cls##func = (Native)&cls::func; } \
		static BYTE cls##func##Temp = GRegisterNative( num, int##cls##func );
	#define IMPLEMENT_STRUCT_REP(strname,func) DLL_EXPORT { StructNetSer netserialize##strname = (StructNetSer)&func; }
#endif

/*-----------------------------------------------------------------------------
	Macros.
-----------------------------------------------------------------------------*/

// Ref parm macros
#define P_GET_REFP(Type,Name)		GNetObject = nullptr;						Stack.Step( Stack.Object, nullptr );	Type* Name=(Type*)GPropAddr; if(GNetObject && GNetObject->IsRegisteredObject()) GNetObject->SetNetworkDirty();
#define P_GET_REFP_OPTX(Type,Name)	GPropAddr = nullptr; GNetObject = nullptr;	Stack.Step( Stack.Object, nullptr );	Type* Name=(Type*)GPropAddr; if(GNetObject && GNetObject->IsRegisteredObject()) GNetObject->SetNetworkDirty();

//
// Macros for grabbing parameters for native functions.
//
#define P_GET_UBOOL(var)              DWORD var=0;                         Stack.Step( Stack.Object, &var    );
#define P_GET_UBOOL_OPTX(var,def)     DWORD var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_STRUCT(typ,var)         typ   var;                           Stack.Step( Stack.Object, &var    );
#define P_GET_STRUCT_OPTX(typ,var,def)typ   var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_STRUCT_REF(typ,var)     P_GET_REFP_OPTX(typ,var)
#define P_GET_INT(var)                INT   var=0;                         Stack.Step( Stack.Object, &var    );
#define P_GET_INT_OPTX(var,def)       INT   var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_INT_REF(var)            P_GET_REFP_OPTX(INT,var)
#define P_GET_FLOAT(var)              FLOAT var=0.f;                       Stack.Step( Stack.Object, &var    );
#define P_GET_FLOAT_OPTX(var,def)     FLOAT var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_FLOAT_REF(var)          P_GET_REFP_OPTX(FLOAT,var)
#define P_GET_BYTE(var)               BYTE  var=0;                         Stack.Step( Stack.Object, &var    );
#define P_GET_BYTE_OPTX(var,def)      BYTE  var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_BYTE_REF(var)           P_GET_REFP_OPTX(BYTE,var)
#define P_GET_NAME(var)               FName var=NAME_None;                 Stack.Step( Stack.Object, &var    );
#define P_GET_NAME_OPTX(var,def)      FName var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_NAME_REF(var)           P_GET_REFP_OPTX(FName,var)
#define P_GET_STR(var)                FString var;                         Stack.Step( Stack.Object, &var    );
#define P_GET_STR_OPTX(var,def)       FString var(def);                    Stack.Step( Stack.Object, &var    );
#define P_GET_STR_REF(var)            P_GET_REFP_OPTX(FString,var)
#define P_GET_OBJECT(cls,var)         cls*  var=NULL;                      Stack.Step( Stack.Object, &var    );
#define P_GET_OBJECT_OPTX(cls,var,def)cls*  var=def;                       Stack.Step( Stack.Object, &var    );
#define P_GET_OBJECT_REF(cls,var)     P_GET_REFP_OPTX(cls*,var)
#define P_GET_ARRAY_REF(typ,var)      P_GET_REFP_OPTX(typ,var)

//
// Convenience macros.
//
#define P_GET_CLASS(var)            P_GET_OBJECT(UClass,var)
#define P_GET_CLASS_OPTX(var,def)   P_GET_OBJECT_OPTX(UClass,var,def)
#define P_GET_CLASS_REF(var)        P_GET_OBJECT_REF(UClass,var)

// Color functions
#define P_GET_COLOR(var)            P_GET_STRUCT(FColor,var)

// Grab string as reference when possible.
#define P_GET_STR_LOCAL(var, code) \
	if (*Stack.Code == EX_LocalVariable || *Stack.Code == EX_InstanceVariable) \
	{ \
		Stack.Step(Stack.Object, nullptr); \
		const FString& var = *reinterpret_cast<FString*>(GPropAddr); \
		code; \
	} \
	else \
	{ \
		FString var; \
		Stack.Step(Stack.Object, &var); \
		code; \
	}

#define P_GET_SKIP_OFFSET(var)		INT var; {Stack.VerifyByte(EX_Skip); var=Stack.ReadWord(); }
#define P_FINISH					Stack.SkipByte();

//
// Convenience macros.
//
#define P_GET_VECTOR(var)           P_GET_STRUCT(FVector,var)
#define P_GET_VECTOR_OPTX(var,def)  P_GET_STRUCT_OPTX(FVector,var,def)
#define P_GET_VECTOR_REF(var)       P_GET_STRUCT_REF(FVector,var)
#define P_GET_ROTATOR(var)          P_GET_STRUCT(FRotator,var)
#define P_GET_ROTATOR_OPTX(var,def) P_GET_STRUCT_OPTX(FRotator,var,def)
#define P_GET_ROTATOR_REF(var)      P_GET_STRUCT_REF(FRotator,var)
#define P_GET_ACTOR(var)            P_GET_OBJECT(AActor,var)
#define P_GET_ACTOR_OPTX(var,def)   P_GET_OBJECT_OPTX(AActor,var,def)
#define P_GET_ACTOR_REF(var)        P_GET_OBJECT_REF(AActor,var)

// quaternion
#define P_GET_QUAT(var)           P_GET_STRUCT(FQuat,var)
#define P_GET_QUAT_OPTX(var,def)  P_GET_STRUCT_OPTX(FQuat,var,def)
#define P_GET_QUAT_REF(var)       P_GET_STRUCT_REF(FQuat,var)

//
// Iterator macros.
//
#define StartIterator						\
	INT wEndOffset = Stack.ReadWord();		\
	BYTE B=0;								\
	BYTE *StartCode = Stack.Code;			\
	FScriptBuffer stBuffer;					\
	void* Buffer = stBuffer.Get();
#define EndIterator Stack.Jump(wEndOffset + 1);
#define LoopIterator( BreakType )									\
	while( (B=*Stack.Code)!=EX_IteratorPop && B!=EX_IteratorNext )	\
		Stack.Step( Stack.Object, Buffer);							\
	if( B==EX_IteratorNext )										\
		Stack.Code = StartCode;										\
	else if( B==EX_IteratorPop )									\
	{																\
		Stack.SkipByte();											\
		BreakType;													\
	}

/*-----------------------------------------------------------------------------
	FFrame implementation.
-----------------------------------------------------------------------------*/

inline FFrame::FFrame( UObject* InObject )
:	Node		(InObject ? InObject->GetClass() : NULL)
,	Object		(InObject)
,	Code		(NULL)
,	Locals		(NULL)
,	ConstRefs	(NULL)
{}
inline FFrame::FFrame( UObject* InObject, UStruct* InNode, INT CodeOffset, void* InLocals )
:	Node		(InNode)
,	Object		(InObject)
,	Code		(&InNode->Script(CodeOffset))
,	Locals		(reinterpret_cast<BYTE*>(InLocals))
,	ConstRefs	(NULL)
{}
inline FFrame::~FFrame()
{
	if (ConstRefs)
		ClearConstRefs();
}
inline void FFrame::Step( UObject* Context, RESULT_DECL )
{
	guardSlow(FFrame::Step);
	const INT B = ReadByte();
	(Context->*GNatives[B])( *this, Result );
	unguardfSlow(( TEXT("(%ls @ %ls : %04X)"), Object->GetFullName(), Node->GetFullName(), Code - &Node->Script(0) ));
}
inline INT FFrame::ReadInt()
{
	INT Result;
	#if __LINUX_ARM__ || __EMSCRIPTEN__
	appMemcpy(&Result, Code, sizeof(INT));
	#else
	Result = *reinterpret_cast<INT*>(Code);
	#endif
	Code += 4; // sizeof(INT);
	return Result;
}
inline INT FFrame::ReadIntInPlace()
{
	INT Result;
#if __LINUX_ARM__ || __EMSCRIPTEN__
	appMemcpy(&Result, Code, sizeof(INT));
#else
	Result = *reinterpret_cast<INT*>(Code);
#endif
	return Result;
}
inline UObject* FFrame::ReadObject()
{
	guardSlow(FFrame::ReadObject);
	const INT ObjectIndex = ReadInt();
	if (!Node->ScriptObjects.IsValidIndex(ObjectIndex))
		appThrowf(TEXT("ReadObject invalid object index %i/%i"), ObjectIndex, Node->ScriptObjects.Num());
	return Node->ScriptObjects(ObjectIndex);
	unguardSlow;
}
inline UObject* FFrame::ReadObjectInPlace()
{
	guardSlow(FFrame::ReadObjectInPlace);
	const INT ObjectIndex = ReadIntInPlace();
	if (!Node->ScriptObjects.IsValidIndex(ObjectIndex))
		appThrowf(TEXT("ReadObject invalid object index %i/%i"), ObjectIndex, Node->ScriptObjects.Num());
	return Node->ScriptObjects(ObjectIndex);
	unguardSlow;
}
inline FLOAT FFrame::ReadFloat()
{
	FLOAT Result;
	#if __LINUX_ARM__ || __EMSCRIPTEN__
	appMemcpy(&Result, Code, sizeof(FLOAT));
	#else
	Result = *reinterpret_cast<FLOAT*>(Code);
	#endif
	Code += 4; // sizeof(FLOAT);
	return Result;
}
inline INT FFrame::ReadWord()
{
	INT Result;
	#if __LINUX_ARM__ || __EMSCRIPTEN__
	_WORD Temporary;
	appMemcpy(&Temporary, Code, sizeof(_WORD));
	Result = Temporary;
	#else
	Result = *reinterpret_cast<_WORD*>(Code);
	#endif
	Code += 2; // sizeof(_WORD);
	return Result;
}
inline FName FFrame::ReadName()
{
	guardSlow(FFrame::ReadName);
	const INT NameIndex = ReadInt();
	return FName((EName)NameIndex);
	unguardSlow;
}
inline void FFrame::ClearConstRefs()
{
	guardSlow(FFrame::ClearConstRefs);
	if (ConstRefs)
	{
		FConstRefParm* NC;
		for (FConstRefParm* C = ConstRefs; C; C = NC)
		{
			NC = C->Next;
			if (C->Property->PropertyFlags & CPF_NeedCtorLink)
			{
				for (INT i = (C->Property->ArrayDim - 1); i >= 0; --i)
					C->Property->DestroyValue(C->Data + (C->Property->ElementSize * i));
			}
			appFree(C);
		}
		ConstRefs = nullptr;
	}
	unguardSlow;
}
inline BYTE* FFrame::AllocConstRef(UProperty* P)
{
	guardSlow(FFrame::AllocConstRef);
	FConstRefParm* C = reinterpret_cast<FConstRefParm*>(appMalloc(sizeof(FConstRefParm) + P->GetSize() - sizeof(INT), TEXT("ConstRef")));
	appMemzero(C->Data, P->GetSize());
	C->Property = P;
	C->Next = ConstRefs;
	ConstRefs = C;
	return C->Data;
	unguardSlow;
}

inline BYTE* FFrame::GetLocalValue(UProperty* P)
{
	guardSlow(FFrame::GetLocalValue);
#if _USE_REF_PARMS
	if (P->IsOutReference())
		return *reinterpret_cast<BYTE**>(Locals + P->Offset);
#endif
	return Locals + P->Offset;
	unguardSlow;
}
inline BYTE* FFrame::GetLocalValueIndex(UProperty* P, INT Index)
{
	guardSlow(FFrame::GetLocalValueIndex);
#if _USE_REF_PARMS
	if (P->IsOutReference())
		return *reinterpret_cast<BYTE**>(Locals + P->Offset + (sizeof(void*) * Index));
#endif
	return Locals + P->Offset + (P->ElementSize * Index);
	unguardSlow;
}

inline void FFrame::Jump(const INT CodeOffset)
{
	guardSlow(FFrame::Jump);
	verifyfSlow(Node->Script.IsValidIndex(CodeOffset), TEXT("Invalid offset %i/%i"), CodeOffset, Node->Script.Num());
	Code = &Node->Script(CodeOffset);
	unguardSlow;
}
inline BYTE FFrame::ReadByte()
{
	return *Code++;
}
inline void FFrame::SkipBytes(const INT Count)
{
	Code += Count;
}
inline void FFrame::SkipByte()
{
	++Code;
}
inline void FFrame::VerifyByte(EExprToken Expr)
{
	guardSlow(FFrame::VerifyByte);
	verifyfSlow(*Code==Expr,TEXT("Got expression %ls, expected %ls"), GetExprName(*Code), GetExprName(Expr));
	++Code;
	unguardSlow;
}

CORE_API void GInitRunaway();

/*-----------------------------------------------------------------------------
	FStateFrame implementation.
-----------------------------------------------------------------------------*/

inline FStateFrame::FStateFrame( UObject* InObject )
:	FFrame		( InObject )
,	CurrentFrame( NULL )
,	StateNode	( InObject->GetClass() )
,	ProbeMask	( ~(QWORD)0 )
{}
inline FStateFrame::~FStateFrame()
{
	if (ConstRefs)
		ClearConstRefs();
}
inline const TCHAR* FStateFrame::Describe()
{
	return Node ? Node->GetFullName() : TEXT("None");
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
