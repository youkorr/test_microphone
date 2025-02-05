#include "i2s_audio.h"
#include "esphome/core/log.h"

namespace esphome {
namespace s3_box_audio {

static const char *const TAG = "s3_box_audio.i2s";

void S3BoxI2SAudio::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2S Audio for ESP32 S3 Box 3");
  i2s_audio::I2SAudio::setup();
}

void S3BoxI2SAudio::dump_config() {
  ESP_LOGCONFIG(TAG, "I2S Audio Configuration:");
  i2s_audio::I2SAudio::dump_config();
}

}  // namespace s3_box_audio
}  // namespace esphome
