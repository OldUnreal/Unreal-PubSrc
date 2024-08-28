/*=============================================================================
    UnMaterialSequence.h
	Created by Marco.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

UMaterialSequence(){}
FTextureInfo* GetTexture(INT LOD, URenderDevice* RenDev);
void InitTex();
void Tick( FLOAT DeltaSeconds );
void Init( INT InUSize, INT InVSize );
void AdvanceFrame( FLOAT DeltaSeconds );
UBOOL IsStaticResolution() const
{
	return TRUE;
}
UBOOL ShouldExportProperty(UProperty* Property) const;
void OnImportDefaults(UClass* OwnerClass);