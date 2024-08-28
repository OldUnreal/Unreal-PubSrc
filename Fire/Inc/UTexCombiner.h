/*=============================================================================
    UTexCombiner.h
	Created by Marco.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

UTexCombiner(){}
FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
void InitTex();
void Tick( FLOAT DeltaSeconds );
void Init( INT InUSize, INT InVSize );
void PostEditChange();
