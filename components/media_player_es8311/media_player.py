import esphome.codegen as cg
from esphome.components import media_player
from esphome.core import CORE, ID
from esphome.components import audio_dac #Change the library
from esphome.components import output

# Custom component namespace
DEPENDENCIES = ["audio_dac", "output"] # Change the dependency
MULTI_CONF = True # Important for using the id()
CONF_AUDIO_DAC_ID = "audio_dac_id" # Change for audio_dac
CONF_OUTPUT_ID = "output_id"
CODEOWNERS = ["@YouKorr"]

media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_(
    "MediaPlayerES8311", media_player.MediaPlayer, cg.Component
