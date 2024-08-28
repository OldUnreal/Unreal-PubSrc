	/* Unreal security class */
	USecurityData(); // Constructor

	UBOOL TempBanClient(const TCHAR* CIP, const TCHAR* CID, const TCHAR* CIDnr, const TCHAR* PLName, const TCHAR* Reason);
	UBOOL PermBanClient(const TCHAR* CIP, const TCHAR* CID, const TCHAR* CIDnr, const TCHAR* PLName);
	UBOOL ErasePermBan(INT Index);
	UBOOL EraseTempBan(INT Index);
	void EraseTempBanList();
	UBOOL GetAliasNames(const TCHAR* CIP, const TCHAR* CID, const TCHAR* CIDnr, const TCHAR* PLName, FString& Result);
	UBOOL CheckClientBanned(const TCHAR* CIP, const TCHAR* CID, const TCHAR* CIDnr);
	static void NotifyLevelChange();
	
	static USecurityData* GetSecurity();
