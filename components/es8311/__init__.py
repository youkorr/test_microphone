import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]
AUTO_LOAD = []

es8311_ns = cg.esphome_ns.namespace("es8311")
audio_dac_ns = cg.esphome_ns.namespace("audio_dac")

# Définissez AudioDac dans le namespace audio_dac
AudioDac = audio_dac_ns.class_("AudioDac")

# Créez ES8311Component en héritant de AudioDac, I2CDevice et Component
ES8311 = es8311_ns.class_("ES8311Component", AudioDac, i2c.I2CDevice, cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES8311),
}).extend(i2c.i2c_device_schema(0x18)).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

