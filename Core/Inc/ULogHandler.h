	ULogHandler();

	void Destroy();
	void SetEnabled(BYTE bEnable);
	static BYTE GrabLogLine(EName Type, const TCHAR* Str);
	void Serialize(FArchive& Ar);