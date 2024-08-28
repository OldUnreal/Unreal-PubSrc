	AProceduralMesh(){}
	void PostLoad();
	void Spawned();
	void PostScriptDestroyed();
	void InitProcMesh();
	void PostEditChange();
	void Serialize( FArchive& Ar );
	UBOOL Tick( FLOAT DeltaTime, enum ELevelTick TickType );
	void NoteDuplicate( AActor* Src );