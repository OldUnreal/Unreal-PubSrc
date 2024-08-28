	enum EActorAction
	{
		NOTE_ActorSpawned,
		NOTE_ActorRedo,
		NOTE_ActorDestroyed,
		NOTE_ActorPropertiesChange,
	};
	
	struct FSavedGameDetails
	{
		FSavedGameInfo Info;
		INT Stamp, Index;
		BYTE bTag;
		FSavedGameDetails()
			: Stamp(0),
			Index(0),
			bTag(0)
		{}
	};

	// Static variables.
	static class FLightManagerBase* GLightManager;
	static UEngine* GEngine;
private:
	static TArray<FCreditsInfo>* GPendingCredits;
public:

	// Constructors.
	UEngine();

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();

	// UEngine interface.
	virtual void Init();
	virtual void Exit();
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Out=*GLog );
	virtual void Flush( UBOOL AllowPrecache );
	virtual UBOOL Key( UViewport* Viewport, EInputKey Key, INT KeyValue );
	virtual UBOOL TypingEvent( UViewport* Viewport );
	virtual UBOOL InputEvent( UViewport* Viewport, EInputKey iKey, EInputAction State, FLOAT Delta=0.f );
	virtual INT ChallengeResponse( INT Challenge );
	virtual FLOAT GetMaxTickRate();
	virtual void SetProgress( const TCHAR* Str1, const TCHAR* Str2, FLOAT Seconds );

	virtual void Tick(FLOAT DeltaSeconds) {}
	virtual void Draw( UViewport* Viewport, UBOOL Blit=1, BYTE* HitData=NULL, INT* HitSize=NULL ) {}
	virtual void MouseDelta( UViewport* Viewport, DWORD Buttons, FLOAT DX, FLOAT DY ) {}
	virtual void MousePosition( UViewport* Viewport, DWORD Buttons, FLOAT X, FLOAT Y ) {}
	virtual void Click( UViewport* Viewport, DWORD Buttons, FLOAT X, FLOAT Y ) {}
	virtual void SetClientTravel( UPlayer* Viewport, const TCHAR* NextURL, UBOOL bItems, ETravelType TravelType ) {}
	void InitAudio();
	static void GetSavedGames(TArray<FSavedGameDetails>** Games);
	static void FlushViewports();
	static void ImplementCredits(const TCHAR* CreditsInfo, const TCHAR* WebURL, const TCHAR* LogoPath = NULL); // add legal credits to a driver.

	virtual void edSetClickLocation( FVector& InLocation ) {};
	virtual void edNoteActor(AActor* Other, EActorAction Action) {}
	virtual UObject* edGetSelection(UClass* ObjClass)
	{
		return NULL;
	}

	// Temporary!!
	virtual int edcamMode( UViewport* Viewport ) {return 0;}
	