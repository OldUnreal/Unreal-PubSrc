	// Constructors. Executed on both instantation and reloading of an object.
	UFractalTexture();

	// UObject interface.
	void PostLoad();
	void PostEditChange()
	{
		Super::PostEditChange();
		InitFractal();
	}

	// UTexture interface.
	void Init(INT InUSize, INT InVSize);
	FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
	void Prime();

	// Export/Import
	virtual UBOOL ExportsPalette() const
	{
		return FALSE;
	}
	void ExportFractalString(FOutputDevice& Ar);
	void ImportFractalString(const TCHAR* Stream);
	void ImportFractalValue(const TCHAR* Key, const TCHAR* Value);

	// UFractalTexture interface.
	virtual void TouchTexture(INT UPos, INT VPos, FLOAT Magnitude) {}
	virtual void InitFractal();
