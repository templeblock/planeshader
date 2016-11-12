// Copyright �2016 Black Sphere Studios

#include "testbed.h"
#include "psImage.h"
#include "psPass.h"

using namespace planeshader;
using namespace bss_util;

struct PBRobj {
  Vector<float, 4> vecEye;
  Vector<float, 4> vecLight;
  Vector<float, 4> colorLight;
};

TESTDEF::RETPAIR test_PBR_System()
{
  BEGINTEST;
  int fps = 0;
  auto timer = cHighPrecisionTimer::OpenProfiler();
  psDriver* driver = engine->GetDriver();
  cStr media(psEngine::Instance()->GetMediaPath());

  // Load and configure PBR shader
  auto pbrfile = bssloadfile<char, true>(media + "/pbr-model.hlsl").first;
  PBRobj data = { {0}, { 0 }, { 1,1,1,1 } };

  psShader* pbrshader = psShader::MergeShaders(2, driver->library.IMAGE, psShader::CreateShader(0, 0, 1,
    &SHADER_INFO::From<void>(pbrfile.get(), "mainPS", PIXEL_SHADER_4_0, 0)));
  pbrshader->SetConstants<PBRobj, PIXEL_SHADER_4_0>(data);

  // Render a shadowmap for all shadow-casting lights
  // Pass in the material information and light data for each light to each object being rendered

  psImage img(media + "blocksrough-b/blocksrough_basecolor.png");
  img.SetShader(pbrshader);



  // Do postprocessing (e.g. tonemapping)



  engine->GetPass(0)->SetClearColor(0);
  engine->GetPass(0)->SetCamera(&globalcam);

  while(!gotonext && engine->Begin(0))
  {
    processGUI();
    engine->End();
    engine->FlushMessages();
    updatefpscount(timer, fps);
  }

  ENDTEST;
}