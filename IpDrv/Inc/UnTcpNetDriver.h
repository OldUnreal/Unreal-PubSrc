/*=============================================================================
	UnTcpNetDriver.h: Unreal TCP/IP driver.
	Copyright 1997-2000 Epic Games, Inc. All Rights Reserved.

Revision history:
	* Created by Brandon Reinhart.
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UTcpipConnection.
-----------------------------------------------------------------------------*/

class UTcpNetDriver;

//
// Windows socket class.
//
class DLL_EXPORT_CLASS UTcpipConnection : public UNetConnection
{
	DECLARE_CLASS(UTcpipConnection,UNetConnection,CLASS_Config|CLASS_Transient,IpDrv)
	NO_DEFAULT_CONSTRUCTOR(UTcpipConnection)

	// Variables.
	sockaddr_in		RemoteAddr;
	SOCKET			Socket;
	UBOOL			OpenedLocally;
	FResolveInfo*	ResolveInfo;
	FTime			OpenedTime; //elmuerte: jack_porter security fix

	// Constructors and destructors.
	UTcpipConnection( SOCKET InSocket, UTcpNetDriver* InDriver, const sockaddr_in InRemoteAddr, EConnectionState InState, UBOOL InOpenedLocally, const FURL& InURL );
	void Destroy();

	void LowLevelSend( void* Data, INT Count );
	FString LowLevelGetRemoteAddress(UBOOL bIncludePort);
	FString LowLevelDescribe();
	void Tick();

	inline UTcpNetDriver* GetTcpDriver() const
	{
		return reinterpret_cast<UTcpNetDriver*>(Driver);
	}
};

/*-----------------------------------------------------------------------------
	UTcpNetDriver.
-----------------------------------------------------------------------------*/

struct FRecentInfo
{
	sockaddr_in ClientAddr;
	FTime ClosedTime;

	FRecentInfo(const sockaddr_in& a, const FTime& CloseTime)
		: ClientAddr(a), ClosedTime(CloseTime)
	{}
};
struct FLoginTrack
{
	FTime ExpireTimer;
	INT LoginCount;

	FLoginTrack(FTime EndTime)
		: ExpireTimer(EndTime), LoginCount(1)
	{}
};

#ifdef _WINDOWS
#define USE_CONNECTION_MAP 1
#else
#define USE_CONNECTION_MAP 1
#endif

//
// Windows sockets network driver.
//
class DLL_EXPORT_CLASS UTcpNetDriver : public UNetDriver, public FOutputDevice
{
	DECLARE_CLASS(UTcpNetDriver,UNetDriver,CLASS_Transient|CLASS_Config,IpDrv)

	// Variables.
	sockaddr_in	LocalAddr;
	SOCKET		Socket;
	UBOOL OverrideBufferAllocation;
	INT MaxConnPerIPPerMinute,MaxConnPerIP;   // elmuerte: jack_porter security fix
	UBOOL LogMaxConnPerIPPerMin; // elmuerte: jack_porter security fix
	UBOOL BanMaxConnectionOverflow;

	TMap<in_addr, FLoginTrack> LoginTracker;
	TSingleMap<in_addr> RJAClients;

#ifdef __GNUG__
	FLOAT RecvSizeMult;
	FLOAT SendSizeMult;
#endif
	UBOOL AllowPlayerPortUnreach;
	UBOOL LogPortUnreach;

	TArray<FRecentInfo> RecentClosed; // Keep track of recently closed sockets for 5 seconds to avoid reopening them with acks
#if USE_CONNECTION_MAP
	TMap<sockaddr_in, UTcpipConnection*> ClientsMap;
#endif

	// Constructor.
	UTcpNetDriver()
	{}

	// FOutputDevice
	void Serialize(const TCHAR* V, EName Event);

	// UObject interface.
	void Serialize( FArchive& Ar );
	void StaticConstructor();

	// UNetDriver interface.
	UBOOL InitConnect( FNetworkNotify* InNotify, FURL& ConnectURL, FString& Error );
	UBOOL InitListen( FNetworkNotify* InNotify, FURL& LocalURL, FString& Error );
	void TickDispatch( FLOAT DeltaTime );
	FString LowLevelGetNetworkNumber();
	void LowLevelDestroy();

	// UTcpNetDriver interface.
	UBOOL InitBase( UBOOL Connect, FNetworkNotify* InNotify, FURL& URL, FString& Error );
	inline UTcpipConnection* GetServerConnection() const
	{
		return reinterpret_cast<UTcpipConnection*>(ServerConnection);
	}
	void SendCustomMessage(const FString& Str, const FIpAddr& IpAddr) const;
	UTcpipConnection* OpenConnection(const sockaddr_in& IpAddr);
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif
