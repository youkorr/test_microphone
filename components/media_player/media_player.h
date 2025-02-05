#pragma once

#include "esphome/core/entity_base.h"
#include "esphome/core/helpers.h"
#include "esphome/components/i2s_audio/i2s_audio.h"  // Ajout de la référence à i2s_audio

namespace esphome {
namespace media_player {

// États du lecteur média
enum MediaPlayerState : uint8_t {
  MEDIA_PLAYER_STATE_NONE = 0,
  MEDIA_PLAYER_STATE_IDLE = 1,
  MEDIA_PLAYER_STATE_PLAYING = 2,
  MEDIA_PLAYER_STATE_PAUSED = 3,
  MEDIA_PLAYER_STATE_ANNOUNCING = 4
};
const char *media_player_state_to_string(MediaPlayerState state);

// Commandes du lecteur média
enum MediaPlayerCommand : uint8_t {
  MEDIA_PLAYER_COMMAND_PLAY = 0,
  MEDIA_PLAYER_COMMAND_PAUSE = 1,
  MEDIA_PLAYER_COMMAND_STOP = 2,  // La commande STOP est bien présente
  MEDIA_PLAYER_COMMAND_MUTE = 3,
  MEDIA_PLAYER_COMMAND_UNMUTE = 4,
  MEDIA_PLAYER_COMMAND_TOGGLE = 5,
  MEDIA_PLAYER_COMMAND_VOLUME_UP = 6,
  MEDIA_PLAYER_COMMAND_VOLUME_DOWN = 7,
};
const char *media_player_command_to_string(MediaPlayerCommand command);

// Classe principale du lecteur média
class MediaPlayer : public EntityBase {
 public:
  MediaPlayerState state{MEDIA_PLAYER_STATE_NONE};  // État actuel du lecteur
  float volume{1.0f};  // Volume actuel

  // Méthode pour créer une commande
  MediaPlayerCall make_call() { return MediaPlayerCall(this); }

  // Méthode pour publier l'état actuel
  void publish_state();

  // Méthode pour ajouter un callback lors des changements d'état
  void add_on_state_callback(std::function<void()> &&callback);

  // Méthode virtuelle pour vérifier si le lecteur est en mode muet
  virtual bool is_muted() const { return false; }

  // Méthode virtuelle pour obtenir les traits du lecteur
  virtual MediaPlayerTraits get_traits() = 0;

 protected:
  friend MediaPlayerCall;

  // Méthode virtuelle pour contrôler le lecteur
  virtual void control(const MediaPlayerCall &call) = 0;

  // CallbackManager pour gérer les callbacks d'état
  CallbackManager<void()> state_callback_{};
};

// Classe pour gérer les commandes du lecteur média
class MediaPlayerCall {
 public:
  MediaPlayerCall(MediaPlayer *parent) : parent_(parent) {}

  // Méthodes pour définir les commandes
  MediaPlayerCall &set_command(MediaPlayerCommand command);
  MediaPlayerCall &set_command(optional<MediaPlayerCommand> command);
  MediaPlayerCall &set_command(const std::string &command);

  // Méthodes pour définir les propriétés de la commande
  MediaPlayerCall &set_media_url(const std::string &url);
  MediaPlayerCall &set_volume(float volume);
  MediaPlayerCall &set_announcement(bool announce);

  // Méthode pour exécuter la commande
  void perform();

  // Getters pour les propriétés de la commande
  const optional<MediaPlayerCommand> &get_command() const { return command_; }
  const optional<std::string> &get_media_url() const { return media_url_; }
  const optional<float> &get_volume() const { return volume_; }
  const optional<bool> &get_announcement() const { return announcement_; }

 protected:
  // Méthode pour valider la commande
  void validate_();

  // Référence au lecteur média parent
  MediaPlayer *const parent_;

  // Propriétés de la commande
  optional<MediaPlayerCommand> command_;
  optional<std::string> media_url_;
  optional<float> volume_;
  optional<bool> announcement_;
};

}  // namespace media_player
}  // namespace esphome
