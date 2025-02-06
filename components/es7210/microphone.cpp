#include "microphone.h"
#include "es7210.h"
#include "esphome/core/log.h"
#include "driver/i2s.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210.microphone";

int ES7210Microphone::read_chunk_(int16_t *buffer, size_t length) {
  if (!parent_) {
    ESP_LOGE(TAG, "ES7210Component not set");
    return -1;
  }

  return parent_->read_chunk_(buffer, length);
}

}  // namespace es7210
}  // namespace esphome



