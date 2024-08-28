/*=============================================================================
	D3D8.cpp: Unreal Direct3D8 support implementation for Windows.
	Copyright 1999 Epic Games, Inc. All Rights Reserved.

	OpenGL renderer by Daniel Vogel <vogel@lokigames.com>
	Loki Software, Inc.

	Other URenderDevice subclasses include:
	* USoftwareRenderDevice: Software renderer.
	* UGlideRenderDevice: 3dfx Glide renderer.
	* UDirect3DRenderDevice: Direct3D renderer.
	* UD3D8RenderDevice: Direct3D8 renderer.

	Revision history:
	* Created by Daniel Vogel based on XMesaGLDrv
	* Changes (John Fulmer, Jeroen Janssen)
	* Major changes (Daniel Vogel)
	* Ported back to Win32 (Fredrik Gustafsson)
	* Unification and addition of vertex arrays (Daniel Vogel)
	* Actor triangle caching (Steve Sinclair)
	* One pass fogging (Daniel Vogel)
	* Windows gamma support (Daniel Vogel)
	* 2X blending support (Daniel Vogel)
	* Better detail texture handling (Daniel Vogel)
	* Scaleability (Daniel Vogel)
	* Texture LOD bias (Daniel Vogel)
	* RefreshRate support on Windows (Jason Dick)
	* Finer control over gamma (Daniel Vogel)
	* (NOT ALWAYS) Fixed Windows bitdepth switching (Daniel Vogel)

	* Various modifications and additions by Chris Dohnal
	* Initial TruForm based on TruForm renderer modifications by NitroGL
	* Additional TruForm and Deus Ex updates by Leonhard Gruenschloss


	UseTrilinear	whether to use trilinear filtering
	UseAlphaPalette	set to 0 for buggy drivers (GeForce)
	UseS3TC			whether to use compressed textures
	MaxAnisotropy	maximum level of anisotropy used
	MaxTMUnits		maximum number of TMUs UT will try to use
	LODBias			texture lod bias
	RefreshRate		requested refresh rate (Windows only)
	GammaOffset		offset for the gamma correction


TODO:
	- DOCUMENTATION!!! (especially all subtle assumptions)

=============================================================================*/

#include "D3D8Drv.h"
#include "D3D8.h"


/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

#ifdef UTD3D8R_USE_DEBUG_D3D8_DLL
static const char *g_d3d8DllName = "d3d8d.dll";
#else
static const char *g_d3d8DllName = "d3d8.dll";
#endif

#ifdef UTGLR_UNREAL_BUILD
// extern DWORD GUglyHackFlags = 0;
#endif

static const TCHAR *g_pSection = TEXT("D3D8Drv.D3D8RenderDevice");


/*-----------------------------------------------------------------------------
	Vertex programs.
-----------------------------------------------------------------------------*/

#if 0
//#define  D3DVSDE_POSITION      0
//#define  D3DVSDE_BLENDWEIGHT   1
//#define  D3DVSDE_BLENDINDICES  2
//#define  D3DVSDE_NORMAL        3
//#define  D3DVSDE_PSIZE         4
//#define  D3DVSDE_DIFFUSE       5
//#define  D3DVSDE_SPECULAR      6
//#define  D3DVSDE_TEXCOORD0     7
//#define  D3DVSDE_TEXCOORD1     8
//#define  D3DVSDE_TEXCOORD2     9
//#define  D3DVSDE_TEXCOORD3     10
//#define  D3DVSDE_TEXCOORD4     11
//#define  D3DVSDE_TEXCOORD5     12
//#define  D3DVSDE_TEXCOORD6     13
//#define  D3DVSDE_TEXCOORD7     14
//#define  D3DVSDE_POSITION2     15
//#define  D3DVSDE_NORMAL2       16
#endif

static const DWORD g_standardSingleTextureVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461,
	0x73734120, 0x6C626D65, 0x56207265, 0x69737265, 0x30206E6F,
	0x0031392E, 0x0014FFFE, 0x54584554, 0x312E7376, 0x2F0A312E,
	0x6C63642F, 0x736F705F, 0x6F697469, 0x3076206E, 0x642F2F0A,
	0x635F6C63, 0x726F6C6F, 0x31762030, 0x766F6D0A, 0x30446F20,
	0x3576202C, 0x78346D0A, 0x506F2034, 0x202C736F, 0x202C3076,
	0x5D305B63, 0x0000000A, 0x0002FFFE, 0x454E494C, 0x00000004,
	0x00000001, 0xD00F0000, 0x90E40005, 0x0002FFFE, 0x454E494C,
	0x00000005, 0x00000014, 0xC00F0000, 0x90E40000, 0xA0E40000,
	0x0000FFFF
};


//Vertex shader definitions
///////////////////////////

#if 0
static const char *g_tempShaderString =
	"vs.1.1\n"

	"dp3 r0.x, v0, c4\n"
	"dp3 r0.y, v0, c5\n"
	"add r0.x, r0.x, -c4.w\n"
	"add r0.y, r0.y, -c5.w\n"

	"add r1.xy, r0.xy, -c6.xy\n"
	"mul oT0.xy, r1.xy, c6.zw\n"

	"mov oD0, v5\n"

	"m4x4 oPos, v0, c0\n"
;
#endif
static const DWORD g_csSingleTextureVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461, 0x73734120, 0x6C626D65, 0x56207265,
	0x69737265, 0x30206E6F, 0x0031392E, 0x002BFFFE, 0x54584554, 0x312E7376, 0x640A312E, 0x72203370,
	0x2C782E30, 0x2C307620, 0x0A346320, 0x20337064, 0x792E3072, 0x3076202C, 0x3563202C, 0x6464610A,
	0x2E307220, 0x72202C78, 0x2C782E30, 0x34632D20, 0x610A772E, 0x72206464, 0x2C792E30, 0x2E307220,
	0x2D202C79, 0x772E3563, 0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79, 0x782E3663,
	0x756D0A79, 0x546F206C, 0x79782E30, 0x3172202C, 0x2C79782E, 0x2E366320, 0x6D0A777A, 0x6F20766F,
	0x202C3044, 0x6D0A3576, 0x20347834, 0x736F506F, 0x3076202C, 0x3063202C, 0x0000000A, 0x0002FFFE,
	0x454E494C, 0x00000002, 0x00000008, 0x80010000, 0x90E40000, 0xA0E40004, 0x0002FFFE, 0x454E494C,
	0x00000003, 0x00000008, 0x80020000, 0x90E40000, 0xA0E40005, 0x0002FFFE, 0x454E494C, 0x00000004,
	0x00000002, 0x80010000, 0x80000000, 0xA1FF0004, 0x0002FFFE, 0x454E494C, 0x00000005, 0x00000002,
	0x80020000, 0x80550000, 0xA1FF0005, 0x0002FFFE, 0x454E494C, 0x00000006, 0x00000002, 0x80030001,
	0x80540000, 0xA1540006, 0x0002FFFE, 0x454E494C, 0x00000007, 0x00000005, 0xE0030000, 0x80540001,
	0xA0FE0006, 0x0002FFFE, 0x454E494C, 0x00000008, 0x00000001, 0xD00F0000, 0x90E40005, 0x0002FFFE,
	0x454E494C, 0x00000009, 0x00000014, 0xC00F0000, 0x90E40000, 0xA0E40000, 0x0000FFFF
};

#if 0
static const char *g_tempShaderString =
	"vs.1.1\n"

	"dp3 r0.x, v0, c4\n"
	"dp3 r0.y, v0, c5\n"
	"add r0.x, r0.x, -c4.w\n"
	"add r0.y, r0.y, -c5.w\n"

	"add r1.xy, r0.xy, -c6.xy\n"
	"mul oT0.xy, r1.xy, c6.zw\n"

	"add r1.xy, r0.xy, -c7.xy\n"
	"mul oT1.xy, r1.xy, c7.zw\n"

	"mov oD0, v5\n"

	"m4x4 oPos, v0, c0\n"
;
#endif
static const DWORD g_csDoubleTextureVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461, 0x73734120, 0x6C626D65, 0x56207265,
	0x69737265, 0x30206E6F, 0x0031392E, 0x0037FFFE, 0x54584554, 0x312E7376, 0x640A312E, 0x72203370,
	0x2C782E30, 0x2C307620, 0x0A346320, 0x20337064, 0x792E3072, 0x3076202C, 0x3563202C, 0x6464610A,
	0x2E307220, 0x72202C78, 0x2C782E30, 0x34632D20, 0x610A772E, 0x72206464, 0x2C792E30, 0x2E307220,
	0x2D202C79, 0x772E3563, 0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79, 0x782E3663,
	0x756D0A79, 0x546F206C, 0x79782E30, 0x3172202C, 0x2C79782E, 0x2E366320, 0x610A777A, 0x72206464,
	0x79782E31, 0x3072202C, 0x2C79782E, 0x37632D20, 0x0A79782E, 0x206C756D, 0x2E31546F, 0x202C7978,
	0x782E3172, 0x63202C79, 0x777A2E37, 0x766F6D0A, 0x30446F20, 0x3576202C, 0x78346D0A, 0x506F2034,
	0x202C736F, 0x202C3076, 0x000A3063, 0x0002FFFE, 0x454E494C, 0x00000002, 0x00000008, 0x80010000,
	0x90E40000, 0xA0E40004, 0x0002FFFE, 0x454E494C, 0x00000003, 0x00000008, 0x80020000, 0x90E40000,
	0xA0E40005, 0x0002FFFE, 0x454E494C, 0x00000004, 0x00000002, 0x80010000, 0x80000000, 0xA1FF0004,
	0x0002FFFE, 0x454E494C, 0x00000005, 0x00000002, 0x80020000, 0x80550000, 0xA1FF0005, 0x0002FFFE,
	0x454E494C, 0x00000006, 0x00000002, 0x80030001, 0x80540000, 0xA1540006, 0x0002FFFE, 0x454E494C,
	0x00000007, 0x00000005, 0xE0030000, 0x80540001, 0xA0FE0006, 0x0002FFFE, 0x454E494C, 0x00000008,
	0x00000002, 0x80030001, 0x80540000, 0xA1540007, 0x0002FFFE, 0x454E494C, 0x00000009, 0x00000005,
	0xE0030001, 0x80540001, 0xA0FE0007, 0x0002FFFE, 0x454E494C, 0x0000000A, 0x00000001, 0xD00F0000,
	0x90E40005, 0x0002FFFE, 0x454E494C, 0x0000000B, 0x00000014, 0xC00F0000, 0x90E40000, 0xA0E40000,
	0x0000FFFF
};

#if 0
static const char *g_tempShaderString =
	"vs.1.1\n"

	"dp3 r0.x, v0, c4\n"
	"dp3 r0.y, v0, c5\n"
	"add r0.x, r0.x, -c4.w\n"
	"add r0.y, r0.y, -c5.w\n"

	"add r1.xy, r0.xy, -c6.xy\n"
	"mul oT0.xy, r1.xy, c6.zw\n"

	"add r1.xy, r0.xy, -c7.xy\n"
	"mul oT1.xy, r1.xy, c7.zw\n"

	"add r1.xy, r0.xy, -c8.xy\n"
	"mul oT2.xy, r1.xy, c8.zw\n"

	"mov oD0, v5\n"

	"m4x4 oPos, v0, c0\n"
;
#endif
static const DWORD g_csTripleTextureVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461, 0x73734120, 0x6C626D65, 0x56207265,
	0x69737265, 0x30206E6F, 0x0031392E, 0x0044FFFE, 0x54584554, 0x312E7376, 0x640A312E, 0x72203370,
	0x2C782E30, 0x2C307620, 0x0A346320, 0x20337064, 0x792E3072, 0x3076202C, 0x3563202C, 0x6464610A,
	0x2E307220, 0x72202C78, 0x2C782E30, 0x34632D20, 0x610A772E, 0x72206464, 0x2C792E30, 0x2E307220,
	0x2D202C79, 0x772E3563, 0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79, 0x782E3663,
	0x756D0A79, 0x546F206C, 0x79782E30, 0x3172202C, 0x2C79782E, 0x2E366320, 0x610A777A, 0x72206464,
	0x79782E31, 0x3072202C, 0x2C79782E, 0x37632D20, 0x0A79782E, 0x206C756D, 0x2E31546F, 0x202C7978,
	0x782E3172, 0x63202C79, 0x777A2E37, 0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79,
	0x782E3863, 0x756D0A79, 0x546F206C, 0x79782E32, 0x3172202C, 0x2C79782E, 0x2E386320, 0x6D0A777A,
	0x6F20766F, 0x202C3044, 0x6D0A3576, 0x20347834, 0x736F506F, 0x3076202C, 0x3063202C, 0x0000000A,
	0x0002FFFE, 0x454E494C, 0x00000002, 0x00000008, 0x80010000, 0x90E40000, 0xA0E40004, 0x0002FFFE,
	0x454E494C, 0x00000003, 0x00000008, 0x80020000, 0x90E40000, 0xA0E40005, 0x0002FFFE, 0x454E494C,
	0x00000004, 0x00000002, 0x80010000, 0x80000000, 0xA1FF0004, 0x0002FFFE, 0x454E494C, 0x00000005,
	0x00000002, 0x80020000, 0x80550000, 0xA1FF0005, 0x0002FFFE, 0x454E494C, 0x00000006, 0x00000002,
	0x80030001, 0x80540000, 0xA1540006, 0x0002FFFE, 0x454E494C, 0x00000007, 0x00000005, 0xE0030000,
	0x80540001, 0xA0FE0006, 0x0002FFFE, 0x454E494C, 0x00000008, 0x00000002, 0x80030001, 0x80540000,
	0xA1540007, 0x0002FFFE, 0x454E494C, 0x00000009, 0x00000005, 0xE0030001, 0x80540001, 0xA0FE0007,
	0x0002FFFE, 0x454E494C, 0x0000000A, 0x00000002, 0x80030001, 0x80540000, 0xA1540008, 0x0002FFFE,
	0x454E494C, 0x0000000B, 0x00000005, 0xE0030002, 0x80540001, 0xA0FE0008, 0x0002FFFE, 0x454E494C,
	0x0000000C, 0x00000001, 0xD00F0000, 0x90E40005, 0x0002FFFE, 0x454E494C, 0x0000000D, 0x00000014,
	0xC00F0000, 0x90E40000, 0xA0E40000, 0x0000FFFF
};

#if 0
static const char *g_tempShaderString =
	"vs.1.1\n"

	"dp3 r0.x, v0, c4\n"
	"dp3 r0.y, v0, c5\n"
	"add r0.x, r0.x, -c4.w\n"
	"add r0.y, r0.y, -c5.w\n"

	"add r1.xy, r0.xy, -c6.xy\n"
	"mul oT0.xy, r1.xy, c6.zw\n"

	"add r1.xy, r0.xy, -c7.xy\n"
	"mul oT1.xy, r1.xy, c7.zw\n"

	"add r1.xy, r0.xy, -c8.xy\n"
	"mul oT2.xy, r1.xy, c8.zw\n"

	"add r1.xy, r0.xy, -c9.xy\n"
	"mul oT3.xy, r1.xy, c9.zw\n"

	"mov oD0, v5\n"

	"m4x4 oPos, v0, c0\n"
;
#endif
static const DWORD g_csQuadTextureVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461, 0x73734120, 0x6C626D65, 0x56207265,
	0x69737265, 0x30206E6F, 0x0031392E, 0x0050FFFE, 0x54584554, 0x312E7376, 0x640A312E, 0x72203370,
	0x2C782E30, 0x2C307620, 0x0A346320, 0x20337064, 0x792E3072, 0x3076202C, 0x3563202C, 0x6464610A,
	0x2E307220, 0x72202C78, 0x2C782E30, 0x34632D20, 0x610A772E, 0x72206464, 0x2C792E30, 0x2E307220,
	0x2D202C79, 0x772E3563, 0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79, 0x782E3663,
	0x756D0A79, 0x546F206C, 0x79782E30, 0x3172202C, 0x2C79782E, 0x2E366320, 0x610A777A, 0x72206464,
	0x79782E31, 0x3072202C, 0x2C79782E, 0x37632D20, 0x0A79782E, 0x206C756D, 0x2E31546F, 0x202C7978,
	0x782E3172, 0x63202C79, 0x777A2E37, 0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79,
	0x782E3863, 0x756D0A79, 0x546F206C, 0x79782E32, 0x3172202C, 0x2C79782E, 0x2E386320, 0x610A777A,
	0x72206464, 0x79782E31, 0x3072202C, 0x2C79782E, 0x39632D20, 0x0A79782E, 0x206C756D, 0x2E33546F,
	0x202C7978, 0x782E3172, 0x63202C79, 0x777A2E39, 0x766F6D0A, 0x30446F20, 0x3576202C, 0x78346D0A,
	0x506F2034, 0x202C736F, 0x202C3076, 0x000A3063, 0x0002FFFE, 0x454E494C, 0x00000002, 0x00000008,
	0x80010000, 0x90E40000, 0xA0E40004, 0x0002FFFE, 0x454E494C, 0x00000003, 0x00000008, 0x80020000,
	0x90E40000, 0xA0E40005, 0x0002FFFE, 0x454E494C, 0x00000004, 0x00000002, 0x80010000, 0x80000000,
	0xA1FF0004, 0x0002FFFE, 0x454E494C, 0x00000005, 0x00000002, 0x80020000, 0x80550000, 0xA1FF0005,
	0x0002FFFE, 0x454E494C, 0x00000006, 0x00000002, 0x80030001, 0x80540000, 0xA1540006, 0x0002FFFE,
	0x454E494C, 0x00000007, 0x00000005, 0xE0030000, 0x80540001, 0xA0FE0006, 0x0002FFFE, 0x454E494C,
	0x00000008, 0x00000002, 0x80030001, 0x80540000, 0xA1540007, 0x0002FFFE, 0x454E494C, 0x00000009,
	0x00000005, 0xE0030001, 0x80540001, 0xA0FE0007, 0x0002FFFE, 0x454E494C, 0x0000000A, 0x00000002,
	0x80030001, 0x80540000, 0xA1540008, 0x0002FFFE, 0x454E494C, 0x0000000B, 0x00000005, 0xE0030002,
	0x80540001, 0xA0FE0008, 0x0002FFFE, 0x454E494C, 0x0000000C, 0x00000002, 0x80030001, 0x80540000,
	0xA1540009, 0x0002FFFE, 0x454E494C, 0x0000000D, 0x00000005, 0xE0030003, 0x80540001, 0xA0FE0009,
	0x0002FFFE, 0x454E494C, 0x0000000E, 0x00000001, 0xD00F0000, 0x90E40005, 0x0002FFFE, 0x454E494C,
	0x0000000F, 0x00000014, 0xC00F0000, 0x90E40000, 0xA0E40000, 0x0000FFFF
};

#if 0
static const char *g_tempShaderString =
	"vs.1.1\n"

	"dp3 r0.x, v0, c4\n"
	"dp3 r0.y, v0, c5\n"
	"add r0.x, r0.x, -c4.w\n"
	"add r0.y, r0.y, -c5.w\n"

	"mul oT0.x, v0.z, c6.x\n"
	"mov oT0.y, c6.y\n"

	"add r1.xy, r0.xy, -c7.xy\n"
	"mul oT1.xy, r1.xy, c7.zw\n"

	"mov oD0, v5\n"

	"m4x4 oPos, v0, c0\n"
;
#endif
static const DWORD g_csDetailAlphaVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461, 0x73734120, 0x6C626D65, 0x56207265,
	0x69737265, 0x30206E6F, 0x0031392E, 0x0034FFFE, 0x54584554, 0x312E7376, 0x640A312E, 0x72203370,
	0x2C782E30, 0x2C307620, 0x0A346320, 0x20337064, 0x792E3072, 0x3076202C, 0x3563202C, 0x6464610A,
	0x2E307220, 0x72202C78, 0x2C782E30, 0x34632D20, 0x610A772E, 0x72206464, 0x2C792E30, 0x2E307220,
	0x2D202C79, 0x772E3563, 0x6C756D0A, 0x30546F20, 0x202C782E, 0x7A2E3076, 0x3663202C, 0x6D0A782E,
	0x6F20766F, 0x792E3054, 0x3663202C, 0x610A792E, 0x72206464, 0x79782E31, 0x3072202C, 0x2C79782E,
	0x37632D20, 0x0A79782E, 0x206C756D, 0x2E31546F, 0x202C7978, 0x782E3172, 0x63202C79, 0x777A2E37,
	0x766F6D0A, 0x30446F20, 0x3576202C, 0x78346D0A, 0x506F2034, 0x202C736F, 0x202C3076, 0x000A3063,
	0x0002FFFE, 0x454E494C, 0x00000002, 0x00000008, 0x80010000, 0x90E40000, 0xA0E40004, 0x0002FFFE,
	0x454E494C, 0x00000003, 0x00000008, 0x80020000, 0x90E40000, 0xA0E40005, 0x0002FFFE, 0x454E494C,
	0x00000004, 0x00000002, 0x80010000, 0x80000000, 0xA1FF0004, 0x0002FFFE, 0x454E494C, 0x00000005,
	0x00000002, 0x80020000, 0x80550000, 0xA1FF0005, 0x0002FFFE, 0x454E494C, 0x00000006, 0x00000005,
	0xE0010000, 0x90AA0000, 0xA0000006, 0x0002FFFE, 0x454E494C, 0x00000007, 0x00000001, 0xE0020000,
	0xA0550006, 0x0002FFFE, 0x454E494C, 0x00000008, 0x00000002, 0x80030001, 0x80540000, 0xA1540007,
	0x0002FFFE, 0x454E494C, 0x00000009, 0x00000005, 0xE0030001, 0x80540001, 0xA0FE0007, 0x0002FFFE,
	0x454E494C, 0x0000000A, 0x00000001, 0xD00F0000, 0x90E40005, 0x0002FFFE, 0x454E494C, 0x0000000B,
	0x00000014, 0xC00F0000, 0x90E40000, 0xA0E40000, 0x0000FFFF
};

#if 0
static const char *g_tempShaderString =
	"vs.1.1\n"

	"dp3 r0.x, v0, c4\n"
	"dp3 r0.y, v0, c5\n"
	"add r0.x, r0.x, -c4.w\n"
	"add r0.y, r0.y, -c5.w\n"

	"mul oT0.x, v0.z, c6.x\n"
	"mov oT0.y, c6.y\n"

	"add r1.xy, r0.xy, -c7.xy\n"
	"mul oT1.xy, r1.xy, c7.zw\n"

	"add r1.xy, r0.xy, -c8.xy\n"
	"mul oT2.xy, r1.xy, c8.zw\n"

	"add r1.xy, r0.xy, -c9.xy\n"
	"mul oT3.xy, r1.xy, c9.zw\n"

	"mov oD0, v5\n"

	"m4x4 oPos, v0, c0\n"
;
#endif
static const DWORD g_csDualTextureAndDetailTextureVertexShaderDef[] = {
	0xFFFE0101, 0x0009FFFE, 0x58443344, 0x68532038, 0x72656461, 0x73734120, 0x6C626D65, 0x56207265,
	0x69737265, 0x30206E6F, 0x0031392E, 0x004DFFFE, 0x54584554, 0x312E7376, 0x640A312E, 0x72203370,
	0x2C782E30, 0x2C307620, 0x0A346320, 0x20337064, 0x792E3072, 0x3076202C, 0x3563202C, 0x6464610A,
	0x2E307220, 0x72202C78, 0x2C782E30, 0x34632D20, 0x610A772E, 0x72206464, 0x2C792E30, 0x2E307220,
	0x2D202C79, 0x772E3563, 0x6C756D0A, 0x30546F20, 0x202C782E, 0x7A2E3076, 0x3663202C, 0x6D0A782E,
	0x6F20766F, 0x792E3054, 0x3663202C, 0x610A792E, 0x72206464, 0x79782E31, 0x3072202C, 0x2C79782E,
	0x37632D20, 0x0A79782E, 0x206C756D, 0x2E31546F, 0x202C7978, 0x782E3172, 0x63202C79, 0x777A2E37,
	0x6464610A, 0x2E317220, 0x202C7978, 0x782E3072, 0x2D202C79, 0x782E3863, 0x756D0A79, 0x546F206C,
	0x79782E32, 0x3172202C, 0x2C79782E, 0x2E386320, 0x610A777A, 0x72206464, 0x79782E31, 0x3072202C,
	0x2C79782E, 0x39632D20, 0x0A79782E, 0x206C756D, 0x2E33546F, 0x202C7978, 0x782E3172, 0x63202C79,
	0x777A2E39, 0x766F6D0A, 0x30446F20, 0x3576202C, 0x78346D0A, 0x506F2034, 0x202C736F, 0x202C3076,
	0x000A3063, 0x0002FFFE, 0x454E494C, 0x00000002, 0x00000008, 0x80010000, 0x90E40000, 0xA0E40004,
	0x0002FFFE, 0x454E494C, 0x00000003, 0x00000008, 0x80020000, 0x90E40000, 0xA0E40005, 0x0002FFFE,
	0x454E494C, 0x00000004, 0x00000002, 0x80010000, 0x80000000, 0xA1FF0004, 0x0002FFFE, 0x454E494C,
	0x00000005, 0x00000002, 0x80020000, 0x80550000, 0xA1FF0005, 0x0002FFFE, 0x454E494C, 0x00000006,
	0x00000005, 0xE0010000, 0x90AA0000, 0xA0000006, 0x0002FFFE, 0x454E494C, 0x00000007, 0x00000001,
	0xE0020000, 0xA0550006, 0x0002FFFE, 0x454E494C, 0x00000008, 0x00000002, 0x80030001, 0x80540000,
	0xA1540007, 0x0002FFFE, 0x454E494C, 0x00000009, 0x00000005, 0xE0030001, 0x80540001, 0xA0FE0007,
	0x0002FFFE, 0x454E494C, 0x0000000A, 0x00000002, 0x80030001, 0x80540000, 0xA1540008, 0x0002FFFE,
	0x454E494C, 0x0000000B, 0x00000005, 0xE0030002, 0x80540001, 0xA0FE0008, 0x0002FFFE, 0x454E494C,
	0x0000000C, 0x00000002, 0x80030001, 0x80540000, 0xA1540009, 0x0002FFFE, 0x454E494C, 0x0000000D,
	0x00000005, 0xE0030003, 0x80540001, 0xA0FE0009, 0x0002FFFE, 0x454E494C, 0x0000000E, 0x00000001,
	0xD00F0000, 0x90E40005, 0x0002FFFE, 0x454E494C, 0x0000000F, 0x00000014, 0xC00F0000, 0x90E40000,
	0xA0E40000, 0x0000FFFF
};


//Stream definitions
////////////////////

static const DWORD g_oneColorStreamDef[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
	D3DVSD_END()
};

static const DWORD g_standardSingleTextureStreamDef[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
	D3DVSD_STREAM(2),
	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
	D3DVSD_END()
};

static const DWORD g_standardDoubleTextureStreamDef[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
	D3DVSD_STREAM(2),
	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
	D3DVSD_STREAM(3),
	D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
	D3DVSD_END()
};

static const DWORD g_standardTripleTextureStreamDef[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
	D3DVSD_STREAM(2),
	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
	D3DVSD_STREAM(3),
	D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
	D3DVSD_STREAM(4),
	D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
	D3DVSD_END()
};

static const DWORD g_standardQuadTextureStreamDef[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
	D3DVSD_STREAM(2),
	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
	D3DVSD_STREAM(3),
	D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
	D3DVSD_STREAM(4),
	D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
	D3DVSD_STREAM(5),
	D3DVSD_REG(D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT2),
	D3DVSD_END()
};

static const DWORD *g_standardNTextureStreamDefs[MAX_TMUNITS] = {
	g_standardSingleTextureStreamDef,
	g_standardDoubleTextureStreamDef,
	g_standardTripleTextureStreamDef,
	g_standardQuadTextureStreamDef
};

static const DWORD g_twoColorSingleTextureStreamDef[] = {
	D3DVSD_STREAM(0),
	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
	D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
	D3DVSD_STREAM(1),
	D3DVSD_REG(D3DVSDE_SPECULAR, D3DVSDT_D3DCOLOR),
	D3DVSD_STREAM(2),
	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
	D3DVSD_END()
};


/*-----------------------------------------------------------------------------
	D3D8Drv.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UD3D8RenderDevice);


void UD3D8RenderDevice::StaticConstructor() {
	unsigned int u;

	guard(UD3D8RenderDevice::StaticConstructor);

#ifdef UTGLR_INCLUDE_OPCODE_PATCH_CODE
	{
		COpcodePatcher opcodePatcher;

		opcodePatcher.ApplyImagePatch();
	}
#endif

#ifdef UTGLR_DX_BUILD
#define UTGLR_DEFAULT_ONEXBLENDING	1
#else
#define UTGLR_DEFAULT_ONEXBLENDING	0
#endif

#define CPP_PROPERTY_LOCAL(_name) _name, CPP_PROPERTY(_name)
#define CPP_PROPERTY_LOCAL_DCV(_name) _name, CPP_PROPERTY(DCV._name)

	UEnum* VSyncs = new(GetClass(), TEXT("VSyncs"))UEnum(NULL);

	// Enum members.
	new(VSyncs->Names)FName(TEXT("Off"));
	new(VSyncs->Names)FName(TEXT("On"));

	//Set parameter defaults and add parameters
	//SC_AddBoolConfigParam(0,  TEXT("DetailTextures"), CPP_PROPERTY_LOCAL(DetailTextures), 0);
	SC_AddFloatConfigParam(TEXT("LODBias"), CPP_PROPERTY_LOCAL(LODBias), 0.0f);
	SC_AddFloatConfigParam(TEXT("GammaOffset"), CPP_PROPERTY_LOCAL(GammaOffset), 0.0f);
	SC_AddFloatConfigParam(TEXT("GammaOffsetRed"), CPP_PROPERTY_LOCAL(GammaOffsetRed), 0.0f);
	SC_AddFloatConfigParam(TEXT("GammaOffsetGreen"), CPP_PROPERTY_LOCAL(GammaOffsetGreen), 0.0f);
	SC_AddFloatConfigParam(TEXT("GammaOffsetBlue"), CPP_PROPERTY_LOCAL(GammaOffsetBlue), 0.0f);
	SC_AddBoolConfigParam(1,  TEXT("GammaCorrectScreenshots"), CPP_PROPERTY_LOCAL(GammaCorrectScreenshots), 0);
	SC_AddBoolConfigParam(0,  TEXT("OneXBlending"), CPP_PROPERTY_LOCAL(OneXBlending), UTGLR_DEFAULT_ONEXBLENDING);
	SC_AddIntConfigParam(TEXT("MaxLogUOverV"), CPP_PROPERTY_LOCAL(MaxLogUOverV), 8);
	SC_AddIntConfigParam(TEXT("MaxLogVOverU"), CPP_PROPERTY_LOCAL(MaxLogVOverU), 8);
	SC_AddIntConfigParam(TEXT("MinLogTextureSize"), CPP_PROPERTY_LOCAL(MinLogTextureSize), 0);
	SC_AddIntConfigParam(TEXT("MaxLogTextureSize"), CPP_PROPERTY_LOCAL(MaxLogTextureSize), 8);
	SC_AddBoolConfigParam(7,  TEXT("UseMultiTexture"), CPP_PROPERTY_LOCAL(UseMultiTexture), 1);
	SC_AddBoolConfigParam(6,  TEXT("UsePalette"), CPP_PROPERTY_LOCAL(UsePalette), 0);
	SC_AddBoolConfigParam(5,  TEXT("UsePrecache"), CPP_PROPERTY_LOCAL(UsePrecache), 0);
	SC_AddBoolConfigParam(4,  TEXT("UseTrilinear"), CPP_PROPERTY_LOCAL(UseTrilinear), 0);
	SC_AddBoolConfigParam(3,  TEXT("UseAlphaPalette"), CPP_PROPERTY_LOCAL(UseAlphaPalette), 0);
	SC_AddBoolConfigParam(2,  TEXT("UseS3TC"), CPP_PROPERTY_LOCAL(UseS3TC), 0);
	SC_AddBoolConfigParam(1,  TEXT("Use16BitTextures"), CPP_PROPERTY_LOCAL(Use16BitTextures), 0);
	SC_AddBoolConfigParam(0,  TEXT("Use565Textures"), CPP_PROPERTY_LOCAL(Use565Textures), 0);
	SC_AddIntConfigParam(TEXT("MaxAnisotropy"), CPP_PROPERTY_LOCAL(MaxAnisotropy), 0);
	SC_AddBoolConfigParam(0,  TEXT("NoFiltering"), CPP_PROPERTY_LOCAL(NoFiltering), 0);
	SC_AddIntConfigParam(TEXT("MaxTMUnits"), CPP_PROPERTY_LOCAL(MaxTMUnits), 0);
	SC_AddIntConfigParam(TEXT("RefreshRate"), CPP_PROPERTY_LOCAL(RefreshRate), 0);
	SC_AddIntConfigParam(TEXT("DetailMax"), CPP_PROPERTY_LOCAL(DetailMax), 0);
	SC_AddBoolConfigParam(11, TEXT("UseDetailAlpha"), CPP_PROPERTY_LOCAL(UseDetailAlpha), 1);
	SC_AddBoolConfigParam(10, TEXT("DetailClipping"), CPP_PROPERTY_LOCAL(DetailClipping), 0);
	SC_AddBoolConfigParam(9,  TEXT("ColorizeDetailTextures"), CPP_PROPERTY_LOCAL(ColorizeDetailTextures), 0);
	SC_AddBoolConfigParam(8,  TEXT("SinglePassFog"), CPP_PROPERTY_LOCAL(SinglePassFog), 0);
	SC_AddBoolConfigParam(7,  TEXT("SinglePassDetail"), CPP_PROPERTY_LOCAL_DCV(SinglePassDetail), 0);
	SC_AddBoolConfigParam(6,  TEXT("BufferClippedActorTris"), CPP_PROPERTY_LOCAL(BufferClippedActorTris), 1);
	SC_AddBoolConfigParam(5,  TEXT("BufferTileQuads"), CPP_PROPERTY_LOCAL(BufferTileQuads), 1);
	SC_AddBoolConfigParam(4,  TEXT("UseSSE"), CPP_PROPERTY_LOCAL(UseSSE), 1);
	SC_AddBoolConfigParam(3,  TEXT("UseSSE2"), CPP_PROPERTY_LOCAL(UseSSE2), 1);
	SC_AddBoolConfigParam(2,  TEXT("UseTexIdPool"), CPP_PROPERTY_LOCAL(UseTexIdPool), 1);
	SC_AddBoolConfigParam(1,  TEXT("UseTexPool"), CPP_PROPERTY_LOCAL(UseTexPool), 1);
	SC_AddBoolConfigParam(0,  TEXT("CacheStaticMaps"), CPP_PROPERTY_LOCAL(CacheStaticMaps), 0);
	SC_AddIntConfigParam(TEXT("DynamicTexIdRecycleLevel"), CPP_PROPERTY_LOCAL(DynamicTexIdRecycleLevel), 100);
	SC_AddBoolConfigParam(1,  TEXT("TexDXT1ToDXT3"), CPP_PROPERTY_LOCAL(TexDXT1ToDXT3), 0);
	SC_AddBoolConfigParam(0,  TEXT("UseVertexProgram"), CPP_PROPERTY_LOCAL_DCV(UseVertexProgram), 0);
	SC_AddEnumConfigParam(TEXT("UseVSync"), CPP_PROPERTY_LOCAL(UseVSync), VSyncs, 1);
	SC_AddIntConfigParam(TEXT("FrameRateLimit"), CPP_PROPERTY_LOCAL(FrameRateLimit), 0);
	SC_AddBoolConfigParam(7,  TEXT("SceneNodeHack"), CPP_PROPERTY_LOCAL(SceneNodeHack), 1);
	SC_AddBoolConfigParam(6,  TEXT("SmoothMaskedTextures"), CPP_PROPERTY_LOCAL(SmoothMaskedTextures), 0);
	SC_AddBoolConfigParam(5,  TEXT("MaskedTextureHack"), CPP_PROPERTY_LOCAL(MaskedTextureHack), 0);
	SC_AddBoolConfigParam(4,  TEXT("UseTripleBuffering"), CPP_PROPERTY_LOCAL(UseTripleBuffering), 0);
	SC_AddBoolConfigParam(3,  TEXT("UsePureDevice"), CPP_PROPERTY_LOCAL(UsePureDevice), 0);
	SC_AddBoolConfigParam(2,  TEXT("UseSoftwareVertexProcessing"), CPP_PROPERTY_LOCAL(UseSoftwareVertexProcessing), 0);
	SC_AddBoolConfigParam(1,  TEXT("RequestHighResolutionZ"), CPP_PROPERTY_LOCAL(RequestHighResolutionZ), 1);
	SC_AddBoolConfigParam(0,  TEXT("UseAA"), CPP_PROPERTY_LOCAL(UseAA), 0);
	SC_AddIntConfigParam(TEXT("NumAASamples"), CPP_PROPERTY_LOCAL(NumAASamples), 0);
	SC_AddBoolConfigParam(1,  TEXT("NoAATiles"), CPP_PROPERTY_LOCAL(NoAATiles), 0);
	SC_AddBoolConfigParam(0,  TEXT("ZRangeHack"), CPP_PROPERTY_LOCAL(ZRangeHack), 0);

#undef CPP_PROPERTY_LOCAL
#undef CPP_PROPERTY_LOCAL_DCV

	//Driver flags
	SpanBased				= 0;
	SupportsFogMaps			= 1;
#ifdef UTGLR_RUNE_BUILD
	SupportsDistanceFog		= 1;
#elif UTGLR_UNREAL_227_BUILD
	SupportsDistanceFog		= 1;
	LegacyRenderingOnly		= 1;
#else
	SupportsDistanceFog		= 0;
#endif
	FullscreenOnly			= 0;

	SupportsLazyTextures	= 0;
	PrefersDeferredLoad		= 0;
	SupportsNoClipRender	= 0;

	//Mark device pointers as invalid
	m_d3d8 = NULL;
	m_d3dDevice = NULL;

	//Invalidate fixed texture ids
	m_pNoTexObj = NULL;
	m_pAlphaTexObj = NULL;

	//Mark all vertex buffer objects as invalid
	m_d3dVertexColorBuffer = NULL;
	m_d3dSecondaryColorBuffer = NULL;
	for (u = 0; u < MAX_TMUNITS; u++) {
		m_d3dTexCoordBuffer[u] = NULL;
	}

	//Mark all stream shader definitions as not created
	for (u = 0; u < MAX_TMUNITS; u++) {
		m_standardNTextureStreamShaderCreated[u] = false;
	}
	m_twoColorSingleTextureStreamShaderCreated = false;

	//Mark all vertex shader definitions as not created
	for (u = 0; u < MAX_TMUNITS; u++) {
		m_csNTextureVertexShaderCreated[u] = false;
	}
	m_csDetailAlphaVertexShaderCreated = false;
	m_csDualTextureAndDetailTextureVertexShaderCreated = false;
	m_standardSingleTextureVertexShaderCreated = false;

	//Reset TMUnits in case resource cleanup code is ever called before this is initialized
	TMUnits = 0;

	//Clear the SetRes is device reset flag
	m_SetRes_isDeviceReset = false;

	unguard;
}


void UD3D8RenderDevice::SC_AddBoolConfigParam(DWORD BitMaskOffset, const TCHAR *pName, UBOOL &param, ECppProperty EC_CppProperty, INT InOffset, UBOOL defaultValue) {
	param = (((defaultValue) != 0) ? 1 : 0) << BitMaskOffset; //Doesn't exactly work like a UBOOL "// Boolean 0 (false) or 1 (true)."
	new(GetClass(), pName, RF_Public)UBoolProperty(EC_CppProperty, InOffset, TEXT("Options"), CPF_Config);
}

void UD3D8RenderDevice::SC_AddIntConfigParam(const TCHAR *pName, INT &param, ECppProperty EC_CppProperty, INT InOffset, INT defaultValue) {
	param = defaultValue;
	new(GetClass(), pName, RF_Public)UIntProperty(EC_CppProperty, InOffset, TEXT("Options"), CPF_Config);
}

void UD3D8RenderDevice::SC_AddFloatConfigParam(const TCHAR *pName, FLOAT &param, ECppProperty EC_CppProperty, INT InOffset, FLOAT defaultValue) {
	param = defaultValue;
	new(GetClass(), pName, RF_Public)UFloatProperty(EC_CppProperty, InOffset, TEXT("Options"), CPF_Config);
}

void UD3D8RenderDevice::SC_AddByteConfigParam(const TCHAR *pName, BYTE &param, ECppProperty EC_CppProperty, INT InOffset, BYTE defaultValue) {
	param = defaultValue;
	new(GetClass(), pName, RF_Public)UByteProperty(EC_CppProperty, InOffset, TEXT("Options"), CPF_Config);
}

void UD3D8RenderDevice::SC_AddEnumConfigParam(const TCHAR *pName, BYTE &param, ECppProperty EC_CppProperty, INT InOffset, UEnum* Enum, BYTE defaultValue) {
	param = defaultValue;
	new(GetClass(), pName, RF_Public)UByteProperty(EC_CppProperty, InOffset, TEXT("Options"), CPF_Config, Enum);
}


void UD3D8RenderDevice::DbgPrintInitParam(const TCHAR *pName, INT value) {
	dout << TEXT("utd3d8r: ") << pName << TEXT(" = ") << value << std::endl;
	return;
}

void UD3D8RenderDevice::DbgPrintInitParam(const TCHAR *pName, FLOAT value) {
	dout << TEXT("utd3d8r: ") << pName << TEXT(" = ") << value << std::endl;
	return;
}


#ifdef UTGLR_INCLUDE_SSE_CODE
bool UD3D8RenderDevice::CPU_DetectCPUID(void) {
	//Check for cpuid instruction support
	__try {
		__asm {
			//CPUID function 0
			xor eax, eax
			cpuid
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}

	return true;
}

bool UD3D8RenderDevice::CPU_DetectSSE(void) {
	bool bSupportsSSE;

	//Check for cpuid instruction support
	if (CPU_DetectCPUID() != true) {
		return false;
	}

	//Check for SSE support
	bSupportsSSE = false;
	__asm {
		//CPUID function 1
		mov eax, 1
		cpuid

		//Check the SSE bit
		test edx, 0x02000000
		jz l_no_sse

		//Set bSupportsSSE to true
		mov bSupportsSSE, 1

l_no_sse:
	}

	//Return if no CPU SSE support
	if (bSupportsSSE == false) {
		return bSupportsSSE;
	}

	//Check for SSE OS support
	__try {
		__asm {
			//Execute SSE instruction
			xorps xmm0, xmm0
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		//Clear SSE support flag
		bSupportsSSE = false;
	}

	return bSupportsSSE;
}

bool UD3D8RenderDevice::CPU_DetectSSE2(void) {
	bool bSupportsSSE2;

	//Check for cpuid instruction support
	if (CPU_DetectCPUID() != true) {
		return false;
	}

	//Check for SSE2 support
	bSupportsSSE2 = false;
	__asm {
		//CPUID function 1
		mov eax, 1
		cpuid

		//Check the SSE2 bit
		test edx, 0x04000000
		jz l_no_sse2

		//Set bSupportsSSE2 to true
		mov bSupportsSSE2, 1

l_no_sse2:
	}

	//Return if no CPU SSE2 support
	if (bSupportsSSE2 == false) {
		return bSupportsSSE2;
	}

	//Check for SSE2 OS support
	__try {
		__asm {
			//Execute SSE2 instruction
			xorpd xmm0, xmm0
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		//Clear SSE2 support flag
		bSupportsSSE2 = false;
	}

	return bSupportsSSE2;
}
#endif //UTGLR_INCLUDE_SSE_CODE


static void FASTCALL Buffer3Verts(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	FGLTexCoord *pTexCoordArray = &pRD->m_pTexCoordArray[0][pRD->BufferedVerts];
	FGLVertexColor *pVertexColorArray = &pRD->m_pVertexColorArray[pRD->BufferedVerts];
	FGLSecondaryColor *pSecondaryColorArray = &pRD->m_pSecondaryColorArray[pRD->BufferedVerts];
	pRD->BufferedVerts += 3;
	for (INT i = 0; i < 3; i++) {
		const FTransTexture* P = *Pts++;

		pTexCoordArray->u = P->U * pRD->TexInfo[0].UMult;
		pTexCoordArray->v = P->V * pRD->TexInfo[0].VMult;
		pTexCoordArray++;

		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		if (pRD->m_requestedColorFlags & CF_FOG_MODE) {
			FLOAT f255_Times_One_Minus_FogW = 255.0f * (1.0f - P->Fog.W);

			pVertexColorArray->color = UD3D8RenderDevice::FPlaneTo_BGRScaled_A255(&P->Light, f255_Times_One_Minus_FogW);

			pSecondaryColorArray->specular = UD3D8RenderDevice::FPlaneTo_BGR_A0(&P->Fog);
			pSecondaryColorArray++;
		}
		else if (pRD->m_requestedColorFlags & CF_COLOR_ARRAY) {
#ifdef UTGLR_RUNE_BUILD
			pVertexColorArray->color = UD3D8RenderDevice::FPlaneTo_BGR_Aub(&P->Light, pRD->m_gpAlpha);
#else
			pVertexColorArray->color = UD3D8RenderDevice::FPlaneTo_BGR_A255(&P->Light);
#endif
		}
		else {
			pVertexColorArray->color = 0xFFFFFFFF;
		}
		pVertexColorArray++;
	}
}

static void FASTCALL Buffer3BasicVerts(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	FGLTexCoord *pTexCoordArray = &pRD->m_pTexCoordArray[0][pRD->BufferedVerts];
	FGLVertexColor *pVertexColorArray = &pRD->m_pVertexColorArray[pRD->BufferedVerts];
	pRD->BufferedVerts += 3;
	FLOAT UMult = pRD->TexInfo[0].UMult;
	FLOAT VMult = pRD->TexInfo[0].VMult;
	for (INT i = 0; i < 3; i++) {
		const FTransTexture* P = *Pts++;

		pTexCoordArray->u = P->U * UMult;
		pTexCoordArray->v = P->V * VMult;
		pTexCoordArray++;

		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		pVertexColorArray->color = 0xFFFFFFFF;
		pVertexColorArray++;
	}
}

static void FASTCALL Buffer3ColoredVerts(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	FGLTexCoord *pTexCoordArray = &pRD->m_pTexCoordArray[0][pRD->BufferedVerts];
	FGLVertexColor *pVertexColorArray = &pRD->m_pVertexColorArray[pRD->BufferedVerts];
	pRD->BufferedVerts += 3;
	for (INT i = 0; i < 3; i++) {
		const FTransTexture* P = *Pts++;

		pTexCoordArray->u = P->U * pRD->TexInfo[0].UMult;
		pTexCoordArray->v = P->V * pRD->TexInfo[0].VMult;
		pTexCoordArray++;

		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		pVertexColorArray->color = UD3D8RenderDevice::FPlaneTo_BGR_A255(&P->Light);
		pVertexColorArray++;
	}
}

#ifdef UTGLR_INCLUDE_SSE_CODE
__declspec(naked) static void FASTCALL Buffer3ColoredVerts_SSE(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	static float f255 = 255.0f;
	__asm {
		//pRD is in ecx
		//Pts is in edx

		push ebx
		push esi
		push edi

		mov eax, [ecx]UD3D8RenderDevice.BufferedVerts

		lea ebx, [eax*8]
		add ebx, [ecx]UD3D8RenderDevice.m_pTexCoordArray[0]

		mov edi, eax
		shl edi, 4
		add edi, [ecx]UD3D8RenderDevice.m_pVertexColorArray

		//BufferedVerts += 3
		add eax, 3
		mov [ecx]UD3D8RenderDevice.BufferedVerts, eax

		lea eax, [ecx]UD3D8RenderDevice.TexInfo
		movss xmm0, [eax]FTexInfo.UMult
		movss xmm1, [eax]FTexInfo.VMult
		movss xmm2, f255

		//Pts in edx
		//Get PtsPlus12B
		lea esi, [edx + 12]

v_loop:
			mov eax, [edx]
			add edx, 4

			movss xmm3, [eax]FTransTexture.U
			mulss xmm3, xmm0
			movss [ebx]FGLTexCoord.u, xmm3
			movss xmm3, [eax]FTransTexture.V
			mulss xmm3, xmm1
			movss [ebx]FGLTexCoord.v, xmm3
			add ebx, TYPE FGLTexCoord

			mov ecx, [eax]FOutVector.Point.X
			mov [edi]FGLVertexColor.x, ecx
			mov ecx, [eax]FOutVector.Point.Y
			mov [edi]FGLVertexColor.y, ecx
			mov ecx, [eax]FOutVector.Point.Z
			mov [edi]FGLVertexColor.z, ecx

			movss xmm3, [eax]FTransSample.Light + 0
			mulss xmm3, xmm2
			movss xmm4, [eax]FTransSample.Light + 4
			mulss xmm4, xmm2
			movss xmm5, [eax]FTransSample.Light + 8
			mulss xmm5, xmm2
			cvtss2si eax, xmm3
			shl eax, 16
			cvtss2si ecx, xmm4
			and ecx, 255
			shl ecx, 8
			or eax, ecx
			cvtss2si ecx, xmm5
			and ecx, 255
			or ecx, 0xFF000000
			or eax, ecx
			mov [edi]FGLVertexColor.color, eax
			add edi, TYPE FGLVertexColor

			cmp edx, esi
			jne v_loop

		pop edi
		pop esi
		pop ebx

		ret
	}
}

__declspec(naked) static void FASTCALL Buffer3ColoredVerts_SSE2(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	static __m128 fColorMul = { 255.0f, 255.0f, 255.0f, 0.0f };
	static DWORD alphaOr = 0xFF000000;
	__asm {
		//pRD is in ecx
		//Pts is in edx

		push ebx
		push esi
		push edi

		mov eax, [ecx]UD3D8RenderDevice.BufferedVerts

		lea ebx, [eax*8]
		add ebx, [ecx]UD3D8RenderDevice.m_pTexCoordArray[0]

		mov edi, eax
		shl edi, 4
		add edi, [ecx]UD3D8RenderDevice.m_pVertexColorArray

		//BufferedVerts += 3
		add eax, 3
		mov [ecx]UD3D8RenderDevice.BufferedVerts, eax

		lea eax, [ecx]UD3D8RenderDevice.TexInfo
		movss xmm0, [eax]FTexInfo.UMult
		movss xmm1, [eax]FTexInfo.VMult
		movaps xmm2, fColorMul
		movd xmm3, alphaOr

		//Pts in edx
		//Get PtsPlus12B
		lea esi, [edx + 12]

v_loop:
			mov eax, [edx]
			add edx, 4

			movss xmm4, [eax]FTransTexture.U
			mulss xmm4, xmm0
			movss [ebx]FGLTexCoord.u, xmm4
			movss xmm4, [eax]FTransTexture.V
			mulss xmm4, xmm1
			movss [ebx]FGLTexCoord.v, xmm4
			add ebx, TYPE FGLTexCoord

			mov ecx, [eax]FOutVector.Point.X
			mov [edi]FGLVertexColor.x, ecx
			mov ecx, [eax]FOutVector.Point.Y
			mov [edi]FGLVertexColor.y, ecx
			mov ecx, [eax]FOutVector.Point.Z
			mov [edi]FGLVertexColor.z, ecx

			movups xmm4, [eax]FTransSample.Light
			shufps xmm4, xmm4, 0xC6
			mulps xmm4, xmm2
			cvtps2dq xmm4, xmm4
			packssdw xmm4, xmm4
			packuswb xmm4, xmm4
			por xmm4, xmm3
			movd [edi]FGLVertexColor.color, xmm4
			add edi, TYPE FGLVertexColor

			cmp edx, esi
			jne v_loop

		pop edi
		pop esi
		pop ebx

		ret
	}
}
#endif

static void FASTCALL Buffer3FoggedVerts(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	FGLTexCoord *pTexCoordArray = &pRD->m_pTexCoordArray[0][pRD->BufferedVerts];
	FGLVertexColor *pVertexColorArray = &pRD->m_pVertexColorArray[pRD->BufferedVerts];
	FGLSecondaryColor *pSecondaryColorArray = &pRD->m_pSecondaryColorArray[pRD->BufferedVerts];
	pRD->BufferedVerts += 3;
	for (INT i = 0; i < 3; i++) {
		const FTransTexture* P = *Pts++;

		pTexCoordArray->u = P->U * pRD->TexInfo[0].UMult;
		pTexCoordArray->v = P->V * pRD->TexInfo[0].VMult;
		pTexCoordArray++;

		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		FLOAT f255_Times_One_Minus_FogW = 255.0f * (1.0f - P->Fog.W);
		pVertexColorArray->color = UD3D8RenderDevice::FPlaneTo_BGRScaled_A255(&P->Light, f255_Times_One_Minus_FogW);
		pVertexColorArray++;

		pSecondaryColorArray->specular = UD3D8RenderDevice::FPlaneTo_BGR_A0(&P->Fog);
		pSecondaryColorArray++;
	}
}

#ifdef UTGLR_INCLUDE_SSE_CODE
__declspec(naked) static void FASTCALL Buffer3FoggedVerts_SSE(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	static float f255 = 255.0f;
	static float f1 = 1.0f;
	__asm {
		//pRD is in ecx
		//Pts is in edx

		push ebx
		push esi
		push edi
		push ebp
		sub esp, 4

		mov eax, [ecx]UD3D8RenderDevice.BufferedVerts

		lea ebx, [eax*8]
		add ebx, [ecx]UD3D8RenderDevice.m_pTexCoordArray[0]

		mov edi, eax
		shl edi, 4
		add edi, [ecx]UD3D8RenderDevice.m_pVertexColorArray

		lea esi, [eax*4]
		add esi, [ecx]UD3D8RenderDevice.m_pSecondaryColorArray

		//BufferedVerts += 3
		add eax, 3
		mov [ecx]UD3D8RenderDevice.BufferedVerts, eax

		lea eax, [ecx]UD3D8RenderDevice.TexInfo
		movss xmm0, [eax]FTexInfo.UMult
		movss xmm1, [eax]FTexInfo.VMult
		movss xmm2, f255

		//Pts in edx
		//Get PtsPlus12B
		lea ebp, [edx + 12]

v_loop:
			mov eax, [edx]
			add edx, 4

			movss xmm3, [eax]FTransTexture.U
			mulss xmm3, xmm0
			movss [ebx]FGLTexCoord.u, xmm3
			movss xmm3, [eax]FTransTexture.V
			mulss xmm3, xmm1
			movss [ebx]FGLTexCoord.v, xmm3
			add ebx, TYPE FGLTexCoord

			mov [esp], ebx

			movss xmm6, f1
			subss xmm6, [eax]FTransSample.Fog + 12
			mulss xmm6, xmm2

			mov ecx, [eax]FOutVector.Point.X
			mov [edi]FGLVertexColor.x, ecx
			mov ecx, [eax]FOutVector.Point.Y
			mov [edi]FGLVertexColor.y, ecx
			mov ecx, [eax]FOutVector.Point.Z
			mov [edi]FGLVertexColor.z, ecx

			movss xmm3, [eax]FTransSample.Light + 0
			mulss xmm3, xmm6
			movss xmm4, [eax]FTransSample.Light + 4
			mulss xmm4, xmm6
			movss xmm5, [eax]FTransSample.Light + 8
			mulss xmm5, xmm6
			cvtss2si ebx, xmm3
			shl ebx, 16
			cvtss2si ecx, xmm4
			and ecx, 255
			shl ecx, 8
			or ebx, ecx
			cvtss2si ecx, xmm5
			and ecx, 255
			or ecx, 0xFF000000
			or ebx, ecx
			mov [edi]FGLVertexColor.color, ebx
			add edi, TYPE FGLVertexColor

			mov ebx, [esp]

			movss xmm3, [eax]FTransSample.Fog + 0
			mulss xmm3, xmm2
			movss xmm4, [eax]FTransSample.Fog + 4
			mulss xmm4, xmm2
			movss xmm5, [eax]FTransSample.Fog + 8
			mulss xmm5, xmm2
			cvtss2si eax, xmm3
			and eax, 255
			shl eax, 16
			cvtss2si ecx, xmm4
			and ecx, 255
			shl ecx, 8
			or eax, ecx
			cvtss2si ecx, xmm5
			and ecx, 255
			or eax, ecx
			mov [esi]FGLSecondaryColor.specular, eax
			add esi, TYPE FGLSecondaryColor

			cmp edx, ebp
			jne v_loop

		add esp, 4
		pop ebp
		pop edi
		pop esi
		pop ebx

		ret
	}
}

__declspec(naked) static void FASTCALL Buffer3FoggedVerts_SSE2(UD3D8RenderDevice *pRD, FTransTexture** Pts) {
	static __m128 fColorMul = { 255.0f, 255.0f, 255.0f, 0.0f };
	static DWORD alphaOr = 0xFF000000;
	static float f1 = 1.0f;
	__asm {
		//pRD is in ecx
		//Pts is in edx

		push ebx
		push esi
		push edi
		push ebp

		mov eax, [ecx]UD3D8RenderDevice.BufferedVerts

		lea ebx, [eax*8]
		add ebx, [ecx]UD3D8RenderDevice.m_pTexCoordArray[0]

		mov edi, eax
		shl edi, 4
		add edi, [ecx]UD3D8RenderDevice.m_pVertexColorArray

		lea esi, [eax*4]
		add esi, [ecx]UD3D8RenderDevice.m_pSecondaryColorArray

		//BufferedVerts += 3
		add eax, 3
		mov [ecx]UD3D8RenderDevice.BufferedVerts, eax

		lea eax, [ecx]UD3D8RenderDevice.TexInfo
		movss xmm0, [eax]FTexInfo.UMult
		movss xmm1, [eax]FTexInfo.VMult
		movaps xmm2, fColorMul
		movd xmm3, alphaOr

		//Pts in edx
		//Get PtsPlus12B
		lea ebp, [edx + 12]

v_loop:
			mov eax, [edx]
			add edx, 4

			movss xmm4, [eax]FTransTexture.U
			mulss xmm4, xmm0
			movss [ebx]FGLTexCoord.u, xmm4
			movss xmm4, [eax]FTransTexture.V
			mulss xmm4, xmm1
			movss [ebx]FGLTexCoord.v, xmm4
			add ebx, TYPE FGLTexCoord

			movss xmm6, f1
			subss xmm6, [eax]FTransSample.Fog + 12
			mulss xmm6, xmm2
			shufps xmm6, xmm6, 0x00

			mov ecx, [eax]FOutVector.Point.X
			mov [edi]FGLVertexColor.x, ecx
			mov ecx, [eax]FOutVector.Point.Y
			mov [edi]FGLVertexColor.y, ecx
			mov ecx, [eax]FOutVector.Point.Z
			mov [edi]FGLVertexColor.z, ecx

			movups xmm4, [eax]FTransSample.Light
			shufps xmm4, xmm4, 0xC6
			mulps xmm4, xmm6
			cvtps2dq xmm4, xmm4
			packssdw xmm4, xmm4
			packuswb xmm4, xmm4
			por xmm4, xmm3
			movd [edi]FGLVertexColor.color, xmm4
			add edi, TYPE FGLVertexColor

			movups xmm4, [eax]FTransSample.Fog
			shufps xmm4, xmm4, 0xC6
			mulps xmm4, xmm2
			cvtps2dq xmm4, xmm4
			packssdw xmm4, xmm4
			packuswb xmm4, xmm4
			movd [esi]FGLSecondaryColor.specular, xmm4
			add esi, TYPE FGLSecondaryColor

			cmp edx, ebp
			jne v_loop

		pop ebp
		pop edi
		pop esi
		pop ebx

		ret
	}
}
#endif


//Must be called with (NumPts > 3)
void UD3D8RenderDevice::BufferAdditionalClippedVerts(FTransTexture** Pts, INT NumPts) {
	INT i;

	i = 3;
	do {
		const FTransTexture* P;
		FGLTexCoord *pTexCoordArray;
		FGLVertexColor *pVertexColorArray;

		P = Pts[0];
		pTexCoordArray = &m_pTexCoordArray[0][BufferedVerts];
		pTexCoordArray->u = P->U * TexInfo[0].UMult;
		pTexCoordArray->v = P->V * TexInfo[0].VMult;
		pVertexColorArray = &m_pVertexColorArray[BufferedVerts];
		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		if (m_requestedColorFlags & CF_FOG_MODE) {
			FLOAT f255_Times_One_Minus_FogW = 255.0f * (1.0f - P->Fog.W);
			pVertexColorArray->color = FPlaneTo_BGRScaled_A255(&P->Light, f255_Times_One_Minus_FogW);

			m_pSecondaryColorArray[BufferedVerts].specular = FPlaneTo_BGR_A0(&P->Fog);
		}
		else if (m_requestedColorFlags & CF_COLOR_ARRAY) {
#ifdef UTGLR_RUNE_BUILD
			pVertexColorArray->color = FPlaneTo_BGR_Aub(&P->Light, m_gpAlpha);
#else
			pVertexColorArray->color = FPlaneTo_BGR_A255(&P->Light);
#endif
		}
		else {
			pVertexColorArray->color = 0xFFFFFFFF;
		}
		BufferedVerts++;

		P = Pts[i - 1];
		pTexCoordArray = &m_pTexCoordArray[0][BufferedVerts];
		pTexCoordArray->u = P->U * TexInfo[0].UMult;
		pTexCoordArray->v = P->V * TexInfo[0].VMult;
		pVertexColorArray = &m_pVertexColorArray[BufferedVerts];
		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		if (m_requestedColorFlags & CF_FOG_MODE) {
			FLOAT f255_Times_One_Minus_FogW = 255.0f * (1.0f - P->Fog.W);
			pVertexColorArray->color = FPlaneTo_BGRScaled_A255(&P->Light, f255_Times_One_Minus_FogW);

			m_pSecondaryColorArray[BufferedVerts].specular = FPlaneTo_BGR_A0(&P->Fog);
		}
		else if (m_requestedColorFlags & CF_COLOR_ARRAY) {
#ifdef UTGLR_RUNE_BUILD
			pVertexColorArray->color = FPlaneTo_BGR_Aub(&P->Light, m_gpAlpha);
#else
			pVertexColorArray->color = FPlaneTo_BGR_A255(&P->Light);
#endif
		}
		else {
			pVertexColorArray->color = 0xFFFFFFFF;
		}
		BufferedVerts++;

		P = Pts[i];
		pTexCoordArray = &m_pTexCoordArray[0][BufferedVerts];
		pTexCoordArray->u = P->U * TexInfo[0].UMult;
		pTexCoordArray->v = P->V * TexInfo[0].VMult;
		pVertexColorArray = &m_pVertexColorArray[BufferedVerts];
		pVertexColorArray->x = P->Point.X;
		pVertexColorArray->y = P->Point.Y;
		pVertexColorArray->z = P->Point.Z;
		if (m_requestedColorFlags & CF_FOG_MODE) {
			FLOAT f255_Times_One_Minus_FogW = 255.0f * (1.0f - P->Fog.W);
			pVertexColorArray->color = FPlaneTo_BGRScaled_A255(&P->Light, f255_Times_One_Minus_FogW);

			m_pSecondaryColorArray[BufferedVerts].specular = FPlaneTo_BGR_A0(&P->Fog);
		}
		else if (m_requestedColorFlags & CF_COLOR_ARRAY) {
#ifdef UTGLR_RUNE_BUILD
			pVertexColorArray->color = FPlaneTo_BGR_Aub(&P->Light, m_gpAlpha);
#else
			pVertexColorArray->color = FPlaneTo_BGR_A255(&P->Light);
#endif
		}
		else {
			pVertexColorArray->color = 0xFFFFFFFF;
		}
		BufferedVerts++;
	} while (++i < NumPts);

	return;
}


void UD3D8RenderDevice::BuildGammaRamp(float redGamma, float greenGamma, float blueGamma, int brightness, D3DGAMMARAMP &ramp) {
	unsigned int u;

	//Parameter clamping
	if (brightness < -50) brightness = -50;
	if (brightness > 50) brightness = 50;

	float oneOverRedGamma = 1.0f / (2.5f * redGamma);
	float oneOverGreenGamma = 1.0f / (2.5f * greenGamma);
	float oneOverBlueGamma = 1.0f / (2.5f * blueGamma);
	for (u = 0; u < 256; u++) {
		int iVal;
		int iValRed, iValGreen, iValBlue;

		//Initial value
		iVal = u;

		//Brightness
		iVal += brightness;
		//Clamping
		if (iVal < 0) iVal = 0;
		if (iVal > 255) iVal = 255;

		//Gamma
		iValRed = (int)appRound((float)appPow(iVal / 255.0f, oneOverRedGamma) * 65535.0f);
		iValGreen = (int)appRound((float)appPow(iVal / 255.0f, oneOverGreenGamma) * 65535.0f);
		iValBlue = (int)appRound((float)appPow(iVal / 255.0f, oneOverBlueGamma) * 65535.0f);

		//Save results
		ramp.red[u] = (_WORD)iValRed;
		ramp.green[u] = (_WORD)iValGreen;
		ramp.blue[u] = (_WORD)iValBlue;
	}

	return;
}

void UD3D8RenderDevice::BuildGammaRamp(float redGamma, float greenGamma, float blueGamma, int brightness, FByteGammaRamp &ramp) {
	unsigned int u;

	//Parameter clamping
	if (brightness < -50) brightness = -50;
	if (brightness > 50) brightness = 50;

	float oneOverRedGamma = 1.0f / (2.5f * redGamma);
	float oneOverGreenGamma = 1.0f / (2.5f * greenGamma);
	float oneOverBlueGamma = 1.0f / (2.5f * blueGamma);
	for (u = 0; u < 256; u++) {
		int iVal;
		int iValRed, iValGreen, iValBlue;

		//Initial value
		iVal = u;

		//Brightness
		iVal += brightness;
		//Clamping
		if (iVal < 0) iVal = 0;
		if (iVal > 255) iVal = 255;

		//Gamma
		iValRed = (int)appRound((float)appPow(iVal / 255.0f, oneOverRedGamma) * 255.0f);
		iValGreen = (int)appRound((float)appPow(iVal / 255.0f, oneOverGreenGamma) * 255.0f);
		iValBlue = (int)appRound((float)appPow(iVal / 255.0f, oneOverBlueGamma) * 255.0f);

		//Save results
		ramp.red[u] = (BYTE)iValRed;
		ramp.green[u] = (BYTE)iValGreen;
		ramp.blue[u] = (BYTE)iValBlue;
	}

	return;
}

void UD3D8RenderDevice::SetGamma(FLOAT GammaCorrection) {
//	FGammaRamp gammaRamp;
	D3DGAMMARAMP gammaRamp;

	GammaCorrection += GammaOffset;

	//Do not attempt to set gamma if <= zero
	if (GammaCorrection <= 0.0f) {
		return;
	}

	BuildGammaRamp(GammaCorrection + GammaOffsetRed, GammaCorrection + GammaOffsetGreen, GammaCorrection + GammaOffsetBlue, Brightness, gammaRamp);

/*	if (g_gammaFirstTime) {
		if (GetDeviceGammaRamp(m_hDC, &g_originalGammaRamp)) {
			g_haveOriginalGammaRamp = true;
		}
		g_gammaFirstTime = false;
	}*/

	m_setGammaRampSucceeded = false;
//	if (SetDeviceGammaRamp(m_hDC, &gammaRamp)) {
	m_d3dDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION, &gammaRamp);
	if (1) {
		m_setGammaRampSucceeded = true;
		SavedGammaCorrection = GammaCorrection;
	}

	return;
}

void UD3D8RenderDevice::ResetGamma(void) {
/*	//Restore gamma ramp if original was successfully saved
	if (g_haveOriginalGammaRamp) {
		HWND hDesktopWnd;
		HDC hDC;

		hDesktopWnd = GetDesktopWindow();
		hDC = GetDC(hDesktopWnd);

		// vogel: grrr, UClient::destroy is called before this gets called so hDC is invalid
		SetDeviceGammaRamp(hDC, &g_originalGammaRamp);

		ReleaseDC(hDesktopWnd, hDC);
	}*/

	return;
}



UBOOL UD3D8RenderDevice::FailedInitf(const TCHAR* Fmt, ...) {
	TCHAR TempStr[4096];
	GET_VARARGS(TempStr, ARRAY_COUNT(TempStr), Fmt);
	debugf(NAME_Init, TempStr);
	Exit();
	return 0;
}

void UD3D8RenderDevice::Exit() {
	guard(UD3D8RenderDevice::Exit);
	check(NumDevices > 0);

	//Shutdown D3D
	if (m_d3d8) {
		UnsetRes();
	}

	//Reset gamma ramp
	ResetGamma();

	//Shut down global D3D
	if (--NumDevices == 0) {
#if 0
		//Free modules
		if (hModuleD3d8) {
			verify(FreeLibrary(hModuleD3d8));
			hModuleD3d8 = NULL;
		}
#endif
	}

	unguard;
}

void UD3D8RenderDevice::ShutdownAfterError() {
	guard(UD3D8RenderDevice::ShutdownAfterError);

	debugf(NAME_Exit, TEXT("UD3D8RenderDevice::ShutdownAfterError"));

	if (DebugBit(DEBUG_BIT_BASIC)) {
		dout << TEXT("utd3d8r: ShutdownAfterError") << std::endl;
	}

	//ChangeDisplaySettings(NULL, 0);

	//Reset gamma ramp
	ResetGamma();

	unguard;
}


UBOOL UD3D8RenderDevice::SetRes(INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen) {
	guard(UD3D8RenderDevice::SetRes);

	HRESULT hResult;
	bool saved_SetRes_isDeviceReset;

	//Get debug bits
	{
		INT i = 0;
		if (!GConfig->GetInt(g_pSection, TEXT("DebugBits"), i)) i = 0;
		m_debugBits = i;
	}
	//Display debug bits
	if (DebugBit(DEBUG_BIT_ANY)) dout << TEXT("utd3d8r: DebugBits = ") << m_debugBits << std::endl;


	debugfSlow(TEXT("Enter SetRes()"));

	//Save parameters in case need to reset device
	m_SetRes_NewX = NewX;
	m_SetRes_NewY = NewY;
	m_SetRes_NewColorBytes = NewColorBytes;
	m_SetRes_Fullscreen = Fullscreen;

	//Save copy of SetRes is device reset flag
	saved_SetRes_isDeviceReset = m_SetRes_isDeviceReset;
	//Reset SetRes is device reset flag
	m_SetRes_isDeviceReset = false;

	// If not fullscreen, and color bytes hasn't changed, do nothing.
	//If SetRes called due to device reset, do full destroy/recreate
	if (m_d3dDevice && !saved_SetRes_isDeviceReset && !Fullscreen && !WasFullscreen && (NewColorBytes == Viewport->ColorBytes)) {
		//Resize viewport
		if (!Viewport->ResizeViewport(BLIT_HardwarePaint | BLIT_Direct3D, NewX, NewY, NewColorBytes)) {
			return 0;
		}

		//Free old resources if they exist
		FreePermanentResources();

		//Get real viewport size
		NewX = Viewport->SizeX;
		NewY = Viewport->SizeY;

		//Don't break editor and tiny windowed mode
		if (NewX < 16) NewX = 16;
		if (NewY < 16) NewY = 16;

		//Set new size
		m_d3dpp.BackBufferWidth = NewX;
		m_d3dpp.BackBufferHeight = NewY;

		//Reset device
		hResult = m_d3dDevice->Reset(&m_d3dpp);
		if (FAILED(hResult)) {
			appErrorf(TEXT("Failed to create D3D device for new window size"));
		}

		//Initialize permanent rendering state, including allocation of some resources
		InitPermanentResourcesAndRenderingState();

		//Set viewport
		D3DVIEWPORT8 d3dViewport;
		d3dViewport.X = 0;
		d3dViewport.Y = 0;
		d3dViewport.Width = NewX;
		d3dViewport.Height = NewY;
		d3dViewport.MinZ = 0.0f;
		d3dViewport.MaxZ = 1.0f;
		m_d3dDevice->SetViewport(&d3dViewport);

		return 1;
	}


	// Exit res.
	if (m_d3d8) {
		debugf(TEXT("UnSetRes() -> m_d3d8 != NULL"));
		UnsetRes();
	}

	//Search for closest resolution match if fullscreen requested
	//No longer changing resolution here
	if (Fullscreen) {
		INT FindX = NewX, FindY = NewY, BestError = MAXINT;
		for (INT i = 0; i < Modes.Num(); i++) {
			if (Modes(i).Z==NewColorBytes*8) {
				INT Error
				=	(Modes(i).X-FindX)*(Modes(i).X-FindX)
				+	(Modes(i).Y-FindY)*(Modes(i).Y-FindY);
				if (Error < BestError) {
					NewX      = Modes(i).X;
					NewY      = Modes(i).Y;
					BestError = Error;
				}
			}
		}
	}

	// Change window size.
	UBOOL Result = Viewport->ResizeViewport(Fullscreen ? (BLIT_Fullscreen | BLIT_Direct3D) : (BLIT_HardwarePaint | BLIT_Direct3D), NewX, NewY, NewColorBytes);
	if (!Result) {
		return 0;
	}


	//Create main D3D8 object
	m_d3d8 = pDirect3DCreate8(D3D_SDK_VERSION);
	if (!m_d3d8) {
		appErrorf(TEXT("Direct3DCreate8 failed"));
	}


	//Get D3D caps
	hResult = m_d3d8->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_d3dCaps);
	if (FAILED(hResult)) {
		appErrorf(TEXT("GetDeviceCaps failed"));
	}


	//Create D3D device

	//Get current display mode
	D3DDISPLAYMODE d3ddm;
	hResult = m_d3d8->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hResult)) {
		appErrorf(TEXT("Failed to get current display mode"));
	}

	//Check if SetRes device reset
	//If so, get current bit depth
	//But don't check if was fullscreen
	if (saved_SetRes_isDeviceReset && !Fullscreen) {
		switch (d3ddm.Format) {
		case D3DFMT_R5G6B5: NewColorBytes = 2; break;
		case D3DFMT_X1R5G5B5: NewColorBytes = 2; break;
		case D3DFMT_A1R5G5B5: NewColorBytes = 2; break;
		default:
			NewColorBytes = 4;
		}
	}
	//Update saved NewColorBytes
	m_SetRes_NewColorBytes = NewColorBytes;

	//Don't break editor and tiny windowed mode
	if (NewX < 16) NewX = 16;
	if (NewY < 16) NewY = 16;

	//Set presentation parameters
	appMemzero(&m_d3dpp, sizeof(m_d3dpp));
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferWidth = NewX;
	m_d3dpp.BackBufferHeight = NewY;
	m_d3dpp.BackBufferFormat = d3ddm.Format;
	m_d3dpp.EnableAutoDepthStencil = TRUE;

	//Check if should be full screen
	if (Fullscreen) {
		m_d3dpp.Windowed = FALSE;
		m_d3dpp.BackBufferFormat = (NewColorBytes <= 2) ? D3DFMT_R5G6B5 : D3DFMT_X8R8G8B8;
	}

	//Choose initial depth buffer format
	if ((NewColorBytes <= 2) && !RequestHighResolutionZ) {
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		m_numDepthBits = 16;
	}
	else {
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
		m_numDepthBits = 32;
	}

	//Reduce depth buffer format if necessary based on what's supported
	if (m_d3dpp.AutoDepthStencilFormat == D3DFMT_D32) {
		if (!CheckDepthFormat(d3ddm.Format, m_d3dpp.BackBufferFormat, D3DFMT_D32)) {
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
			m_numDepthBits = 24;
		}
	}
	if (m_d3dpp.AutoDepthStencilFormat == D3DFMT_D24X8) {
		if (!CheckDepthFormat(d3ddm.Format, m_d3dpp.BackBufferFormat, D3DFMT_D24X8)) {
			m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			m_numDepthBits = 16;
		}
	}

	m_usingAA = false;
	m_curAAEnable = true;
	m_defAAEnable = true;

	//Select AA mode
	if (UseAA) {
		D3DMULTISAMPLE_TYPE MultiSampleType;

		switch (NumAASamples) {
		case  0: MultiSampleType = D3DMULTISAMPLE_NONE; break;
		case  1: MultiSampleType = D3DMULTISAMPLE_NONE; break;
		case  2: MultiSampleType = D3DMULTISAMPLE_2_SAMPLES; break;
		case  3: MultiSampleType = D3DMULTISAMPLE_3_SAMPLES; break;
		case  4: MultiSampleType = D3DMULTISAMPLE_4_SAMPLES; break;
		case  5: MultiSampleType = D3DMULTISAMPLE_5_SAMPLES; break;
		case  6: MultiSampleType = D3DMULTISAMPLE_6_SAMPLES; break;
		case  7: MultiSampleType = D3DMULTISAMPLE_7_SAMPLES; break;
		case  8: MultiSampleType = D3DMULTISAMPLE_8_SAMPLES; break;
		case  9: MultiSampleType = D3DMULTISAMPLE_9_SAMPLES; break;
		case 10: MultiSampleType = D3DMULTISAMPLE_10_SAMPLES; break;
		case 11: MultiSampleType = D3DMULTISAMPLE_11_SAMPLES; break;
		case 12: MultiSampleType = D3DMULTISAMPLE_12_SAMPLES; break;
		case 13: MultiSampleType = D3DMULTISAMPLE_13_SAMPLES; break;
		case 14: MultiSampleType = D3DMULTISAMPLE_14_SAMPLES; break;
		case 15: MultiSampleType = D3DMULTISAMPLE_15_SAMPLES; break;
		case 16: MultiSampleType = D3DMULTISAMPLE_16_SAMPLES; break;
		default:
			MultiSampleType = D3DMULTISAMPLE_NONE;
		}
		m_d3dpp.MultiSampleType = MultiSampleType;

		hResult = m_d3d8->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dpp.BackBufferFormat, m_d3dpp.Windowed, m_d3dpp.MultiSampleType);
		if (FAILED(hResult)) {
			m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		}
		hResult = m_d3d8->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_d3dpp.AutoDepthStencilFormat, m_d3dpp.Windowed, m_d3dpp.MultiSampleType);
		if (FAILED(hResult)) {
			m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		}

		if (m_d3dpp.MultiSampleType != D3DMULTISAMPLE_NONE) {
			m_usingAA = true;
		}
	}

	//Set swap interval
	if (UseVSync >= 0) {
		//D3D8 only allows swap interval support if full screen
		if (!m_d3dpp.Windowed) {
			switch (UseVSync) {
			case 0:
				if (m_d3dCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) {
					m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
				}
				break;
			case 1:
				if (m_d3dCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE) {
					m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				}
				break;
			default:
				;
			}
		}
	}


	//Set increased back buffer count if using triple buffering
	if (UseTripleBuffering) {
		m_d3dpp.BackBufferCount = 2;
	}

	//Set initial HW/SW vertex processing preference
	m_doSoftwareVertexInit = false;
	if (UseSoftwareVertexProcessing) {
		m_doSoftwareVertexInit = true;
	}

	//Try HW vertex init if not forcing SW vertex init
	if (!m_doSoftwareVertexInit) {
		bool tryDefaultRefreshRate = true;
		DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		//Check if should use pure device
		if (UsePureDevice && (m_d3dCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)) {
			behaviorFlags |= D3DCREATE_PUREDEVICE;
		}

		//Possibly attempt to set refresh rate if fullscreen
		if (!m_d3dpp.Windowed && (RefreshRate > 0)) {
			//Attempt to create with specific refresh rate
			m_d3dpp.FullScreen_RefreshRateInHz = RefreshRate;
			hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			//Try again if triple buffering failed
			if (FAILED(hResult) && UseTripleBuffering && (m_d3dpp.BackBufferCount != 2)) {
				hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			}
			if (FAILED(hResult)) {
			}
			else {
				tryDefaultRefreshRate = false;
			}
		}

		if (tryDefaultRefreshRate) {
			//Attempt to create with default refresh rate
			m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			//Try again if triple buffering failed
			if (FAILED(hResult) && UseTripleBuffering && (m_d3dpp.BackBufferCount != 2)) {
				hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			}
			if (FAILED(hResult)) {
				debugf(NAME_Init, TEXT("Failed to create D3D device with hardware vertex processing"));
				m_doSoftwareVertexInit = true;
			}
		}
	}
	//Try SW vertex init if forced earlier or if HW vertex init failed
	if (m_doSoftwareVertexInit) {
		bool tryDefaultRefreshRate = true;
		DWORD behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		//Possibly attempt to set refresh rate if fullscreen
		if (!m_d3dpp.Windowed && (RefreshRate > 0)) {
			//Attempt to create with specific refresh rate
			m_d3dpp.FullScreen_RefreshRateInHz = RefreshRate;
			hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			//Try again if triple buffering failed
			if (FAILED(hResult) && UseTripleBuffering && (m_d3dpp.BackBufferCount != 2)) {
				hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			}
			if (FAILED(hResult)) {
			}
			else {
				tryDefaultRefreshRate = false;
			}
		}

		if (tryDefaultRefreshRate) {
			//Attempt to create with default refresh rate
			m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
			hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			//Try again if triple buffering failed
			if (FAILED(hResult) && UseTripleBuffering && (m_d3dpp.BackBufferCount != 2)) {
				hResult = m_d3d8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, behaviorFlags, &m_d3dpp, &m_d3dDevice);
			}
			if (FAILED(hResult)) {
				appErrorf(TEXT("Failed to create D3D device"));
			}
		}
	}


	//Reset previous SwapBuffers status to okay
	m_prevSwapBuffersStatus = true;

	//Display depth buffer bit depth
	debugf(NAME_Init, TEXT("Depth bits: %u"), m_numDepthBits);

	//Get other defaults
	if (!GConfig->GetInt(g_pSection, TEXT("Brightness"), Brightness)) Brightness = 0;

	//Debug parameter listing
	if (DebugBit(DEBUG_BIT_BASIC)) {
		#define UTGLR_DEBUG_SHOW_PARAM_REG(_name) DbgPrintInitParam(TEXT(#_name), _name)
		#define UTGLR_DEBUG_SHOW_PARAM_DCV(_name) DbgPrintInitParam(TEXT(#_name), DCV._name)

		UTGLR_DEBUG_SHOW_PARAM_REG(LODBias);
		UTGLR_DEBUG_SHOW_PARAM_REG(GammaOffset);
		UTGLR_DEBUG_SHOW_PARAM_REG(GammaOffsetRed);
		UTGLR_DEBUG_SHOW_PARAM_REG(GammaOffsetGreen);
		UTGLR_DEBUG_SHOW_PARAM_REG(GammaOffsetBlue);
		UTGLR_DEBUG_SHOW_PARAM_REG(Brightness);
		UTGLR_DEBUG_SHOW_PARAM_REG(GammaCorrectScreenshots);
		UTGLR_DEBUG_SHOW_PARAM_REG(OneXBlending);
		UTGLR_DEBUG_SHOW_PARAM_REG(MaxLogUOverV);
		UTGLR_DEBUG_SHOW_PARAM_REG(MaxLogVOverU);
		UTGLR_DEBUG_SHOW_PARAM_REG(MinLogTextureSize);
		UTGLR_DEBUG_SHOW_PARAM_REG(MaxLogTextureSize);
		UTGLR_DEBUG_SHOW_PARAM_REG(MaxAnisotropy);
		UTGLR_DEBUG_SHOW_PARAM_REG(TMUnits);
		UTGLR_DEBUG_SHOW_PARAM_REG(MaxTMUnits);
		UTGLR_DEBUG_SHOW_PARAM_REG(RefreshRate);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseMultiTexture);
		UTGLR_DEBUG_SHOW_PARAM_REG(UsePalette);
		UTGLR_DEBUG_SHOW_PARAM_REG(UsePrecache);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseTrilinear);
//		UTGLR_DEBUG_SHOW_PARAM_REG(UseVertexSpecular);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseAlphaPalette);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseS3TC);
		UTGLR_DEBUG_SHOW_PARAM_REG(Use16BitTextures);
		UTGLR_DEBUG_SHOW_PARAM_REG(Use565Textures);
		UTGLR_DEBUG_SHOW_PARAM_REG(NoFiltering);
		UTGLR_DEBUG_SHOW_PARAM_REG(DetailMax);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseDetailAlpha);
		UTGLR_DEBUG_SHOW_PARAM_REG(DetailClipping);
		UTGLR_DEBUG_SHOW_PARAM_REG(ColorizeDetailTextures);
		UTGLR_DEBUG_SHOW_PARAM_REG(SinglePassFog);
		UTGLR_DEBUG_SHOW_PARAM_DCV(SinglePassDetail);
//		UTGLR_DEBUG_SHOW_PARAM_REG(BufferActorTris);
		UTGLR_DEBUG_SHOW_PARAM_REG(BufferClippedActorTris);
		UTGLR_DEBUG_SHOW_PARAM_REG(BufferTileQuads);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseSSE);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseSSE2);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseTexIdPool);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseTexPool);
		UTGLR_DEBUG_SHOW_PARAM_REG(CacheStaticMaps);
		UTGLR_DEBUG_SHOW_PARAM_REG(DynamicTexIdRecycleLevel);
		UTGLR_DEBUG_SHOW_PARAM_REG(TexDXT1ToDXT3);
		UTGLR_DEBUG_SHOW_PARAM_DCV(UseVertexProgram);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseVSync);
		UTGLR_DEBUG_SHOW_PARAM_REG(FrameRateLimit);
		UTGLR_DEBUG_SHOW_PARAM_REG(SceneNodeHack);
		UTGLR_DEBUG_SHOW_PARAM_REG(SmoothMaskedTextures);
		UTGLR_DEBUG_SHOW_PARAM_REG(MaskedTextureHack);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseTripleBuffering);
		UTGLR_DEBUG_SHOW_PARAM_REG(UsePureDevice);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseSoftwareVertexProcessing);
		UTGLR_DEBUG_SHOW_PARAM_REG(RequestHighResolutionZ);
		UTGLR_DEBUG_SHOW_PARAM_REG(UseAA);
		UTGLR_DEBUG_SHOW_PARAM_REG(NumAASamples);
		UTGLR_DEBUG_SHOW_PARAM_REG(NoAATiles);
		UTGLR_DEBUG_SHOW_PARAM_REG(ZRangeHack);

		#undef UTGLR_DEBUG_SHOW_PARAM_REG
		#undef UTGLR_DEBUG_SHOW_PARAM_DCV
	}


#ifdef UTGLR_INCLUDE_SSE_CODE
	if (UseSSE) {
		if (!CPU_DetectSSE()) {
			UseSSE = 0;
		}
	}
	if (UseSSE2) {
		if (!CPU_DetectSSE2()) {
			UseSSE2 = 0;
		}
	}
#else
	UseSSE = 0;
	UseSSE2 = 0;
#endif
	if (DebugBit(DEBUG_BIT_BASIC)) dout << TEXT("utd3d8r: UseSSE = ") << UseSSE << std::endl;
	if (DebugBit(DEBUG_BIT_BASIC)) dout << TEXT("utd3d8r: UseSSE2 = ") << UseSSE2 << std::endl;

	SetGamma(Viewport->GetOuterUClient()->Brightness);

	//Restrict dynamic tex id recycle level range
	if (DynamicTexIdRecycleLevel < 10) DynamicTexIdRecycleLevel = 10;

	//Always use vertex specular unless caps check fails later
	UseVertexSpecular = 1;

	DecompFormat = TEXF_RGBA8_;
	SupportsNewBTC = FALSE;
	SupportsTC = UseS3TC;
	SupportsAlphaBlend = 1;
	SupportsHDLightmaps = 0;

	//Set paletted texture capability flag
	m_palettedTextureCap = false;

	//Set DXT texture capability flags
	//Check for DXT1 support
	m_dxt1TextureCap = true;
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT1);
	if (FAILED(hResult)) {
		m_dxt1TextureCap = false;
	}
	//Check for DXT3 support
	m_dxt3TextureCap = true;
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT3);
	if (FAILED(hResult)) {
		m_dxt3TextureCap = false;
	}

	//Check for DXT5 support
	m_dxt5TextureCap = true;
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT5);
	if (FAILED(hResult)) {
		m_dxt5TextureCap = false;
	}

	//Set 16-bit texture capability flag
	m_16BitTextureCap = true;
	//Check RGB565
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_R5G6B5);
	if (FAILED(hResult)) {
		m_16BitTextureCap = false;
	}
	//Check RGBA5551
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A1R5G5B5);
	if (FAILED(hResult)) {
		m_16BitTextureCap = false;
	}
	//Check RGB555
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_X1R5G5B5);
	if (FAILED(hResult)) {
		m_16BitTextureCap = false;
	}

	//Set alpha texture capability flag
	m_alphaTextureCap = true;
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8);
	if (FAILED(hResult)) {
		m_alphaTextureCap = false;
	}



	// Validate flags.

	//Special extensions validation for init only config pass
	if (!m_dxt1TextureCap) SupportsTC = 0;

	//DCV refresh
	ConfigValidate_RefreshDCV();

	//Required extensions config validation pass
	ConfigValidate_RequiredExtensions();


	if (!MaxTMUnits || (MaxTMUnits > MAX_TMUNITS)) {
		MaxTMUnits = MAX_TMUNITS;
	}

	if (UseMultiTexture) {
		TMUnits = m_d3dCaps.MaxSimultaneousTextures;
		debugf(TEXT("%i Texture Mapping Units found"), TMUnits);
		if (TMUnits > MaxTMUnits) {
			TMUnits = MaxTMUnits;
		}
	}
	else {
		TMUnits = 1;
	}


	//Main config validation pass (after set TMUnits)
	ConfigValidate_Main();


	if (MaxAnisotropy < 0) {
		MaxAnisotropy = 0;
	}
	if (MaxAnisotropy) {
		int iMaxAnisotropyLimit;
		iMaxAnisotropyLimit = m_d3dCaps.MaxAnisotropy;
		debugf(TEXT("MaxAnisotropy = %i"), iMaxAnisotropyLimit);
		if (MaxAnisotropy > iMaxAnisotropyLimit) {
			MaxAnisotropy = iMaxAnisotropyLimit;
		}
	}

	BufferActorTris = 1;

	if (SupportsTC) {
		debugf(TEXT("Trying to use S3TC extension."));
	}

	//Special case MaxLogTextureSize == 0
	if (MaxLogTextureSize == 0) MaxLogTextureSize = 8;

    INT D3DMaxTextureSize = Min(m_d3dCaps.MaxTextureWidth, m_d3dCaps.MaxTextureHeight);
	MaxTextureSize = D3DMaxTextureSize;

	INT Dummy = -1;
	while (D3DMaxTextureSize > 0) {
		D3DMaxTextureSize >>= 1;
		Dummy++;
	}

	if ((MaxLogTextureSize > Dummy) || (SupportsTC)) MaxLogTextureSize = Dummy;
	if ((MinLogTextureSize < 2) || (SupportsTC)) MinLogTextureSize = 2;

	if (SupportsTC) {
		MaxLogUOverV = MaxLogTextureSize;
		MaxLogVOverU = MaxLogTextureSize;
	}
	else {
		MaxLogUOverV = 8;
		MaxLogVOverU = 8;

		INT MaxTextureAspectRatio = m_d3dCaps.MaxTextureAspectRatio;
		if (MaxTextureAspectRatio > 0) {
			INT MaxLogTextureAspectRatio = -1;
			while (MaxTextureAspectRatio > 0) {
				MaxTextureAspectRatio >>= 1;
				MaxLogTextureAspectRatio++;
			}
			if (MaxLogTextureAspectRatio < MaxLogUOverV) MaxLogUOverV = MaxLogTextureAspectRatio;
			if (MaxLogTextureAspectRatio < MaxLogVOverU) MaxLogVOverU = MaxLogTextureAspectRatio;
		}
	}

    debugf(TEXT("MaxTextureSize: %i"), MaxTextureSize);
	debugf(TEXT("MinLogTextureSize = %i"), MinLogTextureSize);
	debugf(TEXT("MaxLogTextureSize = %i"), MaxLogTextureSize);


	//Set pointers to aligned memory
	MapDotArray = (FGLMapDot *)Align(m_MapDotArrayMem, VERTEX_ARRAY_ALIGN);


	// Verify hardware defaults.
	check(MinLogTextureSize >= 0);
	check(MaxLogTextureSize >= 0);
	check(MinLogTextureSize < MaxLogTextureSize);
	check(MinLogTextureSize <= MaxLogTextureSize);

	// Flush textures.
	Flush(1);

	//Invalidate fixed texture ids
	m_pNoTexObj = NULL;
	m_pAlphaTexObj = NULL;

	//Initialize permanent rendering state, including allocation of some resources
	InitPermanentResourcesAndRenderingState();


	//Initialize previous lock variables
	PL_DetailTextures = DetailTextures;
	PL_OneXBlending = OneXBlending;
	PL_MaxLogUOverV = MaxLogUOverV;
	PL_MaxLogVOverU = MaxLogVOverU;
	PL_MinLogTextureSize = MinLogTextureSize;
	PL_MaxLogTextureSize = MaxLogTextureSize;
	PL_NoFiltering = NoFiltering;
	PL_UseTrilinear = UseTrilinear;
	PL_Use16BitTextures = Use16BitTextures;
	PL_Use565Textures = Use565Textures;
	PL_TexDXT1ToDXT3 = TexDXT1ToDXT3;
	PL_MaxAnisotropy = MaxAnisotropy;
	PL_SmoothMaskedTextures = SmoothMaskedTextures;
	PL_MaskedTextureHack = MaskedTextureHack;
	PL_LODBias = LODBias;
	PL_UsePalette = UsePalette;
	PL_UseAlphaPalette = UseAlphaPalette;
	PL_UseDetailAlpha = UseDetailAlpha;
	PL_SinglePassDetail = SinglePassDetail;
	PL_UseVertexProgram = UseVertexProgram;
	PL_UseSSE = UseSSE;
	PL_UseSSE2 = UseSSE2;


	//Reset current frame count
	m_currentFrameCount = 0;

	// Remember fullscreenness.
	WasFullscreen = Fullscreen;

	return 1;

	unguard;
}

void UD3D8RenderDevice::UnsetRes() {
	guard(UD3D8RenderDevice::UnsetRes);

	check(m_d3d8);
	check(m_d3dDevice);

	//Flush textures
	Flush(1);

	//Free fixed textures if they were allocated
	if (m_pNoTexObj) {
		m_pNoTexObj->Release();
		m_pNoTexObj = NULL;
	}
	if (m_pAlphaTexObj) {
		m_pAlphaTexObj->Release();
		m_pAlphaTexObj = NULL;
	}

	//Free permanent resources
	FreePermanentResources();

	//Release D3D device
	m_d3dDevice->Release();
	m_d3dDevice = NULL;

	//Release main D3D8 object
	m_d3d8->Release();
	m_d3d8 = NULL;

	unguard;
}


bool UD3D8RenderDevice::CheckDepthFormat(D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat, D3DFORMAT depthBufferFormat) {
	HRESULT hResult;

	//Check depth format
	hResult = m_d3d8->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, depthBufferFormat);
	if (FAILED(hResult)) {
		return false;
	}

	//Check depth format compatibility
	hResult = m_d3d8->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, backBufferFormat, depthBufferFormat);
	if (FAILED(hResult)) {
		return false;
	}

	return true;
}


void UD3D8RenderDevice::ConfigValidate_RefreshDCV(void) {
	#define UTGLR_REFRESH_DCV(_name) _name = DCV._name

	UTGLR_REFRESH_DCV(SinglePassDetail);
	UTGLR_REFRESH_DCV(UseVertexProgram);

	#undef UTGLR_REFRESH_DCV

	return;
}

void UD3D8RenderDevice::ConfigValidate_RequiredExtensions(void) {
	//Simple checks
	if (m_d3dCaps.VertexShaderVersion < D3DVS_VERSION(1,1)) UseVertexProgram = 0;
	if (!(m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BLENDCURRENTALPHA)) DetailTextures = 0;
	if (!(m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BLENDCURRENTALPHA)) UseDetailAlpha = 0;
	if (!m_alphaTextureCap) UseDetailAlpha = 0;
	if (!(m_d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)) MaxAnisotropy = 0;
	if (!(m_d3dCaps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS)) LODBias = 0;
	if (!m_palettedTextureCap) UsePalette = 0;
	if (!(m_d3dCaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)) UseAlphaPalette = 0;
	if (!m_dxt3TextureCap) TexDXT1ToDXT3 = 0;
	if (!m_16BitTextureCap) Use16BitTextures = 0;

	if (!(m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR)) SinglePassFog = 0;

	//Force 1X blending if no 2X modulation support
	if (!(m_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_MODULATE2X)) OneXBlending = 0x1;	//Must use proper bit offset for Bool param

	return;
}

void UD3D8RenderDevice::ConfigValidate_Main(void) {
	//Detail alpha requires at least two texture units
	if (TMUnits < 2) UseDetailAlpha = 0;

	//Single pass detail texturing requires at least 4 texture units
	if (TMUnits < 4) SinglePassDetail = 0;
	//Single pass detail texturing requires detail alpha
	if (!UseDetailAlpha) SinglePassDetail = 0;

	//Limit maximum DetailMax
	if (DetailMax > 3) DetailMax = 3;

	//Must use detail alpha for vertex program detail textures
	if (DetailTextures) {
		if (!UseDetailAlpha) {
			UseVertexProgram = 0;
		}
	}

	return;
}


void UD3D8RenderDevice::InitPermanentResourcesAndRenderingState(void) {
	guard(InitPermanentResourcesAndRenderingState);

	unsigned int u;
	HRESULT hResult;

#ifdef UTD3D8R_INCLUDE_VERTEX_PROGRAM_ASM
	AssembleVertexProgram();
#endif

	//Set view matrix
	D3DMATRIX d3dView = { +1.0f,  0.0f,  0.0f,  0.0f,
						   0.0f, -1.0f,  0.0f,  0.0f,
						   0.0f,  0.0f, -1.0f,  0.0f,
						   0.0f,  0.0f,  0.0f, +1.0f };
	m_d3dDevice->SetTransform(D3DTS_VIEW, &d3dView);

	//Little white texture for no texture operations
	InitNoTextureSafe();

	m_d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	SetZTestMode(ZTEST_LessEqual);

	m_d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	m_d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	m_d3dDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);

#ifdef UTGLR_RUNE_BUILD
	m_d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	FLOAT fFogStart = 0.0f;
	m_d3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&fFogStart);
#endif

	m_d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//Color and alpha modulation on texEnv0
	m_d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	//Set default texture stage tracking values
	for (u = 0; u < MAX_TMUNITS; u++) {
		m_curTexStageParams[u] = CT_DEFAULT_TEX_PARAMS;
	}

	if (LODBias) {
		SetTexLODBiasState(TMUnits);
	}

	if (MaxAnisotropy) {
		SetTexMaxAnisotropyState(TMUnits);
	}

	if (UseDetailAlpha) {	// vogel: alpha texture for better detail textures (no vertex alpha)
		InitAlphaTextureSafe();
	}

	//Initialize texture environment state
	InitOrInvalidateTexEnvState();

	//Reset current texture ids to hopefully unused values
	for (u = 0; u < MAX_TMUNITS; u++) {
		TexInfo[u].CurrentCacheID = TEX_CACHE_ID_UNUSED;
		TexInfo[u].pBind = NULL;
	}


	//Create vertex buffers
	D3DPOOL vertexBufferPool = D3DPOOL_DEFAULT;

	//Vertex and primary color
	hResult = m_d3dDevice->CreateVertexBuffer(sizeof(FGLVertexColor) * VERTEX_ARRAY_SIZE, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, vertexBufferPool, &m_d3dVertexColorBuffer);
	if (FAILED(hResult)) {
		appErrorf(TEXT("CreateVertexBuffer failed"));
	}

	//Secondary color
	hResult = m_d3dDevice->CreateVertexBuffer(sizeof(FGLSecondaryColor) * VERTEX_ARRAY_SIZE, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, vertexBufferPool, &m_d3dSecondaryColorBuffer);
	if (FAILED(hResult)) {
		appErrorf(TEXT("CreateVertexBuffer failed"));
	}

	//TexCoord
	for (u = 0; u < TMUnits; u++) {
		hResult = m_d3dDevice->CreateVertexBuffer(sizeof(FGLTexCoord) * VERTEX_ARRAY_SIZE, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, vertexBufferPool, &m_d3dTexCoordBuffer[u]);
		if (FAILED(hResult)) {
			appErrorf(TEXT("CreateVertexBuffer failed"));
		}
	}


	//Set vertex shader usage mode
	m_vertexShaderUsage = (m_doSoftwareVertexInit) ? D3DUSAGE_SOFTWAREPROCESSING : 0;

	//Create stream definitions

	//Standard stream definitions with vertices, color, and a variable number of tex coords
	for (u = 0; u < TMUnits; u++) {
		hResult = m_d3dDevice->CreateVertexShader(g_standardNTextureStreamDefs[u], NULL, &m_standardNTextureStreamShader[u], m_vertexShaderUsage);
		if (FAILED(hResult)) {
			appErrorf(TEXT("CreateVertexShader on stream definition failed"));
		}
		m_standardNTextureStreamShaderCreated[u] = true;
	}

	//Stream definition with vertices, two colors, and one tex coord
	hResult = m_d3dDevice->CreateVertexShader(g_twoColorSingleTextureStreamDef, NULL, &m_twoColorSingleTextureStreamShader, m_vertexShaderUsage);
	if (FAILED(hResult)) {
		appErrorf(TEXT("CreateVertexShader on stream definition failed"));
	}
	m_twoColorSingleTextureStreamShaderCreated = true;


	//Initialize vertex buffer state tracking information
	m_curVertexBufferPos = 0;
	m_vertexColorBufferNeedsDiscard = false;
	m_secondaryColorBufferNeedsDiscard = false;
	for (u = 0; u < MAX_TMUNITS; u++) {
		m_texCoordBufferNeedsDiscard[u] = false;
	}


	//Set stream sources
	//Vertex and primary color
	hResult = m_d3dDevice->SetStreamSource(0, m_d3dVertexColorBuffer, sizeof(FGLVertexColor));
	if (FAILED(hResult)) {
		appErrorf(TEXT("SetStreamSource failed"));
	}

	//Secondary Color
	hResult = m_d3dDevice->SetStreamSource(1, m_d3dSecondaryColorBuffer, sizeof(FGLSecondaryColor));
	if (FAILED(hResult)) {
		appErrorf(TEXT("SetStreamSource failed"));
	}

	//TexCoord
	for (u = 0; u < TMUnits; u++) {
		hResult = m_d3dDevice->SetStreamSource(2 + u, m_d3dTexCoordBuffer[u], sizeof(FGLTexCoord));
		if (FAILED(hResult)) {
			appErrorf(TEXT("SetStreamSource failed"));
		}
	}


	//Setup vertex programs
	if (UseVertexProgram) {
		//Attempt to initialize vertex program mode
		TryInitializeVertexProgramMode();
	}


	//Set default stream definition
	hResult = m_d3dDevice->SetVertexShader(m_standardNTextureStreamShader[0]);
	if (FAILED(hResult)) {
		appErrorf(TEXT("SetVertexShader on stream definition failed"));
	}
	m_curVertexShader = m_standardNTextureStreamShader[0];


	//Initialize texture state cache information
	m_texEnableBits = 0x1;

	// Init variables.
	BufferedVerts = 0;
	BufferedTileVerts = 0;

	m_curBlendFlags = PF_Occlude;
	m_smoothMaskedTexturesBit = 0;
	m_lastPolyFlags = 0;

	//Initialize color flags
	m_requestedColorFlags = 0;

	//Initialize Z range hack state
	m_useZRangeHack = false;
	m_nearZRangeHackProjectionActive = false;
	m_requestNearZRangeHackProjection = false;
	m_lastRequestNearZRangeHackProjection = false;


	unguard;
}

void UD3D8RenderDevice::FreePermanentResources(void) {
	guard(FreePermanentResources);

	unsigned int u;
	HRESULT hResult;

	//Free vertex programs if they were allocated and leave vertex program mode if necessary
	ShutdownVertexProgramMode();


	//Unset stream sources
	//Vertex
	hResult = m_d3dDevice->SetStreamSource(0, NULL, 0);
	if (FAILED(hResult)) {
		appErrorf(TEXT("SetStreamSource failed"));
	}

	//Secondary Color
	hResult = m_d3dDevice->SetStreamSource(1, NULL, 0);
	if (FAILED(hResult)) {
		appErrorf(TEXT("SetStreamSource failed"));
	}

	//TexCoord
	for (u = 0; u < TMUnits; u++) {
		hResult = m_d3dDevice->SetStreamSource(2 + u, NULL, 0);
		if (FAILED(hResult)) {
			appErrorf(TEXT("SetStreamSource failed"));
		}
	}


	//Free vertex buffers
	if (m_d3dVertexColorBuffer) {
		m_d3dVertexColorBuffer->Release();
		m_d3dVertexColorBuffer = NULL;
	}
	if (m_d3dSecondaryColorBuffer) {
		m_d3dSecondaryColorBuffer->Release();
		m_d3dSecondaryColorBuffer = NULL;
	}
	for (u = 0; u < TMUnits; u++) {
		if (m_d3dTexCoordBuffer[u]) {
			m_d3dTexCoordBuffer[u]->Release();
			m_d3dTexCoordBuffer[u] = NULL;
		}
	}


	//Set stream shader to something else so that it isn't using a current stream shader
	m_d3dDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	//Free stream definitions
	//Standard stream definitions with vertices, color, and a variable number of tex coords
	for (u = 0; u < TMUnits; u++) {
		if (m_standardNTextureStreamShaderCreated[u]) {
			m_d3dDevice->DeleteVertexShader(m_standardNTextureStreamShader[u]);
			m_standardNTextureStreamShaderCreated[u] = false;
		}
	}
	//Stream definition with vertices, two colors, and one tex coord
	if (m_twoColorSingleTextureStreamShaderCreated) {
		m_d3dDevice->DeleteVertexShader(m_twoColorSingleTextureStreamShader);
		m_twoColorSingleTextureStreamShaderCreated = false;
	}

	unguard;
}


UBOOL UD3D8RenderDevice::Init(UViewport* InViewport, INT NewX, INT NewY, INT NewColorBytes, UBOOL Fullscreen) {
	guard(UD3D8RenderDevice::Init);

	debugf(TEXT("Initializing D3D8Drv..."));

	if (NumDevices == 0) {
		g_gammaFirstTime = true;
		g_haveOriginalGammaRamp = false;
	}

	// Get list of device modes.
	for (INT i = 0; ; i++) {
		UBOOL UnicodeOS;

#if defined(NO_UNICODE_OS_SUPPORT) || !defined(UNICODE)
		UnicodeOS = 0;
#elif defined(NO_ANSI_OS_SUPPORT)
		UnicodeOS = 1;
#else
		UnicodeOS = GUnicodeOS;
#endif

		if (!UnicodeOS) {
#if defined(NO_UNICODE_OS_SUPPORT) || !defined(UNICODE) || !defined(NO_ANSI_OS_SUPPORT)
			DEVMODEA Tmp;
			appMemzero(&Tmp, sizeof(Tmp));
			Tmp.dmSize = sizeof(Tmp);
			if (!EnumDisplaySettingsA(NULL, i, &Tmp)) {
				break;
			}
			Modes.AddUniqueItem(FPlane(Tmp.dmPelsWidth, Tmp.dmPelsHeight, Tmp.dmBitsPerPel, Tmp.dmDisplayFrequency));
#endif
		}
		else {
#if !defined(NO_UNICODE_OS_SUPPORT) && defined(UNICODE)
			DEVMODEW Tmp;
			appMemzero(&Tmp, sizeof(Tmp));
			Tmp.dmSize = sizeof(Tmp);
			if (!EnumDisplaySettingsW(NULL, i, &Tmp)) {
				break;
			}
			Modes.AddUniqueItem(FPlane(Tmp.dmPelsWidth, Tmp.dmPelsHeight, Tmp.dmBitsPerPel, Tmp.dmDisplayFrequency));
#endif
		}
	}

	//Load D3D8 library
	if (!hModuleD3d8) {
		hModuleD3d8 = LoadLibraryA(g_d3d8DllName);
		if (!hModuleD3d8) {
			debugf(NAME_Init, TEXT("Failed to load %ls"), appFromAnsi(g_d3d8DllName));
			return 0;
		}
		pDirect3DCreate8 = (LPDIRECT3DCREATE8)GetProcAddress(hModuleD3d8, "Direct3DCreate8");
		if (!pDirect3DCreate8) {
			debugf(NAME_Init, TEXT("Failed to load function from %ls"), appFromAnsi(g_d3d8DllName));
			return 0;
		}
	}

	NumDevices++;

	// Init this GL rendering context.
	m_zeroPrefixBindTrees = m_localZeroPrefixBindTrees;
	m_nonZeroPrefixBindTrees = m_localNonZeroPrefixBindTrees;
	m_nonZeroPrefixBindChain = &m_localNonZeroPrefixBindChain;
	m_RGBA8TexPool = &m_localRGBA8TexPool;
	m_LastZMode = 255;

	Viewport = InViewport;

	//Save main window handle
	m_hWnd = (HWND)InViewport->GetWindow();
	check(m_hWnd);
	//Get main window DC
	m_hDC = GetDC(m_hWnd);
	check(m_hDC);

	if (!SetRes(NewX, NewY, NewColorBytes, Fullscreen)) {
		return FailedInitf(*LocalizeError(TEXT("ResFailed")));
	}

	return 1;
	unguard;
}

UBOOL UD3D8RenderDevice::Exec(const TCHAR* Cmd, FOutputDevice& Ar) {
	guard(UD3D8RenderDevice::Exec);

#ifndef UTGLR_UNREAL_BUILD
	if (URenderDevice::Exec(Cmd, Ar)) {
		return 1;
	}
#endif
	if (ParseCommand(&Cmd, TEXT("DGL"))) {
		if (ParseCommand(&Cmd, TEXT("BUFFERTRIS"))) {
			BufferActorTris = !BufferActorTris;
			if (!UseVertexSpecular) BufferActorTris = 0;
			debugf(TEXT("BUFFERTRIS [%i]"), BufferActorTris);
			return 1;
		}
		else if (ParseCommand(&Cmd,TEXT("BUILD"))) {
			debugf(TEXT("D3D8 renderer built: ?????"));
			return 1;
		}
		else if (ParseCommand(&Cmd, TEXT("AA"))) {
			if (m_usingAA) {
				m_defAAEnable = !m_defAAEnable;
				debugf(TEXT("AA Enable [%u]"), (m_defAAEnable) ? 1 : 0);
			}
			return 1;
		}

		return 0;
	}
	else if (ParseCommand(&Cmd, TEXT("GetRes"))) {
		TArray<FPlane> Relevant;
		INT i;
		for (i = 0; i < Modes.Num(); i++) {
			if (Modes(i).Z == (Viewport->ColorBytes * 8))
				if
				(	(Modes(i).X!=320 || Modes(i).Y!=200)
				&&	(Modes(i).X!=640 || Modes(i).Y!=400) )
				Relevant.AddUniqueItem(FPlane(Modes(i).X, Modes(i).Y, 0, 0));
		}
		appQsort(&Relevant(0), Relevant.Num(), sizeof(FPlane), (QSORT_COMPARE)CompareRes);
		FString Str;
		for (i = 0; i < Relevant.Num(); i++) {
			Str += FString::Printf(TEXT("%ix%i "), (INT)Relevant(i).X, (INT)Relevant(i).Y);
		}
		Ar.Log(*Str.LeftChop(1));
		return 1;
	}

	return 0;
	unguard;
}

void UD3D8RenderDevice::Lock(FPlane InFlashScale, FPlane InFlashFog, FPlane ScreenClear, DWORD RenderLockFlags, BYTE* InHitData, INT* InHitSize) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: Lock = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::Lock);
	check(LockCount == 0);
	++LockCount;


	//Reset stats
	BindCycles = ImageCycles = ComplexCycles = GouraudCycles = TileCycles = 0;

	m_AASwitchCount = 0;
	m_stat0Count = 0;
	m_stat1Count = 0;


	HRESULT hResult;

	//Check for lost device
	hResult = m_d3dDevice->TestCooperativeLevel();
	if (FAILED(hResult)) {
#if 0
{
	dout << L"utd3d8r: Device Lost" << std::endl;
}
#endif
		//Wait for device to become available again
		while (1) {
			//Check if device can be reset and restored
			if (hResult == D3DERR_DEVICENOTRESET) {
				//Set new resolution
				m_SetRes_isDeviceReset = true;
				if (!SetRes(m_SetRes_NewX, m_SetRes_NewY, m_SetRes_NewColorBytes, m_SetRes_Fullscreen)) {
					appErrorf(TEXT("Failed to reset lost D3D device"));
				}

				//Exit wait loop
				break;
			}
			//If not lost and not ready to be restored, error
			else if (hResult != D3DERR_DEVICELOST) {
				appErrorf(TEXT("Error checking for lost D3D device"));
			}
			//Otherwise, device is lost and cannot be restored yet

			//Wait
			Sleep(100);

			//Don't wait for device to become available here to prevent deadlock
			break;
		}
	}

	//D3D begin scene
	if (FAILED(m_d3dDevice->BeginScene())) {
		appErrorf(TEXT("BeginScene failed"));
	}

	//Clear the Z-buffer
	if (1 || GIsEditor || (RenderLockFlags & LOCKR_ClearScreen)) {
		SetBlend(PF_Occlude);
		m_d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER | ((RenderLockFlags & LOCKR_ClearScreen) ? D3DCLEAR_TARGET : 0), (DWORD)FColor(ScreenClear).TrueColor(), 1.0f, 0);
	}
	SetZTestMode(ZTEST_LessEqual);

	bool flushTextures = false;
	bool needVertexProgramReload = false;


	//DCV refresh
	ConfigValidate_RefreshDCV();

	//Required extensions config validation pass
	ConfigValidate_RequiredExtensions();


	//Main config validation pass
	ConfigValidate_Main();


	//Detect changes in 1X blending setting and force tex env flush if necessary
	if (OneXBlending != PL_OneXBlending) {
		PL_OneXBlending = OneXBlending;
		InitOrInvalidateTexEnvState();
	}

	//Prevent changes to these parameters
	MaxLogUOverV = PL_MaxLogUOverV;
	MaxLogVOverU = PL_MaxLogVOverU;
	MinLogTextureSize = PL_MinLogTextureSize;
	MaxLogTextureSize = PL_MaxLogTextureSize;

	//Detect changes in various texture related options and force texture flush if necessary
	if (NoFiltering != PL_NoFiltering) {
		PL_NoFiltering = NoFiltering;
		flushTextures = true;
	}
	if (UseTrilinear != PL_UseTrilinear) {
		PL_UseTrilinear = UseTrilinear;
		flushTextures = true;
	}
	if (Use16BitTextures != PL_Use16BitTextures) {
		PL_Use16BitTextures = Use16BitTextures;
		flushTextures = true;
	}
	if (Use565Textures != PL_Use565Textures) {
		PL_Use565Textures = Use565Textures;
		flushTextures = true;
	}
	if (TexDXT1ToDXT3 != PL_TexDXT1ToDXT3) {
		PL_TexDXT1ToDXT3 = TexDXT1ToDXT3;
		flushTextures = true;
	}
	//MaxAnisotropy cannot be negative
	if (MaxAnisotropy < 0) {
		MaxAnisotropy = 0;
	}
	if (MaxAnisotropy > m_d3dCaps.MaxAnisotropy) {
		MaxAnisotropy = m_d3dCaps.MaxAnisotropy;
	}
	if (MaxAnisotropy != PL_MaxAnisotropy) {
		PL_MaxAnisotropy = MaxAnisotropy;
		flushTextures = true;

		SetTexMaxAnisotropyState(TMUnits);
	}

	if (SmoothMaskedTextures != PL_SmoothMaskedTextures) {
		PL_SmoothMaskedTextures = SmoothMaskedTextures;
		//Clear masked blending state if set before adjusting smooth masked textures bit
		SetBlend(PF_Occlude);
	}
	//Set smooth masked textures bit
	m_smoothMaskedTexturesBit = (SmoothMaskedTextures != 0) ? PF_Masked : 0;

	if (MaskedTextureHack != PL_MaskedTextureHack) {
		PL_MaskedTextureHack = MaskedTextureHack;
		flushTextures = true;
	}

	if (LODBias != PL_LODBias) {
		PL_LODBias = LODBias;
		SetTexLODBiasState(TMUnits);
	}

	if (UsePalette != PL_UsePalette) {
		PL_UsePalette = UsePalette;
		flushTextures = true;
	}
	if (UseAlphaPalette != PL_UseAlphaPalette) {
		PL_UseAlphaPalette = UseAlphaPalette;
		flushTextures = true;
	}

	if (DetailTextures != PL_DetailTextures) {
		PL_DetailTextures = DetailTextures;
		flushTextures = true;
	}

	if (UseDetailAlpha != PL_UseDetailAlpha) {
		PL_UseDetailAlpha = UseDetailAlpha;
		if (UseDetailAlpha) {
			InitAlphaTextureSafe();
			needVertexProgramReload = true;
		}
	}

	if (SinglePassDetail != PL_SinglePassDetail) {
		PL_SinglePassDetail = SinglePassDetail;
		if (SinglePassDetail) {
			needVertexProgramReload = true;
		}
	}

	if (UseVertexProgram != PL_UseVertexProgram) {
		PL_UseVertexProgram = UseVertexProgram;
		if (UseVertexProgram) {
			//Attempt to initialize vertex program mode
			TryInitializeVertexProgramMode();
			needVertexProgramReload = false;
		}
		else {
			//Free vertex programs if they were allocated and leave vertex program mode if necessary
			ShutdownVertexProgramMode();
		}
	}

	//Check if vertex program reload is necessary
	if (UseVertexProgram) {
		if (needVertexProgramReload) {
			//Attempt to initialize vertex program mode
			TryInitializeVertexProgramMode();
		}
	}

	if (UseSSE != PL_UseSSE) {
#ifdef UTGLR_INCLUDE_SSE_CODE
		if (UseSSE) {
			if (!CPU_DetectSSE()) {
				UseSSE = 0;
			}
		}
#else
		UseSSE = 0;
#endif
		PL_UseSSE = UseSSE;
	}
	if (UseSSE2 != PL_UseSSE2) {
#ifdef UTGLR_INCLUDE_SSE_CODE
		if (UseSSE2) {
			if (!CPU_DetectSSE2()) {
				UseSSE2 = 0;
			}
		}
#else
		UseSSE2 = 0;
#endif
		PL_UseSSE2 = UseSSE2;
	}


#ifdef UTGLR_UNREAL_BUILD
	ZRangeHack = 0;
#endif


	//Initialize buffer verts proc pointers
	m_pBuffer3BasicVertsProc = Buffer3BasicVerts;
	m_pBuffer3ColoredVertsProc = Buffer3ColoredVerts;
	m_pBuffer3FoggedVertsProc = Buffer3FoggedVerts;

#ifdef UTGLR_INCLUDE_SSE_CODE
	//Initialize SSE buffer verts proc pointers
	if (UseSSE) {
		m_pBuffer3ColoredVertsProc = Buffer3ColoredVerts_SSE;
		m_pBuffer3FoggedVertsProc = Buffer3FoggedVerts_SSE;
	}
	if (UseSSE2) {
		m_pBuffer3ColoredVertsProc = Buffer3ColoredVerts_SSE2;
		m_pBuffer3FoggedVertsProc = Buffer3FoggedVerts_SSE2;
	}
#endif //UTGLR_INCLUDE_SSE_CODE

	m_pBuffer3VertsProc = NULL;


	//Initialize render passes no check proc pointers
	if (UseVertexProgram) {
		m_pRenderPassesNoCheckSetupProc = &UD3D8RenderDevice::RenderPassesNoCheckSetup_VP;
		m_pRenderPassesNoCheckSetup_DualTextureAndDetailTextureProc = &UD3D8RenderDevice::RenderPassesNoCheckSetup_DualTextureAndDetailTexture_VP;
	}
	else {
		m_pRenderPassesNoCheckSetupProc = &UD3D8RenderDevice::RenderPassesNoCheckSetup;
		m_pRenderPassesNoCheckSetup_DualTextureAndDetailTextureProc = &UD3D8RenderDevice::RenderPassesNoCheckSetup_DualTextureAndDetailTexture;
	}

	//Initialize buffer detail texture data proc pointer
	m_pBufferDetailTextureDataProc = &UD3D8RenderDevice::BufferDetailTextureData;
#ifdef UTGLR_INCLUDE_SSE_CODE
	if (UseSSE2) {
		m_pBufferDetailTextureDataProc = &UD3D8RenderDevice::BufferDetailTextureData_SSE2;
	}
#endif //UTGLR_INCLUDE_SSE_CODE


	//Precalculate the cutoff for buffering actor triangles based on config settings
	if (!BufferActorTris) {
		m_bufferActorTrisCutoff = 0;
	}
	else if (!BufferClippedActorTris) {
		m_bufferActorTrisCutoff = 3;
	}
	else {
		m_bufferActorTrisCutoff = 10;
	}

	//Precalculate detail texture color
	if (ColorizeDetailTextures) {
		m_detailTextureColor4ub = 0x00408040;
	}
	else {
		m_detailTextureColor4ub = 0x00808080;
	}

	//Precalculate mask for MaskedTextureHack based on if it's enabled
	m_maskedTextureHackMask = (MaskedTextureHack) ? TEX_CACHE_ID_FLAG_MASKED : 0;

	// Remember stuff.
	FlashScale = InFlashScale;
	FlashFog   = InFlashFog;
	//HitCount = 0;
	HitData    = InHitData;
	HitSize    = InHitSize;

	//Flush textures if necessary due to config change
	if (flushTextures) {
		Flush(1);
	}

	unguard;
}

void UD3D8RenderDevice::SetSceneNode(FSceneNode* Frame) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: SetSceneNode = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::SetSceneNode);

	EndBuffering();		// Flush vertex array before changing the projection matrix!

	//No need to set default AA state here
	//No need to set default projection state as this function always sets/initializes it
	SetDefaultStreamState();
	SetDefaultTextureState();

	// Precompute stuff.
	FLOAT One_Over_FX = 1.0f / Frame->FX;
	m_Aspect = Frame->FY * One_Over_FX;
	m_RProjZ = appTan(Viewport->Actor->FovAngle * PI / 360.0);
	m_RFX2 = 2.0f * m_RProjZ * One_Over_FX;
	m_RFY2 = 2.0f * m_RProjZ * One_Over_FX;
#ifdef UTGLR_DX_BUILD
	m_sceneNodeX = Frame->X;
	m_sceneNodeY = Frame->Y;
#endif

	// Set viewport.
	D3DVIEWPORT8 d3dViewport;
	d3dViewport.X = Frame->XB;
	d3dViewport.Y = Frame->YB;
	d3dViewport.Width = Frame->X;
	d3dViewport.Height = Frame->Y;
	d3dViewport.MinZ = 0.0f;
	d3dViewport.MaxZ = 1.0f;
	m_d3dDevice->SetViewport(&d3dViewport);

	//Decide whether or not to use Z range hack
	m_useZRangeHack = false;
	if (ZRangeHack) {
		m_useZRangeHack = true;
	}

	// Set projection.
	if (Frame->Viewport->IsOrtho()) {
		//Don't use Z range hack if ortho projection
		m_useZRangeHack = false;

		SetOrthoProjection();
	}
	else {
		SetProjectionStateNoCheck(false);
	}

	// Set clip planes.
	if (HitData) {
	}

	unguard;
}

void UD3D8RenderDevice::Unlock(UBOOL Blit) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: Unlock = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::Unlock);

	EndBuffering();

	SetDefaultAAState();
	SetDefaultProjectionState();
	SetDefaultStreamState();
	SetDefaultTextureState();

	// Unlock and render.
	check(LockCount == 1);

	//D3D end scene
	if (FAILED(m_d3dDevice->EndScene())) {
		debugf(TEXT("EndScene failed"));
	}

	if (Blit) {
		HRESULT hResult;
		bool swapBuffersStatus;

		//Present
		hResult = m_d3dDevice->Present(NULL, NULL, NULL, NULL);
		swapBuffersStatus = (FAILED(hResult)) ? false : true;
		//Don't signal error if device is lost
		if (hResult == D3DERR_DEVICELOST) swapBuffersStatus = true;

		check(swapBuffersStatus);
		if (!m_prevSwapBuffersStatus) {
			check(swapBuffersStatus);
		}
		m_prevSwapBuffersStatus = swapBuffersStatus;
	}

	--LockCount;

	// Hits.
	if (HitData) {
		*HitSize = 0;
	}

	//Scan for old textures
	if (UseTexIdPool) {
		//Scan for old textures
		ScanForOldTextures();
	}

	//Increment current frame count
	m_currentFrameCount++;

	//Check for optional frame rate limit
	if (FrameRateLimit >= 20) {
#if defined UTGLR_DX_BUILD || defined UTGLR_UNREAL_BUILD || defined UTGLR_RUNE_BUILD
		FLOAT curFrameTimestamp;
#else
		FTime curFrameTimestamp;
#endif
		float timeDiff;
		float rcpFrameRateLimit;

		curFrameTimestamp = appSeconds();
		timeDiff = curFrameTimestamp - m_prevFrameTimestamp;
		m_prevFrameTimestamp = curFrameTimestamp;

		rcpFrameRateLimit = 1.0f / FrameRateLimit;
		if (timeDiff < rcpFrameRateLimit) {
			float sleepTime;

			sleepTime = rcpFrameRateLimit - timeDiff;
			appSleep(sleepTime);

			m_prevFrameTimestamp = appSeconds();
		}
	}


#if 0
	dout << TEXT("AA switch count = ") << m_AASwitchCount << std::endl;
	dout << TEXT("Stat 0 count = ") << m_stat0Count << std::endl;
	dout << TEXT("Stat 1 count = ") << m_stat1Count << std::endl;
#endif


	unguard;
}

void UD3D8RenderDevice::Flush(UBOOL AllowPrecache) {
	guard(UD3D8RenderDevice::Flush);
	unsigned int u;

	if (!m_d3dDevice) {
		return;
	}


	EndBuffering();

	//Disable fog
	m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	for (u = 0; u < TMUnits; u++) {
		m_d3dDevice->SetTexture(u, NULL);
	}

	for (u = 0; u < NUM_CTTree_TREES; u++) {
		DWORD_CTTree_t *zeroPrefixBindTree = &m_zeroPrefixBindTrees[u];
		for (DWORD_CTTree_t::node_t *zpbmPtr = zeroPrefixBindTree->begin(); zpbmPtr != zeroPrefixBindTree->end(); zpbmPtr = zeroPrefixBindTree->next_node(zpbmPtr)) {
			zpbmPtr->data.pTexObj->Release();
		}
		zeroPrefixBindTree->clear(&m_DWORD_CTTree_Allocator);
	}

	for (u = 0; u < NUM_CTTree_TREES; u++) {
		QWORD_CTTree_t *nonZeroPrefixBindTree = &m_nonZeroPrefixBindTrees[u];
		for (QWORD_CTTree_t::node_t *nzpbmPtr = nonZeroPrefixBindTree->begin(); nzpbmPtr != nonZeroPrefixBindTree->end(); nzpbmPtr = nonZeroPrefixBindTree->next_node(nzpbmPtr)) {
			nzpbmPtr->data.pTexObj->Release();
		}
		nonZeroPrefixBindTree->clear(&m_QWORD_CTTree_Allocator);
	}

	m_nonZeroPrefixBindChain->mark_as_clear();

	for (TexPoolMap_t::node_t *RGBA8TpPtr = m_RGBA8TexPool->begin(); RGBA8TpPtr != m_RGBA8TexPool->end(); RGBA8TpPtr = m_RGBA8TexPool->next_node(RGBA8TpPtr)) {
		while (QWORD_CTTree_NodePool_t::node_t *texPoolNodePtr = RGBA8TpPtr->data.try_remove()) {
			texPoolNodePtr->data.pTexObj->Release();
			m_QWORD_CTTree_Allocator.free_node(texPoolNodePtr);
		}
	}
	m_RGBA8TexPool->clear(&m_TexPoolMap_Allocator);

	while (QWORD_CTTree_NodePool_t::node_t *nzpnpPtr = m_nonZeroPrefixNodePool.try_remove()) {
		m_QWORD_CTTree_Allocator.free_node(nzpnpPtr);
	}

	AllocatedTextures = 0;

	//Reset current texture ids to hopefully unused values
	for (u = 0; u < MAX_TMUNITS; u++) {
		TexInfo[u].CurrentCacheID = TEX_CACHE_ID_UNUSED;
		TexInfo[u].pBind = NULL;
	}

	if (AllowPrecache && UsePrecache && !GIsEditor) {
		PrecacheOnFlip = 1;
	}

	SetGamma(Viewport->GetOuterUClient()->Brightness);

	unguard;
}

void UD3D8RenderDevice::DrawComplexSurface(FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: DrawComplexSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::DrawComplexSurface);
	check(Surface.Texture);

	EndBuffering();		// vogel: might have still been locked (can happen!)

	SetDefaultAAState();
	SetDefaultProjectionState();
	//This function uses cached stream state information
	//This function uses cached vertex program state information
	//This function uses cached texture state information

	clockFast(ComplexCycles);

	//Calculate UDot and VDot intermediates for complex surface
	m_csUDot = Facet.MapCoords.XAxis | Facet.MapCoords.Origin;
	m_csVDot = Facet.MapCoords.YAxis | Facet.MapCoords.Origin;

	// Buffer "static" geometry.
	INT Index;
	if (UseVertexProgram) {
		Index = BufferStaticComplexSurfaceGeometry_VP(Facet);
	}
	else {
		Index = BufferStaticComplexSurfaceGeometry(Facet);
	}

	//Reject invalid surfaces early
	if (Index == 0) {
		return;
	}

	//Save number of points
	m_csPtCount = Index;

	//See if detail texture should be drawn
	//FogMap and DetailTexture are mutually exclusive effects
	bool drawDetailTexture = false;
	if ((DetailTextures != 0) && Surface.DetailTexture && !Surface.FogMap) {
		drawDetailTexture = true;
	}

	//Check for detail texture
	if (drawDetailTexture == true) {
		DWORD anyIsNearBits;

		//Buffer detail texture data
		anyIsNearBits = (this->*m_pBufferDetailTextureDataProc)(380.0f);

		//Do not draw detail texture if no vertices are near
		if (anyIsNearBits == 0) {
			drawDetailTexture = false;
		}
	}


	DWORD PolyFlags = Surface.PolyFlags;

	//Initialize render passes state information
	m_rpPassCount = 0;
	m_rpTMUnits = TMUnits;
	m_rpForceSingle = false;
	m_rpMasked = ((PolyFlags & PF_Masked) == 0) ? false : true;
	m_rpSetDepthEqual = false;
	m_rpColor = 0xFFFFFFFF;


	//Do static render passes state setup
	if (UseVertexProgram) {
		const FVector &XAxis = Facet.MapCoords.XAxis;
		const FVector &YAxis = Facet.MapCoords.YAxis;

		FLOAT vsParams[8] = { XAxis.X, XAxis.Y, XAxis.Z, m_csUDot,
							  YAxis.X, YAxis.Y, YAxis.Z, m_csVDot };

		m_d3dDevice->SetVertexShaderConstant(4, vsParams, 2);
	}

	AddRenderPass(Surface.Texture, PolyFlags & ~PF_FlatShaded, 0.0f);

	if (Surface.MacroTexture)
    {
		AddRenderPass(Surface.MacroTexture, PF_Modulated, -0.5f);
	}

	if (Surface.LightMap) {
		AddRenderPass(Surface.LightMap, PF_Modulated, -0.5f);
	}

	if (Surface.FogMap) {
		//Check for single pass fog mode
		if (!SinglePassFog) {
			RenderPasses();
		}

		AddRenderPass(Surface.FogMap, PF_Highlighted, -0.5f);
	}

	// Draw detail texture overlaid, in a separate pass.
	if (drawDetailTexture == true) {
		bool singlePassDetail = (SinglePassDetail != 0);

		//Single pass detail must be done with two normal passes
		//If single pass rendering were forced earlier, it would be impossible to have 2 passes accumulated here
		singlePassDetail &= (m_rpPassCount == 2);

		if (singlePassDetail) {
			RenderPasses_DualTextureAndDetailTexture(*Surface.DetailTexture);
		}
		else {
			RenderPasses();

			bool clipDetailTexture = (DetailClipping != 0);

			if (m_rpMasked) {
				//Cannot use detail texture clipping with masked mode
				//It will not work with m_d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
				clipDetailTexture = false;

				if (m_rpSetDepthEqual == false) {
					SetZTestMode(ZTEST_Equal);
					m_rpSetDepthEqual = true;
				}
			}

			//This function should only be called if at least one polygon will be detail textured
			if (UseVertexProgram) {
				DrawDetailTexture_VP(*Surface.DetailTexture);
			}
			else {
				DrawDetailTexture(*Surface.DetailTexture, clipDetailTexture);
			}
		}
	}
	else {
		RenderPasses();
	}

	// UnrealEd selection.
	if (GIsEditor && (PolyFlags & PF_Selected)) {
		//No need to set default AA state here as it is always set on entry to DrawComplexSurface
		//No need to set default projection state here as it is always set on entry to DrawComplexSurface
		SetDefaultStreamState();
		SetDefaultTextureState();

		SetNoTexture(0);
		SetBlend(PF_Highlighted);

		for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next) {
			INT NumPts = Poly->NumPts;

			//Make sure at least NumPts entries are left in the vertex buffers
			if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
				FlushVertexBuffers();
			}

			//Lock vertexColor and texCoord0 buffers
			LockVertexColorBuffer();
			LockTexCoordBuffer(0);

			FGLTexCoord *pTexCoordArray = m_pTexCoordArray[0];
			FGLVertexColor *pVertexColorArray = m_pVertexColorArray;

			for (INT i = 0; i < Poly->NumPts; i++) {
				pTexCoordArray[i].u = 0.5f;
				pTexCoordArray[i].v = 0.5f;

				pVertexColorArray[i].x = Poly->Pts[i]->Point.X;
				pVertexColorArray[i].y = Poly->Pts[i]->Point.Y;
				pVertexColorArray[i].z = Poly->Pts[i]->Point.Z;
				pVertexColorArray[i].color = 0x7F00007F;
			}

			//Unlock vertexColor and texCoord0 buffers
			UnlockVertexColorBuffer();
			UnlockTexCoordBuffer(0);

			//Draw the triangle fan
			m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

			//Advance vertex buffer position
			m_curVertexBufferPos += NumPts;
		}
	}

	if (m_rpSetDepthEqual == true) {
		SetZTestMode(ZTEST_LessEqual);
	}

	unclockFast(ComplexCycles);
	unguard;
}

#ifdef UTGLR_RUNE_BUILD
void UD3D8RenderDevice::PreDrawFogSurface() {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PreDrawFogSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PreDrawFogSurface);

	EndBuffering();

	SetDefaultAAState();
	SetDefaultProjectionState();
	SetDefaultStreamState();
	SetDefaultTextureState();

	SetBlend(PF_AlphaBlend);

	SetNoTexture(0);

	unguard;
}

void UD3D8RenderDevice::PostDrawFogSurface() {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PostDrawFogSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PostDrawFogSurface);

	SetBlend(0);

	unguard;
}

void UD3D8RenderDevice::DrawFogSurface(FSceneNode* Frame, FFogSurf &FogSurf) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: DrawFogSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::DrawFogSurface);

	FPlane Modulate(Clamp(FogSurf.FogColor.X, 0.0f, 1.0f), Clamp(FogSurf.FogColor.Y, 0.0f, 1.0f), Clamp(FogSurf.FogColor.Z, 0.0f, 1.0f), 0.0f);

	FLOAT RFogDistance = 1.0f / FogSurf.FogDistance;

	if (FogSurf.PolyFlags & PF_Masked) {
		SetZTestMode(ZTEST_Equal);
	}

	//Set stream state
	SetStreamState(m_standardNTextureStreamShader[0]);

	for (FSavedPoly* Poly = FogSurf.Polys; Poly; Poly = Poly->Next) {
		INT NumPts = Poly->NumPts;

		//Make sure at least NumPts entries are left in the vertex buffers
		if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
			FlushVertexBuffers();
		}

		//Lock vertexColor and texCoord0 buffers
		LockVertexColorBuffer();
		LockTexCoordBuffer(0);

		INT Index = 0;
		for (INT i = 0; i < NumPts; i++) {
			FTransform* P = Poly->Pts[i];

			Modulate.W = P->Point.Z * RFogDistance;
			if (Modulate.W > 1.0f) {
				Modulate.W = 1.0f;
			}
			else if (Modulate.W < 0.0f) {
				Modulate.W = 0.0f;
			}

			FGLVertexColor &destVertexColor = m_pVertexColorArray[Index];
			destVertexColor.x = P->Point.X;
			destVertexColor.y = P->Point.Y;
			destVertexColor.z = P->Point.Z;
			destVertexColor.color = FPlaneTo_BGRA(&Modulate);

			FGLTexCoord &destTexCoord = m_pTexCoordArray[0][Index];
			destTexCoord.u = 0.0f;
			destTexCoord.v = 0.0f;

			Index++;
		}

		//Unlock vertexColor and texCoord0 buffers
		UnlockVertexColorBuffer();
		UnlockTexCoordBuffer(0);

		//Draw the triangles
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

		//Advance vertex buffer position
		m_curVertexBufferPos += NumPts;
	}

	if (FogSurf.PolyFlags & PF_Masked) {
		SetZTestMode(ZTEST_LessEqual);
	}

	unguard;
}

void UD3D8RenderDevice::PreDrawGouraud(FSceneNode* Frame, FLOAT FogDistance, FPlane FogColor) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PreDrawGouraud = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PreDrawGouraud);

	if (FogDistance > 0.0f) {
		EndBuffering();

		//Enable fog
		m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

		//Default fog mode is LINEAR
		//Default fog start is 0.0f
		m_d3dDevice->SetRenderState(D3DRS_FOGCOLOR, FPlaneTo_BGRAClamped(&FogColor));
		FLOAT fFogDistance = FogDistance;
		m_d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&fFogDistance);
	}

	unguard;
}

void UD3D8RenderDevice::PostDrawGouraud(FLOAT FogDistance) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PostDrawGouraud = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PostDrawGouraud);

	if (FogDistance > 0.0f) {
		EndBuffering();

		//Disable fog
		m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}

	unguard;
}
#endif
#ifdef UTGLR_UNREAL_227_BUILD
void UD3D8RenderDevice::PreDrawFogSurface() {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PreDrawFogSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PreDrawFogSurface);

	EndBuffering();

	SetDefaultAAState();
	SetDefaultProjectionState();
	SetDefaultStreamState();
	SetDefaultTextureState();

	SetBlend(PF_AlphaBlend);

	SetNoTexture(0);

	unguard;
}

void UD3D8RenderDevice::PostDrawFogSurface() {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PostDrawFogSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PostDrawFogSurface);

	SetBlend(0);

	unguard;
}

void UD3D8RenderDevice::DrawFogSurface(FSceneNode* Frame, FFogSurf &FogSurf) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: DrawFogSurface = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::DrawFogSurface);

	FPlane FogColor	= FogSurf.FogColor;
	if( FogSurf.PolyFlags & PF_Modulated)
	{
		FogColor=FPlane(0.5f,0.5f,0.5f,0.f);
	}
	else if( FogSurf.PolyFlags & PF_Translucent)
	{
		FogColor=FPlane(0.f,0.f,0.f,0.f);
	}

	FPlane Modulate(Clamp(FogColor.X, 0.0f, 1.0f), Clamp(FogColor.Y, 0.0f, 1.0f), Clamp(FogColor.Z, 0.0f, 1.0f), 0.0f);

	FLOAT RFogDistance = 1.f/(FogSurf.FogDistanceEnd-FogSurf.FogDistanceStart);
	FLOAT RFogDistanceStart = FogSurf.FogDistanceStart;

	if (FogSurf.PolyFlags & PF_Masked) {
		SetZTestMode(ZTEST_Equal);
	}

	//Set stream state
	SetStreamState(m_standardNTextureStreamShader[0]);

	for (FSavedPoly* Poly = FogSurf.Polys; Poly; Poly = Poly->Next) {
		INT NumPts = Poly->NumPts;

		//Make sure at least NumPts entries are left in the vertex buffers
		if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
			FlushVertexBuffers();
		}

		//Lock vertexColor and texCoord0 buffers
		LockVertexColorBuffer();
		LockTexCoordBuffer(0);

		INT Index = 0;
		for (INT i = 0; i < NumPts; i++) {
			FTransform* P = Poly->Pts[i];

			Modulate.W = Clamp<FLOAT>((Poly->Pts[i]->Point.Z-RFogDistanceStart) * RFogDistance,0.f,1.f);

			FGLVertexColor &destVertexColor = m_pVertexColorArray[Index];
			destVertexColor.x = P->Point.X;
			destVertexColor.y = P->Point.Y;
			destVertexColor.z = P->Point.Z;
			destVertexColor.color = FPlaneTo_BGRA(&Modulate);

			FGLTexCoord &destTexCoord = m_pTexCoordArray[0][Index];
			destTexCoord.u = 0.0f;
			destTexCoord.v = 0.0f;

			Index++;
		}

		//Unlock vertexColor and texCoord0 buffers
		UnlockVertexColorBuffer();
		UnlockTexCoordBuffer(0);

		//Draw the triangles
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

		//Advance vertex buffer position
		m_curVertexBufferPos += NumPts;
	}

	if (FogSurf.PolyFlags & PF_Masked) {
		SetZTestMode(ZTEST_LessEqual);
	}

	unguard;
}

void UD3D8RenderDevice::PreDrawGouraud(FSceneNode* Frame, FFogSurf &FogSurf) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PreDrawGouraud = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PreDrawGouraud);

	EndBuffering();

	FLOAT fFogstart=FogSurf.FogDistanceStart;
	FLOAT fFogEnd=FogSurf.FogDistanceEnd;
	FLOAT fFogDensity=FogSurf.FogDensity;

	FPlane FogColor	= FogSurf.FogColor;
	if( FogSurf.PolyFlags & PF_Modulated)
	{
		FogColor=FPlane(0.5f,0.5f,0.5f,0.f);
	}
	else if( FogSurf.PolyFlags & PF_Translucent)
	{
		FogColor=FPlane(0.f,0.f,0.f,0.f);
	}

	//Enable fog
	m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	m_d3dDevice->SetRenderState(D3DRS_FOGCOLOR, FPlaneTo_BGRAClamped(&FogColor));

	//Default fog mode is LINEAR
	//Default fog start is 0.0f
	if (FogSurf.FogMode >= 2){
		m_d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);
		m_d3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&fFogDensity);
	}
	else if (FogSurf.FogMode == 1){
		m_d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
		m_d3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&fFogDensity);
	}
	else{
		m_d3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		m_d3dDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&fFogstart)));
		m_d3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)&fFogEnd);
	}
	unguard;
}

void UD3D8RenderDevice::PostDrawGouraud(FLOAT FogDistance) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: PostDrawGouraud = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::PostDrawGouraud);

	EndBuffering();

	//Disable fog
	m_d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	unguard;
}
#endif
void UD3D8RenderDevice::DrawGouraudPolygonOld(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, INT NumPts, DWORD PolyFlags, FSpanBuffer* Span) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: DrawGouraudPolygonOld = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::DrawGouraudPolygonOld);
	clockFast(GouraudCycles);

	//Decide if should request near Z range hack projection
	bool requestNearZRangeHackProjection = false;
	if (m_useZRangeHack && (GUglyHackFlags & HACKFLAGS_NoNearZ)) {
		requestNearZRangeHackProjection = true;
	}
	//Set projection state
	SetProjectionState(requestNearZRangeHackProjection);

	//Check if should render fog and if vertex specular is supported
#ifdef UTGLR_RUNE_BUILD
	bool drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated | PF_AlphaBlend)) == PF_RenderFog) && UseVertexSpecular) ? true : false;
#elif UTGLR_UNREAL_227_BUILD
	bool drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated | PF_AlphaBlend)) == PF_RenderFog) && UseVertexSpecular) ? true : false;
#else
	bool drawFog = (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog) && UseVertexSpecular) ? true : false;
#endif

	//If not drawing fog, disable the PF_RenderFog flag
	if (!drawFog) {
		PolyFlags &= ~PF_RenderFog;
	}

	SetBlend(PolyFlags);
	SetTextureNoPanBias(0, Info, PolyFlags);

#ifdef UTGLR_RUNE_BUILD
	BYTE alpha = 255;
	if (PolyFlags & PF_AlphaBlend) {su
		alpha = appRound(Info.Texture->Alpha * 255.0f);
	}
#endif

	//Set stream state
	SetStreamState(drawFog ? m_twoColorSingleTextureStreamShader : m_standardNTextureStreamShader[0]);

	//Make sure at least NumPts entries are left in the vertex buffers
	if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor and texCoord0 buffers
	//Lock secondary color buffer if fog
	LockVertexColorBuffer();
	if (drawFog) {
		LockSecondaryColorBuffer();
	}
	LockTexCoordBuffer(0);

	INT Index = 0;
	for (INT i = 0; i < NumPts; i++) {
		FTransTexture* P = Pts[i];

		FGLTexCoord &destTexCoord = m_pTexCoordArray[0][Index];
		destTexCoord.u = P->U * TexInfo[0].UMult;
		destTexCoord.v = P->V * TexInfo[0].VMult;

		FGLVertexColor &destVertexColor = m_pVertexColorArray[Index];
		destVertexColor.x = P->Point.X;
		destVertexColor.y = P->Point.Y;
		destVertexColor.z = P->Point.Z;

		if (PolyFlags & PF_Modulated) {
			destVertexColor.color = 0xFFFFFFFF;
		}
		else if (drawFog) {
			FLOAT f255_Times_One_Minus_FogW = 255.0f * (1.0f - P->Fog.W);
			destVertexColor.color = FPlaneTo_BGRScaled_A255(&P->Light, f255_Times_One_Minus_FogW);
			m_pSecondaryColorArray[Index].specular = FPlaneTo_BGR_A0(&P->Fog);
		}
		else {
#ifdef UTGLR_RUNE_BUILD
			destVertexColor.color = FPlaneTo_BGR_Aub(&P->Light, alpha);
#else
			destVertexColor.color = FPlaneTo_BGR_A255(&P->Light);
#endif
		}

		Index++;
	}

	//Unlock vertexColor and texCoord0 buffers
	//Unlock secondary color buffer if fog
	UnlockVertexColorBuffer();
	if (drawFog) {
		UnlockSecondaryColorBuffer();
	}
	UnlockTexCoordBuffer(0);

#ifdef UTGLR_DEBUG_ACTOR_WIREFRAME
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
#endif

	//Draw the triangles
	m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

	//Advance vertex buffer position
	m_curVertexBufferPos += NumPts;

#ifdef UTGLR_DEBUG_ACTOR_WIREFRAME
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	unclockFast(GouraudCycles);
	unguard;
}

void UD3D8RenderDevice::DrawGouraudPolygon(FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, INT NumPts, DWORD PolyFlags, FSpanBuffer* Span) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: DrawGouraudPolygon = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::DrawGouraudPolygon);

	EndTileBuffering();

	//Reject invalid polygons early so that other parts of the code do not have to deal with them
	if (NumPts < 3) {
		return;
	}

	if (Info.Modifier)
	{
		FLOAT UM = Info.USize, VM = Info.VSize;
		for (INT i = 0; i < NumPts; ++i)
			Info.Modifier->TransformPointUV(Pts[i]->U, Pts[i]->V, UM, VM);
	}

	// m_d3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	if (NumPts > m_bufferActorTrisCutoff) {
		EndBuffering();

		SetDefaultAAState();
		//No need to set default projection state here as DrawGouraudPolygonOld will set its own projection state
		//No need to set default stream state here as DrawGouraudPolygonOld will set its own stream state
		SetDefaultTextureState();

		DrawGouraudPolygonOld(Frame, Info, Pts, NumPts, PolyFlags, Span);

		return;
	}

	//Load texture cache id
	QWORD CacheID = Info.CacheID;

	//Only attempt to alter texture cache id on certain textures
	if ((CacheID & 0xFF) == 0xE0) {
		//Alter texture cache id if masked texture hack is enabled and texture is masked
		CacheID |= ((PolyFlags & PF_Masked) ? TEX_CACHE_ID_FLAG_MASKED : 0) & m_maskedTextureHackMask;

		//Check for 16 bit texture option
		if (Use16BitTextures) {
			if (Info.Palette && (Info.Palette[128].A == 255)) {
				CacheID |= TEX_CACHE_ID_FLAG_16BIT;
			}
		}
	}

	//Decide if should request near Z range hack projection
	bool requestNearZRangeHackProjection = false;
	if (m_useZRangeHack && (GUglyHackFlags & HACKFLAGS_NoNearZ)) {
		requestNearZRangeHackProjection = true;
	}

	//Check if need to start new poly buffering
	//Make sure enough entries are left in the vertex buffers
	//based on the current position when it was locked
	if ((m_lastPolyFlags != PolyFlags) ||
		(requestNearZRangeHackProjection != m_lastRequestNearZRangeHackProjection) ||
		(TexInfo[0].CurrentCacheID != CacheID) ||
		((m_curVertexBufferPos + BufferedVerts + NumPts) >= (VERTEX_ARRAY_SIZE - 14)) ||
		(BufferedVerts == 0))
	{
		EndGouraudPolygonBuffering();

		//Check if vertex buffer flush is required
		if ((m_curVertexBufferPos + BufferedVerts + NumPts) >= (VERTEX_ARRAY_SIZE - 14)) {
			FlushVertexBuffers();
		}

		//Check if should render fog and if vertex specular is supported
		//Also set other color flags
		if (PolyFlags & PF_Modulated) {
			m_requestedColorFlags = 0;
		}
		else {
			m_requestedColorFlags = CF_COLOR_ARRAY;

#ifdef UTGLR_RUNE_BUILD
			if (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated | PF_AlphaBlend)) == PF_RenderFog) && UseVertexSpecular) {
#elif UTGLR_UNREAL_227_BUILD
			if (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated | PF_AlphaBlend)) == PF_RenderFog) && UseVertexSpecular) {
#else
			if (((PolyFlags & (PF_RenderFog | PF_Translucent | PF_Modulated)) == PF_RenderFog) && UseVertexSpecular) {
#endif
				m_requestedColorFlags = CF_COLOR_ARRAY | CF_FOG_MODE;
			}
		}

		//If not drawing fog, disable the PF_RenderFog flag
		if (!(m_requestedColorFlags & CF_FOG_MODE)) {
			PolyFlags &= ~PF_RenderFog;
		}

		//Default vertex program state not needed for polygon caching stage
		//Default texture state not needed for polygon caching stage

		SetBlend(PolyFlags);
		SetTextureNoPanBias(0, Info, PolyFlags);

		//Set request near Z range hack projection flag
		m_requestNearZRangeHackProjection = requestNearZRangeHackProjection;

		//Save last used poly flags
		m_lastPolyFlags = PolyFlags;

		//Save last used request near Z range hack projection
		m_lastRequestNearZRangeHackProjection = m_requestNearZRangeHackProjection;

		//Select a buffer verts proc
		if (m_requestedColorFlags & CF_FOG_MODE) {
			m_pBuffer3VertsProc = m_pBuffer3FoggedVertsProc;
		}
		else if (m_requestedColorFlags & CF_COLOR_ARRAY) {
			m_pBuffer3VertsProc = m_pBuffer3ColoredVertsProc;
		}
		else {
			m_pBuffer3VertsProc = m_pBuffer3BasicVertsProc;
		}
#ifdef UTGLR_RUNE_BUILD
		m_gpAlpha = 255;
		if (PolyFlags & PF_AlphaBlend) {
			m_gpAlpha = appRound(Info.Texture->Alpha * 255.0f);
			m_pBuffer3VertsProc = Buffer3Verts;
		}
#endif

		//Lock vertexColor and texCoord0 buffers
		//Lock secondary color buffer if fog
		LockVertexColorBuffer();
		if (m_requestedColorFlags & CF_FOG_MODE) {
			LockSecondaryColorBuffer();
		}
		LockTexCoordBuffer(0);

		//Set stream state
		SetStreamState((m_requestedColorFlags & CF_FOG_MODE) ? m_twoColorSingleTextureStreamShader : m_standardNTextureStreamShader[0]);
	}

	//Buffer 3 vertices from the first (and perhaps only) triangle
	(m_pBuffer3VertsProc)(this, Pts);

	if (NumPts > 3) {
		//Buffer additional vertices from a clipped triangle
		BufferAdditionalClippedVerts(Pts, NumPts);
	}

	// m_d3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	unguard;
}

void UD3D8RenderDevice::DrawTile(FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: DrawTile = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::DrawTile);

	EndGouraudPolygonBuffering();

	if (SceneNodeHack) {
		if ((Frame->X != m_sceneNodeX) || (Frame->Y != m_sceneNodeY)) {
			SetSceneNode(Frame);
		}
	}

	//Adjust Z coordinate if Z range hack is active
	if (m_useZRangeHack) {
		if ((Z >= 0.5f) && (Z < 8.0f)) {
			Z = (((Z - 0.5f) / 7.5f) * 4.0f) + 4.0f;
		}
	}

	FLOAT PX1 = X - Frame->FX2 - 0.5f;
	FLOAT PX2 = PX1 + XL;
	FLOAT PY1 = Y - Frame->FY2 - 0.5f;
	FLOAT PY2 = PY1 + YL;

	FLOAT RFX2_Times_Z = m_RFX2 * Z;
	FLOAT RFY2_Times_Z = m_RFY2 * Z;

	FLOAT RPX1 = RFX2_Times_Z * PX1;
	FLOAT RPX2 = RFX2_Times_Z * PX2;
	FLOAT RPY1 = RFY2_Times_Z * PY1;
	FLOAT RPY2 = RFY2_Times_Z * PY2;

	if (BufferTileQuads) {
		//Load texture cache id
		QWORD CacheID = Info.CacheID;

		//Only attempt to alter texture cache id on certain textures
		if ((CacheID & 0xFF) == 0xE0) {
			//Alter texture cache id if masked texture hack is enabled and texture is masked
			CacheID |= ((PolyFlags & PF_Masked) ? TEX_CACHE_ID_FLAG_MASKED : 0) & m_maskedTextureHackMask;

			//Check for 16 bit texture option
			if (Use16BitTextures) {
				if (Info.Palette && (Info.Palette[128].A == 255)) {
					CacheID |= TEX_CACHE_ID_FLAG_16BIT;
				}
			}
		}

		//Check if need to start new tile buffering
		if ((m_lastPolyFlags != PolyFlags) ||
			(TexInfo[0].CurrentCacheID != CacheID) ||
			(m_curVertexBufferPos + BufferedTileVerts >= (VERTEX_ARRAY_SIZE - 6)) ||
			(BufferedTileVerts == 0))
		{
			//Flush any previously buffered tiles
			EndTileBuffering();

			//Check if vertex buffer flush is required
			if (m_curVertexBufferPos + BufferedTileVerts >= (VERTEX_ARRAY_SIZE - 6)) {
				FlushVertexBuffers();
			}

			//Default vertex program state not needed for polygon caching stage
			//Default texture state not needed for polygon caching stage

			//Save last used poly flags before local modification
			m_lastPolyFlags = PolyFlags;

#ifdef UTGLR_RUNE_BUILD
			if (Info.Palette && Info.Palette[128].A != 255 && !(PolyFlags & (PF_Translucent | PF_AlphaBlend))) {
#elif UTGLR_UNREAL_227_BUILD
			if (Info.Palette && Info.Palette[128].A != 255 && !(PolyFlags & (PF_Translucent | PF_AlphaBlend))) {
#else
			if (Info.Palette && Info.Palette[128].A != 255 && !(PolyFlags & PF_Translucent)) {
#endif
				PolyFlags |= PF_Highlighted;
			}

			SetBlend(PolyFlags);
			SetTextureNoPanBias(0, Info, PolyFlags);

			if (PolyFlags & PF_Modulated) {
				m_requestedColorFlags = 0;
			}
			else {
				m_requestedColorFlags = CF_COLOR_ARRAY;
			}

			//Lock vertexColor and texCoord0 buffers
			LockVertexColorBuffer();
			LockTexCoordBuffer(0);

			//Set stream state
			SetStreamState(m_standardNTextureStreamShader[0]);
		}

		//Get tile color
		DWORD tileColor;
		tileColor = 0xFFFFFFFF;
		if (!(PolyFlags & PF_Modulated)) {
			if (UseSSE2) {
#ifdef UTGLR_INCLUDE_SSE_CODE
				static __m128 fColorMul = { 255.0f, 255.0f, 255.0f, 0.0f };
				__m128 fColorMulReg;
				__m128 fColor;
				__m128 fAlpha;
				__m128i iColor;

				fColorMulReg = fColorMul;
				fColor = _mm_loadu_ps(&Color.X);
				fColor = _mm_mul_ps(fColor, fColorMulReg);

				//RGBA to BGRA
				fColor = _mm_shuffle_ps(fColor, fColor,  _MM_SHUFFLE(3, 0, 1, 2));

				fAlpha = _mm_setzero_ps();
				fAlpha = _mm_move_ss(fAlpha, fColorMulReg);
#ifdef UTGLR_RUNE_BUILD
				if (PolyFlags & PF_AlphaBlend) {
					fAlpha = _mm_mul_ss(fAlpha, _mm_load_ss(&Info.Texture->Alpha));
				}
#elif UTGLR_UNREAL_227_BUILD
				if (PolyFlags & PF_AlphaBlend) {
					fAlpha = _mm_mul_ss(fAlpha, _mm_load_ss(&Color.W));
				}
#endif
				fAlpha = _mm_shuffle_ps(fAlpha, fAlpha,  _MM_SHUFFLE(0, 1, 1, 1));

				fColor = _mm_or_ps(fColor, fAlpha);

				iColor = _mm_cvtps_epi32(fColor);
				iColor = _mm_packs_epi32(iColor, iColor);
				iColor = _mm_packus_epi16(iColor, iColor);

				tileColor = _mm_cvtsi128_si32(iColor);
#endif
			}
			else {
#ifdef UTGLR_RUNE_BUILD
				if (PolyFlags & PF_AlphaBlend) {
					Color.W = Info.Texture->Alpha;
					tileColor = FPlaneTo_BGRAClamped(&Color);
				}
				else {
					tileColor = FPlaneTo_BGRClamped_A255(&Color);
				}
#elif UTGLR_UNREAL_227_BUILD
				if (PolyFlags & PF_AlphaBlend) {
					tileColor = FPlaneTo_BGRAClamped(&Color);
				}
				else {
					tileColor = FPlaneTo_BGRClamped_A255(&Color);
				}
#else
				tileColor = FPlaneTo_BGRClamped_A255(&Color);
#endif
			}
		}

		//Buffer the tile
		FGLVertexColor *pVertexColorArray = &m_pVertexColorArray[BufferedTileVerts];
		FGLTexCoord *pTexCoordArray = &m_pTexCoordArray[0][BufferedTileVerts];

		pVertexColorArray[0].x = RPX1;
		pVertexColorArray[0].y = RPY1;
		pVertexColorArray[0].z = Z;
		pVertexColorArray[0].color = tileColor;

		pVertexColorArray[1].x = RPX2;
		pVertexColorArray[1].y = RPY1;
		pVertexColorArray[1].z = Z;
		pVertexColorArray[1].color = tileColor;

		pVertexColorArray[2].x = RPX2;
		pVertexColorArray[2].y = RPY2;
		pVertexColorArray[2].z = Z;
		pVertexColorArray[2].color = tileColor;

		pVertexColorArray[3].x = RPX1;
		pVertexColorArray[3].y = RPY1;
		pVertexColorArray[3].z = Z;
		pVertexColorArray[3].color = tileColor;

		pVertexColorArray[4].x = RPX2;
		pVertexColorArray[4].y = RPY2;
		pVertexColorArray[4].z = Z;
		pVertexColorArray[4].color = tileColor;

		pVertexColorArray[5].x = RPX1;
		pVertexColorArray[5].y = RPY2;
		pVertexColorArray[5].z = Z;
		pVertexColorArray[5].color = tileColor;

		FLOAT TexInfoUMult = TexInfo[0].UMult;
		FLOAT TexInfoVMult = TexInfo[0].VMult;

		FLOAT SU1 = (U) * TexInfoUMult;
		FLOAT SU2 = (U + UL) * TexInfoUMult;
		FLOAT SV1 = (V) * TexInfoVMult;
		FLOAT SV2 = (V + VL) * TexInfoVMult;

		pTexCoordArray[0].u = SU1;
		pTexCoordArray[0].v = SV1;

		pTexCoordArray[1].u = SU2;
		pTexCoordArray[1].v = SV1;

		pTexCoordArray[2].u = SU2;
		pTexCoordArray[2].v = SV2;

		pTexCoordArray[3].u = SU1;
		pTexCoordArray[3].v = SV1;

		pTexCoordArray[4].u = SU2;
		pTexCoordArray[4].v = SV2;

		pTexCoordArray[5].u = SU1;
		pTexCoordArray[5].v = SV2;

		BufferedTileVerts += 6;
	}
	else {
		DWORD tileColor;

		EndTileBuffering();

		clockFast(TileCycles);

		if (NoAATiles) {
			SetDisabledAAState();
		}
		else {
			SetDefaultAAState();
		}
		SetDefaultProjectionState();
		SetDefaultStreamState();
		SetDefaultTextureState();

#ifdef UTGLR_RUNE_BUILD
		if (Info.Palette && Info.Palette[128].A != 255 && !(PolyFlags & (PF_Translucent | PF_AlphaBlend))) {
#elif UTGLR_UNREAL_227_BUILD
		if (Info.Palette && Info.Palette[128].A != 255 && !(PolyFlags & (PF_Translucent | PF_AlphaBlend))) {
#else
		if (Info.Palette && Info.Palette[128].A != 255 && !(PolyFlags & PF_Translucent)) {
#endif
			PolyFlags |= PF_Highlighted;
		}

		SetBlend(PolyFlags);
		SetTextureNoPanBias(0, Info, PolyFlags);

		tileColor = 0xFFFFFFFF;
		if (!(PolyFlags & PF_Modulated)) {
#ifdef UTGLR_RUNE_BUILD
			if (PolyFlags & PF_AlphaBlend) {
				Color.W = Info.Texture->Alpha;
				tileColor = FPlaneTo_BGRAClamped(&Color);
			}
			else {
				tileColor = FPlaneTo_BGRClamped_A255(&Color);
			}
#elif UTGLR_UNREAL_227_BUILD
			if (PolyFlags & PF_AlphaBlend) {
				tileColor = FPlaneTo_BGRAClamped(&Color);
			}
			else {
				tileColor = FPlaneTo_BGRClamped_A255(&Color);
			}
#else
			tileColor = FPlaneTo_BGRClamped_A255(&Color);
#endif
		}

		//Make sure at least 4 entries are left in the vertex buffers
		if ((m_curVertexBufferPos + 4) >= VERTEX_ARRAY_SIZE) {
			FlushVertexBuffers();
		}

		//Lock vertexColor and texCoord0 buffers
		LockVertexColorBuffer();
		LockTexCoordBuffer(0);

		FGLTexCoord *pTexCoordArray = m_pTexCoordArray[0];
		FGLVertexColor *pVertexColorArray = m_pVertexColorArray;

		FLOAT TexInfoUMult = TexInfo[0].UMult;
		FLOAT TexInfoVMult = TexInfo[0].VMult;

		FLOAT SU1 = (U) * TexInfoUMult;
		FLOAT SU2 = (U + UL) * TexInfoUMult;
		FLOAT SV1 = (V) * TexInfoVMult;
		FLOAT SV2 = (V + VL) * TexInfoVMult;

		pTexCoordArray[0].u = SU1;
		pTexCoordArray[0].v = SV1;

		pTexCoordArray[1].u = SU2;
		pTexCoordArray[1].v = SV1;

		pTexCoordArray[2].u = SU2;
		pTexCoordArray[2].v = SV2;

		pTexCoordArray[3].u = SU1;
		pTexCoordArray[3].v = SV2;

		pVertexColorArray[0].x = RPX1;
		pVertexColorArray[0].y = RPY1;
		pVertexColorArray[0].z = Z;
		pVertexColorArray[0].color = tileColor;

		pVertexColorArray[1].x = RPX2;
		pVertexColorArray[1].y = RPY1;
		pVertexColorArray[1].z = Z;
		pVertexColorArray[1].color = tileColor;

		pVertexColorArray[2].x = RPX2;
		pVertexColorArray[2].y = RPY2;
		pVertexColorArray[2].z = Z;
		pVertexColorArray[2].color = tileColor;

		pVertexColorArray[3].x = RPX1;
		pVertexColorArray[3].y = RPY2;
		pVertexColorArray[3].z = Z;
		pVertexColorArray[3].color = tileColor;

		//Unlock vertexColor and texCoord0 buffers
		UnlockVertexColorBuffer();
		UnlockTexCoordBuffer(0);

		//Draw the tile
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, 2);

		//Advance vertex buffer position
		m_curVertexBufferPos += 4;

		unclockFast(TileCycles);
	}

	unguard;
}

void UD3D8RenderDevice::Draw2DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: Draw2DLine = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::Draw2DLine);

	EndBuffering();

	SetDefaultAAState();
	SetDefaultProjectionState();
	SetDefaultStreamState();
	SetDefaultTextureState();

	SetNoTexture(0);
	SetBlend(PF_Highlighted);

	DWORD lineColor = FPlaneTo_BGR_A255(&Color);

	//Make sure at least 2 entries are left in the vertex buffers
	if ((m_curVertexBufferPos + 2) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor and texCoord0 buffers
	LockVertexColorBuffer();
	LockTexCoordBuffer(0);

	FGLTexCoord *pTexCoordArray = m_pTexCoordArray[0];
	FGLVertexColor *pVertexColorArray = m_pVertexColorArray;

	pTexCoordArray[0].u = 0.0f;
	pTexCoordArray[0].v = 0.0f;

	pTexCoordArray[1].u = 1.0f;
	pTexCoordArray[1].v = 0.0f;

	pVertexColorArray[0].x = m_RFX2 * P1.Z * (P1.X - Frame->FX2 - 0.5f);
	pVertexColorArray[0].y = m_RFY2 * P1.Z * (P1.Y - Frame->FY2 - 0.5f);
	pVertexColorArray[0].z = P1.Z;
	pVertexColorArray[0].color = lineColor;

	pVertexColorArray[1].x = m_RFX2 * P2.Z * (P2.X - Frame->FX2 - 0.5f);
	pVertexColorArray[1].y = m_RFY2 * P2.Z * (P2.Y - Frame->FY2 - 0.5f);
	pVertexColorArray[1].z = P2.Z;
	pVertexColorArray[1].color = lineColor;

	//Unlock vertexColor and texCoord0 buffers
	UnlockVertexColorBuffer();
	UnlockTexCoordBuffer(0);

	//Draw the line
	m_d3dDevice->DrawPrimitive(D3DPT_LINELIST, m_curVertexBufferPos, 1);

	//Advance vertex buffer position
	m_curVertexBufferPos += 2;

	unguard;
}

void UD3D8RenderDevice::Draw3DLine(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: Draw3DLine = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::Draw3DLine);

	EndBuffering();

	SetDefaultAAState();
	SetDefaultProjectionState();
	SetDefaultStreamState();
	SetDefaultTextureState();

	P1 = P1.TransformPointBy(Frame->Coords);
	P2 = P2.TransformPointBy(Frame->Coords);
	if (Frame->Viewport->IsOrtho()) {
		// Zoom.
		P1.X = (P1.X) / Frame->Zoom + Frame->FX2;
		P1.Y = (P1.Y) / Frame->Zoom + Frame->FY2;
		P2.X = (P2.X) / Frame->Zoom + Frame->FX2;
		P2.Y = (P2.Y) / Frame->Zoom + Frame->FY2;
		P1.Z = P2.Z = 1;

		// See if points form a line parallel to our line of sight (i.e. line appears as a dot).
		if (Abs(P2.X - P1.X) + Abs(P2.Y - P1.Y) >= 0.2) {
			Draw2DLine(Frame, Color, LineFlags, P1, P2);
		}
		else if (Frame->Viewport->Actor->OrthoZoom < ORTHO_LOW_DETAIL) {
			Draw2DPoint(Frame, Color, LINE_None, P1.X - 1, P1.Y - 1, P1.X + 1, P1.Y + 1, P1.Z);
		}
	}
	else {
		SetNoTexture(0);
		SetBlend(PF_Highlighted);

		DWORD lineColor = FPlaneTo_BGR_A255(&Color);

		//Make sure at least 2 entries are left in the vertex buffers
		if ((m_curVertexBufferPos + 2) >= VERTEX_ARRAY_SIZE) {
			FlushVertexBuffers();
		}

		//Lock vertexColor and texCoord0 buffers
		LockVertexColorBuffer();
		LockTexCoordBuffer(0);

		FGLTexCoord *pTexCoordArray = m_pTexCoordArray[0];
		FGLVertexColor *pVertexColorArray = m_pVertexColorArray;

		pTexCoordArray[0].u = 0.0f;
		pTexCoordArray[0].v = 0.0f;

		pTexCoordArray[1].u = 1.0f;
		pTexCoordArray[1].v = 0.0f;

		pVertexColorArray[0].x = P1.X;
		pVertexColorArray[0].y = P1.Y;
		pVertexColorArray[0].z = P1.Z;
		pVertexColorArray[0].color = lineColor;

		pVertexColorArray[1].x = P2.X;
		pVertexColorArray[1].y = P2.Y;
		pVertexColorArray[1].z = P2.Z;
		pVertexColorArray[1].color = lineColor;

		//Unlock vertexColor and texCoord0 buffers
		UnlockVertexColorBuffer();
		UnlockTexCoordBuffer(0);

		//Draw the line
		m_d3dDevice->DrawPrimitive(D3DPT_LINELIST, m_curVertexBufferPos, 1);

		//Advance vertex buffer position
		m_curVertexBufferPos += 2;
	}

	unguard;
}

void UD3D8RenderDevice::Draw2DPoint(FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Z) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: Draw2DPoint = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::Draw2DPoint);

	EndBuffering();

	SetDefaultAAState();
	SetDefaultProjectionState();
	SetDefaultStreamState();
	SetDefaultTextureState();

	SetBlend(PF_Highlighted);
	SetNoTexture(0);

	DWORD pointColor = FPlaneTo_BGR_A255(&Color);

	//Make sure at least 4 entries are left in the vertex buffers
	if ((m_curVertexBufferPos + 4) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor and texCoord0 buffers
	LockVertexColorBuffer();
	LockTexCoordBuffer(0);

	FGLTexCoord *pTexCoordArray = m_pTexCoordArray[0];
	FGLVertexColor *pVertexColorArray = m_pVertexColorArray;

	pTexCoordArray[0].u = 0.0f;
	pTexCoordArray[0].v = 0.0f;

	pTexCoordArray[1].u = 1.0f;
	pTexCoordArray[1].v = 0.0f;

	pTexCoordArray[2].u = 1.0f;
	pTexCoordArray[2].v = 1.0f;

	pTexCoordArray[3].u = 0.0f;
	pTexCoordArray[3].v = 1.0f;

	FLOAT X1Pos = m_RFX2 * Z * (X1 - Frame->FX2 - 0.5f);
	FLOAT Y1Pos = m_RFY2 * Z * (Y1 - Frame->FY2 - 0.5f);
	FLOAT X2Pos = m_RFX2 * Z * (X2 - Frame->FX2 - 0.5f);
	FLOAT Y2Pos = m_RFY2 * Z * (Y2 - Frame->FY2 - 0.5f);

	pVertexColorArray[0].x = X1Pos;
	pVertexColorArray[0].y = Y1Pos;
	pVertexColorArray[0].z = Z;
	pVertexColorArray[0].color = pointColor;

	pVertexColorArray[1].x = X2Pos;
	pVertexColorArray[1].y = Y1Pos;
	pVertexColorArray[1].z = Z;
	pVertexColorArray[1].color = pointColor;

	pVertexColorArray[2].x = X2Pos;
	pVertexColorArray[2].y = Y2Pos;
	pVertexColorArray[2].z = Z;
	pVertexColorArray[2].color = pointColor;

	pVertexColorArray[3].x = X1Pos;
	pVertexColorArray[3].y = Y2Pos;
	pVertexColorArray[3].z = Z;
	pVertexColorArray[3].color = pointColor;

	//Unlock vertexColor and texCoord0 buffers
	UnlockVertexColorBuffer();
	UnlockTexCoordBuffer(0);

	//Draw the point
	m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, 2);

	//Advance vertex buffer position
	m_curVertexBufferPos += 4;

	unguard;
}


void UD3D8RenderDevice::ClearZ(FSceneNode* Frame) {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: ClearZ = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::ClearZ);

	EndBuffering();

	//Default AA state not required for glClear
	//Default projection state not required for glClear
	//Default stream state not required for glClear
	//Default vertex program state not required for glClear
	//Default texture state not required for glClear

	SetBlend(PF_Occlude);
	m_d3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	unguard;
}

void UD3D8RenderDevice::PushHit(const BYTE* Data, INT Count) {
	guard(UD3D8RenderDevice::PushHit);
	unguard;
}

void UD3D8RenderDevice::PopHit(INT Count, UBOOL bForce) {
	guard(UD3D8RenderDevice::PopHit);
	unguard;
}

void UD3D8RenderDevice::GetStats(TCHAR* Result) {
	guard(UD3D8RenderDevice::GetStats);
	appSprintf
	(
		Result,
		TEXT("D3D8 stats: Bind=%04.1f Image=%04.1f Complex=%04.1f Gouraud=%04.1f Tile=%04.1f"),
		GSecondsPerCycle*1000 * BindCycles,
		GSecondsPerCycle*1000 * ImageCycles,
		GSecondsPerCycle*1000 * ComplexCycles,
		GSecondsPerCycle*1000 * GouraudCycles,
		GSecondsPerCycle*1000 * TileCycles
	);
	unguard;
}

void UD3D8RenderDevice::ReadPixels(FColor* Pixels) {
	guard(UD3D8RenderDevice::ReadPixels);

	INT x, y;
	INT SizeX, SizeY;
	INT StartX = 0, StartY = 0;
	HRESULT hResult;
	IDirect3DSurface8 *d3dsFrontBuffer = NULL;
	D3DDISPLAYMODE d3ddm;

	SizeX = Viewport->SizeX;
	SizeY = Viewport->SizeY;

	//Get current display mode
	hResult = m_d3dDevice->GetDisplayMode(&d3ddm);
	if (FAILED(hResult)) {
		return;
	}

	//Check if windowed
	if (m_d3dpp.Windowed) {
		POINT upperLeft = { 0, 0 };

		if (!ClientToScreen(m_hWnd, &upperLeft)) {
			return;
		}

		//Check if entirely off the screen
		if (upperLeft.x <= -(INT)d3ddm.Width) return;
		if (upperLeft.y <= -(INT)d3ddm.Height) return;
		if (upperLeft.x >= (INT)d3ddm.Width) return;
		if (upperLeft.y >= (INT)d3ddm.Height) return;

		//Set start position
		StartX = upperLeft.x;
		if (StartX < 0) {
			SizeX -= (0 - StartX);
			StartX = 0;
		}
		if (SizeX <= 0) return;

		StartY = upperLeft.y;
		if (StartY < 0) {
			SizeY -= (0 - StartY);
			StartY = 0;
		}
		if (SizeY <= 0) return;

		if ((StartX + SizeX) > d3ddm.Width) {
			SizeX = d3ddm.Width - StartX;
		}
		if (SizeX <= 0) return;

		if ((StartY + SizeY) > d3ddm.Height) {
			SizeY = d3ddm.Height - StartY;
		}
		if (SizeY <= 0) return;
	}

	//Create surface to hold screenshot
	hResult = m_d3dDevice->CreateImageSurface(d3ddm.Width, d3ddm.Height, D3DFMT_A8R8G8B8, &d3dsFrontBuffer);
	if (FAILED(hResult)) {
		return;
	}

	//Get copy of front buffer
	hResult = m_d3dDevice->GetFrontBuffer(d3dsFrontBuffer);
	if (FAILED(hResult)) {
		//Release surface to hold screenshot
		d3dsFrontBuffer->Release();

		return;
	}

	//Clamp size just in case
	if (SizeX > d3ddm.Width) SizeX = d3ddm.Width;
	if (SizeY > d3ddm.Height) SizeY = d3ddm.Height;

	//Lock screenshot surface
	D3DLOCKED_RECT lockRect;
	hResult = d3dsFrontBuffer->LockRect(&lockRect, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
	if (FAILED(hResult)) {
		//Release surface to hold screenshot
		d3dsFrontBuffer->Release();

		return;
	}

	DWORD *pScreenshot = (DWORD *)lockRect.pBits;
	pScreenshot = (DWORD *)((BYTE *)pScreenshot + (StartY * lockRect.Pitch));
	INT DestSizeX = Viewport->SizeX;
	INT DestSizeY = Viewport->SizeY;
	for (y = 0; y < SizeY; y++) {
		for (x = 0; x < SizeX; x++) {
			DWORD dwPixel = pScreenshot[StartX + x];
			Pixels[(y * DestSizeX) + x] = FColor(((dwPixel >> 0) & 0xFF), ((dwPixel >> 8) & 0xFF), ((dwPixel >> 16) & 0xFF), 0xFF);
		}
		pScreenshot = (DWORD *)((BYTE *)pScreenshot + lockRect.Pitch);
	}

	//Unlock screenshot surface
	d3dsFrontBuffer->UnlockRect();

	//Release surface to hold screenshot
	d3dsFrontBuffer->Release();

	//Gamma correct screenshots if the option is true and the gamma ramp was set successfully
	if (GammaCorrectScreenshots && m_setGammaRampSucceeded) {
		FByteGammaRamp gammaByteRamp;
		BuildGammaRamp(SavedGammaCorrection, SavedGammaCorrection, SavedGammaCorrection, Brightness, gammaByteRamp);
		for (y = 0; y < DestSizeY; y++) {
			for (x = 0; x < DestSizeX; x++) {
				Pixels[x + y * DestSizeX].R = gammaByteRamp.red[Pixels[x + y * DestSizeX].R];
				Pixels[x + y * DestSizeX].G = gammaByteRamp.green[Pixels[x + y * DestSizeX].G];
				Pixels[x + y * DestSizeX].B = gammaByteRamp.blue[Pixels[x + y * DestSizeX].B];
			}
		}
	}

	unguard;
}

void UD3D8RenderDevice::EndFlash() {
#ifdef UTGLR_DEBUG_SHOW_CALL_COUNTS
{
	static int si;
	dout << L"utd3d8r: EndFlash = " << si++ << std::endl;
}
#endif
	guard(UD3D8RenderDevice::EndFlash);
	if (FlashScale != FPlane(.5,.5,.5,0) || FlashFog != FPlane(0,0,0,0)) {
		EndBuffering();

		SetDefaultAAState();
		SetDefaultProjectionState();
		SetDefaultStreamState();
		SetDefaultTextureState();

		SetBlend(PF_Highlighted);
		SetNoTexture(0);

		FPlane tempPlane = FPlane(FlashFog.X, FlashFog.Y, FlashFog.Z, 1.0 - Min(FlashScale.X * 2.0f, 1.0f));
		DWORD flashColor = FPlaneTo_BGRA(&tempPlane);

		FLOAT RFX2 = m_RProjZ;
		FLOAT RFY2 = m_RProjZ * m_Aspect;

		//Adjust Z coordinate if Z range hack is active
		FLOAT ZCoord = 1.0f;
		if (m_useZRangeHack) {
			ZCoord = (((ZCoord - 0.5f) / 7.5f) * 4.0f) + 4.0f;
		}

		//Make sure at least 4 entries are left in the vertex buffers
		if ((m_curVertexBufferPos + 4) >= VERTEX_ARRAY_SIZE) {
			FlushVertexBuffers();
		}

		//Lock vertexColor and texCoord0 buffers
		LockVertexColorBuffer();
		LockTexCoordBuffer(0);

		FGLTexCoord *pTexCoordArray = m_pTexCoordArray[0];
		FGLVertexColor *pVertexColorArray = m_pVertexColorArray;

		pTexCoordArray[0].u = 0.0f;
		pTexCoordArray[0].v = 0.0f;

		pTexCoordArray[1].u = 1.0f;
		pTexCoordArray[1].v = 0.0f;

		pTexCoordArray[2].u = 1.0f;
		pTexCoordArray[2].v = 1.0f;

		pTexCoordArray[3].u = 0.0f;
		pTexCoordArray[3].v = 1.0f;

		pVertexColorArray[0].x = RFX2 * (-1.0f * ZCoord);
		pVertexColorArray[0].y = RFY2 * (-1.0f * ZCoord);
		pVertexColorArray[0].z = ZCoord;
		pVertexColorArray[0].color = flashColor;

		pVertexColorArray[1].x = RFX2 * (+1.0f * ZCoord);
		pVertexColorArray[1].y = RFY2 * (-1.0f * ZCoord);
		pVertexColorArray[1].z = ZCoord;
		pVertexColorArray[1].color = flashColor;

		pVertexColorArray[2].x = RFX2 * (+1.0f * ZCoord);
		pVertexColorArray[2].y = RFY2 * (+1.0f * ZCoord);
		pVertexColorArray[2].z = ZCoord;
		pVertexColorArray[2].color = flashColor;

		pVertexColorArray[3].x = RFX2 * (-1.0f * ZCoord);
		pVertexColorArray[3].y = RFY2 * (+1.0f * ZCoord);
		pVertexColorArray[3].z = ZCoord;
		pVertexColorArray[3].color = flashColor;

		//Unlock vertexColor and texCoord0 buffers
		UnlockVertexColorBuffer();
		UnlockTexCoordBuffer(0);

		//Draw the square
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, 2);

		//Advance vertex buffer position
		m_curVertexBufferPos += 4;
	}
	unguard;
}

void UD3D8RenderDevice::PrecacheTexture(FTextureInfo& Info, DWORD PolyFlags) {
	guard(UD3D8RenderDevice::PrecacheTexture);
	SetTextureNoPanBias(0, Info, PolyFlags);
	unguard;
}


//This function is safe to call multiple times to initialize once
void UD3D8RenderDevice::InitNoTextureSafe(void) {
	guard(UD3D8RenderDevice::InitNoTexture);
	unsigned int u, v;
	HRESULT hResult;
	D3DLOCKED_RECT lockRect;
	DWORD *pTex;

	//Return early if already initialized
	if (m_pNoTexObj != 0) {
		return;
	}

	//Create the texture
	hResult = m_d3dDevice->CreateTexture(4, 4, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pNoTexObj);
	if (FAILED(hResult)) {
		appErrorf(TEXT("CreateTexture (basic RGBA8) failed"));
	}

	//Lock texture level 0
	if (FAILED(m_pNoTexObj->LockRect(0, &lockRect, NULL, D3DLOCK_NOSYSLOCK))) {
		appErrorf(TEXT("Texture lock failed"));
	}

	//Write texture
	pTex = (DWORD *)lockRect.pBits;
	for (u = 0; u < 4; u++) {
		for (v = 0; v < 4; v++) {
			pTex[v] = 0xFFFFFFFF;
		}
		pTex = (DWORD *)((BYTE *)pTex + lockRect.Pitch);
	}

	//Unlock texture level 0
	if (FAILED(m_pNoTexObj->UnlockRect(0))) {
		appErrorf(TEXT("Texture unlock failed"));
	}

	return;
	unguard;
}

//This function is safe to call multiple times to initialize once
void UD3D8RenderDevice::InitAlphaTextureSafe(void) {
	guard(UD3D8RenderDevice::InitAlphaTexture);
	unsigned int u;
	HRESULT hResult;
	D3DLOCKED_RECT lockRect;
	BYTE *pTex;

	//Return early if already initialized
	if (m_pAlphaTexObj != 0) {
		return;
	}

	//Create the texture
	hResult = m_d3dDevice->CreateTexture(256, 1, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &m_pAlphaTexObj);
	if (FAILED(hResult)) {
		appErrorf(TEXT("CreateTexture (alpha) failed"));
	}

	//Lock texture level 0
	if (FAILED(m_pAlphaTexObj->LockRect(0, &lockRect, NULL, D3DLOCK_NOSYSLOCK))) {
		appErrorf(TEXT("Texture lock failed"));
	}

	//Write texture
	pTex = (BYTE *)lockRect.pBits;
	for (u = 0; u < 256; u++) {
		pTex[u] = 255 - u;
	}

	//Unlock texture level 0
	if (FAILED(m_pAlphaTexObj->UnlockRect(0))) {
		appErrorf(TEXT("Texture unlock failed"));
	}

	return;
	unguard;
}

void UD3D8RenderDevice::ScanForOldTextures(void) {
	guard(UD3D8RenderDevice::ScanForOldTextures);

	unsigned int u;
	FCachedTexture *pCT;

	//Prevent currently bound textures from being recycled
	for (u = 0; u < MAX_TMUNITS; u++) {
		FCachedTexture *pBind = TexInfo[u].pBind;
		if (pBind != NULL) {
			//Update last used frame count so that the texture will not be recycled
			pBind->LastUsedFrameCount = m_currentFrameCount;

			//Move node to tail of linked list if in LRU list
			if (pBind->bindType == BIND_TYPE_NON_ZERO_PREFIX_LRU_LIST) {
				m_nonZeroPrefixBindChain->unlink(pBind);
				m_nonZeroPrefixBindChain->link_to_tail(pBind);
			}
		}
	}

	pCT = m_nonZeroPrefixBindChain->begin();
	while (pCT != m_nonZeroPrefixBindChain->end()) {
		DWORD numFramesSinceUsed = m_currentFrameCount - pCT->LastUsedFrameCount;
		if (numFramesSinceUsed > DynamicTexIdRecycleLevel) {
			//See if the tex pool is not enabled, or the tex format is not RGBA8, or the texture has mipmaps
			if (!UseTexPool || (pCT->texFormat != D3DFMT_A8R8G8B8) || (pCT->texParams.filter & CT_HAS_MIPMAPS_BIT)) {
				//Remove node from linked list
				m_nonZeroPrefixBindChain->unlink(pCT);

				//Get pointer to node in bind map
				QWORD_CTTree_t::node_t *pNode = (QWORD_CTTree_t::node_t *)((BYTE *)pCT - (PTRINT)&(((QWORD_CTTree_t::node_t *)0)->data));
				//Extract tree index
				BYTE treeIndex = pCT->treeIndex;
				//Advanced cached texture pointer to next entry in linked list
				pCT = pCT->pNext;

				//Remove node from bind map
				m_nonZeroPrefixBindTrees[treeIndex].remove(pNode);

				//Delete the texture
				pNode->data.pTexObj->Release();
#if 0
{
	static int si;
	dout << L"utd3d8r: Texture delete = " << si++ << std::endl;
}
#endif

				continue;
			}
			else {
				TexPoolMap_t::node_t *texPoolPtr;

#if 0
{
	static int si;
	dout << L"utd3d8r: TexPool free = " << si++ << L", Id = 0x" << HexString(32, (DWORD)pCT->pTexObj)
		<< L", u = " << pCT->UBits << L", v = " << pCT->VBits << std::endl;
}
#endif

				//Remove node from linked list
				m_nonZeroPrefixBindChain->unlink(pCT);

				//Create a key from the lg2 width and height of the texture object
				TexPoolMapKey_t texPoolKey = MakeTexPoolMapKey(pCT->UBits, pCT->VBits);

				//Get pointer to node in bind map
				QWORD_CTTree_t::node_t *pNode = (QWORD_CTTree_t::node_t *)((BYTE *)pCT - (PTRINT)&(((QWORD_CTTree_t::node_t *)0)->data));
				//Extract tree index
				BYTE treeIndex = pCT->treeIndex;
				//Advanced cached texture pointer to next entry in linked list
				pCT = pCT->pNext;

				//Remove node from bind map
				m_nonZeroPrefixBindTrees[treeIndex].remove(pNode);

				//See if the key does not yet exist
				texPoolPtr = m_RGBA8TexPool->find(texPoolKey);
				//If the key does not yet exist, add an empty vector in its place
				if (texPoolPtr == 0) {
					texPoolPtr = m_TexPoolMap_Allocator.alloc_node();
					texPoolPtr->key = texPoolKey;
					texPoolPtr->data = QWORD_CTTree_NodePool_t();
					m_RGBA8TexPool->insert(texPoolPtr);
				}

				//Add node plus texture id to a list in the tex pool based on its dimensions
				texPoolPtr->data.add(pNode);

				continue;
			}
		}

		//The list is sorted
		//Stop searching on first one not to be recycled
		break;

		pCT = pCT->pNext;
	}

	unguard;
}

void UD3D8RenderDevice::SetNoTextureNoCheck(INT Multi) {
	STAT(STAT_TRACK_TEXTURELOCK);

	guard(UD3D8RenderDevice::SetNoTexture);

	// Set small white texture.
	//Set texture
	m_d3dDevice->SetTexture(Multi, m_pNoTexObj);

	//Set filter
	SetTexFilter(Multi, CT_MIN_FILTER_POINT | CT_MIP_FILTER_NONE);

	TexInfo[Multi].CurrentCacheID = TEX_CACHE_ID_NO_TEX;
	TexInfo[Multi].pBind = NULL;

	unguard;
}

void UD3D8RenderDevice::SetAlphaTextureNoCheck(INT Multi) {
	STAT(STAT_TRACK_TEXTURELOCK);

	guard(UD3D8RenderDevice::SetAlphaTexture);

	// Set alpha gradient texture.
	//Set texture
	m_d3dDevice->SetTexture(Multi, m_pAlphaTexObj);

	//Set filter
	SetTexFilter(Multi, CT_MIN_FILTER_LINEAR | CT_MIP_FILTER_NONE | CT_MAG_FILTER_LINEAR_NOT_POINT_BIT | CT_ADDRESS_U_CLAMP | CT_ADDRESS_V_CLAMP);

	TexInfo[Multi].CurrentCacheID = TEX_CACHE_ID_ALPHA_TEX;
	TexInfo[Multi].pBind = NULL;

	unguard;
}

//This function must use Tex.CurrentCacheID and NEVER use Info.CacheID to reference the texture cache id
//This makes it work with the masked texture hack code
void UD3D8RenderDevice::SetTextureNoCheck(DWORD texNum, FTexInfo& Tex, FTextureInfo& Info, DWORD PolyFlags) {
	STAT(STAT_TRACK_TEXTURELOCK);

	guard(UD3D8RenderDevice::SetTexture);

	// Make current.
	bool isZeroPrefixCacheID = ((Tex.CurrentCacheID & 0xFFFFFFFF00000000) == 0) ? true : false;

	FCachedTexture *pBind = NULL;
	bool existingBind = false;
	HRESULT hResult;

	if (isZeroPrefixCacheID) {
		DWORD CacheIDSuffix = (Tex.CurrentCacheID & 0x00000000FFFFFFFF);

		DWORD_CTTree_t *zeroPrefixBindTree = &m_zeroPrefixBindTrees[CTZeroPrefixCacheIDSuffixToTreeIndex(CacheIDSuffix)];
		DWORD_CTTree_t::node_t *bindTreePtr = zeroPrefixBindTree->find(CacheIDSuffix);
		if (bindTreePtr != 0) {
			pBind = &bindTreePtr->data;
			existingBind = true;
		}
		else {
			DWORD_CTTree_t::node_t *pNewNode;

			//Insert new texture info
			pNewNode = m_DWORD_CTTree_Allocator.alloc_node();
			pNewNode->key = CacheIDSuffix;
			zeroPrefixBindTree->insert(pNewNode);
			pBind = &pNewNode->data;

			//Set bind type
			pBind->bindType = BIND_TYPE_ZERO_PREFIX;

			//Set default tex params
			pBind->texParams = CT_DEFAULT_TEX_PARAMS;
			pBind->dynamicTexBits = (PolyFlags & PF_NoSmooth) ? DT_NO_SMOOTH_BIT : 0;

			//Cache texture info for the new texture
			CacheTextureInfo(pBind, Info, PolyFlags);

#if 0
{
	static int si;
	dout << L"utd3d8r: Create texture zp = " << si++ << std::endl;
}
#endif
			//Create the texture
			hResult = m_d3dDevice->CreateTexture(1U << pBind->UBits, 1U << pBind->VBits, (Info.NumMips == 1) ? 1 : (pBind->MaxLevel + 1), 0, pBind->texFormat, D3DPOOL_MANAGED, &pBind->pTexObj);
			if (FAILED(hResult)) {
				appErrorf(TEXT("CreateTexture failed"));
			}

			//Allocate a new texture Id
			AllocatedTextures++;
		}
	}
	else {
		DWORD CacheIDSuffix = (Tex.CurrentCacheID & 0x00000000FFFFFFFF);
		DWORD treeIndex = CTNonZeroPrefixCacheIDSuffixToTreeIndex(CacheIDSuffix);

		QWORD_CTTree_t *nonZeroPrefixBindTree = &m_nonZeroPrefixBindTrees[treeIndex];
		QWORD_CTTree_t::node_t *bindTreePtr = nonZeroPrefixBindTree->find(Tex.CurrentCacheID);
		if (bindTreePtr != 0) {
			pBind = &bindTreePtr->data;
			pBind->LastUsedFrameCount = m_currentFrameCount;

			//Check if texture is in LRU list
			if (pBind->bindType == BIND_TYPE_NON_ZERO_PREFIX_LRU_LIST) {
				//Move node to tail of linked list
				m_nonZeroPrefixBindChain->unlink(pBind);
				m_nonZeroPrefixBindChain->link_to_tail(pBind);
			}

			existingBind = true;
		}
		else {
			QWORD_CTTree_t::node_t *pNewNode;

			//Allocate a new node
			//Use the node pool if it is not empty
			pNewNode = m_nonZeroPrefixNodePool.try_remove();
			if (!pNewNode) {
				pNewNode = m_QWORD_CTTree_Allocator.alloc_node();
			}

			//Insert new texture info
			pNewNode->key = Tex.CurrentCacheID;
			nonZeroPrefixBindTree->insert(pNewNode);
			pBind = &pNewNode->data;
			pBind->LastUsedFrameCount = m_currentFrameCount;

			//Set bind type
			pBind->bindType = BIND_TYPE_NON_ZERO_PREFIX_LRU_LIST;
			if (CacheStaticMaps && ((Tex.CurrentCacheID & 0xFF) == 0x18)) {
				pBind->bindType = BIND_TYPE_NON_ZERO_PREFIX;
			}

			//Save tree index
			pBind->treeIndex = (BYTE)treeIndex;

			//Set default tex params
			pBind->texParams = CT_DEFAULT_TEX_PARAMS;
			pBind->dynamicTexBits = (PolyFlags & PF_NoSmooth) ? DT_NO_SMOOTH_BIT : 0;

			//Check if texture should be in LRU list
			if (pBind->bindType == BIND_TYPE_NON_ZERO_PREFIX_LRU_LIST) {
				//Add node to linked list
				m_nonZeroPrefixBindChain->link_to_tail(pBind);
			}

			//Cache texture info for the new texture
			CacheTextureInfo(pBind, Info, PolyFlags);

			//See if the tex pool is enabled
			bool needTexIdAllocate = true;
			if (UseTexPool) {
				//See if the format will be RGBA8
				//Only textures without mipmaps are stored in the tex pool
				if ((pBind->texType == TEX_TYPE_NORMAL) && (Info.NumMips == 1)) {
					TexPoolMap_t::node_t *texPoolPtr;

					//Create a key from the lg2 width and height of the texture object
					TexPoolMapKey_t texPoolKey = MakeTexPoolMapKey(pBind->UBits, pBind->VBits);

					//Search for the key in the map
					texPoolPtr = m_RGBA8TexPool->find(texPoolKey);
					if (texPoolPtr != 0) {
						QWORD_CTTree_NodePool_t::node_t *texPoolNodePtr;

						//Get a reference to the pool of nodes with tex ids of the right dimension
						QWORD_CTTree_NodePool_t &texPool = texPoolPtr->data;

						//Attempt to get a texture id for the tex pool
						if ((texPoolNodePtr = texPool.try_remove()) != 0) {
							//Use texture id from node in tex pool
							pBind->pTexObj = texPoolNodePtr->data.pTexObj;

							//Use tex params from node in tex pool
							pBind->texParams = texPoolNodePtr->data.texParams;
							pBind->dynamicTexBits = texPoolNodePtr->data.dynamicTexBits;

							//Then add node to free list
							m_nonZeroPrefixNodePool.add(texPoolNodePtr);

#if 0
{
	static int si;
	dout << L"utd3d8r: TexPool retrieve = " << si++ << L", Id = 0x" << HexString(32, (DWORD)pBind->pTexObj)
		<< L", u = " << pBind->UBits << L", v = " << pBind->VBits << std::endl;
}
#endif

							//Clear the need tex id allocate flag
							needTexIdAllocate = false;
						}
					}
				}
			}
			if (needTexIdAllocate) {
#if 0
{
	static int si;
	dout << L"utd3d8r: Create texture nzp = " << si++ << std::endl;
}
#endif
				//Create the texture
				hResult = m_d3dDevice->CreateTexture(1U << pBind->UBits, 1U << pBind->VBits, (Info.NumMips == 1) ? 1 : (pBind->MaxLevel + 1), 0, pBind->texFormat, D3DPOOL_MANAGED, &pBind->pTexObj);
				if (FAILED(hResult)) {
					appErrorf(TEXT("CreateTexture failed"));
				}

				//Allocate a new texture id
				AllocatedTextures++;
			}
		}
	}

	//Save pointer to current texture bind for current texture unit
	Tex.pBind = pBind;

	//Set texture
	m_d3dDevice->SetTexture(texNum, pBind->pTexObj);

	// Account for all the impact on scale normalization.
	Tex.UMult = pBind->UMult;
	Tex.VMult = pBind->VMult;

	//Check for any changes to dynamic texture object parameters
	{
		BYTE desiredDynamicTexBits;

		desiredDynamicTexBits = (PolyFlags & PF_NoSmooth) ? DT_NO_SMOOTH_BIT : 0;
		if (desiredDynamicTexBits != pBind->dynamicTexBits) {
			BYTE dynamicTexBitsXor;

			dynamicTexBitsXor = desiredDynamicTexBits ^ pBind->dynamicTexBits;

			//Update dynamic tex bits early as there are no subsequent dependencies
			pBind->dynamicTexBits = desiredDynamicTexBits;

			if (dynamicTexBitsXor & DT_NO_SMOOTH_BIT) {
				BYTE desiredTexParamsFilter;

				//Set partial desired filter tex params
				desiredTexParamsFilter = 0;
				if (NoFiltering) {
					desiredTexParamsFilter |= CT_MIN_FILTER_POINT | CT_MIP_FILTER_NONE;
				}
				else if (PolyFlags & PF_NoSmooth) {
					desiredTexParamsFilter |= CT_MIN_FILTER_POINT;
					desiredTexParamsFilter |= ((pBind->texParams.filter & CT_HAS_MIPMAPS_BIT) == 0) ? CT_MIP_FILTER_NONE : CT_MIP_FILTER_POINT;
				}
				else {
					desiredTexParamsFilter |= (MaxAnisotropy) ? CT_MIN_FILTER_ANISOTROPIC : CT_MIN_FILTER_LINEAR;
					desiredTexParamsFilter |= ((pBind->texParams.filter & CT_HAS_MIPMAPS_BIT) == 0) ? CT_MIP_FILTER_NONE : (UseTrilinear ? CT_MIP_FILTER_LINEAR : CT_MIP_FILTER_POINT);
					desiredTexParamsFilter |= CT_MAG_FILTER_LINEAR_NOT_POINT_BIT;
				}

				//Store partial updated texture parameter state in cached texture object
				const BYTE MODIFIED_TEX_PARAMS_FILTER_BITS = CT_MIN_FILTER_MASK | CT_MIP_FILTER_MASK | CT_MAG_FILTER_LINEAR_NOT_POINT_BIT;
				pBind->texParams.filter = (pBind->texParams.filter & ~MODIFIED_TEX_PARAMS_FILTER_BITS) | desiredTexParamsFilter;
			}
		}
	}

	// Upload if needed.
	if (!existingBind || Info.bRealtimeChanged) {
		FColor paletteIndex0;

		// Cleanup texture flags.
		if (SupportsLazyTextures) {
			Info.Load();
		}
		Info.bRealtimeChanged = 0;

		//Set palette index 0 to black for masked paletted textures
		if (Info.Palette && (PolyFlags & PF_Masked)) {
			paletteIndex0 = Info.Palette[0];
			Info.Palette[0] = FColor(0,0,0,0);
		}

		// Download the texture.
		clockFast(ImageCycles);

		if (pBind->texType == TEX_TYPE_PALETTED) {
//			glColorTableEXT(GL_TEXTURE_2D, GL_RGBA, 256, GL_RGBA, GL_UNSIGNED_BYTE, Info.Palette);
		}

		m_texConvertCtx.pBind = pBind;

		UBOOL SkipMipmaps = (Info.NumMips == 1);
		INT MaxLevel = pBind->MaxLevel;

		//Only calculate texture filter parameters for new textures
		if (!existingBind) {
			tex_params_t desiredTexParams;

			//Set desired filter tex params
			desiredTexParams.filter = 0;
			if (NoFiltering) {
				desiredTexParams.filter |= CT_MIN_FILTER_POINT | CT_MIP_FILTER_NONE;
			}
			else if (PolyFlags & PF_NoSmooth) {
				desiredTexParams.filter |= CT_MIN_FILTER_POINT;
				desiredTexParams.filter |= SkipMipmaps ? CT_MIP_FILTER_NONE : CT_MIP_FILTER_POINT;
			}
			else {
				desiredTexParams.filter |= (MaxAnisotropy) ? CT_MIN_FILTER_ANISOTROPIC : CT_MIN_FILTER_LINEAR;
				desiredTexParams.filter |= SkipMipmaps ? CT_MIP_FILTER_NONE : (UseTrilinear ? CT_MIP_FILTER_LINEAR : CT_MIP_FILTER_POINT);
				desiredTexParams.filter |= CT_MAG_FILTER_LINEAR_NOT_POINT_BIT;
			}

			if (!SkipMipmaps) {
				desiredTexParams.filter |= CT_HAS_MIPMAPS_BIT;
			}

			#if  UTGLR_UNREAL_227_BUILD
			// Check if clamped or wrapped (f.e. for Skyboxes). Smirftsch
			if (Info.UClampMode)
				desiredTexParams.filter |= CT_ADDRESS_U_CLAMP;

			if (Info.VClampMode)
				desiredTexParams.filter |= CT_ADDRESS_V_CLAMP;
			#endif

			//Store updated texture parameter state in cached texture object
			pBind->texParams = desiredTexParams;
		}


		//Some textures only upload the base texture
		INT MaxUploadLevel = MaxLevel;
		if (SkipMipmaps) {
			MaxUploadLevel = 0;
		}


		//Set initial texture width and height in the context structure
		//Setup code must ensure that both UBits and VBits are greater than or equal to 0
		m_texConvertCtx.texWidthPow2 = 1 << pBind->UBits;
		m_texConvertCtx.texHeightPow2 = 1 << pBind->VBits;

		guard(WriteTexture);
		for (INT Level = 0; Level <= MaxUploadLevel; Level++) {
			// Convert the mipmap.
			INT MipIndex = pBind->BaseMip + Level;
			INT stepBits = 0;
			if (MipIndex >= Info.NumMips) {
				stepBits = MipIndex - (Info.NumMips - 1);
				MipIndex = Info.NumMips - 1;
			}
			m_texConvertCtx.stepBits = stepBits;

			FMipmapBase* Mip = Info.Mips[MipIndex];
			if (Mip->DataPtr) {
				//Lock texture level
				if (FAILED(pBind->pTexObj->LockRect(Level, &m_texConvertCtx.lockRect, NULL, D3DLOCK_NOSYSLOCK))) {
					appErrorf(TEXT("Texture lock failed"));
				}

				switch (pBind->texType) {
				case TEX_TYPE_COMPRESSED_DXT1:
					guard(ConvertDXT1_DXT1);
					ConvertDXT1_DXT1(Mip, Level);
					unguard;
					break;

				case TEX_TYPE_COMPRESSED_DXT1_TO_DXT3:
					guard(ConvertDXT1_DXT3);
					ConvertDXT1_DXT3(Mip, Level);
					unguard;
					break;

				case TEX_TYPE_COMPRESSED_DXT3:
					guard(ConvertDXT3_DXT3);
					ConvertDXT35_DXT35(Mip, Level);
					unguard;
					break;

				case TEX_TYPE_COMPRESSED_DXT5:
					guard(ConvertDXT5_DXT5);
					ConvertDXT35_DXT35(Mip, Level);
					unguard;
					break;

				case TEX_TYPE_PALETTED:
					guard(ConvertP8_P8);
					if (stepBits == 0) {
//						ConvertP8_P8_NoStep(Mip, Level);
					}
					else {
//						ConvertP8_P8(Mip, Level);
					}
					unguard;
					break;

				case TEX_TYPE_HAS_PALETTE:
					switch (pBind->texFormat) {
					case D3DFMT_R5G6B5:
						guard(ConvertP8_RGB565);
						if (stepBits == 0) {
							ConvertP8_RGB565_NoStep(Mip, Info.Palette, Level);
						}
						else {
							ConvertP8_RGB565(Mip, Info.Palette, Level);
						}
						unguard;
						break;

					case D3DFMT_X1R5G5B5:
					case D3DFMT_A1R5G5B5:
						guard(ConvertP8_RGBA5551);
						if (stepBits == 0) {
							ConvertP8_RGBA5551_NoStep(Mip, Info.Palette, Level);
						}
						else {
							ConvertP8_RGBA5551(Mip, Info.Palette, Level);
						}
						unguard;
						break;

					default:
						guard(ConvertP8_RGBA8888);
						if (stepBits == 0) {
							ConvertP8_RGBA8888_NoStep(Mip, Info.Palette, Level);
						}
						else {
							ConvertP8_RGBA8888(Mip, Info.Palette, Level);
						}
						unguard;
					}
					break;

				default:
					guard(ConvertBGRA7777);
					(this->*pBind->pConvertBGRA7777)(Mip, Level);
					unguard;
				}

				DWORD texWidth, texHeight;

				//Get current texture width and height
				texWidth = m_texConvertCtx.texWidthPow2;
				texHeight = m_texConvertCtx.texHeightPow2;

				//Calculate and save next texture width and height
				//Both are divided by two down to a floor of 1
				//Texture width and height must be even powers of 2 for the following code to work
				m_texConvertCtx.texWidthPow2 = (texWidth & 0x1) | (texWidth >> 1);
				m_texConvertCtx.texHeightPow2 = (texHeight & 0x1) | (texHeight >> 1);

				//Unlock texture level
				if (FAILED(pBind->pTexObj->UnlockRect(Level))) {
					appErrorf(TEXT("Texture unlock failed"));
				}
			}
		}
		unguard;

		unclockFast(ImageCycles);

		//Restore palette index 0 for masked paletted textures
		if (Info.Palette && (PolyFlags & PF_Masked)) {
			Info.Palette[0] = paletteIndex0;
		}

		// Cleanup.
		if (SupportsLazyTextures) {
			Info.Unload();
		}
	}

	//Set texture filter parameters
	SetTexFilter(texNum, pBind->texParams.filter);

	unguard;
}

BYTE UD3D8RenderDevice::SetZTestMode(BYTE Mode)
{
	if (m_LastZMode == Mode || Mode > 6)
		return Mode;

	EndBuffering();

	static DWORD ModeList[] = { D3DCMP_LESS, D3DCMP_EQUAL, D3DCMP_LESSEQUAL, D3DCMP_GREATER, D3DCMP_GREATEREQUAL, D3DCMP_NOTEQUAL, D3DCMP_ALWAYS };
	m_d3dDevice->SetRenderState(D3DRS_ZFUNC, ModeList[Mode]);
	BYTE Prev = m_LastZMode;
	m_LastZMode = Mode;
	return Prev;
}

void UD3D8RenderDevice::CacheTextureInfo(FCachedTexture *pBind, const FTextureInfo &Info, DWORD PolyFlags) {
#if 0
{
	dout << L"utd3d8r: CacheId = "
		<< HexString(32, (DWORD)((QWORD)Info.CacheID >> 32)) << L":"
		<< HexString(32, (DWORD)((QWORD)Info.CacheID & 0xFFFFFFFF)) << std::endl;
}
{
	const UTexture *pTexture = Info.Texture;
	const TCHAR *pName = pTexture->GetFullName();
	if (pName) dout << L"utd3d8r: TexName = " << pName << std::endl;
}
{
	dout << L"utd3d8r: NumMips = " << Info.NumMips << std::endl;
}
{
	unsigned int u;

	dout << L"utd3d8r: ZPBindTree Size = ";
	for (u = 0; u < NUM_CTTree_TREES; u++) {
		dout << m_zeroPrefixBindTrees[u].calc_size();
		if (u != (NUM_CTTree_TREES - 1)) dout << L", ";
	}
	dout << std::endl;

	dout << L"utd3d8r: NZPBindTree Size = ";
	for (u = 0; u < NUM_CTTree_TREES; u++) {
		dout << m_nonZeroPrefixBindTrees[u].calc_size();
		if (u != (NUM_CTTree_TREES - 1)) dout << L", ";
	}
	dout << std::endl;
}
#endif

	// Figure out scaling info for the texture.
	DWORD texFlags = 0;
	INT BaseMip = 0;
	INT UBits = Info.Mips[0]->UBits;
	INT VBits = Info.Mips[0]->VBits;
	INT UCopyBits = 0;
	INT VCopyBits = 0;
	if ((UBits - VBits) > MaxLogUOverV) {
		VCopyBits += (UBits - VBits) - MaxLogUOverV;
		VBits = UBits - MaxLogUOverV;
	}
	if ((VBits - UBits) > MaxLogVOverU) {
		UCopyBits += (VBits - UBits) - MaxLogVOverU;
		UBits = VBits - MaxLogVOverU;
	}
	if (UBits < MinLogTextureSize) {
		UCopyBits += MinLogTextureSize - UBits;
		UBits += MinLogTextureSize - UBits;
	}
	if (VBits < MinLogTextureSize) {
		VCopyBits += MinLogTextureSize - VBits;
		VBits += MinLogTextureSize - VBits;
	}
	if (UBits > MaxLogTextureSize) {
		BaseMip += UBits - MaxLogTextureSize;
		VBits -= UBits - MaxLogTextureSize;
		UBits = MaxLogTextureSize;
		if (VBits < 0) {
			VCopyBits = -VBits;
			VBits = 0;
		}
	}
	if (VBits > MaxLogTextureSize) {
		BaseMip += VBits - MaxLogTextureSize;
		UBits -= VBits - MaxLogTextureSize;
		VBits = MaxLogTextureSize;
		if (UBits < 0) {
			UCopyBits = -UBits;
			UBits = 0;
		}
	}

	pBind->BaseMip = BaseMip;
	pBind->MaxLevel = Min(UBits, VBits) - MinLogTextureSize;
	pBind->UBits = UBits;
	pBind->VBits = VBits;

	pBind->UMult = 1.0f / (Info.UScale * (Info.USize << UCopyBits));
	pBind->VMult = 1.0f / (Info.VScale * (Info.VSize << VCopyBits));

	pBind->UClampVal = Info.UClamp - 1;
	pBind->VClampVal = Info.VClamp - 1;

	//Check for texture that does not require clamping
	//No clamp required if ((Info.UClamp == Info.USize) & (Info.VClamp == Info.VSize))
	if (((Info.UClamp ^ Info.USize) | (Info.VClamp ^ Info.VSize)) == 0) {
		texFlags |= TEX_FLAG_NO_CLAMP;
	}


	//Determine texture type
	//PolyFlags PF_Masked cannot change if existing texture is updated as it caches texture type information here
	bool paletted = false;
	if (UsePalette && Info.Palette) {
		paletted = true;
		if (!UseAlphaPalette) {
			if ((PolyFlags & PF_Masked) || (Info.Palette[0].A != 255)) {
				paletted = false;
			}
		}
	}

	pBind->texType = TEX_TYPE_NONE;
     if (SupportsTC) {
          switch (Info.Format) {
          case TEXF_DXT1:
               if (TexDXT1ToDXT3 && (!(PolyFlags & PF_Masked))) {
                    pBind->texType = TEX_TYPE_COMPRESSED_DXT1_TO_DXT3;
                    pBind->texFormat = D3DFMT_DXT3;
               }
               else {
                    pBind->texType = TEX_TYPE_COMPRESSED_DXT1;
                    pBind->texFormat = D3DFMT_DXT1;
               }
               break;

          case TEXF_DXT3:
               pBind->texType = TEX_TYPE_COMPRESSED_DXT3;
               pBind->texFormat = D3DFMT_DXT3;
               break;

          case TEXF_DXT5:
               pBind->texType = TEX_TYPE_COMPRESSED_DXT5;
               pBind->texFormat = D3DFMT_DXT5;
               break;

          default:
               ;
          }
     }
    if (pBind->texType != TEX_TYPE_NONE) {
          //Using compressed texture
     }
	else if (paletted) {
		pBind->texType = TEX_TYPE_PALETTED;
		pBind->texFormat = D3DFMT_P8;
	}
	else if (Info.Palette) {
		pBind->texType = TEX_TYPE_HAS_PALETTE;
		pBind->texFormat = D3DFMT_A8R8G8B8;
		//Check if texture should be 16-bit
		if (PolyFlags & PF_Memorized) {
			pBind->texFormat = (PolyFlags & PF_Masked) ? D3DFMT_A1R5G5B5 : ((Use565Textures) ? D3DFMT_R5G6B5 : D3DFMT_X1R5G5B5);
		}
	}
	else {
		pBind->texType = TEX_TYPE_NORMAL;
		if (texFlags & TEX_FLAG_NO_CLAMP) {
			pBind->pConvertBGRA7777 = (Info.Format == TEXF_BGRA8) ? &UD3D8RenderDevice::ConvertBGRA8888_NoClamp : &UD3D8RenderDevice::ConvertBGRA7777_BGRA8888_NoClamp;
		}
		else {
			pBind->pConvertBGRA7777 = (Info.Format == TEXF_BGRA8) ? &UD3D8RenderDevice::ConvertBGRA8888 : &UD3D8RenderDevice::ConvertBGRA7777_BGRA8888;
		}
		pBind->texFormat = D3DFMT_A8R8G8B8;
	}

	return;
}


void UD3D8RenderDevice::ConvertDXT1_DXT1(const FMipmapBase *Mip, INT Level) {
	const DWORD *pSrc = (DWORD *)Mip->DataPtr;
	DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
	DWORD UBlocks = 1U << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level - 2);
	DWORD VBlocks = 1U << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level - 2);

	for (DWORD v = 0; v < VBlocks; v++) {
		DWORD *pDest = pTex;
		for (DWORD u = 0; u < UBlocks; u++) {
			//Copy one block
			pDest[0] = pSrc[0];
			pDest[1] = pSrc[1];
			pSrc += 2;
			pDest += 2;
		}
		pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	}

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertDXT1_DXT1 = " << si++ << std::endl;
	}
#endif
}

void UD3D8RenderDevice::ConvertDXT1_DXT3(const FMipmapBase *Mip, INT Level) {
	const DWORD *pSrc = (DWORD *)Mip->DataPtr;
	DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
	DWORD UBlocks = 1U << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level - 2);
	DWORD VBlocks = 1U << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level - 2);

	for (DWORD v = 0; v < VBlocks; v++) {
		DWORD *pDest = pTex;
		for (DWORD u = 0; u < UBlocks; u++) {
			//Copy one block
			pDest[0] = 0xFFFFFFFF;
			pDest[1] = 0xFFFFFFFF;
			pDest[2] = pSrc[0];
			pDest[3] = pSrc[1];
			pSrc += 2;
			pDest += 4;
		}
		pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	}

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertDXT1_DXT3 = " << si++ << std::endl;
	}
#endif
}
void UD3D8RenderDevice::ConvertDXT35_DXT35(const FMipmapBase *Mip, INT Level) {
     const DWORD *pSrc = (DWORD *)Mip->DataPtr;
     DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
     DWORD UBlocks = 1U << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level - 2);
     DWORD VBlocks = 1U << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level - 2);

     for (DWORD v = 0; v < VBlocks; v++) {
          DWORD *pDest = pTex;
          for (DWORD u = 0; u < UBlocks; u++) {
               //Copy one block
               pDest[0] = pSrc[0];
               pDest[1] = pSrc[1];
               pDest[2] = pSrc[2];
               pDest[3] = pSrc[3];
               pSrc += 4;
               pDest += 4;
          }
          pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
     }

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
     {
          static int si;
          dout << L"utd3d9r: ConvertDXT35_DXT35 = " << si++ << std::endl;
     }
#endif
}

/*void UD3D8RenderDevice::ConvertP8_P8(const FMipmapBase *Mip, INT Level) {
	BYTE* Ptr = (BYTE*)m_texConvertCtx.pCompose;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			*Ptr++ = Base[j & UMask];
		} while ((j += ij_inc) < j_stop);
	} while ((i += ij_inc) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_P8 = " << si++ << std::endl;
	}
#endif
}*/

/*void UD3D8RenderDevice::ConvertP8_P8_NoStep(const FMipmapBase *Mip, INT Level) {
	BYTE* Ptr = (BYTE*)m_texConvertCtx.pCompose;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT i_stop = m_texConvertCtx.texHeightPow2;
	INT j_stop = m_texConvertCtx.texWidthPow2;
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			*Ptr++ = Base[j & UMask];
		} while ((j += 1) < j_stop);
	} while ((i += 1) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_P8_NoStep = " << si++ << std::endl;
	}
#endif
}*/

void UD3D8RenderDevice::ConvertP8_RGBA8888(const FMipmapBase *Mip, const FColor *Palette, INT Level) {
	DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			DWORD dwColor = GET_COLOR_DWORD(Palette[Base[j & UMask]]);
			pTex[j] = (dwColor & 0xFF00FF00) | ((dwColor >> 16) & 0xFF) | ((dwColor << 16) & 0xFF0000);
		} while ((j += ij_inc) < j_stop);
		pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_RGBA8888 = " << si++ << std::endl;
	}
#endif
}

void UD3D8RenderDevice::ConvertP8_RGBA8888_NoStep(const FMipmapBase *Mip, const FColor *Palette, INT Level) {
	DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT i_stop = m_texConvertCtx.texHeightPow2;
	INT j_stop = m_texConvertCtx.texWidthPow2;
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			DWORD dwColor = GET_COLOR_DWORD(Palette[Base[j & UMask]]);
			pTex[j] = (dwColor & 0xFF00FF00) | ((dwColor >> 16) & 0xFF) | ((dwColor << 16) & 0xFF0000);
		} while ((j += 1) < j_stop);
		pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += 1) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_RGBA8888_NoStep = " << si++ << std::endl;
	}
#endif
}

void FASTCALL UD3D8RenderDevice::ConvertP8_RGB565(const FMipmapBase *Mip, const FColor *Palette, INT Level) {
	_WORD *pTex = (_WORD *)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			DWORD dwColor = GET_COLOR_DWORD(Palette[Base[j & UMask]]);
			pTex[j] = ((dwColor >> 19) & 0x001F) | ((dwColor >> 5) & 0x07E0) | ((dwColor << 8) & 0xF800);
		} while ((j += ij_inc) < j_stop);
		pTex = (WORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_RGBA565 = " << si++ << std::endl;
	}
#endif
}

void FASTCALL UD3D8RenderDevice::ConvertP8_RGB565_NoStep(const FMipmapBase *Mip, const FColor *Palette, INT Level) {
	_WORD *pTex = (_WORD *)m_texConvertCtx.lockRect.pBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT i_stop = m_texConvertCtx.texHeightPow2;
	INT j_stop = m_texConvertCtx.texWidthPow2;
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			DWORD dwColor = GET_COLOR_DWORD(Palette[Base[j & UMask]]);
			pTex[j] = ((dwColor >> 19) & 0x001F) | ((dwColor >> 5) & 0x07E0) | ((dwColor << 8) & 0xF800);
		} while ((j += 1) < j_stop);
		pTex = (_WORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += 1) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_RGBA565_NoStep = " << si++ << std::endl;
	}
#endif
}

void FASTCALL UD3D8RenderDevice::ConvertP8_RGBA5551(const FMipmapBase *Mip, const FColor *Palette, INT Level) {
	_WORD *pTex = (_WORD *)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			DWORD dwColor = GET_COLOR_DWORD(Palette[Base[j & UMask]]);
			pTex[j] = ((dwColor >> 19) & 0x001F) | ((dwColor >> 6) & 0x03E0) | ((dwColor << 7) & 0x7C00) | ((dwColor >> 16) & 0x8000);
		} while ((j += ij_inc) < j_stop);
		pTex = (WORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_RGBA5551 = " << si++ << std::endl;
	}
#endif
}

void FASTCALL UD3D8RenderDevice::ConvertP8_RGBA5551_NoStep(const FMipmapBase *Mip, const FColor *Palette, INT Level) {
	_WORD *pTex = (_WORD *)m_texConvertCtx.lockRect.pBits;
	DWORD UMask = Mip->USize - 1;
	DWORD VMask = Mip->VSize - 1;
	INT i_stop = m_texConvertCtx.texHeightPow2;
	INT j_stop = m_texConvertCtx.texWidthPow2;
	INT i = 0;
	do { //i_stop always >= 1
		BYTE* Base = (BYTE*)Mip->DataPtr + (i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1
			DWORD dwColor = GET_COLOR_DWORD(Palette[Base[j & UMask]]);
			pTex[j] = ((dwColor >> 19) & 0x001F) | ((dwColor >> 6) & 0x03E0) | ((dwColor << 7) & 0x7C00) | ((dwColor >> 16) & 0x8000);
		} while ((j += 1) < j_stop);
		pTex = (_WORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += 1) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertP8_RGBA5551_NoStep = " << si++ << std::endl;
	}
#endif
}

void UD3D8RenderDevice::ConvertBGRA7777_BGRA8888(const FMipmapBase *Mip, INT Level) {
	DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD VMask = Mip->VSize - 1;
	DWORD VClampVal = m_texConvertCtx.pBind->VClampVal;
	DWORD UMask = Mip->USize - 1;
	DWORD UClampVal = m_texConvertCtx.pBind->UClampVal;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		FColor* Base = (FColor*)Mip->DataPtr + Min<DWORD>(i & VMask, VClampVal) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1;
			DWORD dwColor = GET_COLOR_DWORD(Base[Min<DWORD>(j & UMask, UClampVal)]);
			pTex[j] = dwColor * 2; // because of 7777
		} while ((j += ij_inc) < j_stop);
		pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertBGRA7777_BGRA8888 = " << si++ << std::endl;
	}
#endif
}

void UD3D8RenderDevice::ConvertBGRA7777_BGRA8888_NoClamp(const FMipmapBase *Mip, INT Level) {
	DWORD *pTex = (DWORD *)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD VMask = Mip->VSize - 1;
	DWORD UMask = Mip->USize - 1;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		FColor* Base = (FColor*)Mip->DataPtr + (DWORD)(i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1;
			DWORD dwColor = GET_COLOR_DWORD(Base[(DWORD)(j & UMask)]);
			pTex[j] = dwColor * 2; // because of 7777
		} while ((j += ij_inc) < j_stop);
		pTex = (DWORD *)((BYTE *)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);

#ifdef UTGLR_DEBUG_SHOW_TEX_CONVERT_COUNTS
	{
		static int si;
		dout << L"utd3d8r: ConvertBGRA7777_BGRA8888_NoClamp = " << si++ << std::endl;
	}
#endif
}

void UD3D8RenderDevice::ConvertBGRA8888(const FMipmapBase* Mip, INT Level) {
	DWORD* pTex = (DWORD*)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD VMask = Mip->VSize - 1;
	DWORD VClampVal = m_texConvertCtx.pBind->VClampVal;
	DWORD UMask = Mip->USize - 1;
	DWORD UClampVal = m_texConvertCtx.pBind->UClampVal;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		FColor* Base = (FColor*)Mip->DataPtr + Min<DWORD>(i & VMask, VClampVal) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1;
			pTex[j] = GET_COLOR_DWORD(Base[Min<DWORD>(j & UMask, UClampVal)]);
		} while ((j += ij_inc) < j_stop);
		pTex = (DWORD*)((BYTE*)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);
}

void UD3D8RenderDevice::ConvertBGRA8888_NoClamp(const FMipmapBase* Mip, INT Level) {
	DWORD* pTex = (DWORD*)m_texConvertCtx.lockRect.pBits;
	INT StepBits = m_texConvertCtx.stepBits;
	DWORD VMask = Mip->VSize - 1;
	DWORD UMask = Mip->USize - 1;
	INT ij_inc = 1 << StepBits;
	INT i_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->VBits - Level + StepBits);
	INT j_stop = 1 << Max(0, (INT)m_texConvertCtx.pBind->UBits - Level + StepBits);
	INT i = 0;
	do { //i_stop always >= 1
		FColor* Base = (FColor*)Mip->DataPtr + (DWORD)(i & VMask) * Mip->USize;
		INT j = 0;
		do { //j_stop always >= 1;
			pTex[j] = GET_COLOR_DWORD(Base[(DWORD)(j & UMask)]);
		} while ((j += ij_inc) < j_stop);
		pTex = (DWORD*)((BYTE*)pTex + m_texConvertCtx.lockRect.Pitch);
	} while ((i += ij_inc) < i_stop);
}

void UD3D8RenderDevice::SetBlendNoCheck(DWORD blendFlags) {
	guardSlow(UD3D8RenderDevice::SetBlend);

	// Detect changes in the blending modes.
	DWORD Xor = m_curBlendFlags ^ blendFlags;

	//Update main copy of current blend flags early
	m_curBlendFlags = blendFlags;

#ifdef UTGLR_RUNE_BUILD
	const DWORD GL_BLEND_FLAG_BITS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_AlphaBlend;
#elif  UTGLR_UNREAL_227_BUILD
	const DWORD GL_BLEND_FLAG_BITS = PF_Translucent | PF_Modulated | PF_Highlighted | PF_AlphaBlend;
#else
	const DWORD GL_BLEND_FLAG_BITS = PF_Translucent | PF_Modulated | PF_Highlighted;
#endif
	DWORD relevantBlendFlagBits = GL_BLEND_FLAG_BITS | m_smoothMaskedTexturesBit;
	if (Xor & (relevantBlendFlagBits)) {
		if (!(blendFlags & (relevantBlendFlagBits))) {
			m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		}
		else {
			if (blendFlags & PF_Translucent) {
				m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
			}
			else if (blendFlags & PF_Modulated) {
				m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
				m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
			}
			else if (blendFlags & PF_Highlighted) {
				m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
#ifdef UTGLR_RUNE_BUILD
			else if (blendFlags & PF_AlphaBlend) {
				m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
#elif  UTGLR_UNREAL_227_BUILD
			else if (blendFlags & PF_AlphaBlend) {
				m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
#endif
			else if (blendFlags & PF_Masked) {
				m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
		}
	}
	if (Xor & PF_Masked) {
		m_d3dDevice->SetRenderState(D3DRS_ALPHAREF, 127);
		m_d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, ((blendFlags & PF_Masked) == 0) ? FALSE : TRUE);
	}
	if (Xor & PF_AlphaBlend) {
		m_d3dDevice->SetRenderState(D3DRS_ALPHAREF, 2);
		m_d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, ((blendFlags & PF_AlphaBlend) == 0) ? FALSE : TRUE);
	}
	if (Xor & PF_Invisible) {
		DWORD colorEnableBits = ((blendFlags & PF_Invisible) == 0) ? D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED : 0;
		m_d3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorEnableBits);
	}
	if (Xor & PF_Occlude) {
		DWORD flag = ((blendFlags & PF_Occlude) == 0) ? FALSE : TRUE;
		m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, flag);
	}
	if (Xor & PF_RenderFog) {
		DWORD flag = ((blendFlags & PF_RenderFog) == 0) ? FALSE : TRUE;
		m_d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, flag);
	}

	unguardSlow;
}

//This function will initialize or invalidate the texture environment state
//The current architecture allows both operations to be done in the same way
void UD3D8RenderDevice::InitOrInvalidateTexEnvState(void) {
	INT TMU;

	//For initialization, flags for all texture units are cleared
	//For initialization, first texture unit is modulated by default rather
	//than disabled, but priority bit encoding of flags will prevent problems
	//from the mismatch and will only result in one extra state update
	//For invalidation, flags for all texture units are also cleared as it is
	//fast enough and has no potential outside interaction side effects
	for (TMU = 0; TMU < MAX_TMUNITS; TMU++) {
		m_curTexEnvFlags[TMU] = 0;
	}

	//Set TexEnv 0 to modulated by default
	SetTexEnv(0, PF_Modulated);

	return;
}

void UD3D8RenderDevice::SetTexLODBiasState(INT TMUnits) {
	INT TMU;

	//Set texture LOD bias for all texture units
	for (TMU = 0; TMU < TMUnits; TMU++) {
		float fParam;

		//Set texture LOD bias
		fParam = LODBias;
		m_d3dDevice->SetTextureStageState(TMU, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&fParam);
	}

	return;
}

void UD3D8RenderDevice::SetTexMaxAnisotropyState(INT TMUnits) {
	INT TMU;

	//Set maximum level of anisotropy for all texture units
	for (TMU = 0; TMU < TMUnits; TMU++) {
		m_d3dDevice->SetTextureStageState(TMU, D3DTSS_MAXANISOTROPY, MaxAnisotropy);
	}

	return;
}

void UD3D8RenderDevice::SetTexEnvNoCheck(DWORD texUnit, DWORD texEnvFlags) {
	guardSlow(UD3D8RenderDevice::SetTexEnv);

	//Update current tex env flags early as there are no subsequent dependencies
	m_curTexEnvFlags[texUnit] = texEnvFlags;

	//Mark the texture unit as enabled
	m_texEnableBits |= 1U << texUnit;

	if (texEnvFlags & PF_Modulated) {
		D3DTEXTUREOP texOp;

		if ((texEnvFlags & PF_FlatShaded) || (texUnit != 0) && !OneXBlending) {
			texOp = D3DTOP_MODULATE2X;
		}
		else {
			texOp = D3DTOP_MODULATE;
		}

		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLOROP, texOp);
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

//		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}
	else if (texEnvFlags & PF_Memorized) {
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

//		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	}
	else if (texEnvFlags & PF_Highlighted) {
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLOROP, D3DTOP_MODULATEINVALPHA_ADDCOLOR);
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

//		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_d3dDevice->SetTextureStageState(texUnit, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	unguardSlow;
}


void FASTCALL UD3D8RenderDevice::SetTexFilterNoCheck(DWORD texNum, BYTE texFilterParams) {
	guardSlow(UD3D8RenderDevice::SetTexFilter);

	BYTE texFilterParamsXor = m_curTexStageParams[texNum].filter ^ texFilterParams;

	//Update main copy of current tex filter params early
	m_curTexStageParams[texNum].filter = texFilterParams;

	if (texFilterParamsXor & CT_MIN_FILTER_MASK) {
		D3DTEXTUREFILTERTYPE texFilterType = D3DTEXF_POINT;

		switch (texFilterParams & CT_MIN_FILTER_MASK) {
		case CT_MIN_FILTER_POINT: texFilterType = D3DTEXF_POINT; break;
		case CT_MIN_FILTER_LINEAR: texFilterType = D3DTEXF_LINEAR; break;
		case CT_MIN_FILTER_ANISOTROPIC: texFilterType = D3DTEXF_ANISOTROPIC; break;
		default:
			;
		}

		m_d3dDevice->SetTextureStageState(texNum, D3DTSS_MINFILTER, texFilterType);
	}
	if (texFilterParamsXor & CT_MIP_FILTER_MASK) {
		D3DTEXTUREFILTERTYPE texFilterType = D3DTEXF_NONE;

		switch (texFilterParams & CT_MIP_FILTER_MASK) {
		case CT_MIP_FILTER_NONE: texFilterType = D3DTEXF_NONE; break;
		case CT_MIP_FILTER_POINT: texFilterType = D3DTEXF_POINT; break;
		case CT_MIP_FILTER_LINEAR: texFilterType = D3DTEXF_LINEAR; break;
		default:
			;
		}

		m_d3dDevice->SetTextureStageState(texNum, D3DTSS_MIPFILTER, texFilterType);
	}
	if (texFilterParamsXor & CT_MAG_FILTER_LINEAR_NOT_POINT_BIT) {
		m_d3dDevice->SetTextureStageState(texNum, D3DTSS_MAGFILTER, (texFilterParams & CT_MAG_FILTER_LINEAR_NOT_POINT_BIT) ? D3DTEXF_LINEAR : D3DTEXF_POINT);
	}
	if (texFilterParamsXor & CT_ADDRESS_U_CLAMP) {
		D3DTEXTUREADDRESS texAddressMode = (texFilterParams & CT_ADDRESS_U_CLAMP) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
		m_d3dDevice->SetTextureStageState(texNum, D3DTSS_ADDRESSU, texAddressMode);
	}
	if (texFilterParamsXor & CT_ADDRESS_V_CLAMP) {
		D3DTEXTUREADDRESS texAddressMode = (texFilterParams & CT_ADDRESS_V_CLAMP) ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
		m_d3dDevice->SetTextureStageState(texNum, D3DTSS_ADDRESSV, texAddressMode);
	}
	unguardSlow;
}

void UD3D8RenderDevice::SetStreamStateNoCheck(DWORD streamShader) {
	HRESULT hResult;

	//Set stream definition
	hResult = m_d3dDevice->SetVertexShader(streamShader);
	if (FAILED(hResult)) {
		appErrorf(TEXT("SetVertexShader on stream definition failed"));
	}

	//Save new shader id
	m_curVertexShader = streamShader;

	return;
}


void UD3D8RenderDevice::SetAAStateNoCheck(bool AAEnable) {
	//Save new AA state
	m_curAAEnable = AAEnable;

	m_AASwitchCount++;

	//Set new AA state
	m_d3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, (AAEnable) ? TRUE : FALSE);

	return;
}


bool UD3D8RenderDevice::InitializeVertexPrograms(void) {
	bool initOk = true;

	//Create vertex programs if not already created

	//Complex surface single texture
	if (!m_csNTextureVertexShaderCreated[0]) {
		if (LoadVertexProgram(g_oneColorStreamDef, g_csSingleTextureVertexShaderDef, &m_csNTextureVertexShader[0], TEXT("Complex surface single texture"))) {
			m_csNTextureVertexShaderCreated[0] = true;
		}
		else {
			initOk = false;
		}
	}

	//Complex surface double texture
	if (TMUnits >= 2) {
		if (!m_csNTextureVertexShaderCreated[1]) {
			if (LoadVertexProgram(g_oneColorStreamDef, g_csDoubleTextureVertexShaderDef, &m_csNTextureVertexShader[1], TEXT("Complex surface dual texture"))) {
				m_csNTextureVertexShaderCreated[1] = true;
			}
			else {
				initOk = false;
			}
		}
	}

	//Complex surface triple texture
	if (TMUnits >= 3) {
		if (!m_csNTextureVertexShaderCreated[2]) {
			if (LoadVertexProgram(g_oneColorStreamDef, g_csTripleTextureVertexShaderDef, &m_csNTextureVertexShader[2], TEXT("Complex surface triple texture"))) {
				m_csNTextureVertexShaderCreated[2] = true;
			}
			else {
				initOk = false;
			}
		}
	}

	//Complex surface quad texture
	if (TMUnits >= 4) {
		if (!m_csNTextureVertexShaderCreated[3]) {
			if (LoadVertexProgram(g_oneColorStreamDef, g_csQuadTextureVertexShaderDef, &m_csNTextureVertexShader[3], TEXT("Complex surface quad texture"))) {
				m_csNTextureVertexShaderCreated[3] = true;
			}
			else {
				initOk = false;
			}
		}
	}

	//Complex surface detail alpha
	if (UseDetailAlpha) {
		if (!m_csDetailAlphaVertexShaderCreated) {
			if (LoadVertexProgram(g_oneColorStreamDef, g_csDetailAlphaVertexShaderDef, &m_csDetailAlphaVertexShader, TEXT("Complex surface detail alpha"))) {
				m_csDetailAlphaVertexShaderCreated = true;
			}
			else {
				initOk = false;
			}
		}
	}

	//Complex surface dual texture and detail texture
	if (SinglePassDetail) {
		if (!m_csDualTextureAndDetailTextureVertexShaderCreated) {
			if (LoadVertexProgram(g_oneColorStreamDef, g_csDualTextureAndDetailTextureVertexShaderDef, &m_csDualTextureAndDetailTextureVertexShader, TEXT("Complex surface dual texture and detail texture"))) {
				m_csDualTextureAndDetailTextureVertexShaderCreated = true;
			}
			else {
				initOk = false;
			}
		}
	}

	//Standard single texture
	if (!m_standardSingleTextureVertexShaderCreated) {
		if (LoadVertexProgram(g_oneColorStreamDef, g_standardSingleTextureVertexShaderDef, &m_standardSingleTextureVertexShader, TEXT("Standard single texture"))) {
			m_standardSingleTextureVertexShaderCreated = true;
		}
		else {
			initOk = false;
		}
	}

	return initOk;
}

bool UD3D8RenderDevice::LoadVertexProgram(const DWORD *pDeclaration, const DWORD *pFunction, DWORD *pHandle, const TCHAR *pName) {
	HRESULT hResult;

	if (DebugBit(DEBUG_BIT_BASIC)) {
		dout << TEXT("utd3d8r: Loading vertex program \"") << pName << TEXT("\"") << std::endl;
	}

	hResult = m_d3dDevice->CreateVertexShader(pDeclaration, pFunction, pHandle, m_vertexShaderUsage);
	if (FAILED(hResult)) {
		if (DebugBit(DEBUG_BIT_BASIC)) {
			dout << TEXT("utd3d8r: Vertex program load error") << std::endl;
		}

		return false;
	}

	return true;
}

//Attempts to initializes vertex program mode
//Safe to call multiple times as already created vertex programs will not be recreated
void UD3D8RenderDevice::TryInitializeVertexProgramMode(void) {
	//Initialize vertex programs
	if (!InitializeVertexPrograms()) {
		//Shutdown vertex program mode
		ShutdownVertexProgramMode();

		//Disable vertex program mode
		DCV.UseVertexProgram = 0;
		UseVertexProgram = 0;
		PL_UseVertexProgram = 0;

		if (DebugBit(DEBUG_BIT_BASIC)) dout << TEXT("utd3d8r: Vertex program initialization failed") << std::endl;
	}

	return;
}

//Shuts down vertex program mode if it is active
//Freeing the vertex program names takes care of releasing resources
//Safe to call even if vertex program mode is not supported or was never initialized
void UD3D8RenderDevice::ShutdownVertexProgramMode(void) {
	//Set default stream state so that a vertex program is not current
	SetDefaultStreamState();

	//Free vertex programs if they were created

	//Complex surface single texture
	if (m_csNTextureVertexShaderCreated[0]) {
		m_d3dDevice->DeleteVertexShader(m_csNTextureVertexShader[0]);
		m_csNTextureVertexShaderCreated[0] = false;
	}

	//Complex surface double texture
	if (m_csNTextureVertexShaderCreated[1]) {
		m_d3dDevice->DeleteVertexShader(m_csNTextureVertexShader[1]);
		m_csNTextureVertexShaderCreated[1] = false;
	}

	//Complex surface triple texture
	if (m_csNTextureVertexShaderCreated[2]) {
		m_d3dDevice->DeleteVertexShader(m_csNTextureVertexShader[2]);
		m_csNTextureVertexShaderCreated[2] = false;
	}

	//Complex surface quad texture
	if (m_csNTextureVertexShaderCreated[3]) {
		m_d3dDevice->DeleteVertexShader(m_csNTextureVertexShader[3]);
		m_csNTextureVertexShaderCreated[3] = false;
	}

	//Complex surface detail alpha
	if (m_csDetailAlphaVertexShaderCreated) {
		m_d3dDevice->DeleteVertexShader(m_csDetailAlphaVertexShader);
		m_csDetailAlphaVertexShaderCreated = false;
	}

	//Complex surface dual texture and detail texture
	if (m_csDualTextureAndDetailTextureVertexShaderCreated) {
		m_d3dDevice->DeleteVertexShader(m_csDualTextureAndDetailTextureVertexShader);
		m_csDualTextureAndDetailTextureVertexShaderCreated = false;
	}

	//Standard single texture
	if (m_standardSingleTextureVertexShaderCreated) {
		m_d3dDevice->DeleteVertexShader(m_standardSingleTextureVertexShader);
		m_standardSingleTextureVertexShaderCreated = false;
	}

	return;
}


void UD3D8RenderDevice::SetProjectionStateNoCheck(bool requestNearZRangeHackProjection) {
	float left, right, bottom, top, zNear, zFar;
	float invRightMinusLeft, invTopMinusBottom, invNearMinusFar;
	D3DMATRIX d3dProj;

	//Save new Z range hack projection state
	m_nearZRangeHackProjectionActive = requestNearZRangeHackProjection;

	//Set default zNearVal
	FLOAT zNearVal = 0.5f;

	FLOAT zScaleVal = 1.0f;
	if (requestNearZRangeHackProjection) {
#ifdef UTGLR_DEBUG_Z_RANGE_HACK_WIREFRAME
		m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
#endif

		zScaleVal = 0.125f;
		zNearVal = 0.5;
	}
	else {
#ifdef UTGLR_DEBUG_Z_RANGE_HACK_WIREFRAME
		m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

		if (m_useZRangeHack) {
			zNearVal = 4.0f;
		}
	}

	left = -m_RProjZ * zNearVal;
	right = +m_RProjZ * zNearVal;
	bottom = -m_Aspect*m_RProjZ * zNearVal;
	top = +m_Aspect*m_RProjZ * zNearVal;
	zNear = 1.0f * zNearVal;
	zFar = 65536.0f; // Sunspire fix
	if (requestNearZRangeHackProjection) {
		zFar = 4096.0f;
	}

	invRightMinusLeft = 1.0f / (right - left);
	invTopMinusBottom = 1.0f / (top - bottom);
	invNearMinusFar = 1.0f / (zNear - zFar);

	d3dProj.m[0][0] = 2.0f * zNear * invRightMinusLeft;
	d3dProj.m[0][1] = 0.0f;
	d3dProj.m[0][2] = 0.0f;
	d3dProj.m[0][3] = 0.0f;

	d3dProj.m[1][0] = 0.0f;
	d3dProj.m[1][1] = 2.0f * zNear * invTopMinusBottom;
	d3dProj.m[1][2] = 0.0f;
	d3dProj.m[1][3] = 0.0f;

	d3dProj.m[2][0] = 0.0f;
	d3dProj.m[2][1] = 0.0f;
	d3dProj.m[2][2] = zScaleVal * (zFar * invNearMinusFar);
	d3dProj.m[2][3] = -1.0f;

	d3dProj.m[3][0] = 0.0f;
	d3dProj.m[3][1] = 0.0f;
	d3dProj.m[3][2] = zScaleVal * zScaleVal * (zNear * zFar * invNearMinusFar);
	d3dProj.m[3][3] = 0.0f;

	m_d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dProj);

	if (UseVertexProgram) {
		FLOAT vsTransMatrix[16];

		//Transpose and scale by -1y and -1z
		vsTransMatrix[0]  = d3dProj.m[0][0];
		vsTransMatrix[1]  = -d3dProj.m[1][0];
		vsTransMatrix[2]  = -d3dProj.m[2][0];
		vsTransMatrix[3]  = d3dProj.m[3][0];
		vsTransMatrix[4]  = d3dProj.m[0][1];
		vsTransMatrix[5]  = -d3dProj.m[1][1];
		vsTransMatrix[6]  = -d3dProj.m[2][1];
		vsTransMatrix[7]  = d3dProj.m[3][1];
		vsTransMatrix[8]  = d3dProj.m[0][2];
		vsTransMatrix[9]  = -d3dProj.m[1][2];
		vsTransMatrix[10] = -d3dProj.m[2][2];
		vsTransMatrix[11] = d3dProj.m[3][2];
		vsTransMatrix[12] = d3dProj.m[0][3];
		vsTransMatrix[13] = -d3dProj.m[1][3];
		vsTransMatrix[14] = -d3dProj.m[2][3];
		vsTransMatrix[15] = d3dProj.m[3][3];

		m_d3dDevice->SetVertexShaderConstant(0, vsTransMatrix, 4);
	}

	return;
}

void UD3D8RenderDevice::SetOrthoProjection(void) {
	float left, right, bottom, top, zNear, zFar;
	float invNearMinusFar;
	D3DMATRIX d3dProj;

	//Save new Z range hack projection state
	m_nearZRangeHackProjectionActive = false;

	left = -m_RProjZ * 0.5f;
	right = +m_RProjZ * 0.5f;
	bottom = -m_Aspect*m_RProjZ * 0.5f;
	top = +m_Aspect*m_RProjZ * 0.5f;
	zNear = 1.0f * 0.5f;
	zFar = 49152.0f;

	invNearMinusFar = 1.0f / (zNear - zFar);

	d3dProj.m[0][0] = 1.0f / (right - left);
	d3dProj.m[0][1] = 0.0f;
	d3dProj.m[0][2] = 0.0f;
	d3dProj.m[0][3] = 0.0f;

	d3dProj.m[1][0] = 0.0f;
	d3dProj.m[1][1] = 1.0f / (top - bottom);
	d3dProj.m[1][2] = 0.0f;
	d3dProj.m[1][3] = 0.0f;

	d3dProj.m[2][0] = 0.0f;
	d3dProj.m[2][1] = 0.0f;
	d3dProj.m[2][2] = 1.0f * invNearMinusFar;
	d3dProj.m[2][3] = 0.0f;

	d3dProj.m[3][0] = 0.0f;
	d3dProj.m[3][1] = 0.0f;
	d3dProj.m[3][2] = zNear * invNearMinusFar;
	d3dProj.m[3][3] = 1.0f;

	m_d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dProj);

	if (UseVertexProgram) {
		FLOAT vsTransMatrix[16];

		//Transpose and scale by -1y and -1z
		vsTransMatrix[0]  = d3dProj.m[0][0];
		vsTransMatrix[1]  = -d3dProj.m[1][0];
		vsTransMatrix[2]  = -d3dProj.m[2][0];
		vsTransMatrix[3]  = d3dProj.m[3][0];
		vsTransMatrix[4]  = d3dProj.m[0][1];
		vsTransMatrix[5]  = -d3dProj.m[1][1];
		vsTransMatrix[6]  = -d3dProj.m[2][1];
		vsTransMatrix[7]  = d3dProj.m[3][1];
		vsTransMatrix[8]  = d3dProj.m[0][2];
		vsTransMatrix[9]  = -d3dProj.m[1][2];
		vsTransMatrix[10] = -d3dProj.m[2][2];
		vsTransMatrix[11] = d3dProj.m[3][2];
		vsTransMatrix[12] = d3dProj.m[0][3];
		vsTransMatrix[13] = -d3dProj.m[1][3];
		vsTransMatrix[14] = -d3dProj.m[2][3];
		vsTransMatrix[15] = d3dProj.m[3][3];

		m_d3dDevice->SetVertexShaderConstant(0, vsTransMatrix, 4);
	}

	return;
}


void UD3D8RenderDevice::RenderPassesExec(void) {
	guard(UD3D8RenderDevice::RenderPassesExec);

	if (m_rpMasked && m_rpForceSingle && !m_rpSetDepthEqual) {
		SetZTestMode(ZTEST_Equal);
		m_rpSetDepthEqual = true;
	}

	//Call the render passes no check setup proc
	(this->*m_pRenderPassesNoCheckSetupProc)();

	m_rpTMUnits = 1;
	m_rpForceSingle = true;


	for (INT PolyNum = 0; PolyNum < m_csPolyCount; PolyNum++) {
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos + MultiDrawFirstArray[PolyNum], MultiDrawCountArray[PolyNum] - 2);
	}

#ifdef UTGLR_DEBUG_WORLD_WIREFRAME
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	SetBlend(PF_Modulated);

	for (PolyNum = 0; PolyNum < m_csPolyCount; PolyNum++) {
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos + MultiDrawFirstArray[PolyNum], MultiDrawCountArray[PolyNum] - 2);
	}

	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	//Advance vertex buffer position
	m_curVertexBufferPos += m_csPtCount;

#if 0
{
	dout << L"utd3d8r: PassCount = " << m_rpPassCount << std::endl;
}
#endif
	m_rpPassCount = 0;


	unguard;
}

void UD3D8RenderDevice::RenderPassesExec_DualTextureAndDetailTexture(FTextureInfo &DetailTextureInfo) {
	guard(UD3D8RenderDevice::RenderPassesExec_DualTextureAndDetailTexture);

	//The dual texture and detail texture path can never be executed if single pass rendering were forced earlier
	//The depth function will never need to be changed due to single pass rendering here

	m_rpPassCount = 4;

	//Call the render passes no check setup dual texture and detail texture proc
	(this->*m_pRenderPassesNoCheckSetup_DualTextureAndDetailTextureProc)(DetailTextureInfo);

	//Single pass rendering does not need to be forced here since the detail texture is always the last pass


	for (INT PolyNum = 0; PolyNum < m_csPolyCount; PolyNum++) {
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos + MultiDrawFirstArray[PolyNum], MultiDrawCountArray[PolyNum] - 2);
	}

#ifdef UTGLR_DEBUG_WORLD_WIREFRAME
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	SetBlend(PF_Modulated);

	for (PolyNum = 0; PolyNum < m_csPolyCount; PolyNum++) {
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos + MultiDrawFirstArray[PolyNum], MultiDrawCountArray[PolyNum] - 2);
	}

	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	//Advance vertex buffer position
	m_curVertexBufferPos += m_csPtCount;

#if 0
{
	dout << L"utd3d8r: PassCount = " << m_rpPassCount << std::endl;
}
#endif
	m_rpPassCount = 0;


	unguard;
}

//Must be called with (m_rpPassCount > 0)
void UD3D8RenderDevice::RenderPassesNoCheckSetup(void) {
	INT i;
	INT t;

	SetBlend(MultiPass.TMU[0].PolyFlags);

	i = 0;
	do {
		if (i != 0) {
			SetTexEnv(i, MultiPass.TMU[i].PolyFlags);
		}

		SetTexture(i, *MultiPass.TMU[i].Info, MultiPass.TMU[i].PolyFlags, MultiPass.TMU[i].PanBias);
	} while (++i < m_rpPassCount);

	//Set stream state based on number of texture units in use
	SetStreamState(m_standardNTextureStreamShader[m_rpPassCount - 1]);

	//Check for additional enabled texture units that should be disabled
	DisableSubsequentTextures(m_rpPassCount);

	//Make sure at least m_csPtCount entries are left in the vertex buffers
	if ((m_curVertexBufferPos + m_csPtCount) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor and texCoord buffers
	LockVertexColorBuffer();
	t = 0;
	do {
		LockTexCoordBuffer(t);
	} while (++t < m_rpPassCount);

	//Write vertex and color
	const FGLVertex *pSrcVertexArray = m_csVertexArray;
	FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
	DWORD rpColor = m_rpColor;
	i = m_csPtCount;
	do {
		pVertexColorArray->x = pSrcVertexArray->x;
		pVertexColorArray->y = pSrcVertexArray->y;
		pVertexColorArray->z = pSrcVertexArray->z;
		pVertexColorArray->color = rpColor;
		pSrcVertexArray++;
		pVertexColorArray++;
	} while (--i != 0);

	//Write texCoord
	t = 0;
	do {
		FLOAT UPan = TexInfo[t].UPan;
		FLOAT VPan = TexInfo[t].VPan;
		FLOAT UMult = TexInfo[t].UMult;
		FLOAT VMult = TexInfo[t].VMult;
		const FGLMapDot *pMapDot = MapDotArray;
		FGLTexCoord *pTexCoord = m_pTexCoordArray[t];

		INT ptCounter = m_csPtCount;
		do {
			pTexCoord->u = (pMapDot->u - UPan) * UMult;
			pTexCoord->v = (pMapDot->v - VPan) * VMult;

			pMapDot++;
			pTexCoord++;
		} while (--ptCounter != 0);
	} while (++t < m_rpPassCount);

	//Unlock vertexColor and texCoord buffers
	UnlockVertexColorBuffer();
	t = 0;
	do {
		UnlockTexCoordBuffer(t);
	} while (++t < m_rpPassCount);

	return;
}

//Must be called with (m_rpPassCount > 0)
void UD3D8RenderDevice::RenderPassesNoCheckSetup_VP(void) {
	INT i;
	FLOAT vsParams[MAX_TMUNITS * 4];

	SetBlend(MultiPass.TMU[0].PolyFlags);

	i = 0;
	do {
		if (i != 0) {
			SetTexEnv(i, MultiPass.TMU[i].PolyFlags);
		}

		SetTexture(i, *MultiPass.TMU[i].Info, MultiPass.TMU[i].PolyFlags, MultiPass.TMU[i].PanBias);

		vsParams[(i * 4) + 0] = TexInfo[i].UPan;
		vsParams[(i * 4) + 1] = TexInfo[i].VPan;
		vsParams[(i * 4) + 2] = TexInfo[i].UMult;
		vsParams[(i * 4) + 3] = TexInfo[i].VMult;
	} while (++i < m_rpPassCount);
	m_d3dDevice->SetVertexShaderConstant(6, vsParams, m_rpPassCount);

	//Set vertex program based on number of texture units in use
	SetStreamState(m_csNTextureVertexShader[m_rpPassCount - 1]);

	//Check for additional enabled texture units that should be disabled
	DisableSubsequentTextures(m_rpPassCount);

	//Make sure at least m_csPtCount entries are left in the vertex buffers
	if ((m_curVertexBufferPos + m_csPtCount) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor buffer
	LockVertexColorBuffer();

	//Write vertex and color
	const FGLVertex *pSrcVertexArray = m_csVertexArray;
	FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
	DWORD rpColor = m_rpColor;
	i = m_csPtCount;
	do {
		pVertexColorArray->x = pSrcVertexArray->x;
		pVertexColorArray->y = pSrcVertexArray->y;
		pVertexColorArray->z = pSrcVertexArray->z;
		pVertexColorArray->color = rpColor;
		pSrcVertexArray++;
		pVertexColorArray++;
	} while (--i != 0);

	//Unlock vertexColor buffer
	UnlockVertexColorBuffer();

	return;
}

//Must be called with (m_rpPassCount > 0)
void UD3D8RenderDevice::RenderPassesNoCheckSetup_DualTextureAndDetailTexture(FTextureInfo &DetailTextureInfo) {
	INT i;
	INT t;
	FLOAT NearZ  = 380.0f;
	FLOAT RNearZ = 1.0f / NearZ;

	SetBlend(MultiPass.TMU[0].PolyFlags);

	//Surface texture must be 2X blended
	//Also force PF_Modulated for the TexEnv stage
	MultiPass.TMU[0].PolyFlags |= (PF_Modulated | PF_FlatShaded);

	//Detail texture uses first two texture units
	i = 2;
	do {
		if (i != 0) {
			SetTexEnv(i, MultiPass.TMU[i - 2].PolyFlags);
		}

		SetTexture(i, *MultiPass.TMU[i - 2].Info, MultiPass.TMU[i - 2].PolyFlags, MultiPass.TMU[i - 2].PanBias);
	} while (++i < 4);

	SetAlphaTexture(0);

	SetTexEnv(1, PF_Memorized);
	SetTextureNoPanBias(1, DetailTextureInfo, PF_Modulated);

	//Set stream state based on number of texture units in use
	SetStreamState(m_standardNTextureStreamShader[m_rpPassCount - 1]);

	//Check for additional enabled texture units that should be disabled
	DisableSubsequentTextures(4);

	//Make sure at least m_csPtCount entries are left in the vertex buffers
	if ((m_curVertexBufferPos + m_csPtCount) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor and texCoord buffers
	LockVertexColorBuffer();
	t = 0;
	do {
		LockTexCoordBuffer(t);
	} while (++t < m_rpPassCount);

	//Write vertex and color
	const FGLVertex *pSrcVertexArray = m_csVertexArray;
	FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
	DWORD detailColor = m_detailTextureColor4ub | 0xFF000000;
	i = m_csPtCount;
	do {
		pVertexColorArray->x = pSrcVertexArray->x;
		pVertexColorArray->y = pSrcVertexArray->y;
		pVertexColorArray->z = pSrcVertexArray->z;
		pVertexColorArray->color = detailColor;
		pSrcVertexArray++;
		pVertexColorArray++;
	} while (--i != 0);

	//Alpha texture for detail texture uses texture unit 0
	{
		INT t = 0;
		const FGLVertex *pVertex = &m_csVertexArray[0];
		FGLTexCoord *pTexCoord = m_pTexCoordArray[t];

		INT ptCounter = m_csPtCount;
		do {
			pTexCoord->u = pVertex->z * RNearZ;
			pTexCoord->v = 0.5f;

			pVertex++;
			pTexCoord++;
		} while (--ptCounter != 0);
	}
	//Detail texture uses texture unit 1
	//Remaining 2 textures use texture units 2 and 3
	t = 1;
	do {
		FLOAT UPan = TexInfo[t].UPan;
		FLOAT VPan = TexInfo[t].VPan;
		FLOAT UMult = TexInfo[t].UMult;
		FLOAT VMult = TexInfo[t].VMult;
		const FGLMapDot *pMapDot = &MapDotArray[0];
		FGLTexCoord *pTexCoord = m_pTexCoordArray[t];

		INT ptCounter = m_csPtCount;
		do {
			pTexCoord->u = (pMapDot->u - UPan) * UMult;
			pTexCoord->v = (pMapDot->v - VPan) * VMult;

			pMapDot++;
			pTexCoord++;
		} while (--ptCounter != 0);
	} while (++t < 4);

	//Unlock vertexColor and texCoord buffers
	UnlockVertexColorBuffer();
	t = 0;
	do {
		UnlockTexCoordBuffer(t);
	} while (++t < m_rpPassCount);

	return;
}

//Must be called with (m_rpPassCount > 0)
void UD3D8RenderDevice::RenderPassesNoCheckSetup_DualTextureAndDetailTexture_VP(FTextureInfo &DetailTextureInfo) {
	INT i;
	FLOAT vsParams[MAX_TMUNITS * 4];

	SetBlend(MultiPass.TMU[0].PolyFlags);

	//Surface texture must be 2X blended
	//Also force PF_Modulated for the TexEnv stage
	MultiPass.TMU[0].PolyFlags |= (PF_Modulated | PF_FlatShaded);

	//Detail texture uses first two texture units
	i = 2;
	do {
		if (i != 0) {
			SetTexEnv(i, MultiPass.TMU[i - 2].PolyFlags);
		}

		SetTexture(i, *MultiPass.TMU[i - 2].Info, MultiPass.TMU[i - 2].PolyFlags, MultiPass.TMU[i - 2].PanBias);

		vsParams[(i * 4) + 0] = TexInfo[i].UPan;
		vsParams[(i * 4) + 1] = TexInfo[i].VPan;
		vsParams[(i * 4) + 2] = TexInfo[i].UMult;
		vsParams[(i * 4) + 3] = TexInfo[i].VMult;
	} while (++i < 4);

	SetAlphaTexture(0);

	SetTextureNoPanBias(1, DetailTextureInfo, PF_Modulated);
	SetTexEnv(1, PF_Memorized);

	vsParams[0] = 0.002631578947f;
	vsParams[1] = 0.5f;
	vsParams[2] = 0.0f;
	vsParams[3] = 0.0f;
	vsParams[4] = TexInfo[1].UPan;
	vsParams[5] = TexInfo[1].VPan;
	vsParams[6] = TexInfo[1].UMult;
	vsParams[7] = TexInfo[1].VMult;
	m_d3dDevice->SetVertexShaderConstant(6, vsParams, 4);

	//Set dual texture and detail texture vertex program
	SetStreamState(m_csDualTextureAndDetailTextureVertexShader);

	//Check for additional enabled texture units that should be disabled
	DisableSubsequentTextures(4);

	//Make sure at least m_csPtCount entries are left in the vertex buffers
	if ((m_curVertexBufferPos + m_csPtCount) >= VERTEX_ARRAY_SIZE) {
		FlushVertexBuffers();
	}

	//Lock vertexColor buffer
	LockVertexColorBuffer();

	//Write vertex and color
	const FGLVertex *pSrcVertexArray = m_csVertexArray;
	FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
	DWORD detailColor = m_detailTextureColor4ub | 0xFF000000;
	i = m_csPtCount;
	do {
		pVertexColorArray->x = pSrcVertexArray->x;
		pVertexColorArray->y = pSrcVertexArray->y;
		pVertexColorArray->z = pSrcVertexArray->z;
		pVertexColorArray->color = detailColor;
		pSrcVertexArray++;
		pVertexColorArray++;
	} while (--i != 0);

	//Unlock vertexColor buffer
	UnlockVertexColorBuffer();

	return;
}

//Modified this routine to always set up detail texture state
//It should only be called if at least one polygon will be detail textured
void UD3D8RenderDevice::DrawDetailTexture(FTextureInfo &DetailTextureInfo, bool clipDetailTexture) {
	//Setup detail texture state
	SetBlend(PF_Modulated);

	//Set detail alpha mode flag
	bool detailAlphaMode = ((clipDetailTexture == false) && UseDetailAlpha) ? true : false;

	if (detailAlphaMode) {
		SetAlphaTexture(0);
		//TexEnv 0 is PF_Modulated by default

		SetTextureNoPanBias(1, DetailTextureInfo, PF_Modulated);
		SetTexEnv(1, PF_Memorized);

		//Set stream state for two textures
		SetStreamState(m_standardNTextureStreamShader[1]);

		//Check for additional enabled texture units that should be disabled
		DisableSubsequentTextures(2);
	}
	else {
		SetTextureNoPanBias(0, DetailTextureInfo, PF_Modulated);
		SetTexEnv(0, PF_Memorized);

		if (clipDetailTexture == true) {
			m_d3dDevice->SetRenderState(D3DRS_ZBIAS, 4);
		}

		//Set stream state for one texture
		SetStreamState(m_standardNTextureStreamShader[0]);

		//Check for additional enabled texture units that should be disabled
		DisableSubsequentTextures(1);
	}


	//Get detail texture color
	DWORD detailColor = m_detailTextureColor4ub;

	INT detailPassNum = 0;
	FLOAT NearZ  = 380.0f;
	FLOAT RNearZ = 1.0f / NearZ;
	FLOAT DetailScale = 1.0f;
	do {
		//Set up new NearZ and rescan points if subsequent pass
		if (detailPassNum > 0) {
			//Adjust NearZ and detail texture scaling
			NearZ /= 4.223f;
			RNearZ *= 4.223f;
			DetailScale *= 4.223f;

			//Rescan points
			(this->*m_pBufferDetailTextureDataProc)(NearZ);
		}

		//Calculate scaled UMult and VMult for detail texture based on mode
		FLOAT DetailUMult;
		FLOAT DetailVMult;
		if (detailAlphaMode) {
			DetailUMult = TexInfo[1].UMult * DetailScale;
			DetailVMult = TexInfo[1].VMult * DetailScale;
		}
		else {
			DetailUMult = TexInfo[0].UMult * DetailScale;
			DetailVMult = TexInfo[0].VMult * DetailScale;
		}

		INT Index = 0;

		INT *pNumPts = &MultiDrawCountArray[0];
		DWORD *pDetailTextureIsNear = DetailTextureIsNearArray;
		for (DWORD PolyNum = 0; PolyNum < m_csPolyCount; PolyNum++, pNumPts++, pDetailTextureIsNear++) {
			DWORD NumPts = *pNumPts;
			DWORD isNearBits = *pDetailTextureIsNear;

			//Skip the polygon if it will not be detail textured
			if (isNearBits == 0) {
				Index += NumPts;
				continue;
			}
			INT StartIndex = Index;

			DWORD allPtsBits = ~(~0U << NumPts);
			//Detail alpha mode
			if (detailAlphaMode) {
				//Make sure at least NumPts entries are left in the vertex buffers
				if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
					FlushVertexBuffers();
				}

				//Lock vertexColor, texCoord0, and texCoord1 buffers
				LockVertexColorBuffer();
				LockTexCoordBuffer(0);
				LockTexCoordBuffer(1);

				FGLTexCoord *pTexCoord0 = m_pTexCoordArray[0];
				FGLTexCoord *pTexCoord1 = m_pTexCoordArray[1];
				FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
				const FGLVertex *pSrcVertexArray = &m_csVertexArray[StartIndex];
				for (INT i = 0; i < NumPts; i++) {
					FLOAT U = MapDotArray[Index].u;
					FLOAT V = MapDotArray[Index].v;

					FLOAT PointZ_Times_RNearZ = pSrcVertexArray[i].z * RNearZ;
					pTexCoord0[i].u = PointZ_Times_RNearZ;
					pTexCoord0[i].v = 0.5f;
					pTexCoord1[i].u = (U - TexInfo[1].UPan) * DetailUMult;
					pTexCoord1[i].v = (V - TexInfo[1].VPan) * DetailVMult;

					pVertexColorArray[i].x = pSrcVertexArray[i].x;
					pVertexColorArray[i].y = pSrcVertexArray[i].y;
					pVertexColorArray[i].z = pSrcVertexArray[i].z;
					pVertexColorArray[i].color = detailColor | 0xFF000000;

					Index++;
				}

				//Unlock vertexColor,texCoord0, and texCoord1 buffers
				UnlockVertexColorBuffer();
				UnlockTexCoordBuffer(0);
				UnlockTexCoordBuffer(1);

				//Draw the triangles
				m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

				//Advance vertex buffer position
				m_curVertexBufferPos += NumPts;
			}
			//Otherwise, no clipping required, or clipping required, but DetailClipping not enabled
			else if ((clipDetailTexture == false) || (isNearBits == allPtsBits)) {
				//Make sure at least NumPts entries are left in the vertex buffers
				if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
					FlushVertexBuffers();
				}

				//Lock vertexColor and texCoord0 buffers
				LockVertexColorBuffer();
				LockTexCoordBuffer(0);

				FGLTexCoord *pTexCoord = m_pTexCoordArray[0];
				FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
				const FGLVertex *pSrcVertexArray = &m_csVertexArray[StartIndex];
				for (INT i = 0; i < NumPts; i++) {
					FLOAT U = MapDotArray[Index].u;
					FLOAT V = MapDotArray[Index].v;

					pTexCoord[i].u = (U - TexInfo[0].UPan) * DetailUMult;
					pTexCoord[i].v = (V - TexInfo[0].VPan) * DetailVMult;

					pVertexColorArray[i].x = pSrcVertexArray[i].x;
					pVertexColorArray[i].y = pSrcVertexArray[i].y;
					pVertexColorArray[i].z = pSrcVertexArray[i].z;
					DWORD alpha = appRound((1.0f - (Clamp(pSrcVertexArray[i].z, 0.0f, NearZ) * RNearZ)) * 255.0f);
					pVertexColorArray[i].color = detailColor | (alpha << 24);

					Index++;
				}

				//Unlock vertexColor and texCoord0 buffers
				UnlockVertexColorBuffer();
				UnlockTexCoordBuffer(0);

				//Draw the triangles
				m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

				//Advance vertex buffer position
				m_curVertexBufferPos += NumPts;
			}
			//Otherwise, clipping required and DetailClipping enabled
			else {
				//Make sure at least (NumPts * 2) entries are left in the vertex buffers
				if ((m_curVertexBufferPos + (NumPts * 2)) >= VERTEX_ARRAY_SIZE) {
					FlushVertexBuffers();
				}

				//Lock vertexColor and texCoord0 buffers
				LockVertexColorBuffer();
				LockTexCoordBuffer(0);

				DWORD NextIndex = 0;
				DWORD isNear_i_bit = 1U << (NumPts - 1);
				DWORD isNear_j_bit = 1U;
				FGLTexCoord *pTexCoord = m_pTexCoordArray[0];
				for (INT i = 0, j = NumPts - 1; i < NumPts; j = i++, isNear_j_bit = isNear_i_bit, isNear_i_bit >>= 1) {
					const FGLVertex &Point = m_csVertexArray[Index];
					FLOAT U = MapDotArray[Index].u;
					FLOAT V = MapDotArray[Index].v;

					if (((isNear_i_bit & isNearBits) != 0) && ((isNear_j_bit & isNearBits) == 0)) {
						const FGLVertex &PrevPoint = m_csVertexArray[StartIndex + j];
						FLOAT PrevU = MapDotArray[StartIndex + j].u;
						FLOAT PrevV = MapDotArray[StartIndex + j].v;

						FLOAT dist = PrevPoint.z - Point.z;
						FLOAT m = 1.0f;
						if (dist > 0.001f) {
							m = (NearZ - Point.z) / dist;
						}
						FGLVertexColor *pVertexColor = &m_pVertexColorArray[NextIndex];
						pVertexColor->x = (m * (PrevPoint.x - Point.x)) + Point.x;
						pVertexColor->y = (m * (PrevPoint.y - Point.y)) + Point.y;
						pVertexColor->z = NearZ;
						DWORD alpha = 0;
						pVertexColor->color = detailColor | (alpha << 24);

						pTexCoord[NextIndex].u = ((m * (PrevU - U)) + U - TexInfo[0].UPan) * DetailUMult;
						pTexCoord[NextIndex].v = ((m * (PrevV - V)) + V - TexInfo[0].VPan) * DetailVMult;

						NextIndex++;
					}

					if ((isNear_i_bit & isNearBits) != 0) {
						pTexCoord[NextIndex].u = (U - TexInfo[0].UPan) * DetailUMult;
						pTexCoord[NextIndex].v = (V - TexInfo[0].VPan) * DetailVMult;

						FGLVertexColor *pVertexColor = &m_pVertexColorArray[NextIndex];
						pVertexColor->x = Point.x;
						pVertexColor->y = Point.y;
						pVertexColor->z = Point.z;
						DWORD alpha = appRound((1.0f - (Clamp(Point.z, 0.0f, NearZ) * RNearZ)) * 255.0f);
						pVertexColor->color = detailColor | (alpha << 24);

						NextIndex++;
					}

					if (((isNear_i_bit & isNearBits) == 0) && ((isNear_j_bit & isNearBits) != 0)) {
						const FGLVertex &PrevPoint = m_csVertexArray[StartIndex + j];
						FLOAT PrevU = MapDotArray[StartIndex + j].u;
						FLOAT PrevV = MapDotArray[StartIndex + j].v;

						FLOAT dist = Point.z - PrevPoint.z;
						FLOAT m = 1.0f;
						if (dist > 0.001f) {
							m = (NearZ - PrevPoint.z) / dist;
						}
						FGLVertexColor *pVertexColor = &m_pVertexColorArray[NextIndex];
						pVertexColor->x = (m * (Point.x - PrevPoint.x)) + PrevPoint.x;
						pVertexColor->y = (m * (Point.y - PrevPoint.y)) + PrevPoint.y;
						pVertexColor->z = NearZ;
						DWORD alpha = 0;
						pVertexColor->color = detailColor | (alpha << 24);

						pTexCoord[NextIndex].u = ((m * (U - PrevU)) + PrevU - TexInfo[0].UPan) * DetailUMult;
						pTexCoord[NextIndex].v = ((m * (V - PrevV)) + PrevV - TexInfo[0].VPan) * DetailVMult;

						NextIndex++;
					}

					Index++;
				}

				//Unlock vertexColor and texCoord0 buffers
				UnlockVertexColorBuffer();
				UnlockTexCoordBuffer(0);

				//Draw the triangles
				m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NextIndex - 2);

				//Advance vertex buffer position
				m_curVertexBufferPos += NextIndex;
			}
		}
	} while (++detailPassNum < DetailMax);


	//Clear detail texture state
	if (detailAlphaMode) {
		//TexEnv 0 was left in default state of PF_Modulated
	}
	else {
		SetTexEnv(0, PF_Modulated);

		if (clipDetailTexture == true) {
			m_d3dDevice->SetRenderState(D3DRS_ZBIAS, 0);
		}
	}

	return;
}

//Modified this routine to always set up detail texture state
//It should only be called if at least one polygon will be detail textured
void UD3D8RenderDevice::DrawDetailTexture_VP(FTextureInfo &DetailTextureInfo) {
	INT Index = 0;

	//Setup detail texture state
	SetBlend(PF_Modulated);

	SetAlphaTexture(0);
	//TexEnv 0 is PF_Modulated by default

	SetTextureNoPanBias(1, DetailTextureInfo, PF_Modulated);
	SetTexEnv(1, PF_Memorized);

	FLOAT vsParams[8] = { 0.002631578947f, 0.5f, 0.0f, 0.0f,
						  TexInfo[1].UPan, TexInfo[1].VPan, TexInfo[1].UMult, TexInfo[1].VMult };
	m_d3dDevice->SetVertexShaderConstant(6, vsParams, 2);

	//Set detail alpha vertex program
	SetStreamState(m_csDetailAlphaVertexShader);

	//Check for additional enabled texture units that should be disabled
	DisableSubsequentTextures(2);

	DWORD detailColor = m_detailTextureColor4ub | 0xFF000000;
	INT *pNumPts = &MultiDrawCountArray[0];
	DWORD *pDetailTextureIsNear = DetailTextureIsNearArray;
	DWORD csPolyCount = m_csPolyCount;
	for (DWORD PolyNum = 0; PolyNum < csPolyCount; PolyNum++, pNumPts++, pDetailTextureIsNear++) {
		INT NumPts = *pNumPts;
		DWORD isNearBits = *pDetailTextureIsNear;
		INT i;

		//Skip the polygon if it will not be detail textured
		if (isNearBits == 0) {
			Index += NumPts;
			continue;
		}

		//Make sure at least NumPts entries are left in the vertex buffers
		if ((m_curVertexBufferPos + NumPts) >= VERTEX_ARRAY_SIZE) {
			FlushVertexBuffers();
		}

		//Lock vertexColor buffer
		LockVertexColorBuffer();

		//Write vertex
		const FGLVertex *pSrcVertexArray = &m_csVertexArray[Index];
		FGLVertexColor *pVertexColorArray = m_pVertexColorArray;
		for (i = 0; i < NumPts; i++) {
			pVertexColorArray[i].x = pSrcVertexArray[i].x;
			pVertexColorArray[i].y = pSrcVertexArray[i].y;
			pVertexColorArray[i].z = pSrcVertexArray[i].z;
			pVertexColorArray[i].color = detailColor;
		}

		//Unlock vertexColor buffers
		UnlockVertexColorBuffer();

		//Draw the triangles
		m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, m_curVertexBufferPos, NumPts - 2);

		//Advance vertex buffer position
		m_curVertexBufferPos += NumPts;

		Index += NumPts;
	}


	//Clear detail texture state
	//TexEnv 0 was left in default state of PF_Modulated

	return;
}

INT UD3D8RenderDevice::BufferStaticComplexSurfaceGeometry(const FSurfaceFacet& Facet) {
	INT Index = 0;

	// Buffer "static" geometry.
	m_csPolyCount = 0;
	FGLMapDot *pMapDot = &MapDotArray[0];
	FGLVertex *pVertex = &m_csVertexArray[0];
	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next) {
		//Skip if no points
		INT NumPts = Poly->NumPts;
		if (NumPts <= 0) {
			continue;
		}

		DWORD csPolyCount = m_csPolyCount;
		MultiDrawFirstArray[csPolyCount] = Index;
		MultiDrawCountArray[csPolyCount] = NumPts;
		m_csPolyCount = csPolyCount + 1;

		Index += NumPts;
		if (Index > VERTEX_ARRAY_SIZE) {
			return 0;
		}
		FTransform **pPts = &Poly->Pts[0];
		do {
			const FVector &Point = (*pPts++)->Point;

			pMapDot->u = (Facet.MapCoords.XAxis | Point) - m_csUDot;
			pMapDot->v = (Facet.MapCoords.YAxis | Point) - m_csVDot;
			pMapDot++;

			pVertex->x = Point.X;
			pVertex->y = Point.Y;
			pVertex->z = Point.Z;
			pVertex++;
		} while (--NumPts != 0);
	}

	return Index;
}

INT UD3D8RenderDevice::BufferStaticComplexSurfaceGeometry_VP(const FSurfaceFacet& Facet) {
	INT Index = 0;

	// Buffer "static" geometry.
	m_csPolyCount = 0;
	FGLVertex *pVertex = &m_csVertexArray[0];
	for (FSavedPoly* Poly = Facet.Polys; Poly; Poly = Poly->Next) {
		//Skip if no points
		INT NumPts = Poly->NumPts;
		if (NumPts <= 0) {
			continue;
		}

		DWORD csPolyCount = m_csPolyCount;
		MultiDrawFirstArray[csPolyCount] = Index;
		MultiDrawCountArray[csPolyCount] = NumPts;
		m_csPolyCount = csPolyCount + 1;

		Index += NumPts;
		if (Index > VERTEX_ARRAY_SIZE) {
			return 0;
		}
		FTransform **pPts = &Poly->Pts[0];
		do {
			const FVector &Point = (*pPts++)->Point;

			pVertex->x = Point.X;
			pVertex->y = Point.Y;
			pVertex->z = Point.Z;
			pVertex++;
		} while (--NumPts != 0);
	}

	return Index;
}

DWORD UD3D8RenderDevice::BufferDetailTextureData(FLOAT NearZ) {
	DWORD *pDetailTextureIsNear = DetailTextureIsNearArray;
	DWORD anyIsNearBits = 0;

	FGLVertex *pVertex = &m_csVertexArray[0];
	INT *pNumPts = &MultiDrawCountArray[0];
	DWORD csPolyCount = m_csPolyCount;
	do {
		INT NumPts = *pNumPts++;
		DWORD isNear = 0;

		do {
			isNear <<= 1;
			if (pVertex->z < NearZ) {
				isNear |= 1;
			}
			pVertex++;
		} while (--NumPts != 0);

		*pDetailTextureIsNear++ = isNear;
		anyIsNearBits |= isNear;
	} while (--csPolyCount != 0);

	return anyIsNearBits;
}

#ifdef UTGLR_INCLUDE_SSE_CODE
__declspec(naked) DWORD UD3D8RenderDevice::BufferDetailTextureData_SSE2(FLOAT NearZ) {
	__asm {
		movd xmm0, [esp+4]

		push esi
		push edi

		lea esi, [ecx]this.m_csVertexArray
		lea edx, [ecx]this.MultiDrawCountArray
		lea edi, [ecx]this.DetailTextureIsNearArray

		pxor xmm1, xmm1

		mov ecx, [ecx]this.m_csPolyCount

		poly_count_loop:
			mov eax, [edx]
			add edx, 4

			pxor xmm2, xmm2

			num_pts_loop:
				movss xmm3, [esi+8]
				add esi, TYPE FGLVertex

				pslld xmm2, 1

				cmpltss xmm3, xmm0
				psrld xmm3, 31

				por xmm2, xmm3

				dec eax
				jne num_pts_loop

			movd [edi], xmm2
			add edi, 4

			por xmm1, xmm2

			dec ecx
			jne poly_count_loop

		movd eax, xmm1

		pop edi
		pop esi

		ret 4
	}
}
#endif //UTGLR_INCLUDE_SSE_CODE

void UD3D8RenderDevice::EndBufferingNoCheck(void) {
	SetDefaultAAState();
	//No need to set default projection state here as EndBufferingNoCheck sets its own projection state
	//No need to set stream state here as it is set when beginning buffering
	SetDefaultTextureState();

	clockFast(GouraudCycles);

	//Set projection state
	SetProjectionState(m_requestNearZRangeHackProjection);


	//Unlock vertexColor and texCoord0 buffers
	//Unlock secondary color buffer if fog
	UnlockVertexColorBuffer();
	if (m_requestedColorFlags & CF_FOG_MODE) {
		UnlockSecondaryColorBuffer();
	}
	UnlockTexCoordBuffer(0);

#ifdef UTGLR_DEBUG_ACTOR_WIREFRAME
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
#endif

	//Draw the triangles
	m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, m_curVertexBufferPos, BufferedVerts / 3);

	//Advance vertex buffer position
	m_curVertexBufferPos += BufferedVerts;

#ifdef UTGLR_DEBUG_ACTOR_WIREFRAME
	m_d3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	BufferedVerts = 0;

	unclockFast(GouraudCycles);
}

void UD3D8RenderDevice::EndTileBufferingNoCheck(void) {
	if (NoAATiles) {
		SetDisabledAAState();
	}
	else {
		SetDefaultAAState();
	}
	SetDefaultProjectionState();
	//No need to set default stream state here as it is set when beginning buffering
	SetDefaultTextureState();

	clockFast(TileCycles);

	//Unlock vertexColor and texCoord0 buffers
	UnlockVertexColorBuffer();
	UnlockTexCoordBuffer(0);

	//Draw the quads (stored as triangles)
	m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, m_curVertexBufferPos, BufferedTileVerts / 3);

	//Advance vertex buffer position
	m_curVertexBufferPos += BufferedTileVerts;

	BufferedTileVerts = 0;

	unclockFast(TileCycles);
}


// Static variables.
INT UD3D8RenderDevice::NumDevices = 0;
INT UD3D8RenderDevice::LockCount = 0;

HMODULE UD3D8RenderDevice::hModuleD3d8 = NULL;
LPDIRECT3DCREATE8 UD3D8RenderDevice::pDirect3DCreate8 = NULL;

bool UD3D8RenderDevice::g_gammaFirstTime = false;
bool UD3D8RenderDevice::g_haveOriginalGammaRamp = false;
//UD3D8RenderDevice::FGammaRamp UD3D8RenderDevice::g_originalGammaRamp;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
