#pragma once
#include "esphome/components/media_player/media_player.h"
#include "esphome/core/component.h"
#include "esphome/components/i2s_audio/i2s_audio.h"

namespace esphome {
namespace media_player_es8311 {

class MediaPlayerES8311 : public media_player::MediaPlayer, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_i2s_audio(i2s_audio::I2SAudioOut *i2s_audio) { this->i2s_audio_ = i2s_audio; }
  void set_sample_rate(uint32_t sample_rate) { this->sample_rate_ = sample_rate; }

  void play() override;
  void stop() override;
  void pause() override;
  void set_volume(float volume) override;
  void mute(bool mute) override;
  void set_media_source(const std::string &source);

 protected:
  i2s_audio::I2SAudioOut *i2s_audio_;
  uint32_t sample_rate_;
};

}  // namespace media_player_es8311
}  // namespace esphome
