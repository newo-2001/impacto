#pragma once

#include "./optionsentry.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsSlider : public OptionsEntry {
 public:
  OptionsSlider(const Sprite& box, const Sprite& label, glm::vec2 pos,
                glm::vec4 highlightTint);

  void Render() override;
  void UpdateInput() override;

 private:
  const Sprite& BoxSprite;

  float Progress = 0.0f;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto