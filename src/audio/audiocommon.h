#pragma once

namespace Impacto {
namespace Audio {

enum AudioChannelGroup { ACG_BGM, ACG_SE, ACG_Voice, ACG_Movie, ACG_Count };
enum AudioChannelId {
  AC_SE0 = 0,
  AC_SE1,
  AC_SE2,
  AC_VOICE0,
  AC_VOICE1,
  AC_VOICE2,
  AC_REV,  // backlog
  AC_BGM0,
  AC_BGM1,
  AC_BGM2,
  AC_SSE,  // "system sound effect"
  AC_Count
};

enum AudioChannelState {
  ACS_Stopped,
  ACS_Playing,
  ACS_Paused,
  ACS_FadingIn,
  ACS_FadingOut
};

constexpr int VoiceCount = 32;

class AudioChannel;
class AudioStream;

}  // namespace Audio
}  // namespace Impacto