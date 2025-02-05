#include "es8311.h"
#include "esphome/core/log.h"
#include "driver/gpio.h"

namespace esphome {
namespace es8311 {

static const char *const TAG = "es8311";

void ES8311Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES8311 Audio Codec");

  // Configure GPIO first
  if (!configure_gpio()) {
    ESP_LOGE(TAG, "GPIO configuration failed");
    mark_failed();
    return;
  }

  // Initialize codec
  if (!initialize_codec()) {
    ESP_LOGE(TAG, "Codec initialization failed");
    mark_failed();
    return;
  }

  // Configure I2S
  if (!configure_i2s()) {
    ESP_LOGE(TAG, "I2S configuration failed");
    mark_failed();
    return;
  }
}

bool ES8311Component::configure_gpio() {
  // Configure enable pin for the codec
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << enable_pin_);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  
  esp_err_t result = gpio_config(&io_conf);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to configure enable pin");
    return false;
  }

  // Enable the codec
  gpio_set_level(enable_pin_, 1);
  delay(10);

  return true;
}

bool ES8311Component::initialize_codec() {
  // Soft reset
  write_register(REG_RESET, 0x80);
  delay(10);
  write_register(REG_RESET, 0x00);

  // Power management and system control
  write_register(REG_POWER_DOWN, 0x00);  // Power up
  write_register(REG_SYSTEM_CTRL, 0x02); // Enable DAC
  write_register(REG_ANALOG_CTRL, 0x33); // Analog path configuration

  return true;
}

bool ES8311Component::configure_i2s() {
  // I2S Channel Configuration
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
  ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

  // I2S Standard Configuration for Transmit
  i2s_std_config_t i2s_config = {
    .clk_cfg = {
      .mclk_freq = sample_rate_ * 256,
      .bclk_freq = sample_rate_ * 32,
      .mclk_multiple = I2S_MCLK_MULTIPLE_256
    },
    .slot_cfg = {
      .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
      .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
      .slot_mode = I2S_SLOT_MODE_STEREO,
      .slot_mask = I2S_SLOT_RIGHT
    },
    .gpio_cfg = {
      .mclk = GPIO2,   // MCLK
      .bclk = GPIO17,  // BCLK
      .ws = GPIO45,    // LRCLK
      .dout = GPIO15,  // Audio output pin
      .din = I2S_GPIO_UNUSED,
      .invert_flags = {
        .mclk_inv = false,
        .bclk_inv = false,
        .ws_inv = false
      }
    }
  };

  ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &i2s_config));
  ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

  return true;
}

void ES8311Component::write_audio_data(const int16_t* data, size_t length) {
  if (!tx_handle) {
    ESP_LOGE(TAG, "I2S handle not initialized");
    return;
  }

  size_t bytes_written = 0;
  esp_err_t result = i2s_channel_write(
    tx_handle, 
    data, 
    length * sizeof(int16_t), 
    &bytes_written, 
    portMAX_DELAY
  );

  if (result != ESP_OK) {
    ESP_LOGE(TAG, "I2S write error: %s", esp_err_to_name(result));
  }
}

void ES8311Component::shutdown() {
  // Disable codec
  gpio_set_level(enable_pin_, 0);

  // Shutdown I2S
  if (tx_handle) {
    i2s_channel_disable(tx_handle);
    i2s_del_channel(tx_handle);
    tx_handle = NULL;
  }
}

void ES8311Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ES8311 Audio Codec Configuration:");
  ESP_LOGCONFIG(TAG, "  Sample Rate: %u Hz", sample_rate_);
  ESP_LOGCONFIG(TAG, "  Bits per Sample: %u", bits_per_sample_);
  ESP_LOGCONFIG(TAG, "  Enable Pin: GPIO%d", enable_pin_);
}

bool ES8311Component::write_register(uint8_t reg, uint8_t value) {
  return this->write_byte(reg, value);
}

uint8_t ES8311Component::read_register(uint8_t reg) {
  uint8_t value;
  return this->read_byte(reg, &value) ? value : 0xFF;
}

}  // namespace es8311
}  // namespace esphome
