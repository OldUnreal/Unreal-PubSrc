	UShadowBitMap();
	
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	void Clear(DWORD ClearFlags);
	void Init( INT InUSize, INT InVSize );
	void Tick( FLOAT DeltaSeconds ) {}
	void ConstantTimeTick() {}
	void Update() {}
	void Prime() {}
	void Serialize( FArchive& Ar );
	void DrawFullShadow(BYTE* Target, FLOAT FadeOutDist);
	void Destroy();
	