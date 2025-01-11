#include "tipstabgroup.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/ui/tipsmenu.h"
#include "../../../profile/games/cclcc/tipsmenu.h"
#include "../../../inputsystem.h"
#include "../../../vm/interface/input.h"
#include "../../../ui/ui.h"
#include "../../../text.h"
#include "../../../vm/vm.h"
#include "../../../profile/game.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

using namespace Impacto::TipsSystem;
using namespace Impacto::Profile::CCLCC::TipsMenu;
using namespace Impacto::UI::CCLCC;

TipsTabButton::TipsTabButton(
    TipsTabType type, std::function<void(Widgets::Button*)> onClickHandler)

    : Button(
          type, TipsHighlightedTabSprite, Sprite(), Sprite(),
          TipsTabNameDisplay + glm::vec2(type * TipsHighlightedTabAdder, 0)) {
  OnClickHandler = std::move(onClickHandler);
  NormalSprite.Bounds.X += type * TipsHighlightedTabAdder;
}

void TipsTabButton::Reset() {
  Bounds.X = TipsTabNameDisplay.x + Id * TipsHighlightedTabAdder;
  Bounds.Y = TipsTabNameDisplay.y;
}

void TipsTabButton::UpdateInput() {
  if (Enabled) {
    if (Input::CurrentInputDevice == Input::Device::Mouse &&
        Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    } else if (Input::CurrentInputDevice == Input::Device::Touch &&
               Input::TouchIsDown[0] &&
               Input::PrevTouchPos != Input::CurTouchPos) {
      Hovered = Bounds.ContainsPoint(Input::CurTouchPos);
    }
    if (OnClickHandler && HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

TipsTabGroup::TipsTabGroup(
    TipsTabType type, std::function<void(Widgets::Button*)> tabClickHandler,
    std::function<void(Widgets::Button*)> tipClickHandler)
    : Type(type),
      TabName(type, tabClickHandler),
      TipsEntriesGroup(this),
      TipClickHandler(tipClickHandler) {
  TipsEntriesGroup.WrapFocus = true;

  TipsScrollStartPos = {TipsScrollEntriesX, TipsScrollYStart};

  TipsScrollTrackBounds = {TipsScrollThumbSprite.Bounds.Width,
                           TipsScrollYEnd - TipsScrollYStart};
  EntriesPerPage = std::ceil(TipsTabBounds.Height / TipsEntryBounds.Height);
}

// Todo: Next page with left right keys
void TipsTabGroup::UpdatePageInput(float dt) {
  using namespace Vm::Interface;
  if (IsFocused) {
    auto prevEntry = CurrentlyFocusedElement;
    TipsEntriesScrollbar->UpdateInput();

    FocusDirection dir =
        (PADinputButtonIsDown & PAD1LEFT) ? FDIR_UP : FDIR_DOWN;

    bool holdScroll = UpdatePageUpDownTimes(dt);

    auto checkScrollBounds = [&]() {
      return !TipsTabBounds.Contains(CurrentlyFocusedElement->Bounds);
    };

    if (PADinputButtonWentDown & PAD1DOWN ||
        holdScroll && PADinputButtonIsDown & PAD1DOWN) {
      AdvanceFocus(FDIR_DOWN);
      if (CurrentlyFocusedElement != prevEntry && checkScrollBounds()) {
        if (CurrentlyFocusedElement == TipsEntriesGroup.Children.front()) {
          ScrollPosY = 0;
        } else {
          ScrollPosY += TipsEntryBounds.Height;
        }
      }
    } else if (PADinputButtonWentDown & PAD1UP ||
               holdScroll && PADinputButtonIsDown & PAD1UP) {
      AdvanceFocus(FDIR_UP);
      if (CurrentlyFocusedElement != prevEntry && checkScrollBounds()) {
        if (CurrentlyFocusedElement == TipsEntriesGroup.Children.back()) {
          ScrollPosY = TipsEntriesScrollbar->EndValue;
        } else {
          ScrollPosY -= TipsEntryBounds.Height;
        }
      }
    } else if (PADinputButtonWentDown & PAD1RIGHT) {
      AdvanceFocus(FDIR_RIGHT);
      if (CurrentlyFocusedElement != prevEntry) {
        if (checkScrollBounds())
          ScrollPosY += TipsEntryBounds.Height * EntriesPerPage;
      } else {
        CurrentlyFocusedElement->HasFocus = false;
        CurrentlyFocusedElement = TipsEntriesGroup.Children.front();
        CurrentlyFocusedElement->HasFocus = true;
        if (checkScrollBounds()) {
          ScrollPosY = 0;
        }
      }
    } else if (PADinputButtonWentDown & PAD1LEFT) {
      AdvanceFocus(FDIR_LEFT);
      if (CurrentlyFocusedElement != prevEntry) {
        if (checkScrollBounds())
          ScrollPosY -= TipsEntryBounds.Height * EntriesPerPage;
      } else {
        CurrentlyFocusedElement->HasFocus = false;
        CurrentlyFocusedElement = TipsEntriesGroup.Children.back();
        CurrentlyFocusedElement->HasFocus = true;
        if (checkScrollBounds()) {
          ScrollPosY = TipsEntriesScrollbar->EndValue;
        }
      }
    }
  }
}

void TipsTabGroup::Update(float dt) {
  TabName.Enabled = true;
  TabName.Update(dt);
  TabName.UpdateInput();
  if (State == Hidden) {
    TipsEntriesGroup.Enabled = false;
    // Inverting since we want buttons to be clickable when the tab is not
    // shown
    TabName.HasFocus = true;
  } else {
    TipsEntriesGroup.Enabled = true;
    TabName.HasFocus = false;
    TipsEntriesGroup.Update(dt);
  }
  float oldScrollPosY = ScrollPosY;
  UpdatePageInput(dt);
  if (TipsEntriesScrollbar) {
    TipsEntriesScrollbar->Update(dt);
    TipsEntriesScrollbar->UpdateInput();
    if (oldScrollPosY != ScrollPosY) {
      TipsEntriesGroup.Move({0, oldScrollPosY - ScrollPosY});
    }
  }
}

bool TipsTabGroup::UpdatePageUpDownTimes(float dt) {
  bool pageUpDown =
      (bool)(Vm::Interface::PADinputButtonIsDown & Vm::Interface::PAD1UP) ^
      (bool)(Vm::Interface::PADinputButtonIsDown & Vm::Interface::PAD1DOWN);
  if (!pageUpDown) {
    PageUpDownButtonHeldTime = 0.0f;
    PageUpDownWaitTime = 0.0f;
    return false;
  }

  if (0.0f < PageUpDownButtonHeldTime &&
      PageUpDownButtonHeldTime < MinHoldTime) {
    PageUpDownButtonHeldTime += dt;
    PageUpDownWaitTime = 0.0f;
    return false;
  }

  if (PageUpDownWaitTime > 0.0f) {
    PageUpDownWaitTime -= dt;
    return false;
  }

  PageUpDownButtonHeldTime += dt;
  PageUpDownWaitTime = AdvanceFocusTimeInterval;
  return true;
}

void TipsTabGroup::Render() {
  if (State != Hidden) {
    TabName.Tint = Tint;
    TabName.Render();
    TipsEntriesGroup.Tint = Tint;
    TipsEntriesGroup.Render();
    TipsEntriesScrollbar->Tint = Tint;
    TipsEntriesScrollbar->Render();
  }
}

void TipsTabGroup::UpdateTipsEntries(std::vector<int> const& SortedTipIds) {
  auto tipsFilterPredicate = [&](TipsSystem::TipsDataRecord const& record) {
    switch (Type) {
      case TipsTabType::AllTips:
        return true;
      case TipsTabType::UnlockedTips:
        return !record.IsLocked;
      case TipsTabType::UnreadTips:
        return record.IsUnread && !record.IsLocked;
      case TipsTabType::NewTips:
        return record.IsNew && !record.IsLocked;
    }
  };

  int sortIndex = 1;
  TipsEntriesGroup.Clear();
  TipsEntryButtons.clear();
  int shownEntryCount = 0;
  for (auto& tipId : SortedTipIds) {
    auto& record = *TipsSystem::GetTipRecord(tipId);
    if (!tipsFilterPredicate(record)) {
      sortIndex++;
      continue;
    }
    bool dispNew = record.IsNew && !record.IsLocked;
    RectF buttonBounds = TipsEntryBounds;
    buttonBounds.Y += TipsEntryButtons.size() * buttonBounds.Height;
    TipsEntryButton* button = new TipsEntryButton(
        tipId, sortIndex++, buttonBounds, TipsHighlightedSprite, dispNew);
    button->OnClickHandler = TipClickHandler;
    TipsEntriesGroup.Add(button, FDIR_DOWN);
    TipsEntryButtons.push_back(button);
    if (Type == TipsTabType::NewTips) {
      TipsSystem::SetTipNewState(tipId, false);
    }
    shownEntryCount++;
  }

  for (int i = 0;
       i < static_cast<int>(TipsEntriesGroup.Children.size()) - EntriesPerPage;
       i++) {
    TipsEntriesGroup.Children[i]->SetFocus(
        TipsEntriesGroup.Children[i + EntriesPerPage], FDIR_RIGHT);
    TipsEntriesGroup.Children[i + EntriesPerPage]->SetFocus(
        TipsEntriesGroup.Children[i], FDIR_LEFT);
  }

  auto roundUpMultiple = [](float numToRound, float multiple) {
    return std::ceil(numToRound / multiple) * multiple;
  };
  int scrollDistance =
      TipsEntryBounds.Height * TipsEntryButtons.size() -
      roundUpMultiple(TipsTabBounds.Height, TipsEntryBounds.Height);

  TipsEntriesScrollbar = std::make_unique<Scrollbar>(
      0, TipsScrollStartPos, 0, std::max(0, scrollDistance), &ScrollPosY,
      SBDIR_VERTICAL, TipsScrollThumbSprite, TipsScrollTrackBounds,
      TipsScrollThumbLength, TipsTabBounds);
  TipsEntriesGroup.RenderingBounds = TipsTabBounds;
  TabName.Reset();
}

void TipsTabGroup::Show() {
  if (State != Shown) {
    State = Shown;
    IsFocused = true;
    TipsEntriesGroup.Show();
    CurrentlyFocusedElement = TipsEntriesGroup.GetFocus(FDIR_DOWN);

    if (CurrentlyFocusedElement) {
      static_cast<TipsEntryButton*>(CurrentlyFocusedElement)->PrevFocusState =
          true;
      CurrentlyFocusedElement->HasFocus = true;
    }
  }
}
void TipsTabGroup::Hide() {
  if (State != Hidden) {
    State = Hidden;
    IsFocused = false;
    ScrollPosY = 0.0f;
    TipsEntriesGroup.Hide();
  }
}

void TipsTabGroup::Move(glm::vec2 relativePosition) {
  TabName.Move(relativePosition);
  TipsEntriesGroup.Move(relativePosition);
  TipsEntriesScrollbar->Move(relativePosition);
  TipsEntriesGroup.RenderingBounds.X += relativePosition.x;
  TipsEntriesGroup.RenderingBounds.Y += relativePosition.y;
}

void TipsTabGroup::MoveTo(glm::vec2 pos) {
  TabName.MoveTo(pos);
  TipsEntriesGroup.MoveTo(pos);
  TipsEntriesScrollbar->MoveTo(pos);
  TipsEntriesGroup.RenderingBounds.X = pos.x;
  TipsEntriesGroup.RenderingBounds.Y = pos.y;
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto