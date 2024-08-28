/*=============================================================================
	UnPlayer.h: Unreal player class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UPlayer.
-----------------------------------------------------------------------------*/

//
// A player, the base class of UViewport (local players) and UNetConnection (remote players).
//
class ENGINE_API UPlayer : public UObject, public FOutputDevice, public FExec
{
	DECLARE_ABSTRACT_CLASS(UPlayer,UObject,CLASS_Transient|CLASS_Config,Engine)

	// Objects.
	APlayerPawn*	Actor;
	UConsole*		Console;

	BITFIELD		bWindowsMouseAvailable:1;
	BITFIELD		bShowWindowsMouse:1;
	FLOAT			WindowsMouseX;
	FLOAT			WindowsMouseY;
	BYTE			SelectedCursor;
	class UUnrealCursor* CustomCursor;

	// Constructor.
	UPlayer();

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	// FExec interface.
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar );

	// UPlayer interface.
	virtual void ReadInput( FLOAT DeltaSeconds ) _VF_BASE;
	virtual void NotifyLevelChange();
	virtual UBOOL AllowDestroyPlayer();
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

struct FCursorData
{
	FCursorData(){}
	virtual ~FCursorData() noexcept(false) {}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
