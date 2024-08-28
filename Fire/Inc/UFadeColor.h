	UFadeColor() {}
	void Tick(FLOAT DeltaSeconds);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	virtual void SetColors(const FColor* A, const FColor* B);
	virtual void InitPalette();
	virtual void InitTexture();