#include "titlemenu.h"
#include "../../../log.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace TitleMenu {

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  FenceSprite = EnsureGetMemberSprite("FenceSprite");
  CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
  OverlaySprite = EnsureGetMemberSprite("OverlaySprite");
  SmokeSprite = EnsureGetMemberSprite("SmokeSprite");
  ItemHighlightSprite = EnsureGetMemberSprite("ItemHighlightSprite");
  LoadSprite = EnsureGetMemberSprite("LoadSprite");
  LoadHighlightSprite = EnsureGetMemberSprite("LoadHighlightSprite");
  QuickLoadSprite = EnsureGetMemberSprite("QuickLoadSprite");
  QuickLoadHighlightSprite = EnsureGetMemberSprite("QuickLoadHighlightSprite");
  TipsSprite = EnsureGetMemberSprite("TipsSprite");
  TipsHighlightSprite = EnsureGetMemberSprite("TipsHighlightSprite");
  LibrarySprite = EnsureGetMemberSprite("LibrarySprite");
  LibraryHighlightSprite = EnsureGetMemberSprite("LibraryHighlightSprite");
  EndingListSprite = EnsureGetMemberSprite("EndingListSprite");
  EndingListHighlightSprite =
      EnsureGetMemberSprite("EndingListHighlightSprite");
  MenuSprite = EnsureGetMemberSprite("MenuSprite");

  PressToStartAnimFastDurationIn =
      EnsureGetMemberFloat("PressToStartAnimFastDurationIn");
  PressToStartAnimFastDurationOut =
      EnsureGetMemberFloat("PressToStartAnimFastDurationOut");
  BackgroundX = EnsureGetMemberFloat("BackgroundX");
  BackgroundY = EnsureGetMemberFloat("BackgroundY");
  BackgroundBoundsX = EnsureGetMemberFloat("BackgroundBoundsX");
  BackgroundBoundsYNormal = EnsureGetMemberFloat("BackgroundBoundsYNormal");
  BackgroundBoundsYTrue = EnsureGetMemberFloat("BackgroundBoundsYTrue");
  BackgroundBoundsWidth = EnsureGetMemberFloat("BackgroundBoundsWidth");
  BackgroundBoundsHeight = EnsureGetMemberFloat("BackgroundBoundsHeight");
  FenceX = EnsureGetMemberFloat("FenceX");
  FenceY = EnsureGetMemberFloat("FenceY");
  FenceBoundsWidth = EnsureGetMemberFloat("FenceBoundsWidth");
  FenceBoundsHeight = EnsureGetMemberFloat("FenceBoundsHeight");
  FenceBoundsX = EnsureGetMemberFloat("FenceBoundsX");
  FenceBoundsYNormal = EnsureGetMemberFloat("FenceBoundsYNormal");
  FenceBoundsYTrue = EnsureGetMemberFloat("FenceBoundsYTrue");
  CopyrightX = EnsureGetMemberFloat("CopyrightX");
  CopyrightY = EnsureGetMemberFloat("CopyrightY");
  CopyrightXMoveOffset = EnsureGetMemberFloat("CopyrightXMoveOffset");
  SmokeOpacityNormal = EnsureGetMemberFloat("SmokeOpacityNormal");
  SmokeX = EnsureGetMemberFloat("SmokeX");
  SmokeY = EnsureGetMemberFloat("SmokeY");
  SmokeBoundsX = EnsureGetMemberFloat("SmokeBoundsX");
  SmokeBoundsY = EnsureGetMemberFloat("SmokeBoundsY");
  SmokeBoundsWidth = EnsureGetMemberFloat("SmokeBoundsWidth");
  SmokeBoundsHeight = EnsureGetMemberFloat("SmokeBoundsHeight");
  SmokeAnimationBoundsXOffset =
      EnsureGetMemberFloat("SmokeAnimationBoundsXOffset");
  SmokeAnimationBoundsXMax = EnsureGetMemberFloat("SmokeAnimationBoundsXMax");
  SmokeAnimationDurationIn = EnsureGetMemberFloat("SmokeAnimationDurationIn");
  SmokeAnimationDurationOut = EnsureGetMemberFloat("SmokeAnimationDurationOut");
  MoveLeftAnimationDurationIn =
      EnsureGetMemberFloat("MoveLeftAnimationDurationIn");
  MoveLeftAnimationDurationOut =
      EnsureGetMemberFloat("MoveLeftAnimationDurationOut");
  ItemHighlightOffsetX = EnsureGetMemberFloat("ItemHighlightOffsetX");
  ItemHighlightOffsetY = EnsureGetMemberFloat("ItemHighlightOffsetY");
  ItemPadding = EnsureGetMemberFloat("ItemPadding");
  ItemYBase = EnsureGetMemberFloat("ItemYBase");
  SecondaryFirstItemHighlightOffsetX =
      EnsureGetMemberFloat("SecondaryFirstItemHighlightOffsetX");
  SecondarySecondItemHighlightOffsetX =
      EnsureGetMemberFloat("SecondarySecondItemHighlightOffsetX");
  SecondaryThirdItemHighlightOffsetX =
      EnsureGetMemberFloat("SecondaryThirdItemHighlightOffsetX");
  ItemHighlightPointerSprite =
      EnsureGetMemberSprite("ItemHighlightPointerSprite");
  ItemHighlightPointerY = EnsureGetMemberFloat("ItemHighlightPointerY");
  MenuX = EnsureGetMemberFloat("MenuX");
  MenuY = EnsureGetMemberFloat("MenuY");

  UI::CC::TitleMenu* menu = new UI::CC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;

  menu->SmokeAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  menu->SmokeAnimation.DurationOut = SmokeAnimationDurationOut;
  menu->MoveLeftAnimation.DurationIn = MoveLeftAnimationDurationIn;
  menu->MoveLeftAnimation.DurationOut = MoveLeftAnimationDurationOut;
  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto