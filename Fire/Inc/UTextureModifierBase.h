	UTextureModifierBase();
	void Serialize( FArchive& Ar );
	void PostLoad();

	struct FMaterialSequenceItem
	{
		UTexture* Material;
		FLOAT DisplayTime,FadeOutTime;
	};

	inline FColor ByteExchangePalette( const FColor& C )
	{
		return FColor(FF_HALF(C.B), FF_HALF(C.G), FF_HALF(C.R), 255);
	}
	UBOOL IsStaticResolution() const
	{
		return FALSE;
	}
