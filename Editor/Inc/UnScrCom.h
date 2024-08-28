/*=============================================================================
	UnScrCom.h: UnrealScript compiler.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Constants & types.
-----------------------------------------------------------------------------*/

class FScripWarnContext : public FOutputDevice
{
public:
	FScripWarnContext();
	~FScripWarnContext();

	class FScriptCompiler* Compiler;

	void Serialize(const TCHAR* V, EName Event);
};

extern FScripWarnContext* GScriptWarn;

// Max nesting.
enum {MAX_NEST_LEVELS = 16};

//
// Token types.
//
enum ETokenType
{
	TOKEN_None				= 0x00,		// No token.
	TOKEN_Identifier		= 0x01,		// Alphanumeric identifier.
	TOKEN_Symbol			= 0x02,		// Symbol.
	TOKEN_Const				= 0x03,		// A constant.
	TOKEN_Max				= 0x0D
};

//
// Code nesting types.
//
enum ENestType
{
	NEST_None				=0x0000,	//  No nesting.
	NEST_Class				=0x0001,	//  Class/EndClass.
	NEST_State				=0x0002,	//	State/EndState.
	NEST_Function			=0x0003,	//	Function/EndFunction.
	NEST_If					=0x0005,	//  If/ElseIf/EndIf.
	NEST_Loop				=0x0006,	//  While/Do/Loop/Until.
	NEST_Switch				=0x0007,	//  Switch.
	NEST_For				=0x0008,	//  For.
	NEST_ForEach            =0x000A,    //  ForEach.
	NEST_Max				=0x000A
};

//NEW: CDH... missing return value warning
//
// Code nesting flags.
//
enum ENestFlags
{
	NESTF_ReturnValueFound	= 0x0001,	// Return w/ value has been found
};

//
// Types of statements to allow within a particular nesting block.
//
enum ENestAllowFlags : INT
{
	ALLOW_StateCmd			= 0x00001,	// Allow commands that reside in states only.
	ALLOW_Cmd				= 0x00002,	// Allow commands that take 0 cycle time.
	ALLOW_Function			= 0x00004,	// Allow Event declarations at this level.
	ALLOW_State				= 0x00008,	// Allow State declarations at this level.
	ALLOW_ElseIf			= 0x00010,	// Allow ElseIf commands at this level.
	ALLOW_VarDecl			= 0x00040,	// Allow variable declarations at this level.
	ALLOW_Class				= 0x00080,	// Allow class definition heading.
	ALLOW_Case				= 0x00100,	// Allow 'case' statement.
	ALLOW_Default			= 0x00200,	// Allow 'default' case statement.
	ALLOW_Return			= 0x00400,	// Allow 'return' within a function.
	ALLOW_Break				= 0x00800,	// Allow 'break' from loop.
	ALLOW_Label				= 0x02000,	// Allow any label.
	ALLOW_Ignores			= 0x04000,	// Allow function masks like 'Ignores'.
	ALLOW_Instance          = 0x08000,  // Allow instance variables and functions.
	ALLOW_InWhile           = 0x10000,  // In a 'while' loop.
	ALLOW_Continue			= 0x20000,	// Allow 'continue' from loop.
	ALLOW_Iterator          = 0x40000,	// Allow iterator expressions.
	ALLOW_Invariant			= 0x80000,	// Currently on an invariant function code.
};

/*-----------------------------------------------------------------------------
	FPropertyBase.
-----------------------------------------------------------------------------*/

//
// Property data type enums.
//warning: Script compiler has a hardcoded table based on these enum indices.
//
enum EPropertyType
{
	CPT_None,
	CPT_Byte,
	CPT_Int,
	CPT_Bool,
	CPT_Float,
	CPT_ObjectReference,
	CPT_Name,
	CPT_Struct = 10,
	CPT_Vector,
	CPT_Rotation,
	CPT_String,
	CPT_Pointer,
	CPT_Button,
	CPT_AnyValue,
	CPT_MAX,
};

#define CCASE(t) case t: return TEXT(#t)
#define DCASE(t,n) default: {TCHAR* Res = appStaticString1024(); appSprintf(Res, TEXT(#t) TEXT("%i"), INT(n)); return Res;}
static inline const TCHAR* GetEPropertyType(EPropertyType T)
{
	switch (T)
	{
		CCASE(CPT_None);
		CCASE(CPT_Byte);
		CCASE(CPT_Int);
		CCASE(CPT_Bool);
		CCASE(CPT_Float);
		CCASE(CPT_ObjectReference);
		CCASE(CPT_Name);
		CCASE(CPT_Struct);
		CCASE(CPT_Vector);
		CCASE(CPT_Rotation);
		CCASE(CPT_String);
		CCASE(CPT_Pointer);
		CCASE(CPT_Button);
		CCASE(CPT_AnyValue);
		DCASE(CPT_, T);
	}
}
static inline const TCHAR* GetETokenType(ETokenType T)
{
	switch (T)
	{
		CCASE(TOKEN_None);
		CCASE(TOKEN_Identifier);
		CCASE(TOKEN_Symbol);
		CCASE(TOKEN_Const);
		DCASE(TOKEN_, T);
	}
}
#undef CCASE
#undef DCASE

static UBOOL PropertyTypesMatch(UProperty* A, UProperty* B)
{
	if (A->GetClass() != B->GetClass() || A->ArrayDim!=B->ArrayDim)
		return 0;

	if (A->IsA(UClassProperty::StaticClass()))
		return (((UClassProperty*)A)->MetaClass == ((UClassProperty*)B)->MetaClass);
	if (A->IsA(UObjectProperty::StaticClass()))
		return (((UObjectProperty*)A)->PropertyClass == ((UObjectProperty*)B)->PropertyClass);
	if (A->IsA(UArrayProperty::StaticClass()))
		return PropertyTypesMatch(((UArrayProperty*)A)->Inner, ((UArrayProperty*)B)->Inner);
	if (A->IsA(UMapProperty::StaticClass()))
		return PropertyTypesMatch(((UMapProperty*)A)->Key, ((UMapProperty*)B)->Key) && PropertyTypesMatch(((UMapProperty*)A)->Value, ((UMapProperty*)B)->Value);
	if (A->IsA(UStructProperty::StaticClass()))
		return (((UStructProperty*)A)->Struct == ((UStructProperty*)B)->Struct);
	if (A->IsA(UByteProperty::StaticClass()))
		return (((UByteProperty*)A)->Enum == ((UByteProperty*)B)->Enum);
	return 1;
}

//
// Basic information describing a type.
//
class FPropertyBase
{
public:
	// Variables.
	EPropertyType Type;
	INT ArrayDim;
	DWORD PropertyFlags;
	DWORD TempPropertyFlags;
	BYTE DynArrayFlags;
	union
	{
		class UEnum* Enum;
		class UClass* PropertyClass;
		class UStruct* Struct;
	};
	union
	{
		DWORD BitMask;
		INT StringSize;
	};
	UClass* MetaClass;
	FPropertyBase* KeyType;
	UProperty* HighProperty;
	FString CppType;

	// Constructors.
	FPropertyBase( EPropertyType InType )
	:	Type(InType), ArrayDim(1), PropertyFlags(0), TempPropertyFlags(0), Enum(NULL), BitMask(0), MetaClass(NULL), DynArrayFlags(0), KeyType(NULL), HighProperty(NULL)
	{}
	FPropertyBase(EPropertyType InType, DWORD InFlags)
		: Type(InType), ArrayDim(1), PropertyFlags(InFlags), TempPropertyFlags(0), Enum(NULL), BitMask(0), MetaClass(NULL), DynArrayFlags(0), KeyType(NULL), HighProperty(NULL)
	{}
	FPropertyBase( UEnum* InEnum )
	:	Type(CPT_Byte), ArrayDim(1), PropertyFlags(0), TempPropertyFlags(0), Enum(InEnum), BitMask(0), MetaClass(NULL), DynArrayFlags(0), KeyType(NULL), HighProperty(NULL)
	{}
	FPropertyBase( UClass* InClass, UClass* InMetaClass=NULL )
	:	Type(CPT_ObjectReference), ArrayDim(1), PropertyFlags(0), TempPropertyFlags(0), PropertyClass(InClass), BitMask(0), MetaClass(InMetaClass), DynArrayFlags(0), KeyType(NULL), HighProperty(NULL)
	{}
	FPropertyBase( UStruct* InStruct )
	:	Type(CPT_Struct), ArrayDim(1), PropertyFlags(0), TempPropertyFlags(0), Struct(InStruct), BitMask(0), MetaClass(NULL), DynArrayFlags(0), KeyType(NULL), HighProperty(NULL)
	{}
	FPropertyBase( UProperty* Property )
	:	BitMask(0), KeyType(NULL), HighProperty(NULL)
	{
		guard(FPropertyBase::FPropertyBase);
		UBOOL DynArray=0;
		DynArrayFlags = 0;
		BYTE DynFlags = 0;

		if (!Property)
			return;

		UProperty* Org = Property;
		UProperty* KeyProperty = NULL;
		if( Property->GetClass()==UArrayProperty::StaticClass() )
		{
			DynFlags = 1;
			DynArray = 1;
			if (Property->PropertyFlags & CPF_Const)
				DynFlags |= 4;
			Property = CastChecked<UArrayProperty>(Property)->Inner;
			if( Property->GetClass()==UStructProperty::StaticClass() && ((UStructProperty*)Property)->Struct->GetFName()==NAME_Template )
				DynFlags |= 2;
		}
		if (Property->GetClass() == UMapProperty::StaticClass())
		{
			UMapProperty* Map = (UMapProperty*)Property;
			KeyProperty = Map->Key;
			Property = Map->Value;
			DynArray = 2;
		}
		if( Property->GetClass()==UByteProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_Byte);
			Enum = Cast<UByteProperty>(Property)->Enum;
		}
		else if( Property->GetClass()==UIntProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_Int);
		}
		else if( Property->GetClass()==UBoolProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_Bool);
			BitMask = Cast<UBoolProperty>(Property)->BitMask;
		}
		else if( Property->GetClass()==UFloatProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_Float);
		}
		else if( Property->GetClass()==UClassProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_ObjectReference);
			PropertyClass = Cast<UClassProperty>(Property)->PropertyClass;
			MetaClass = Cast<UClassProperty>(Property)->MetaClass;
		}
		else if( Property->GetClass()==UObjectProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_ObjectReference);
			PropertyClass = Cast<UObjectProperty>(Property)->PropertyClass;
		}
		else if( Property->GetClass()==UNameProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_Name);
		}
		else if( Property->GetClass()==UStrProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_String);
		}
		else if( Property->GetClass()==UStructProperty::StaticClass() )
		{
			*this = FPropertyBase(CPT_Struct);
			Struct = Cast<UStructProperty>(Property)->Struct;
		}
		else if (Property->GetClass() == UPointerProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Pointer);
		}
		else if (Property->GetClass() == UButtonProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_Button);
		}
		else if (Property->GetClass() == UAnyProperty::StaticClass())
		{
			*this = FPropertyBase(CPT_AnyValue);
		}
		else
		{
			appErrorf( TEXT("Unknown property type '%ls'"), Property->GetFullName() );
		}
		HighProperty = Org;
		ArrayDim = DynArray ? 0 : Property->ArrayDim;
		PropertyFlags = Property->PropertyFlags;
		TempPropertyFlags = Property->TempPropertyFlags;
		DynArrayFlags = DynFlags;
		if (DynFlags & 4)
			PropertyFlags |= CPF_Const;
		if (DynArray == 2)
			KeyType = new(GMem) FPropertyBase(KeyProperty);
		unguard;
	}

	// Functions.
	INT GetSize() const //hardcoded sizes!!
	{
		static const INT ElementSizes[CPT_MAX] =
		{
			0,
			sizeof(BYTE),
			sizeof(INT),
			sizeof(DWORD),
			sizeof(FLOAT),
			sizeof(DWORD), // stijn: was sizeof(UObject*) before but we don't want to keep the serialized property format equal on all architectures
			sizeof(FName),
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			sizeof(DWORD), // anyproperty
		};
		INT ElementSize
		=	Type!=CPT_Struct
		?	ElementSizes[ Type ]
		:	Struct->GetSerializedSize();
		return ElementSize * ArrayDim;
	}
	UBOOL IsVector() const
	{
		return Type==CPT_Struct && appStricmp(*Struct->GetFName(),TEXT("Vector"))==0;
	}
	UBOOL IsRotator() const
	{
		return Type==CPT_Struct && appStricmp(*Struct->GetFName(),TEXT("Rotator"))==0;
	}
	UBOOL IsIdenticalParmType(const FPropertyBase& Other) const // Check if type is identical, for function paramaters comparison.
	{
		if (Type != Other.Type)
			return FALSE;
		if ((PropertyFlags & (CPF_OutParm | CPF_Const | CPF_SkipParm | CPF_CoerceParm | CPF_OptionalParm)) != (Other.PropertyFlags & (CPF_OutParm | CPF_Const | CPF_SkipParm | CPF_CoerceParm | CPF_OptionalParm)))
			return FALSE;
		if ((DynArrayFlags & 2) && (Other.DynArrayFlags & 1))
			return !(Other.DynArrayFlags & 4); // Template array.
		else if (ArrayDim != Other.ArrayDim)
		{
			// Mismatched array dimensions.
			return 0;
		}
		switch (Type)
		{
		case CPT_Byte:
			// Make sure enums match.
			return (Enum == Other.Enum);
		case CPT_ObjectReference:
			// Exact match required.
			return (PropertyClass == Other.PropertyClass && MetaClass == Other.MetaClass);
		case CPT_Struct:
			return (Struct == Other.Struct);
		default:
			return TRUE;
		}
	}
	UBOOL MatchesType( const FPropertyBase& Other, UBOOL Identity ) const
	{
		check(Type!=CPT_None || !Identity);

		// If converting to an l-value, we require an exact match with an l-value.
		if (PropertyFlags & CPF_OutParm)
		{
			if (!(PropertyFlags & CPF_Const) || !(Other.PropertyFlags & (CPF_OutParm | CPF_Const)))
			{
				if ((Other.PropertyFlags & CPF_Const) || !(Other.PropertyFlags & CPF_OutParm))
					return 0;
			}
			Identity = 1;
		}

		// Check everything.
		if (Type == CPT_None && (Other.Type == CPT_None || !Identity))
		{
			// If Other has no type, accept anything.
			return 1;
		}
		else if ((DynArrayFlags & 2) && (Other.DynArrayFlags & 1))
			return !(Other.DynArrayFlags & 4); // Template array.
		else if( Type != Other.Type )
		{
			// Mismatched base types.
			return 0;
		}
		else if( ArrayDim != Other.ArrayDim )
		{
			// Mismatched array dimensions.
			return 0;
		}
		else if( Type==CPT_Byte )
		{
			// Make sure enums match, or we're generalizing.
			return Enum==Other.Enum || (Enum==NULL && !Identity);
		}
		else if( Type==CPT_ObjectReference )
		{
			check(PropertyClass!=NULL);

			// Make sure object types match, or we're generalizing.
			if( Identity )
			{
				// Exact match required.
				return PropertyClass==Other.PropertyClass && MetaClass==Other.MetaClass;
			}
			else if( Other.PropertyClass==NULL )
			{
				// Cannonical 'None' matches all object classes.
				return 1;
			}
			else
			{
				// Generalization is ok.
				if( Other.PropertyClass->IsChildOf(PropertyClass) )
					if( PropertyClass!=UClass::StaticClass() || Other.MetaClass->IsChildOf(MetaClass) )
						return 1;
				return 0;
			}
		}
		else if( Type==CPT_Struct )
		{
			check(Struct!=NULL);
			check(Other.Struct!=NULL);

			if (Struct != Other.Struct)
			{
				// 227j: Allow structs with same members types to match.
				TFieldIterator<UProperty> OIt(Other.Struct);
				UBOOL bMismatch = 0;
				for (TFieldIterator<UProperty> It(Struct); It; ++It, ++OIt)
				{
					if (!OIt || !PropertyTypesMatch(*It, *OIt))
					{
						bMismatch = 1;
						break;
					}
				}
				if (!bMismatch && !OIt)
					return 1;
			}

			// Make sure struct types match, or we're generalizing.
			if( Identity ) return Struct==Other.Struct;            // Exact match required.
			else           return Other.Struct->IsChildOf(Struct); // Generalization is ok.
		}
		else
		{
			// General match.
			return 1;
		}
	}

	inline void DumpDebug()
	{
		debugf(TEXT("PropertyType %ls ArrayDim %i MetaClass %ls HighProperty %ls DynArrayFlags %i"), GetEPropertyType(Type), ArrayDim, MetaClass->GetFullName(), HighProperty->GetFullName(), (INT)DynArrayFlags);
		debugf(TEXT("Flags Out: %ls Const: %ls"), (PropertyFlags & CPF_OutParm) ? GTrue : GFalse, (PropertyFlags & CPF_Const) ? GTrue : GFalse);
	}
};

/*-----------------------------------------------------------------------------
	FToken.
-----------------------------------------------------------------------------*/

//
// Information about a token that was just parsed.
//
class FToken : public FPropertyBase
{
public:
	// Variables.
	ETokenType	TokenType;		// Type of token.
	FName		TokenName;		// Name of token.
	INT			StartPos;		// Starting position in script where this token came from.
	INT			StartLine;		// Starting line in script.
	TCHAR		Identifier[NAME_SIZE]; // Always valid.
	UBOOL		bMathIdentifier; // Upcoming maths contains variables.

	// Marco: Removed union because of 64-bit alignment problems!
	// TOKEN_Const values.
	BYTE	Byte;								// If CPT_Byte.
	INT		Int;								// If CPT_Int.
	UBOOL	Bool;								// If CPT_Bool.
	FLOAT	Float;								// If CPT_Float.
	UObject* Object;							// If CPT_ObjectReference.
	FName	NameBytes;							// If CPT_Name.
	FString String;
	FVector VectorBytes;						// If CPT_Struct && IsVector().
	FRotator RotationBytes;						// If CPT_Struct && IsRotator().

	// Constructors.
	FToken()
	: FPropertyBase( CPT_None )
	{
		InitToken( CPT_None );
	}
	FToken( const FPropertyBase& InType )
	: FPropertyBase( CPT_None )
	{
		InitToken( CPT_None );
		(FPropertyBase&)*this = InType;
	}

	// Inlines.
	void InitToken( EPropertyType InType )
	{
		(FPropertyBase&)*this = FPropertyBase(InType);
		TokenType		= ETokenType::TOKEN_None;
		TokenName		= NAME_None;
		StartPos		= 0;
		StartLine		= 0;
		bMathIdentifier = FALSE;
	}
	UBOOL Matches( const TCHAR* Str )
	{
		return (TokenType==TOKEN_Identifier || TokenType==TOKEN_Symbol) && appStricmp(Identifier,Str)==0;
	}
	UBOOL Matches( FName Name )
	{
		return TokenType==TOKEN_Identifier && TokenName==Name;
	}
	void AttemptToConvertConstant( const FPropertyBase& NewType )
	{
		check(TokenType==TOKEN_Const);
		switch( NewType.Type )
		{
			case CPT_Int:		{INT        V(0);           if( GetConstInt     (V) ) SetConstInt     (V); break;}
			case CPT_Bool:		{UBOOL      V(0);           if( GetConstBool    (V) ) SetConstBool    (V); break;}
			case CPT_Float:		{FLOAT      V(0.f);         if( GetConstFloat   (V) ) SetConstFloat   (V); break;}
			case CPT_Name:		{FName      V(NAME_None);   if( GetConstName    (V) ) SetConstName    (V); break;}
			case CPT_Struct:
			{
				if( NewType.IsVector() )
				{
					FVector V( 0.f, 0.f, 0.f );
					if( GetConstVector( V ) )
						SetConstVector( V );
				}
				else if( NewType.IsRotator() )
				{
					FRotator V( 0, 0, 0 );
					if( GetConstRotation( V ) )
						SetConstRotation( V );
				}
				//!!struct conversion support would be nice
				break;
			}
			case CPT_Byte:
			{
				BYTE V=0;
				if( NewType.Enum==NULL && GetConstByte(V) )
					SetConstByte(NULL,V);
				break;
			}
			case CPT_ObjectReference:
			{
				UObject* Ob=NULL;
				if( GetConstObject( NewType.PropertyClass, Ob ) )
					SetConstObject( Ob );
				break;
			}
			case CPT_None:
            case CPT_Vector:
            case CPT_Rotation:
            case CPT_String:
            case CPT_Pointer:
			case CPT_Button:
			case CPT_AnyValue:
            case CPT_MAX:
			{
				break;
			}
		}
	}

	// Setters.
	void SetConstByte( UEnum* InEnum, BYTE InByte )
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Byte);
		Enum			= InEnum;
		Byte			= InByte;
		TokenType		= TOKEN_Const;
	}
	void SetConstInt( INT InInt )
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Int);
		Int				= InInt;
		TokenType		= TOKEN_Const;
	}
	void SetConstBool( UBOOL InBool )
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Bool);
		Bool 			= InBool;
		TokenType		= TOKEN_Const;
	}
	void SetConstFloatInt(FLOAT InFloat)
	{
		if (appFloor(InFloat) == InFloat)
		{
			(FPropertyBase&)*this = FPropertyBase(CPT_Int);
			Int = appFloor(InFloat);
		}
		else
		{
			(FPropertyBase&)*this = FPropertyBase(CPT_Float);
			Float = InFloat;
		}
		TokenType = TOKEN_Const;
	}
	void SetConstFloat( FLOAT InFloat )
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_Float);
		Float			= InFloat;
		TokenType		= TOKEN_Const;
	}
	void SetConstObject( UObject* InObject )
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_ObjectReference);
		PropertyClass	= InObject ? InObject->GetClass() : NULL;
		Object			= InObject;
		TokenType		= TOKEN_Const;
		if( PropertyClass==UClass::StaticClass() )
			MetaClass = CastChecked<UClass>(InObject);
	}
	void SetConstName( FName InName )
	{
		(FPropertyBase&)*this	= FPropertyBase(CPT_Name);
		NameBytes				= InName;
		TokenType				= TOKEN_Const;
	}
	void SetConstString( const TCHAR* InString )
	{
		(FPropertyBase&)*this = FPropertyBase(CPT_String);
		String = InString;
		TokenType = TOKEN_Const;
	}
	void SetConstVector( const FVector &InVector )
	{
		(FPropertyBase&)*this	= FPropertyBase(CPT_Struct);
		Struct					= FindObjectChecked<UStruct>( ANY_PACKAGE, TEXT("Vector") );//!!
		VectorBytes				= InVector;
		TokenType				= TOKEN_Const;
	}
	void SetConstRotation( const FRotator &InRotation )
	{
		(FPropertyBase&)*this	= FPropertyBase(CPT_Struct);
		Struct					= FindObjectChecked<UStruct>( ANY_PACKAGE, TEXT("Rotator") );//!!
		RotationBytes			= InRotation;
		TokenType				= TOKEN_Const;
	}
	//!!struct constants

	// Getters.
	UBOOL GetConstByte( BYTE& B )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Byte )
		{
			B = Byte;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Int && Int>=0 && Int<255 )
		{
			B = Int;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Float && Float>=0 && Float<255 && Float==(INT)Float)
		{
			B = appRound(Float);
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstInt( INT& I )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Int )
		{
			I = Int;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Byte )
		{
			I = Byte;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Float && Float==(INT)Float)
		{
			I=appRound(Float);
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstBool( UBOOL& B )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Bool )
		{
			B = Bool;
			return 1;
		}
		else return 0;
	}
	UBOOL GetConstFloat( FLOAT& R )
	{
		if (TokenType == TOKEN_Const)
		{
			switch (Type)
			{
			case CPT_Float:
				R = Float;
				return TRUE;
			case CPT_Int:
				R = Int;
				return TRUE;
			case CPT_Byte:
				R = Byte;
				return TRUE;
			}
		}
		return FALSE;
	}
	UBOOL GetConstStringConversion(FString& V)
	{
		if (TokenType == TOKEN_Const)
		{
			switch (Type)
			{
			case CPT_String:
				V = String;
				return TRUE;
			case CPT_Float:
				V = FString::Printf(TEXT("%f"), Float);
				return TRUE;
			case CPT_Int:
				V = FString::Printf(TEXT("%i"), Int);
				return TRUE;
			case CPT_Byte:
				V = FString::Printf(TEXT("%i"), (INT)Byte);
				return TRUE;
			case CPT_Bool:
				V = Bool ? TEXT("True") : TEXT("False");
				return TRUE;
			case CPT_Name:
				V = *NameBytes;
				return TRUE;
			}
		}
		return FALSE;
	}
	UBOOL GetConstObject( UClass* DesiredClass, UObject*& Ob )
	{
		if( TokenType==TOKEN_Const && Type==CPT_ObjectReference && (DesiredClass==NULL || PropertyClass->IsChildOf(DesiredClass)) )
		{
			Ob = Object;
			return 1;
		}
		return 0;
	}
	UBOOL GetConstName( FName& n )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Name )
		{
			n = NameBytes;
			return 1;
		}
		return 0;
	}
	UBOOL GetConstVector( FVector& v )
	{
		if( TokenType==TOKEN_Const && IsVector() )
		{
			v = VectorBytes;
			return 1;
		}
		return 0;
	}
	//!!struct constants
	UBOOL GetConstRotation( FRotator& r )
	{
		if( TokenType==TOKEN_Const && IsRotator() )
		{
			r = RotationBytes;
			return 1;
		}
		return 0;
	}

	inline void DumpDebug()
	{
		TCHAR* CCType = appStaticString1024();
		appStrcpy(CCType, TEXT("<Unknown>"));
		if (TokenType == TOKEN_Const)
		{
			switch (Type)
			{
			case CPT_Name:
				appStrcpy(CCType, *NameBytes);
				break;
			case CPT_String:
				appStrcpy(CCType, *String);
				break;
			case CPT_Float:
				appSprintf(CCType, TEXT("%f"), Float);
				break;
			case CPT_Int:
				appSprintf(CCType, TEXT("%i"), Int);
				break;
			case CPT_Byte:
				appSprintf(CCType, TEXT("%i"), (INT)Byte);
				break;
			case CPT_Bool:
				appStrcpy(CCType, Bool ? TEXT("True") : TEXT("False"));
				break;
			}
		}
		debugf(TEXT("TokenType %ls TokenName %ls Identifier %ls Const '%ls'"), GetETokenType(TokenType), *TokenName, Identifier, CCType);
		FPropertyBase::DumpDebug();
	}
};

/*-----------------------------------------------------------------------------
	Retry points.
-----------------------------------------------------------------------------*/

//
// A point in the script compilation state that can be set and returned to
// using InitRetry() and PerformRetry().  This is used in cases such as testing
// to see which overridden operator should be used, where code must be compiled
// and then "undone" if it was found not to match.
//
// Retries are not allowed to cross command boundaries (and thus nesting
// boundaries).  Retries can occur across a single command or expressions and
// subexpressions within a command.
//
struct FRetryPoint
{
	const TCHAR* Input;
	INT InputPos;
	INT InputLine;
	INT CodeTop;
};

/*-----------------------------------------------------------------------------
	FNestInfo.
-----------------------------------------------------------------------------*/

//
// Types of code offset fixups we can perform.
//
enum EFixupType
{
	FIXUP_SwitchEnd		= 0, // Address past end of Switch construct.
	FIXUP_IfEnd			= 1, // Address past end of If construct.
	FIXUP_LoopStart		= 2, // Address of loop start.
	FIXUP_LoopEnd		= 3, // Address past end of Loop construct.
	FIXUP_LoopPostCond	= 4, // Address of loop post condition.
	FIXUP_ForStart		= 5, // Address of for start.
	FIXUP_ForEnd		= 6, // Address past end of For construct.
	FIXUP_Label			= 7, // Address of a label.
	FIXUP_IteratorEnd   = 8, // Address of end of iterator.
	FIXUP_ForInc        = 9, // Address where for variable is incremented.
	FIXUP_MAX			=10, // Maximum value.
};

//
// A fixup request.
//
struct FNestFixupRequest
{
	// Variables.
	EFixupType			Type;			// Type of fixup request.
	INT					iCode;			// Address in script code to apply the fixup.
	FName				Name;			// Label name, if FIXUP_Label.
	FNestFixupRequest*	Next;			// Next fixup request in nest info's linked list.

	// Constructor.
	FNestFixupRequest( EFixupType InType, INT iInCode, FName InName, FNestFixupRequest *InNext )
	:	Type	(InType)
	,	iCode	(iInCode)
	,	Name	(InName)
	,	Next	(InNext)
	{}
};

//
// Temporary compiler information about a label, stored at a nest level.
//
struct FLabelRecord : public FLabelEntry
{
	// Variables.
	FLabelRecord* Next; // Next label in the nest info's linked list of labels.

	// Constructor.
	FLabelRecord( FName InName, INT iInCode, FLabelRecord *InNext )
	:	FLabelEntry		( InName, iInCode )
	,	Next			( InNext )
	{}
};

//
// Information about a function we're compiling.
//
struct FFuncInfo
{
	// Variables;
	FToken		Function;		// Name of the function or operator.
	INT			Precedence;		// Binary operator precedence.
	DWORD		FunctionFlags;	// Function flags.
	INT			iNative;		// Index of native function.
	INT			ExpectParms;	// Number of parameters expected for operator.

	// Constructor.
	FFuncInfo()
	:	Function		()
	,	Precedence		(0)
	,	FunctionFlags   (0)
	,	iNative			(0)
	,	ExpectParms		(0)
	{}
};

//
// Information for a particular nesting level.
//
struct FNestInfo
{
	// Information for all nesting levels.
	UStruct*		Node;               // Link to the stack node.
	ENestType		NestType;			// Statement that caused the nesting.
	INT				Allow;				// Types of statements to allow at this nesting level.
	INT				NestFlags;			// NESTF_ nest level flags

	// Information for nesting levels.
	INT				Fixups[FIXUP_MAX];	// Fixup addresses for PopNest to use.
	FNestFixupRequest* FixupList;		// Pending fixup requests.
	FLabelRecord*	LabelList;			// Linked list of labels.
	INT				iCodeChain;			// Code index for next command, i.e. in strings of if/elseif/elseif...

	// Command specific info.
	FToken			SwitchType;			// Type of Switch statement.
	FRetryPoint		ForRetry;			// Retry point (used in For command).

	// Set a fixup address.
	void SetFixup( EFixupType Type, INT iCode )
	{
		check(Fixups[Type]==MAXWORD);
		Fixups[Type] = iCode;
	}
};

/*-----------------------------------------------------------------------------
	FScriptCompiler.
-----------------------------------------------------------------------------*/

//
// Little class for writing to scripts.
//
class FScriptWriter : public FArchive
{
public:
	FScriptWriter( class FScriptCompiler& InCompiler )
	: Compiler( InCompiler )
	{}
	void Serialize( void* V, INT Length );
	FArchive& operator<<(class FName& N);
	FArchive& operator<<(class UObject*& Res);
	FArchive& operator<<( TCHAR* S )
		{Serialize(S,appStrlen(S)+1); return *this;}
	FArchive& operator<<( enum EExprToken E )
		{BYTE B=E; return *this<<B;}
	FArchive& operator<<( enum EPropertyType E )
		{BYTE B=E; return *this<<B;}
	FScriptCompiler& Compiler;
};


//
// Script compiler class.
//
class EDITOR_API FScriptCompiler : public FContextSupplier
{
public:
	// Variables.
	FString			ContextFormat;			// Printf formatting string.
	FFeedbackContext* Warn;					// For compiling messages and errors.
	UClass*			Class;					// Actor class info while compiling is happening.
	FScriptWriter	Writer;					// Script writer.
	FMemStack*		Mem;					// Pool for temporary allocations.
	const TCHAR*	Input;					// Input text.
	INT				InputLen;				// Length of input text.
	INT				InputPos;				// Current position in text.
	INT				InputLine;				// Current line in text.
	INT				PrevPos;				// Position previous to last GetChar() call.
	INT				PrevLine;				// Line previous to last GetChar() call.
	INT				StatementsCompiled;		// Number of statements compiled.
	INT				LinesCompiled;			// Total number of lines compiled.
	UBOOL			GotAffector;			// Got an expression that has a side effect?
	UBOOL			GotIterator;			// Got an iterator.
	INT				Booting;				// Bootstrap compiling classes.
	INT				Pass;					// Compilation pass.
	INT				NestLevel;				// Current nest level, starts at 0.
	FNestInfo*		TopNest;				// Top nesting level.
	UStruct*		TopNode;				// Top stack node.
	FNestInfo		Nest[MAX_NEST_LEVELS];	// Information about all nesting levels.
	INT             OriginalPropertiesSize;	// Original native properties size before compile.
	UBOOL			ShowDep;				// Show dependencies when recompiling.
	INT				ErrorCount;				// Error counting
	UBOOL			bCompileFailed;
	INT				WarnedClassInUse;
	UFunction*		TopLevelCall;
	TMultiMap<FName, UFunction*>	InvariantMap;
	TMap<UFunction*, DWORD> NetFunctions;	// List of functions that have preset networking set to declaration.

	INT CompileDenom, CompileNom;

	// Constructor.
	FScriptCompiler(FFeedbackContext* InWarn);

	~FScriptCompiler()
	{
		UnReferencedLocal* Next;
		while (unReferencedLocals)
		{
			Next = unReferencedLocals->next;
			delete unReferencedLocals;
			unReferencedLocals = Next;
		}
	}

	static void ClearScripts(UStruct* Node);

	// Precomputation.
	void PostParse( UStruct* Node );

	// FContextSupplier interface.
	FString GetContext();

	// High-level compiling functions.
	INT				CompileScript( UClass* Class, FMemStack* Mem, UBOOL Booting, INT Pass );
	INT				CompileDeclaration( FToken& Token, UBOOL& NeedSemicolon );
	void			CompileCommand( FToken& Token, UBOOL& NeedSemicolon );
	INT				CompileStatement();
	void			CompileStatements();
	INT				CompileExpr( const FPropertyBase RequiredType, const TCHAR* ErrorTag=NULL, FToken* ResultType=NULL, INT MaxPrecedence=MAXINT, FPropertyBase* HintType=NULL );
	UBOOL			CompileFieldExpr( UStruct* Scope, FPropertyBase& RequiredType, FToken Token, FToken& ResultType, UBOOL IsSelf, UBOOL IsConcrete, UBOOL IsInvariant=FALSE );
	INT				CompileDynamicCast( FToken Token, FToken& ResultType );
	void			CompileAssignment( const TCHAR* Tag );
	void			CompileAffector();
	void			CompileDirective();
	void			CompileSecondPass( UStruct* Node );
	UEnum*			CompileEnum( UStruct* Owner );
	UStruct*		CompileStruct( UStruct* Owner );
	void			CompileConst( UStruct* Owner );
	BYTE			CompileFunction(FToken& Token, UStruct* Owner, UBOOL& NeedSemicolon, BYTE bStruct);

	// High-level parsing functions.
	UBOOL			GetToken( FToken& Token, const FPropertyBase* Hint=NULL, UBOOL NoConsts=FALSE, UBOOL NoMacro=FALSE, UBOOL bNoMathOps=FALSE );
	UBOOL			GetRawToken( FString& Token );
	void			UngetToken( FToken& Token );
	void			TryMergeStrTokens(FToken& Token);
	UBOOL			SolveMathOps(FToken& Token, UBOOL bSolveMaths = TRUE);
	UBOOL			NestedSolveMath(FToken& Token, UBOOL& bSolved, BYTE MinLevel = 0);
	UBOOL			GetIdentifier( FToken& Token, INT NoConsts=0 );
	UClass*			GetQualifiedClass( const TCHAR* Thing );
	UBOOL			GetSymbol( FToken& Token );
	void			CheckAllow( const TCHAR* Thing, DWORD AllowFlags );
	void			CheckInScope( UObject* Obj );
	UField*			FindField( UStruct* InScope, const TCHAR* InIdentifier, UClass* FieldClass=UField::StaticClass(), const TCHAR* Thing=NULL );
	INT				ConversionCost( const FPropertyBase& Dest, const FPropertyBase& Source );
	void			SkipStatements( int SubCount, const TCHAR* ErrorTag );
	UBOOL			GetVarType( UStruct* Scope, FPropertyBase& VarProperty, DWORD& ObjectFlags, DWORD Disallow, const TCHAR* Thing );
	UProperty*      GetVarNameAndDim( UStruct* Struct, FPropertyBase& VarProperty, DWORD ObjectFlags, UBOOL NoArrays, UBOOL IsFunction, const TCHAR* HardcodedName, const TCHAR* Thing, FName Category, UBOOL Skip );
	FString			GetCommentLine();

	// Low-level parsing functions.
	const TCHAR*	NestTypeName( ENestType NestType );
	TCHAR			GetChar( UBOOL Literal=0 );
	TCHAR			PeekChar();
	TCHAR			GetLeadingChar();
	void			UngetChar();
	UBOOL			IsEOL( TCHAR c );
	void VARARGS	AddResultText( const TCHAR* Fmt, ... );
	UBOOL			GetConstInt( int& Result, const TCHAR* Tag=NULL, UStruct* Scope=NULL ); //NEW: LanguageEnhancements -- from Paul DuBois, InfiniteMachine
	UBOOL			GetConstFloat( FLOAT& Result, const TCHAR* Tag=NULL );
	const TCHAR*	FunctionNameCpp( UFunction* Function );

	// Nest management functions.
	void			PushNest( ENestType NestType, FName ThisName, UStruct* InNode, UStruct* StructNode=NULL );
	void			PopNest( ENestType NestType, const TCHAR* Descr );
	INT				FindNest( ENestType NestType );

	// Matching predefined text.
	UBOOL			MatchIdentifier( FName Match );
	UBOOL			PeekIdentifier( FName Match );
	UBOOL			MatchSymbol( const TCHAR* Match );
	UBOOL			PeekSymbol( const TCHAR* Match );

	// Requiring predefined text.
	void			RequireIdentifier( FName Match, const TCHAR* Tag );
	void			RequireSymbol( const TCHAR* Match, const TCHAR* Tag );
	void			RequireSizeOfParm( FToken& TypeToken, const TCHAR* Tag );

	// Retry functions.
	void			InitRetry( FRetryPoint& Retry );
	void			PerformRetry( FRetryPoint& Retry, UBOOL Binary=1, UBOOL Text=1 );
	void			CodeSwitcheroo( FRetryPoint& LowRetry, FRetryPoint& HighRetry );
	void			CodeReplace(FRetryPoint& LowRetry, FRetryPoint& MidRetry, FRetryPoint& HighRetry);

	// Emitters.
	void			EmitConstant( FToken& ConstToken );
	void			EmitStackNodeLinkFunction( UFunction* Node, UBOOL ForceFinal, UBOOL Global );
	void			EmitAddressToFixupLater( FNestInfo* Nest, EFixupType Type, FName Name );
	void			EmitAddressToChainLater( FNestInfo* Nest );
	void			EmitChainUpdate( FNestInfo* Nest );
	void			EmitSize( int Size, const TCHAR* Tag );
	void			EmitLet( const FPropertyBase& Type, const TCHAR* Tag );

	INT				EmitOuterContext();

	// Error handle
	void			SkipEndOfLine();

    // gam ---

    struct UnReferencedLocal
    {
        UProperty *property;
        INT declarationLine;

        UnReferencedLocal *next;
    };

    UnReferencedLocal *unReferencedLocals;

    // --- gam

	void PrintProgressMessage(const TCHAR* Msg);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
