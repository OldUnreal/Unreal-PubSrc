	// Constructors.
	UWetTexture();

	// UObject interface.
	void Destroy();

	// UTexture interface.
	void Init(INT InUSize, INT InVSize);
	void Clear(DWORD ClearFlags);
	void ConstantTimeTick();
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev)
	{
		if (SourceTexture && SourceTexture != this)
			SourceTexture->GetTexture(0, NULL);
		return Super::GetTexture(0, RenDev);
	}
	void InitFractal();

	UBOOL ExportsPalette() const
	{
		return FALSE;
	}

private:
	// UWetTexture interface.
	void SetRefractionTable();
	void ApplyWetTexture();
