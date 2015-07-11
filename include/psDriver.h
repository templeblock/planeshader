// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#ifndef __DRIVER_H__PS__
#define __DRIVER_H__PS__

#include "psLine.h"

namespace planeshader {
  class psTex;
  class psDirectX9;
  class psDirectX10;
  class psDirectX11;
  class psOpenGL1;
  class psNullDriver;
  struct STATEINFO;
  class psShader;
  class psTexblock;

  struct BSS_COMPILER_DLLEXPORT RealDriver
  {
    union {
      psDirectX9* dx9;
      psDirectX10* dx10;
      psDirectX11* dx11;
      psOpenGL1* ogl1;
      psNullDriver* nul;
    };
    enum DRIVERTYPE : unsigned char
    {
      DRIVERTYPE_DX9,
      DRIVERTYPE_DX10,
      DRIVERTYPE_DX11,
      DRIVERTYPE_DX12,
      DRIVERTYPE_GL1,
      DRIVERTYPE_GL2,
      DRIVERTYPE_GL3,
      DRIVERTYPE_GL4,
      DRIVERTYPE_GL_ES2,
      DRIVERTYPE_GL_ES3,
      DRIVERTYPE_VULKAN,
      DRIVERTYPE_WEBGL,
      DRIVERTYPE_NULL,
      DRIVERTYPE_NUM
    } type;
  };

  struct psVertex
  {
    float x;
    float y;
    float z;
    float w; // We have a w element to pad this to 32 bytes
    unsigned int color;
  };

  enum PRIMITIVETYPE : unsigned char {
    TRIANGLELIST=0,
    TRIANGLESTRIP,
    LINELIST,
    LINESTRIP,
    POINTLIST
  };

  enum FORMATS : unsigned char {
    FMT_UNKNOWN,
    FMT_A8R8G8B8,
    FMT_X8R8G8B8,
    FMT_R5G6B5,
    FMT_A1R5G5B5,
    FMT_A4R4G4B4,
    FMT_A8,
    FMT_A2B10G10R10,
    FMT_A8B8G8R8,
    FMT_G16R16,
    FMT_A16B16G16R16,
    FMT_V8U8,
    FMT_Q8W8V8U8,
    FMT_V16U16,
    FMT_R8G8_B8G8,
    FMT_G8R8_G8B8,
    FMT_DXT1,
    FMT_DXT2,
    FMT_DXT3,
    FMT_DXT4,
    FMT_DXT5,
    FMT_D16,
    FMT_D32F,
    FMT_S8D24,
    FMT_INDEX16,
    FMT_INDEX32,
    FMT_Q16W16V16U16,
    FMT_R16F,
    FMT_G16R16F,
    FMT_A16B16G16R16F,
    FMT_R32F,
    FMT_G32R32F,
    FMT_A32B32G32R32F,
    FMT_R8G8B8A8_UINT,
    FMT_R16G16_SINT,
    FMT_FORMAT_R16G16B16A16_SINT,
    FMT_R8G8B8A8_UNORM,
    FMT_R16G16_SNORM,
    FMT_R16G16B16A16_SNORM,
    FMT_R16G16_UNORM,
    FMT_R16G16B16A16_UNORM,
    FMT_R16G16_FLOAT,
    FMT_R16G16B16A16_FLOAT
  };

  struct psVertObj
  {
    void* verts;
    void* indices;
    unsigned int nvert; // Number of vertices 
    unsigned int nindice; // Number of indices
    unsigned int vsize; // Size of the vertices in this buffer
    FORMATS ifmt; // Either FMT_INDEX16 or FMT_INDEX32
    PRIMITIVETYPE mode; // Mode to render in
  };

  enum USAGETYPES : unsigned int {
    USAGE_DEFAULT=0,
    USAGE_IMMUTABLE=1,
    USAGE_DYNAMIC=2,
    USAGE_STAGING=3,
    USAGE_VERTEX=4,
    USAGE_INDEX=8,
    USAGE_CONSTANT_BUFFER=12,
    USAGE_AUTOGENMIPMAP=(1<<4),
    USAGE_TEXTURECUBE=(1<<5),
    USAGE_RENDERTARGET=(1<<6),
    USAGE_SHADER_RESOURCE=(1<<7),
    USAGE_DEPTH_STENCIL=(1<<8),
    USAGE_USAGEMASK=3,
    USAGE_BINDMASK=12
  };

  enum ELEMENT_SEMANTICS : unsigned char {
    ELEMENT_BINORMAL=0,
    ELEMENT_BLENDINDICES,
    ELEMENT_BLENDWEIGHT,
    ELEMENT_COLOR,
    ELEMENT_NORMAL,
    ELEMENT_POSITION,
    ELEMENT_POSITIONT,
    ELEMENT_PSIZE,
    ELEMENT_TANGENT,
    ELEMENT_TEXCOORD
  };

  struct ELEMENT_DESC
  {
    ELEMENT_SEMANTICS semantic;
    unsigned char semanticIndex;
    FORMATS format;
    unsigned char IAslot;
    unsigned int byteOffset; // Set to -1 to define it as being directly after the next one
  };

  // A list of shader versions that we recognize
  enum SHADER_VER : unsigned char
  {
    VERTEX_SHADER_1_1=0, //vs_1_1
    VERTEX_SHADER_2_0,
    VERTEX_SHADER_2_a,
    VERTEX_SHADER_3_0,
    VERTEX_SHADER_4_0,
    VERTEX_SHADER_4_1,
    VERTEX_SHADER_5_0,
    PIXEL_SHADER_1_1, //ps_1_1
    PIXEL_SHADER_1_2,
    PIXEL_SHADER_1_3,
    PIXEL_SHADER_1_4,
    PIXEL_SHADER_2_0,
    PIXEL_SHADER_2_a,
    PIXEL_SHADER_2_b,
    PIXEL_SHADER_3_0,
    PIXEL_SHADER_4_0,
    PIXEL_SHADER_4_1,
    PIXEL_SHADER_5_0,
    GEOMETRY_SHADER_4_0, //gs_4_0
    GEOMETRY_SHADER_4_1,
    GEOMETRY_SHADER_5_0,
    COMPUTE_SHADER_4_0, //cs_4_0
    COMPUTE_SHADER_4_1,
    COMPUTE_SHADER_5_0,
    DOMAIN_SHADER_5_0, //ds_5_0
    HULL_SHADER_5_0, //hs_5_0
    NUM_SHADER_VERSIONS
  };

  enum LOCK_FLAGS : unsigned char
  {
    LOCK_READ=1,
    LOCK_WRITE=2,
    LOCK_READ_WRITE=3,
    LOCK_WRITE_DISCARD=4,
    LOCK_WRITE_NO_OVERWRITE=5,
    LOCK_DONOTWAIT=8,
    LOCK_TYPEMASK=LOCK_READ|LOCK_WRITE|LOCK_READ_WRITE|LOCK_WRITE_DISCARD|LOCK_WRITE_NO_OVERWRITE,
  };

  struct TEXTURE_DESC
  {
    psVec3Diu dim;
    USAGETYPES usage;
    FORMATS format;
    unsigned char miplevels;
  };
  
  enum FILTERS : unsigned char 
  {
    FILTER_NONE=0,
    FILTER_NEAREST,
    FILTER_LINEAR,
    FILTER_BOX,
    FILTER_PRECOMPUTEALPHA,
    FILTER_ALPHABOX,
    NUM_FILTERS
  };
  class PS_DLLEXPORT psDriver
  {
  protected:
    inline psDriver(const psVeciu& Screendim) : screendim(Screendim) {}

  public:
    inline virtual ~psDriver() {}
    // Begins a scene
    virtual bool Begin()=0;
    // Ends a scene
    virtual char End()=0;
    // Draws a vertex object
    virtual void BSS_FASTCALL Draw(psVertObj* buf, FLAG_TYPE flags, const float(&transform)[4][4]=identity)=0;
    // Draws a rectangle
    virtual void BSS_FASTCALL DrawRect(const psRectRotateZ rect, const psRect& uv, unsigned int color, const psTex* const* texes, unsigned char numtex, FLAG_TYPE flags)=0;
    virtual void BSS_FASTCALL DrawRectBatchBegin(const psTex* const* texes, unsigned char numtex, FLAG_TYPE flags)=0;
    virtual void BSS_FASTCALL DrawRectBatch(const psRectRotateZ rect, const psRect& uv, unsigned int color, const float(&xform)[4][4]=identity)=0;
    virtual void DrawRectBatchEnd(const float(&xform)[4][4]=identity)=0;
    // Draws a polygon
    virtual void BSS_FASTCALL DrawPolygon(const psVec* verts, FNUM Z, int num, unsigned long vertexcolor, FLAG_TYPE flags)=0;
    // Draws points (which are always batch rendered)
    virtual void BSS_FASTCALL DrawPointsBegin(const psTex* const* texes, unsigned char numtex, float size, FLAG_TYPE flags)=0;
    virtual void BSS_FASTCALL DrawPoints(psVertex* particles, unsigned int num)=0;
    virtual void DrawPointsEnd()=0;
    // Draws lines (which are also always batch rendered)
    virtual void BSS_FASTCALL DrawLinesStart(FLAG_TYPE flags)=0;
    virtual void BSS_FASTCALL DrawLines(const psLine& line, float Z1, float Z2, unsigned long vertexcolor, FLAG_TYPE flags)=0;
    virtual void DrawLinesEnd()=0;
    // Applies a camera (if you need the current camera, look at the pass you belong to, not the driver)
    virtual void BSS_FASTCALL ApplyCamera(const psVec3D& pos, const psVec& pivot, FNUM rotation, const psRectiu& viewport)=0;
    virtual void BSS_FASTCALL ApplyCamera3D(const float(&m)[4][4], const psRectiu& viewport)=0;
    // Applies the camera transform (or it's inverse) according to the flags to a point.
    virtual psVec3D BSS_FASTCALL TransformPoint(const psVec3D& point, FLAG_TYPE flags) const=0;
    virtual psVec3D BSS_FASTCALL ReversePoint(const psVec3D& point, FLAG_TYPE flags) const=0;
    // Draws a fullscreen quad
    virtual void DrawFullScreenQuad()=0;
    // Gets/Sets the extent
    virtual const psVec& GetExtent() const=0;
    virtual void BSS_FASTCALL SetExtent(float znear, float zfar)=0;
    // Creates a vertex or index buffer
    virtual void* BSS_FASTCALL CreateBuffer(unsigned short bytes, unsigned int usage, const void* initdata=0)=0;
    virtual void* BSS_FASTCALL LockBuffer(void* target, unsigned int flags)=0;
    virtual void BSS_FASTCALL UnlockBuffer(void* target)=0;
    virtual void* BSS_FASTCALL LockTexture(void* target, unsigned int flags, unsigned int& pitch, unsigned char miplevel = 0)=0;
    virtual void BSS_FASTCALL UnlockTexture(void* target, unsigned char miplevel = 0)=0;
    // Creates a texture
    virtual void* BSS_FASTCALL CreateTexture(psVeciu dim, FORMATS format, unsigned int usage=USAGE_SHADER_RESOURCE, unsigned char miplevels=0, const void* initdata=0, void** additionalview=0, psTexblock* texblock=0)=0;
    virtual void* BSS_FASTCALL LoadTexture(const char* path, unsigned int usage=USAGE_SHADER_RESOURCE, FORMATS format=FMT_UNKNOWN, void** additionalview=0, unsigned char miplevels=0, FILTERS mipfilter = FILTER_BOX, FILTERS loadfilter = FILTER_NONE, psVeciu dim = VEC_ZERO, psTexblock* texblock=0)=0;
    virtual void* BSS_FASTCALL LoadTextureInMemory(const void* data, size_t datasize, unsigned int usage=USAGE_SHADER_RESOURCE, FORMATS format=FMT_UNKNOWN, void** additionalview=0, unsigned char miplevels=0, FILTERS mipfilter = FILTER_BOX, FILTERS loadfilter = FILTER_NONE, psVeciu dim = VEC_ZERO, psTexblock* texblock=0)=0;
    virtual void BSS_FASTCALL CopyTextureRect(psRectiu srcrect, psVeciu destpos, void* src, void* dest, unsigned char miplevel = 0)=0;
    // Pushes or pops a scissor rect on to the stack
    virtual void BSS_FASTCALL PushScissorRect(const psRectl& rect)=0;
    virtual void PopScissorRect()=0;
    // Sets the current rendertargets, setting all the rest to null.
    virtual void BSS_FASTCALL SetRenderTargets(const psTex* const* texes, unsigned char num, const psTex* depthstencil=0)=0;
    // Sets shader constants
    virtual void BSS_FASTCALL SetShaderConstants(void* constbuf, SHADER_VER shader)=0;
    // Sets textures for a given type of shader (in DX9 this is completely ignored)
    virtual void BSS_FASTCALL SetTextures(const psTex* const* texes, unsigned char num, SHADER_VER shader=PIXEL_SHADER_1_1)=0;
    // Builds a stateblock from the given set of state changes
    virtual void* BSS_FASTCALL CreateStateblock(const STATEINFO* states)=0;
    // Builds a texblock from the given set of sampler states
    virtual void* BSS_FASTCALL CreateTexblock(const STATEINFO* states)=0;
    // Sets a given stateblock
    virtual void BSS_FASTCALL SetStateblock(void* stateblock)=0;
    // Create a vertex layout from several element descriptions
    virtual void* BSS_FASTCALL CreateLayout(void* shader, const ELEMENT_DESC* elements, unsigned char num)=0;
    virtual void BSS_FASTCALL SetLayout(void* layout)=0;
    // Frees a created resource of the specified type
    enum RESOURCE_TYPE : unsigned char { RES_TEXTURE, RES_SURFACE, RES_DEPTHVIEW, RES_SHADERVS, RES_SHADERPS, RES_SHADERGS, RES_SHADERCS, RES_SHADERDS, RES_SHADERHS, RES_STATEBLOCK, RES_TEXBLOCK, RES_INDEXBUF, RES_VERTEXBUF, RES_CONSTBUF, RES_LAYOUT };
    virtual TEXTURE_DESC BSS_FASTCALL GetTextureDesc(void* t)=0;
    virtual void BSS_FASTCALL FreeResource(void* p, RESOURCE_TYPE t)=0;
    virtual void BSS_FASTCALL GrabResource(void* p, RESOURCE_TYPE t)=0;
    virtual void BSS_FASTCALL CopyResource(void* dest, void* src, RESOURCE_TYPE t)=0;
    virtual void BSS_FASTCALL Resize(psVeciu dim, FORMATS format, bool fullscreen)=0;
    // Clears everything to a specified color
    virtual void BSS_FASTCALL Clear(unsigned int color)=0;
    // Gets the backbuffer texture
    virtual psTex* GetBackBuffer()=0;
    // Gets a pointer to the driver implementation
    virtual RealDriver GetRealDriver()=0;
    // Sets default rendertarget
    virtual void SetDefaultRenderTarget(const psTex* rt=0) { _defaultrt = !rt?GetBackBuffer():rt; }
    // Gets number of bytes per pixel of a given format
    virtual unsigned short GetBytesPerPixel(FORMATS format)=0;

    // Compile a shader from a string
    virtual void* BSS_FASTCALL CompileShader(const char* source, SHADER_VER profile, const char* entrypoint="")=0;
    // Create an actual shader object from compiled shader source (either precompiled or from CompileShader())
    virtual void* BSS_FASTCALL CreateShader(const void* data, SHADER_VER profile)=0;
    // Sets current shader
    virtual char BSS_FASTCALL SetShader(void* shader, SHADER_VER profile)=0;
    // Returns true if shader version is supported
    virtual bool BSS_FASTCALL ShaderSupported(SHADER_VER profile)=0;

    BSS_FORCEINLINE static void _MatrixTranslate(float(&out)[4][4], float x, float y, float z) { out[3][0]=x; out[3][1]=y; out[3][2]=z; } //This is the transpose of what is NORMALLY done, presumably due to the order of multiplication
    BSS_FORCEINLINE static void _MatrixScale(float(&out)[4][4], float x, float y, float z) { out[0][0]=x; out[1][1]=y; out[2][2]=z; }
    BSS_FORCEINLINE static void _MatrixRotateZ(float(&out)[4][4], float angle) { float ca=cos(angle); float sa=sin(angle); out[0][0]=ca; out[1][0]=-sa; out[0][1]=sa; out[1][1]=ca; } //Again, we need the transpose
    BSS_FORCEINLINE static void BSS_FASTCALL _inversetransform(float(&mat)[4][4]) { mat[3][0]=(-mat[3][0]); mat[3][1]=(-mat[3][1]); }
    BSS_FORCEINLINE static void BSS_FASTCALL _inversetransformadd(float(&mat)[4][4], const float(&add)[4][4]) { mat[3][0]=add[3][0]-mat[3][0]; mat[3][1]=add[3][1]-mat[3][1]; mat[3][2]=add[3][2]; }
    static const float identity[4][4];

    struct SHADER_LIBRARY
    {
      psShader* IMAGE;
      psShader* IMAGE2;
      psShader* IMAGE3;
      psShader* IMAGE4;
      psShader* RECT; // Same as IMAGE but with no texture lookups
      psShader* CIRCLE;
      psShader* POLYGON;
      psShader* LINE;
      psShader* PARTICLE;
    } library;

    psVeciu screendim;

  protected:
    const psTex* _defaultrt;
  };

  struct PS_DLLEXPORT psDriverHold
  { 
    static psDriver* GetDriver(); // Cannot be inline'd for DLL reasons.

  protected:
    static psDriver* _driver; //public pointer to the driver
  };
}

#endif