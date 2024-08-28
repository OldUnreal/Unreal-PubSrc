/*=============================================================================
	UnEngine.h: Unreal engine definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

constexpr TCHAR GRunningFile[] = TEXT("Running.ini");

/*-----------------------------------------------------------------------------
	UServerCommandlet.
-----------------------------------------------------------------------------*/

class UServerCommandlet : public UCommandlet
{
	DECLARE_CLASS(UServerCommandlet,UCommandlet,CLASS_Transient,Engine);
	UServerCommandlet(){}
	void StaticConstructor();
	INT Main( const TCHAR* Parms );
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
