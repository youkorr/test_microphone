import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import media_player
from esphome.components.audio_dac import AudioDAC

DEPENDENCIES = ["media_player", "i2s_audio", "audio_dac"]

media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_("MediaPlayerES8311", media_player.MediaPlayer, cg.Component)

CONFIG_SCHEMA = media_player.MEDIA_PLAYER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(MediaPlayerES8311),
    cv.Required("audio_dac"): cv.use_id(AudioDAC),
    cv.Optional("sample_rate", default=48000): cv.int_range(min=8000, max=96000),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)
    
    audio_dac = await cg.get_variable(config["audio_dac"])
    cg.add(var.set_audio_dac(audio_dac))
    cg.add(var.set_sample_rate(config["sample_rate"]))

