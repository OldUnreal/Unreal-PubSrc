	AFluidSurfaceInfo(){}
	void PostLoad();
	void Spawned();
	void PostScriptDestroyed();
	void InitFluidSurface();
	void PostEditChange();
	UBOOL Tick( FLOAT DeltaTime, ELevelTick TickType );
	void TickWaves(FLOAT DeltaTime);
	void Serialize( FArchive& Ar );
	void PostEditMove();
	void PockWave3D( const FVector& Pos, FLOAT Force, FLOAT Radius, BYTE bForce=0 );
	void RenderSelectInfo( FSceneNode* Frame );
	FStaticLightActor* GetStaticLights();
	BYTE HasSlowRayTrace();
	void PostRaytrace(FThreadLock& Mutex);
	void Modify();
	void NoteDuplicate( AActor* Src );
	void AbortThread(UBOOL bWaitForFinish = FALSE);