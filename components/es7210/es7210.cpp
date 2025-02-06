namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

// Définir les GPIOs manquants si non définis
#define GPIO_NUM_17 17
#define GPIO_NUM_45 45
#define GPIO_NUM_16 16

// Déclare le port I2S ici si nécessaire
i2s_port_t i2s_port_ = I2S_NUM_0;  // Définir le port I2S (ou I2S_NUM_1 selon ton matériel)

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

// Implémentation de la méthode read_chunk_
int ES7210Microphone::read_chunk_(int16_t *buffer, size_t length) {
  // Ici tu peux ajouter la logique de lecture des données I2S du codec ES7210
  esp_err_t err = i2s_read(i2s_port_, buffer, length * sizeof(int16_t), &length, portMAX_DELAY);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read chunk: %s", esp_err_to_name(err));
    return -1;
  }
  return length;  // Retourne le nombre d'échantillons lus
}

}  // namespace es7210
}  // namespace esphome





