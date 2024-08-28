/*=============================================================================
	UI.cpp: Unreal Integrity.
	This is private anti-cheat header.

	Revision history:
		* Created by Marco
=============================================================================*/

#pragma once

class ENGINE_API USHAUpdateCommandlet : public UCommandlet
{
public:
	USHAUpdateCommandlet(){}
	INT Main( const TCHAR* Parms );
	DECLARE_CLASS(USHAUpdateCommandlet,UCommandlet,CLASS_Transient,Engine);
};

class ENGINE_API ULinkerUpdateCommandlet : public UCommandlet
{
public:
 ULinkerUpdateCommandlet(){}
 INT Main( const TCHAR* Parms );
 DECLARE_CLASS(ULinkerUpdateCommandlet,UCommandlet,CLASS_Transient,UnrealIntegrity);
};

// Misc functionality of Engine.
const TCHAR* GetFileSHA( const TCHAR* FileName, BYTE bAllowCache=0, const TCHAR* MatchCheck=NULL, BYTE bForceUpdate=0 );
void CheckFileLinkers( const TCHAR* FileName );

