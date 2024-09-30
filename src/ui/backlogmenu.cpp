#include "backlogmenu.h"

#include "ui.h"
#include "../profile/game.h"
#include "../renderer/renderer.h"
#include "../mem.h"
#include "../vm/interface/input.h"
#include "../profile/scriptvars.h"
#include "../profile/scriptinput.h"
#include "../profile/dialogue.h"
#include "../profile/ui/backlogmenu.h"
#include "../profile/ui/systemmenu.h"
#include "../profile/games/mo6tw/backlogmenu.h"
#include "../profile/games/cc/backlogmenu.h"
#include "../profile/games/cclcc/systemmenu.h"
#include "../inputsystem.h"
#include "../io/vfs.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ScriptInput;
using namespace Impacto::Profile::BacklogMenu;
using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  if (target->AudioId != -1) {
    Audio::Channels[Audio::AC_REV]->Play("voice", target->AudioId, false, 0.0f);
  }
}

BacklogMenu::BacklogMenu() {
  MainItems = new Widgets::Group(this, EntriesStart);
  MainItems->RenderingBounds = RenderingBounds;
  MainItems->WrapFocus = false;

  MainScrollbar = new Scrollbar(0, ScrollbarPosition, 0.0f, 1.0f, &PageY,
                                SBDIR_VERTICAL, ScrollbarTrack, ScrollbarThumb,
                                glm::vec2(0), ScrollbarThumbLength);
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;

  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void BacklogMenu::Show() {
  if (State == Hidden) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();

    if (!MainItems->Children.empty()) {
      auto el = MainItems->Children.back();
      FocusStart[FDIR_UP] = el;
      FocusStart[FDIR_DOWN] = el;
    }

    if (UI::FocusedMenu != nullptr) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;

    DirectionalButtonHeldTime = 0.0f;
    PageUpDownButtonHeldTime = 0.0f;

    // Set scrollbar back to default position
    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      PageY = MainScrollbar->EndValue;
      MainScrollbar->Update(0);
      MainItems->MoveTo(glm::vec2(EntriesStart.x, PageY));
    }
  }
}
void BacklogMenu::Hide() {
  if (State == Shown) {
    State = Hiding;
    FadeAnimation.StartOut();

    DirectionalButtonHeldTime = 0.0f;
    PageUpDownButtonHeldTime = 0.0f;

    if (LastFocusedMenu != nullptr) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = nullptr;
    }
  }
}

static inline bool IsBeyondShiftedHoverBounds(const Widget* el, float delta,
                                              bool up) {
  if (up) return el->Bounds.Y < HoverBounds.Y + delta;

  return el->Bounds.Y + el->Bounds.Height >
         HoverBounds.Y + HoverBounds.Height + delta;
}

void BacklogMenu::UpdatePageUpDownInput(float dt) {
  bool pageUpDown = (bool)(PADinputButtonIsDown & PAD1LEFT) ^
                    (bool)(PADinputButtonIsDown & PAD1RIGHT);
  if (!pageUpDown) {
    PageUpDownButtonHeldTime = 0.0f;
    PageUpDownWaitTime = 0.0f;
    return;
  }

  if (0.0f < PageUpDownButtonHeldTime &&
      PageUpDownButtonHeldTime < MinHoldTime) {
    PageUpDownButtonHeldTime += dt;
    PageUpDownWaitTime = 0.0f;
    return;
  }

  if (PageUpDownWaitTime > 0.0f) {
    PageUpDownWaitTime -= dt;
    return;
  }

  PageUpDownButtonHeldTime += dt;
  PageUpDownWaitTime = AdvanceFocusTimeInterval;

  FocusDirection dir = (PADinputButtonIsDown & PAD1LEFT) ? FDIR_UP : FDIR_DOWN;

  if (MainScrollbar->Enabled) {
    float delta = (dir == FDIR_UP) ? PageUpDownHeight : -PageUpDownHeight;
    PageY += delta;
    MainScrollbar->ClampValue();

    if (!CurrentlyFocusedElement) {
      CurrentlyFocusedElement = (dir == FDIR_UP) ? MainItems->Children.front()
                                                 : MainItems->Children.back();
    }

    CurrentlyFocusedElement->HasFocus = false;

    Widget* nextEl = CurrentlyFocusedElement->GetFocus(dir);
    while (nextEl &&
           IsBeyondShiftedHoverBounds(nextEl, delta, dir == FDIR_UP)) {
      CurrentlyFocusedElement = nextEl;
      nextEl = CurrentlyFocusedElement->GetFocus(dir);
    }

  } else {
    if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = false;

    CurrentlyFocusedElement = (dir == FDIR_UP) ? MainItems->Children.front()
                                               : MainItems->Children.back();
  }

  CurrentlyFocusedElement->HasFocus = true;
}

static inline bool inVerticalHoverBounds(const Widget* entry) {
  if (entry == nullptr) return false;

  return (HoverBounds.Y <= entry->Bounds.Y &&
          entry->Bounds.Y + entry->Bounds.Height <=
              HoverBounds.Y + HoverBounds.Height);
}

void BacklogMenu::UpdateScrollingInput(float dt) {
  bool padScrolling = (bool)(PADinputButtonIsDown & PAD1DOWN) ^
                      (bool)(PADinputButtonIsDown & PAD1UP);
  if (!padScrolling) {
    DirectionalButtonHeldTime = 0.0f;
    return;
  };

  FocusDirection dir = (PADinputButtonIsDown & PAD1DOWN) ? FDIR_DOWN : FDIR_UP;

  bool focusOnEdge = false;
  const Widget* nextEl = nullptr;
  if (CurrentlyFocusedElement != nullptr) {
    nextEl = CurrentlyFocusedElement->GetFocus(dir);
    focusOnEdge = !inVerticalHoverBounds(nextEl);
  }

  // Gradual scrolling
  if (MainScrollbar->Enabled && focusOnEdge) {
    PageY += (dir == FDIR_UP) ? ScrollingSpeed * dt : -ScrollingSpeed * dt;
    MainScrollbar->ClampValue();
    MainItems->Update(dt);
  }

  if (focusOnEdge) {
    if (nextEl) {
      float excess = (dir == FDIR_UP)
                         ? HoverBounds.Y - nextEl->Bounds.Y
                         : nextEl->Bounds.Y + nextEl->Bounds.Height -
                               HoverBounds.Y - HoverBounds.Height;
      if (excess < ScrollingSpeed * dt) AdvanceFocus(dir);
    }
  } else {
    if (DirectionalButtonHeldTime == 0.0f) {
      // Button just pressed
      AdvanceFocusWaitTime = 0.0f;

      AdvanceFocus(dir);
    } else if (DirectionalButtonHeldTime > MinHoldTime) {
      // Button held down
      if (AdvanceFocusWaitTime <= 0.0f) {
        AdvanceFocus(dir);
        AdvanceFocusWaitTime = AdvanceFocusTimeInterval;
      }

      AdvanceFocusWaitTime -= dt;
    }
  }

  DirectionalButtonHeldTime += dt;
}

void BacklogMenu::UpdateInput(float dt) {
  MainScrollbar->UpdateInput();
  UpdatePageUpDownInput(dt);
  UpdateScrollingInput(dt);
}

void BacklogMenu::Update(float dt) {
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] > 0 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 1) {
    Show();
  }

  if (State != Hidden && State != Shown) FadeAnimation.Update(dt);

  if (State == Showing && FadeAnimation.IsIn() &&
      ScrWork[SW_SYSSUBMENUCT] == 32) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && FadeAnimation.IsOut() &&
             ScrWork[SW_SYSSUBMENUCT] == 0) {
    State = Hidden;
    IsFocused = false;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;

    MainItems->Hide();
  }

  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 1) {
    UpdateInput(dt);

    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      MainScrollbar->Enabled = true;
      MainScrollbar->StartValue = MainItems->RenderingBounds.Y + EntryYPadding;
      MainScrollbar->EndValue = -ItemsHeight +
                                MainItems->RenderingBounds.Height +
                                MainItems->RenderingBounds.Y;
    }

    if (MainScrollbar->Enabled) {
      MainItems->MoveTo(glm::vec2(EntriesStart.x, PageY));
      auto lastEntry = MainItems->Children.back();
      CurrentEntryPos.y =
          lastEntry->Bounds.Y + lastEntry->Bounds.Height + EntryYPadding;
    }

    MainItems->Update(dt);
    MainScrollbar->Update(dt);

    // Handle entry moving out of hover bounds
    if (CurrentlyFocusedElement &&
        !inVerticalHoverBounds(CurrentlyFocusedElement)) {
      FocusDirection dir = (CurrentlyFocusedElement->Bounds.Y < HoverBounds.Y)
                               ? FDIR_DOWN
                               : FDIR_UP;
      Widget* newFocusedElement = CurrentlyFocusedElement->GetFocus(dir);
      while (newFocusedElement && !inVerticalHoverBounds(newFocusedElement))
        newFocusedElement = newFocusedElement->GetFocus(dir);

      CurrentlyFocusedElement->Hovered = false;
      CurrentlyFocusedElement->HasFocus = false;
      CurrentlyFocusedElement = newFocusedElement;
      if (newFocusedElement != nullptr) newFocusedElement->HasFocus = true;
    }
  }
}

void BacklogMenu::RenderHighlight() const {
  if (EntryHighlightLocation == +EntryHighlightLocationType::None ||
      CurrentlyFocusedElement == nullptr ||
      !MainItems->RenderingBounds.Intersects(CurrentlyFocusedElement->Bounds))
    return;

  RectF pos;
  const Widget& el = *CurrentlyFocusedElement;

  switch (EntryHighlightLocation) {
    default:
    case EntryHighlightLocationType::BottomLeftOfEntry:
      pos = RectF(
          el.Bounds.X,
          el.Bounds.Y + el.Bounds.Height - EntryHighlight.ScaledHeight(),
          Profile::Dialogue::REVBounds.Width, EntryHighlight.ScaledHeight());
      break;
    case EntryHighlightLocationType::TopLineLeftOfScreen:
      pos = RectF(0.0f, el.Bounds.Y, EntryHighlight.ScaledWidth(),
                  EntryHighlight.ScaledHeight());
      break;
  }

  pos.X += EntryHighlightOffset.x;
  pos.Y += EntryHighlightOffset.y;

  float opacity = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
  Renderer->DrawSprite(EntryHighlight, pos,
                       glm::vec4(1.0f, 1.0f, 1.0f, opacity));
}

void BacklogMenu::Render() {}

BacklogEntry* BacklogMenu::CreateBacklogEntry(int id, uint8_t* str, int audioId,
                                              glm::vec2 pos,
                                              const RectF& hoverBounds) const {
  return new BacklogEntry(id, str, audioId, pos, HoverBounds);
}

void BacklogMenu::AddMessage(uint8_t* str, int audioId) {
  if (!GetFlag(SF_REVADDDISABLE) || ScrWork[SW_MESWIN0TYPE] == 0) {
    auto onClick =
        std::bind(&BacklogMenu::MenuButtonOnClick, this, std::placeholders::_1);

    auto backlogEntry = CreateBacklogEntry(CurrentId, str, audioId,
                                           CurrentEntryPos, HoverBounds);
    CurrentId += 1;
    CurrentEntryPos.y += backlogEntry->TextHeight + EntryYPadding;
    backlogEntry->OnClickHandler = onClick;
    MainItems->Add(backlogEntry, FDIR_DOWN);
    ItemsHeight += backlogEntry->TextHeight + EntryYPadding;
    if (ItemsHeight > MainItems->RenderingBounds.Height) {
      MainScrollbar->EndValue = -ItemsHeight +
                                MainItems->RenderingBounds.Height +
                                MainItems->RenderingBounds.Y;
      PageY = MainScrollbar->EndValue;
      MainItems->MoveTo(glm::vec2(EntriesStart.x, PageY));
      CurrentEntryPos.y =
          backlogEntry->Bounds.Y + backlogEntry->Bounds.Height + EntryYPadding;
    }
  }
}

void BacklogMenu::Clear() {
  MainItems->Clear();
  PageY = 0.0f;
  CurrentId = 0;
  ItemsHeight = 0.0f;
  MainScrollbar->StartValue = 0.0f;
  MainScrollbar->EndValue = 0.0f;
  MainScrollbar->Enabled = false;
  CurrentEntryPos = EntriesStart;
}

}  // namespace UI
}  // namespace Impacto