	// Constructors. Executed on both instantation and reloading of an object.
	UScriptedTexture();

	// UObject interface.
	void PostLoad();
	void PostEditChange();
	void Destroy();
	void Serialize( FArchive& Ar );

	// UTexture interface.
	void Init( INT InUSize, INT InVSize );
	void Tick(FLOAT DeltaSeconds);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	
	// UScriptedTexture Interface.
	void DrawTile( FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, UTexture *Tex, FTextureInfo *Info, UBOOL bMasked, UBOOL bUseColor=0, BYTE ColorIndex=0, FPlane* Color=NULL );
	void DrawString( UFont* Font, INT DrawX, INT DrawY, const TCHAR* Text, UBOOL bUseColor=0, BYTE PaletteEntry=0, INT NewX=0, INT ClipX=INDEX_NONE, FPlane* DrawColor=NULL );
	BYTE WriteTexture( TArray<BYTE>& List, BYTE Comp );
	BYTE ReadTexture( TArray<BYTE>& List, BYTE Comp );
	void DrawPortal( ULevel* Map, const FVector& Pos, const FRotator& Rot, AActor* CamActor=NULL );
	
	// Static:
	static TArray<FDelayedPortal>	DelayedPortals;
	static void DrawDelayedPortals();