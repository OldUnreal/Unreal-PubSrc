	static UBOOL bEnableMultiThreading;
	static USound* CurrentSound;
	static UMusic* CurrentMusic;
	static UMesh* CurrentMesh;
	static UBOOL IgnoreActorNotifications;
	static TMap<UBoolProperty*, FName>*	CppPackedBools;
	static TArray<UObject*> BuilderButtons; // Objects to be kept out of garbage collector.
	static TArray<FVertexHit> VertexHitList;

	// Constructor.
	void StaticConstructor();
	UEditorEngine();

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );
	void PostEditChange();

	// FNotify interface.
	void NotifyDestroy( void* Src );
	void NotifyPreChange( void* Src );
	void NotifyPostChange( void* Src );
	void NotifyExec( void* Src, const TCHAR* Cmd );

	// UEngine interface.
	void Init();
	virtual void InitEditor();
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	UBOOL HookExec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	int Key( UViewport* Viewport, EInputKey Key, INT KeyValue );
	void Tick( FLOAT DeltaSeconds );
	void Draw( UViewport* Viewport, UBOOL Blit=1, BYTE* HitData=NULL, INT* HitSize=NULL );
	void MouseDelta( UViewport* Viewport, DWORD Buttons, FLOAT DX, FLOAT DY );
	void MousePosition( UViewport* Viewport, DWORD Buttons, FLOAT X, FLOAT Y );
	void Click( UViewport* Viewport, DWORD Buttons, FLOAT X, FLOAT Y );
	void SetClientTravel( UPlayer* Viewport, const TCHAR* NextURL, UBOOL bItems, ETravelType TravelType ) {}
	void UpdateVertices();
	void Flush(UBOOL AllowPrecache);
	static void TryMonoCompression(UTexture* T);

	virtual void edSetClickLocation( FVector& InLocation );

	// General functions.
	virtual UBOOL SafeExec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	void ExecMacro( const TCHAR* Filename, FOutputDevice& Ar );
	FString SearchCaseInsensitive(const TCHAR* CIName);
	virtual void Cleanse( UBOOL Redraw, const TCHAR* TransReset );
	virtual void FinishAllSnaps( ULevel* Level );
	virtual void RedrawLevel( ULevel* Level );
	void FlushViewports(ULevel* Level);
	virtual void ResetSound();
	virtual AActor* AddActor(ULevel* Level, UClass* Class, const FVector& V, const FVector& Normal, UBOOL bSnap = FALSE, UBOOL bForce = TRUE);
	virtual void NoteSelectionChange( ULevel* Level );
	virtual void NoteActorMovement( ULevel* Level );
	virtual void SetPivot( FVector NewPivot, UBOOL SnapPivotToGrid, UBOOL MoveActors );
	virtual void ResetPivot();
	virtual void UpdatePropertiesWindows();
	virtual class UTransactor* CreateTrans();
	void CleanupMapGarbage( ULevel* Level );

	// Editor mode virtuals from UnEdCam.cpp.
	virtual void edcamSetMode( INT Mode );
	virtual void edcamSetTerrainMode(const FTerrainModeSettings& NewMode);
	virtual int edcamMode( UViewport* Viewport );
	virtual void LoadFullPackage( UObject* Obj );

	// Editor CSG virtuals from UnEdCsg.cpp.
	virtual void csgPrepMovingBrush( AActor* Actor );
	virtual void csgCopyBrush( ABrush* Dest, ABrush* Src, DWORD PolyFlags, DWORD ResFlags, UBOOL IsMovingBrush );
	virtual ABrush*	csgAddOperation( ABrush* Actor, ULevel* Level, DWORD PolyFlags, ECsgOper CSG );
	virtual void csgRebuild(ULevel* Level, UBOOL bVisibleOnly, INT BuildCount = INDEX_NONE, ESemiBuildFlags SemiSolidFlags = SEMI_BuildAll);
	virtual const TCHAR* csgGetName( ECsgOper CsgOper );
	virtual void LoadBrushDataFromFile( const TCHAR* FileName, ULevel* Map );
	virtual void SaveBrushDataToFile( const TCHAR* FileName, bool bSelected, ULevel* Map );

	// Editor EdPoly/BspSurf assocation virtuals from UnEdCsg.cpp.
	virtual INT polyFindMaster( UModel* Model, INT iSurf, FPoly& Poly );
	virtual void polyUpdateMaster( UModel* Model, INT iSurf, INT UpdateTexCoords, INT UpdateBase );

	// Bsp Poly search virtuals from UnEdCsg.cpp.
	virtual void polySetAndClearPolyFlags( UModel* Model, DWORD SetBits, DWORD ClearBits, INT SelectedOnly, INT UpdateMaster, DWORD SetXBits=0, DWORD ClearXBits=0);

	// Selection.
	virtual void SelectNone( ULevel* Level, UBOOL Notify );
	UObject* edGetSelection(UClass* ObjClass);

	// Bsp Poly selection virtuals from UnEdCsg.cpp.
	virtual void polyResetSelection( UModel* Model );
	virtual void polySelectAll ( UModel* Model );
	virtual void polySelectMatchingGroups( UModel* Model );
	virtual void polySelectMatchingItems( UModel* Model );
	virtual void polySelectCoplanars( UModel* Model );
	virtual void polySelectAdjacents( UModel* Model );
	virtual void polySelectAdjacentWalls( UModel* Model );
	virtual void polySelectAdjacentFloors( UModel* Model );
	virtual void polySelectAdjacentSlants( UModel* Model );
	virtual void polySelectMatchingBrush( UModel* Model );
	virtual void polySelectMatchingTexture( UModel* Model );
	virtual void polySelectMatchingPolyFlags( UModel* Model );
	virtual void polySelectReverse( UModel* Model );
	virtual void polyMemorizeSet( UModel* Model );
	virtual void polyRememberSet( UModel* Model );
	virtual void polyXorSet( UModel* Model );
	virtual void polyUnionSet( UModel* Model );
	virtual void polyIntersectSet( UModel* Model );
	virtual void polySelectZone( UModel *Model );

	// Poly texturing virtuals from UnEdCsg.cpp.
	virtual void polyTexPan( UModel* Model, INT PanU, INT PanV, INT Absolute );
	virtual void polyTexScale( UModel* Model,FLOAT UU, FLOAT UV, FLOAT VU, FLOAT VV, UBOOL Absolute );
	virtual void polyTexAlign( UModel* Model, enum ETexAlign TexAlignType, DWORD Texels );

	// Map brush selection virtuals from UnEdCsg.cpp.
	virtual void mapSelectOperation( ULevel* Level, ECsgOper CSGOper );
	virtual void mapSelectFlags(ULevel* Level, DWORD Flags );
	virtual void mapSelectFirst( ULevel* Level );
	virtual void mapSelectLast( ULevel* Level );
	virtual void mapBrushGet( ULevel* Level );
	virtual void mapBrushPut( ULevel* Level );
	virtual void mapSendToFirst( ULevel* Level );
	virtual void mapSendToLast( ULevel* Level );
	virtual void mapSetBrush( ULevel* Level, enum EMapSetBrushFlags PropertiesMask, _WORD BrushColor, FName Group, DWORD SetPolyFlags, DWORD ClearPolyFlags, DWORD CSGOper );

	// Editor actor virtuals from UnEdAct.cpp.
	virtual void edactSelectAll( ULevel* Level );
	virtual void edactSelectMatching( ULevel* Level );
	virtual void edactSelectInside( ULevel* Level );
	virtual void edactSelectInvert( ULevel* Level );
	virtual void edactSelectOfClass( ULevel* Level, UClass* Class );
	virtual void edactSelectSubclassOf( ULevel* Level, UClass* Class );
	virtual void edactSelectDeleted( ULevel* Level );
	virtual void edactDeleteSelected( ULevel* Level );
	virtual void edactDuplicateSelected( ULevel* Level );
	virtual void edactCopySelected( ULevel* Level );
	virtual void edactPasteSelected( ULevel* Level );
	virtual void edactPasteSelectedPos( ULevel* Level );
	virtual void edactReplaceSelectedBrush( ULevel* Level );
	virtual void edactReplaceSelectedWithClass(ULevel* Level, UClass* Class, UBOOL bKeep);
	virtual void edactReplaceClassWithClass( ULevel* Level, UClass* Class, UClass* WithClass );
	virtual void edactAlignVertices( ULevel* Level );
	virtual void edactHideSelected( ULevel* Level );
	virtual void edactHideUnselected( ULevel* Level );
	virtual void edactHideInvert(ULevel* Level);
	virtual void edactUnHideAll( ULevel* Level );
	virtual void edactApplyTransform( ULevel* Level );
	virtual void edactApplyTransformToBrush( ABrush* InBrush );
	virtual void TransformPerma( ULevel* Map, bool bSelected );
	virtual void edactBoxSelect( UViewport* Viewport, ULevel* Level, FVector Start, FVector End );
	virtual void MakeTessellate();
	static UBOOL TessellatePoly(UPolys* P, const INT Index);
	virtual void MakeMerged(ULevel* Map);
	virtual void FlipFaces(ULevel* Map);
	virtual void DeleteFaces(ULevel* Map);
	virtual void edNoteActor(AActor* Other, EActorAction Action);

	static void ExportActorProperties(AActor* Other, FOutputDevice& Out);
	static void ImportActorProperties(AActor* Other, const TCHAR* Str);

	// Copy/Paste object defaultproperties from/to clipboard.
	static void CopyDefPropsToClipboard(UClass* Class);
	static void PasteDefPropsFromClipboard(UClass* Class);

	// Bsp virtuals from UnBsp.cpp.
	virtual void bspRepartition( ULevel* Level, INT iNode, UBOOL Simple );
	virtual INT bspAddVector( UModel* Model, FVector* V, UBOOL Exact );
	virtual INT bspAddPoint( UModel* Model, FVector* V, UBOOL Exact );
	virtual INT bspNodeToFPoly( UModel* Model, INT iNode, FPoly* EdPoly );
	virtual void bspBuild( UModel* Model, enum EBspOptimization Opt, INT Balance, INT RebuildSimplePolys, INT iNode );
	virtual void bspRefresh( UModel* Model, UBOOL NoRemapSurfs );
	virtual void bspCleanup( UModel* Model );
	virtual void bspBuildBounds( UModel* Model, DWORD Flags=0 );
	virtual void bspBuildFPolys( UModel* Model, UBOOL SurfLinks, INT iNode );
	virtual void bspMergeCoplanars( UModel* Model, UBOOL RemapLinks, UBOOL MergeDisparateTextures );
	virtual INT bspBrushCSG( ABrush* Actor, UModel* Model, DWORD PolyFlags, ECsgOper CSGOper, UBOOL RebuildBounds, UBOOL MergePolys = 1 );
	virtual void bspOptGeom( UModel* Model );
	virtual void bspValidateBrush( UModel* Brush, INT ForceValidate, INT DoStatusUpdate );
	virtual void bspUnlinkPolys( UModel* Brush );
	virtual INT	bspAddNode( UModel* Model, INT iParent, enum ENodePlace ENodePlace, DWORD NodeFlags, FPoly* EdPoly );
	virtual INT bspVerifyHoles(UModel* Model, UBOOL bRefresh);
	void bspBuildInfiniteModel(FPoly* Polys) const;
	void bspCreateNewFace();
	void bspSplitVerts();
	void bspDeleteVertices();
	void bspSnapGrid(UBOOL bSelected);
	static void bspCleanupVertPool(UModel* Model);

	// Shadow virtuals (UnShadow.cpp).
	virtual void shadowIlluminateBsp( ULevel* Level, UBOOL Selected, UBOOL bVisibleOnly);

	// Mesh functions (UnMeshEd.cpp).
	virtual UMesh* meshImport( const TCHAR* MeshName, UObject* InParent, const TCHAR* AnivFname, const TCHAR* DataFname, UBOOL Unmirror, UBOOL ZeroTex, INT UnMirrorTex, ULODProcessInfo* LODInfo);
	virtual void modelImport( const TCHAR* MeshName, UObject* InParent, const TCHAR* SkinFname, UBOOL Unmirror, UBOOL ZeroTex, INT UnMirrorTex, ULODProcessInfo* LODInfo );
	virtual void meshBuildBounds( UMesh* Mesh );
	virtual void modelBuildBounds( USkeletalMesh* Mesh );
	virtual void meshLODProcess( ULodMesh* Mesh, ULODProcessInfo* LODInfo);
	virtual void modelLODProcess( USkeletalMesh* Mesh, ULODProcessInfo* LODInfo, USkelImport* RawData );
	virtual void meshDropFrames( UMesh* Mesh, INT StartFrame, INT NumFrame );
	virtual void ExportMesh( UMesh* Mesh, const TCHAR* FileName );

	// Skeletal animation, digest and linkup functions (UnMeshEd.cpp).
	virtual void modelAssignWeaponBone( USkeletalMesh* Mesh, FName TempFname );
	virtual INT  animGetBoneIndex( UAnimation* Anim, FName TempFname );
	virtual void modelSetWeaponPosition( USkeletalMesh* Mesh, FCoords WeaponCoords );
	virtual void animationImport( const TCHAR* AnimName, UObject* InParent, const TCHAR* DataFname, UBOOL Unmirror, UBOOL ImportSeqs, FLOAT CompDefault );
	virtual void digestMovementRepertoire( UAnimation* Anim );
	virtual void movementDigest( UAnimation* Anim, INT Index );

	// Visibility.
	virtual void TestVisibility( ULevel* Level, UModel* Model, int A, int B );
	static void VisualizeBspHulls(UModel* Model);
	static void FixSurfZones(UModel* Model);

	// Scripts.
	virtual int MakeScripts( FFeedbackContext* Warn, UBOOL MakeAll, UBOOL Booting, UBOOL bOptimizeCode );
	virtual int CheckScripts( FFeedbackContext* Warn, UClass* Class, FOutputDevice& Ar );

	// Topics.
	virtual void Get( const TCHAR* Topic, const TCHAR* Item, FOutputDevice& Ar );
	virtual void Set( const TCHAR* Topic, const TCHAR* Item, const TCHAR* Value );
	virtual void EdCallback( DWORD Code, UBOOL Send );
	virtual void SendKeyStroke(BYTE iKey, UBOOL bCtrl, UBOOL bAlt, UBOOL bShift);
	virtual const FEdKeyBinding* FindMappedKey(const TCHAR* Cmd);

	// Bsp statistics.
	struct FBspStats
	{
		INT Polys;			// Number of BspSurfs.
		INT Nodes;			// Total number of Bsp nodes.
		INT MaxDepth;		// Maximum tree depth.
		INT AvgDepth;		// Average tree depth.
		INT Branches;		// Number of nodes with two children.
		INT Coplanars;		// Number of nodes holding coplanar polygons (in same plane as parent).
		INT Fronts;			// Number of nodes with only a front child.
		INT Backs;			// Number of nodes with only a back child.
		INT Leaves;			// Number of nodes with no children.
		INT FrontLeaves;	// Number of leaf nodes that are in front of their parent.
		INT BackLeaves;		// Number of leaf nodes that are in back of their parent.
		INT DepthCount;		// Depth counter (used for finding average depth).
		INT Brushes;		// Number of brushes in level.
		INT Zones;			// Number of zones in level.
		INT LightCount;		// Number of lights in level.

		inline void Clear()
		{
			appMemzero(this, sizeof(FBspStats));
		}
	};
	virtual void GetBSPStats(FBspStats& Stats);
	virtual void GetSurfStats(UBOOL bSurfStats, const TCHAR*& KeyNames, const TCHAR*& KeyValues);

	// Far-plane Z clipping state control functions.
	virtual void SetZClipping();
	virtual void ResetZClipping();

	// Editor rendering functions.
	virtual void DrawFPoly( struct FSceneNode* Frame, FPoly *Poly, FPlane WireColor, DWORD LineFlags );
	virtual void DrawGridSection( struct FSceneNode* Frame, INT ViewportLocX, INT ViewportSXR, INT ViewportGridY, FVector* A, FVector* B, FLOAT* AX, FLOAT* BX, INT AlphaCase );
	virtual void DrawWireBackground( struct FSceneNode* Frame );
	virtual void DrawLevelBrushes( struct FSceneNode* Frame, UBOOL bStatic, UBOOL bDynamic, UBOOL bActive );
	virtual void DrawLevelBrush( struct FSceneNode* Frame, AActor* Actor, UBOOL bStatic, UBOOL bDynamic, UBOOL bActive );
	virtual void DrawBoundingBox( struct FSceneNode* Frame, FBox* Bound, AActor* Actor );
	void RepaintAllViewports() const; // Redraw all 3D/ortho viewports (not mesh/texture browsers).

	// MeshEditor functionality.
	void DrawMeshEd(struct FSceneNode* Frame);
	void MeshEdLoadFile(const TCHAR* FilePath);
	void MeshEdSaveFile(const TCHAR* FilePath);
	void MeshEdClick(UViewport* Viewport, DWORD Buttons, FLOAT MouseX, FLOAT MouseY);
	UBOOL MeshEdGetInfo(DWORD* PolyFlags, BYTE* iTexture, BYTE* iMaxTexture, DWORD* iSmoothGroup, UBOOL& bIsSkeletal);
	void MeshEdSetFlags(DWORD Flags, UBOOL bAdd);
	void MeshEdSetTexture(BYTE NewTexture);
	void MeshEdDeleteSurf();
	void MeshEdNewSurf();
	void MeshEdSurfFlip();
	void MeshEdSelect(INT Mode);
	INT MeshEdKey(UViewport* Viewport, EInputKey Key, INT KeyValue);

	// Multi file mesh exporter.
	static UBOOL meshExportJames(UMesh* Mesh, const TCHAR* FileName);
	static UBOOL meshExport3DS(UMesh* Mesh, const TCHAR* PathName, struct F3DSMeshExportConfig* Config = NULL);

	// Constraints
	virtual void Snap( FVector& Point, FVector GridBase );
	virtual void Snap( FRotator& Rotation );
	virtual UBOOL Snap( ULevel* Level, FVector& Location, FVector GridBase, FRotator& Rotation );

	static UBOOL GForceExactBSP;

	// Ragdoll data.
	static UPX_SkeletalBody* importRagdoll(const TCHAR* File, const TCHAR* RagdollName, const TCHAR* ObjName, UObject* Pck, FLOAT Scaling = 1.f);
