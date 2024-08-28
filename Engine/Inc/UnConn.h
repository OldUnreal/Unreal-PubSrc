/*=============================================================================
	UnConn.h: Unreal network connection base class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

class UNetDriver;

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UNetConnection.
-----------------------------------------------------------------------------*/

//
// Whether to support net lag and packet loss testing.
//
#define DO_ENABLE_NET_TEST 1

//
// State of a connection.
//
enum EConnectionState
{
	USOCK_Invalid   = 0, // Connection is invalid, possibly uninitialized.
	USOCK_Closed    = 1, // Connection permanently closed.
	USOCK_Pending	= 2, // Connection is awaiting connection.
	USOCK_Open      = 3, // Connection is open.
};

// Game verification state.
enum EClientVerState
{
	CLIENT_Unverified,
	CLIENT_Unreal,
	CLIENT_UnrealTournament,
};

#if DO_ENABLE_NET_TEST
//
// A lagged packet
//
struct FDelayedPacket
{
	TArray<BYTE> Data;
	FTime SendTime;

	FDelayedPacket(const BYTE* DataStream, const INT DataSize, const FTime& TransmitTime)
		: Data(DataSize), SendTime(TransmitTime)
	{
		appMemcpy(&Data(0), DataStream, DataSize);
	}
};
#endif

//
// A network connection.
//
class ENGINE_API UNetConnection : public UPlayer
{
	DECLARE_ABSTRACT_CLASS(UNetConnection,UPlayer,CLASS_Transient|CLASS_Config,Engine)

	// Constants.
	enum{ MAX_PROTOCOL_VERSION = 1     };	// Maximum protocol version supported.
	enum{ MIN_PROTOCOL_VERSION = 1     };	// Minimum protocol version supported.
	enum{ DEFAULT_MAX_CHANNELS = 1023  };	// Maximum channels.

	// Connection information.
	UNetDriver*		Driver;					// Owning driver.
	EConnectionState State;					// State this connection is in.
	FURL			URL;					// URL of the other side.
	UPackageMap*	PackageMap;				// Package map between local and remote.

	// Negotiated parameters.
	INT				ProtocolVersion;		// Protocol version we're communicating with (<=PROTOCOL_VERSION).
	INT				RemoteVersion;			// The remote engine version number
	INT				MaxPacket;				// Maximum packet size.
	INT				PacketOverhead;			// Bytes overhead per packet sent.
	UBOOL			InternalAck;			// Internally ack all packets, for 100% reliable connections.
	INT				ConnectionNetSpeed;		// Maximum bytes per second.
	INT				Challenge;				// Server-generated challenge.
	INT				NegotiatedVer;			// Negotiated version for new channels.
	INT				NegotiatedSubVer;		// Negotiated subversion for new channels.
	INT				UserFlags;				// User-specified flags.
	FString			RequestURL;				// URL requested by client
	INT				NewNetworkVer;			// (227j) Network communtication method.
	DWORD			MaxChannels;			// (227j) Maximum network channels currently in use.

	// Internal.
	FTime			LastReceiveTime;		// Last time a packet was received, for timeout checking.
	FTime			LastSendTime;			// Last time a packet was sent, for keepalives.
	FTime			LastTickTime;			// Last time of polling.
	FTime			LastRepTime;			// Time of last replication.
	INT				QueuedBytes;			// Bytes assumed to be queued up.
	INT				TickCount;				// Count of ticks.
	UBOOL			PassedChallenge;		// Whether client has passed the challenge/response mechanism. // elmuerte: jack_porter security fix
	INT				MaxLevelID;				// For Multi-Map sub-level map, maximum bits reserved for level ID (0 if old NewNetID).
	DWORD			NextChannelSlot;		// Next slot to use when opening new channel on server.
	FLOAT			TimeoutSeconds;			// (227k) Timeout time for this connection.
	EClientVerState ClientVerState;			// (227k) Client game.

	// Merge info.
	FBitWriterMark  LastStart;				// Most recently sent bunch start.
	FBitWriterMark  LastEnd;				// Most recently sent bunch end.
	UBOOL			AllowMerge;				// Whether to allow merging.
	UBOOL			TimeSensitive;			// Whether contents are time-sensitive.
	FOutBunch*		LastOutBunch;			// Most recent outgoing bunch.
	FOutBunch		LastOut;

	// Stat display.
	FTime			StatUpdateTime;			// Time of last stat update.
	FLOAT			StatPeriod;				// Interval between gathering stats.
	FLOAT			InRate,    OutRate;		// Rate for last interval.
	FLOAT			InPackets, OutPackets;	// Packet counts.
	FLOAT			InBunches, OutBunches;	// Bunch counts.
	FLOAT			InLoss,    OutLoss;		// Packet loss percent.
	FLOAT			InOrder,   OutOrder;	// Out of order incoming packets.
	FLOAT			BestLag;				// Lag.

	// Stat accumulators.
	FLOAT			LagAcc;					// Previous msec lag.
	INT				InLossAcc, OutLossAcc;	// Packet loss accumulator.
	INT				InPktAcc,  OutPktAcc;	// Packet accumulator.
	INT				InBunAcc,  OutBunAcc;	// Bunch accumulator.
	INT				InByteAcc, OutByteAcc;	// Byte accumulator.
	INT				InOrdAcc,  OutOrdAcc;	// Out of order accumulator.
	INT				HighLossCount;			// Counts high packet loss.

	// Packet.
	FBitWriter		Out;					// Outgoing packet.
	FTime			OutLagTime[256];		// For lag measuring.
	INT				OutLagPacketId[256];	// For lag measuring.
	INT				InPacketId;				// Full incoming packet index.
	INT				OutPacketId;			// Most recently sent packet.
	INT 			OutAckPacketId;			// Most recently acked outgoing packet.

	// Shambler: RPC flood detection
	INT				TickRPCNum;				// Number of RPC's during the current tick.
	FLOAT			RPCTimestamp;			// Timestamp representing the time that counting started.
	INT				TimedRPCCount;			// Total number of RPC's counted sofar.

	// Shambler: Advanced UScript DoS attack profiling
	void AddToRPCCache	( const TCHAR* Msg );
	TMap<UFunction*,INT> RPCList;
	TArray<FString>	RPCNames;				// List of cached RPCNames.
	INT				LastRPCName;			// Index of the last RPCName to be added to the list.

	FString			ComputerName;			// Remote ComputerName for banning.
	FString			ComputerIdent;			// Remote ComputerMac for banning.
	FString			Aliases;				// Remote client aliases.
	FString			UserName;				// Remote client user name on login.
	FString			DisconnectReason;		// Reason this client is disconnecting.

	UBOOL			bVerified;				// Make sure handshake isn't bypassed.

	// HTTP download information (serverside)
	FString			DownloadURL;
	INT				DownloadSize, DownloadProgress;

	// Acknowledgement tables.
	BYTE  OutAcked[ MAX_PACKETID/8 ], OutNaked[ MAX_PACKETID/8 ];
	UBOOL GetOutBit  ( BYTE* Tbl, INT i ) {return (Tbl[(i>>3)&(MAX_PACKETID/8-1)]  &  (1<<(i&7)))!=0;}
	void  SetOutBit  ( BYTE* Tbl, INT i ) {        Tbl[(i>>3)&(MAX_PACKETID/8-1)] |=  (1<<(i&7));    }
	void  ClearOutBit( BYTE* Tbl, INT i ) {        Tbl[(i>>3)&(MAX_PACKETID/8-1)] &= ~(1<<(i&7));    }

	// Channel table.
	FChannel**					Channels;
	INT*						OutReliable;
	INT*						InReliable;
	FChannel*					ChannelList; // Allocated channels for this client.
	TMap<AActor*,FActorChannel*> ActorChannels;
	TSingleMap<AActor*>			SentTemporaries;
	TMap<AActor*,TArray<BYTE>>	RecentHistory;
	FControlChannel*			MainChannel;

	// File Download
	UDownload*				Download;

	// Diagnostic mode.
	struct FClientVerification* ClientVerify;

#if DO_ENABLE_NET_TEST
	// For development.
	INT				PktLoss;
	INT				PktOrder;
	INT				PktDup;
	INT				PktLag;
	TArray<FDelayedPacket> Delayed;
  #endif

	// Constructors and destructors.
	UNetConnection();
	UNetConnection( UNetDriver* Driver, const FURL& InURL );

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	UBOOL AllowScriptDestroy()
	{
		return FALSE;
	}

	// UPlayer interface.
	void ReadInput( FLOAT DeltaSeconds );
	UBOOL AllowDestroyPlayer();

	// FArchive interface.
	void Serialize( const TCHAR* Data, EName MsgType );

	// FExec interface.
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );

	// UNetConnection interface.
	virtual FString LowLevelGetRemoteAddress(UBOOL bIncludePort = TRUE) = 0;
	virtual FString LowLevelDescribe()=0;
	virtual void LowLevelSend( void* Data, INT Count )=0; //!! "Looks like an FArchive"
	virtual void InitOut();
	virtual void AssertValid();
	virtual void SendAck( INT PacketId );
	virtual void FlushNet();
	virtual void Tick();
	virtual INT IsNetReady( UBOOL Saturate );

	// Functions.
	void SendPackageMap();
	virtual void PreSend( INT SizeBits );
	virtual void PostSend();
	virtual void ReceivedRawPacket( void* Data, INT Count );//!! "looks like an FArchive"
	virtual INT SendRawBunch( FOutBunch& Bunch, UBOOL InAllowMerge );
	UNetDriver* GetDriver() {return Driver;}
	FChannel* CreateChannel( enum EChannelType Type, UBOOL bOpenedLocally, INT ChannelIndex=INDEX_NONE );
	void ReceivedPacket( FBitReader& Reader );
	void ReceiveFile(INT PackageIndex, UDownload* Downloader, UBOOL bRetry);
	void SlowAssertValid()
	{
#if DO_GUARD_SLOW
		AssertValid();
#endif
	}
	void KickConnection(const TCHAR* Reason = TEXT("Kicked"));
	virtual UBOOL IsDemoRecording() const;
	inline UBOOL IsUnrealTournamentClient() const
	{
		return (ClientVerState == CLIENT_UnrealTournament);
	}

	// Shambler: Flood detection
	UBOOL RPCFloodDetect(UBOOL bTimed=0);	// Returns true if the rest of the packet should be dropped
	UBOOL CheckRPCFlood  ( UFunction* F );

	// 227j verify network integrity of clients.
	void VerifyNetworkClient();
	void VerifyResponse(const TCHAR* S);

	// Actor channel size control.
	void SetMaxChannels(DWORD NewSize);

	static DWORD GetValidMaxChannels(DWORD InCh)
	{
		return Max<DWORD>(FNextPowerOfTwo(InCh), 256) - 1;
	}
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
