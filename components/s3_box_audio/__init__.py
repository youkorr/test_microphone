import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2s_audio

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ['i2s_audio']

s3_box_audio_ns = cg.esphome_ns.namespace('s3_box_audio')
S3BoxI2SAudio = s3_box_audio_ns.class_('S3BoxI2SAudio', i2s_audio.I2SAudio)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(S3BoxI2SAudio),
    cv.Optional('sample_rate', default=16000): cv.int_,
    cv.Optional('bits_per_sample', default=16): cv.int_,
})

async def to_code(config):
    var = cg.new_Pvariable(config['id'])
    await i2s_audio.register_i2s_audio(var, config)
    
    if 'sample_rate' in config:
        cg.add(var.set_sample_rate(config['sample_rate']))
    if 'bits_per_sample' in config:
        cg.add(var.set_bits_per_sample(config['bits_per_sample']))
