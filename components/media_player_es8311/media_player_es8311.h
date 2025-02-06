// media_player_es8311.h
#pragma once

#include "esphome/components/media_player/media_player.h"
#include "esphome/components/i2s_audio/i2s_audio.h"
#include "esphome/components/audio_dac/audio_dac.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace es8311 {

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

 protected:
  void process_audio_();

  std::string current_source_;
  bool is_muted_ = false;
  float volume_ = 1.0f;
};

}  // namespace es8311
}  // namespace esphome
