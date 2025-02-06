import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import media_player
from esphome.components.i2s_audio import I2SAudioOut
from ..es8311 import ES8311

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ["media_player", "i2s_audio", "es8311"]

media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_("MediaPlayerES8311", media_player.MediaPlayer, cg.Component)

CONFIG_SCHEMA = media_player.MEDIA_PLAYER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(MediaPlayerES8311),
    cv.Required("i2s_audio"): cv.use_id(I2SAudioOut),
    cv.Required("es8311"): cv.use_id(ES8311),
    cv.Optional("sample_rate", default=48000): cv.int_range(min=8000, max=96000),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)
    
    i2s_audio = await cg.get_variable(config["i2s_audio"])
    cg.add(var.set_i2s_audio(i2s_audio))
    
    es8311 = await cg.get_variable(config["es8311"])
    cg.add(var.set_es8311(es8311))
    
    cg.add(var.set_sample_rate(config["sample_rate"]))

