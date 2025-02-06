import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, microphone

DEPENDENCIES = ['i2c']
MULTI_CONF = True

es7210_ns = cg.esphome_ns.namespace('es7210')
ES7210Component = es7210_ns.class_('ES7210Component', cg.Component, i2c.I2CDevice)
ES7210Microphone = es7210_ns.class_('ES7210Microphone', microphone.Microphone)

CONF_SAMPLE_RATE = 'sample_rate'
CONF_BITS_PER_SAMPLE = 'bits_per_sample'
CONF_ID = 'id'  # Define CONF_ID explicitly

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Component),
    cv.Optional(CONF_SAMPLE_RATE, default=16000): cv.int_,
    cv.Optional(CONF_BITS_PER_SAMPLE, default=16): cv.int_,
    cv.Optional(CONF_ID, default='default_es7210'): cv.string,  # Define a default ID
}).extend(i2c.i2c_device_schema(0x40))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    sample_rate = config.get(CONF_SAMPLE_RATE)
    bits_per_sample = config.get(CONF_BITS_PER_SAMPLE)

    # Set the sample rate and bits per sample
    if sample_rate is not None:
        cg.add(var.set_sample_rate(sample_rate))
    
    if bits_per_sample is not None:
        cg.add(var.set_bits_per_sample(bits_per_sample))

    mic = cg.new_Pvariable(config[CONF_ID])
    await microphone.register_microphone(mic, config)
    cg.add(mic.set_es7210(var))

