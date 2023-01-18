#pragma once

#include "../impacto.h"

#include <queue>

struct AVFrame;
struct AVPacket;
struct AVCodecContext;
struct AVStream;

namespace Impacto {
namespace Video {

struct AVPacketItem {
  AVPacket* Packet;
  int Serial;
};

struct AVFrameItem {
  AVFrame* Frame;
  int Serial;
  double Timestamp;
  double Duration;
};

class FFmpegStream {
 public:
  AVFrame* Frame;
  AVPacket* Packet;
  AVCodecContext* CodecContext;
  AVStream* stream;
  std::queue<AVPacketItem> PacketQueue;
  std::queue<AVFrameItem> FrameQueue;
  int Duration;
  int PacketQueueSerial = 0;
  int CurrentPacketSerial = 0;

  SDL_Thread* DecoderThreadID;

  SDL_mutex* PacketLock;
  SDL_mutex* FrameLock;
  SDL_cond* DecodeCond;

  FFmpegStream(AVStream* stream, AVCodecContext* codecCtx);
  ~FFmpegStream();

  void FlushPacketQueue();
  void FlushFrameQueue();
};

}  // namespace Video
}  // namespace Impacto
