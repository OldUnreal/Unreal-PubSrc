/*=============================================================================
	AActor.h.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Constructors.
	AActor(){}
	void Destroy();

	// UObject interface.
	void ProcessEvent( UFunction* Function, void* Parms, void* Result=NULL );
	void ProcessState( FLOAT DeltaSeconds );
	UBOOL ProcessRemoteFunction( UFunction* Function, void* Parms, FFrame* Stack );
	void ProcessDemoRecFunction( UFunction* Function, void* Parms, FFrame* Stack );
	void Serialize( FArchive& Ar );
	void InitExecution();
	void PostEditChange();
	void PostLoad();
	void onPropertyChange(UProperty* Property, UProperty* OuterProperty);
	void SetNetworkDirty();
	UBOOL ShouldExportProperty(UProperty* Property) const;
	UBOOL ImportScriptVariable(UProperty* Property, const TCHAR* Value);
	UBOOL AllowScriptDestroy()
	{
		return (XLevel==NULL);
	}

	// AActor interface.
	class ULevel* GetLevel() const;
	UBOOL IsPlayerPawn() const;
	class APlayerPawn* GetPlayerPawn();
	UBOOL IsPlayer() const;
	UBOOL IsOwnedBy( const AActor *TestOwner ) const;
	FLOAT WorldSoundRadius() const {return 25.f * ((INT)SoundRadius+1);}
	FLOAT WorldVolumetricRadius() const {return 25.f * ((INT)VolumeRadius+1);}
	UBOOL IsBlockedBy( const AActor* Other ) const;
	UBOOL IsInZone( const AZoneInfo* Other ) const;
	UBOOL IsBasedOn( const AActor *Other ) const;
	UBOOL CheckThisEncroachment();
	UBOOL SendToLevel(ALevelInfo* LevelOther, const FVector& Pos, const FRotator& Rot);
	virtual void OnChangedLevel(ALevelInfo* OldLevel);
	virtual FLOAT GetNetPriority( AActor* Sent, FLOAT Time, FLOAT Lag );
	virtual FLOAT WorldLightRadius() const {return 25.f * ((INT)LightRadius+1);}
	virtual UBOOL Tick( FLOAT DeltaTime, enum ELevelTick TickType );
	virtual void PostEditMove();
	virtual void PreRaytrace() {}
	virtual BYTE HasSlowRayTrace() { return 0; } // Return 1 to run PostRaytrace on another thread.
	virtual void PostRaytrace(FThreadLock& Mutex) {}
	virtual void Spawned() {}
	virtual void PreInitLevel();
	virtual void PreBeginPlay(UBOOL bPreInitGame);
	virtual void BeginPlay();
	virtual void PostBeginPlay();
	virtual void PostScriptDestroyed();
	virtual void ScriptDestroyed();
	virtual void NotifyActorMoved() {}
	virtual void PreNetReceive();
	virtual void PostNetReceive();
	virtual INT* NativeReplicationRep(NETCOND_PARAMS);
	virtual void InitDefaultValues();
	virtual void ZoneChange(class AZoneInfo* NewZone);
	virtual UBOOL IsEncroacher() const;
	virtual UBOOL EncroachingOn(AActor* Other);
	virtual UBOOL RanInto(AActor* Other);
	virtual UTexture* GetSkin( INT Index );
	virtual FMeshAnimSeq* GetAnim( FName SequenceName );
	virtual void TagPersistentActors(); // Tag actors we plan to keep when loading level in-game.
	void PlayAnimation(FName NewSeq, FLOAT Rate = 1.f, FLOAT TweenTime = -1.f, UBOOL bLoop = FALSE, FLOAT MinRate = 0.f, UBOOL bTweenFrame = FALSE, UBOOL bNetwork = TRUE, FOutputDevice* Out = GLog);
	void TweenAnimation(FName NewSeq, FLOAT TweenTime, UBOOL bNetwork = TRUE, FOutputDevice* Out = GLog);
	virtual FCoords ToLocal() const
	{
		return GMath.UnitCoords / FScale(DrawScale*DrawScale3D) / Rotation / (Location+PrePivot);
	}
	virtual FCoords ToWorld() const
	{
		return GMath.UnitCoords * (Location+PrePivot) * Rotation * FScale(DrawScale*DrawScale3D);
	}
	FLOAT LifeFraction()
	{
		return Clamp( 1.f - LifeSpan / GetClass()->GetDefaultActor()->LifeSpan, 0.f, 1.f );
	}
	FVector GetCylinderExtent() const {return FVector(CollisionRadius,CollisionRadius,CollisionHeight);}
	AActor* GetTopOwner();
	virtual UBOOL ShouldBaseOnStart() const;

	// Corona & seeing checks
	virtual FStaticLightActor* GetStaticLights() { return NULL; }
	virtual void DrawCorona( struct FSceneNode* Frame, FLOAT Delta ); // Draw Light actor corona
	virtual BYTE ActorCanSee( APlayerPawn* RealViewer, AActor* Viewer, const FVector& SrcLocation );

	// AActor collision functions.
	UPrimitive* GetPrimitive() const;
	UBOOL IsOverlapping( AActor *Other );
	virtual bool ShouldTrace( DWORD TraceFlags, AActor* SourceActor );

	// AActor general functions.
	void BeginTouch(AActor *Other);
	void EndTouch(AActor *Other, UBOOL NoNotifySelf);
	void SetOwner( AActor *Owner );
	virtual void OwnerChanged();

	virtual UBOOL IsDynamicBSP() const { return FALSE; } // This actor should do dynamic BSP tracking.
	virtual UBOOL IsDynamicBSPHidden() const { return FALSE; } // Dynamic BSP is currently hidden.
	UBOOL IsBrush()       const;
	UBOOL IsStaticBrush() const;
	UBOOL IsMovingBrush() const;
	UBOOL IsAnimating() const
	{
		return
			(AnimSequence!=NAME_None)
		&&	(AnimFrame>=0 ? AnimRate!=0.f : TweenRate!=0.f);
	}
	void SetCollision(UBOOL NewCollideActors, UBOOL NewBlockActors, UBOOL NewBlockPlayers, UBOOL NewBlockRbPhys = FALSE);
	void SetCollisionSize( FLOAT NewRadius, FLOAT NewHeight);
	void SetBase(AActor *NewBase, UBOOL bNotifyActor=1);
	FRotator GetViewRotation() const;
	UBOOL IsDynamicLight() const;
	UBOOL ShouldBakeLight() const;
	virtual FBox GetVisibilityBox();
	virtual FLOAT GetZoneDeltaTime( const FLOAT& InDelta );
	void TickTimers( FLOAT DeltaSeconds );

	// AActor audio.
	void MakeSound( USound *Sound, FLOAT Radius=0.f, FLOAT Volume=1.f, FLOAT Pitch=1.f );
	void CheckHearSound(APawn* Hearer, INT Id, USound* Sound, FVector Parameters, FLOAT RadiusSquared);

	// Physics functions.
	virtual void setPhysics(BYTE NewPhysics, AActor *NewFloor = NULL);
	void FindBase(UBOOL bNotifyActor = 1);
	virtual void performPhysics(FLOAT DeltaSeconds);
	void physProjectile(FLOAT deltaTime, INT Iterations);
	void processHitWall(FVector HitNormal, AActor *HitActor);
	virtual void processLanded(FVector HitNormal, AActor *HitActor, FLOAT remainingTime, INT Iterations);
	void physFalling(FLOAT deltaTime, INT Iterations);
	void physRolling(FLOAT deltaTime, INT Iterations);
	void physicsRotation(FLOAT deltaTime);
	int fixedTurn(int current, int desired, int deltaRate);
	inline void TwoWallAdjust(FVector& DesiredDir, FVector& Delta, FVector& HitNormal, FVector& OldHitNormal, FLOAT HitTime, FVector* VelocityCompensation = nullptr)
	{
		guard(AActor::TwoWallAdjust);

		if ((OldHitNormal | HitNormal) <= 0) //90 or less corner, so use cross product for dir
		{
			FVector NewDir = (HitNormal ^ OldHitNormal);
			NewDir = NewDir.SafeNormal();
			Delta = (Delta | NewDir) * (1.f - HitTime) * NewDir;
			if ((DesiredDir | Delta) < 0)
				Delta = -1 * Delta;
		}
		else //adjust to new wall
		{
			// stijn: added an additional 0.0001UU adjustment in the direction of the hit normal here.
			// See my comments in UnPhysic.cpp (AActor::physFalling) for an explanation.
			Delta = (Delta - HitNormal * (Delta | HitNormal)) * (1.f - HitTime) + 0.002f * HitNormal;
			if (VelocityCompensation)
				*VelocityCompensation = -0.002f * HitNormal;
			if ((Delta | DesiredDir) <= 0)
				Delta = FVector(0, 0, 0);
		}
		unguard;
	}
	void physPathing(FLOAT DeltaTime);
	void physTrailer(FLOAT DeltaTime);
	int moveSmooth(FVector Delta);
	virtual bool OverrideMeshRender( struct FSceneNode* Frame ) { return false; }
	void CleanupCache( UBOOL bFullCleanup );
	EActorRenderPass GetRenderPass() const;
	virtual void NotifyBaseMoved(){};
	UBOOL IsNotDistanceCulled(const FVector& CamPos) const;
	UBOOL ShouldDistanceFade(const FVector& CamPos, FLOAT* Alpha) const;
	static FVector SuggestFallVelocity(const FVector& Start, const FVector& End, const FVector& Gravity, FLOAT BaseZ, FLOAT ZSpeed, FLOAT MaxXYSpeed);

	// Rigidbody Physics Engine
	virtual void InitRbPhysics();
	virtual void ExitRbPhysics();
	virtual void DrawRbDebug();
	virtual PX_PhysicsObject* GetRbPhysics();
	virtual void OnJointBroken(class UPXJ_BaseJoint* Joint);
	virtual void PhysicsImpact(FLOAT Threshold, const FVector& HitLocation, const FVector& HitNormal, AActor* Other);

	// AI functions.
	void CheckNoiseHearing(FLOAT Loudness);
	int TestCanSeeMe(APlayerPawn *Viewer);

	// Special editor behavior
	AActor* GetHitActor();
	virtual UBOOL IsValidOnImport();
	virtual UBOOL ShouldSunlightLit(const INT iZoneNumber) const; // LE_Sunlight should be baked into specific leaf.

	// Networking cache.
	static UPackageMap* CurrentMap;
	static INT CurrentNetVer;
	static UBOOL bActorStayDirty;

	// Editor
	virtual void RenderSelectInfo( FSceneNode* Frame );
	virtual void NotifyPathsDefine( UBOOL bPreNotify );
	virtual void NoteDuplicate( AActor* Src );

	// Natives.
	DECLARE_FUNCTION(execPollSleep)
	DECLARE_FUNCTION(execPollFinishAnim)
	DECLARE_FUNCTION(execPollFinishInterpolation)
	DECLARE_FUNCTION(execFunctionA);
	DECLARE_FUNCTION(execFunctionB);
	DECLARE_FUNCTION(execFunctionC);
	DECLARE_FUNCTION(execSpawnAct); // LEGACY support!
	DECLARE_FUNCTION(execNativeExec); // LEGACY support!

	// Global variable
	static FThreadLock GBasedListMutex;
	static FThreadLock GActorKillMutex;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
