/*=============================================================================
	UnPenLev.h: Unreal pending level definition.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UPendingLevel.
-----------------------------------------------------------------------------*/

//
// Class controlling a pending game level.
//
class UPendingLevel : public ULevelBase
{
	DECLARE_ABSTRACT_CLASS(UPendingLevel,UObject,0,Engine)
	NO_DEFAULT_CONSTRUCTOR(UPendingLevel)

	// Variables.
	UBOOL		Success;
	UBOOL		SentJoin;
	UBOOL		LonePlayer;
	UBOOL		EnableScriptHooks;
	INT			FilesNeeded;
	FString		Error, FailCode;
	UBOOL		AlreadyErrorRecieved; //make sure not to override a failure message with connection failed - Smirftsch
	UBOOL		bMirrorLevel;
	FString		PreloaderName;
	UClientPreloginScene* LoadingScene;

	// Constructors.
	UPendingLevel( UEngine* InEngine, const FURL& InURL );

	// UPendingLevel interface.
	virtual void Tick( FLOAT DeltaTime ) _VF_BASE;
	virtual UNetDriver* GetDriver() _VF_BASE_RET(NULL);
	virtual UBOOL TrySkipFile() _VF_BASE_RET(0);
	UBOOL Exec(const TCHAR* Cmd, FOutputDevice& Ar = *GLog);
};

/*-----------------------------------------------------------------------------
	UNetPendingLevel.
-----------------------------------------------------------------------------*/

class UNetPendingLevel : public UPendingLevel
{
	DECLARE_CLASS(UNetPendingLevel, UPendingLevel, CLASS_Transient, Engine);
	NO_DEFAULT_CONSTRUCTOR(UNetPendingLevel);

	// Client download managers.
	struct FDownloadInfo
	{
		UClass* Class;
		FString Params;
		UDownload* Object;
		UBOOL Compression;

		FDownloadInfo(UClass* InClass, const TCHAR* InParms, const UBOOL InCompress)
			: Class(InClass), Params(InParms), Object(nullptr), Compression(InCompress)
		{}
	};
	TArray<FDownloadInfo>	DownloadInfo;
	INT						CurrentManager;		// Currently used download manager on client.
	UBOOL					bDownloadRetry;		// Retrying previous download manager.

	// Constructors.
	UNetPendingLevel( UEngine* InEngine, const FURL& InURL );
	void Destroy();
	void Serialize(FArchive& Ar);

	// FNetworkNotify interface.
	EAcceptConnection NotifyAcceptingConnection();
	void NotifyAcceptedConnection( class UNetConnection* Connection );
	UBOOL NotifyAcceptingChannel( struct FChannel* Channel );
	ULevel* NotifyGetLevel();
	class UClientPreloginScene* NotifyGetClientScene();
	void NotifyReceivedText( UNetConnection* Connection, const TCHAR* Text );
	void NotifyReceivedFile( UNetConnection* Connection, INT PackageIndex, const TCHAR* Error, UBOOL Skipped );
	UBOOL NotifySendingFile( UNetConnection* Connection, FGuid Guid );

	// UPendingLevel interface.
	void Tick( FLOAT DeltaTime );
	UNetDriver* GetDriver() { return NetDriver; }
	UBOOL TrySkipFile();

	// UNetPendingLevel interface
	void ReceiveNextFile( UNetConnection* Connection );
	UDownload* GetNextDownloader(UNetConnection* Connection);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
