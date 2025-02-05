#include "microphone.h"
#include "esphome/core/log.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210.microphone";

int ES7210Microphone::read_chunk_(int16_t *buffer, size_t length) {
  size_t bytes_read = 0;
  esp_err_t result = i2s_read(
    i2s_port_, 
    buffer, 
    length * sizeof(int16_t), 
    &bytes_read, 
    portMAX_DELAY
  );

  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Error reading I2S data: %s", esp_err_to_name(result));
    return 0;
  }

  return bytes_read / sizeof(int16_t);
}

}  // namespace es7210
}  // namespace esphome
