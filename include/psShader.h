// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#ifndef __SHADER_H__PS__
#define __SHADER_H__PS__

#include "psDriver.h"
#include "bss-util/cRefCounter.h"
#include <stdarg.h>

namespace planeshader {
  // Represents a single shader program of a certain type (pixel/vertex/etc.)
  struct SHADER_INFO : psDriverHold
  {
    // Directly creates a shader info object, which is not usually suggested but can be done if you have no constant data you care about.
    SHADER_INFO(void* Shader, SHADER_VER V, unsigned short sz=0, const void* Init=0) : shader(Shader), v(V), init(Init), ty_sz(sz) {}
    SHADER_INFO(const char* Shader, const char* entrypoint, SHADER_VER V, unsigned short sz=0, const void* Init=0) : shader(_driver->CompileShader(Shader, V, entrypoint)), v(V), init(Init), ty_sz(sz) {}
    void* shader;
    SHADER_VER v;
    unsigned short ty_sz;
    const void* init;

    // Creates a shader from either precompiled source, or a string with a given entrypoint, with optional initial data of type T stored as a constant buffer.
    template<class T> // We can't have templates on constructors so we have to do this instead
    BSS_FORCEINLINE static SHADER_INFO From(void* Shader, SHADER_VER V, const T* Init=0) { return SHADER_INFO(Shader, V, std::is_void<T>::value?0:sizeof(std::conditional<std::is_void<T>::value,char,T>::type), Init); }
    template<class T>
    BSS_FORCEINLINE static SHADER_INFO From(const char* Shader, const char* entrypoint, SHADER_VER V, const T* Init=0) { return SHADER_INFO(Shader, entrypoint, V, std::is_void<T>::value?0:sizeof(std::conditional<std::is_void<T>::value, char, T>::type), Init); }
  };

  // Encapsulates an entire shader effect: each type of shader program (vertex/pixel/etc.), the constants for each individual shader program, and the shader layout.
  class PS_DLLEXPORT psShader : protected psDriverHold, public bss_util::cRefCounter
  {
  public:
    void Activate();
    template<typename T, int I>
    inline bool BSS_FASTCALL SetConstants(const T& src)
    { 
      static_assert(I>=0 && I < 6, "I must be less than 6");
      return SetConstants(&src, sizeof(T), I);
    }
    bool BSS_FASTCALL SetConstants(const void* data, size_t sz, unsigned char I);

    // Creates a new shader object out of a given layout and a list of SHADER_INFOs, which represent all included shader programs and their associated constant buffers.
    template<unsigned char I>
    inline static psShader* BSS_FASTCALL CreateShader(const ELEMENT_DESC(&layout)[I], unsigned char num, ...)
    {
      DYNARRAY(SHADER_INFO, infos, num);
      va_list vl;
      va_start(vl, num);
      for(unsigned int i = 0; i < num; ++i) infos[i] = *va_arg(vl, const SHADER_INFO*);
      va_end(vl);
      return CreateShader(I, layout, num, infos);
    }
    static psShader* BSS_FASTCALL CreateShader(unsigned char nlayout, const ELEMENT_DESC* layout, unsigned char num, ...); // All arguments here must be passed in as const SHADER_INFO*
    template<unsigned char I>
    inline static psShader* BSS_FASTCALL CreateShader(const ELEMENT_DESC(&layout)[I], unsigned char num, const SHADER_INFO* infos) { return CreateShader(I, layout, num, infos); }
    static psShader* BSS_FASTCALL CreateShader(unsigned char nlayout, const ELEMENT_DESC* layout, unsigned char num, const SHADER_INFO* infos);
    // Copies a single shader
    static psShader* BSS_FASTCALL CreateShader(psShader* copy); 
    // merges num shaders into a new shader in left to right order (so the first will be overwritten by the rest). num cannot be 0.
    static psShader* BSS_FASTCALL MergeShaders(unsigned int num, const psShader* first, ...); 

    psShader& operator+=(const psShader& right);

  protected:
    psShader(const psShader& copy);
    psShader(psShader&& mov);
    explicit psShader(void* layout, void* ss[6], void* sc[6], size_t sz[6]);
    ~psShader();
    void _destroy();
    void _copy(const psShader& copy);
    void _move(psShader&& mov);
    virtual void DestroyThis();
    psShader& operator=(const psShader& copy);
    psShader& operator=(psShader&& mov);

    void* _ss[6]; // Vertex, Pixel, Geometry, Compute, Domain, Hull
    void* _sc[6]; // Shader constants
    size_t _sz[6]; // Size of the shader constants (so we can copy the shader)
    void* _layout;
  };

  // This is just an imaginary wrapper around psShader that lets you do typesafe constant setting. It is completely optional.
  template<typename VS, typename PS=void, typename GS=void, typename CS=void, typename DS=void, typename HS=void>
  class PS_DLLEXPORT psShaderT : protected psShader
  {
  public:
    inline psShaderT(void* layout) : psShader<VS, PS, GS, CS, DS, HS>(layout)
    template<typename T> // Automatically picks the correct shader to apply the constants to by match T to that shader's constant declaration.
    inline bool BSS_FASTCALL SetConstants(const T& src)
    {
      static_assert((std::is_same<T, VS>::value+std::is_same<T, PS>::value+std::is_same<T, HS>::value+std::is_same<T, CS>::value+std::is_same<T, DS>::value+std::is_same<T, HS>::value) == 1);
      return SetConstants<T, (std::is_same<T, PS>::value*1)|(std::is_same<T, PS>::value*2)|(std::is_same<T, PS>::value*3)|(std::is_same<T, PS>::value*4)|(std::is_same<T, PS>::value*5)>(src);
    }

    template<unsigned char I>
    inline static psShaderT* BSS_FASTCALL CreateShader(const ELEMENT_DESC(&layout)[I], unsigned char num, ...) // All arguments here must be passed in as const SHADER_INFO*
    {
      DYNARRAY(SHADER_INFO, infos, num);
      va_list vl;
      va_start(vl, num);
      for(unsigned int i = 0; i < num; ++i) infos[i] = *va_arg(vl, const SHADER_INFO*);
      va_end(vl);
      return static_cast<psShaderT*>(psShader::CreateShader(I, layout, num, infos));
    }
    inline static psShaderT* BSS_FASTCALL CreateShader(unsigned char nlayout, const ELEMENT_DESC* layout, unsigned char num, ...) // All arguments here must be passed in as const SHADER_INFO*
    {
      DYNARRAY(SHADER_INFO, infos, num);
      va_list vl;
      va_start(vl, num);
      for(unsigned int i = 0; i < num; ++i) infos[i] = *va_arg(vl, const SHADER_INFO*);
      va_end(vl);
      return static_cast<psShaderT*>(psShader::CreateShader(nlayout, layout, num, infos));
    } 
    template<unsigned char I>
    inline static psShaderT* BSS_FASTCALL CreateShader(const ELEMENT_DESC(&layout)[I], unsigned char num, const SHADER_INFO* infos) { return static_cast<psShaderT*>(psShader::CreateShader(I, layout, num, infos)); }
    inline static psShaderT* BSS_FASTCALL CreateShader(unsigned char nlayout, const ELEMENT_DESC* layout, unsigned char num, const SHADER_INFO* infos) { return static_cast<psShaderT*>(psShader::CreateShader(layout, num, infos)); }
    inline static psShaderT* BSS_FASTCALL CreateShader(const psShaderT* copy) { return static_cast<psShaderT*>(psShader::CreateShader(copy)); }
  };
}

#endif