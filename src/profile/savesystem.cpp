#include "savesystem.h"
#include "profile_internal.h"

#include "../games/chlcc/savesystem.h"

namespace Impacto {
namespace Profile {
namespace SaveSystem {

using namespace Impacto::SaveSystem;

SaveDataType Type = SaveDataType::None;

std::string SaveFilePath;

void Configure() {
  EnsurePushMemberOfType("SaveData", kObjectType);

  Type = SaveDataType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  switch (Type) {
    case SaveDataType::CHLCC:
      Implementation = new Impacto::CHLCC::SaveSystem();
      break;
  }

  SaveFilePath = EnsureGetMemberString("SaveFilePath");

  Pop();
}

}  // namespace SaveSystem
}  // namespace Profile
}  // namespace Impacto