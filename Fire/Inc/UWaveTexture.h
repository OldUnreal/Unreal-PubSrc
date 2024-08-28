	// Constructors.
	UWaveTexture();

	// UTexture interface.
	void Init(INT InUSize, INT InVSize);
	void Clear(DWORD ClearFlags);
	void ConstantTimeTick();

	// UWaveTexture interface.
	void SetWaveLight();
	void InitFractal();
