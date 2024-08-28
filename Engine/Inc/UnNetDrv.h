/*=============================================================================
	UnNetDrv.h: Unreal network driver base class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _UNNETDRV_H_
#define _UNNETDRV_H_

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UPackageMapLevel.
-----------------------------------------------------------------------------*/

struct FPackageGenInfo;

class ENGINE_API UPackageMapLevel : public UPackageMap
{
	DECLARE_CLASS(UPackageMapLevel,UPackageMap,CLASS_Transient,Engine);
	UNetConnection* Connection;
	FActorChannel* RecentChannel{}; // Recent channel were reciving data for (for Forged object warnings).
	INT ForgedCount{};

	UBOOL CanSerializeObject( UObject* Obj );
	UBOOL SerializeObject( FArchive& Ar, UClass* Class, UObject*& Obj );
	UPackageMapLevel()
	{}
	UPackageMapLevel( UNetConnection* InConnection )
	: Connection( InConnection )
	{}
};

class ENGINE_API UPackageMapUTLevel : public UPackageMapLevel
{
	DECLARE_CLASS(UPackageMapUTLevel, UPackageMapLevel, CLASS_Transient, Engine);
	NO_DEFAULT_CONSTRUCTOR(UPackageMapUTLevel);

	TArray<FPackageGenInfo*> UTList;
	TMap<DWORD, INT> ObjToIntMap;
	TMap<INT, INT> IntToObjMap;
	UNetDriver* Driver;
	UBOOL IsMasterList;

	// UObject interface.
	void Destroy();
	void Serialize(FArchive& Ar);

	// UPackageMap interface.
	INT AddLinker(ULinkerLoad* Linker);
	void Copy(UPackageMap* Other);
	void Compute();
	INT ObjectToIndex(UObject* Object);
	UObject* IndexToObject(INT Index, UBOOL Load);
	FClassNetCache* GetClassNetCache(UClass* Class);
	void ChangeGeneration(const INT PackageIndex, const INT NewGeneration);

	UPackageMapUTLevel(UNetDriver* InDriver, UPackageMap* Src);
	UPackageMapUTLevel(UNetConnection* InConnection, UPackageMapUTLevel* Master);
};

// Preload package manager.
struct ENGINE_API FPreloadPackageList
{
	struct FPreloadPackage
	{
		UPackage* Package;
		ULinkerLoad* Linker;
		FGuid GUID;
		FString FileGUID, Filename, URL;
		INT Filesize;
		UBOOL bNeedsDownload;

		FPreloadPackage(UPackage* P, ULinkerLoad* L);
		FPreloadPackage(const TCHAR* P, const FGuid& G, INT fz);
	};
	TArray<FPreloadPackage> List;

	void AddPackage(ULinkerLoad* L);
	void SendPackageList(UNetConnection* C);
};

/*-----------------------------------------------------------------------------
	UNetDriver.
-----------------------------------------------------------------------------*/

//
// Base class of a network driver attached to an active or pending level.
//
class ENGINE_API UNetDriver : public USubsystem
{
	DECLARE_ABSTRACT_CLASS(UNetDriver,USubsystem,CLASS_Transient|CLASS_Config,Engine)

	// Variables.
	TArray<UNetConnection*>		ClientConnections;
	UNetConnection*				ServerConnection;
	FNetworkNotify*				Notify;
	UPackageMap*				MasterMap;
	UPackageMapUTLevel*			MasterMapUT;
	FPreloadPackageList*		PreloadList;
	FTime 						Time;
	FLOAT						ConnectionTimeout;
	FLOAT						InitialConnectTimeout;
	FLOAT						PreLoginTimeout;
	FLOAT						KeepAliveTime;
	FLOAT						RelevantTimeout;
	FLOAT						SpawnPrioritySeconds;
	FLOAT						ServerTravelPause;
	INT							MaxClientRate;
	INT							NetServerMaxTickRate;
	INT							LanServerMaxTickRate;
	UBOOL						AllowDownloads;
	UBOOL						AllowOldClients;
	UBOOL						AllowUTClients;
	UBOOL						OldClientCompatMode;
	UBOOL						AllowFastDownload;
	INT							DownloadSpeedLimit;
	//UBOOL						CorruptedPackageCheck;
	UBOOL						ProfileStats;
	UProperty*					RoleProperty;
	UProperty*					RemoteRoleProperty;
	INT							SendCycles, RecvCycles;
	INT							MaxDownloadSize;
	INT							HibernateSeconds;
	INT							DeadlockSeconds;
	UBOOL						ServerHibernating;
	FLOAT						ServerHibernateTime;
	TArrayNoInit<FString>		DownloadManagers;
	UBOOL						UseTransientNames;
	TArray<FString>				InitDLManagers;
	TArrayNoInit<FString>		CompatPaths;
	INT							LanModeClients;
	UBOOL						GameStateDirty; // Needs to break out of processing packets loop now.
	TMap<FString, FString>*		CompPackageMap;

	// Shambler: RPC flood detection variables
	INT							MaxRPCPerTick;
	INT							MaxRepRPCPerTick;
	FLOAT						TimedRPCDuration;
	INT							MaxTimedRPC;
	UBOOL						bCacheRPCNames;
	INT							MaxCachedRPCNames;
	TArrayNoInit<FName>			ExcludedRPCNames;

	// Marco: Server has script hooks enabled.
	UBOOL bHasScriptHooks;

	// Constructors.
	UNetDriver();
	void StaticConstructor();

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );

	// UNetDriver interface.
	virtual void LowLevelDestroy() _VF_BASE;
	virtual FString LowLevelGetNetworkNumber() _VF_BASE_RET(E_NoInit);
	virtual void AssertValid();
	virtual UBOOL InitConnect( FNetworkNotify* InNotify, FURL& ConnectURL, FString& Error );
	virtual UBOOL InitListen( FNetworkNotify* InNotify, FURL& ListenURL, FString& Error );
	virtual void TickFlush();
	virtual void TickDispatch( FLOAT DeltaTime );
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	virtual void NotifyActorDestroyed( AActor* Actor );
	virtual void ReconnectClients();
	virtual void AddPreloadPackage(const TCHAR* PackageName);
	const TCHAR* FindCompatPackage(const TCHAR* PackageName);
	UBOOL ShouldHibernate(FLOAT DeltaTime);

	TArray<FString>& DownloadGetManagers();
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
