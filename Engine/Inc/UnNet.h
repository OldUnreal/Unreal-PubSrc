/*=============================================================================
	UnNet.h: Unreal networking.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Forward declarations.
-----------------------------------------------------------------------------*/

#ifndef _UNNET_H_
#define _UNNET_H_

struct	FNetworkData;
struct	FChannel;
struct	FControlChannel;
struct	FActorChannel;
struct	FFileChannel;
class	FInBunch;
class	FOutBunch;
class	UChannelIterator;
class	UNetDriver;
class	UNetConnection;
class	UPendingLevel;
class	UNetPendingLevel;
class	UDemoPlayPendingLevel;

/*-----------------------------------------------------------------------------
	Types.
-----------------------------------------------------------------------------*/

// Up to this many reliable channel bunches may be buffered.
enum {RELIABLE_BUFFER			= 128   }; // Power of 2 >= 1.
enum {MAX_PACKETID				= 65536 }; // Power of 2 >= 1, covering guaranteed loss/misorder time.
enum {MAX_CHSEQUENCE			= 65536 }; // Power of 2 >RELIABLE_BUFFER, covering loss/misorder time.
enum {MAX_PACKETID_UT			= 16384 }; // Power of 2 >= 1, covering guaranteed loss/misorder time.
enum {MAX_CHSEQUENCE_UT			= 1024  }; // Power of 2 >RELIABLE_BUFFER, covering loss/misorder time.
enum {MAX_BUNCH_HEADER_BITS		= 64    };
enum {MAX_PACKET_HEADER_BITS	= 16    };
enum {MAX_PACKET_TRAILER_BITS	= 1     };

// Return the value of Max/2 <= Value-Reference+some_integer*Max < Max/2.
inline INT BestSignedDifference( INT Value, INT Reference, INT Max )
{
	return ((Value - Reference + (Max >> 1)) & (Max - 1)) - (Max >> 1);
}
inline INT MakeRelative( INT Value, INT Reference, INT Max )
{
	return Reference + BestSignedDifference(Value,Reference,Max);
}

// Types of channels.
enum EChannelType
{
	CHTYPE_None			= 0,  // Invalid type.
	CHTYPE_Control		= 1,  // Connection control.
	CHTYPE_Actor  		= 2,  // Actor-update channel.
	CHTYPE_File         = 3,  // Binary file transfer.
	CHTYPE_MAX          = 8,  // Maximum.
};

#define HAS_ENGINE_SOCKETS 0
#if HAS_ENGINE_SOCKETS
enum ESocketSignal : DWORD
{
	SOCK_ResolveURL,
	SOCK_BindPort,
	SOCK_CreateTcp,
	SOCK_CreateUdp,
	SOCK_EndSocket,
	SOCK_WSAGetLastError,
	SOCK_TickSocket,
	SOCK_TcpListen,
	SOCK_TcpCloseClient,
	SOCK_TcpSendClient,
	SOCK_TcpSendServer,
	SOCK_TcpOpen,
	SOCK_UdpSend,
	SOCK_MAX_Signal
};
enum ETcpConnectionState : BYTE
{
	TCPSTATE_Initialized,
	TCPSTATE_Ready,
	TCPSTATE_Listening,
	TCPSTATE_Connecting,
	TCPSTATE_Connected,
	TCPSTATE_ListenClosePending,
	TCPSTATE_ConnectClosePending,
	TCPSTATE_ListenClosing,
	TCPSTATE_ConnectClosing,
};
typedef void (SocketFunc)(struct FNetConnectionBase* Soc, ESocketSignal Handle, void* MiscData);
typedef FString (GetIPAddrFunc)(const struct FNetIpAddress* Addr, UBOOL bIncludePort);
typedef UBOOL (IPAddrMatchFunc)(const struct FNetIpAddress* A, const struct FNetIpAddress* B);

struct ENGINE_API FNetIpAddress
{
	DWORD A, B;
	INT Port;

	FNetIpAddress()
		: A(0), B(0), Port(0)
	{}

	static GetIPAddrFunc* GetAddrString;
	static IPAddrMatchFunc* AddrMatches;

	inline FString GetAddr(UBOOL bIncludePort = 1) const
	{
		return (*GetAddrString)(this, bIncludePort);
	}
	inline UBOOL operator==(const FNetIpAddress& B) const
	{
		return (*AddrMatches)(this, &B);
	}
};

struct ENGINE_API FNetConnectionBase : public FTickableObjectBase
{
protected:
	FNetConnectionBase();

public:
	~FNetConnectionBase();

	void* SocketData;
	static SocketFunc* ConnectionHandle; // Note: IpDrv MUST implement this handle!

	void Tick(FLOAT DeltaTime);

	// Resolve an URL.
	void Resolve(const TCHAR* URL);
	virtual void OnResolvedURL(const FNetIpAddress& IpAddr) {}
	virtual void OnResolveFailed() {}

	// Get last WinSock error.
	static INT GetLastError();

	// Bind this connection to a port.
	INT BindPort(INT Port = 0, UBOOL bNextAvailable = 0);

protected:
	inline void CallAPI(ESocketSignal Msg, void* Data)
	{
		guard(CallSocketAPI);
		(*ConnectionHandle)(this, Msg, Data);
		unguardf((TEXT("(%i)"), (INT)Msg));
	}
};
struct ENGINE_API FTcpNetConnection : public FNetConnectionBase
{
	TArray<FNetIpAddress> Clients; // Currently open client connections.
	FNetIpAddress Server; // Currently open server connection.
	ETcpConnectionState State;

	FTcpNetConnection();
	~FTcpNetConnection();

	// Listen for connections.
	UBOOL Listen();

	// Should accept this connection?
	virtual UBOOL AcceptConnection(const FNetIpAddress& IpAddr)
	{
		return 1;
	}

	// Close an connection from Clients list.
	UBOOL CloseConnection(INT Index);
	virtual void ClientClosed(const FNetIpAddress& IpAddr) {}

	// Send or receive data from/to a client.
	virtual void OnReceiveClientData(INT Index, BYTE* Data, INT DataSize) {}
	void SendClientData(INT Index, BYTE* Data, INT DataSize);

	// Open an Tcp connection.
	UBOOL Open(const FNetIpAddress& DestAddr);
	virtual void Opened() {}
	virtual void Closed() {}

	// Send/receive data from server connection.
	virtual void OnReceiveServerData(BYTE* Data, INT DataSize) {}
	void SendServerData(BYTE* Data, INT DataSize);

	// TcpConnection state was changed.
	virtual void StateChanged() {}

	inline void ChangeState(ETcpConnectionState NewState)
	{
		State = NewState;
		StateChanged();
	}
};
struct ENGINE_API FUdpNetConnection : public FNetConnectionBase
{
	FUdpNetConnection();
	~FUdpNetConnection();

	virtual void OnReceiveData(const FNetIpAddress& IpAddr, BYTE* Data, INT DataSize) _VF_BASE;
	void SendData(const FNetIpAddress& IpAddr, BYTE* Data, INT DataSize);
};
#endif

/*-----------------------------------------------------------------------------
	Includes.
-----------------------------------------------------------------------------*/

#include "UnNetDrv.h"		// Network driver class.
#include "UnBunch.h"		// Bunch class.
#include "UnDownload.h"		// Autodownloading classes.
#include "UnConn.h"			// Connection class.
#include "UnChan.h"			// Channel class.
#include "UnPenLev.h"		// Pending levels.
#include "UnDemoPenLev.h"	// Demo playback pending level
#include "UnDemoRec.h"		// Demo recording classes.

#endif
/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
