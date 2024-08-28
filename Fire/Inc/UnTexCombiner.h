/*=============================================================================
	UnTexCombiner.cpp
	Created by Marco.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

void UTexCombiner::Init( INT InUSize, INT InVSize )
{
	MatUSize = InUSize;
	MatVSize = InVSize;
	InitTex();
}
void UTexCombiner::PostEditChange()
{
	Super::PostEditChange();
	bComputedFrame = 0;
}

FTextureInfo* UTexCombiner::GetTexture(INT LOD, URenderDevice* RenDev)
{
	guard(UTexCombiner::GetTexture);
	STAT(STAT_TRACK_FRACTAL);
	if (Material1 == this)
		Material1 = NULL;
	if (Material2 == this)
		Material2 = NULL;

	if( !RenDev || RenDev->SpanBased )
		return (Material1 ? Material1 : SoftwareFallback)->GetTexture(LOD, RenDev);

	if( !Mips.Num() || USize!=MatUSize || VSize!=MatVSize )
		InitTex();

	if (Material1 && (!bComputedFrame || (!bStaticFrame && LastRenderedTime != GFrameNumber)))
	{
		LastRenderedTime = GFrameNumber;
		Tick(GetLevelDeltaTime());
	}

	if (!TextureHandle)
	{
		TextureHandle = new FTextureInfo;
		bParmsDirty = TRUE;

		// Set static texture info.
		TextureHandle->Texture = this;
		TextureHandle->Pan = FVector(0, 0, 0);
		TextureHandle->MaxColor = &MaxColor;
		TextureHandle->LOD = 0;
		TextureHandle->bHighColorQuality = 0;
		TextureHandle->bHighTextureQuality = 0;
		TextureHandle->bRealtime = 1;
		TextureHandle->bParametric = 0;
		TextureHandle->NumMips = 1;
		TextureHandle->Format = TEXF_BGRA8;
		TextureHandle->PaletteCacheID = 0;
		TextureHandle->Palette = NULL;
		TextureHandle->CacheID = MakeCacheID((ECacheIDBase)(CID_RenderTexture), this);
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
		TextureHandle->UClampMode = UClampMode;
		TextureHandle->VClampMode = VClampMode;
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
	unguardobj;
}
void UTexCombiner::InitTex()
{
	guard(UTexCombiner::InitTex);
	MaxColor = FColor(255,255,255,255);
	Palette = NULL;
	Mips.Empty();
	Mips.AddZeroed(1);
	FMipmap& Mip = Mips(0);
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
	Mip.DataArray.Add(DataSize*sizeof(FColor));
	Mip.UBits = UBits;
	Mip.VBits = VBits;
	Mip.USize = USize;
	Mip.VSize = VSize;
	Mip.DataPtr = &Mip.DataArray(0);
	appMemset(Mip.DataPtr,50,Mip.DataArray.Num());
	bComputedFrame = 0;
	bRealtimeChanged = 1;
	if( CombineOperation==CO_MaskOnly )
		PolyFlags|=PF_Masked;
	else PolyFlags&=(~PF_Masked);
	bParmsDirty = TRUE;
	unguard;
}

#define ScaleValue(ID,Value,Num) ((Material##ID##Scale[Num]>0) ? Value<<Material##ID##Scale[Num] : ((Material##ID##Scale[Num]<0) ? Value>>(-Material##ID##Scale[Num]) : Value))
#define YFOR_LOOP(ID) INT Mat##ID##Multi = (ScaleValue(ID,y,1) & Mat##ID##ScV)*Mat##ID##U;
#define XYFOR_GETOFFSET(MipMapName,ID) (MipMapName->Palette[Mat##ID##Bits[(ScaleValue(ID,x,0) & Mat##ID##ScU)+Mat##ID##Multi]])
#define XYFOR_GETBYTE(ID) (Mat##ID##Bits[(ScaleValue(ID,x,0) & Mat##ID##ScU)+Mat##ID##Multi])

inline void CO_MaskOperator( FColor& Res, const FColor& A, BYTE Num )
{
	BYTE M(Num ? 1 : 0);
	Res = FColor(FF_HALF(A.B*M),FF_HALF(A.G*M),FF_HALF(A.R*M),Num ? 255 : 0);
}
inline void CO_MaskOperatorDual( FColor& Res, const FColor& A, const FColor& B, BYTE Num )
{
	Res = FColor(FF_HALF(Num ? A.B : B.B),FF_HALF(Num ? A.G : B.G),FF_HALF(Num ? A.R : B.R),255);
}

static void CO_AddColors( FColor& Res, const FColor& A, const FColor& B )
{
	Res = FColor(FF_HALF(Min<INT>(A.B+B.B,255)),FF_HALF(Min<INT>(A.G+B.G,255)),FF_HALF(Min<INT>(A.R+B.R,255)),255);
}
static void CO_AddColorsMask( FColor& Res, const FColor& A, const FColor& B, BYTE Num )
{
	BYTE M(Num ? 1 : 0);
	Res = FColor(FF_HALF(Min<INT>(A.B+B.B*M,255)),FF_HALF(Min<INT>(A.G+B.G*M,255)),FF_HALF(Min<INT>(A.R+B.R*M,255)),255);
}
static void CO_SubtractColors( FColor& Res, const FColor& A, const FColor& B )
{
	Res = FColor(FF_HALF(Max<INT>(A.B-B.B,0)),FF_HALF(Max<INT>(A.G-B.G,0)),FF_HALF(Max<INT>(A.R-B.R,0)),255);
}
static void CO_SubtractColorsMask( FColor& Res, const FColor& A, const FColor& B, BYTE Num )
{
	BYTE M(Num ? 1 : 0);
	Res = FColor(FF_HALF(Max<INT>(A.B-B.B*M,0)),FF_HALF(Max<INT>(A.G-B.G*M,0)),FF_HALF(Max<INT>(A.R-B.R*M,0)),255);
}
static void CO_TranslucentColorsMask( FColor& Res, const FColor& A, const FColor& B, BYTE Num )
{
	FLOAT Scale(Min<FLOAT>(Num/255.f,1.f));
	FLOAT RevScale(1.f-Scale);
	Res = FColor(FF_HALF(Min<INT>(appFloor(A.B*RevScale+B.B*Scale),255)),FF_HALF(Min<INT>(appFloor(A.G*RevScale+B.G*Scale),255)),FF_HALF(Min<INT>(appFloor(A.R*RevScale+B.R*Scale),255)),255);
}

void UTexCombiner::Tick( FLOAT DeltaSeconds )
{
	if( (bComputedFrame && bStaticFrame) || !Material1 )
		return;
	bComputedFrame = 1;
	bRealtimeChanged = 1;

	FTextureInfo* TMain = Material1->GetTexture(INDEX_NONE, NULL);
	if (!TMain || !TMain->NumMips || TMain->Format != TEXF_P8)
		return;

	FTextureInfo* TSec = NULL, * TMask = NULL;
	if( Material2 )
	{
		TSec = Material2->GetTexture(INDEX_NONE, NULL);
		if (TSec && (!TSec->NumMips || TSec->Format != TEXF_P8))
			TSec = NULL;
	}
	if( MaskTexture )
	{
		TMask = MaskTexture->GetTexture(INDEX_NONE, NULL);
		if (TMask && (!TMask->NumMips || TMask->Format != TEXF_P8))
			TMask = NULL;
	}
	FColor* MyBits = (FColor*)Mips(0).DataPtr;
	FMipmapBase* MMipA = TMain->Mips[0];
	FMipmapBase* MMipB = (TSec ? TSec->Mips[0] : NULL);
	FMipmapBase* MMipC = (TMask ? TMask->Mips[0] : NULL);

	BYTE* MatABits(MMipA->DataPtr);
	BYTE* MatBBits(TSec ? MMipB->DataPtr : NULL);
	BYTE* MatCBits(TMask ? MMipC->DataPtr : NULL);

	INT MatAU=(MMipA->USize);
	INT MatBU=(TSec ? MMipB->USize : 0);
	INT MatCU=(TMask ? MMipC->USize : 0);
	INT MatAScU=(MMipA->USize-1),MatAScV=(MMipA->VSize-1);
	INT MatBScU=(TSec ? MMipB->USize-1 : 0),MatBScV=(TSec ? MMipB->VSize-1 : 0);
	INT MatCScU=(TMask ? MMipC->USize-1 : 0),MatCScV=(TMask ? MMipC->VSize-1 : 0);

	if (CombineOperation != CO_MaskOnly ? !TSec : !TMask) // Invalid operation, just draw main tex.
	{
		for( INT y=0; y<VSize; y++ )
		{
			INT YMulti = (y*USize);
			YFOR_LOOP(A);
			for( INT x=0; x<USize; x++ )
				MyBits[x+YMulti] = ByteExchangePalette(XYFOR_GETOFFSET(TMain,A));
		}
		return;
	}
	if( CombineOperation==CO_MaskOnly )
	{
		if (!TSec)
		{
			for( INT y=0; y<VSize; y++ )
			{
				INT YMulti = (y*USize);
				YFOR_LOOP(A);
				YFOR_LOOP(C);
				for( INT x=0; x<USize; x++ )
					CO_MaskOperator(MyBits[x+YMulti],XYFOR_GETOFFSET(TMain,A),XYFOR_GETBYTE(C));
			}
		}
		else
		{
			for( INT y=0; y<VSize; y++ )
			{
				INT YMulti = (y*USize);
				YFOR_LOOP(A);
				YFOR_LOOP(B);
				YFOR_LOOP(C);
				for( INT x=0; x<USize; x++ )
					CO_MaskOperatorDual(MyBits[x+YMulti],XYFOR_GETOFFSET(TMain,A),XYFOR_GETOFFSET(TSec,B),XYFOR_GETBYTE(C));
			}
		}
		return;
	}

	if (!TMask)
	{
		void ( *DualColorOperation)( FColor& Res, const FColor& A, const FColor& B );

		switch( CombineOperation )
		{
		case CO_Add:
			DualColorOperation = CO_AddColors;
			break;
		case CO_Subtract:
			DualColorOperation = CO_SubtractColors;
			break;
		default:
			DualColorOperation = CO_AddColors;
		}
		for( INT y=0; y<VSize; y++ )
		{
			INT YMulti = (y*USize);
			YFOR_LOOP(A);
			YFOR_LOOP(B);
			for( INT x=0; x<USize; x++ )
				DualColorOperation(MyBits[x+YMulti],XYFOR_GETOFFSET(TMain,A),XYFOR_GETOFFSET(TSec,B));
		}
	}
	else
	{
		void ( *MultiColorOperation)( FColor& Res, const FColor& A, const FColor& B, BYTE MaskNum );

		switch( CombineOperation )
		{
		case CO_Add:
			MultiColorOperation = CO_AddColorsMask;
			break;
		case CO_Subtract:
			MultiColorOperation = CO_SubtractColorsMask;
			break;
		case CO_OverlayWithTransp:
			MultiColorOperation = CO_TranslucentColorsMask;
			break;
		default:
			MultiColorOperation = CO_AddColorsMask;
		}
		for( INT y=0; y<VSize; y++ )
		{
			INT YMulti = (y*USize);
			YFOR_LOOP(A);
			YFOR_LOOP(B);
			YFOR_LOOP(C);
			for( INT x=0; x<USize; x++ )
				MultiColorOperation(MyBits[x+YMulti],XYFOR_GETOFFSET(TMain,A),XYFOR_GETOFFSET(TSec,B),XYFOR_GETBYTE(C));
		}
	}
}
