import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, microphone, i2s_audio
from esphome.const import CONF_ID, CONF_BITS_PER_SAMPLE, CONF_SAMPLE_RATE, CONF_I2S_AUDIO_ID

DEPENDENCIES = ['i2c', 'i2s_audio']
AUTO_LOAD = ['microphone']
CODEOWNERS = ['@your_github_handle']

es7210_ns = cg.esphome_ns.namespace('es7210')
ES7210Component = es7210_ns.class_('ES7210Component', cg.Component, i2c.I2CDevice)
ES7210Microphone = es7210_ns.class_('ES7210Microphone', microphone.Microphone)

def validate_bits_per_sample(value):
    if isinstance(value, str):
        if value.endswith('bit'):
            value = int(value[:-3])
    if value not in [16, 24, 32]:
        raise cv.Invalid("bits_per_sample must be 16, 24 or 32")
    return value

def validate_sample_rate(value):
    if isinstance(value, str):
        if value.endswith('Hz'):
            value = int(value[:-2])
    return cv.int_range(min=8000, max=48000)(value)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES7210Microphone),
    cv.Required(CONF_ID): cv.use_id(ES7210Microphone),
    cv.Required(CONF_I2S_AUDIO_ID): cv.use_id(i2s_audio.I2SAudioComponent),
    cv.Optional(CONF_SAMPLE_RATE, default='16000Hz'): validate_sample_rate,
    cv.Optional(CONF_BITS_PER_SAMPLE, default='16bit'): validate_bits_per_sample,
    cv.Optional('i2c_id'): cv.use_id(i2c.I2CComponent),
    cv.Optional('i2s_din_pin'): cv.int_,
    cv.Optional('adc_type', default='external'): cv.string_strict,
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x40))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    i2s = await cg.get_variable(config[CONF_I2S_AUDIO_ID])
    cg.add(var.set_i2s_audio(i2s))
    
    if 'i2c_id' in config:
        i2c_bus = await cg.get_variable(config['i2c_id'])
        cg.add(var.set_i2c_bus(i2c_bus))
        
    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))
    
    if 'i2s_din_pin' in config:
        cg.add(var.set_i2s_din_pin(config['i2s_din_pin']))
        
    cg.add(var.set_adc_type(config['adc_type']))
