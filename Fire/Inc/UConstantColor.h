	UConstantColor() {}
	void PostLoad();
	void Serialize(FArchive& Ar);
	void Init(INT InUSize, INT InVSize);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	void PostEditChange();
	virtual void SetColors(const FColor* A, const FColor* B);
	virtual void InitPalette();
	virtual void InitTexture();

	UBOOL IsStaticResolution() const
	{
		return FALSE;
	}
