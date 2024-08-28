	UIK_LipSync(){}

	void Reset();
	void UpdateInstance();
	void Tick(FLOAT DeltaTime);
	void Disable();
	void StartLipSyncTrack(USound* Sound, FLOAT Pitch = 1.f);