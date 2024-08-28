/*=============================================================================
	APawn.h: Class functions residing in the APawn class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Also defined in UnPath.h
	enum EReachSpecFlags
	{
		R_WALK = 1,	//walking required
		R_FLY = 2,   //flying required 
		R_SWIM = 4,  //swimming required
		R_JUMP = 8,   // jumping required
		R_DOOR = 16,
		R_SPECIAL = 32,
		R_PLAYERONLY = 64
	}; 

	// Constructors.
	APawn() {}

	// APawn interface.
	FLOAT GetNetPriority( AActor* Sent, FLOAT Time, FLOAT Lag );
	virtual INT* NativeReplicationRep(NETCOND_PARAMS);
	void PostScriptDestroyed();
	void ScriptDestroyed();
	void PreBeginPlay(UBOOL bPreInitGame);
	void BeginPlay();
	void OnChangedLevel(ALevelInfo* OldLevel);
	bool ShouldTrace(DWORD TraceFlags, AActor* SourceActor);
	UBOOL ShouldExportProperty(UProperty* Property) const;
	UBOOL ShouldBaseOnStart() const;

	// Latent movement
	void setMoveTimer(FLOAT MoveSize);
	int moveToward(const FVector &Dest, UBOOL bFirstMove=0);
	int rotateToward(const FVector &FocalPoint);
	int PickWallAdjust();

	// Seeing and hearing checks
	BYTE ActorCanSee( APlayerPawn* RealViewer, AActor* Viewer, const FVector& SrcLocation );
	void ShowSelf();
	int CanHear(FVector NoiseLoc, FLOAT Loudness, APawn *Other); 
	DWORD LineOfSightTo(AActor *Other, int bShowSelf = 0);
	void CheckEnemyVisible();
	int walkToward(const FVector &Destination, FLOAT Movesize);
	void SightCheck();

	// reach tests
	int pointReachable(FVector aPoint, int bKnowVisible = 0);
	int actorReachable(AActor *Other, int bKnowVisible = 0);
	int Reachable(FVector aPoint, FLOAT Threshold, AActor* GoalActor);
	int walkReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	int jumpReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	int flyReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	int swimReachable(FVector Dest, FLOAT Threshold, int reachFlags, AActor* GoalActor);
	void jumpLanding(FVector testvel, FVector &Landing, int moveActor = 0);
	int walkMove(FVector Delta, FCheckResult& Hit, AActor* GoalActor = NULL, FLOAT threshold = 4.1f, int bAdjust = 1);
	int flyMove(FVector Delta, AActor* GoalActor, FLOAT threshold = 4.1f, int bAdjust = 1);
	int swimMove(FVector Delta, AActor* GoalActor, FLOAT threshold = 4.1f, int bAdjust = 1);
	int FindBestJump(FVector Dest, FVector vel, FVector &Landing, int moveActor = 0); 
	int FindJumpUp(FVector Dest, FVector vel, FVector &Landing, int moveActor = 0); 
	void SuggestJumpVelocity(FVector Dest, FVector &Vel);

	// Path finding
	AActor* FindBestPathToward( AActor* Goal, FVector Destination, UBOOL bSinglePath, UBOOL bClearPaths, FLOAT DetourWeight=0.f );
	FLOAT findPathTowardBestInventory(AActor *&bestPath, INT bClearPaths, FLOAT MinWeight, INT bPredictRespawns);
	int findRandomDest(AActor *&bestPath);
	int TraverseFrom(class ANavigationPoint *startnode, int moveFlags);
	ANavigationPoint* breadthPathTo( ANavigationPoint *start, UBOOL bSinglePath, INT moveFlags, FLOAT DetourWeight );
	ANavigationPoint* FindAnchor( FLOAT* StartDist );
	FLOAT breadthPathToInventory(AActor *startnode, AActor *&bestPath, int moveFlags, FLOAT bestInventoryWeight, INT bPredictRespawns);
	inline int calcMoveFlags()
	{
		guardSlow(APawn::calcMoveFlags);
		return ( bCanWalk ? R_WALK : 0) | (bCanFly ? R_FLY : 0) | (bCanSwim ? R_SWIM : 0) | (bCanJump ? R_JUMP : 0)
				| (bCanOpenDoors ? R_DOOR : 0) | (bCanDoSpecial ? R_SPECIAL : 0) | (bIsPlayer ? R_PLAYERONLY : 0); 
		unguardSlow;
	}
	void clearPaths();
	void HandleSpecial(AActor *&bestPath);
	int CanMoveTo(AActor *Anchor, AActor *Dest);
	void SetRouteCache(ANavigationPoint *BestPath);
	void ReverseRouteFor(ANavigationPoint *BestDest);

	// Pawn physics modes
	void physWalking(FLOAT deltaTime, INT Iterations);
	void physFlying(FLOAT deltaTime, INT Iterations);
	void physSwimming(FLOAT deltaTime, INT Iterations);
	void physSpider(FLOAT deltaTime, INT Iterations);
	void startSwimming(FVector OldVelocity, FLOAT timeTick, FLOAT remainingTime, INT Iterations);
	void physicsRotation(FLOAT deltaTime, FVector OldVelocity);
	void performPhysics(FLOAT DeltaSeconds);
	void processLanded(FVector HitNormal, AActor *HitActor, FLOAT remainingTime, INT Iterations);
	FVector physLedgeAdjust(FLOAT DeltaTime, FVector AccelDir, FVector Delta, FVector GravDir, UBOOL& bCheckedFall, UBOOL& bMustJump);
	FVector physSlopeAdjust(FVector Delta, FCheckResult& Hit);

	// Natives.
	DECLARE_FUNCTION(execPollWaitForLanding)
	DECLARE_FUNCTION(execPollMoveTo)
	DECLARE_FUNCTION(execPollMoveToward)
	DECLARE_FUNCTION(execPollStrafeTo)
	DECLARE_FUNCTION(execPollStrafeFacing)
	DECLARE_FUNCTION(execPollTurnToward)
	DECLARE_FUNCTION(execPollTurnTo)

private:
	FLOAT Swim(FVector Delta, FCheckResult &Hit); 
	void findWaterLine(FVector Start, FVector &End);
	void stepUp(FVector GravDir, FVector DesiredDir, FVector Delta, FCheckResult &Hit);
	void calcVelocity(FVector AccelDir, FLOAT deltaTime, FLOAT maxSpeed, FLOAT friction, INT bFluid, INT bBrake, INT bBuoyant);
	int findNewFloor(FVector OldLocation, FLOAT deltaTime, FLOAT remainingTime, INT Iterations, FCheckResult& Hit);
	inline int checkFloor(FVector Dir, FCheckResult &Hit);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
