#include "es8311.h"
#include "es8311_const.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <cinttypes>

namespace esphome {
namespace es8311 {

static const char *const TAG = "es8311";

// Mark the component as failed; use only in setup
#define ES8311_ERROR_FAILED(func) \
  if (!(func)) { \
    this->mark_failed(); \
    return; \
  }
// Return false; use outside of setup
#define ES8311_ERROR_CHECK(func) \
  if (!(func)) { \
    return false; \
  }

void ES8311::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES8311...");

  // Reset
  ESP_LOGD(TAG, "Resetting ES8311...");
  if (!this->write_byte(ES8311_REG00_RESET, 0x1F)) {
    ESP_LOGE(TAG, "Failed to reset ES8311!");
    this->mark_failed();
    return;
  }
  if (!this->write_byte(ES8311_REG00_RESET, 0x00)) {
    ESP_LOGE(TAG, "Failed to clear reset!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Reset completed.");

  // Configure clock
  ESP_LOGD(TAG, "Configuring clock...");
  if (!this->configure_clock_()) {
    ESP_LOGE(TAG, "Clock configuration failed!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Clock configuration completed.");

  // Configure format
  ESP_LOGD(TAG, "Configuring audio format...");
  if (!this->configure_format_()) {
    ESP_LOGE(TAG, "Audio format configuration failed!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Audio format configuration completed.");

  // Configure microphone
  ESP_LOGD(TAG, "Configuring microphone...");
  if (!this->configure_mic_()) {
    ESP_LOGE(TAG, "Microphone configuration failed!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Microphone configuration completed.");

  // Set initial volume
  ESP_LOGD(TAG, "Setting initial volume...");
  this->set_volume(0.75);  // 0.75 = 0xBF = 0dB
  ESP_LOGD(TAG, "Initial volume set to 0.75.");

  // Power up analog circuitry
  ESP_LOGD(TAG, "Powering up analog circuitry...");
  if (!this->write_byte(ES8311_REG0D_SYSTEM, 0x01)) {
    ESP_LOGE(TAG, "Failed to power up analog circuitry!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Analog circuitry powered up.");

  // Enable analog PGA, enable ADC modulator
  ESP_LOGD(TAG, "Enabling analog PGA and ADC modulator...");
  if (!this->write_byte(ES8311_REG0E_SYSTEM, 0x02)) {
    ESP_LOGE(TAG, "Failed to enable analog PGA and ADC modulator!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Analog PGA and ADC modulator enabled.");

  // Power up DAC
  ESP_LOGD(TAG, "Powering up DAC...");
  if (!this->write_byte(ES8311_REG12_SYSTEM, 0x00)) {
    ESP_LOGE(TAG, "Failed to power up DAC!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "DAC powered up.");

  // Enable output to HP drive
  ESP_LOGD(TAG, "Enabling output to HP drive...");
  if (!this->write_byte(ES8311_REG13_SYSTEM, 0x10)) {
    ESP_LOGE(TAG, "Failed to enable output to HP drive!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Output to HP drive enabled.");

  // ADC Equalizer bypass, cancel DC offset in digital domain
  ESP_LOGD(TAG, "Configuring ADC equalizer...");
  if (!this->write_byte(ES8311_REG1C_ADC, 0x6A)) {
    ESP_LOGE(TAG, "Failed to configure ADC equalizer!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "ADC equalizer configured.");

  // Bypass DAC equalizer
  ESP_LOGD(TAG, "Bypassing DAC equalizer...");
  if (!this->write_byte(ES8311_REG37_DAC, 0x08)) {
    ESP_LOGE(TAG, "Failed to bypass DAC equalizer!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "DAC equalizer bypassed.");

  // Power On
  ESP_LOGD(TAG, "Powering on ES8311...");
  if (!this->write_byte(ES8311_REG00_RESET, 0x80)) {
    ESP_LOGE(TAG, "Failed to power on ES8311!");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "ES8311 powered on.");

  ESP_LOGCONFIG(TAG, "ES8311 setup completed successfully.");
}

void ES8311::dump_config() {
  ESP_LOGCONFIG(TAG, "ES8311 Audio Codec:");
  ESP_LOGCONFIG(TAG, "  Use MCLK: %s", YESNO(this->use_mclk_));
  ESP_LOGCONFIG(TAG, "  Use Microphone: %s", YESNO(this->use_mic_));
  ESP_LOGCONFIG(TAG, "  DAC Bits per Sample: %" PRIu8, this->resolution_out_);
  ESP_LOGCONFIG(TAG, "  Sample Rate: %" PRIu32, this->sample_frequency_);

  if (this->is_failed()) {
    ESP_LOGCONFIG(TAG, "  Failed to initialize!");
    return;
  }
}

bool ES8311::set_volume(float volume) {
  volume = clamp(volume, 0.0f, 1.0f);
  uint8_t reg32 = remap<uint8_t, float>(volume, 0.0f, 1.0f, 0, 255);
  return this->write_byte(ES8311_REG32_DAC, reg32);
}

float ES8311::volume() {
  uint8_t reg32;
  this->read_byte(ES8311_REG32_DAC, &reg32);
  return remap<float, uint8_t>(reg32, 0, 255, 0.0f, 1.0f);
}

uint8_t ES8311::calculate_resolution_value(ES8311Resolution resolution) {
  switch (resolution) {
    case ES8311_RESOLUTION_16:
      return (3 << 2);
    case ES8311_RESOLUTION_18:
      return (2 << 2);
    case ES8311_RESOLUTION_20:
      return (1 << 2);
    case ES8311_RESOLUTION_24:
      return (0 << 2);
    case ES8311_RESOLUTION_32:
      return (4 << 2);
    default:
      return 0;
  }
}

const ES8311Coefficient *ES8311::get_coefficient(uint32_t mclk, uint32_t rate) {
  for (const auto &coefficient : ES8311_COEFFICIENTS) {
    if (coefficient.mclk == mclk && coefficient.rate == rate)
      return &coefficient;
  }
  return nullptr;
}

bool ES8311::configure_clock_() {
  uint32_t mclk_frequency = this->sample_frequency_ * this->mclk_multiple_;
  if (!this->use_mclk_) {
    mclk_frequency = this->sample_frequency_ * (int) this->resolution_out_ * 2;
  }

  auto *coefficient = get_coefficient(mclk_frequency, this->sample_frequency_);
  if (coefficient == nullptr) {
    ESP_LOGE(TAG, "Unsupported sample rate: %" PRIu32 "Hz with MCLK: %" PRIu32 "Hz", this->sample_frequency_, mclk_frequency);
    return false;
  }

  // Register 0x01
  uint8_t reg01 = 0x3F;
  if (!this->use_mclk_) {
    reg01 |= BIT(7);  // Use SCLK
  }
  if (this->mclk_inverted_) {
    reg01 |= BIT(6);  // Invert MCLK pin
  }
  if (!this->write_byte(ES8311_REG01_CLK_MANAGER, reg01)) {
    ESP_LOGE(TAG, "Failed to write to register 0x01!");
    return false;
  }

  // Register 0x02
  uint8_t reg02;
  if (!this->read_byte(ES8311_REG02_CLK_MANAGER, &reg02)) {
    ESP_LOGE(TAG, "Failed to read register 0x02!");
    return false;
  }
  reg02 &= 0x07;
  reg02 |= (coefficient->pre_div - 1) << 5;
  reg02 |= coefficient->pre_mult << 3;
  if (!this->write_byte(ES8311_REG02_CLK_MANAGER, reg02)) {
    ESP_LOGE(TAG, "Failed to write to register 0x02!");
    return false;
  }

  // Register 0x03
  const uint8_t reg03 = (coefficient->fs_mode << 6) | coefficient->adc_osr;
  if (!this->write_byte(ES8311_REG03_CLK_MANAGER, reg03)) {
    ESP_LOGE(TAG, "Failed to write to register 0x03!");
    return false;
  }

  // Register 0x04
  if (!this->write_byte(ES8311_REG04_CLK_MANAGER, coefficient->dac_osr)) {
    ESP_LOGE(TAG, "Failed to write to register 0x04!");
    return false;
  }

  // Register 0x05
  const uint8_t reg05 = ((coefficient->adc_div - 1) << 4) | (coefficient->dac_div - 1);
  if (!this->write_byte(ES8311_REG05_CLK_MANAGER, reg05)) {
    ESP_LOGE(TAG, "Failed to write to register 0x05!");
    return false;
  }

  // Register 0x06
  uint8_t reg06;
  if (!this->read_byte(ES8311_REG06_CLK_MANAGER, &reg06)) {
    ESP_LOGE(TAG, "Failed to read register 0x06!");
    return false;
  }
  if (this->sclk_inverted_) {
    reg06 |= BIT(5);
  } else {
    reg06 &= ~BIT(5);
  }
  reg06 &= 0xE0;
  if (coefficient->bclk_div < 19) {
    reg06 |= (coefficient->bclk_div - 1) << 0;
  } else {
    reg06 |= (coefficient->bclk_div) << 0;
  }
  if (!this->write_byte(ES8311_REG06_CLK_MANAGER, reg06)) {
    ESP_LOGE(TAG, "Failed to write to register 0x06!");
    return false;
  }

  // Register 0x07
  uint8_t reg07;
  if (!this->read_byte(ES8311_REG07_CLK_MANAGER, &reg07)) {
    ESP_LOGE(TAG, "Failed to read register 0x07!");
    return false;
  }
  reg07 &= 0xC0;
  reg07 |= coefficient->lrck_h << 0;
  if (!this->write_byte(ES8311_REG07_CLK_MANAGER, reg07)) {
    ESP_LOGE(TAG, "Failed to write to register 0x07!");
    return false;
  }

  // Register 0x08
  if (!this->write_byte(ES8311_REG08_CLK_MANAGER, coefficient->lrck_l)) {
    ESP_LOGE(TAG, "Failed to write to register 0x08!");
    return false;
  }

  return true;
}

bool ES8311::configure_format_() {
  uint8_t reg00;
  if (!this->read_byte(ES8311_REG00_RESET, &reg00)) {
    ESP_LOGE(TAG, "Failed to read register 0x00!");
    return false;
  }
  reg00 &= 0xBF;
  if (!this->write_byte(ES8311_REG00_RESET, reg00)) {
    ESP_LOGE(TAG, "Failed to write to register 0x00!");
    return false;
  }

  uint8_t reg09 = calculate_resolution_value(this->resolution_in_);
  if (!this->write_byte(ES8311_REG09_SDPIN, reg09)) {
    ESP_LOGE(TAG, "Failed to write to register 0x09!");
    return false;
  }

  uint8_t reg0a = calculate_resolution_value(this->resolution_out_);
  if (!this->write_byte(ES8311_REG0A_SDPOUT, reg0a)) {
    ESP_LOGE(TAG, "Failed to write to register 0x0A!");
    return false;
  }

  return true;
}

bool ES8311::configure_mic_() {
  uint8_t reg14 = 0x1A;  // Enable analog MIC and max PGA gain
  if (this->use_mic_) {
    reg14 |= BIT(6);  // Enable PDM digital microphone
  }
  if (!this->write_byte(ES8311_REG14_SYSTEM, reg14)) {
    ESP_LOGE(TAG, "Failed to write to register 0x14!");
    return false;
  }

  if (!this->write_byte(ES8311_REG16_ADC, this->mic_gain_)) {
    ESP_LOGE(TAG, "Failed to write to register 0x16!");
    return false;
  }

  if (!this->write_byte(ES8311_REG17_ADC, 0xC8)) {
    ESP_LOGE(TAG, "Failed to write to register 0x17!");
    return false;
  }

  return true;
}

bool ES8311::set_mute_state_(bool mute_state) {
  uint8_t reg31;

  this->is_muted_ = mute_state;

  if (!this->read_byte(ES8311_REG31_DAC, &reg31)) {
    return false;
  }

  if (mute_state) {
    reg31 |= BIT(6) | BIT(5);
  } else {
    reg31 &= ~(BIT(6) | BIT(5));
  }

  return this->write_byte(ES8311_REG31_DAC, reg31);
}

}  // namespace es8311
}  // namespace esphome
