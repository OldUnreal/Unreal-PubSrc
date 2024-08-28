/*=============================================================================
	UnFileCache.h: Unreal local file cache handler
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
=============================================================================*/
class CORE_API GameCacheFolderIni
{
private:
	FString IniName;

public:
	GameCacheFolderIni();
	void SaveConfig();
	void AddCacheFile(const TCHAR* GUID, const TCHAR* FileName);
	const TCHAR* FindMatchingFile(const TCHAR* FileName);

	inline const TCHAR* GetIniFileName() const
	{
		return *IniName;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
