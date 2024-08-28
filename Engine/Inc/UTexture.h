	// Constructor.
	UTexture();
	UTexture(BYTE InFormat, INT InUSize, INT InVSize);

	// UObject interface.
	void Serialize( FArchive& Ar );
	const TCHAR* Import( const TCHAR* Buffer, const TCHAR* BufferEnd, const TCHAR* FileType );
	void Export( FArchive& Ar, const TCHAR* FileType, INT Indent );
	void PostLoad();
	void Destroy();
	void onPropertyChange(UProperty* Property, UProperty* OuterProperty);
	void onPackageChanged(UObject* OldPackage);

	// UBitmap interface.
	DWORD GetColorsIndex();
	FColor* GetColors();
	INT GetNumMips()
	{
		return Min<INT>(Mips.Num(),MAX_MIPS);
	}
	FMipmapBase* GetMip( INT i )
	{
		return &Mips(i);
	}
	FTextureInfo* GetTexture( INT LOD, URenderDevice* RenDev );

	// UTexture interface.
	virtual void Clear( DWORD ClearFlags );
	virtual void Init( INT InUSize, INT InVSize );
	virtual void Tick( FLOAT DeltaSeconds );
	virtual void ConstantTimeTick();
	virtual void MousePosition( DWORD Buttons, FLOAT X, FLOAT Y ) {}
	virtual void Click( DWORD Buttons, FLOAT X, FLOAT Y ) {}
	virtual void Update();
	virtual void Prime();

	// Fractal texture exporter/importer implementation.
	virtual UBOOL IsParametric() const; // TRUE if can be exported/imported as FX.
	virtual UBOOL IsStaticResolution() const; // FALSE if it's independent resolution.
	virtual UBOOL ShouldExportProperty(UProperty* Property) const; // TRUE if allowed to export fractal texture property.
	virtual void ExportFractalString(FOutputDevice& Ar);
	virtual void ImportFractalString(const TCHAR* Stream);
	virtual void ImportFractalValue(const TCHAR* Key, const TCHAR* Value);

	// UTexture functions.
	void BuildRemapIndex( UBOOL Masked );
	void CreateMips( UBOOL FullMips, UBOOL Downsample );
	void CreateColorRange();
	UBOOL Compress( ETextureFormat Format, UBOOL Mipmap );
	UBOOL Decompress( ETextureFormat ToFormat,  UBOOL bUncompressed );
    void TransformPalette();
	INT DefaultLOD();
	void PostEditChange();
	UTexture* GetHDTexture(FLOAT XS, FLOAT YS);

	// UTexture accessors.
	inline UTexture* Get()
	{
		Update();
		return AnimCurrent ? AnimCurrent : this;
	}
	inline UTexture* GetAnimFract(FLOAT Fract)
	{
		guardSlow(UTexture::GetAnimFract);
		if (!AnimNext)
			return this;
		INT Count = 1;
		UTexture* Test;
		for (Test = AnimNext; Test && Test != this; Test = Test->AnimNext)
			Count++;
		Test = this;
		INT Num = Clamp(appFloor(Fract * Count), 0, Count - 1);
		while (Num-- > 0)
			Test = Test->AnimNext;
		return Test;
		unguardSlow;
	}

	UBOOL CreateSourceMip();
	UBOOL ConvertFormat(ETextureFormat Dest, BOOL bMipmap);

	// Static
	// For distance culled decals (mainly used by shadows).
	static FLOAT FadeOutDistance;
	
	// Run Heckbert Quantize to turn RGB/RGB8 texture into P8 texture.
	// Result array must be NUM_PAL_COLORS wide.
	static INT Palettetize(const FMipmap* Src, BYTE* Dest, FColor* Result);

	//
	// Run ISPCTextureCompressor to compress to BC1/2/3/7 format
	// SourceFormat - Format of source texture
	// Src - Source mipmap
	// DestFormat - Destination format
	// DestSize - Number of bits returned.
	// Result - Result bits.
	// 
	static BYTE* CompressMip(ETextureFormat SourceFormat, const FMipmap* Src, ETextureFormat DestFormat, INT& DestSize);
	//
	// Decompress from BC1/2/3/7 to RGBA8
	// 
	static TArray<BYTE> DecompressMip(ETextureFormat SourceFormat, const FMipmap* Src, ETextureFormat DestFormat);

	//
	// Converts a base mip in textureformat @Src into a set of mips in textureformat @Dest.
	// We use the RGBA8 format as an intermediate format for all conversions where neither @Src nor @Dest are RGBA8.
	// 
	static TArray<FMipmap>* ConvertMip(ETextureFormat Src, ETextureFormat Dest, FMipmap* Mip, UBOOL bMipmap, UObject* Obj, UPalette*& CPalette, DWORD TexPolyFlags = PF_None);
	
	// Get most recent delta seconds.
	static FLOAT GetLevelDeltaTime();
	static FLOAT GetLevelTimeSeconds();

	FString String()
	{
		return FString::Printf(TEXT("Texture %ls - DrawScale %f"), GetPathName(), DrawScale);
	}