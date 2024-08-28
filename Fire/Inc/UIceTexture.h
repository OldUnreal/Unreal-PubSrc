	// Constructors.
	UIceTexture();

	// UObject interface.
	void Destroy();

	// UTexture interface.
	void Init(INT InUSize, INT InVSize);
	void Clear(DWORD ClearFlags);
	void ConstantTimeTick();
	void Tick(FLOAT DeltaSeconds);
	void MousePosition(DWORD Buttons, FLOAT X, FLOAT Y);
	void Click(DWORD Buttons, FLOAT X, FLOAT Y);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev)
	{
		if (GlassTexture && GlassTexture != this)
			GlassTexture->GetTexture(0, NULL);
		if (SourceTexture && SourceTexture != this)
			SourceTexture->GetTexture(0, NULL);
		return Super::GetTexture(LOD, RenDev);
	}
	void InitFractal();

private:
	void MoveIcePosition(FLOAT VTicks);
	void RenderIce(FLOAT DeltaTime);
	void BlitTexIce();
	void BlitIceTex();
