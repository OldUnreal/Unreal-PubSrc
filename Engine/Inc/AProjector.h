/*=============================================================================
	AProjector.h
=============================================================================*/

AProjector() {}

void AttachPrjDecal(BYTE bNoActors=0);
void DeattachPrjDecal(BYTE bCleanup=0);
void AttachActor( AActor* Other );
void DeattachActor( AActor* Other=NULL, bool bCleanup=false );
void Spawned();
void RenderSelectInfo( struct FSceneNode* Frame );
void PostScriptDestroyed();
void PostEditMove();
void Modify();
bool PointCheck( const FVector& Point, const FVector& Extent );
void performPhysics(FLOAT DeltaSeconds);
void NotifyBaseMoved();
void PostLoad();
void Serialize(FArchive& Ar);
UBOOL ShouldExportProperty(UProperty* Property) const;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
