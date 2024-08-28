	UZoneCurtain() {}
	void Serialize(FArchive& Ar);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	void Init(INT InUSize, INT InVSize);
	void PostEditChange();
	void InitPaletteMap();
	void PostLoad();
	void Update() {}
	void Prime() {}
	void Tick(FLOAT DeltaSeconds) {}

	UBOOL IsStaticResolution() const
	{
		return FALSE;
	}
