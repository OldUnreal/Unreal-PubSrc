	UPaletteModifier() {}
	void PostLoad();
	void Serialize(FArchive& Ar);
	void Init(INT InUSize, INT InVSize);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	void PostEditChange();

	UBOOL IsStaticResolution() const
	{
		return FALSE;
	}
