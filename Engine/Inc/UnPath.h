/*=============================================================================
	UnPath.h: Path node creation and ReachSpec creations and management specific classes
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/

#define DEBUGGINGPATHS  1 //1 to put path info in log
#define MAXMARKERS 3000 //bound number of turn markers
#define MAXREACHSPECS 3000 //bound number of reachspecs 

//Reachability flags - using bits to save space

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

// path node placement parameters
#define MAXWAYPOINTDIST 2.f  // max distance to a usable waypoint to avoid placing a new one after left turn
							// (ratio to collisionradius)
 
#define MAXSORTED 32
class FSortedPathList
{
public:
	AActor *Path[MAXSORTED];
	INT Dist[MAXSORTED];
	int numPoints;
	int checkPos;

	FSortedPathList()
	 : numPoints(0)
	 , checkPos(0)
	{}

	void addPath(AActor * node, INT dist);
	void removePath(int p);
	int findEndPoint(APawn *Searcher, INT &startanchor); 
	int checkAnchorPath(APawn *Searcher, FVector Dest, AActor* GoalActor=NULL);
	void expandAnchor(APawn *Searcher); 
	void findAltEndPoint(APawn *Searcher, AActor *&bestPath); 
	void FindVisiblePaths(APawn *Searcher, FVector Dest, FSortedPathList *DestPoints, INT bClearPaths, INT &startanchor, INT &endanchor);
	ANavigationPoint* findStartAnchor( APawn *Searcher, UBOOL bMarkAll );
	ANavigationPoint* findEndAnchor( APawn *Searcher, AActor *GoalActor, FVector EndLocation, UBOOL bOnlyCheckVisible, UBOOL bMarkAll );
};

class ENGINE_API FPathBuilder
{
public:
	
	int buildPaths (ULevel *ownerLevel, int optimization);
	int removePaths (ULevel *ownerLevel);
	int showPaths (ULevel *ownerLevel);
	int hidePaths (ULevel *ownerLevel);
	void definePaths (ULevel *ownerLevel);
	void undefinePaths (ULevel *ownerLevel);
	void CleanupPruned();

private:
	ULevel * Level;
	APawn * Scout;

	int Prune(AActor *Node);
	INT specFor(AActor* Start, AActor* End);
	int createPaths (int optimization);
	ANavigationPoint*	newPath(FVector spot);
	void getScout();
	int findScoutStart(FVector start);
	void Pass2From(FVector start, FVector moveDirection, FLOAT TurnDir);
	int TestWalk(FVector WalkDir, FCheckResult Hit, FLOAT Threshold, INT bAdjust);
	INT TestReach(FVector Start, FVector End);
	INT ValidNode(ANavigationPoint *node, AActor *node2);
	void testPathsFrom(FVector start);
	void addReachSpecs(AActor * start);
	int insertReachSpec(INT *SpecArray, FReachSpec &Spec);
	void FindBlockingNormal(FVector &BlockNormal);
	void addVisNoReach(AActor * start);
};
