#include "esphome/components/es7210/es7210.h"
#include "esphome/components/es7210/microphone.h"
#include "esphome/core/log.h"
#include "driver/i2s.h"
#include "gpio_definitions.h"  // Include the GPIO definitions here

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

void ES7210Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES7210 Codec for ESP32-S3 Box 3");

  // Vérifier la présence du périphérique
  if (!this->read_register(ES7210_RESET_REG)) {
    ESP_LOGE(TAG, "ES7210 not found at address 0x%02X", this->address_);
    mark_failed();
    return;
  }

  // Initialiser le codec et configurer I2S
  initialize_codec();
  configure_analog_path();
  configure_i2s();
}

void ES7210Component::initialize_codec() {
  // Séquence de réinitialisation
  write_register(ES7210_RESET_REG, 0x80);
  vTaskDelay(10 / portTICK_PERIOD_MS);  // Remplacer delay() par vTaskDelay
  write_register(ES7210_RESET_REG, 0x00);

  // Gestion de l'alimentation et configuration de l'horloge
  write_register(ES7210_MAINCLK_REG, 0x00);  // Configuration de l'horloge maître
  write_register(ES7210_POWER_REG, 0x00);    // Alimentation de tous les canaux
  write_register(ES7210_PDN_REG, 0x00);      // Désactivation de la mise en veille
}

void ES7210Component::configure_analog_path() {
  // Configuration du chemin analogique pour le S3 Box 3
  write_register(ES7210_ANALOG_REG, 0x33);  // Exemple : configurer le chemin d'entrée analogique
}

bool ES7210Component::configure_i2s() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sample_rate_,
    .bits_per_sample = (i2s_bits_per_sample_t)bits_per_sample_,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = GPIO_NUM_17,     // Pin BCLK
    .ws_io_num = GPIO_NUM_45,      // Pin LRCLK
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = GPIO_NUM_16     // Pin DOUT pour microphone
  };

  esp_err_t result = i2s_driver_install(i2s_port_, &i2s_config, 0, NULL);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to install I2S driver: %s", esp_err_to_name(result));
    return false;
  }

  result = i2s_set_pin(i2s_port_, &pin_config);
  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set I2S pins: %s", esp_err_to_name(result));
    return false;
  }

  return true;
}

void ES7210Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ES7210 Codec Configuration:");
  ESP_LOGCONFIG(TAG, "  I2C Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Sample Rate: %u Hz", sample_rate_);
  ESP_LOGCONFIG(TAG, "  Bits per Sample: %u", bits_per_sample_);
}

bool ES7210Component::write_register(uint8_t reg, uint8_t value) {
  return this->write_byte(reg, value);
}

uint8_t ES7210Component::read_register(uint8_t reg) {
  uint8_t value;
  return this->read_byte(reg, &value) ? value : 0xFF;
}

// Implémentation de la méthode read_chunk_
int ES7210Microphone::read_chunk_(int16_t *buffer, size_t length) {
  // Ici tu peux ajouter la logique de lecture des données I2S du codec ES7210
  esp_err_t err = i2s_read(i2s_port_, buffer, length * sizeof(int16_t), &length, portMAX_DELAY);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read chunk: %s", esp_err_to_name(err));
    return -1;
  }
  return static_cast<int>(length);  // Retourne le nombre d'échantillons lus
}

}  // namespace es7210
}  // namespace esphome

