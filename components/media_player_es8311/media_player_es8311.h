#pragma once
#include "esphome/components/media_player/media_player.h"
#include "esphome/core/component.h"
#include "esphome/components/audio_dac/audio_dac.h" //This has to be included.
#include "esphome/components/output/binary_output.h" //This has to be included

namespace esphome {
namespace media_player_es8311 {

class MediaPlayerES8311 : public media_player::MediaPlayer, public Component {
 public:
  MediaPlayerES8311() {}

  void setup() override;
  void loop() override;
  void dump_config() override;

  void play() override;
  void stop() override;
  void pause() override;
  void set_volume(float volume) override;
  void mute(bool mute) override;
  void set_media_source(const std::string &source);

  void set_audio_dac(audio_dac::AudioDAC *dac) { dac_ = dac; } //This has to be included
  void set_speaker(output::BinaryOutput *speaker) { speaker_ = speaker; } //This has to be included.
  void set_sample_rate(uint32_t sample_rate) { sample_rate_ = sample_rate; } //This has to be included

 protected:
  audio_dac::AudioDAC *dac_{nullptr}; //This has to be included
  output::BinaryOutput *speaker_{nullptr}; //This has to be included
  uint32_t sample_rate_{16000}; //This has to be included

};

}  // namespace media_player_es8311
}  // namespace esphome

