import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import (
    media_player,
    audio_dac,  # Change the library
    output,
)
from esphome.const import (
    CONF_ID,
    CONF_AUDIO_DAC,  # Change the library
    CONF_OUTPUT,
    CONF_SAMPLE_RATE,
)

# Custom component namespace
DEPENDENCIES = ["audio_dac", "output"]  # Change the dependency
MULTI_CONF = True  # Important for using the id()
CONF_AUDIO_DAC_ID = "audio_dac_id"  # Change for audiodac
CONF_OUTPUT_ID = "output_id"
CODEOWNERS = ["@YouKorr"]

media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_(
    "MediaPlayerES8311", media_player.MediaPlayer, cg.Component
)

def audio_format(value):
    return cv.enum(
        {
            "codec16": 0,
            "codec24": 1,
            "codec32": 2,
        }
    )(value)

def audio_bit(value):
    return cv.enum(
        {
            "16bit": 0,
            "24bit": 1,
            "32bit": 2,
        }
    )(value)

def validate_sample_rate(value):
    return cv.positive_int(value)  # Only allows for positive integer sample rates.


CONFIG_SCHEMA = media_player.MEDIA_PLAYER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MediaPlayerES8311),
        cv.Required(CONF_AUDIO_DAC): cv.use_id(audio_dac.AudioDAC),  # Change for audio_dac
        cv.Required(CONF_OUTPUT): cv.use_id(output.BinaryOutput),
        cv.Optional(CONF_SAMPLE_RATE, default=16000): cv.All(
            cv.Coerce(int), validate_sample_rate
        ),  # Default value of 16000
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], template=MediaPlayerES8311())
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)

    # Get the audio_dac audio component and set it on the media player
    dac = await cg.get_variable(config[CONF_AUDIO_DAC])  # Set the correct Audio Dac settings.
    cg.add(var.set_audio_dac(dac))  # Call setter method to pass in AudioDac object

    # Get the speaker component and set it on the media player
    speaker = await cg.get_variable(config[CONF_OUTPUT])  # Set the correct configuration.
    cg.add(var.set_speaker(speaker))  # Call setter method to pass in speaker object.

    sample_rate = config[CONF_SAMPLE_RATE]
    cg.add(var.set_sample_rate(sample_rate))


