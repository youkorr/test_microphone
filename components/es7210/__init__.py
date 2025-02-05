import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ['i2c']
MULTI_CONF = True

CONF_SAMPLE_RATE = 'sample_rate'
CONF_BITS_PER_SAMPLE = 'bits_per_sample'

es7210_ns = cg.esphome_ns.namespace('es7210')
ES7210Component = es7210_ns.class_('ES7210Component', cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Component),
    cv.Optional(CONF_SAMPLE_RATE, default=16000): cv.int_,
    cv.Optional(CONF_BITS_PER_SAMPLE, default=16): cv.int_,
}).extend(i2c.i2c_device_schema(0x40))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_SAMPLE_RATE in config:
        cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    if CONF_BITS_PER_SAMPLE in config:
        cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))

