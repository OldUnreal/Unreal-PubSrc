/*=============================================================================
	UnMatSeq.cpp
	Created by Marco.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

void UMaterialSequence::OnImportDefaults(UClass* OwnerClass)
{
	guardSlow(UMaterialSequence::OnImportDefaults);
	Super::OnImportDefaults(OwnerClass);
	InitTex();
	unguardSlow;
}
void UMaterialSequence::Init( INT InUSize, INT InVSize )
{
	guard(UMaterialSequence::Init);
	MatUSize = InUSize;
	MatVSize = InVSize;
	InitTex();
	unguardobj;
}
FTextureInfo* UMaterialSequence::GetTexture(INT LOD, URenderDevice* RenDev)
{
	guard(UMaterialSequence::GetTexture);
	STAT(STAT_TRACK_FRACTAL);
	if( !RenDev || RenDev->SpanBased )
	{
		// Software fallback.
		if (SequenceItems.Num() && LastRenderedTime != GFrameNumber)
		{
			LastRenderedTime = GFrameNumber;
			AdvanceFrame(GetLevelDeltaTime());
			bRealtime = FALSE;
		}
		FLOAT TestTime = CurrentTime; // Check at which offset are now on.
		UTexture* Mat = NULL;
		for( INT i=0; i<SequenceItems.Num(); i++ )
		{
			const FMaterialSequenceItem& Item = SequenceItems(i);
			FLOAT TotalTime = Item.DisplayTime+Item.FadeOutTime;
			if( TotalTime>TestTime )
			{
				Mat = Item.Material;
				break;
			}
			else TestTime-=TotalTime;
		}
		if( !Mat )
			Mat = SoftwareFallback;
		return Mat->GetTexture(LOD, RenDev);
	}

	if( !Mips.Num() || USize!=MatUSize || VSize!=MatVSize )
		InitTex();

	if (SequenceItems.Num())
	{
		if (LastRenderedTime != GFrameNumber)
		{
			LastRenderedTime = GFrameNumber;
			Tick(GetLevelDeltaTime());
			bRealtime = TRUE;
		}
		else if (!bRealtime)
		{
			Tick(0.f);
			bRealtime = TRUE;
		}
	}

	if (!TextureHandle)
	{
		bRealtime = TRUE;
		TextureHandle = new FTextureInfo;
		bParmsDirty = TRUE;

		// Set static texture info.
		TextureHandle->Texture = this;
		TextureHandle->Pan = FVector(0, 0, 0);
		TextureHandle->MaxColor = &MaxColor;
		TextureHandle->LOD = 0;
		TextureHandle->PaletteCacheID = 0;
		TextureHandle->Palette = NULL;
		TextureHandle->CacheID = MakeCacheID((ECacheIDBase)(CID_RenderTexture), this);
		TextureHandle->NumMips = 1;
		TextureHandle->Format = TEXF_BGRA8;
		TextureHandle->bHighColorQuality = FALSE;
		TextureHandle->bHighTextureQuality = FALSE;
		TextureHandle->bRealtime = TRUE;
		TextureHandle->bParametric = FALSE;
	}
	if (bParmsDirty)
	{
		// Reset the texture flags.
		TextureHandle->bHighColorQuality = bHighColorQuality;
		TextureHandle->bHighTextureQuality = bHighTextureQuality;
		TextureHandle->bRealtime = bRealtime;
		TextureHandle->bParametric = bParametric;
		TextureHandle->UClampMode = UClampMode;
		TextureHandle->VClampMode = VClampMode;
		TextureHandle->UScale = DrawScale;
		TextureHandle->VScale = DrawScale;
		TextureHandle->USize = USize;
		TextureHandle->VSize = VSize;
		TextureHandle->UClamp = UClamp;
		TextureHandle->VClamp = VClamp;
		TextureHandle->Mips[0] = &Mips(0);
		bParmsDirty = FALSE;
	}
	if (bRealtimeChanged)
	{
		++RenderTag;
		bRealtimeChanged = FALSE;
		TextureHandle->bRealtimeChanged = TRUE;
	}
	TextureHandle->RenderTag = RenderTag;
	TextureHandle->Modifier = NULL;

	return TextureHandle;
	unguard;
}
void UMaterialSequence::InitTex()
{
	guard(UMaterialSequence::InitTex);
	MaxColor = FColor(255,255,255,255);
	Palette = NULL;
	Mips.Empty(1);
	UBits = Max<BYTE>(appCeilLogTwo(MatUSize),2);
	VBits = Max<BYTE>(appCeilLogTwo(MatVSize),2);
	USize = (1<<UBits);
	VSize = (1<<VBits);
	MatUSize = USize;
	MatVSize = VSize;
	UClamp = USize;
	VClamp = VSize;
	Format = TEXF_BGRA8;
	INT DataSize = USize*VSize;
	FMipmap* Mip = new(Mips) FMipmap(UBits,VBits,DataSize*sizeof(FColor));
	Mip->DataPtr = &Mip->DataArray(0);
	appMemset(Mip->DataPtr,50,Mip->DataArray.Num());
	unguard;
}

#define GetTransScalePix(InVal,TransScale) \
	(TransScale<0 ? InVal>>(-TransScale) : (TransScale>0 ? InVal<<TransScale : InVal))

inline FColor GetBlendedColor( const FColor& A, const FColor& B, FLOAT Alpha, FLOAT NAlpha )
{
	return FColor(FF_HALF(appFloor(A.B * NAlpha + B.B * Alpha)), FF_HALF(appFloor(A.G * NAlpha + B.G * Alpha)), FF_HALF(appFloor(A.R * NAlpha + B.R * Alpha)), 255);
}

void UMaterialSequence::AdvanceFrame( FLOAT DeltaSeconds )
{
	guardSlow(UMaterialSequence::AdvanceFrame);
	if( !Paused )
	{
		FLOAT TotalTime=0.f; // Check timer looping...
		INT i=0;
		for(; i<SequenceItems.Num(); i++ )
			TotalTime+=(SequenceItems(i).DisplayTime+SequenceItems(i).FadeOutTime);
		CurrentTime+=DeltaSeconds;
		if( CurrentTime>=TotalTime )
		{
			if( Loop )
				CurrentTime = 0.f;
			else
			{
				CurrentTime = TotalTime-0.01f;
				Paused = 1;
			}
		}
	}
	unguardSlow;
}
void UMaterialSequence::Tick( FLOAT DeltaSeconds )
{
	guardSlow(UMaterialSequence::Tick);
	if (!Mips.Num() || USize != MatUSize || VSize != MatVSize)
		InitTex();

	if( DeltaSeconds>0.f )
		AdvanceFrame(DeltaSeconds);

	FLOAT TestTime = CurrentTime; // Check at which offset are now on.
	for( INT i=0; i<SequenceItems.Num(); i++ )
	{
		const FMaterialSequenceItem& Item = SequenceItems(i);
		if( Item.DisplayTime>TestTime )
		{
			// Simply display a single texture...
			if( !Item.Material || Item.Material==this )
				break; // Bad choise!

			if( !Item.Material->bRealtime && ConstFrameTex==Item.Material )
				break; // Constant frame, no need for update.

			// Simply copy the texture...
			FTextureInfo* TI = Item.Material->GetTexture(INDEX_NONE,NULL);
			if( !TI || !TI->NumMips || TI->Format!=TEXF_P8 )
				break;
			const FMipmapBase& MMip = *TI->Mips[0];
			INT BitDifU=(MMip.UBits-UBits),BitDifV=(MMip.VBits-VBits);
			FColor* MyBits = (FColor*)Mips(0).DataPtr;

			BYTE* OthBits = MMip.DataPtr;
			INT MultiSize = GetTransScalePix(USize,BitDifU);
			for( INT y=0; y<VSize; y++ )
			{
				INT ym = (y*USize);
				INT oy = GetTransScalePix(y,BitDifV)*MultiSize;
				for( INT x=0; x<USize; x++ )
				{
					INT ox = GetTransScalePix(x,BitDifU);
					MyBits[x+ym] = ByteExchangePalette(TI->Palette[OthBits[ox+oy]]);
				}
			}
			bRealtimeChanged = 1;
			ConstFrameTex = Item.Material;
			break;
		}
		else TestTime-=Item.DisplayTime;
		if( Item.FadeOutTime>TestTime )
		{
			// Alpha fade between 2 textures...
			FLOAT Alpha(TestTime/Item.FadeOutTime);
			const FMaterialSequenceItem& NextItem = SequenceItems((i==(SequenceItems.Num()-1) ? 0 : (i+1)));
			if( !Item.Material || Item.Material==this || !NextItem.Material || NextItem.Material==this )
				break;

			// Copy 2 textures and blend between colors.
			FTextureInfo* TI = Item.Material->GetTexture(INDEX_NONE, NULL);
			if( !TI || !TI->NumMips || TI->Format!=TEXF_P8 )
				break;
			FTextureInfo* TIB = NextItem.Material->GetTexture(INDEX_NONE, NULL);
			if( !TIB || !TIB->NumMips || TIB->Format!=TEXF_P8 )
				break;
			FLOAT NAlpha(1.f-Alpha);
			const FMipmapBase& MMip = *TI->Mips[0];
			const FMipmapBase& MMipB = *TIB->Mips[0];
			INT BitDifU=(MMip.UBits-UBits),BitDifV=(MMip.VBits-VBits);
			INT BBitDifU=(MMipB.UBits-UBits),BBitDifV=(MMipB.VBits-VBits);
			FColor* MyBits = (FColor*)Mips(0).DataPtr;

			BYTE* OthBits = MMip.DataPtr;
			BYTE* OthBitsB = MMipB.DataPtr;
			INT MultiSizeA = GetTransScalePix(USize,BitDifU);
			INT MultiSizeB = GetTransScalePix(USize,BBitDifU);
			for( INT y=0; y<VSize; y++ )
			{
				INT ym = (y*USize);
				INT oy = GetTransScalePix(y,BitDifV)*MultiSizeA;
				INT oyB = GetTransScalePix(y,BBitDifV)*MultiSizeB;
				for( INT x=0; x<USize; x++ )
				{
					INT ox = GetTransScalePix(x,BitDifU);
					INT oxB = GetTransScalePix(x,BBitDifU);
					MyBits[x+ym] = GetBlendedColor(TI->Palette[OthBits[ox+oy]],TIB->Palette[OthBitsB[oxB+oyB]],Alpha,NAlpha);
				}
			}
			bRealtimeChanged = 1;
			ConstFrameTex = NULL;
			break;
		}
		else TestTime-=Item.FadeOutTime;
	}
	unguardSlow;
}
UBOOL UMaterialSequence::ShouldExportProperty(UProperty* Property) const
{
	guardSlow(UMaterialSequence::ShouldExportProperty);
	return Super::ShouldExportProperty(Property) && appStricmp(Property->GetName(), TEXT("MatUSize")) && appStricmp(Property->GetName(), TEXT("MatVSize"));
	unguardSlow;
}
