struct  DDCOLORKEY
{
    DDCOLORKEY():
        dwColorSpaceLowValue(0),
        dwColorSpaceHighValue(0) {}

    DWORD    dwColorSpaceLowValue;
    DWORD    dwColorSpaceHighValue;
};

struct DDPIXELFORMAT
{

    DDPIXELFORMAT():
        dwSize(0),
        dwFlags(0),
        dwFourCC(0),
        dwRGBBitCount(0),
        dwRBitMask(0),
        dwGBitMask(0),
        dwBBitMask(0),
        dwRGBAlphaBitMask(0) {}


    DWORD    dwSize;
    DWORD    dwFlags;
    DWORD    dwFourCC;
    union
    {
        DWORD    dwRGBBitCount;
        DWORD    dwYUVBitCount;
        DWORD    dwZBufferBitDepth;
        DWORD    dwAlphaBitDepth;
    };
    union
    {
        DWORD    dwRBitMask;
        DWORD    dwYBitMask;
    };
    union
    {
        DWORD    dwGBitMask;
        DWORD    dwUBitMask;
    };
    union
    {
        DWORD    dwBBitMask;
        DWORD    dwVBitMask;
    };
    union
    {
        DWORD    dwRGBAlphaBitMask;
        DWORD    dwYUVAlphaBitMask;
        DWORD    dwRGBZBitMask;
        DWORD    dwYUVZBitMask;
    };
};

struct  DDSCAPS2
{
     DDSCAPS2():
        dwCaps(0),
        dwCaps2(0),
        dwCaps3(0),
        dwCaps4(0) {}

    DWORD       dwCaps;
    DWORD       dwCaps2;
    DWORD       dwCaps3;
    union
    {
        DWORD       dwCaps4;
        DWORD       dwVolumeDepth;
    };
};

struct DDSURFACEDESC2
{
    DDSURFACEDESC2():
        dwSize(0),
        dwFlags(0),
        dwHeight(0),
        dwWidth(0),
        lPitch(0),
        dwBackBufferCount(0),
        dwMipMapCount(0),
        dwAlphaBitDepth(0),
        dwReserved(0),
        lpSurface(0),
        dwTextureStage(0) {}


    DWORD         dwSize;
    DWORD         dwFlags;
    DWORD         dwHeight;
    DWORD         dwWidth;
    union
    {
        _WORD     lPitch;
        DWORD     dwLinearSize;
    };
    union
    {
        DWORD      dwBackBufferCount;
        DWORD      dwDepth;
    };
    union
    {
        DWORD     dwMipMapCount;
        DWORD     dwRefreshRate;
    };
    DWORD         dwAlphaBitDepth;
    DWORD         dwReserved;
    DWORD		  lpSurface;
    DDCOLORKEY    ddckCKDestOverlay;
    DDCOLORKEY    ddckCKDestBlt;
    DDCOLORKEY    ddckCKSrcOverlay;
    DDCOLORKEY    ddckCKSrcBlt;
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS2      ddsCaps;
    DWORD dwTextureStage;
};

//
// DDSURFACEDESC2 flags that mark the validity of the struct data
//
#define DDSD_CAPS               0x00000001l     // default
#define DDSD_HEIGHT             0x00000002l        // default
#define DDSD_WIDTH              0x00000004l        // default
#define DDSD_PIXELFORMAT        0x00001000l        // default
#define DDSD_PITCH              0x00000008l     // For uncompressed formats
#define DDSD_MIPMAPCOUNT        0x00020000l
#define DDSD_LINEARSIZE         0x00080000l     // For compressed formats
#define DDSD_DEPTH              0x00800000l        // Volume Textures

//
// DDPIXELFORMAT flags
//
#define DDPF_ALPHAPIXELS        0x00000001l
#define DDPF_FOURCC             0x00000004l        // Compressed formats
#define DDPF_RGB                0x00000040l        // Uncompressed formats
#define DDPF_ALPHA              0x00000002l
#define DDPF_COMPRESSED         0x00000080l
#define DDPF_LUMINANCE          0x00020000l

//
// DDSCAPS flags
//
#define DDSCAPS_TEXTURE         0x00001000l     // default
#define DDSCAPS_COMPLEX         0x00000008l
#define DDSCAPS_MIPMAP          0x00400000l
#define DDSCAPS2_VOLUME         0x00200000l

typedef enum DXGI_FORMAT {
    DXGI_FORMAT_UNKNOWN,
    DXGI_FORMAT_R32G32B32A32_TYPELESS,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_R32G32B32_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32_UINT,
    DXGI_FORMAT_R32G32B32_SINT,
    DXGI_FORMAT_R16G16B16A16_TYPELESS,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_UNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_R32G32_TYPELESS,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32_UINT,
    DXGI_FORMAT_R32G32_SINT,
    DXGI_FORMAT_R32G8X24_TYPELESS,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,
    DXGI_FORMAT_R10G10B10A2_TYPELESS,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R10G10B10A2_UINT,
    DXGI_FORMAT_R11G11B10_FLOAT,
    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_UINT,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R8G8B8A8_SINT,
    DXGI_FORMAT_R16G16_TYPELESS,
    DXGI_FORMAT_R16G16_FLOAT,
    DXGI_FORMAT_R16G16_UNORM,
    DXGI_FORMAT_R16G16_UINT,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_R16G16_SINT,
    DXGI_FORMAT_R32_TYPELESS,
    DXGI_FORMAT_D32_FLOAT,
    DXGI_FORMAT_R32_FLOAT,
    DXGI_FORMAT_R32_UINT,
    DXGI_FORMAT_R32_SINT,
    DXGI_FORMAT_R24G8_TYPELESS,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT,
    DXGI_FORMAT_R8G8_TYPELESS,
    DXGI_FORMAT_R8G8_UNORM,
    DXGI_FORMAT_R8G8_UINT,
    DXGI_FORMAT_R8G8_SNORM,
    DXGI_FORMAT_R8G8_SINT,
    DXGI_FORMAT_R16_TYPELESS,
    DXGI_FORMAT_R16_FLOAT,
    DXGI_FORMAT_D16_UNORM,
    DXGI_FORMAT_R16_UNORM,
    DXGI_FORMAT_R16_UINT,
    DXGI_FORMAT_R16_SNORM,
    DXGI_FORMAT_R16_SINT,
    DXGI_FORMAT_R8_TYPELESS,
    DXGI_FORMAT_R8_UNORM,
    DXGI_FORMAT_R8_UINT,
    DXGI_FORMAT_R8_SNORM,
    DXGI_FORMAT_R8_SINT,
    DXGI_FORMAT_A8_UNORM,
    DXGI_FORMAT_R1_UNORM,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
    DXGI_FORMAT_R8G8_B8G8_UNORM,
    DXGI_FORMAT_G8R8_G8B8_UNORM,
    DXGI_FORMAT_BC1_TYPELESS,
    DXGI_FORMAT_BC1_UNORM,
    DXGI_FORMAT_BC1_UNORM_SRGB,
    DXGI_FORMAT_BC2_TYPELESS,
    DXGI_FORMAT_BC2_UNORM,
    DXGI_FORMAT_BC2_UNORM_SRGB,
    DXGI_FORMAT_BC3_TYPELESS,
    DXGI_FORMAT_BC3_UNORM,
    DXGI_FORMAT_BC3_UNORM_SRGB,
    DXGI_FORMAT_BC4_TYPELESS,
    DXGI_FORMAT_BC4_UNORM,
    DXGI_FORMAT_BC4_SNORM,
    DXGI_FORMAT_BC5_TYPELESS,
    DXGI_FORMAT_BC5_UNORM,
    DXGI_FORMAT_BC5_SNORM,
    DXGI_FORMAT_B5G6R5_UNORM,
    DXGI_FORMAT_B5G5R5A1_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
    DXGI_FORMAT_B8G8R8A8_TYPELESS,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
    DXGI_FORMAT_B8G8R8X8_TYPELESS,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
    DXGI_FORMAT_BC6H_TYPELESS,
    DXGI_FORMAT_BC6H_UF16,
    DXGI_FORMAT_BC6H_SF16,
    DXGI_FORMAT_BC7_TYPELESS,
    DXGI_FORMAT_BC7_UNORM,
    DXGI_FORMAT_BC7_UNORM_SRGB,
    DXGI_FORMAT_AYUV,
    DXGI_FORMAT_Y410,
    DXGI_FORMAT_Y416,
    DXGI_FORMAT_NV12,
    DXGI_FORMAT_P010,
    DXGI_FORMAT_P016,
    DXGI_FORMAT_420_OPAQUE,
    DXGI_FORMAT_YUY2,
    DXGI_FORMAT_Y210,
    DXGI_FORMAT_Y216,
    DXGI_FORMAT_NV11,
    DXGI_FORMAT_AI44,
    DXGI_FORMAT_IA44,
    DXGI_FORMAT_P8,
    DXGI_FORMAT_A8P8,
    DXGI_FORMAT_B4G4R4A4_UNORM,
    DXGI_FORMAT_P208,
    DXGI_FORMAT_V208,
    DXGI_FORMAT_V408,
    DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
    DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
    DXGI_FORMAT_FORCE_UINT
} DXGI_FORMATS;

typedef enum D3D10_RESOURCE_DIMENSION {
    D3D10_RESOURCE_DIMENSION_UNKNOWN,
    D3D10_RESOURCE_DIMENSION_BUFFER,
    D3D10_RESOURCE_DIMENSION_TEXTURE1D,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D
} D3D10_RESOURCE_DIMENSIONS;

typedef struct {
    DXGI_FORMAT              dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    UINT                     miscFlag;
    UINT                     arraySize;
    UINT                     miscFlags2;
} DDS_HEADER_DXT10;

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
    ((DWORD)(char)(ch0) | ((DWORD)(char)(ch1) << 8) |   \
    ((DWORD)(char)(ch2) << 16) | ((DWORD)(char)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

inline FString MakeFOURCCFromINT(INT value)
{
    return FString::Printf(TEXT("%c%c%c%c"), value & 0xff, (value >> 8) & 0xff, (value >> 16) & 0xff, (value >> 24) & 0xff);
}

/*
* FOURCC codes for DX compressed-texture pixel formats
*/
#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))
#define FOURCC_BC4U  (MAKEFOURCC('B','C','4','U'))
#define FOURCC_BC4S  (MAKEFOURCC('B','C','4','S'))
#define FOURCC_BC5U  (MAKEFOURCC('A','T','I','2'))
#define FOURCC_BC5S  (MAKEFOURCC('B','C','5','S'))
#define FOURCC_BC5S  (MAKEFOURCC('B','C','5','S'))
#define FOURCC_BC5S  (MAKEFOURCC('B','C','5','S'))
#define FOURCC_BC7   (MAKEFOURCC('B','C','7','L'))  //DXGI_FORMAT_BC7_UNORM
#define FOURCC_BC7U  (MAKEFOURCC('B','C','7','\0')) //DXGI_FORMAT_BC7_UNORM
#define FOURCC_DX10  (MAKEFOURCC('D','X','1','0')) //BC6H and BC7 are using the extended DX10 header
