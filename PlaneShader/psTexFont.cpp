// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#include "psTexFont.h"
#include "psTex.h"

using namespace planeshader;

psVec psTexFont::DrawText(const int* text, psRect area, unsigned short drawflags, FNUM Z, unsigned int color, FLAG_TYPE flags, psVec dim)
{
  float linewidth;
  float curwidth = 0.0f;
  float linestart;
  psVec cur;
  char c;
  const psGlyph* g;
  const int* pos;
  const int* peek = text;
  psVec maxdim = area.GetDimensions();
  psVec texdim;
  unsigned char svar=_textures.Size();
  psRectRotateZ rect(0, 0, 0, 0, 0, VEC_ZERO, Z);

  if((dim.y == 0.0f && (drawflags&TDT_BOTTOM || drawflags&TDT_VCENTER || drawflags&TDT_CLIP)) ||
    (dim.x == 0.0f && (drawflags&TDT_RIGHT || drawflags&TDT_CENTER || drawflags&TDT_CLIP)))
  {
    dim = VEC_ZERO;
    while(peek)
    {
      linewidth = _getlinewidth(peek, maxdim.x, drawflags, curwidth);
      if(linewidth > dim.x) dim.x = linewidth;
      dim.y += _lineheight;
    }
  }
  if(maxdim.x == 0.0f) maxdim.x = dim.x;
  if(maxdim.y == 0.0f) maxdim.y = dim.y;
  area.right = area.left + maxdim.x;
  area.bottom = area.top + maxdim.y;

  if(drawflags&TDT_CLIP) //this applies a scissor rect to clip the text (text renders so fast anything more complicated would be too expensive to be worth it)
  {
    psRectl transfer;
    transfer.topleft = _driver->TransformPoint(psVec3D(area.top, area.left, 0), flags).xy;
    transfer.bottomright = _driver->TransformPoint(psVec3D(area.bottom, area.right, 0), flags).xy;
    _driver->PushScissorRect(transfer);
  }

  for(unsigned char i = 0; i < svar; ++i)
  {
    pos = text;
    peek = text;

    if(drawflags&TDT_BOTTOM)
      cur.y = area.bottom - dim.y;
    else if(drawflags&TDT_VCENTER)
      cur.y = area.top + (maxdim.y - dim.y)*0.5f;
    else
      cur.y = area.top;

    curwidth = 0.0f;
    _driver->DrawRectBatchBegin(&_textures[i], 1, flags);
    texdim = _textures[i]->GetDim();

    while(pos)
    {
      linewidth = _getlinewidth(peek, maxdim.x, drawflags, curwidth) + area.left;

      if(drawflags&TDT_RIGHT)
        cur.x = area.right - linewidth;
      else if(drawflags&TDT_CENTER)
        cur.x = area.left + (maxdim.x - linewidth)*0.5f;
      else
        cur.x = area.left;
      linestart = cur.x;

      while(pos && cur.x < linewidth)
      {
        c = *(pos++);
        g = _glyphs[c];
        if(!g) continue; // note: we don't bother attempting to load glyphs here because they should've already been loaded by _getlinewidth
        if(g->texnum != i) { cur.x += g->width; continue; }

        rect.left = cur.x + g->advance;
        rect.top= cur.y + g->ascender + _lineheight;

        if(flags&TDT_RTL) // implement right to left rendering by flipping over the middle axis
        {
          rect.left = linewidth - (rect.left - linestart) - (g->uv.right-g->uv.left)*texdim.x;
        }
        if(flags&TDT_PIXELSNAP)
        {
          rect.left = (float)bss_util::fFastRound(rect.left);
          rect.top = (float)bss_util::fFastRound(rect.top);
        }

        rect.right = rect.left + (g->uv.right-g->uv.left)*texdim.x;
        rect.bottom = rect.top + (g->uv.bottom-g->uv.top)*texdim.y;

        _driver->DrawRectBatch(rect, g->uv, color);

        cur.x += g->width;
      }
    }
    _driver->DrawRectBatchEnd();
  }

  if(drawflags&TDT_CLIP)
    _driver->PopScissorRect();

  return dim;
}

float psTexFont::_getlinewidth(const int*& text, float maxwidth, unsigned short drawflags, float& cur)
{
  bool dobreak = maxwidth != 0.0f && (drawflags&TDT_CHARBREAK || drawflags&TDT_WORDBREAK);
  float width = 0.0f;
  int c;
  const psGlyph* g;
  while(text)
  {
    c = *text;
    g = _glyphs[c];
    if(!g)
    {
      g = _loadglyph(c);
      if(!g) continue; // Note: Bad glyphs usually just have 0 width, so we don't have to check for them.
    }
    if(c == '\n' || (dobreak && (cur + g->width)>maxwidth))
    {
      cur -= width;
      return width;
    }
    if(!(drawflags&TDT_WORDBREAK)) width = isspace(c)?cur:(cur + g->width);
    else if(isspace(c)) width = cur;
    cur += g->width;

    ++text;
  }
  return cur;
}
psGlyph* psTexFont::_loadglyph(unsigned int codepoint)
{
  return 0;
}

void psTexFont::CalcTextDim(const int* text, psVec& dest, unsigned short drawflags)
{
  float cur = 0.0f;
  float height = (!text || !text[0])?0.0f:_lineheight;
  psVec maxdim = dest;
  if(maxdim.x != 0.0f && maxdim.y != 0.0f) maxdim = VEC_ZERO;
  float width = 0.0f;

  float w;
  while(text)
  {
    w = _getlinewidth(text, maxdim.x, drawflags, cur);
    if(w > width) width = w;
    height += _lineheight;
  }

  if(maxdim.x == 0.0f) dest.x = width;
  if(maxdim.y == 0.0f) dest.y = height;
}

void psTexFont::AddGlyph(int character, const psGlyph& glyph)
{
  _glyphs.Insert(character, glyph);
}

unsigned char psTexFont::AddTexture(psTex* tex){ tex->Grab(); _textures.Insert(tex, _textures.Size()); return _textures.Size()-1; }
void psTexFont::DestroyThis(){ delete this; }

psTexFont* psTexFont::CreateTexFont(psTex* tex, float lineheight)
{
  return new psTexFont(tex, lineheight);
}

psTexFont::psTexFont(const psTexFont& copy) : _lineheight(copy._lineheight), _glyphs(copy._glyphs)
{
  for(int i = 0; i < copy._textures.Size(); ++i)
  {
    copy._textures[i]->Grab();
    _textures.Insert(copy._textures[i], _textures.Size());
  }
}
psTexFont::psTexFont(psTexFont&& mov) : _lineheight(mov._lineheight), _textures(std::move(mov._textures)), _glyphs(std::move(mov._glyphs)) {}
psTexFont::psTexFont(psTex* tex, float lineheight) : _lineheight(lineheight), _textures(1) { _textures[0] = tex; Grab(); }
psTexFont::~psTexFont() { for(unsigned int i = 0; i < _textures.Size(); ++i) _textures[i]->Drop(); }