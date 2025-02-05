#pragma once

#include "esphome/components/media_player/media_player.h"
#include "es8311.h"

namespace esphome {
namespace es8311 {

class ES8311MediaPlayer : public media_player::MediaPlayer {
 public:
  void set_es8311(ES8311Component *es8311) { es8311_ = es8311; }

 protected:
  bool play_() override;
  bool pause_() override;
  bool stop_() override;
  bool unmute_() override;
  bool mute_() override;
  bool control_() override;

 private:
  ES8311Component *es8311_ = nullptr;
};

}  // namespace es8311
}  // namespace esphome

