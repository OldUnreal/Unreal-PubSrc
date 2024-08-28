/*=============================================================================
	UnDownload.h: Unreal file-download interface
	Copyright 1997-2000 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Jack Porter.
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

class ENGINE_API UDownload : public UObject
{
	DECLARE_ABSTRACT_CLASS(UDownload,UObject,CLASS_Transient|CLASS_Config,Engine);
    NO_DEFAULT_CONSTRUCTOR(UDownload);

	// Variables.
	UNetConnection* Connection;			// Connection
	INT				PackageIndex;		// Index of package in Map.
	FPackageInfo*	Info;				// Package Info
	FPreloadPackageList::FPreloadPackage* PreInfo;
	FString			DownloadParams;		// Download params sent to the client.
	UBOOL			UseCompression;		// Send compressed files to the client.
	FArchive*		RecvFileAr;			// File being received.
	TCHAR			TempFilename[256];	// Filename being transfered.
	TCHAR			Error[256];			// A download error occurred.
	INT				Transfered;			// Bytes transfered.
	UBOOL			SkippedFile;		// File was skipped.
	UBOOL			IsCompressed;		// Use file compression.
	INT				RealFileSize;		// elmuerte: use real file size for downloading

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );

	// UDownload Interface.
	virtual void Init(UNetConnection* InConnection, const TCHAR* Params, UBOOL bIsCompress);
	virtual UBOOL TrySkipFile();
	virtual void ReceiveFile(INT PackageIndex, UBOOL bRetry);
	virtual void ReceiveData( BYTE* Data, INT Count );
	virtual void Tick() {}
	virtual void DownloadError( const TCHAR* Error );
	virtual void DownloadDone();
	virtual UBOOL ShouldRetry() { return FALSE; }
};

class ENGINE_API UChannelDownload : public UDownload
{
	DECLARE_CLASS(UChannelDownload,UDownload,CLASS_Transient|CLASS_Config,Engine);
    NO_DEFAULT_CONSTRUCTOR(UChannelDownload);

	// Variables.
	FFileChannel* Ch;
	FString CompressDir;

	// Constructors.
	void StaticConstructor();

	// UObject interface.
	void Destroy();

	// UDownload Interface.
	void ReceiveFile(INT PackageIndex, UBOOL bRetry);
	void ReceiveData( BYTE* Data, INT Count );
	UBOOL TrySkipFile();
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
