	// Constructors.
	UPalette();

	// UObject interface.
	void Serialize( FArchive& Ar );

	// UPalette interface.
	BYTE BestMatch( FColor Color, INT First );
	UPalette* ReplaceWithExisting();
	void FixPalette();
	UBOOL Matches(UPalette* Other);