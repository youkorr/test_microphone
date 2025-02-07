#pragma once
#include "esphome/components/media_player/media_player.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace media_player_es8311 {

class MediaPlayerES8311 : public media_player::MediaPlayer, public Component, public i2c::I2CDevice {
 public:
  MediaPlayerES8311() {}

  void setup() override;
  void loop() override {}
  void dump_config() override;

  void play() override;
  void stop() override;
  void pause() override;
  void set_volume(float volume) override;
  void mute(bool mute) override;
  void set_media_source(const std::string &source);

  void set_i2c(i2c::I2CComponent *i2c) { i2c_ = i2c; }
  void set_sample_rate(uint32_t sample_rate) { sample_rate_ = sample_rate; }

 protected:
  i2c::I2CComponent *i2c_{nullptr};
  uint32_t sample_rate_{16000};
};

}  // namespace media_player_es8311
}  // namespace esphome

