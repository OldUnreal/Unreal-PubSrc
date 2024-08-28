	ALight(){}
	void PostRaytrace(FThreadLock& Mutex);
	void PostLoad();
	void InitBSPSurfs();
	void Destroy();
	void Serialize(FArchive& Ar);
