// Copyright �2018 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in ps_dec.h

#include "psEngine.h"
#include "psVector.h"
#include "bss-util/algo.h"
#include "psTex.h"

using namespace planeshader;
using namespace bss;

psQuadraticHull::psQuadraticHull() : psRenderable(0, 0, 0, _driver->library.CURVE, 0) {}
psQuadraticHull::~psQuadraticHull() {}

void psQuadraticHull::_render(const psTransform2D& parent)
{
  static psBufferObj bufobj = *_driver->CreateBufferObj(&bufobj, CURVEBUFSIZE, sizeof(QuadVertex), USAGE_VERTEX | USAGE_DYNAMIC, 0);
  if(parent == psTransform2D::Zero)
    _driver->DrawArray(GetShader(), GetStateblock(), _verts.begin(), _verts.Length(), &bufobj, 0, TRIANGLELIST, GetFlags());
  else
  {
    psMatrix m;
    parent.GetMatrix(m);
    _driver->PushTransform(m);
    _driver->DrawArray(GetShader(), GetStateblock(), _verts.begin(), _verts.Length(), &bufobj, 0, TRIANGLELIST, GetFlags());
    _driver->PopTransform();
  }
}

void psQuadraticHull::Clear()
{
  _verts.Clear();
}

void psQuadraticHull::SetVert(float (&v)[4], psVec& x, float thickness)
{
  v[0] = x.x;
  v[1] = x.y;
  v[2] = 0;
  v[3] = thickness;
}

void psQuadraticHull::AppendQuadraticCurve(psVec p0, psVec p1, psVec p2, float thickness, uint32_t color, char cap)
{
  const float BUFFER = 1.0;
  psVec p[3] = { p0, p1, p2 };

  for(int i = 0; i < 3; ++i) p[i] -= p0;
  float angle = atan2(-p[2].y, p[2].x);
  for(int i = 0; i < 3; ++i) p[i] = p[i].Rotate(angle, 0, 0);

  // Now that we've aligned the curve, find an axis-aligned bounding box for it. 
  psVec t = (p[0] - p[1]) / (p[0] - 2.0f*p[1] + p[2]); // Gets the roots of the derivative, one t value for both x and y.
  if(!isfinite(t.x)) t.x = 0;
  if(!isfinite(t.y)) t.y = 0;

  t = psVec(bssclamp(t.x, 0, 1), bssclamp(t.y, 0, 1)); // Clamp to [0,1]
  psVec pmax = QuadraticBezierCurve<psVec, psVec>(t, p[0], p[1], p[2]); // Simultaneously plug in t values for x and y to get our candidate maximum point.
  psVec pmin = pmax;
  if(p[0].x > pmax.x) pmax.x = p[0].x;
  if(p[0].y > pmax.y) pmax.y = p[0].y;
  if(p[2].x > pmax.x) pmax.x = p[2].x;
  if(p[2].y > pmax.y) pmax.y = p[2].y;
  if(p[0].x < pmin.x) pmin.x = p[0].x;
  if(p[0].y < pmin.y) pmin.y = p[0].y;
  if(p[2].x < pmin.x) pmin.x = p[2].x;
  if(p[2].y < pmin.y) pmin.y = p[2].y;
  pmax += psVec(thickness + BUFFER);
  pmin -= psVec(thickness + BUFFER);

  psVec rect[4] = { pmin, { pmax.x, pmin.y}, { pmin.x, pmax.y }, pmax };
    //float m = (p[1].x - p[0].x) / (p[1].y - p[0].y); // y and x are reversed here to give us a perpendicular line.
    //float b = (p[1].y*p[0].x - p[0].y*p[1].x) / (p[1].y - p[0].y);
    //rect[0].x = std::min((rect[0].y - b) / m, rect[0].x + thickness + 1);
    //rect[2].x = std::min((rect[2].y - b) / m, rect[0].x + thickness + 1);
    //m = (p[2].x - p[1].x) / (p[2].y - p[1].y);
    //b = (p[2].y*p[1].x - p[1].y*p[2].x) / (p[2].y - p[1].y);
    //rect[1].x = std::max((rect[1].y - b) / m, rect[1].x - thickness - 1);
    //rect[3].x = std::max((rect[3].y - b) / m, rect[3].x - thickness - 1);

  for(int i = 0; i < 4; ++i) rect[i] = rect[i].Rotate(-angle, 0, 0);
  for(int i = 0; i < 4; ++i) rect[i] += p0;

  static const int VSIZE = 6;
  psVec verts[VSIZE] = { rect[0], rect[1], rect[2], rect[2], rect[1], rect[3] };

  _verts.SetLength(_verts.Length() + VSIZE);
  QuadVertex* v = _verts.end() - VSIZE;
  for(int i = 0; i < VSIZE; ++i)
  {
    v[i].x = verts[i].x;
    v[i].y = verts[i].y;
    v[i].z = 0;
    v[i].t = 1;
    SetVert(v[i].p0, p0, thickness * ((cap & 1) ? 1 : -1));
    SetVert(v[i].p1, p1, thickness);
    SetVert(v[i].p2, p2, thickness * ((cap & 2) ? 1 : -1));
    v[i].color = color;
  }
}

psQuadraticCurve::psQuadraticCurve(psVec p0, psVec p1, psVec p2, float thickness, uint32_t color) : _color(color), _thickness(thickness) { Set(p0, p1, p2); }
psQuadraticCurve::psQuadraticCurve(psVec(&p)[3], float thickness, uint32_t color) : _color(color), _thickness(thickness) { Set(p); }
psQuadraticCurve::~psQuadraticCurve() {}
void psQuadraticCurve::Set(psVec p0, psVec p1, psVec p2)
{
  _p[0] = p0;
  _p[1] = p1;
  _p[2] = p2;
  Clear();
  AppendQuadraticCurve(_p[0], _p[1], _p[2], _thickness, _color.color, 3);
}
void psQuadraticCurve::Set(psVec(&p)[3])
{
  Set(p[0], p[1], p[2]);
}

template<typename T, typename FN>
inline static void IterateCubic(const T(&P0)[2], const T(&P1)[2], const T(&P2)[2], const T(&P3)[2], FN fn, int n)
{
  T N1[2];
  T N2[2];
  T N3[2];
  T R1[2];
  T R2[2];
  T dt = 1.0 / (n + 1);

  T W0[2] = { P0[0], P0[1] };
  T W1[2] = { P1[0], P1[1] };
  T W2[2] = { P2[0], P2[1] };
  T W3[2] = { P3[0], P3[1] };
  for(int i = 0; i < n; ++i)
  {
    SplitCubic(dt * (i + 1), W0, W1, W2, W3, N1, N2, N3, R1, R2);

    T C[2] = { (N1[0] + N2[0]) / 2, (N1[1] + N2[1]) / 2 };
    fn(W0, C, N3);
    for(int i = 0; i < 2; ++i)
    {
      W0[i] = N3[i];
      W1[i] = R1[i];
      W2[i] = R2[i];
    }
  }

  T C[2] = {(W1[0] + W2[0]) / 2, (W1[1] + W2[1]) / 2 };
  fn(W0, C, W3);
}

psCubicCurve::psCubicCurve(psVec p0, psVec p1, psVec p2, psVec p3, float thickness, uint32_t color, float maxerr) : _color(color), _thickness(thickness), _maxerr(maxerr) { Set(p0, p1, p2, p3); }
psCubicCurve::psCubicCurve(psVec(&p)[4], float thickness, uint32_t color) : _color(color), _thickness(thickness) { Set(p); }
psCubicCurve::~psCubicCurve() {}
void psCubicCurve::Set(psVec p0, psVec p1, psVec p2, psVec p3)
{
  _p[0] = p0;
  _p[1] = p1;
  _p[2] = p2;
  _p[3] = p3;

  Clear();
  //IterateCubic<float>(p0.v, p1.v, p2.v, p3.v, Delegate<void, const float(&)[2], const float(&)[2], const float(&)[2]>::From<psCubicCurve, &psCubicCurve::_addquad>(this), 20);
  ApproxCubic<float>(p0.v, p1.v, p2.v, p3.v, Delegate<void, const float(&)[2], const float(&)[2], const float(&)[2]>::From<psCubicCurve, &psCubicCurve::_addquad>(this), _maxerr);
}
void psCubicCurve::Set(psVec(&p)[4])
{
  Set(p[0], p[1], p[2], p[3]);
}
void psCubicCurve::_addquad(const float(&P0)[2], const float(&P1)[2], const float(&P2)[2])
{
  AppendQuadraticCurve(psVec(P0), psVec(P1), psVec(P2), _thickness, _color.color, !_verts.Length() | (psVec(P2) == _p[3])*2);
}

psRoundRect::psRoundRect(const psRoundRect& copy) : psSolid(copy), _color(copy._color), _outline(copy._outline), _corners(copy._corners), _edge(copy._edge) {}
psRoundRect::psRoundRect(psRoundRect&& mov) : psSolid(std::move(mov)), _color(mov._color), _outline(mov._outline), _corners(mov._corners), _edge(mov._edge) {}
psRoundRect::psRoundRect(const psRectRotateZ& rect, psFlag flags, int zorder, psStateblock* stateblock, psShader* shader, psLayer* pass, const psVec& scale) :
  psSolid(psVec3D(rect.left, rect.top, rect.z), rect.rotation, rect.pivot, flags, zorder, !stateblock ? STATEBLOCK_LIBRARY::PREMULTIPLIED : stateblock,
  !shader?_driver->library.ROUNDRECT:shader, pass, scale), _outline(0), _edge(-1), _corners(0,0,0,0), _color(0xFFFFFFFF)
{
  SetDim(rect.Dim());
}
psRoundRect::~psRoundRect(){}

psRoundRect& psRoundRect::operator=(const psRoundRect& copy)
{
  psSolid::operator=(copy);
  _color = copy._color;
  _outline = copy._outline;
  _edge = copy._edge;
  _corners = copy._corners;
  return *this;
}
psRoundRect& psRoundRect::operator=(psRoundRect&& mov)
{
  psSolid::operator=(std::move(mov));
  _color = mov._color;
  _outline = mov._outline;
  _edge = mov._edge;
  _corners = mov._corners;
  return *this;
}

void psRoundRect::DrawRoundRect(psShader* shader, psStateblock* stateblock, const psRectRotateZ& rect, const psRect& corners, psFlag flags, psColor32 color32, psColor32 outline32, float edge)
{
  static psBufferObj bufobj = *_driver->CreateBufferObj(&bufobj, RRBUFSIZE, sizeof(RRVertex), USAGE_VERTEX | USAGE_DYNAMIC, 0);
  psBatchObj* obj = _driver->DrawBatchBegin(shader, !stateblock ? 0 : stateblock->GetSB(), flags, &bufobj, 0, POINTLIST, _driver->PeekTransform(), 1);

  uint32_t color;
  uint32_t outline;
  color32.WriteFormat(FMT_R8G8B8A8, &color);
  outline32.WriteFormat(FMT_R8G8B8A8, &outline);

  RRVertex* vert = (RRVertex*)obj->buffer.get();
  *vert = { rect.left, rect.top, rect.z, rect.rotation,
  { rect.right - rect.left, rect.bottom - rect.top, rect.pivot.x, rect.pivot.y },
    corners, edge, color, outline };
  ++obj->buffer.nvert;
}
void psRoundRect::_render(const psTransform2D& parent)
{
  DrawRoundRect(GetShader(), _stateblock, GetCollisionRect(parent), _corners, GetFlags(), _color, _outline, _edge);
}

psRoundTri::psRoundTri(const psRoundTri& copy) : psSolid(copy), _color(copy._color), _outline(copy._outline), _corners(copy._corners), _edge(copy._edge) {}
psRoundTri::psRoundTri(psRoundTri&& mov) : psSolid(std::move(mov)), _color(mov._color), _outline(mov._outline), _corners(mov._corners), _edge(mov._edge) {}
psRoundTri::psRoundTri(const psRectRotateZ& rect, psFlag flags, int zorder, psStateblock* stateblock, psShader* shader, psLayer* pass, const psVec& scale) :
  psSolid(psVec3D(rect.left, rect.top, rect.z), rect.rotation, rect.pivot, flags, zorder, !stateblock ? STATEBLOCK_LIBRARY::PREMULTIPLIED : stateblock,
    !shader ? _driver->library.ROUNDTRI : shader, pass, scale), _outline(0), _edge(-1), _corners(0, 0, 0, 0), _color(0xFFFFFFFF)
{
  SetDim(rect.Dim());
}
psRoundTri::~psRoundTri() {}

psRoundTri& psRoundTri::operator=(const psRoundTri& copy)
{
  psSolid::operator=(copy);
  _color = copy._color;
  _outline = copy._outline;
  _edge = copy._edge;
  _corners = copy._corners;
  return *this;
}
psRoundTri& psRoundTri::operator=(psRoundTri&& mov)
{
  psSolid::operator=(std::move(mov));
  _color = mov._color;
  _outline = mov._outline;
  _edge = mov._edge;
  _corners = mov._corners;
  return *this;
}

void psRoundTri::DrawRoundTri(psShader* shader, psStateblock* stateblock, const psRectRotateZ& rect, const psRect& corners, psFlag flags, psColor32 color32, psColor32 outline32, float edge)
{
  static psBufferObj bufobj = *_driver->CreateBufferObj(&bufobj, RTRIBUFSIZE, sizeof(RRVertex), USAGE_VERTEX | USAGE_DYNAMIC, 0);
  psBatchObj* obj = _driver->DrawBatchBegin(shader, !stateblock ? 0 : stateblock->GetSB(), flags, &bufobj, 0, POINTLIST, _driver->PeekTransform(), 1);

  uint32_t color;
  uint32_t outline;
  color32.WriteFormat(FMT_R8G8B8A8, &color);
  outline32.WriteFormat(FMT_R8G8B8A8, &outline);

  RRVertex* vert = (RRVertex*)obj->buffer.get();
  *vert = { rect.left, rect.top, rect.z, rect.rotation,
  { rect.right - rect.left, rect.bottom - rect.top, rect.pivot.x, rect.pivot.y },
    corners, edge, color, outline };
  ++obj->buffer.nvert;
}
void psRoundTri::_render(const psTransform2D& parent)
{
  DrawRoundTri(GetShader(), _stateblock, GetCollisionRect(parent), _corners, GetFlags(), _color, _outline, _edge);
}

psRenderCircle::psRenderCircle(const psRenderCircle& copy) : psSolid(copy), _color(copy._color), _outline(copy._outline), _arcs(copy._arcs), _edge(copy._edge) {}
psRenderCircle::psRenderCircle(psRenderCircle&& mov) : psSolid(std::move(mov)), _color(mov._color), _outline(mov._outline), _arcs(mov._arcs), _edge(mov._edge) {}
psRenderCircle::psRenderCircle(float radius, const psVec3D& position, psFlag flags, int zorder, psStateblock* stateblock, psShader* shader, psLayer* pass, const psVec& scale) :
  psSolid(position, 0, VEC_HALF, flags, zorder, !stateblock ? STATEBLOCK_LIBRARY::PREMULTIPLIED : stateblock, !shader ? _driver->library.CIRCLE : shader, pass, scale),
  _outline(0), _edge(0), _arcs(0, PI_DOUBLEf, 0, PI_DOUBLEf), _color(0xFFFFFFFF)
{
  SetDim(psVec(radius*2));
}
psRenderCircle::~psRenderCircle() {}

psRenderCircle& psRenderCircle::operator=(const psRenderCircle& copy)
{
  psSolid::operator=(copy);
  _color = copy._color;
  _outline = copy._outline;
  _edge = copy._edge;
  _arcs = copy._arcs;
  return *this;
}
psRenderCircle& psRenderCircle::operator=(psRenderCircle&& mov)
{
  psSolid::operator=(std::move(mov));
  _color = mov._color;
  _outline = mov._outline;
  _edge = mov._edge;
  _arcs = mov._arcs;
  return *this;
}
void psRenderCircle::DrawCircle(psShader* shader, psStateblock* stateblock, const psRectRotateZ& rect, const psRect& arcs, psFlag flags, psColor32 color32, psColor32 outline32, float edge)
{
  static psBufferObj bufobj = *_driver->CreateBufferObj(&bufobj, CIRCLEBUFSIZE, sizeof(CircleVertex), USAGE_VERTEX | USAGE_DYNAMIC, 0);
  psBatchObj* obj = _driver->DrawBatchBegin(shader, !stateblock ? 0 : stateblock->GetSB(), flags, &bufobj, 0, POINTLIST, _driver->PeekTransform(), 1);

  uint32_t color;
  uint32_t outline;
  color32.WriteFormat(FMT_R8G8B8A8, &color);
  outline32.WriteFormat(FMT_R8G8B8A8, &outline);

  CircleVertex* vert = (CircleVertex*)obj->buffer.get(); // we don't need to check length here because we reserved 1 vertex in DrawBatchBegin
  *vert = { rect.left, rect.top, rect.z, rect.rotation,
  { rect.right - rect.left, rect.bottom - rect.top, rect.pivot.x, rect.pivot.y },
    arcs, edge, color, outline };
  ++obj->buffer.nvert;
}

void psRenderCircle::_render(const psTransform2D& parent)
{
  DrawCircle(GetShader(), _stateblock, GetCollisionRect(parent), _arcs, GetFlags(), _color, _outline, _edge);
}