#include "audio_dac.h"
#include "esphome/core/log.h"

namespace esphome {
namespace es8311 {

static const char *const TAG = "es8311.audio_dac";

void ES8311AudioDAC::start() {
  if (!es8311_) {
    ESP_LOGE(TAG, "ES8311 component not initialized");
    return;
  }
  
  ESP_LOGD(TAG, "Starting ES8311 Audio DAC");
}

void ES8311AudioDAC::stop() {
  if (!es8311_) {
    ESP_LOGE(TAG, "ES8311 component not initialized");
    return;
  }
  
  ESP_LOGD(TAG, "Stopping ES8311 Audio DAC");
}

void ES8311AudioDAC::write_sample(int16_t sample) {
  if (!es8311_) {
    ESP_LOGE(TAG, "ES8311 component not initialized");
    return;
  }
  
  es8311_->write_audio_data(&sample, 1);
}

void ES8311AudioDAC::write_samples(const int16_t *samples, size_t count) {
  if (!es8311_) {
    ESP_LOGE(TAG, "ES8311 component not initialized");
    return;
  }
  
  es8311_->write_audio_data(samples, count);
}

}  // namespace es8311
}  // namespace esphome
