import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2s_audio, i2c
from esphome.const import CONF_ID, CONF_ADDRESS

CODEOWNERS = ["@youkorr"]
DEPENDENCIES = ["i2c"]

es8311_ns = cg.esphome_ns.namespace("es8311")
ES8311AudioDAC = es8311_ns.class_("ES8311AudioDAC", i2c.I2CDevice)  # Suppression de i2s_audio.I2SAudioDAC

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ES8311AudioDAC),
            cv.Required(CONF_ADDRESS): cv.i2c_address,
        }
    )
    .extend(i2c.i2c_device_schema(0x18))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_address(config[CONF_ADDRESS]))


