// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#include "psStateblock.h"
#include "psDriver.h"
#include "bss-util/profiler.h"
#include <stdarg.h>
#include <algorithm>

using namespace planeshader;

STATEINFO::BLOCKHASH STATEINFO::_blocks;
psStateblock* psStateblock::DEFAULT = 0;
psStateblock* STATEBLOCK_LIBRARY::GLOW = 0;
psTexblock* STATEBLOCK_LIBRARY::UVBORDER = 0;
psTexblock* STATEBLOCK_LIBRARY::UVMIRROR = 0;
psTexblock* STATEBLOCK_LIBRARY::UVMIRRORONCE = 0;
psTexblock* STATEBLOCK_LIBRARY::UVWRAP = 0;
psStateblock* STATEBLOCK_LIBRARY::SUBPIXELBLEND = 0;
psStateblock* STATEBLOCK_LIBRARY::SUBPIXELBLEND1 = 0;
psStateblock* STATEBLOCK_LIBRARY::PREMULTIPLIED = 0;

STATEINFO::STATEINFOS* STATEINFO::Exists(STATEINFOS* infos)
{
  khiter_t iter = _blocks.Iterator(infos);
  if(_blocks.ExistsIter(iter))
    return _blocks.GetKey(iter);
  _blocks.Insert(infos);
  return 0;
}
psStateblock::psStateblock(const STATEINFO* infos, unsigned int numstates) : bss_util::cArray<STATEINFO, unsigned short>(numstates)
{
  memcpy(_array, infos, numstates*sizeof(STATEINFO));
  std::sort<STATEINFO*>(_array, _array+numstates, &STATEINFO::SILESS);
  _sb = psDriverHold::GetDriver()->CreateStateblock(_array, numstates);
}
psStateblock::~psStateblock()
{
  psDriverHold::GetDriver()->FreeResource(_sb, psDriver::RES_STATEBLOCK);
}
void psStateblock::DestroyThis() { delete this; }

psStateblock* BSS_FASTCALL psStateblock::Create(unsigned int numstates, ...)
{
  PROFILE_FUNC();
  DYNARRAY(STATEINFO, states, numstates);

  va_list vl;
  va_start(vl, numstates);
  for(unsigned int i = 0; i < numstates; ++i)
    states[i]=*va_arg(vl, const STATEINFO*);
  va_end(vl);

  return Create(states, numstates);
}
psStateblock* BSS_FASTCALL psStateblock::Create(const STATEINFO* infos, unsigned int numstates)
{
  PROFILE_FUNC();
  psStateblock* r = new psStateblock(infos, numstates);
  psStateblock* n;
  if(n = static_cast<psStateblock*>(STATEINFO::Exists(r)))
  {
    delete r;
    r = n;
  }
  r->Grab();
  return r;
}

template<class T>
T* stateblock_combine(bss_util::cArray<STATEINFO, unsigned short>& left, STATEINFO* right, unsigned short num)
{
  DYNARRAY(STATEINFO, states, left.Capacity() + num);
  memcpy(states, (STATEINFO*)left, left.Capacity()*sizeof(STATEINFO));
  memcpy(states + left.Capacity(), (STATEINFO*)right, num*sizeof(STATEINFO));
  return T::Create(states, left.Capacity() + num);
}
psStateblock* psStateblock::Combine(psStateblock* other) const
{ 
  return stateblock_combine<psStateblock>((bss_util::cArray<STATEINFO, unsigned short>&)*this, (STATEINFO*)*other, other->Capacity());
}
psStateblock* psStateblock::Append(STATEINFO state) const
{
  return stateblock_combine<psStateblock>((bss_util::cArray<STATEINFO, unsigned short>&)*this, &state, 1);
}

psTexblock::psTexblock(const STATEINFO* infos, unsigned int numstates) : bss_util::cArray<STATEINFO, unsigned short>(numstates)
{
  memcpy(_array, infos, numstates*sizeof(STATEINFO));
  std::sort<STATEINFO*>(_array, _array+numstates, &STATEINFO::SILESS);
  _tb = psDriverHold::GetDriver()->CreateTexblock(_array, numstates);
}
psTexblock::~psTexblock()
{
  psDriverHold::GetDriver()->FreeResource(_tb, psDriver::RES_TEXBLOCK);
}
void psTexblock::DestroyThis() { delete this; }

psTexblock* BSS_FASTCALL psTexblock::Create(unsigned int numstates, ...)
{
  PROFILE_FUNC();
  DYNARRAY(STATEINFO, states, numstates);

  va_list vl;
  va_start(vl, numstates);
  for(unsigned int i = 0; i < numstates; ++i)
    states[i]=*va_arg(vl, const STATEINFO*);
  va_end(vl);

  return Create(states, numstates);
}
psTexblock* BSS_FASTCALL psTexblock::Create(const STATEINFO* infos, unsigned int numstates)
{
  PROFILE_FUNC();
  psTexblock* r = new psTexblock(infos, numstates);
  psTexblock* n;
  if(n = static_cast<psTexblock*>(STATEINFO::Exists(r)))
  {
    delete r;
    r = n;
  }
  r->Grab();
  return r;
}
psTexblock* psTexblock::Combine(psTexblock* other) const
{
  return stateblock_combine<psTexblock>((bss_util::cArray<STATEINFO, unsigned short>&)*this, (STATEINFO*)*other, other->Capacity());
}
psTexblock* psTexblock::Append(STATEINFO state) const
{
  return stateblock_combine<psTexblock>((bss_util::cArray<STATEINFO, unsigned short>&)*this, &state, 1);
}

void STATEBLOCK_LIBRARY::INITLIBRARY()
{
  GLOW = psStateblock::Create(2,
    &STATEINFO(TYPE_BLEND_SRCBLEND, STATEBLEND_ONE),
    &STATEINFO(TYPE_BLEND_DESTBLEND, STATEBLEND_ONE));

  SUBPIXELBLEND = psStateblock::Create(2,
    &STATEINFO(TYPE_BLEND_SRCBLEND, STATEBLEND_BLEND_FACTOR),
    &STATEINFO(TYPE_BLEND_DESTBLEND, STATEBLEND_INV_SRC_COLOR));

  SUBPIXELBLEND1 = psStateblock::Create(2,
    &STATEINFO(TYPE_BLEND_SRCBLEND, STATEBLEND_SRC1_COLOR),
    &STATEINFO(TYPE_BLEND_DESTBLEND, STATEBLEND_INV_SRC_COLOR));
  
  PREMULTIPLIED = psStateblock::Create(2,
    &STATEINFO(TYPE_BLEND_SRCBLEND, STATEBLEND_ONE),
    &STATEINFO(TYPE_BLEND_DESTBLEND, STATEBLEND_INV_SRC_ALPHA));

  UVBORDER = psTexblock::Create(4,
    &STATEINFO(TYPE_SAMPLER_ADDRESSU, STATETEXTUREADDRESS_BORDER),
    &STATEINFO(TYPE_SAMPLER_ADDRESSV, STATETEXTUREADDRESS_BORDER),
    &STATEINFO(TYPE_SAMPLER_ADDRESSW, STATETEXTUREADDRESS_BORDER),
    &STATEINFO(TYPE_SAMPLER_BORDERCOLOR1, 0));

  UVMIRROR = psTexblock::Create(3,
    &STATEINFO(TYPE_SAMPLER_ADDRESSU, STATETEXTUREADDRESS_MIRROR),
    &STATEINFO(TYPE_SAMPLER_ADDRESSV, STATETEXTUREADDRESS_MIRROR),
    &STATEINFO(TYPE_SAMPLER_ADDRESSW, STATETEXTUREADDRESS_MIRROR));

  UVMIRRORONCE = psTexblock::Create(3,
    &STATEINFO(TYPE_SAMPLER_ADDRESSU, STATETEXTUREADDRESS_MIRROR_ONCE),
    &STATEINFO(TYPE_SAMPLER_ADDRESSV, STATETEXTUREADDRESS_MIRROR_ONCE),
    &STATEINFO(TYPE_SAMPLER_ADDRESSW, STATETEXTUREADDRESS_MIRROR_ONCE));

  UVWRAP = psTexblock::Create(3,
    &STATEINFO(TYPE_SAMPLER_ADDRESSU, STATETEXTUREADDRESS_WRAP),
    &STATEINFO(TYPE_SAMPLER_ADDRESSV, STATETEXTUREADDRESS_WRAP),
    &STATEINFO(TYPE_SAMPLER_ADDRESSW, STATETEXTUREADDRESS_WRAP));
}
