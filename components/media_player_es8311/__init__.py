import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import (
    media_player
)
from esphome.const import CONF_ID

# Déclaration du composant
media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_("MediaPlayerES8311", media_player.MediaPlayer, cg.Component)

# Configuration du schéma
CONFIG_SCHEMA = media_player.MEDIA_PLAYER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(MediaPlayerES8311),
}).extend(cv.COMPONENT_SCHEMA)

# Enregistrement de la plateforme
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)

# Exportation obligatoire pour ESPHome
def __getattr__(name):
    if name == "MEDIA_PLAYER_ES8311_SCHEMA":
        return CONFIG_SCHEMA
    raise AttributeError(f"Module 'media_player_es8311' n'a pas d'attribut '{name}'")


