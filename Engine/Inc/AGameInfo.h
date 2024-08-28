	AGameInfo() {}

	void PostBeginPlay();
	
	static void StripColorCodes(FString& Str);
	static const TCHAR* MakeColorCode(const FColor& C);
	static void SanitizeString(FString& Str);
