#pragma once

#include "esphome/components/audio_dac/audio_dac.h"
#include "es8311.h"

namespace esphome {
namespace es8311 {

class ES8311AudioDAC : public audio_dac::AudioDAC {
 public:
  void set_es8311(ES8311Component *es8311) { es8311_ = es8311; }

  void start() override;
  void stop() override;
  void write_sample(int16_t sample) override;
  void write_samples(const int16_t *samples, size_t count) override;

 private:
  ES8311Component *es8311_ = nullptr;
};

}  // namespace es8311
}  // namespace esphome
