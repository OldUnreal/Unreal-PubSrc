/*=============================================================================
	UnActor.h: AActor class inlines.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

/*-----------------------------------------------------------------------------
	FActorLink.
-----------------------------------------------------------------------------*/

//
// Linked list of actors.
//
struct FActorLink
{
	// Variables.
	AActor*     Actor;
	FActorLink* Next;

	// Functions.
	FActorLink( AActor* InActor, FActorLink* InNext )
	: Actor(InActor), Next(InNext)
	{}
};

/*-----------------------------------------------------------------------------
	AActor inlines.
-----------------------------------------------------------------------------*/

//
// Brush checking.
//
inline UBOOL AActor::IsBrush()       const {return Brush && IsA(ABrush::StaticClass());}
inline UBOOL AActor::IsStaticBrush() const {return Brush && (GetClass()==ABrush::StaticClass()) && bStatic;}
inline UBOOL AActor::IsMovingBrush() const {return Brush && bIsMover;}

//
// See if this actor is owned by TestOwner.
//
inline UBOOL AActor::IsOwnedBy( const AActor* TestOwner ) const
{
	guardSlow(AActor::IsOwnedBy);
	for( const AActor* Arg=this; Arg; Arg=Arg->Owner )
	{
		if( Arg == TestOwner )
			return 1;
	}
	return 0;
	unguardSlow;
}

//
// Get the top-level owner of an actor.
//
inline AActor* AActor::GetTopOwner()
{
	AActor* Top;
	for( Top=this; Top->Owner; Top=Top->Owner );
	return Top;
}

//
// See if this actor is in the specified zone.
//
inline UBOOL AActor::IsInZone( const AZoneInfo* TestZone ) const
{
	return Region.Zone!=Level ? Region.Zone==TestZone : 1;
}

//
// If this actor is a player, return it as a Pawn.
// Otherwise return NULL.
//
inline UBOOL AActor::IsPlayerPawn() const
{
	guardSlow(AActor::IsPlayerPawn);

	// Only descendents of the pawn class may be players.
	return bIsPlayerPawn;

	unguardSlow;
}
inline APlayerPawn* AActor::GetPlayerPawn()
{
	guardSlow(AActor::GetPlayerPawn);

	// Only descendents of the pawn class may be players.
	if( !bIsPlayerPawn )
		return NULL;

	// Players must have player objects.
	if(!reinterpret_cast<APlayerPawn*>(this)->Player)
		return NULL;

	// This is a player.
	return reinterpret_cast<APlayerPawn*>(this);

	unguardSlow;
}

//
// Return whether this actor looks like a player.
// Note - it could be a bot, so it may not have a viewport
//
inline UBOOL AActor::IsPlayer() const
{
	guardSlow(AActor::IsPlayer);

	// Only descendents of the pawn class may be players.
	return (bIsPawn && reinterpret_cast<const APawn*>(this)->bIsPlayer);

	unguardSlow;
}

//
// Determine if BlockingActor should block actors of the given class.
// This routine needs to be reflexive or else it will create funky
// results, i.e. A->IsBlockedBy(B) <-> B->IsBlockedBy(A).
//
inline UBOOL AActor::IsBlockedBy( const AActor* Other ) const
{
	guardSlow(AActor::IsBlockedBy);
	checkSlow(this!=NULL);
	checkSlow(Other!=NULL);

	if (((CollisionRadius + CollisionHeight) > 0.f) ? !Other->bBlockNonZeroExtentTraces : !Other->bBlockZeroExtentTraces)
		return FALSE;
	else if (Other->bWorldGeometry)
		return bCollideWorld;
	else if ((CollisionGroups & Other->CollisionFlag) == 0) // 227j: Custom blocking behaviour for mods!
		return FALSE;
	else if( Other->bIsMover )
		return bCollideWorld && (bIsPlayerPawn ? Other->bBlockPlayers : Other->bBlockActors);
	else if ( bIsMover )
		return Other->bCollideWorld && (Other->bIsPlayerPawn ? bBlockPlayers : bBlockActors);
	else
		return ( (bIsPlayerPawn || bIsProjectile) ? Other->bBlockPlayers : Other->bBlockActors)
		&&	   ( (Other->bIsPlayerPawn || Other->bIsProjectile) ? bBlockPlayers : bBlockActors);

	unguardSlow;
}

//
// Return whether this actor's movement is based on another actor.
//
inline UBOOL AActor::IsBasedOn( const AActor* Other ) const
{
	guard(AActor::IsBasedOn);
	if (!Other || Other->bDeleteMe)
		return FALSE;
	if (Other->bStatic) // If were checking with world geometry, only accept if were checking on the base actor itself.
		return (Other == this);

	for( const AActor* Test=this; (Test && !Test->bStatic); Test=Test->Base )
		if( Test == Other )
			return TRUE;
	return FALSE;
	unguard;
}

//
// Render passes for handling translucent Meshes
//
inline EActorRenderPass AActor::GetRenderPass() const
{
	guardSlow(AActor::GetRenderPass);
	if (RenderPass != RP_Auto)
		return (EActorRenderPass)RenderPass;
	if (Style >= STY_Translucent)
		return RENDPASS_Third;
	if (DrawType == DT_Mesh && Mesh)
		return Mesh->GetRenderPass();
	return RENDPASS_First;
	unguardSlow;
}

//
// Return the level of an actor.
//
inline class ULevel* AActor::GetLevel() const
{
	return XLevel;
}

//
// Return the actor's view rotation.
//
inline FRotator AActor::GetViewRotation() const
{
	guardSlow(AActor::GetViewRotation);
	return bIsPawn ? reinterpret_cast<const APawn*>(this)->ViewRotation : Rotation;
	unguardSlow;
}

// Actor is currently a dynamic light source.
inline UBOOL AActor::IsDynamicLight() const
{
	return (bDynamicLight || (!bStatic && !bNoDelete));
}

// Editor: Actor should bake lightmap information into BSP.
inline UBOOL AActor::ShouldBakeLight() const
{
	return (bStatic || (bNoDelete && !bMovable));
}

inline UBOOL AActor::IsNotDistanceCulled(const FVector& CamPos) const
{
	guardSlow(AActor::IsNotDistanceCulled);
	if (VisibilityRadius == 0.f)
		return TRUE;
	return (VisibilityRadius > 0.f) ? (Location.DistSquared(CamPos) < Square(VisibilityRadius)) : (Location.DistSquared(CamPos) > Square(VisibilityRadius));
	unguardSlow;
}

inline UBOOL AActor::ShouldDistanceFade(const FVector& CamPos, FLOAT* Alpha) const
{
	guardSlow(AActor::ShouldDistanceFade);
	if (VisibilityRadius != 0.f)
	{
		FLOAT CamDistance = Location.DistSquared(CamPos);
		if (VisibilityRadius > 0.f)
		{
			if (CamDistance > Square(VisibilityRadius * (1.f - ACTOR_VIEWFADE_DIST)))
			{
				if (Alpha)
					*Alpha = 1.f - ((appSqrt(CamDistance) - (VisibilityRadius * (1.f - ACTOR_VIEWFADE_DIST))) / (VisibilityRadius * ACTOR_VIEWFADE_DIST));
				return TRUE;
			}
		}
		else if (CamDistance < Square(VisibilityRadius * (1.f + ACTOR_VIEWFADE_DIST)))
		{
			if (Alpha)
				*Alpha = -((appSqrt(CamDistance) + VisibilityRadius) / (VisibilityRadius * ACTOR_VIEWFADE_DIST));
			return TRUE;
		}
	}
	return FALSE;
	unguardSlow;
}

inline FInterpolateData::FInterpolateData(const FVector& StartPos, const FRotator& StartRot, AActor* TargetActor, FLOAT InterpTime)
	: Actor(TargetActor), Start(StartPos), StartDir(StartRot), StartTime(TargetActor->Level->RealTimeSeconds), InterpolateTime(InterpTime), LastTime(0), bExpired(FALSE)
{
	if (TargetActor->MeshInstance)
		TargetActor->MeshInstance->bCoordsDirty = TRUE;
}
inline INT deltaTurnToward(INT current, INT desired, FLOAT Alpha)
{
	INT DeltaTurn = (desired - current) & 65535;
	if (DeltaTurn >= 32768)
		DeltaTurn -= 65536;
	return current + appRound(FLOAT(DeltaTurn) * Alpha);
}
inline void FInterpolateData::GetRenderPos(FVector* Pos, FRotator* Rot)
{
	if (bExpired)
	{
		if (Pos)
			*Pos = Actor->Location;
		if (Rot)
			*Rot = Actor->Rotation;
		return;
	}
	if (LastTime != GFrameNumber)
	{
		LastTime = GFrameNumber;
		FLOAT Alpha = (Actor->Level->RealTimeSeconds - StartTime) / InterpolateTime;
		if (Alpha >= 1.f)
		{
			if (Pos)
				*Pos = Actor->Location;
			if (Rot)
				*Rot = Actor->Rotation;
			bExpired = TRUE;
			return;
		}
		CachePos = Start.Lerp(Actor->Location, Alpha);
		CacheRot = StartDir.SlerpAligned(FQuat(Actor->Rotation), Alpha).GetRotator();

		if (Actor->MeshInstance)
			Actor->MeshInstance->bCoordsDirty = TRUE;
	}
	if (Pos)
		*Pos = CachePos;
	if (Rot)
		*Rot = CacheRot;
}
inline void FInterpolateData::UpdateNewPos(FLOAT InterpTime)
{
	FVector S;
	FRotator R;
	GetRenderPos(&S, &R);

	Start = S;
	StartDir = R;
	StartTime = Actor->Level->RealTimeSeconds;
	InterpolateTime = InterpTime;
	bExpired = FALSE;
	LastTime = 0;
}
inline UBOOL FInterpolateData::HasExpired() const
{
	return bExpired || ((Actor->Level->RealTimeSeconds - InterpolateTime) >= StartTime);
}

inline BYTE UPortalModifier::ShouldDrawActor(AActor* Other)
{
	if (GIsEditor)
		return !Other->bHiddenEd;
	return !(Other->bHidden && (!bDrawSecretGoal || !Other->bIsSecretGoal))
		&& !(bHideSecretGoal && Other->bIsSecretGoal)
		&& !(bHideMeshes && Other->DrawType == DT_Mesh)
		&& !(bHideSprites && Other->DrawType != DT_Mesh)
		&& !(bScriptFilterActors && !eventAllowDraw(Other));
}
inline void UPortalModifier::GetMeshTexture(AActor* Other, UTexture*& Tex)
{
	if (bScriptMeshTextures && !GIsEditor)
		eventGetMeshTexture(Other, Tex);
	else if (ForcedMeshTexture)
		Tex = ForcedMeshTexture;
}

inline void UPortalModifier::GetFog(FFogSurf* F)
{
	F->FogDistanceStart = DistanceFogStart;
	F->FogDistanceEnd = DistanceFogEnd;
	F->FogColor = FogColor.Plane();
	F->FogDensity = 1.f;
	F->FogMode = 0;
}
inline void APlayerPawn::GetFog(FFogSurf* F)
{
	F->FogDistanceStart = DistanceFogDistance[0];
	F->FogDistanceEnd = DistanceFogDistance[1];
	F->FogColor = DistanceFogColor;
	F->FogDensity = FogDensity;
	F->FogMode = FogMode;
}
inline void AZoneInfo::GetFog(FFogSurf* F)
{
	F->FogDistanceStart = FogDistanceStart;
	F->FogDistanceEnd = FogDistance;
	F->FogColor = FogColor.Plane();
	F->FogDensity = FogDensity;
	F->FogMode = FogMode;
}

#if 0 // TODO
inline FLOAT AZoneViewRotator::GetPointDepth(const FVector& Point) const
{
	guardSlow(AZoneViewRotator::GetPointDepth);
	const FLOAT DotA = PortalPlanes[0].PlaneDot(Point);
	const FLOAT DotB = PortalPlanes[1].PlaneDot(Point);
	return DotA / (DotA + DotB);
	unguardSlow;
}
#endif

inline AActor* UIK_SolverBase::GetOwner() const
{
	return MeshInstance->GetActor();
}
inline USkeletalMesh* UIK_SolverBase::GetMesh() const
{
	return (USkeletalMesh*)MeshInstance->GetMesh();
}

inline FShapeProperties::FShapeProperties(PX_PhysicsObject* O, AZoneInfo* Zone)
	: RbObject(O), Coords(NULL), bUseDefault(TRUE), bContactReport(FALSE), bModifyContact(FALSE)
{
	if (Zone && Zone->ZoneGroundFriction != GetDefault<AZoneInfo>()->ZoneGroundFriction)
	{
		Restitution = 0.5f;
		Friction = Zone->ZoneGroundFriction * (0.5f / 4.f);
		bUseDefault = FALSE;
	}
}

inline void AWeapon::UpdateAttachment(UBOOL bCloneSkin)
{
	guardSlow(AWeapon::UpdateAttachment);
	if (!Attachment && ThirdPersonAttachment)
	{
		Attachment = reinterpret_cast<AWeaponAttachment*>(GetLevel()->SpawnActor(ThirdPersonAttachment, NAME_None, this));
		if (Attachment)
			Attachment->eventSetWeaponOwner(this);
	}
	if (Attachment && Attachment->bCopyDisplay)
	{
		Attachment->DrawScale = ThirdPersonScale;
		Attachment->AnimSequence = AnimSequence;
		Attachment->AnimFrame = AnimFrame;

		if (bCloneSkin && Attachment->LastUpdateTime < GFrameNumber)
		{
			Attachment->LastUpdateTime = GFrameNumber + 4 + (GetIndex() & 3);

			// Copy texture
			appMemcpy(&Attachment->Sprite, &Sprite, sizeof(UTexture*) * 3);
			// Copy multiskins
			appMemcpy(Attachment->MultiSkins, MultiSkins, sizeof(UTexture*) * ARRAY_COUNT(MultiSkins));
			// Misc display
			Attachment->Style = Style;
			Attachment->Fatness = Fatness;
			Attachment->bMeshEnviroMap = bMeshEnviroMap;
			Attachment->bUnlit = bUnlit;
			Attachment->ScaleGlow = ScaleGlow;
		}
	}
	unguardSlow;
}

// Special editor support.
inline AActor* AActor::GetHitActor()
{
	if (HitActor)
		return HitActor;
	return this;
}

inline FLOAT UTexture::GetLevelDeltaTime()
{
	guardSlow(UTexture::GetLevelDeltaTime);
	if (ALevelInfo::CurrentLevel)
		return ALevelInfo::CurrentLevel->LastDeltaTime;
	return SLERP_DELTA;
	unguardSlow;
}
inline FLOAT UTexture::GetLevelTimeSeconds()
{
	guardSlow(UTexture::GetLevelTimeSeconds);
	if (ALevelInfo::CurrentLevel)
		return ALevelInfo::CurrentLevel->TimeSeconds;
	return 0.f;
	unguardSlow;
}
inline DWORD UTexture::GetColorsIndex()
{
	return Palette ? Palette->GetIndex() : 0;
}
inline FColor* UTexture::GetColors()
{
	return Palette ? &Palette->Colors(0) : NULL;
}

inline UBOOL URenderIterator::ShouldDrawIterator(DWORD ShowFlags) const
{
	guardSlow(URenderIterator::ShouldDrawIterator);
	if (bIsParticleEmitter)
		return (ShowFlags & SHOW_Emitters) != 0;
	if (bIsTerrain)
		return (ShowFlags & SHOW_Terrains) != 0;
	return TRUE;
	unguardSlow;
}

#define CHECK_PRIMARY_TRACE_CONDITION \
	if (TraceFlags & TRACE_WalkTextures) \
		return (bBlockTextureTrace!=0); \
	else if(bBlockTextureTrace) \
		return false; \
	if (bBlockAISight && (TraceFlags & TRACE_AISightBlock)) \
		return true; \
	if (((TraceFlags & TRACE_IgnoreHidden) && bHidden) || ((TraceFlags & TRACE_LightBlocking) && !bShadowCast)) \
		return false;

#define SHOULDNT_BLOCK_SECONDARY_CONDITION \
	if (TraceFlags & TRACE_OnlyProjActor) \
	{ \
		if (!bProjTarget && (!bBlockActors || !bBlockPlayers)) \
			return false; \
	} \
	else if (TraceFlags & TRACE_Blocking) \
	{ \
		if (!SourceActor || !SourceActor->IsBlockedBy(this)) \
			return false; \
	}

/*-----------------------------------------------------------------------------
	AActor audio.
-----------------------------------------------------------------------------*/

//
// Get the actor's primitive.
//
inline UPrimitive* AActor::GetPrimitive() const
{
	guardSlow(AActor::GetPrimitive);
	if( CollisionOverride ) return CollisionOverride;
	else if     ( Brush  ) return Brush;
	else if( Mesh   ) return Mesh;
	else              return GetLevel()->Engine->Cylinder;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	NativeReplication definitions
-----------------------------------------------------------------------------*/

static inline UBOOL NEQ(BYTE A,BYTE B) {return A!=B;}
static inline UBOOL NEQ(INT A,INT B) {return A!=B;}
static inline UBOOL NEQ(BITFIELD A,BITFIELD B) {return A!=B;}
static inline UBOOL NEQ(FLOAT& A,FLOAT& B) {return *(INT*)&A!=*(INT*)&B;}
static inline UBOOL NEQ(FVector& A, FVector& B)
{
	if (((INT*)&A)[0] != ((INT*)&B)[0] || ((INT*)&A)[1] != ((INT*)&B)[1] || ((INT*)&A)[2] != ((INT*)&B)[2])
	{
		if (appFloor(A.X) == appFloor(B.X) && appFloor(A.Y) == appFloor(B.Y) && appFloor(A.Z) == appFloor(B.Z))
		{
			B = A; // Rounded components still matched, so don't send and sync up recent.
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
static inline UBOOL NEQ(FRotator& A,FRotator& B) { return (A.Pitch >> 8)!=(B.Pitch >> 8) || (A.Yaw >> 8)!=(B.Yaw >> 8) || (A.Roll >> 8)!=(B.Roll >> 8); }
static inline UBOOL NEQ(UObject* A,UObject* B)
{
	if (AActor::CurrentMap->CanSerializeObject(A))
		return (A!=B);
	if (A && !A->IsPendingKill())
		AActor::bActorStayDirty = TRUE;
	return (B!=NULL);
}
static inline UBOOL NEQ_ND(UObject* A, UObject* B)
{
	if (AActor::CurrentMap->CanSerializeObject(A))
		return (A != B);
	return (B != NULL);
}
static inline UBOOL NEQ(FName& A,FName B) {return *(INT*)&A!=*(INT*)&B;}
static inline UBOOL NEQ(FColor& A, FColor& B) { return GET_COLOR_DWORD(A) != GET_COLOR_DWORD(B); }
static inline UBOOL NEQ(FPlane& A,FPlane& B)
{
	if (((INT*)&A)[0] != ((INT*)&B)[0] || ((INT*)&A)[1] != ((INT*)&B)[1] || ((INT*)&A)[2] != ((INT*)&B)[2] || ((INT*)&A)[3] != ((INT*)&B)[3])
	{
		if (appFloor(A.X) == appFloor(B.X) && appFloor(A.Y) == appFloor(B.Y) && appFloor(A.Z) == appFloor(B.Z) && appFloor(A.W) == appFloor(B.W))
		{
			B = A;
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
static inline UBOOL NEQ(const FString& A,const FString& B) {return A!=B;}
static inline UBOOL NEQ(class AAmmo* A, class AAmmo* B)
{
	return NEQ((UObject*)A, (UObject*)B);
}

#define FORCEREP(Cls,Var) \
	{ \
		static UProperty* sp##Var = FindObjectChecked<UProperty>(Cls::StaticClass(),TEXT(#Var)); \
		*RepLink++ = sp##Var->RepIndex; \
	}

#define DOREP(Cls,Var) \
	if( NEQ(Var,((Cls*)Recent)->Var) ) \
	{ \
		static UProperty* sp##Var = FindObjectChecked<UProperty>(Cls::StaticClass(),TEXT(#Var)); \
		*RepLink++ = sp##Var->RepIndex; \
	}

#define DOREP_NODIRTY(Cls,Var) \
	if( NEQ_ND(Var,((Cls*)Recent)->Var) ) \
	{ \
		static UProperty* sp##Var = FindObjectChecked<UProperty>(Cls::StaticClass(),TEXT(#Var)); \
		*RepLink++ = sp##Var->RepIndex; \
	}

#define DOREPARRAY(Cls,Var) \
	{ \
		static UProperty* sp##Var = FindObjectChecked<UProperty>(Cls::StaticClass(),TEXT(#Var)); \
		for( INT i=0; i<ARRAY_COUNT(Var); i++ ) \
			if( NEQ(Var[i],((Cls*)Recent)->Var[i]) ) \
				*RepLink++ = sp##Var->RepIndex+i; \
	}

#define DOREPARRAY_NODIRTY(Cls,Var) \
	{ \
		static UProperty* sp##Var = FindObjectChecked<UProperty>(Cls::StaticClass(),TEXT(#Var)); \
		for( INT i=0; i<ARRAY_COUNT(Var); i++ ) \
			if( NEQ_ND(Var[i],((Cls*)Recent)->Var[i]) ) \
				*RepLink++ = sp##Var->RepIndex+i; \
	}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
