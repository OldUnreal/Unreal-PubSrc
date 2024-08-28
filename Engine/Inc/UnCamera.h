/*=============================================================================
	UnViewport.h: Unreal viewport object.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UCanvas.
	227j: UCanvas definition moved to EngineClasses.h
-----------------------------------------------------------------------------*/

// Constants.
enum {MAX_COLOR_BYTES=9}; // Maximum expected frame buffer depth = 8 bytes = 64-bit.

//
// Flags for drawing text, not yet used.
//
enum ETextOut
{
	TEXTOUT_Center			= 0x01,	// Center horizontally.
	TEXTOUT_CenterVertical	= 0x02,	// Center vertically.
	TEXTOUT_Right			= 0x04, // Right justify.
	TEXTOUT_Bottom			= 0x08, // Bottom justify.
	TEXTOUT_Wrap			= 0x10,	// Word wrap.
};

//
// Blitting types.
//
enum EViewportBlitFlags
{
	// Bitflags.
	BLIT_Fullscreen     = 0x0001, // Fullscreen.
	BLIT_Temporary      = 0x0002, // Temporary viewport.
	BLIT_DibSection     = 0x0004, // Create a DibSection for windowed rendering.
	BLIT_DirectDraw     = 0x0008, // Create Direct3D along with DirectDraw.
	BLIT_Direct3D       = 0x0010, // Create Direct3D along with DirectDraw.
	BLIT_NoWindowChange = 0x0020, // Don't resize existing window frame.
	BLIT_NoWindowFrame  = 0x0040, // Turn off the window frame.
	BLIT_OpenGL			= 0x0080, // OpenGL rendering.
	BLIT_HardwarePaint  = 0x0100, // Window should be repainted in hardware when drawn.
	BLIT_Metal          = 0x0200, // Apple Metal rendering.

	// Special.
	BLIT_ParameterFlags	= BLIT_NoWindowChange, // Only parameters to ResizeViewport, not permanent flags.
};

enum E3DOffsetFlags
{
	E3DC_Disable		= 0x00,
	E3DC_Enable			= 0x01,
	E3DC_FlatZ			= 0x04,
	E3DC_WorldOffset	= 0x08,
};

struct FCanvasScaler
{
	FLOAT Scale;
	INT X, Y;

	FCanvasScaler(FLOAT InScale, INT InX, INT InY)
		: Scale(InScale), X(InX), Y(InY)
	{}
};

/*-----------------------------------------------------------------------------
	FHitCause.
-----------------------------------------------------------------------------*/

//
// Information about a hit-test cause.
//
struct ENGINE_API FHitCause
{
	class FHitObserver* Observer;
	UViewport* Viewport;
	DWORD Buttons;
	FLOAT MouseX;
	FLOAT MouseY;
	FHitCause( FHitObserver* InObserver, UViewport* InViewport, DWORD InButtons, FLOAT InMouseX, FLOAT InMouseY )
	: Observer( InObserver ), Viewport( InViewport ), Buttons( InButtons ), MouseX( InMouseX ), MouseY( InMouseY )
	{}
};

/*-----------------------------------------------------------------------------
	FHitObserver.
-----------------------------------------------------------------------------*/

//
// Hit observer class for receiving hit-test notification.
//
class ENGINE_API FHitObserver
{
public:
	virtual void Click( const struct FHitCause& Cause, const struct HHitProxy& Hit ) {}
};

/*-----------------------------------------------------------------------------
	Viewport callback (for custom viewport handle).
-----------------------------------------------------------------------------*/

struct ENGINE_API FViewportCallback
{
protected:
	UViewport* Viewport;

public:
	FViewportCallback(UViewport* V);
	virtual ~FViewportCallback() noexcept(false)
	{}

	inline UViewport* GetViewport() const
	{
		return Viewport;
	}
	virtual void ViewportDestroyed();
	virtual void Draw(UBOOL Blit);
	virtual UBOOL InputEvent(EInputKey iKey, EInputAction State, FLOAT Delta);
	virtual UBOOL Key(EInputKey Key, INT KeyValue);
	virtual void Click(DWORD Buttons, FLOAT X, FLOAT Y);
	virtual void MouseDelta(DWORD Buttons, FLOAT DX, FLOAT DY);
	virtual void MousePosition(DWORD Buttons, FLOAT X, FLOAT Y);
	virtual void edSetClickLocation(FVector& Point);
	virtual BYTE GetCursor();
	virtual const TCHAR* GetWindowName();
};

/*-----------------------------------------------------------------------------
	UViewport.
-----------------------------------------------------------------------------*/

//
// Editor mode settings.
//
// These are also referenced by help files and by the editor client, so
// they shouldn't be changed.
//
enum EEditorMode
{
	EM_None 			= 0,	// Gameplay, editor disabled.
	EM_ViewportMove		= 1,	// Move viewport normally.
	EM_ViewportZoom		= 2,	// Move viewport with acceleration.
	EM_BrushRotate		= 5,	// Rotate brush.
	EM_BrushSheer		= 6,	// Sheer brush.
	EM_BrushScale		= 7,	// Scale brush.
	EM_BrushStretch		= 8,	// Stretch brush.
	EM_TexturePan		= 11,	// Pan textures.
	EM_TextureRotate	= 13,	// Rotate textures.
	EM_TextureScale		= 14,	// Scale textures.
	EM_BrushSnap		= 18,	// Brush snap-scale.
	EM_TexView			= 19,	// Viewing textures.
	EM_TexBrowser		= 20,	// Browsing textures.
	EM_MeshView			= 21,	// Viewing mesh.
	EM_MeshBrowser		= 22,	// Browsing mesh.
	EM_BrushClip		= 23,	// brush Clipping.
	EM_VertexEdit		= 24,	// Multiple Vertex Editing.
	EM_FaceDrag			= 25,	// Face Dragging.
	EM_TerrainEdit		= 26,	// Terrain heightmap.
	EM_CustomCursor		= 30,	// To override cursor to viewport StandardCursors
};

// Information for rendering the viewport (detail level settings).
enum ERenderType
{
	REN_None			= 0,	// Hide completely.
	REN_Wire			= 1,	// Wireframe of EdPolys.
	REN_Zones			= 2,	// Show zones and zone portals.
	REN_Polys			= 3,	// Flat-shaded Bsp.
	REN_PolyCuts		= 4,	// Flat-shaded Bsp with normals displayed.
	REN_DynLight		= 5,	// Illuminated texture mapping.
	REN_PlainTex		= 6,	// Plain texture mapping.
	REN_OrthXY			= 13,	// Orthogonal overhead (XY) view.
	REN_OrthXZ			= 14,	// Orthogonal XZ view.
	REN_OrthYZ			= 15,	// Orthogonal YZ view.
	REN_TexView			= 16,	// Viewing a texture (no actor).
	REN_TexBrowser		= 17,	// Viewing a texture browser (no actor).
	REN_MeshView		= 18,	// Viewing a mesh.
	REN_LightingOnly	= 19,	// Render only lights on plain white texture.
	REN_Normals			= 20,	// Render normals only.
	REN_MeshEditor		= 21,	// Mesh editor.
	REN_MAX
};

// Viewport capabilities.
enum EViewportCaps
{
	CC_RGB565			= 1,	// RGB 565 format (otherwise 555).
	CC_Mask				= 0xff, // Caps mask which affects cached rendering info.
};

// ShowFlags for viewport.
enum EViewportShowFlags
{
	SHOW_Frame     		= 0x00000001, 	// Show world bounding cube.
	SHOW_ActorRadii		= 0x00000002, 	// Show actor collision radii.
	SHOW_Backdrop  		= 0x00000004, 	// Show background scene.
	SHOW_Actors    		= 0x00000008,	// Show actors.
	SHOW_Coords    		= 0x00000010,	// Show brush/actor coords.
	SHOW_ActorIcons		= 0x00000020,	// Show actors as icons.
	SHOW_Brush			= 0x00000040,	// Show the active brush.
	SHOW_StandardView	= 0x00000080,	// Viewport is a standard view.
	SHOW_Menu			= 0x00000100,	// Show menu on viewport.
	SHOW_ChildWindow	= 0x00000200,	// Show as true child window.
	SHOW_MovingBrushes	= 0x00000400,	// Show moving brushes.
	SHOW_PlayerCtrl		= 0x00000800,	// Player controls are on.
	SHOW_Paths          = 0x00001000,   // Show paths.
	SHOW_NoButtons		= 0x00002000,	// No menu/view buttons.
	SHOW_RealTime		= 0x00004000,	// Update window in realtime.
	SHOW_RealTimeBackdrop = 0x00008000,	// Update backdrop in realtime.
	SHOW_DistanceFog	= 0x00010000,	// Show DistanceFog.
	SHOW_PathsPreview	= 0x00020000,	// Show PathsPreview.
	SHOW_LightColorIcon	= 0x00040000,	// Show light actor icons to be rendered in editor in the color of their light source.
	SHOW_Projectors		= 0x00100000,	// Should draw projectors.
	SHOW_StaticMeshes	= 0x00200000,	// Should draw static mesh actors.
	SHOW_InGameMode		= 0x00400000,	// Hide bHidden actors (and show bHiddenEd if bHidden is false).
	SHOW_Emitters		= 0x01000000,	// Show Emitters.
	SHOW_Events			= 0x02000000,	// Draw event lines.
	SHOW_Terrains		= 0x04000000,	// Draw terrains.

	SHOW_EditorOrtho	= (SHOW_Menu | SHOW_Frame | SHOW_Actors | SHOW_Brush | SHOW_StandardView | SHOW_MovingBrushes | SHOW_RealTimeBackdrop | SHOW_DistanceFog | SHOW_ChildWindow
						| SHOW_LightColorIcon | SHOW_Projectors | SHOW_StaticMeshes | SHOW_Events | SHOW_Terrains), // Default editor draw mode for ortho viewports.
	SHOW_EditorMode		= ((SHOW_EditorOrtho | SHOW_Emitters) & (~SHOW_Events)), // Default editor draw mode for 3D viewports.

	SHOW_GameMode		= (SHOW_Backdrop | SHOW_Actors | SHOW_PlayerCtrl | SHOW_RealTime | SHOW_DistanceFog | SHOW_Terrains
						| SHOW_RealTimeBackdrop | SHOW_Projectors | SHOW_StaticMeshes | SHOW_Emitters | SHOW_InGameMode), // Draw mode while in-game.
};

// Mouse buttons and commands.
enum EMouseButtons
{
	MOUSE_None			= 0x0000,
	MOUSE_Left			= 0x0001,		// Left mouse button.
	MOUSE_Right			= 0x0002,		// Right mouse button.
	MOUSE_Middle 		= 0x0004,		// Middle mouse button.
	MOUSE_FirstHit		= 0x0008,		// Sent when a mouse button is initially hit.
	MOUSE_LastRelease	= 0x0010,		// Sent when last mouse button is released.
	MOUSE_SetMode		= 0x0020,		// Called when a new viewport mode is first set.
	MOUSE_ExitMode		= 0x0040,		// Called when the existing mode is changed.
	MOUSE_Ctrl			= 0x0080,		// Ctrl is pressed.
	MOUSE_Shift			= 0x0100,		// Shift is pressed.
	MOUSE_Alt			= 0x0200,		// Alt is pressed.
	MOUSE_LeftDouble	= 0x0400,		// Left double click.
	MOUSE_WheelUp		= 0x0800,		// Mouse wheel up.
	MOUSE_WheelDown		= 0x1000,		// Mouse wheel down.
	MOUSE_Button4		= 0x2000,		// Shoulder button 1.
	MOUSE_Button5		= 0x4000		// Shoulder button 2.
};

//
// A viewport object, which associates an actor (which defines
// most view parameters) with a Windows window.
//
class ENGINE_API UViewport : public UPlayer
{
	DECLARE_ABSTRACT_CLASS(UViewport,UPlayer,CLASS_Transient,Engine)
	DECLARE_WITHIN(UClient)

	// Referenced objects.
	class UCanvas*		 Canvas;	// Viewport's painting canvas.
	class UInput*		 Input;		// Input system.
	class URenderDevice* RenDev;	// Render device.

	// Normal variables.
	UObject*		MiscRes;		// Used in in modes like EM_TEXVIEW.
	FName			Group;			// Group for editing.
	FTime			LastUpdateTime;	// Time of last update.
	INT				SizeX, SizeY;   // Buffer X & Y resolutions. -- stijn: This is the resolution of the framebuffer. It might be different from the window resolution on hi-dpi displays
	INT				ColorBytes;		// 1=256-color, 4=32-bit color.
	INT				FrameCount;		// Frame count, incremented when locked.
	DWORD			Caps;			// Capabilities (CC_).
	UBOOL			Current;		// If this is the current input viewport.
	UBOOL			Dragging;		// Dragging mouse.
	DWORD			RenderFlags;	// Render locking flags (only when locked).
	DWORD			ExtraPolyFlags;	// Additional poly flags associated with camera.
	FLOAT			LastExecTime;	// Time of last update.

	// Level traveling.
	ETravelType		TravelType;
	FStringNoInit	TravelURL;
	UBOOL			bTravelItems;

	// Frame buffer info; only valid when locked.
	FTime			CurrentTime;	// Time when initially locked.
	BYTE*			ScreenPointer;	// Pointer to screen frame buffer, or NULL if none.
	INT				Stride;			// Stride in pixels.

	// Hit testing.
	UBOOL			HitTesting;		// Whether hit-testing.
	INT				HitX, HitY;		// Hit rectangle top left.
	INT				HitXL, HitYL;	// Hit size.
	TArray<INT>		HitSizes;		// Currently pushed hit sizes.

	//UED2 Skeletal Mesh support
	UBOOL			ShowSkel,ShowNames; //To enable/disable Skeleton Mesh information in MeshBrowser.

	// Saved-actor parameters.
	FLOAT SavedOrthoZoom, SavedFovAngle;
	INT SavedShowFlags, SavedRendMap, SavedMisc1, SavedMisc2;

	// stijn: fix for slow dragging in non-realtime viewports
	UBOOL			RepaintPending;

protected:
	// Marco: Input device output callback (API so that modders can override this)
	FViewportCallback* OutputDev;

public:
	// Constructor.
	UViewport();

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );

	UBOOL AllowScriptDestroy()
	{
		return FALSE;
	}

	// FArchive interface.
	void Serialize( const TCHAR* Data, EName MsgType );

	// UPlayer interface.
	void ReadInput( FLOAT DeltaSeconds );

	// UViewport interface.
	virtual UBOOL SetDrag( UBOOL NewDrag );
	virtual UBOOL IsFullscreen() _VF_BASE_RET(0);
	virtual UBOOL Lock( FPlane FlashScale, FPlane FlashFog, FPlane ScreenClear, DWORD RenderLockFlags, BYTE* HitData=NULL, INT* HitSize=NULL );
	virtual UBOOL ResizeViewport( DWORD BlitType, INT X=INDEX_NONE, INT Y=INDEX_NONE, INT ColorBytes=INDEX_NONE ) _VF_BASE_RET(0);
	virtual void Unlock( UBOOL Blit=0 );
	virtual void* GetWindow() _VF_BASE_RET(NULL);

	virtual void SetModeCursor() _VF_BASE;
	virtual void UpdateWindowFrame() _VF_BASE;
	virtual void OpenWindow(void* ParentWindow, UBOOL Temporary, INT NewX, INT NewY, INT OpenX, INT OpenY, const TCHAR* ForcedRenDevClass = NULL) _VF_BASE;
	virtual void CloseWindow() _VF_BASE;
	virtual void UpdateInput( UBOOL Reset ) _VF_BASE;
	virtual void SetMouseCapture( UBOOL Capture, UBOOL Clip, UBOOL FocusOnly=0 ) _VF_BASE;
	virtual void Repaint( UBOOL Blit ) _VF_BASE;

	virtual void* GetServer();
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	virtual void TryRenderDevice( const TCHAR* ClassName, INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen ) {}

	virtual void MoveWindow(int Left, int Top, int Width, int Height, UBOOL bRepaint) {}

	void ChangeOutputAPI(FViewportCallback* NewOutput); // Swap output device while killing old one.
	inline FViewportCallback* GetOutputAPI() const
	{
		return OutputDev;
	}

	// Functions.
	void ExecuteHits( const FHitCause& Cause, BYTE* HitData, INT HitSize );
	void PushHit( const struct HHitProxy& Hit, INT Size );
	void PopHit( UBOOL bForce );
	UBOOL IsWire();
	void ExecMacro( const TCHAR* Filename, FOutputDevice& Ar=*GLog );

	// UViewport inlines.
	BYTE* _Screen( INT X, INT Y )
	{
		return ScreenPointer + (X+Y*Stride)*ColorBytes;
	}
	UBOOL IsOrtho()
	{
		return Actor->RendMap==REN_OrthXY||Actor->RendMap==REN_OrthXZ||Actor->RendMap==REN_OrthYZ;
	}
	UBOOL IsRealtime()
	{
		return Actor && (Actor->ShowFlags&(SHOW_RealTime | SHOW_PlayerCtrl));
	}
	BYTE ByteID()
	{
		BYTE ColorBytesID[5]={0,0,1,2,3};
		return ColorBytesID[ColorBytes] | (Caps & 7)*4;
	}

#ifdef UTPG_WINXP_FIREWALL
	virtual UBOOL FireWallHack(INT Cmd) { return 1; }	// JMW - Hack for fixing the UT2004 Firewall support
#endif
};

// Viewport hit-testing macros.
#define PUSH_HIT(frame,expr) if( (frame->Viewport)->HitTesting ) (frame->Viewport)->PushHit(expr,sizeof(expr));
#define POP_HIT(frame)       if( (frame->Viewport)->HitTesting ) (frame->Viewport)->PopHit(0);
#define POP_HIT_FORCE(frame) if( (frame->Viewport)->HitTesting ) (frame->Viewport)->PopHit(1);

/*-----------------------------------------------------------------------------
	Base hit proxy.
-----------------------------------------------------------------------------*/

// Hit proxy declaration macro.
#define DECLARE_HIT_PROXY(cls,parent) \
	const TCHAR* GetName() const \
		{ return TEXT(#cls); } \
	UBOOL IsA( const TCHAR* Str ) const \
		{ return appStricmp(TEXT(#cls),Str)==0 || parent::IsA(Str); }

// Base class for detecting user-interface hits.
struct ENGINE_API HHitProxy
{
	union
	{
		mutable INT Size;
		HHitProxy* Parent;
	};
	const TCHAR* GetName() const
	{
		return TEXT("HHitProxy");
	}
	virtual UBOOL IsA( const TCHAR* Str ) const
	{
		return appStricmp(TEXT("HHitProxy"),Str)==0;
	}
	virtual void Click( const FHitCause& Cause )
	{
		Cause.Observer->Click( Cause, *this );
	}
};

/*-----------------------------------------------------------------------------
	UClient.
-----------------------------------------------------------------------------*/

//
// Client, responsible for tracking viewports.
//
class ENGINE_API UClient : public UObject
{
	DECLARE_ABSTRACT_CLASS(UClient,UObject,CLASS_Config,Engine)

	enum ESkyBoxFogMode
	{
		SKYFOG_HD,
		SKYFOG_Low,
		SKYFOG_None,
	};

	// Variables.
	UEngine*			Engine;
	TArray<UViewport*>	Viewports;
	INT					DrawCycles;

	// Configurable.
	BITFIELD	CaptureMouse;
	BITFIELD	CurvedSurfaces;
	BITFIELD	ScreenFlashes;
	BITFIELD	NoLighting;
	BITFIELD	Decals;
	BITFIELD	NoDynamicLights;
	BITFIELD    NoFractalAnim;
	INT			WindowedViewportX;
	INT			WindowedViewportY;
	INT			WindowedColorBits;
	INT			FullscreenViewportX;
	INT			FullscreenViewportY;
	INT			FullscreenColorBits;
	FLOAT		Brightness;
	FLOAT		MipFactor;
	INT			TextureLODSet[LODSET_MAX];
	FLOAT		MinDesiredFrameRate;
	FLOAT		AnimTexFrameRate;
	INT			ParticleDensity;
	INT			LightMapLOD;
	BYTE		SkyboxFogMode;
	FLOAT		FrameRateLimit;

	BITFIELD	FlatShading;
	BITFIELD    ContinuousKeyEvents;
	BITFIELD	UseHDTextures, UseNoSmoothWorld;

	// Shambler: Custom stat-net colour
	FPlane		StatNetColour;

	static UBOOL GEnableHDTex;

	// Constructors.
	UClient();
	void StaticConstructor();

	// UObject interface.
	void Serialize( FArchive& Ar );
	void Destroy();
	void PostEditChange();
	UBOOL AllowScriptDestroy()
	{
		return FALSE;
	}

	// UClient interface.

	virtual void Flush( UBOOL AllowPrecache );
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
	virtual class UViewport* NewViewport( const FName Name ) _VF_BASE_RET(NULL);
	virtual void Init( UEngine* InEngine );

	virtual void ShowViewportWindows( DWORD ShowFlags, int DoShow ) _VF_BASE;
	virtual void EnableViewportWindows( DWORD ShowFlags, int DoEnable ) _VF_BASE;
	virtual void Tick() _VF_BASE;
	virtual void MakeCurrent( UViewport* NewViewport ) _VF_BASE;
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
