// Copyright �2018 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in ps_dec.h

#ifndef __IMAGE_H__PS__
#define __IMAGE_H__PS__

#include "psTextured.h"
#include "psColor.h"
#include "psSolid.h"

namespace planeshader {
  // Represents a renderable image with UV source coordinates
  class PS_DLLEXPORT psImage : public psSolid, public psTextured, public psDriverHold
  {
  public:
    psImage(const psImage& copy);
    psImage(psImage&& mov);
    explicit psImage(psTex* tex = 0, const psVec3D& position=VEC3D_ZERO, FNUM rotation=0.0f, const psVec& pivot=VEC_ZERO, psFlag flags=0, int zorder=0, psStateblock* stateblock=0, psShader* shader=0, psLayer* pass = 0, const psVec& scale=VEC_ONE, uint32_t color=0xFFFFFFFF);
    explicit psImage(const char* file, const psVec3D& position=VEC3D_ZERO, FNUM rotation=0.0f, const psVec& pivot=VEC_ZERO, psFlag flags=0, int zorder=0, psStateblock* stateblock=0, psShader* shader=0, psLayer* pass = 0, const psVec& scale=VEC_ONE, uint32_t color=0xFFFFFFFF);
    virtual ~psImage();
    void AddSource(const psRect& r = RECT_UNITRECT);
    void ClearSources();
    inline psRect GetSource(size_t index=0) const { if(index>=_uvs.Capacity() || index >= _tex.Capacity()) return RECT_ZERO; return _uvs[index]*_tex[index]->GetDim(); }
    inline const psRect& GetRelativeSource(size_t index=0) const { return _uvs[index]; }
    inline const psRect* GetRelativeSources() const { return _uvs; }
    inline void SetRelativeSource(const psRect& uv, size_t index = 0) { _setuvs(index+1); _uvs[index] = uv; if(!index) _recalcdim(); }
    inline void SetSource(const psRect& uv, size_t index=0) { _setuvs(index+1); _uvs[index] = uv/_tex[index]->GetDim(); if(!index) _recalcdim(); }
    size_t NumSources() const { return _uvs.Capacity(); }
    virtual void SetTexture(psTex* tex, size_t index = 0) override;
    void ApplyEdgeBuffer(); // Applies a 1 pixel edge buffer to the image by expanding the UV coordinate out by one pixel at the border to prevent artifacts caused by rasterization.
    inline psColor32 GetColor() const { return _color; }
    inline void SetColor(psColor32 color) { _color = color; }
    inline virtual psImage* Clone() const { return new psImage(*this); } //Clone function

    psImage& operator =(const psImage& right);
    psImage& operator =(psImage&& right);

  protected:
    virtual void _render(const psTransform2D& parent) override;
    void _setuvs(size_t size);
    void _recalcdim();

    bss::CompactArray<psRect> _uvs;
    psColor32 _color;
  };
}

#endif