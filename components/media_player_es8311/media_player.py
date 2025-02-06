import esphome.codegen as cg
from esphome.components import media_player
from esphome.core import CORE, ID
from esphome.components import audio_dac #Change the library
from esphome.components import output

# Custom component namespace
DEPENDENCIES = ["audio_dac", "output"] #Change the dependency
MULTI_CONF = True #Important for using the id()
CONF_AUDIO_DAC_ID = "audio_dac_id" #Change for audiodac
CONF_OUTPUT_ID = "output_id"
CODEOWNERS = ["@YouKorr"]
media_player_es8311_ns = cg.esphome_ns.namespace("media_player_es8311")
MediaPlayerES8311 = media_player_es8311_ns.class_(
    "MediaPlayerES8311", media_player.MediaPlayer, cg.Component
)
SET_AUDIO_DAC_ACTION = media_player_es8311_ns.class_( #Change for audio_dac
    "SetAudioDACAction", cg.ScriptAction #Change for audio_dac
)
SET_SPEAKER_ACTION = media_player_es8311_ns.class_(
    "SetSpeakerAction", cg.ScriptAction
)
SET_SAMPLE_RATE_ACTION = media_player_es8311_ns.class_(
    "SetSampleRateAction", cg.ScriptAction
)
# MediaPlayerES8311 = media_player_es8311_ns.class_("MediaPlayerES8311", media_player.MediaPlayer, cg.Component)
async def new_media_player_action(variable, obj, field, value, const=False):
  if const:
    template_arg = cg.const_(value)
  else:
    template_arg = value
  cg.add(obj.setattr(field, template_arg))

def audio_format(value):
    return cv.enum({
        "codec16": 0,
        "codec24": 1,
        "codec32": 2,
    })(value)

# def i2s_id(value):
#   return cv.use_id(i2s_audio.I2S)(value)
#  if i2s_id == "i2s":
#    return true
#
#  return false

def audio_bit(value):
    return cv.enum({
        "16bit": 0,
        "24bit": 1,
        "32bit": 2,
    })(value)

def es8311_media_player_schema(audio_dac_id,speaker_id): #Change for audio_dac
  return {
    cv.GenerateID(CONF_AUDIO_DAC_ID): cv.use_id(audio_dac.AudioDAC), #Change for audio_dac
    cv.GenerateID(CONF_OUTPUT_ID): cv.use_id(output.BinaryOutput),
    cv.Optional("audio_format",default="codec16"): audio_format,
    cv.Optional("audio_bit",default="16bit"): audio_bit,
  }

@automation.register_action(
    "media_player.media_player_es8311.set_audio_dac", #Change for audio_dac
    SET_AUDIO_DAC_ACTION, #Change for audio_dac
    cv.Schema({
        cv.GenerateID(): cv.use_id(MediaPlayerES8311),
        cv.Required("audio_dac_id"): cv.use_id(audio_dac.AudioDAC), #Change for audio_dac
    }),
    cv.has_at_least_one_key("audio_dac_id"), #Change for audio_dac
)
async def media_player_set_audio_dac_to_code(config, action_id, template_arg, args): #Change for audio_dac
  paren = await cg.get_variable(config[CONF_ID])
  var = cg.new_Pvariable(action_id, template_arg)
  cg.add(var.set_parent(paren))
  dac = await cg.get_variable(config["audio_dac_id"]) #Change for audio_dac
  cg.add(var.set_audio_dac(dac)) #Change for audio_dac
  return var

@automation.register_action(
    "media_player.media_player_es8311.set_speaker",
    SET_SPEAKER_ACTION,
    cv.Schema({
        cv.GenerateID(): cv.use_id(MediaPlayerES8311),
        cv.Required("output_id"): cv.use_id(output.BinaryOutput),
    }),
    cv.has_at_least_one_key("output_id"),
)
async def media_player_set_speaker_to_code(config, action_id, template_arg, args):
  paren = await cg.get_variable(config[CONF_ID])
  var = cg.new_Pvariable(action_id, template_arg)
  cg.add(var.set_parent(paren))
  speaker = await cg.get_variable(config["output_id"])
  cg.add(var.set_output(speaker))
  return var

@automation.register_action(
    "media_player.media_player_es8311.set_sample_rate",
    SET_SAMPLE_RATE_ACTION,
    cv.Schema({
        cv.GenerateID(): cv.use_id(MediaPlayerES8311),
        cv.Required("sample_rate"): cv.positive_int,
    }),
    cv.has_at_least_one_key("sample_rate"),
)
async def media_player_set_sample_rate_to_code(config, action_id, template_arg, args):
  paren = await cg.get_variable(config[CONF_ID])
  var = cg.new_Pvariable(action_id, template_arg)
  cg.add(var.set_parent(paren))
  sr = await cg.get_variable(config["sample_rate"])
  cg.add(var.set_sample_rate(sr))
  return var

@cg.coroutine
def new_es8311_media_player(config):
  storage = yield cg.get_variable(CORE.config_path)
  #cg.add(template_.set_flash(flash))
  var = cg.new_Pvariable(config[CONF_ID], storage)
  yield cg.register_component(var, config)
  yield media_player.register_media_player(var, config)
#  es8311_media_player_schema_values = es8311_media_player_schema(storage)

#  if len(es8311_media_player_schema_values) > 0:
#    for item in es8311_media_player_schema_values:
#      cg.add(storage.set_item)

    #for item in es8311_media_player_schema_values.items():
    #    cg.add(var.set_config(item))
#  if "audio_format" in config:
#    cg.add(var.set_audio_format(config["audio_format"]))
  return var
