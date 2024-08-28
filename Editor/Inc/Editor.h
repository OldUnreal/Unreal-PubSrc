/*=============================================================================
	Editor.h: Unreal editor public header file.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#ifndef _INC_EDITOR
#define _INC_EDITOR

/*-----------------------------------------------------------------------------
	Dependencies.
-----------------------------------------------------------------------------*/

#include "Engine.h"
#include "lib3ds.h"

#ifndef EDITOR_API
#define EDITOR_API DLL_IMPORT
#endif

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack(push,OBJECT_ALIGNMENT)
#endif

struct EDITOR_API FBuilderPoly
{
	TArray<INT> VertexIndices;
	INT Direction;
	FName ItemName;
	INT PolyFlags;
	FBuilderPoly()
	: VertexIndices(), Direction(0), ItemName(NAME_None)
	{}
};
struct FCompileError
{
	FString ErrorMsg GCC_PACK(INT_ALIGNMENT);
	FString File;
	INT Line;
	FName MsgType;

	FCompileError(const TCHAR* Er, const TCHAR* F, INT l, FName Type)
		: ErrorMsg(Er), File(F), Line(l), MsgType(Type)
	{}
};
struct EDITOR_API FCppCmdHook
{
	FCppCmdHook();
	virtual ~FCppCmdHook();
	virtual UBOOL HookExec(const TCHAR* Cmd, FOutputDevice& Ar) = 0;
};

struct FEditScriptScope
{
	UBOOL bOldScriptable;

	FEditScriptScope()
		: bOldScriptable(GIsScriptable)
	{
		GIsScriptable = TRUE;
	}
	~FEditScriptScope()
	{
		GIsScriptable = bOldScriptable;
	}
};
#define BEGIN_USCRIPT_CODE FEditScriptScope USScope;

// Semi solid build flags
enum ESemiBuildFlags : DWORD
{
	SEMI_BuildAll,
	SEMI_BuildSolids,
	SEMI_BuildSemiSolids,
};

// Used by the multiple vertex editing function to keep track of selected vertices.
struct FVertexHit
{
	// Variables.
	AActor* pBrush;
	INT PolyIndex;
	INT VertexIndex;

	// Constructors.
	FVertexHit()
	{
		pBrush = NULL;
		PolyIndex = VertexIndex = 0;
	}
	FVertexHit(AActor* _pBrush, INT _PolyIndex, INT _VertexIndex)
	{
		pBrush = _pBrush;
		PolyIndex = _PolyIndex;
		VertexIndex = _VertexIndex;
	}

	// Functions.
	UBOOL operator==(const FVertexHit& V) const
	{
		return pBrush == V.pBrush && PolyIndex == V.PolyIndex && VertexIndex == V.VertexIndex;
	}
	UBOOL operator!=(const FVertexHit& V) const
	{
		return pBrush != V.pBrush || PolyIndex != V.PolyIndex || VertexIndex != V.VertexIndex;
	}
};
struct FFaceDragHit
{
	FFaceDragHit(ABrush* InBrush, FPoly* InPoly)
	{
		Brush = InBrush;
		Poly = InPoly;
	}
	ABrush* Brush;
	FPoly* Poly;
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

#include "EditorClasses.h"

/*-----------------------------------------------------------------------------
	Editor public.
-----------------------------------------------------------------------------*/

#define dED_MAX_VIEWPORTS	8

//
// The editor object.
//
EDITOR_API extern class UEditorEngine* GEditor;

#if _WINDOWS
EDITOR_API extern HWND GhwndEditorFrame;
#endif

enum ETerrainPaintMode
{
	TPM_Brush,
	TPM_Flatten,
	TPM_Visibility,
	TPM_EdgeTurn,
	TPM_Material,
	TPM_Decoration,
};

//
// Importing object properties.
//
EDITOR_API const TCHAR* ImportProperties
(
	UObject*			Object,
	const TCHAR*		Data,
	UObject*			InParent,
	FFeedbackContext*	Warn,
	INT*				StartLineIndex=NULL,
	const TCHAR*		ContextName=NULL
);

//
// Editor callback codes.
//
enum EUnrealEdCallbacks
{
	EDC_None						= 0,
	EDC_Browse						= 1,
	EDC_UseCurrent					= 2,
	EDC_CurTexChange				= 10,
	EDC_SelPolyChange				= 20,
	EDC_SelChange					= 21,
	EDC_RtClickTexture				= 23,
	EDC_RtClickPoly,
	EDC_RtClickActor,
	EDC_RtClickVertex,
	EDC_RtClickWindow,
	EDC_RtClickWindowCanAdd,
	EDC_RtClickMeshEdit,
	EDC_MapChange					= 42,
	EDC_ViewportUpdateWindowFrame	= 43,
	EDC_SurfProps					= 44,
	EDC_SaveMap						= 45,
	EDC_SaveMapAs					= 46,
	EDC_LoadMap						= 47,
	EDC_PlayMap						= 48,
	EDC_CamModeChange				= 49,
	EDC_RedrawAllViewports			= 50,
};

//
// Importing & Exporting INT files (gam).
//
EDITOR_API UBOOL IntExport (UObject *Package, const TCHAR *IntName, UBOOL ExportFresh, UBOOL ExportInstances, UBOOL NoDelete=0);
EDITOR_API UBOOL IntExport (const TCHAR *PackageName, const TCHAR *IntName, UBOOL ExportFresh, UBOOL ExportInstances, UBOOL NoDelete=0);
EDITOR_API UBOOL IntMatchesPackage (UObject *Package, const TCHAR *IntName);
EDITOR_API UBOOL IntMatchesPackage (const TCHAR *PackageName, const TCHAR *IntName);
EDITOR_API UBOOL IntMatchesPackage (const TCHAR *PackageName);
EDITOR_API void IntGetNameFromPackageName (const FString &PackageName, FString &IntName);

struct FActorGroupCallback
{
	virtual void NotifyLevelActor(AActor* Other, UEngine::EActorAction Action) {}
};
EDITOR_API extern FActorGroupCallback* GEdCallback;

/*-----------------------------------------------------------------------------
3DS Export Config.
-----------------------------------------------------------------------------*/

// Config struct used for meshExport3DS.
struct F3DSMeshExportConfig
{
	FName StudioTextureExtension; // File extension for texture maps.
	UBOOL SkipStudioTextureMaps;  // Skip texture map file names. At least early Max version doesn't like more then 8.3 file names.
};

/*-----------------------------------------------------------------------------
	FEditorHitObserver.
-----------------------------------------------------------------------------*/

//
// Hit observer for editor events.
//
class EDITOR_API FEditorHitObserver : public FHitObserver
{
public:
	// FHitObserver interface.
	void Click( const FHitCause& Cause, const HHitProxy& Hit )
	{
		if     ( Hit.IsA(TEXT("HBspSurf"        )) ) Click( Cause, *(HBspSurf        *)&Hit );
		else if( Hit.IsA(TEXT("HActor"          )) ) Click( Cause, *(HActor          *)&Hit );
		else if( Hit.IsA(TEXT("HBrushVertex"    )) ) Click( Cause, *(HBrushVertex    *)&Hit );
		else if( Hit.IsA(TEXT("HGlobalPivot"    )) ) Click( Cause, *(HGlobalPivot    *)&Hit );
		else if( Hit.IsA(TEXT("HBrowserTexture" )) ) Click( Cause, *(HBrowserTexture *)&Hit );
		else FHitObserver::Click( Cause, Hit );
	}

	// FEditorHitObserver interface.
	virtual void Click( const FHitCause& Cause, const struct HBspSurf&        Hit );
	virtual void Click( const FHitCause& Cause, const struct HActor&          Hit );
	virtual void Click( const FHitCause& Cause, const struct HBrushVertex&    Hit );
	virtual void Click( const FHitCause& Cause, const struct HGlobalPivot&    Hit );
	virtual void Click( const FHitCause& Cause, const struct HBrowserTexture& Hit );
};

/*-----------------------------------------------------------------------------
	Hit proxies.
-----------------------------------------------------------------------------*/

// Hit a texture view.
struct HTextureView : public HHitProxy
{
	DECLARE_HIT_PROXY(HTextureView,HHitProxy)
	UTexture* Texture;
	INT ViewX, ViewY;
	HTextureView( UTexture* InTexture, INT InX, INT InY ) : Texture(InTexture), ViewX(InX), ViewY(InY) {}
	void Click( const FHitCause& Cause );
};

// Hit a brush vertex.
struct HBrushVertex : public HHitProxy
{
	DECLARE_HIT_PROXY(HBrushVertex,HHitProxy)
	AActor* Brush;
	FVector Location;
	HBrushVertex( AActor* InBrush, FVector InLocation ) : Brush(InBrush), Location(InLocation) {}
};

// Hit a global pivot.
struct HGlobalPivot : public HHitProxy
{
	DECLARE_HIT_PROXY(HGlobalPivot,HHitProxy)
	FVector Location;
	HGlobalPivot( FVector InLocation ) : Location(InLocation) {}
};

// Hit a browser texture.
struct HBrowserTexture : public HHitProxy
{
	DECLARE_HIT_PROXY(HBrowserTexture,HHitProxy)
	UObject* Texture;
	HBrowserTexture( UObject* InTexture ) : Texture(InTexture) {}
};

// Hit the backdrop.
struct HBackdrop : public HHitProxy
{
	DECLARE_HIT_PROXY(HBackdrop,HHitProxy)
	FVector Location;
	HBackdrop( FVector InLocation ) : Location(InLocation) {}
	void Click( const FHitCause& Cause );
};

void ImportPropertiesFor(UClass* Class, class FScriptCompiler* Compiler);

/*-----------------------------------------------------------------------------
	FScan.
-----------------------------------------------------------------------------*/

typedef void (*POLY_CALLBACK)( UModel* Model, INT iSurf );

/*-----------------------------------------------------------------------------
	Misc definition.
-----------------------------------------------------------------------------*/

class FCheckObjRefArc : public FArchive
{
private:
	TArray<UObject*>* OutputAr;
	UObject* Seek;
	UObject* Context;
	bool bResult,bMayCleanUp;

	inline bool CheckResFor( const UObject* Checking )
	{
		if( Checking==Seek || Checking->GetClass()==UTextBuffer::StaticClass()
			 || appStricmp(Checking->GetClass()->GetName(),TEXT("TransBuffer"))==0)
			return false;
		if( !Checking->GetOuter() )
			return true;
		for( UObject* C=Checking->GetOuter(); C; C=C->GetOuter() )
			if( C==Seek )
				return false;
		return true;
	}
public:

	FArchive& operator<<( class UObject*& Res )
	{
		if( Seek==Res )
		{
			bResult = true;
			if( OutputAr )
				OutputAr->AddUniqueItem(Context);
			if( bMayCleanUp )
				Res = NULL;
		}
		return *this;
	}
	FCheckObjRefArc( TArray<UObject*>* Out, UObject* FindRef, bool bCleanUp )
	: OutputAr( Out )
	, Seek( FindRef )
	, Context( NULL )
	, bResult( false )
	, bMayCleanUp( bCleanUp )
	{}
	bool HasReferences()
	{
		for( FObjectIterator It; It; ++It )
		{
			UObject* O=*It;
			if ((bMayCleanUp || !O->IsA(UEngine::StaticClass())) && CheckResFor(O))
			{
				Context = O;
				O->Serialize( *this );
			}
		}
		return bResult;
	}
};

// Selected actors iterator.
class FSelectedActorIterator
{
public:
	FSelectedActorIterator(ULevel* InLevel)
		: ActorList(InLevel->Actors), Index(INDEX_NONE)
	{
		++(*this);
	}
	inline void operator++()
	{
		while (++Index < ActorList.Num())
		{
			if (ActorList(Index) && ActorList(Index)->bSelected)
				break;
		}
	}
	inline AActor* operator*()
	{
		return ActorList(Index);
	}
	inline AActor* operator->()
	{
		return ActorList(Index);
	}
	inline operator UBOOL()
	{
		return Index < ActorList.Num();
	}
private:
	TTransArray<AActor*>& ActorList;
	INT Index;
};

/*-----------------------------------------------------------------------------
	Parameter parsing functions.
-----------------------------------------------------------------------------*/

EDITOR_API UBOOL GetFVECTOR( const TCHAR* Stream, const TCHAR* Match, FVector& Value );
EDITOR_API UBOOL GetFVECTOR( const TCHAR* Stream, FVector& Value );
EDITOR_API UBOOL GetFROTATOR( const TCHAR* Stream, const TCHAR* Match, FRotator& Rotation, int ScaleFactor );
EDITOR_API UBOOL GetFROTATOR( const TCHAR* Stream, FRotator& Rotation, int ScaleFactor );
EDITOR_API UBOOL GetBEGIN( const TCHAR** Stream, const TCHAR* Match );
EDITOR_API UBOOL GetEND( const TCHAR** Stream, const TCHAR* Match );
EDITOR_API TCHAR* SetFVECTOR( TCHAR* Dest, const FVector* Value );
EDITOR_API UBOOL GetFSCALE( const TCHAR* Stream, FScale& Scale );

/*-----------------------------------------------------------------------------
	2D shape tesselation.
-----------------------------------------------------------------------------*/

class EDITOR_API F2DTessellator
{
public:
	struct FInputShape
	{
		TArray<INT> Links;
	};
	struct FOutputTris
	{
		INT Verts[3];

		FOutputTris()
		{}
		FOutputTris(const INT A, const INT B, const INT C)
		{
			Verts[0] = A;
			Verts[1] = B;
			Verts[2] = C;
		}
	};
	TArray<FVector2D> Vertices; // CCW order of tris that make up a polygon.
	TArray<FInputShape> Shapes; // Optional input list of multiple shapes.
	TArray<FOutputTris> Tris; // Output triangle indices.

	// Project a BSP surf to 2D vertices.
	UBOOL Populate(const FPoly* Poly);

	// Breaks up the shape into triangles.
	void Tesselate();

	void ClearTesselation()
	{
		guardSlow(F2DTessellator::ClearTesselation);
		Tris.Empty();
		unguardSlow;
	}
};

/*-----------------------------------------------------------------------------
	Multi-threading task force.
-----------------------------------------------------------------------------*/

class FMultiThreadTasker
{
public:
	FThreadLock TaskMutex;
	struct FLightThreadBase* ThreadList;
	volatile INT TaskProgress, TaskFinal;

	FMultiThreadTasker()
		: ThreadList(NULL), TaskProgress(0), TaskFinal(0)
	{}

	inline void InitMTTask(const INT iFinal)
	{
		guardSlow(FMultiThreadTasker::InitMTTask);
		ThreadList = nullptr;
		TaskProgress = 0;
		TaskFinal = iFinal;
		unguardSlow;
	}
	inline INT GetNextTask()
	{
		guardSlow(FMultiThreadTasker::GetNextTask);
		INT Result;
		TaskMutex.lock();
		Result = TaskProgress;
		if (Result >= TaskFinal)
			Result = INDEX_NONE;
		else ++TaskProgress;
		TaskMutex.unlock();
		return Result;
		unguardSlow;
	}
	void ExitThreads();
	INT CountThreads();
	INT WaitProgress();
};

struct FLightThreadBase : public FThreadingBase
{
	FMultiThreadTasker* Task;
	FLightThreadBase* Next;
	volatile UBOOL bFinished;

	FLightThreadBase(FMultiThreadTasker* M, const TCHAR* TaskName)
		: FThreadingBase(TaskName), Task(M), bFinished(FALSE)
	{
		Next = M->ThreadList;
		M->ThreadList = this;
	}
	~FLightThreadBase()
	{
		if (Task->ThreadList == this)
			Task->ThreadList = Next;
		else
		{
			for (FLightThreadBase* h = Task->ThreadList; h; h = h->Next)
				if (h->Next == this)
				{
					h->Next = Next;
					break;
				}
		}
	}
	void Main()
	{
		bFinished = TRUE;
	}
};

inline void FMultiThreadTasker::ExitThreads()
{
	guardSlow(FMultiThreadTasker::ExitThreads);
	FLightThreadBase* n;
	for (FLightThreadBase* h = ThreadList; h; h = n)
	{
		n = h->Next;
		delete h;
	}
	ThreadList = nullptr;
	unguardSlow;
}
inline INT FMultiThreadTasker::CountThreads()
{
	guardSlow(FMultiThreadTasker::CountThreads);
	INT Result = 0;
	FLightThreadBase* n;
	for (FLightThreadBase* h = ThreadList; h; h = n)
	{
		n = h->Next;
		if (h->bFinished)
			delete h;
		else ++Result;
	}
	return Result;
	unguardSlow;
}
inline INT FMultiThreadTasker::WaitProgress()
{
	guardSlow(FMultiThreadTasker::WaitProgress);
	const INT Cnt = TaskProgress;
	for (INT i = 0; (i < 20) && (Cnt == TaskProgress); ++i)
	{
		appSleep(0.1f);
		if (CountThreads() == 0)
			return INDEX_NONE;
	}
	return TaskProgress;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif
