	// Constructors.
	UFireTexture();

	// UObject interface.
	void Serialize(FArchive& Ar);

	// UTexture interface.
	void Init(INT InUSize, INT InVSize);
	void Clear(DWORD ClearFlags);
	void ConstantTimeTick();
	void MousePosition(DWORD Buttons, FLOAT X, FLOAT Y);
	void Click(DWORD Buttons, FLOAT X, FLOAT Y);
	void ExportFractalString(FOutputDevice& Ar);
	void ImportFractalString(const TCHAR* Stream);
	void ImportFractalValue(const TCHAR* Key, const TCHAR* Value);
	UBOOL ExportsPalette() const
	{
		return TRUE;
	}

	// UFractalTexture interface.
	void TouchTexture(INT UPos, INT VPos, FLOAT Magnitude);

	// UFireTexture interface.
	void InitFractal();
private:
	void AddSpark(INT SparkX, INT SparkY);
	void DrawSparkLine(INT StartX, INT StartY, INT DestX, INT DestY, INT Density);
	void FirePaint(INT MouseX, INT MouseY, DWORD Buttons);
	void CloseSpark(INT SparkX, INT SparkY);
	void DeleteSparks(INT SparkX, INT SparkY, INT AreaWidth);
	void DrawFlashRamp(LineSeg LL, BYTE Color1, BYTE Color2);
	void MoveSpark(FSpark* Spark);
	void MoveSparkTwo(FSpark* Spark);
	void MoveSparkXY(FSpark* Spark, SBYTE Xspeed, SBYTE Yspeed);
	void MoveSparkAngle(FSpark* Spark, BYTE Direction);
	void RedrawSparks();
	void PostDrawSparks();
	void TempDrawSpark(INT PosX, INT PosY, INT Intensity);
