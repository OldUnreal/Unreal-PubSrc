/*=============================================================================
	HTTPDownload.h: Unreal HTTP File Download
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

enum EHTTPState
{
	HTTP_Initialized		=0,
	HTTP_Resolving			=1,
	HTTP_Resolved			=2,
	HTTP_Connecting			=3,
	HTTP_ReceivingHeader		=4,
	HTTP_ReceivingData		=5,
	HTTP_Closed			=6,
};

class UHTTPDownload : public UDownload
{
	DECLARE_CLASS(UHTTPDownload,UDownload,CLASS_Transient|CLASS_Config,IpDrv);

	// Config.
	FStringNoInit	ProxyServerHost;
	INT		ProxyServerPort;

	// mh -- HTTP 301/302 support
	INT		MaxRedirection;
	INT		CurRedirectLevel;
	// -- mh

	// Variables.
	BYTE bValidatedHeader;
	BYTE		HTTPState;
	BYTE		OldHTTPState;
	SOCKADDR_IN	LocalAddr;
	SOCKADDR_IN	ServerAddr;
	SOCKET		ServerSocket;
	FResolveInfo*	ResolveInfo;
	FURL		DownloadURL;
	TArray<BYTE>	ReceivedData;
	TArray<FString>	Headers;
	FTime		ConnectStartTime;
	FTime		NextProgressTime;
	INT			Retries;
	UBOOL		bNotifiedServer, bURLResolved, bShouldRetry;

	// Constructors.
	void StaticConstructor();
	UHTTPDownload();

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );

	// UDownload Interface.
	void ReceiveFile(INT PackageIndex, UBOOL bRetry);
	void DownloadFile(); // elmuerte: used for redirection
	UBOOL TrySkipFile();
	void Tick(void);
	void DownloadDone();
	UBOOL ShouldRetry()
	{
		guardSlow(ShouldRetry);
		return bShouldRetry && (Retries < 2);
		unguardSlow;
	}

	// UHTTPDownload Interface.
	UBOOL FetchData();
	void EndConnection();
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
