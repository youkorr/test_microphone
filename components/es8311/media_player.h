#include "esphome/core/log.h"
#include "ES8311.h"

namespace esphome {
namespace es8311 {

static const char *const TAG = "es8311";

// ES8311 Register Map
static const uint8_t ES8311_CLK_MANAGER_REG01 = 0x01;
static const uint8_t ES8311_CHIP_STATUS_REG02 = 0x02;
static const uint8_t ES8311_RESET_REG03 = 0x03;
static const uint8_t ES8311_CLOCK_DIV_REG04 = 0x04;
static const uint8_t ES8311_DAC_OSR_REG07 = 0x07;
static const uint8_t ES8311_DAC_MIXING_REG15 = 0x15;
static const uint8_t ES8311_DAC_VOLUME_REG33 = 0x33;
static const uint8_t ES8311_ADC_VOLUME_REG34 = 0x34;
static const uint8_t ES8311_DAC_PDN_REG38 = 0x38;
static const uint8_t ES8311_ADC_PDN_REG3C = 0x3C;

void ES8311Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES8311...");
  
  // Initialize enable pin if set
  if (this->enable_pin_ != 0) {
    pinMode(this->enable_pin_, OUTPUT);
    digitalWrite(this->enable_pin_, HIGH);
    delay(10);  // Wait for power-up
  }
  
  // Software reset
  if (!this->write_byte(ES8311_RESET_REG03, 0x3F)) {
    ESP_LOGE(TAG, "Error writing reset register");
    this->mark_failed();
    return;
  }
  delay(10);
  if (!this->write_byte(ES8311_RESET_REG03, 0x00)) {
    ESP_LOGE(TAG, "Error clearing reset register");
    this->mark_failed();
    return;
  }
  
  // Configure clock settings
  this->write_byte(ES8311_CLK_MANAGER_REG01, 0x30);  // MCLK from external, BCLK from MCLK
  this->write_byte(ES8311_CLOCK_DIV_REG04, 0x10);    // LRCK = MCLK/256
  
  // Configure sample rate
  uint8_t osr_value;
  if (this->sample_rate_ <= 16000)
    osr_value = 0x20;      // OSR = 256
  else if (this->sample_rate_ <= 32000)
    osr_value = 0x10;      // OSR = 128
  else
    osr_value = 0x00;      // OSR = 64
    
  this->write_byte(ES8311_DAC_OSR_REG07, osr_value);
  
  // Configure DAC
  this->write_byte(ES8311_DAC_MIXING_REG15, 0x10);  // DAC mixer settings
  this->write_byte(ES8311_DAC_PDN_REG38, 0x02);     // Power up DAC
  
  // Configure ADC
  this->write_byte(ES8311_ADC_PDN_REG3C, 0x02);     // Power up ADC
  this->write_byte(ES8311_ADC_VOLUME_REG34, this->mic_gain_);  // Set mic gain
  
  // Set initial volume
  this->set_volume(this->volume_);
  
  ESP_LOGCONFIG(TAG, "ES8311 setup complete");
}

void ES8311Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ES8311:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Sample Rate: %u Hz", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  Bits Per Sample: %u", this->bits_per_sample_);
  ESP_LOGCONFIG(TAG, "  Enable Pin: %u", this->enable_pin_);
  ESP_LOGCONFIG(TAG, "  Mic Gain: %u", this->mic_gain_);
}

void ES8311Component::set_volume(float volume) {
  if (volume < 0.0f)
    volume = 0.0f;
  else if (volume > 1.0f)
    volume = 1.0f;
    
  this->volume_ = volume;
  
  // Convert float volume (0.0-1.0) to ES8311 volume register value (0x00-0xFF)
  // ES8311 volume is inverse: 0x00 is maximum volume, 0xFF is minimum
  uint8_t reg_value = static_cast<uint8_t>((1.0f - volume) * 255);
  
  if (!this->is_muted_) {
    this->write_byte(ES8311_DAC_VOLUME_REG33, reg_value);
  }
  
  ESP_LOGD(TAG, "Setting volume: %.2f (reg: 0x%02X)", volume, reg_value);
}

void ES8311Component::set_mute(bool mute) {
  this->is_muted_ = mute;
  
  if (mute) {
    this->write_byte(ES8311_DAC_VOLUME_REG33, 0xFF);  // Mute by setting volume to minimum
  } else {
    // Restore previous volume
    float volume = this->volume_;
    uint8_t reg_value = static_cast<uint8_t>((1.0f - volume) * 255);
    this->write_byte(ES8311_DAC_VOLUME_REG33, reg_value);
  }
  
  ESP_LOGD(TAG, "%s audio", mute ? "Muting" : "Unmuting");
}

// MediaPlayer Implementation
void ES8311MediaPlayer::setup() {
  this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
}

media_player::MediaPlayerTraits ES8311MediaPlayer::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_pause(true);
  traits.set_supports_volume(true);
  traits.set_supports_volume_set(true);
  return traits;
}

void ES8311MediaPlayer::control(const media_player::MediaPlayerCall &call) {
  if (call.get_volume().has_value()) {
    float volume = *call.get_volume();
    this->parent_->set_volume(volume);
  }
  
  if (call.get_state().has_value()) {
    auto state = *call.get_state();
    switch (state) {
      case media_player::MEDIA_PLAYER_STATE_PLAYING:
        this->state = media_player::MEDIA_PLAYER_STATE_PLAYING;
        this->parent_->set_mute(false);
        break;
      case media_player::MEDIA_PLAYER_STATE_PAUSED:
        this->state = media_player::MEDIA_PLAYER_STATE_PAUSED;
        this->parent_->set_mute(true);
        break;
      case media_player::MEDIA_PLAYER_STATE_IDLE:
        this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
        this->parent_->set_mute(true);
        break;
      default:
        break;
    }
  }
}

}  // namespace es8311
}  // namespace esphome

