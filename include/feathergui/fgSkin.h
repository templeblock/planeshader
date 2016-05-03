// Copyright �2016 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "feathergui.h"

#ifndef _FG_SKIN_H__
#define _FG_SKIN_H__

#include "fgStyle.h"
#include "fgControl.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _FG_STYLE_LAYOUT {
  char* type;
  char* name;
  fgTransform transform;
  fgFlag flags;
  fgStyle style; // style overrides
  int order;
} fgStyleLayout;

struct __kh_fgSkins_t;
struct __kh_fgStyles_t;

typedef struct _FG_SKIN
{
  struct _FG_SKIN* inherit;
  fgStyle style; // style overrides
  fgVector resources; // type: void*
  fgVector fonts;
  fgDeclareVector(fgStyleLayout, StyleLayout) children; // type: fgStyleLayout
  fgDeclareVector(fgStyle, Style) styles; // type: fgStyle
  struct __kh_fgSkins_t* skinmap;
} fgSkin;


FG_EXTERN void FG_FASTCALL fgSubskin_Init(fgSkin* self, int index);
FG_EXTERN void FG_FASTCALL fgSkin_Init(fgSkin* self);
FG_EXTERN void FG_FASTCALL fgSkin_Destroy(fgSkin* self);
FG_EXTERN size_t FG_FASTCALL fgSkin_AddResource(fgSkin* self, void* resource);
FG_EXTERN char FG_FASTCALL fgSkin_RemoveResource(fgSkin* self, FG_UINT resource);
FG_EXTERN void* FG_FASTCALL fgSkin_GetResource(const fgSkin* self, FG_UINT resource);
FG_EXTERN size_t FG_FASTCALL fgSkin_AddFont(fgSkin* self, void* font);
FG_EXTERN char FG_FASTCALL fgSkin_RemoveFont(fgSkin* self, FG_UINT font);
FG_EXTERN void* FG_FASTCALL fgSkin_GetFont(const fgSkin* self, FG_UINT font);
FG_EXTERN size_t FG_FASTCALL fgSkin_AddChild(fgSkin* self, const char* type, const char* name, fgFlag flags, const fgTransform* transform);
FG_EXTERN char FG_FASTCALL fgSkin_RemoveChild(fgSkin* self, FG_UINT child);
FG_EXTERN fgStyleLayout* FG_FASTCALL fgSkin_GetChild(const fgSkin* self, FG_UINT child);
FG_EXTERN size_t FG_FASTCALL fgSkin_AddStyle(fgSkin* self, const char* name);
FG_EXTERN char FG_FASTCALL fgSkin_RemoveStyle(fgSkin* self, FG_UINT style);
FG_EXTERN fgStyle* FG_FASTCALL fgSkin_GetStyle(const fgSkin* self, FG_UINT style);
FG_EXTERN fgSkin* FG_FASTCALL fgSkin_AddSkin(fgSkin* self, const char* name);
FG_EXTERN char FG_FASTCALL fgSkin_RemoveSkin(fgSkin* self, const char* name);
FG_EXTERN fgSkin* FG_FASTCALL fgSkin_GetSkin(const fgSkin* self, const char* name);

FG_EXTERN void FG_FASTCALL fgStyleLayout_Init(fgStyleLayout* self, const char* type, const char* name, fgFlag flags, const fgTransform* transform);
FG_EXTERN void FG_FASTCALL fgStyleLayout_Destroy(fgStyleLayout* self);

FG_EXTERN fgSkin* FG_FASTCALL fgSkins_LoadFileUBJSON(struct __kh_fgSkins_t* self, const char* file);
FG_EXTERN fgSkin* FG_FASTCALL fgSkins_LoadUBJSON(struct __kh_fgSkins_t* self, const void* data, FG_UINT length);

#ifdef  __cplusplus
}
#endif

#endif