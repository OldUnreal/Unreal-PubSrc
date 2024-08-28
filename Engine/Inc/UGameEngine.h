	static class UTravelDataManager* GServerTravelData;

	// Constructors.
	UGameEngine();

	// UObject interface.
	void Destroy();
	void Serialize(FArchive& Ar);

	// UEngine interface.
	void Flush( UBOOL AllowPrecache );
	void Init();
	void Exit();
	void Tick( FLOAT DeltaSeconds );
	void Draw( UViewport* Viewport, UBOOL Blit=1, BYTE* HitData=NULL, INT* HitSize=NULL );
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	void MouseDelta( UViewport*, DWORD, FLOAT, FLOAT );
	void MousePosition( class UViewport*, DWORD, FLOAT, FLOAT );
	void Click( UViewport*, DWORD, FLOAT, FLOAT );
	void SetClientTravel( UPlayer* Viewport, const TCHAR* NextURL, UBOOL bItems, ETravelType TravelType );
	FLOAT GetMaxTickRate();
	INT ChallengeResponse( INT Challenge );
	void SetProgress( const TCHAR* Str1, const TCHAR* Str2, FLOAT Seconds );

	// UGameEngine interface.
	virtual UBOOL Browse( FURL URL, const TMap<FString,FString>* TravelInfo, FString& Error);
	virtual ULevel* LoadMap( const FURL& URL, UPendingLevel* Pending, const TMap<FString,FString>* TravelInfo, FString& Error );
	virtual UBOOL SaveGame( INT Position, const TCHAR* ExtraInfo );
	virtual void CancelPending();
	virtual void PaintProgress();
	virtual void UpdateConnectingMessage();
	virtual void BuildServerMasterMap( class UNetDriver* NetDriver, ULevel* InLevel );
	virtual void NotifyLevelChange();
	static void StoreLoginPassword(const TCHAR* Addr, const TCHAR* Password);

	// Cache ripping stuff.
	void CacheRipPackageAndDependencies(const TCHAR* InName = NULL);
