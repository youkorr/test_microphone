import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, media_player, audio_dac

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ['i2c']
MULTI_CONF = True

es8311_ns = cg.esphome_ns.namespace('es8311')
ES8311Component = es8311_ns.class_('ES8311Component', cg.Component, i2c.I2CDevice)
ES8311MediaPlayer = es8311_ns.class_('ES8311MediaPlayer', media_player.MediaPlayer)
ES8311AudioDAC = es8311_ns.class_('ES8311AudioDAC', audio_dac.AudioDAC)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES8311Component),
    cv.Optional('sample_rate', default=44100): cv.int_,
    cv.Optional('bits_per_sample', default=16): cv.int_,
    cv.Optional('enable_pin', default=46): cv.int_,
}).extend(i2c.i2c_device_schema(0x18))

async def to_code(config):
    var = cg.new_Pvariable(config['id'])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    var.set_sample_rate(config['sample_rate'])
    var.set_bits_per_sample(config['bits_per_sample'])
    var.set_enable_pin(config['enable_pin'])

    # Media Player configuration
    player = cg.new_Pvariable(config['id'])
    await media_player.register_media_player(player, config)
    cg.add(player.set_es8311(var))

    # Audio DAC configuration
    dac = cg.new_Pvariable(config['id'])
    await audio_dac.register_audio_dac(dac, config)
    cg.add(dac.set_es8311(var))

