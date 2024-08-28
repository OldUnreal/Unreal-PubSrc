	UTexOscillator(){}
	void Tick( FLOAT DeltaSeconds );
	void PostEditChange()
	{
		LastSu = 0.f;
		LastSv = 0.f;
		Super::PostEditChange();
	}
	void UpdatePtrList();
	void GetCoords(FCoords2D& C);
