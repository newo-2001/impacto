#pragma once

#include "../../spriteanimation.h"
#include "../../hud/delusiontrigger.h"

namespace Impacto {
namespace CHLCC {
using namespace Impacto::DelusionTrigger;

class DelusionTrigger : public DelusionTriggerBase {
 public:
  DelusionTrigger();

  void Show();
  void Hide();
  void Update(float dt);
  void Render();
  void Reset();

 protected:
  Animation SpinAnimation;
  Animation TextAnimation;
  Animation ZoomAnimation;

  SpriteAnimation TextFade;
  float maskScaleFactor;
  float spinAngle;
  float spinRate;
  int spinDirection;
  float underLayerAlpha;
};

}  // namespace CHLCC
}  // namespace Impacto