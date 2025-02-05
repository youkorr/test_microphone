import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, audio_dac

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ['i2c']
MULTI_CONF = True

es8311_ns = cg.esphome_ns.namespace('es8311')
ES8311Component = es8311_ns.class_('ES8311Component', cg.Component, i2c.I2CDevice)
ES8311AudioDAC = es8311_ns.class_('ES8311AudioDAC', audio_dac.AudioDAC)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES8311AudioDAC),
    cv.GenerateID('es8311_id'): cv.use_id(ES8311Component),
    cv.Optional('sample_rate', default=44100): cv.int_,
    cv.Optional('bits_per_sample', default=16): cv.int_,
    cv.Optional('mic_gain', default=0): cv.int_,
}).extend(i2c.i2c_device_schema(0x18))

async def to_code(config):
    var = cg.new_Pvariable(config['id'])
    await audio_dac.register_audio_dac(var, config)

    # Link to ES8311 component
    es8311_component = await cg.get_variable(config['es8311_id'])
    cg.add(var.set_es8311(es8311_component))

    # Set additional parameters
    if 'sample_rate' in config:
        cg.add(es8311_component.set_sample_rate(config['sample_rate']))
    if 'bits_per_sample' in config:
        cg.add(es8311_component.set_bits_per_sample(config['bits_per_sample']))
    if 'mic_gain' in config:
        cg.add(es8311_component.set_mic_gain(config['mic_gain']))

