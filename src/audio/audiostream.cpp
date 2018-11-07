#include "vorbisaudiostream.h"
#include "atrac9audiostream.h"
#include "adxaudiostream.h"
#include "hcaaudiostream.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

AudioStream* AudioStream::Create(SDL_RWops* stream) {
  if (AudioIsVorbis(stream)) return new VorbisAudioStream(stream);
  if (AudioIsHca(stream)) return new HcaAudioStream(stream);
  if (AudioIsAdx(stream)) return new AdxAudioStream(stream);
  if (AudioIsAtrac9(stream)) return new Atrac9AudioStream(stream);
  ImpLog(LL_Error, LC_Audio, "No audio decoder found\n");
  return 0;
}

}  // namespace Audio
}  // namespace Impacto