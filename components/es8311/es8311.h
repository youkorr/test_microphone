#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "driver/i2s_std.h"
#include "esp_err.h"

namespace esphome {
namespace es8311 {

class ES8311Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  void shutdown() override;

  void set_sample_rate(uint32_t rate) { sample_rate_ = rate; }
  void set_bits_per_sample(uint8_t bits) { bits_per_sample_ = bits; }

  i2s_chan_handle_t get_i2s_handle() { return tx_handle; }
  void write_audio_data(const int16_t* data, size_t length);

 private:
  uint32_t sample_rate_ = 16000;
  uint8_t bits_per_sample_ = 16;
  
  i2s_chan_handle_t tx_handle = NULL;
  gpio_num_t enable_pin_ = GPIO_NUM_46;

  // ES8311 Register Definitions
  static constexpr uint8_t REG_RESET = 0x00;
  static constexpr uint8_t REG_POWER_DOWN = 0x03;
  static constexpr uint8_t REG_ANALOG_CTRL = 0x04;
  static constexpr uint8_t REG_SYSTEM_CTRL = 0x0D;

  bool initialize_codec();
  bool configure_i2s();
  bool configure_gpio();
  
  bool write_register(uint8_t reg, uint8_t value);
  uint8_t read_register(uint8_t reg);
};

}  // namespace es8311
}  // namespace esphome
