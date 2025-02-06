#ifndef ES7210_H
#define ES7210_H

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "driver/i2s.h"
#include "microphone.h"

namespace esphome {
namespace es7210 {

class ES7210Microphone;  // Forward declaration

class ES7210Component : public Component, public i2c::I2CDevice {
public:
  void setup() override;
  void dump_config() override;

  void set_sample_rate(uint32_t rate) { sample_rate_ = rate; }
  void set_bits_per_sample(uint8_t bits) { bits_per_sample_ = bits; }

  bool configure_i2s();
  
  // Make this public and change to non-override
  int read_chunk_(int16_t *buffer, size_t length);

private:
  uint32_t sample_rate_ = 16000;
  uint8_t bits_per_sample_ = 16;
  i2s_port_t i2s_port_ = I2S_NUM_0;

  static constexpr uint8_t ES7210_RESET_REG = 0x00;
  static constexpr uint8_t ES7210_MAINCLK_REG = 0x01;
  static constexpr uint8_t ES7210_POWER_REG = 0x02;
  static constexpr uint8_t ES7210_PDN_REG = 0x03;
  static constexpr uint8_t ES7210_ANALOG_REG = 0x04;

  bool write_register(uint8_t reg, uint8_t value);
  uint8_t read_register(uint8_t reg);

  void initialize_codec();
  void configure_analog_path();
};

}  // namespace es7210
}  // namespace esphome

#endif  // ES7210_H


