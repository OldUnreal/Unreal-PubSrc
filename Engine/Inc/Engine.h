/*=============================================================================
	Engine.h: Unreal engine public header file.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#ifndef _INC_ENGINE
#define _INC_ENGINE

/*----------------------------------------------------------------------------
	API.
----------------------------------------------------------------------------*/

#ifndef ENGINE_API
	#define ENGINE_API DLL_IMPORT
#endif

/*-----------------------------------------------------------------------------
	Dependencies.
-----------------------------------------------------------------------------*/

#include "Core.h"

/*-----------------------------------------------------------------------------
	Global variables.
-----------------------------------------------------------------------------*/

ENGINE_API extern class FMemStack			GEngineMem;
ENGINE_API extern class FMemCache			GCache;

ENGINE_API extern FVector _ClientCameraLocation;
ENGINE_API extern FRotator _ClientCameraRotation;

#if STATS
struct FFPSStats : public FStatGroup
{
	struct FStatFPS : public FStatValueTimer
	{
		FStatFPS(FStatGroup* G, const TCHAR* T);
		FString GetValue()
		{
			DWORD Avg = 0;
			for (BYTE i = 0; i < HISTORY_SIZE; ++i)
				Avg += History[i];
			return FString::Printf(TEXT("%.1f FPS (%.1f FPS avg)"), 1.0 / ((DOUBLE)Max<DWORD>(LastFrame,1) * GSecondsPerCycle), 1.0 / ((DOUBLE)Max<DWORD>(Avg,1) / HISTORY_SIZE * GSecondsPerCycle));
		}
	};
	FStatFPS FPS;
	FStatStaticCount FrameCount;

	FFPSStats();
};
struct FStatValueLightMem : public FStatValueBase
{
	QWORD Value;

	FStatValueLightMem(FStatGroup* Parent, const TCHAR* UI)
		: FStatValueBase(Parent, UI), Value(0)
	{
	}
	FString GetValue()
	{
		return FString::Printf(TEXT("%llu kb"), (Value >> 10));
	}
};
struct FTextureStats : public FStatGroup
{
	FStatValueCounter FractCounter;
	FStatValueTimer FractTime;
	FStatValueTimer HardwareTime;
	FStatStaticCount TextureInfo;

	FTextureStats();
};
struct FDecalStats : public FStatGroup
{
	FStatValueCounter VisDecals;
	FStatValueTimer DecalDrawTime,DecalAttachTime;
	FStatValueCounter VisProjectors;
	FStatValueTimer ProjectorDrawTime,ProjAttachTime;

	FDecalStats();
};
struct FNetStats : public FStatGroup
{
	struct FStatNetwork : public FStatValueBase
	{
		BYTE Value;
		FNetStats& Net;
		FStatNetwork(FNetStats* Parent, const TCHAR* UI, BYTE Index);
		FString GetValue();
	};
	FStatNetwork Ping, Channels, Unordered, Loss, Packets, Bunches, Bytes, NetSpeed;

	class UNetConnection* Conn;
	FNetStats();
};
struct FEmitterStats : public FStatGroup
{
	FStatValueCounter UpdEmitters;
	FStatValueTimer UpdEmitTime;

	FEmitterStats();
};
struct FRenderStats : public FStatGroup
{
	FStatValueCounter MeshCount;
	FStatValueCounter MeshPolyCount;
	FStatValueTimer MeshDrawTime, RenderIteratorTime, BSPOccludeTime;
	FStatValueCounter BSPPolyCount, BSPPortalCount, VisZoneCount, LightDetailLevel;
	FStatValueLightMem LightMemBSP, LightMemActor;

	FRenderStats();
};
struct FCollisionStats : public FStatGroup
{
	FStatValueTimer CylinderTime,BSPTime,MeshTime;

	FCollisionStats();
};
struct FHWRenderStats : public FStatGroup
{
	struct FHWStats : public FStatValueBase
	{
		class URenderDevice* RenDev;
		FHWStats(FHWRenderStats* Parent);
		FString GetValue();
	};
	FHWStats HW;

	FHWRenderStats();
};

ENGINE_API extern FFPSStats GStatFPS;
ENGINE_API extern FTextureStats GStatTexture;
ENGINE_API extern FDecalStats GStatDecals;
ENGINE_API extern FNetStats GStatNet;
ENGINE_API extern FEmitterStats GStatEmitter;
ENGINE_API extern FRenderStats GStatRender;
ENGINE_API extern FCollisionStats GStatCollision;
ENGINE_API extern FHWRenderStats GHWRenderStats;

#define STAT_TRACK_FRACTAL ++GStatTexture.FractCounter.Calls; FStatTimerScope TScope(GStatTexture.FractTime);
#define STAT_TRACK_TEXTURELOCK FStatTimerScope TScope(GStatTexture.HardwareTime);

#endif

/*-----------------------------------------------------------------------------
	Engine compiler specific includes.
-----------------------------------------------------------------------------*/

#if __GNUG__
	#include "UnEngineGnuG.h"
#endif

/*-----------------------------------------------------------------------------
	Engine public includes.
-----------------------------------------------------------------------------*/

#include "UnObj.h"				// Standard object definitions.
#include "UnRbPhysics.h"		// RigidBody physics definitions.
#include "UnPrim.h"				// Primitive class.
#include "U3DCollision.h"		// U3D Collision.
#include "UnModel.h"			// Model class.
#include "UnTex.h"				// Texture and palette.
#include "UnScrTex.h"			// Scripted textures.
#include "UnAnim.h"				// Animations.
#include "UnURL.h"				// Uniform resource locators.
#include "EngineClasses.h"		// All actor classes.
#include "UnColor.h"
#include "UnTexComp.h"
#include "UnTexFmt.h"			// Texture formats, tools, etc...
#include "UnReach.h"			// Reach specs.
#include "UnLevel.h"			// Level object.
#include "UnIn.h"				// Input system.
#include "UnPlayer.h"			// Player class.
#include "UnEngine.h"			// Unreal engine.
#include "UnCamera.h"			// Viewport subsystem.
#include "UnMesh.h"				// Mesh objects.
#include "UnStaticMesh.h"		// StaticMesh object.
#include "UnSkeletalMesh.h"		// Skeletal model objects.
#include "UnActor.h"			// Actor inlines.
#include "UnAudio.h"			// Audio code.
#include "UnDynBsp.h"			// Dynamic Bsp objects.
#include "UnRender.h"
#include "UnCollisionPlane.h"
#include "UnProjector.h"
#include "UnStaticLight.h"
#include "UI.h"                 // Unreal Integrity

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif
