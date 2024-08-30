#include "backlogmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace BacklogMenu {

void Configure() {
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::BacklogMenuPtr = new UI::BacklogMenu();
  UI::Menus[drawType].push_back(UI::BacklogMenuPtr);
}

}  // namespace BacklogMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto