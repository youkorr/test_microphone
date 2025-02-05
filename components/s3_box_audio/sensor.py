import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2s_audio

DEPENDENCIES = ['i2s_audio']

s3_box_audio_ns = cg.esphome_ns.namespace('s3_box_audio')
S3BoxI2SAudio = s3_box_audio_ns.class_('S3BoxI2SAudio', i2s_audio.I2SAudio)

async def to_code(config):
    var = cg.new_Pvariable(config['id'])
    await i2s_audio.register_i2s_audio(var, config)
