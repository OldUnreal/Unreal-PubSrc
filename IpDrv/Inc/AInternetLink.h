/*=============================================================================
	AInternetLink.h.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	AInternetLink();
	virtual void ExitNetwork();
	void Destroy();
	void PostScriptDestroyed();
	UBOOL Tick( FLOAT DeltaTime, enum ELevelTick TickType );	
	inline SOCKET& GetSocket() 
	{ 
		return Socket;
	}
	inline FResolveInfo*& GetResolveInfo()
	{
		return PrivateResolveInfo;
	}

	INT EncodeText(const TCHAR* Str, TArray<BYTE>& Buffer) const;
	FString DecodeText(const BYTE* Data, INT DataLen) const;

	static char DataBuffer[MAXRECVDATASIZE];

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
