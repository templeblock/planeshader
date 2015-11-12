// Copyright �2015 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in PlaneShader.h

#include "psEngine.h"
#include "bss-util\cDynArray.h"
#include "psDXGI.h"

using namespace planeshader;

const char* psDXGI::shader_profiles[] = { "vs_1_1", "vs_2_0", "vs_2_a", "vs_3_0", "vs_4_0", "vs_4_1", "vs_5_0", "ps_1_1", "ps_1_2", "ps_1_3", "ps_1_4", "ps_2_0",
"ps_2_a", "ps_2_b", "ps_3_0", "ps_4_0", "ps_4_1", "ps_5_0", "gs_4_0", "gs_4_1", "gs_5_0", "cs_4_0", "cs_4_1",
"cs_5_0", "ds_5_0", "hs_5_0" };

DXGI_SAMPLE_DESC psDXGI::DEFAULT_SAMPLE_DESC = { 1, 0 };

IDXGIAdapter* psDXGI::_createfactory(HWND hwnd, psVeciu& dpi, IDXGIOutput*& out)
{
  HRESULT _lasterr;
  if(FAILED(_lasterr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&_factory))))
    PSLOG(2) << "CreateDXGIFactory failed with error: " << GetDXGIError(_lasterr);

  if(FAILED(_lasterr = _factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)))
    PSLOG(2) << "MakeWindowAssociation failed with error: " << GetDXGIError(_lasterr);

  dpi = psGUIManager::GetMonitorDPI();
  //dpi = psVeciu(192); // DEBUG
  bss_util::cDynArray<IDXGIAdapter*, unsigned char> _adapters;
  IDXGIAdapter* adapter = NULL;
  for(unsigned char i = 0; _factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    _adapters.Add(adapter);

  if(!_adapters.Length())
  {
    PSLOGV(0, "No adapters to attach to!");
    return 0;
  }
  adapter = _adapters[0];

  bss_util::cDynArray<IDXGIOutput*, unsigned char> _outputs;
  IDXGIOutput* output = NULL;
  for(unsigned char i = 0; adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; ++i)
    _outputs.Add(output);

  if(!_outputs.Length())
  {
    PSLOGV(0, "No outputs to attach to!");
    return adapter;
  }
  out = _outputs[0];

  return adapter;
}

const char* BSS_FASTCALL psDXGI::GetDXGIError(HRESULT err)
{
  static char buf[64] = { 0 };

  switch(err)
  {
  case DXGI_ERROR_INVALID_CALL: return "DXGI_ERROR_INVALID_CALL";
  case DXGI_ERROR_NOT_FOUND: return "DXGI_ERROR_NOT_FOUND";
  case DXGI_ERROR_MORE_DATA: return "DXGI_ERROR_MORE_DATA";
  case DXGI_ERROR_UNSUPPORTED: return "DXGI_ERROR_UNSUPPORTED";
  case DXGI_ERROR_DEVICE_REMOVED: return "DXGI_ERROR_DEVICE_REMOVED";
  case DXGI_ERROR_DEVICE_HUNG: return "DXGI_ERROR_DEVICE_HUNG";
  case DXGI_ERROR_DEVICE_RESET: return "DXGI_ERROR_DEVICE_RESET";
  case DXGI_ERROR_WAS_STILL_DRAWING: return "DXGI_ERROR_WAS_STILL_DRAWING";
  case DXGI_ERROR_FRAME_STATISTICS_DISJOINT: return "DXGI_ERROR_FRAME_STATISTICS_DISJOINT";
  case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE: return "DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE";
  case DXGI_ERROR_DRIVER_INTERNAL_ERROR: return "DXGI_ERROR_DRIVER_INTERNAL_ERROR";
  case DXGI_ERROR_NONEXCLUSIVE: return "DXGI_ERROR_NONEXCLUSIVE";
  case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: return "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";
  case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED: return "DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED";
  case DXGI_ERROR_REMOTE_OUTOFMEMORY: return "DXGI_ERROR_REMOTE_OUTOFMEMORY";
  case E_FAIL: return "E_FAIL";
  case E_INVALIDARG: return "E_INVALIDARG";
  case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
  case E_NOTIMPL: return "E_NOTIMPL";
  case S_FALSE: return "S_FALSE";
  }

  _itoa_s(err, buf, 16);
  return buf;
}

unsigned int BSS_FASTCALL psDXGI::FMTtoDXGI(FORMATS format)
{
  switch(format)
  {
  case FMT_UNKNOWN: return DXGI_FORMAT_UNKNOWN;
  case FMT_R32G32B32A32X: return DXGI_FORMAT_R32G32B32A32_TYPELESS;
  case FMT_R32G32B32A32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
  case FMT_R32G32B32A32U: return DXGI_FORMAT_R32G32B32A32_UINT;
  case FMT_R32G32B32A32S: return DXGI_FORMAT_R32G32B32A32_SINT;
  case FMT_R32G32B32X: return DXGI_FORMAT_R32G32B32_TYPELESS;
  case FMT_R32G32B32F: return DXGI_FORMAT_R32G32B32_FLOAT;
  case FMT_R32G32B32U: return DXGI_FORMAT_R32G32B32_UINT;
  case FMT_R32G32B32S: return DXGI_FORMAT_R32G32B32_SINT;
  case FMT_R16G16B16A16X: return DXGI_FORMAT_R16G16B16A16_TYPELESS;
  case FMT_R16G16B16A16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
  case FMT_R16G16B16A16: return DXGI_FORMAT_R16G16B16A16_UNORM;
  case FMT_R16G16B16A16U: return DXGI_FORMAT_R16G16B16A16_UINT;
  case FMT_U16V16W16Q16: return DXGI_FORMAT_R16G16B16A16_SNORM;
  case FMT_R16G16B16A16S: return DXGI_FORMAT_R16G16B16A16_SINT;
  case FMT_R32G32X: return DXGI_FORMAT_R32G32_TYPELESS;
  case FMT_R32G32F: return DXGI_FORMAT_R32G32_FLOAT;
  case FMT_R32G32U: return DXGI_FORMAT_R32G32_UINT;
  case FMT_R32G32S: return DXGI_FORMAT_R32G32_SINT;
  case FMT_R32G8X24X: return DXGI_FORMAT_R32G8X24_TYPELESS;
  case FMT_D32S8X24: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
  case FMT_R32X8X24: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
  case FMT_X32G8X24: return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
  case FMT_R10G10B10A2X: return DXGI_FORMAT_R10G10B10A2_TYPELESS;
  case FMT_R10G10B10A2: return DXGI_FORMAT_R10G10B10A2_UNORM;
  case FMT_R10G10B10A2U: return DXGI_FORMAT_R10G10B10A2_UINT;
  case FMT_R11G11B10F: return DXGI_FORMAT_R11G11B10_FLOAT;
  case FMT_R8G8B8A8X: return DXGI_FORMAT_R8G8B8A8_TYPELESS;
  case FMT_R8G8B8A8: return DXGI_FORMAT_R8G8B8A8_UNORM;
  case FMT_R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
  case FMT_R8G8B8A8U: return DXGI_FORMAT_R8G8B8A8_UINT;
  case FMT_U8V8W8Q8: return DXGI_FORMAT_R8G8B8A8_SNORM;
  case FMT_R8G8B8A8S: return DXGI_FORMAT_R8G8B8A8_SINT;
  case FMT_R16G16X: return DXGI_FORMAT_R16G16_TYPELESS;
  case FMT_R16G16F: return DXGI_FORMAT_R16G16_FLOAT;
  case FMT_R16G16: return DXGI_FORMAT_R16G16_UNORM;
  case FMT_R16G16U: return DXGI_FORMAT_R16G16_UINT;
  case FMT_U16V16: return DXGI_FORMAT_R16G16_SNORM;
  case FMT_R16G16S: return DXGI_FORMAT_R16G16_SINT;
  case FMT_R32X: return DXGI_FORMAT_R32_TYPELESS;
  case FMT_D32F: return DXGI_FORMAT_D32_FLOAT;
  case FMT_R32F: return DXGI_FORMAT_R32_FLOAT;
  case FMT_INDEX32: return DXGI_FORMAT_R32_UINT;
  case FMT_R32S: return DXGI_FORMAT_R32_SINT;
  case FMT_R24G8X: return DXGI_FORMAT_R24G8_TYPELESS;
  case FMT_D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
  case FMT_R24X8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
  case FMT_X24G8: return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
  case FMT_R8G8_B8G8: return DXGI_FORMAT_R8G8_B8G8_UNORM;
  case FMT_G8R8_G8B8: return DXGI_FORMAT_G8R8_G8B8_UNORM;
  case FMT_B8G8R8A8: return DXGI_FORMAT_B8G8R8A8_UNORM;
  case FMT_B8G8R8A8_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
  case FMT_B8G8R8X8: return DXGI_FORMAT_B8G8R8X8_UNORM;
  case FMT_B8G8R8X8_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
  case FMT_B8G8R8A8X: return DXGI_FORMAT_B8G8R8A8_TYPELESS;
  case FMT_B8G8R8X8X: return DXGI_FORMAT_B8G8R8X8_TYPELESS;
  case FMT_R8G8X: return DXGI_FORMAT_R8G8_TYPELESS;
  case FMT_R8G8: return DXGI_FORMAT_R8G8_UNORM;
  case FMT_R8G8U: return DXGI_FORMAT_R8G8_UINT;
  case FMT_U8V8: return DXGI_FORMAT_R8G8_SNORM;
  case FMT_R8G8S: return DXGI_FORMAT_R8G8_SINT;
  case FMT_R16X: return DXGI_FORMAT_R16_TYPELESS;
  case FMT_R16F: return DXGI_FORMAT_R16_FLOAT;
  case FMT_D16: return DXGI_FORMAT_D16_UNORM;
  case FMT_R16: return DXGI_FORMAT_R16_UNORM;
  case FMT_INDEX16: return DXGI_FORMAT_R16_UINT;
  case FMT_U16: return DXGI_FORMAT_R16_SNORM;
  case FMT_R16S: return DXGI_FORMAT_R16_SINT;
  case FMT_B5G6R5: return DXGI_FORMAT_B5G6R5_UNORM;
  case FMT_B5G5R5A1: return DXGI_FORMAT_B5G5R5A1_UNORM;
  case FMT_R8X: return DXGI_FORMAT_R8_TYPELESS;
  case FMT_R8: return DXGI_FORMAT_R8_UNORM;
  case FMT_R8U: return DXGI_FORMAT_R8_UINT;
  case FMT_U8: return DXGI_FORMAT_R8_SNORM;
  case FMT_R8S: return DXGI_FORMAT_R8_SINT;
  case FMT_A8: return DXGI_FORMAT_A8_UNORM;
  case FMT_R1: return DXGI_FORMAT_R1_UNORM;
  case FMT_BC1X: return DXGI_FORMAT_BC1_TYPELESS;
  case FMT_BC1: return DXGI_FORMAT_BC1_UNORM;
  case FMT_BC1_SRGB: return DXGI_FORMAT_BC1_UNORM_SRGB;
  case FMT_BC2X: return DXGI_FORMAT_BC2_TYPELESS;
  case FMT_BC2: return DXGI_FORMAT_BC2_UNORM;
  case FMT_BC2_SRGB: return DXGI_FORMAT_BC2_UNORM_SRGB;
  case FMT_BC3X: return DXGI_FORMAT_BC3_TYPELESS;
  case FMT_BC3: return DXGI_FORMAT_BC3_UNORM;
  case FMT_BC3_SRGB: return DXGI_FORMAT_BC3_UNORM_SRGB;
  case FMT_BC4X: return DXGI_FORMAT_BC4_TYPELESS;
  case FMT_BC4: return DXGI_FORMAT_BC4_UNORM;
  case FMT_WC4: return DXGI_FORMAT_BC4_SNORM;
  case FMT_BC5X: return DXGI_FORMAT_BC5_TYPELESS;
  case FMT_BC5: return DXGI_FORMAT_BC5_UNORM;
  case FMT_WC5: return DXGI_FORMAT_BC5_SNORM;
  case FMT_BC6HX: return DXGI_FORMAT_BC6H_TYPELESS;
  case FMT_BC6H_UF16: return DXGI_FORMAT_BC6H_UF16;
  case FMT_BC6H_SF16: return DXGI_FORMAT_BC6H_SF16;
  case FMT_BC7X: return DXGI_FORMAT_BC7_TYPELESS;
  case FMT_BC7: return DXGI_FORMAT_BC7_UNORM;
  case FMT_BC7_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;
  }
  return -1;
}

FORMATS BSS_FASTCALL psDXGI::DXGItoFMT(unsigned int format)
{
  switch(format)
  {
  case DXGI_FORMAT_UNKNOWN: return FMT_UNKNOWN;
  case DXGI_FORMAT_R32G32B32A32_TYPELESS: return FMT_R32G32B32A32X;
  case DXGI_FORMAT_R32G32B32A32_FLOAT: return FMT_R32G32B32A32F;
  case DXGI_FORMAT_R32G32B32A32_UINT: return FMT_R32G32B32A32U;
  case DXGI_FORMAT_R32G32B32A32_SINT: return FMT_R32G32B32A32S;
  case DXGI_FORMAT_R32G32B32_TYPELESS: return FMT_R32G32B32X;
  case DXGI_FORMAT_R32G32B32_FLOAT: return FMT_R32G32B32F;
  case DXGI_FORMAT_R32G32B32_UINT: return FMT_R32G32B32U;
  case DXGI_FORMAT_R32G32B32_SINT: return FMT_R32G32B32S;
  case DXGI_FORMAT_R16G16B16A16_TYPELESS: return FMT_R16G16B16A16X;
  case DXGI_FORMAT_R16G16B16A16_FLOAT: return FMT_R16G16B16A16F;
  case DXGI_FORMAT_R16G16B16A16_UNORM: return FMT_R16G16B16A16;
  case DXGI_FORMAT_R16G16B16A16_UINT: return FMT_R16G16B16A16U;
  case DXGI_FORMAT_R16G16B16A16_SNORM: return FMT_U16V16W16Q16;
  case DXGI_FORMAT_R16G16B16A16_SINT: return FMT_R16G16B16A16S;
  case DXGI_FORMAT_R32G32_TYPELESS: return FMT_R32G32X;
  case DXGI_FORMAT_R32G32_FLOAT: return FMT_R32G32F;
  case DXGI_FORMAT_R32G32_UINT: return FMT_R32G32U;
  case DXGI_FORMAT_R32G32_SINT: return FMT_R32G32S;
  case DXGI_FORMAT_R32G8X24_TYPELESS: return FMT_R32G8X24X;
  case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return FMT_D32S8X24;
  case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS: return FMT_R32X8X24;
  case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT: return FMT_X32G8X24;
  case DXGI_FORMAT_R10G10B10A2_TYPELESS: return FMT_R10G10B10A2X;
  case DXGI_FORMAT_R10G10B10A2_UNORM: return FMT_R10G10B10A2;
  case DXGI_FORMAT_R10G10B10A2_UINT: return FMT_R10G10B10A2U;
  case DXGI_FORMAT_R11G11B10_FLOAT: return FMT_R11G11B10F;
  case DXGI_FORMAT_R8G8B8A8_TYPELESS: return FMT_R8G8B8A8X;
  case DXGI_FORMAT_R8G8B8A8_UNORM: return FMT_R8G8B8A8;
  case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return FMT_R8G8B8A8_SRGB;
  case DXGI_FORMAT_R8G8B8A8_UINT: return FMT_R8G8B8A8U;
  case DXGI_FORMAT_R8G8B8A8_SNORM: return FMT_U8V8W8Q8;
  case DXGI_FORMAT_R8G8B8A8_SINT: return FMT_R8G8B8A8S;
  case DXGI_FORMAT_R16G16_TYPELESS: return FMT_R16G16X;
  case DXGI_FORMAT_R16G16_FLOAT: return FMT_R16G16F;
  case DXGI_FORMAT_R16G16_UNORM: return FMT_R16G16;
  case DXGI_FORMAT_R16G16_UINT: return FMT_R16G16U;
  case DXGI_FORMAT_R16G16_SNORM: return FMT_U16V16;
  case DXGI_FORMAT_R16G16_SINT: return FMT_R16G16S;
  case DXGI_FORMAT_R32_TYPELESS: return FMT_R32X;
  case DXGI_FORMAT_D32_FLOAT: return FMT_D32F;
  case DXGI_FORMAT_R32_FLOAT: return FMT_R32F;
  case DXGI_FORMAT_R32_UINT: return FMT_INDEX32;
  case DXGI_FORMAT_R32_SINT: return FMT_R32S;
  case DXGI_FORMAT_R24G8_TYPELESS: return FMT_R24G8X;
  case DXGI_FORMAT_D24_UNORM_S8_UINT: return FMT_D24S8;
  case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return FMT_R24X8;
  case DXGI_FORMAT_X24_TYPELESS_G8_UINT: return FMT_X24G8;
  case DXGI_FORMAT_R8G8_B8G8_UNORM: return FMT_R8G8_B8G8;
  case DXGI_FORMAT_G8R8_G8B8_UNORM: return FMT_G8R8_G8B8;
  case DXGI_FORMAT_B8G8R8A8_UNORM: return FMT_B8G8R8A8;
  case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return FMT_B8G8R8A8_SRGB;
  case DXGI_FORMAT_B8G8R8X8_UNORM: return FMT_B8G8R8X8;
  case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return FMT_B8G8R8X8_SRGB;
  case DXGI_FORMAT_B8G8R8A8_TYPELESS: return FMT_B8G8R8A8X;
  case DXGI_FORMAT_B8G8R8X8_TYPELESS: return FMT_B8G8R8X8X;
  case DXGI_FORMAT_R8G8_TYPELESS: return FMT_R8G8X;
  case DXGI_FORMAT_R8G8_UNORM: return FMT_R8G8;
  case DXGI_FORMAT_R8G8_UINT: return FMT_R8G8U;
  case DXGI_FORMAT_R8G8_SNORM: return FMT_U8V8;
  case DXGI_FORMAT_R8G8_SINT: return FMT_R8G8S;
  case DXGI_FORMAT_R16_TYPELESS: return FMT_R16X;
  case DXGI_FORMAT_R16_FLOAT: return FMT_R16F;
  case DXGI_FORMAT_D16_UNORM: return FMT_D16;
  case DXGI_FORMAT_R16_UNORM: return FMT_R16;
  case DXGI_FORMAT_R16_UINT: return FMT_INDEX16;
  case DXGI_FORMAT_R16_SNORM: return FMT_U16;
  case DXGI_FORMAT_R16_SINT: return FMT_R16S;
  case DXGI_FORMAT_B5G6R5_UNORM: return FMT_B5G6R5;
  case DXGI_FORMAT_B5G5R5A1_UNORM: return FMT_B5G5R5A1;
  case DXGI_FORMAT_R8_TYPELESS: return FMT_R8X;
  case DXGI_FORMAT_R8_UNORM: return FMT_R8;
  case DXGI_FORMAT_R8_UINT: return FMT_R8U;
  case DXGI_FORMAT_R8_SNORM: return FMT_U8;
  case DXGI_FORMAT_R8_SINT: return FMT_R8S;
  case DXGI_FORMAT_A8_UNORM: return FMT_A8;
  case DXGI_FORMAT_R1_UNORM: return FMT_R1;
  case DXGI_FORMAT_BC1_TYPELESS: return FMT_BC1X;
  case DXGI_FORMAT_BC1_UNORM: return FMT_BC1;
  case DXGI_FORMAT_BC1_UNORM_SRGB: return FMT_BC1_SRGB;
  case DXGI_FORMAT_BC2_TYPELESS: return FMT_BC2X;
  case DXGI_FORMAT_BC2_UNORM: return FMT_BC2;
  case DXGI_FORMAT_BC2_UNORM_SRGB: return FMT_BC2_SRGB;
  case DXGI_FORMAT_BC3_TYPELESS: return FMT_BC3X;
  case DXGI_FORMAT_BC3_UNORM: return FMT_BC3;
  case DXGI_FORMAT_BC3_UNORM_SRGB: return FMT_BC3_SRGB;
  case DXGI_FORMAT_BC4_TYPELESS: return FMT_BC4X;
  case DXGI_FORMAT_BC4_UNORM: return FMT_BC4;
  case DXGI_FORMAT_BC4_SNORM: return FMT_WC4;
  case DXGI_FORMAT_BC5_TYPELESS: return FMT_BC5X;
  case DXGI_FORMAT_BC5_UNORM: return FMT_BC5;
  case DXGI_FORMAT_BC5_SNORM: return FMT_WC5;
  case DXGI_FORMAT_BC6H_TYPELESS: return FMT_BC6HX;
  case DXGI_FORMAT_BC6H_UF16: return FMT_BC6H_UF16;
  case DXGI_FORMAT_BC6H_SF16: return FMT_BC6H_SF16;
  case DXGI_FORMAT_BC7_TYPELESS: return FMT_BC7X;
  case DXGI_FORMAT_BC7_UNORM: return FMT_BC7;
  case DXGI_FORMAT_BC7_UNORM_SRGB: return FMT_BC7_SRGB;
  }
  return FMT_UNKNOWN;
}