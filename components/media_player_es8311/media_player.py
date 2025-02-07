import esphome.codegen as cg
from esphome.components import media_player
from esphome.core import CORE, ID
from esphome.components import audio_dac  # Change the library
from esphome.components import output

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
# Implemented the default properties within h file.
MediaPlayerES8311.set_audio_dac = cg.MockComponentMethod(
    [audio_dac.AudioDAC], template_arg=None
)
MediaPlayerES8311.set_speaker = cg.MockComponentMethod(
    [output.BinaryOutput], template_arg=None
)
MediaPlayerES8311.set_sample_rate = cg.MockComponentMethod(
    [cg.uint32], template_arg=None
)

@cg.coroutine
def new_es8311_media_player(config):
    var = cg.new_Pvariable(config[CONF_ID], template=MediaPlayerES8311())
    yield cg.register_component(var, config)
    yield media_player.register_media_player(var, config)

    # Get the audio_dac audio component and set it on the media player
    dac = yield cg.get_variable(config[CONF_AUDIO_DAC])  # Set the correct Audio Dac settings.
    cg.add(var.set_audio_dac(dac))  # Call setter method to pass in AudioDac object

    # Get the speaker component and set it on the media player
    speaker = yield cg.get_variable(config[CONF_OUTPUT])  # Ensure the correct configuration.
    cg.add(var.set_speaker(speaker))  # Call setter method to pass in speaker object.

    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    #Implement the sample_rate within media_player_es8311.h

    return var
