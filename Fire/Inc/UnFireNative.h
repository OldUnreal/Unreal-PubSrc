/*=============================================================================
	UnFireNative.h: Native function lookup table for static libraries.
	Copyright 2000 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Brandon Reinhart
=============================================================================*/

#ifndef UNFIRENATIVE_H
#define UNFIRENATIVE_H

#if __STATIC_LINK
/* No native execs.
DECLARE_NATIVE_TYPE(Fire,UFractalTexture);
DECLARE_NATIVE_TYPE(Fire,UFireTexture);
DECLARE_NATIVE_TYPE(Fire,UWaterTexture);
DECLARE_NATIVE_TYPE(Fire,UWaveTexture);
DECLARE_NATIVE_TYPE(Fire,UWetTexture);
DECLARE_NATIVE_TYPE(Fire,UIceTexture);
*/

DECLARE_NATIVE_TYPE(Fire,AProceduralMesh);
DECLARE_NATIVE_TYPE(Fire,UPaletteModifier);
DECLARE_NATIVE_TYPE(Fire,UConstantColor);
DECLARE_NATIVE_TYPE(Fire,UTextureModifierBase);
DECLARE_NATIVE_TYPE(Fire,AFluidSurfaceInfo);

#define AUTO_INITIALIZE_REGISTRANTS_FIRE		\
	UFractalTexture::StaticClass();				\
	UFireTexture::StaticClass();				\
	UWaterTexture::StaticClass();				\
	UWaveTexture::StaticClass();				\
	UWetTexture::StaticClass();					\
	UIceTexture::StaticClass();					\
	AFluidSurfaceInfo::StaticClass();			\
	AFluidSurfaceOscillator::StaticClass();		\
	UFluidPrimitive::StaticClass();				\
	UFluidSurfaceRI::StaticClass();				\
	AProceduralMesh::StaticClass();				\
	UProcPrimitive::StaticClass();				\
	AGrassDeco::StaticClass();					\
	UTexModifier::StaticClass();				\
	UTexScaler::StaticClass();					\
	UTexPanner::StaticClass();					\
	UTexOscillator::StaticClass();				\
	UTexRotator::StaticClass();					\
	UTexCombiner::StaticClass();				\
	UMaterialSequence::StaticClass();			\
	UTextureModifierBase::StaticClass();		\
	UConstantColor::StaticClass();				\
	UFadeColor::StaticClass();					\
	UPaletteModifier::StaticClass();

#endif

#endif
