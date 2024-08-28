/*=============================================================================
	UnIpDrvCommandlets.h: IpDrv package commandlet declarations.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

Revision history:
	* (4/14/99) Created by Brandon Reinhart
	* (4/15/99) Ported to UCC Commandlet interface by Brandon Reinhart

Todo:
	* Gspy style interface could be expanded to complete compliance
	  (allows for custom queries)
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

#define DO_VERIFY_SERVER 0 // Marco: Old version compatibility, they don't respond to verification process...

struct FServerEntry
{
	sockaddr_in serverAddr;
	FString IpAdr;
	INT CommPort, QueryPort, GamePort, PollTime, NewVerify;

	FServerEntry(const sockaddr_in& addr, const FString& IP, INT qp, INT pt)
		: serverAddr(addr), IpAdr(IP), CommPort(qp), QueryPort(qp), GamePort(INDEX_NONE), PollTime(pt), NewVerify(appRand())
	{}
	inline UBOOL HasGamePort() const
	{
		return GamePort != INDEX_NONE;
	}
};

struct FValidationEntry
{
	FString ValidKey;
	INT QueryPort;

	FValidationEntry(const FString& Key, INT qp)
		: ValidKey(Key), QueryPort(qp)
	{}
};

enum EOPMode : BYTE
{
	OP_TcpLink,
	OP_TextFile,
	OP_Undefined,
};
inline const TCHAR* OpToString(EOPMode Mode)
{
	switch (Mode)
	{
	case OP_TcpLink:
		return TEXT("TcpLink");
	case OP_TextFile:
		return TEXT("TextFile");
	default:
		return TEXT("No Output");
	}
}

struct FClientConnection
{
	SOCKET Socket;
	FString Addr;
	INT Timeout;

	FClientConnection* Last;
	FClientConnection* Next;

	FClientConnection(SOCKET InSocket, const FString& RemoteAddr, FClientConnection* List)
		: Socket(InSocket), Addr(RemoteAddr), Timeout(0), Last(nullptr), Next(List)
	{
		if (List)
			List->Last = this;
	}
	~FClientConnection() noexcept(false);
};

class UMasterServerCommandlet : public UCommandlet
{
	DECLARE_CLASS(UMasterServerCommandlet, UCommandlet, CLASS_Transient, IpDrv);

	FSocketData ListenSocket;
	FOperationStats OpStats, OldStats;

	// Output
	FString GameName;
	EOPMode OpMode;

	// TextFile Mode
	FString OutputFileName;

	// TCPLink Mode
	INT TCPPort;
	FSocketData TCPSocket;
	FClientConnection* Connections;

	// Server Map
#if DO_VERIFY_SERVER
	TMap<FString, FValidationEntry*> ValidationMap;	// Servers awaiting validation.
	TMap<FString, INT> ValidationIP;
#endif
	TMap<FString, FServerEntry*> MasterMaps;		// Servers in the master list
	FString IPInvalidPortID;
	FString IPCompFromAddr;
	INT NumServers;
	FTime Next10Seconds, Next300Seconds;
	INT IgnoredValidations;
	INT RejectedValidations;

	UMasterServerCommandlet();

	void Destroy();

	void InitSockets( const TCHAR* ConfigFileName );
	UBOOL ConsoleReadInput();
	void ListenSockets();
	void ServiceMessage( const TCHAR* Message, sockaddr_in* FromAddr );
	void DoHeartbeat(sockaddr_in* FromAddr, INT PortID);
#if DO_VERIFY_SERVER
	void DoValidate(sockaddr_in* FromAddr, FString& ChallengeResponse);
	void PurgeValidationMap();
#endif
	void AssignPortNum(sockaddr_in* FromAddr, INT Challenge, UBOOL bQuery);
	void PollConnections();
	void ServiceTCPMessage(const TCHAR* Message, FClientConnection* C);
	void PurgeMasterMap();
	void WriteMasterMap();
	void CleanUp();
	void CloseConnection(FClientConnection* C);
	INT Main( const TCHAR* Parms );
};

class UUpdateServerCommandlet : public UCommandlet
{
	DECLARE_CLASS(UUpdateServerCommandlet, UCommandlet, CLASS_Transient,IpDrv);

	// System
	FSocketData Socket;
	FOperationStats OpStats;

	// Key/Response
	TMap<FString, FString> Pairs;

	FArchive* Log;

	UBOOL GetNextKey( FString* Message, FString* Result );
	INT SendResponse( FString Key, sockaddr_in* FromAddr );
	FString GetIpAddress( sockaddr_in* FromAddr );
	void ServiceMessage( FString Message, sockaddr_in* FromAddr );
	void InitSockets( const TCHAR* ConfigFileName );
	UBOOL ConsoleReadInput( const TCHAR* ConfigFileName );
	void ListenSockets();
	void ReadKeyResponses( const TCHAR* ConfigFileName );
	void CleanUp();
	void ReloadSettings( const TCHAR* ConfigFileName );
	INT Main( const TCHAR* Parms );
};

class UCompressCommandlet : public UCommandlet
{
	DECLARE_CLASS(UCompressCommandlet, UCommandlet, CLASS_Transient,IpDrv);

	INT Main( const TCHAR* Parms );
};

class UDecompressCommandlet : public UCommandlet
{
	DECLARE_CLASS(UDecompressCommandlet, UCommandlet, CLASS_Transient,IpDrv);

	INT Main( const TCHAR* Parms );
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
