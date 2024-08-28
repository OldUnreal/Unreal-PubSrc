	// Constructors.
	UWaterTexture();

	// UObject interface.
	void Destroy();

	// UTexture interface.
	void Init(INT InUSize, INT InVSize);
	void Clear(DWORD ClearFlags);
	void MousePosition(DWORD Buttons, FLOAT X, FLOAT Y);
	void Click(DWORD Buttons, FLOAT X, FLOAT Y);

	// UFractalTexture interface.
	void TouchTexture(INT UPos, INT VPos, FLOAT Magnitude);
	UBOOL ExportsPalette() const
	{
		return TRUE;
	}

	// UWaterTexture interface.
	void CalculateWater();
	void WaterRedrawDrops();
	void InitFractal();
private:
	void AddDrop(INT DropX, INT DropY);
	void WaterPaint(INT MouseX, INT MouseY, DWORD Buttons);
	void DeleteDrops(INT DropX, INT DropY, INT AreaWidth);
