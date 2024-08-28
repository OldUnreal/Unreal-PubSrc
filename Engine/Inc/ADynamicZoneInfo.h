/*=============================================================================
    ADynamicZoneInfo.h
=============================================================================*/

ADynamicZoneInfo(){}
UBOOL Tick( FLOAT DeltaTime, enum ELevelTick TickType );
void NotifyActorMoved();
void LinkUpZone();
void Spawned();
void PostLoad();
void PostScriptDestroyed();
void UpdateActorZones();
void RenderSelectInfo( struct FSceneNode* Frame );
BYTE ShouldFilterZone(const FVector& Point, AActor* InActor, AZoneInfo* Current);
void UpdateBounds();

/*-----------------------------------------------------------------------------
    The End.
-----------------------------------------------------------------------------*/
