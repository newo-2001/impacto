#include "titlemenu.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/dash/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace Dash {
namespace TitleMenu {

Sprite BackgroundSprite;
Animation PressToStartAnimation;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  UI::TitleMenuPtr = new UI::Dash::TitleMenu();
}

}  // namespace TitleMenu
}  // namespace Dash
}  // namespace Profile
}  // namespace Impacto