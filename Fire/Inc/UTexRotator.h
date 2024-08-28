	UTexRotator(){}
	void Tick( FLOAT DeltaSeconds );
	void PostEditChange()
	{
		CurrentAngle = 0.f;
		bWasFixedRot = 0;
		Super::PostEditChange();
	}
	void UpdatePtrList();
	void GetCoords(FCoords2D& C);