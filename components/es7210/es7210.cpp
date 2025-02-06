#include "es7210.h"
#include "microphone.h"
#include "esphome/core/log.h"
#include "driver/i2s.h"
#include "gpio_definitions.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

// [Rest of the existing implementation remains the same]

int ES7210Component::read_chunk_(int16_t *buffer, size_t length) {
  esp_err_t err = i2s_read(i2s_port_, buffer, length * sizeof(int16_t), &length, portMAX_DELAY);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read chunk: %s", esp_err_to_name(err));
    return -1;
  }
  return static_cast<int>(length);  // Return number of samples read
}

}  // namespace es7210
}  // namespace esphome

