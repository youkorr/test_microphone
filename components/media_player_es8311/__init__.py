import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import media_player, i2c
from esphome.const import CONF_ID, CONF_I2C_ID, CONF_SAMPLE_RATE

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ["i2c"]

media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_(
    "MediaPlayerES8311", media_player.MediaPlayer, cg.Component, i2c.I2CDevice
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MediaPlayerES8311),
    cv.Required(CONF_I2C_ID): cv.use_id(i2c.I2CComponent), #CHANGED
    cv.Optional(CONF_SAMPLE_RATE, default=16000): cv.int_range(min=1),
}).extend(media_player.MEDIA_PLAYER_SCHEMA).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], template=MediaPlayerES8311())
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)

    i2c_component = await cg.get_variable(config[CONF_I2C_ID])
    cg.add(var.set_i2c(i2c_component))

    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))


