// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#ifndef __ELLIPSE_H__PS__
#define __ELLIPSE_H__PS__

#include "psLine.h"

namespace planeshader {
  template <class T>
  struct BSS_COMPILER_DLLEXPORT psEllipseT
  {
    typedef bss_util::Vector<T, 2> VEC;
    inline psEllipseT() {} //The following constructors allow for implicit conversion between types
    template<class U>
    inline psEllipseT(const psEllipseT<U>& other) : pos(other.pos), axes(other.axes) {}
    template<class U>
    inline explicit psEllipseT(const psCircleT<U>& other) : pos(other.pos), axes(other.r) {}
    inline psEllipseT(T X, T Y, T A, T B) : pos(X, Y), axes(A, B) {}
    inline psEllipseT(const VEC& Pos, T A, T B) : pos(Pos), axes(A, B) {}
    inline psEllipseT(const VEC& Pos, const VEC& Axes) : pos(Pos), axes(Axes) {}
    inline T Area() const { return (T)(PI*a*b); }
    inline bool BSS_FASTCALL IntersectPoint(T X, T Y) const { return IntersectEllipse(x, y, a, b, X, Y); }
    inline bool BSS_FASTCALL IntersectPoint(const VEC& point) const { return IntersectEllipse(x, y, a, b, point.x, point.y); }
    inline bool BSS_FASTCALL IntersectCircle(const psCircleT<T>& other) const { return psCircleT<T>::CircleEllipseIntersect(x, y, a, b, other.x, other.y, other.r); }
    inline bool BSS_FASTCALL IntersectCircle(const cVec& pos, T R) const { return psCircleT<T>::CircleEllipseIntersect(x, y, a, b, pos.x, pos.y, R); }
    inline bool BSS_FASTCALL IntersectCircle(T X, T Y, T R) const { return psCircleT<T>::CircleEllipseIntersect(x, y, a, b, X, Y, R); }
    inline bool BSS_FASTCALL IntersectLineInf(T X1, T Y1, T X2, T Y2) const { T s=a/b; PointLineInfDistSqr(0, 0, X1-x, (Y1-y)*s, X2-x, (Y2-y)*s)<=(a*a); } //transforms entire coordinate system so ellipse is a circle
    inline bool BSS_FASTCALL IntersectLine(T X1, T Y1, T X2, T Y2) const { T s=a/b; PointLineDistSqr(0, 0, X1-x, (Y1-y)*s, X2-x, (Y2-y)*s)<=(a*a); }
    inline bool BSS_FASTCALL IntersectLine(psLineT<T>& line) const { return IntersectLine(line.x1, line.y1, line.x2, line.y2); }
    inline bool BSS_FASTCALL IntersectRect(T left, T top, T right, T bottom) const { T s=a/b; return psCircleT<T>::CircleRectIntersect(x, y*s, a, left, top*s, right, bottom*s); }
    inline bool BSS_FASTCALL IntersectRect(const T(&rect)[4]) const { T s=a/b; return psCircleT<T>::CircleRectIntersect(x, y*s, a, rect[0], rect[1]*s, rect[2], rect[3]*s); }
    inline bool BSS_FASTCALL IntersectEllipse(const cEllipseT<T>& other) const { return IntersectEllipse(other.x, other.y, other.a, other.b); }
    inline bool BSS_FASTCALL IntersectEllipse(T X, T Y, T A, T B) const { T s=A/B; return psCircleT<T>::CircleEllipseIntersect(x, y*s, a, b*s, X, Y*s, A); }
    inline void BSS_FASTCALL NearestPoint(T X, T Y, T& outX, T& outY) const { EllipseNearestPoint(a, b, X-x, Y-y, outX, outY); outX+=x; outY+=y; }
    inline void BSS_FASTCALL NearestPoint(const cVec& pos, T& outX, T& outY) const { EllipseNearestPoint(a, b, pos.x-x, pos.y-y, outX, outY); outX+=x; outY+=y; }

    union {
      struct {
        VEC pos;
        VEC axes;
      };
      struct {
        T x;
        T y;
        T a;
        T b;
      };
    };
  };

  typedef psEllipseT<float> psEllipse; //default typedef
  typedef psEllipseT<int> psEllipsei;
  typedef psEllipseT<double> psEllipsed;
  typedef psEllipseT<short> psEllipses;
  typedef psEllipseT<long> psEllipsel;
  typedef psEllipseT<unsigned int> psEllipseiu;
  typedef psEllipseT<unsigned short> psEllipsesu;
  typedef psEllipseT<unsigned long> psEllipselu;
}

#endif