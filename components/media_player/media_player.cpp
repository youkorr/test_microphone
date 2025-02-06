#include "media_player.h"
#include "esphome/core/log.h"

namespace esphome {
namespace media_player {

static const char *const TAG = "media_player";

void MediaPlayer::play_audio(const std::string &url) {
    ESP_LOGD(TAG, "Lecture audio : %s", url.c_str());

    // Vérifier que ES8311 est actif
    ESP_LOGD(TAG, "Vérification de l'activation du DAC ES8311...");
    uint8_t reg_value = this->read_register(0x02);
    ESP_LOGD(TAG, "Registre DAC (0x02) = %02X", reg_value);
    if (reg_value != 0x3C) {
        ESP_LOGW(TAG, "Le DAC ES8311 n'est PAS activé correctement !");
        return;
    }

    // Sélectionner le format et lire l'audio
    if (url.find(".wav") != std::string::npos) {
        ESP_LOGD(TAG, "Lecture d'un fichier WAV...");
        // Ajouter ici la fonction de lecture WAV -> I2S
    } else if (url.find(".mp3") != std::string::npos) {
        ESP_LOGD(TAG, "Lecture d'un fichier MP3 (décodage requis)...");
        // Ajouter ici la fonction de décodage MP3 et envoi I2S
    } else {
        ESP_LOGW(TAG, "Format non supporté !");
    }
}

void MediaPlayerCall::perform() {
    ESP_LOGD(TAG, "'%s' - Exécution de la commande", this->parent_->get_name().c_str());
    this->validate_();
    if (this->media_url_.has_value()) {
        ESP_LOGD(TAG, "  Media URL: %s", this->media_url_.value().c_str());
        this->parent_->play_audio(this->media_url_.value());
    }
}

}  // namespace media_player
}  // namespace esphome

