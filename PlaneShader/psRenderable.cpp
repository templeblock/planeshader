// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#include "psRenderable.h"
#include "psEngine.h"
#include "psPass.h"
#include "psStateblock.h"
#include "psShader.h"
#include "bss-util/profiler.h"

using namespace planeshader;

psRenderable::psRenderable(const psRenderable& copy) : _flags(copy._flags), _pass((unsigned short)-1), _internalflags(copy._internalflags), _zorder(copy._zorder),
  _stateblock(copy._stateblock), _shader(psShader::CreateShader(copy._shader))
{
  _llist.next=_llist.prev=0;
  if(_stateblock) 
    _stateblock->Grab();
  SetPass(copy._pass);
}
psRenderable::psRenderable(psRenderable&& mov) : _flags(mov._flags), _pass((unsigned short)-1), _internalflags(mov._internalflags), _zorder(mov._zorder),
  _stateblock(mov._stateblock), _shader(mov._shader)
{ 
  _llist.next=_llist.prev=0;
  unsigned short p = mov._pass;
  mov.SetPass(-1);
  SetPass(p);
  mov._shader=0;
  mov._stateblock=0;
}

psRenderable::psRenderable(const DEF_RENDERABLE& def) : _flags(def.flags), _zorder(def.zorder), _pass((unsigned short)-1), _internalflags(0),
  _stateblock(def.stateblock), _shader(psShader::CreateShader(def.shader))
{ 
  _llist.next=_llist.prev=0;
  if(_stateblock)
    _stateblock->Grab();
  SetPass(def.pass); 
}

psRenderable::psRenderable(FLAG_TYPE flags, int zorder, psStateblock* stateblock, psShader* shader, unsigned short pass) : _flags(flags), _zorder(zorder),
  _stateblock(stateblock), _shader(psShader::CreateShader(shader)), _pass((unsigned short)-1)
{ 
  _llist.next=_llist.prev=0;
  if(_stateblock)
    _stateblock->Grab();
  SetPass(pass);
}

psRenderable::~psRenderable() { _destroy(); }
void psRenderable::Render()
{
  assert(psPass::_curpass!=0); // This is only called manually, when you want to render from outside a pass, usually because you don't belong to a pass.
  psPass::_curpass->_queue(this); // Get the current pass, and add this to it's render queue.
}

void BSS_FASTCALL psRenderable::SetPass(unsigned short pass)
{
  PROFILE_FUNC();
  if(pass==_pass) return;
  //if(_pinfo) _pinfo->p=0;
  //_pinfo=0;
  if(_pass!=((unsigned short)-1))
    psEngine::Instance()->GetPass(_pass)->Remove(this);
  _pass=pass;
  if(pass==(unsigned short)-1) return;

  if(pass>=psEngine::Instance()->NumPass())
  {
    PSLOG(2) << pass << "is an invalid pass index" << std::endl;
    pass=(unsigned short)-1;
  }
  else
    psEngine::Instance()->GetPass(_pass)->Insert(this);
}

void BSS_FASTCALL psRenderable::SetStateblock(psStateblock* stateblock)
{
  PROFILE_FUNC();
  if(_stateblock)
    _stateblock->Drop();
  _stateblock = stateblock;
  if(_stateblock)
    _stateblock->Grab();
  _invalidate();
}

psTex* const* psRenderable::GetTextures() const { return 0; } // these aren't inline because they're virtual
unsigned char psRenderable::NumTextures() const { return 0; }
psTex* const* psRenderable::GetRenderTargets() const { return 0; }
unsigned char psRenderable::NumRT() const { return 0; }
char psRenderable::_sort(psRenderable* r) const { return SGNCOMPARE(this, r); }
bool psRenderable::_batch(psRenderable* r) const { return false; }
void psRenderable::_renderbatch(psRenderable** rlist) { for(; *rlist != 0; ++rlist) (*rlist)->_render(); }

void psRenderable::_destroy()
{
  SetPass((unsigned short)-1);
  if(_stateblock) _stateblock->Drop();
  if(_shader) _shader->Drop();
}

void psRenderable::_invalidate()
{
  if(_pass!=((unsigned short)-1))
  {
    if(_psort!=0 && ((_psort->prev!=0 && psPass::StandardCompare(_psort->prev->value, this)>0) || (_psort->next && psPass::StandardCompare(this, _psort->next->value)<0)))
    { // We only invalidate if the new parameters actually invalidate the object's position.
      psEngine::Instance()->GetPass(_pass)->_renderlist.Remove(_psort);
      _psort = 0;
    }
  }
}

psRenderable& psRenderable::operator =(const psRenderable& right)
{
  PROFILE_FUNC();
  _destroy();
  _flags=right._flags;
  _internalflags=right._internalflags;
  _zorder=right._zorder;
  _stateblock=right._stateblock;
  _shader=psShader::CreateShader(right._shader);

  _llist.next=_llist.prev=0;
  if(_stateblock)
    _stateblock->Grab();
  SetPass(right._pass);
  return *this;
}

psRenderable& psRenderable::operator =(psRenderable&& right)
{
  PROFILE_FUNC();
  _destroy();
  _flags=right._flags;
  _internalflags=right._internalflags;
  _zorder=right._zorder;
  _stateblock=right._stateblock;
  _shader=right._shader;

  _llist.next=_llist.prev=0;
  unsigned short p = right._pass;
  right.SetPass(-1);
  SetPass(p);
  right._shader=0;
  right._stateblock=0;
  return *this;
}
