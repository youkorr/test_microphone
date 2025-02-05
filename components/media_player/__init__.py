import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME
from esphome.components import i2s_audio
from esphome.core import coroutine_with_priority

CODEOWNERS = ["@youkorr"]

IS_PLATFORM_COMPONENT = True

media_player_ns = cg.esphome_ns.namespace("media_player")
MediaPlayer = media_player_ns.class_("MediaPlayer", cg.Component)

# Configuration schema MODIFIÉE
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MediaPlayer),
    cv.Required(CONF_NAME): cv.string,
    # Utilisez directement l'ID du composant i2s_audio sans référence de classe
    cv.Required("i2s_audio_id"): cv.use_id(i2s_audio.I2SAudioComponent),  # <-- Changement clé ici
}).extend(cv.COMPONENT_SCHEMA)

@coroutine_with_priority(100.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_name(config[CONF_NAME]))
    
    # Lier i2s_audio via son ID générique
    i2s_audio_var = await cg.get_variable(config["i2s_audio_id"])
    cg.add(var.set_i2s_audio(i2s_audio_var))
