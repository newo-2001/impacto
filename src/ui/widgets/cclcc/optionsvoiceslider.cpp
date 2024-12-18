#include "optionsvoiceslider.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsVoiceSlider::OptionsVoiceSlider(
    const Sprite& box, const Sprite& label, const Sprite& portrait,
    const Sprite& mutedPortrait, glm::vec2 pos, glm::vec4 highlightTint,
    float sliderSpeed, std::function<void(OptionsEntry*)> select)
    : OptionsSlider(
          box, label, pos, highlightTint,
          RectF(pos.x + VoiceSliderOffset.x, pos.y + VoiceSliderOffset.y,
                box.ScaledWidth(), box.ScaledHeight()),
          sliderSpeed, select),
      Portrait(portrait),
      MutedPortrait(mutedPortrait) {
  Bounds =
      RectF(Bounds.X, Bounds.Y, VoiceEntryDimensions.x, VoiceEntryDimensions.y);
  EntryButton.Bounds = Bounds;
}

void OptionsVoiceSlider::Render() {
  HighlightTint.a = Tint.a;

  if (HasFocus) {
    RectF highlightBoundBox(Bounds.X, Bounds.Y, VoiceEntryDimensions.x,
                            VoiceEntryDimensions.y);
    Renderer->DrawRect(highlightBoundBox, HighlightTint);
    Renderer->DrawRect(highlightBoundBox + RectF(2.0f, 2.0f, -4.0f, -4.0f),
                       glm::vec4(1.0f, 1.0f, 1.0f, Tint.a));
  }

  Renderer->DrawSprite(Muted ? MutedPortrait : Portrait,
                       Bounds.GetPos() + PortraitOffset, Tint);
  Renderer->DrawSprite(LabelSprite, Bounds.GetPos() + NametagOffset,
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));

  RectF highlightBounds(
      Bounds.X + VoiceSliderOffset.x, Bounds.Y + VoiceSliderOffset.y,
      Progress * BoxSprite.ScaledWidth(), BoxSprite.ScaledHeight());
  Renderer->DrawRect(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, Bounds.GetPos() + VoiceSliderOffset, Tint);
}

void OptionsVoiceSlider::UpdateInput() {
  OptionsSlider::UpdateInput();

  if (HasFocus) Muted ^= (bool)(PADinputButtonWentDown & PAD1Y);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto