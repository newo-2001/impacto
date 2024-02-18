#pragma once
#include "../../ui/mapsystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
class MapSystemCCLCC : public Impacto::UI::MapSystem::MapSystemBase {
 public:
  enum class MapBGState { Hidden, Showing, Shown, Hiding };
  void MapInit() override;
  void MapSetFadein(int arg1, int arg2) override;
  void MapSetGroup(int arg1, int arg2, int arg3, int arg4) override;
  void MapSetFadeout(int arg1, int arg2) override;
  void MapSetDisp(int arg1, int arg2) override;
  void MapSetHide(int arg1, int arg2);
  bool MapFadeEndChk_Wait();
  void MapMoveAnimeInit(int arg1, int arg2, int arg3);
  void MapMoveAnimeMain();
  void MapGetPos(int arg1, int arg2, int &arg3, int &arg4) override;
  void MapSetPool(int arg1, int arg2, int arg3) override;
  void MapResetPoolAll(int arg1);
  bool MapPoolFadeEndChk_Wait();
  void MapPoolShuffle(int arg1);
  void MapPoolSetDisp(int arg1, int arg2);
  void MapPoolSetHide(int arg1, int arg2);
  void MapPoolSetFadein(int arg1, int arg2);
  void MapPoolSetFadeout(int arg1, int arg2);
  bool MapPlayerPhotoSelect(int arg1) override;
  void MapResetPool(int arg1) override;
  void MapSetGroupEx(int arg1, int arg2, int arg3) override;
  void MapZoomInit(int arg1, int arg2, int arg3) override;
  bool MapZoomMain() override;
  void MapZoomInit2(int arg1, int arg2);
  bool MapZoomMain3();
  bool MapZoomInit3(int arg1, int arg2, int arg3, bool ex = false) override;
  bool MapMoveAnimeInit2(int arg1, int arg2, int arg3) override;
  bool MapMoveAnimeMain2() override;
  void MapPlayerPotalSelectInit();
  bool MapPlayerPotalSelect();
  void MapSystem_28();

  void Update(float dt) override;
  void Render() override;
  void RenderButtonGuide() override;

  struct MapPoolStruct {
    int id;
    int type;
  };

  struct MapGroupStruct {
    int a;
    int b;
    int c;
  };

  struct MapPoolDispStruct {
    struct {
      int shown;
      int inOrOut;
      int progress;
    } info;
    int animState;
    union {
      int angle;
      int pinId;
    };
  };

  struct MapPartsDispStruct {
    int partId;
    int type;
    int shown;
    int inOrOut;
    int progress;
    int animState;
    int angle;
    int dist;
  };
  MapBGState mapBgState = MapBGState::Hidden;

  struct MapPositionState {
    float x;
    float y;
    float size;
  };

  struct MapPositionTransitions {
    MapPositionState Start;
    MapPositionState Current;
    MapPositionState End;
  };

 private:
  void MapFadeMain();
  void MapSetPos();
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto