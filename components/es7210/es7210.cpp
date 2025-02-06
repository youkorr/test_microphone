#include "esphome/core/log.h"  // Ajout pour ESP_LOGCONFIG et ESP_LOGE
#include "esphome/components/i2c/i2c.h"
#include "driver/i2s.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

void ES7210Component::setup() {
    ESP_LOGCONFIG(TAG, "Setting up ES7210 Codec for ESP32 S3 Box 3");

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

    // Enregistrer le taux d'échantillonnage et les bits par échantillon depuis la configuration
    uint32_t sample_rate = this->sample_rate_;
    uint8_t bits_per_sample = this->bits_per_sample_;

    ESP_LOGCONFIG(TAG, "Sample Rate: %u Hz", sample_rate);
    ESP_LOGCONFIG(TAG, "Bits per Sample: %u", bits_per_sample);
}

void ES7210Component::initialize_codec() {
    // Réinitialisation douce
    write_register(ES7210_RESET_REG, 0x80);
    vTaskDelay(10 / portTICK_PERIOD_MS);  // Remplacer delay() par vTaskDelay
    write_register(ES7210_RESET_REG, 0x00);

    // Gestion de l'alimentation et configuration de l'horloge
    write_register(ES7210_MAINCLK_REG, 0x00);
    write_register(ES7210_POWER_REG, 0x00);
    write_register(ES7210_PDN_REG, 0x00);
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
        .bck_io_num = GPIO_NUM_17,     // BCLK pin
        .ws_io_num = GPIO_NUM_45,      // LRCLK pin
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = GPIO_NUM_16     // DOUT pin pour microphone
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

}  // namespace es7210
}  // namespace esphome




