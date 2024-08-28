/*=============================================================================
	UnDemoPenLev.h: Unreal demo recording.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UDemoPlayPendingLevel.
-----------------------------------------------------------------------------*/

//
// Class controlling a pending demo playback level.
//
class UDemoPlayPendingLevel : public UPendingLevel
{
	DECLARE_CLASS(UDemoPlayPendingLevel,UPendingLevel,CLASS_Transient,Engine)
	NO_DEFAULT_CONSTRUCTOR(UDemoPlayPendingLevel)

	// Constructors.
	UDemoPlayPendingLevel( UEngine* InEngine, const FURL& InURL );

	// FNetworkNotify interface.
	EAcceptConnection NotifyAcceptingConnection() { return ACCEPTC_Reject; }
	void NotifyAcceptedConnection( class UNetConnection* Connection ) {}
	UBOOL NotifyAcceptingChannel( struct FChannel* Channel ) { return 1; }
	ULevel* NotifyGetLevel();
	void NotifyReceivedText( UNetConnection* Connection, const TCHAR* Text );
	void NotifyReceivedFile( UNetConnection* Connection, INT PackageIndex, const TCHAR* Error, UBOOL Skipped ) {}
	UBOOL NotifySendingFile( UNetConnection* Connection, FGuid GUID ) {return 0;}

	// UPendingLevel interface.
	void Tick( FLOAT DeltaTime );
	UNetDriver* GetDriver() { return DemoRecDriver; }
	UBOOL TrySkipFile() { return 0; }
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/


