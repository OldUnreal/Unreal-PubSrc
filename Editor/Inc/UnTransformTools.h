/*=============================================================================
	UnTransformTools.h: Unreal enhanced brush tools
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Marco
		* Implementation by Smirftsch
=============================================================================*/

EDITOR_API void SaveDataToFile( FArchive* A, bool bSelected, ULevel* Map );
EDITOR_API void LoadDataFromFile( FArchive* A, ULevel* Map );
EDITOR_API void TransformPerma( ULevel* Map, bool bSelected );
EDITOR_API void MakeTessellate( ULevel* Map );
EDITOR_API void TransformModel( AActor* A );
