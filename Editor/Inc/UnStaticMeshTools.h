
/* Create a simple collision model for a static mesh actor.
 @ Owner = The StaticMesh Actor owner. */
EDITOR_API extern void MakeCollisionModel( AActor* Owner );

/* Create a simple collision model for a mesh actor. */
EDITOR_API extern UModel* MakeCollisionModel(FName BrName, UObject* InPck, const TCHAR* File);

/* Import a Wavefront .OBJ text to static mesh object.
 @ S = OBJ file loaded to string.
 @ M = Static mesh object to import for. */
EDITOR_API extern void ImportObjFile(FString& S, UStaticMesh* M, UBOOL bIsUV2Map = FALSE);

/* Export a mesh to Wavefront .OBJ text.
 @ A = Output device to write on.
 @ Owner = The desired exporting mesh owner. */
EDITOR_API extern void ExportObjFile(FOutputDevice& A, AActor* Owner, UBOOL bWriteUV2Map = FALSE);

/* Convert a StaticMesh to Brush.
 @ SMeshActor = Desired static mesh actor. */
EDITOR_API extern void ConvertMeshToBrush( AActor* SMeshActor );

/* Convert a bunch of Brush or Mesh actors into one StaticMesh.
 @ TA = Array of Brush/Mesh actors.
 @ MeshName = Desired name of static mesh to build.
 @ MeshPackage = Package name of the static mesh. */
EDITOR_API extern void ConvertBrushToMesh( TArray<AActor*>& TA, const TCHAR* MeshName, const TCHAR* MeshPackage, INT Cuts );
EDITOR_API extern void ImportOBJ( const TCHAR* File, const TCHAR* MeshName, const TCHAR* PackName );
EDITOR_API extern void ExportOBJ( const TCHAR* File, const TCHAR* ViewOwner );
