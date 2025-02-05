import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, microphone

DEPENDENCIES = ['i2c']
MULTI_CONF = True

es7210_ns = cg.esphome_ns.namespace('es7210')
ES7210Component = es7210_ns.class_('ES7210Component', cg.Component, i2c.I2CDevice)
ES7210Microphone = es7210_ns.class_('ES7210Microphone', microphone.Microphone)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Component),
    cv.Optional('sample_rate', default=16000): cv.int_,
    cv.Optional('bits_per_sample', default=16): cv.int_,
    cv.Optional('i2s_port', default=0): cv.int_,
}).extend(i2c.i2c_device_schema(0x40))

async def to_code(config):
    var = cg.new_Pvariable(config['id'])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    var.set_sample_rate(config['sample_rate'])
    var.set_bits_per_sample(config['bits_per_sample'])
    var.set_i2s_port(config['i2s_port'])

    mic = cg.new_Pvariable(config['id'])
    await microphone.register_microphone(mic, config)
    cg.add(mic.set_es7210(var))
