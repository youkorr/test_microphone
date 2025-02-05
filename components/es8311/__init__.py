import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ['i2c']
MULTI_CONF = True

es8311_ns = cg.esphome_ns.namespace('es8311')
ES8311Component = es8311_ns.class_('ES8311Component', cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES8311Component),
    cv.Optional('sample_rate', default=44100): cv.int_,
    cv.Optional('bits_per_sample', default=16): cv.int_,
    cv.Optional('mic_gain', default=0): cv.int_,
    cv.Optional('enable_pin', default=46): cv.int_,
}).extend(i2c.i2c_device_schema(0x18))

async def to_code(config):
    var = cg.new_Pvariable(config['id'])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if 'sample_rate' in config:
        cg.add(var.set_sample_rate(config['sample_rate']))
    if 'bits_per_sample' in config:
        cg.add(var.set_bits_per_sample(config['bits_per_sample']))
    if 'mic_gain' in config:
        cg.add(var.set_mic_gain(config['mic_gain']))
    if 'enable_pin' in config:
        cg.add(var.set_enable_pin(config['enable_pin']))
