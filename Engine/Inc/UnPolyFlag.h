/*=============================================================================
	UnPolyFlag.h: Flags describing effects and properties of a bsp surface or 
    mesh polygon.
	Copyright 2024 OldUnreal. All Rights Reserved.

	Revision history:
		* Created by Stijn Volckaert
=============================================================================*/

#pragma once

// Flags describing effects and properties of a Bsp polygon.
enum EPolyFlags
{
	// Regular in-game flags.
	PF_None             = 0x00000000,   // No Flags.
	PF_Invisible		= 0x00000001,	// Poly is invisible.
	PF_Masked			= 0x00000002,	// Poly should be drawn masked.
	PF_Translucent	 	= 0x00000004,	// Poly is transparent.
	PF_NotSolid			= 0x00000008,	// Poly is not solid, doesn't block.
	PF_Environment   	= 0x00000010,	// Poly should be drawn environment mapped.
	PF_Semisolid	  	= 0x00000020,	// Poly is semi-solid = collision solid, Csg nonsolid.
	PF_Modulated 		= 0x00000040,	// Modulation transparency.
	PF_FakeBackdrop		= 0x00000080,	// Poly looks exactly like backdrop.
	PF_TwoSided			= 0x00000100,	// Poly is visible from both sides.
	PF_AutoUPan		 	= 0x00000200,	// Automatically pans in U direction.
	PF_AutoVPan 		= 0x00000400,	// Automatically pans in V direction.
	PF_NoSmooth			= 0x00000800,	// Don't smooth textures.
	PF_BigWavy 			= 0x00001000,	// Poly has a big wavy pattern in it.
	PF_SpecialPoly		= PF_BigWavy,	// Game-specific poly-level render control (reuse BigWavy flag)
	PF_SmallWavy		= 0x00002000,	// Small wavy pattern (for water/enviro reflection).
	PF_Flat				= 0x00004000,	// Flat Mesh
	PF_WaterWavy        = PF_Flat,      // WaterWavy Surface flag
	PF_ForceViewZone	= PF_Flat,	    // Force current iViewZone in OccludeBSP (reuse Flat flag)
	PF_LowShadowDetail	= 0x00008000,	// Low detail shadows.
	PF_NoMerge			= 0x00010000,	// Don't merge poly's nodes before lighting when rendering.
	PF_AlphaBlend 		= 0x00020000,	// This poly should be alpha blended
	PF_DirtyShadows		= 0x00040000,	// Dirty shadows.
	PF_BrightCorners	= 0x00080000,	// Brighten convex corners.
	PF_SpecialLit		= 0x00100000,	// Only speciallit lights apply to this poly.
	PF_Gouraud			= 0x00200000,	// Gouraud shaded.
	PF_NoBoundRejection = PF_Gouraud,	// Disable bound rejection in OccludeBSP (reuse Gourard flag)
	PF_Unlit			= 0x00400000,	// Unlit.
	PF_HighShadowDetail	= 0x00800000,	// High detail shadows.
	PF_Portal			= 0x04000000,	// Portal between iZones.
	PF_Mirrored			= 0x08000000,	// Reflective surface.
	PF_OccluderPoly		= 0x20000000,	// Hidden occluder.

	// Editor flags.
	PF_Memorized     	= 0x01000000,	// Editor: Poly is remembered.
	PF_Selected      	= 0x02000000,	// Editor: Poly is selected.
	PF_Highlighted      = 0x10000000,	// Editor: Poly is highlighted.
	PF_FlatShaded		= 0x40000000,	// FPoly has been split by SplitPolyWithPlane.

	// Internal.
	PF_EdProcessed 		= PF_FlatShaded,	// FPoly was already processed in editorBuildFPolys.
	PF_EdCut       		= 0x80000000,	// FPoly has been split by SplitPolyWithPlane.
	PF_RenderFog		= PF_FlatShaded,	// Render with fogmapping.
	PF_Occlude			= PF_EdCut,	// Occludes even if PF_NoOcclude.
	PF_RenderHint       = PF_Memorized,   // Rendering optimization hint (used to tell software render that DrawTile is 1:1 scale factor).
	PF_NoTexAnim		= PF_LowShadowDetail,   // Disable texture animation in UCanvas::DrawIcon

	// Combinations of flags.
	PF_NoOcclude		= PF_Masked | PF_Translucent | PF_Invisible | PF_Modulated | PF_AlphaBlend,
	PF_NoEdit			= PF_Memorized | PF_Selected | PF_EdProcessed | PF_NoMerge | PF_EdCut,
	PF_NoImport			= PF_NoEdit | PF_NoMerge | PF_Memorized | PF_Selected | PF_EdProcessed | PF_EdCut,
	PF_AddLast			= PF_Semisolid | PF_NotSolid,
	PF_NoAddToBSP		= PF_EdCut | PF_EdProcessed | PF_Selected | PF_Memorized,
	PF_NoShadows		= PF_Unlit | PF_Invisible | PF_FakeBackdrop | PF_OccluderPoly,
	PF_Transient		= PF_Highlighted,
};

#define PF_Straight_AlphaBlend PF_AlphaBlend
#define PF_Premultiplied_AlphaBlend PF_Highlighted

// 227 specific bonus polyflags.
enum EExtraPolyFlags
{
	EPF_None		= 0x00000000, 
	EPF_NoSplitSurf	= 0x00000001, // Do not allow to split this surface when BSP partitioning, to avoid impossible cuts.
	EPF_NoFog		= 0x00000002, // Disable volumetric fogging on this surface.
	EPF_ZoneHack	= 0x00000004, // Experimental surface zone hack fix.
};
