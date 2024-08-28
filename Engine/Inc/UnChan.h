/*=============================================================================
	UnChan.h: Unreal datachannel class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UChannel base class.
-----------------------------------------------------------------------------*/

#define DECLARE_CHANNEL(cht,chc) \
	EChannelType GetType() const \
	{ \
		return cht; \
	} \
	UBOOL IsType(EChannelType Type) const \
	{ \
		return (Type == cht); \
	} \
	chc(UNetConnection* InConnection, INT InChIndex, UBOOL InOpenedLocally);

//
// Base class of communication channels.
//
struct ENGINE_API FChannel
{
	// Variables.
	UNetConnection*	Connection;		// Owner connection.
	UBOOL			OpenAcked;		// Whether open has been acknowledged.
	INT				Closing;		// State of the channel.
	INT             ChIndex;		// Index of this channel.
	UBOOL			OpenedLocally;	// Whether channel was opened locally or by remote.
	INT				OpenPacketId;	// Packet the spawn message was sent in.
	UBOOL			OpenTemporary;	// Opened temporarily.
	UBOOL			RecentNak;		// Recently received a Nak.
	INT				NumInRec;		// Number of packets in InRec.
	INT				NumOutRec;		// Number of packets in OutRec.
	INT				NegotiatedVer;	// Negotiated version of engine = Min(client version, server version).
	FInBunch*		InRec;			// Incoming data with queued dependencies.
	FOutBunch*		OutRec;			// Outgoing reliable unacked data.
	FLOAT			ClosingTimer;	// Expire countdown for unacked closing channel.

	FChannel*		NextChannel;
	FChannel*		PrevChannel;

	// Statics.
	static inline UBOOL IsKnownChannelType(INT Type)
	{
		return (Type >= CHTYPE_Control && Type <= CHTYPE_File);
	}
	static inline const TCHAR* GetTypeName(INT Type)
	{
		switch (Type)
		{
		case CHTYPE_None: return TEXT("None");
		case CHTYPE_Control: return TEXT("Control");
		case CHTYPE_Actor: return TEXT("Actor");
		case CHTYPE_File: return TEXT("File");
		default:
			{
				TCHAR* Res = appStaticString1024();
				appSprintf(Res, TEXT("UnknownType(%i)"), Type);
				return Res;
			}
		}
	}

	virtual EChannelType GetType() const // Type of this channel.
	{
		return CHTYPE_None;
	}
	virtual UBOOL IsType(EChannelType Type) const
	{
		return FALSE;
	}

	// Constructor.
	FChannel(UNetConnection* InConnection, INT InChIndex, UBOOL InOpenedLocally);
	void Destroy(UBOOL bCloseConnection = FALSE);

	// Destructor.
	virtual ~FChannel() = default;

	// UChannel interface.
	virtual void SetClosingFlag();
	virtual void Close();
	virtual void Describe(TCHAR* Str);
	virtual void ReceivedBunch(FInBunch& Bunch) _VF_BASE;
	virtual void Tick();
	virtual void Serialize(FArchive& Ar) {}
	virtual void ReceivedNak(INT NakPacketId);
	virtual void DereferenceActor(AActor* Other) {}

	// General channel functions.
	INT RouteDestroy();
	void ReceivedAcks();
	UBOOL ReceivedSequencedBunch( FInBunch& Bunch );
	void ReceivedRawBunch( FInBunch& Bunch );
	INT SendBunch( FOutBunch* Bunch, UBOOL Merge );
	INT IsNetReady( UBOOL Saturate );
	void AssertInSequenced();
	INT MaxSendBytes();

protected:
	virtual void OnDestroy(UBOOL bCloseConnection) _VF_BASE;

private:
	FChannel() {}
};

/*-----------------------------------------------------------------------------
	UControlChannel base class.
-----------------------------------------------------------------------------*/

//
// A channel for exchanging text.
//
struct ENGINE_API FControlChannel : public FChannel, public FOutputDevice
{
	// Constructor.
	DECLARE_CHANNEL(CHTYPE_Control, FControlChannel);

	// UChannel interface.
	void ReceivedBunch( FInBunch& Bunch );

	// FArchive interface.
	void Serialize( const TCHAR* Data, EName Event );

	// UControlChannel interface.
	void Describe(TCHAR* Str);

protected:
	void OnDestroy(UBOOL bCloseConnection);
};

/*-----------------------------------------------------------------------------
	UActorChannel.
-----------------------------------------------------------------------------*/

//
// Information for tracking retirement and retransmission of a property.
//
struct FPropertyRetirement
{
    INT    OutPacketId;		// Packet sent on, INDEX_NONE=none.
	INT    InPacketId;		// Packet received on, INDEX_NONE=none.
	BYTE   Dirty;			// Whether it needs to be resent.
	BYTE   Reliable;		// Whether it was sent reliably.
	FPropertyRetirement()
	:	OutPacketId	( INDEX_NONE )
	,	InPacketId	( INDEX_NONE )
	,	Dirty		( 0 )
	{}
};

//
// A channel for exchanging actor properties.
//
struct ENGINE_API FActorChannel : public FChannel
{
	// Variables.
	ULevel*	Level;			// Level this actor channel is associated with.
	AActor* Actor;			// Actor this corresponds to.
	UClass* ActorClass;		// Class of the actor.
	FTime	RelevantTime;	// Last time this actor was relevant to client.
	FTime	LastUpdateTime;	// Last time this actor was replicated.
	TArray<BYTE> Recent;	// Most recently sent values.
	TArray<BYTE> RepEval;	// Evaluated replication conditions.
	TArray<FPropertyRetirement> Retirement; // Property retransmission.
	TArray<INT> Dirty;	// Properties that are dirty and need resending.

	BITFIELD SpawnAcked : 1;	    // Whether spawn has been acknowledged.
	BITFIELD bBrokenChannel : 1;	// Channel networking desynced.
	BITFIELD bInitRecent : 1;		// Has initialized recent data.
	BITFIELD bNetworkedInit : 1;	// Has this actor managed to fully network net init variables yet.
	BITFIELD bNetDirty : 1;			// This actor is currently dirty!
	BITFIELD bInReplication : 1;	// Safeguard to prevent replication within replication error.
	BITFIELD bSendHeader : 1;		// Has this actors general header been sent yet?
	BITFIELD bUTJumpHack : 1;		// Hackfix for difference to JumpStatus flag in ServerMove.

	// Constructor.
	DECLARE_CHANNEL(CHTYPE_Actor, FActorChannel);

	// UChannel interface.
	void SetClosingFlag();
	void ReceivedBunch( FInBunch& Bunch );
	void Close();
	void Serialize(FArchive& Ar);
	void ReceivedNak(INT NakPacketId);
	void DereferenceActor(AActor* Other);

	// UActorChannel interface and accessors.
	inline AActor* GetActor() const {return Actor;}
	void Describe(TCHAR* Str);
	virtual void ReplicateActor();
	virtual void SetChannelActor( AActor* InActor );
	virtual void ChannelActorKilled();
	virtual void CallRPC( UFunction* Function, void* Parms, FFrame* Stack, FClassNetCache* ClassCache, FFieldNetCache* FieldCache );
	virtual void CloseTimeOut();

protected:
	void OnDestroy(UBOOL bCloseConnection);
};

struct FNetworkData
{
	AActor* Owner;
	TArray<FActorChannel*> Channels;

	FNetworkData(AActor* InActor, FActorChannel* ChOwner)
		: Owner(InActor), Channels(1)
	{
		InActor->NetworkChannels = this;
		Channels(0) = ChOwner;
	}
	~FNetworkData()
	{
		Owner->NetworkChannels = nullptr;
	}

	inline void ActorDestroyed()
	{
		guard(FNetworkData::ActorDestroyed);
		INT i;
		for (i = (Channels.Num() - 1); i >= 0; --i)
			Channels(i)->ChannelActorKilled();
		unguard;
	}
	inline void DerefCh(FActorChannel* Ch)
	{
		guardSlow(FNetworkData::DerefCh);
		if (Channels.RemoveItem(Ch) && !Channels.Num())
			delete this;
		unguardSlow;
	}
	inline void MarkDirty()
	{
		guardSlow(FNetworkData::MarkDirty);
		for (INT i = (Channels.Num() - 1); i >= 0; --i)
			Channels(i)->bNetDirty = TRUE;
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	File transfer channel.
-----------------------------------------------------------------------------*/

//
// A channel for exchanging binary files.
//
struct ENGINE_API FFileChannel : public FChannel
{
	// Receive Variables.
	UChannelDownload*	Download;		 // UDownload when receiving.

	// Send Variables.
	FArchive*			SendFileAr;		 // File being sent.
	TCHAR				SrcFilename[256];// Filename being sent.
	INT					PackageIndex;	 // Index of package in map.
	INT					SentData;		 // Number of bytes sent.
	INT					TotalData;		// Total amount of bytes to send.

	// Constructor.
	DECLARE_CHANNEL(CHTYPE_File, FFileChannel);

	// UChannel interface.
	void ReceivedBunch( FInBunch& Bunch );
	void Serialize(FArchive& Ar);

	// UFileChannel interface.
	void Describe(TCHAR* Str);
	void Tick();

protected:
	void OnDestroy(UBOOL bCloseConnection);
	UBOOL PrepareSendFile(const TCHAR* URL, const FGuid& Guid);
};

#undef DECLARE_CHANNEL

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
