#pragma once
#include "esphome/components/media_player/media_player.h"
#include "esphome/core/component.h"

namespace esphome {
namespace media_player_es8311 {

class MediaPlayerES8311 : public media_player::MediaPlayer, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void play() override;
  void stop() override;
  void pause() override;
  void set_volume(float volume) override;
  void mute(bool mute) override;
  void set_media_source(const std::string &source);
};

}  // namespace media_player_es8311
}  // namespace esphome

