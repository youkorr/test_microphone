import esphome.codegen as cg
from esphome.components import media_player

# Custom component namespace
media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_(
    "MediaPlayerES8311", media_player.MediaPlayer, cg.Component
)
