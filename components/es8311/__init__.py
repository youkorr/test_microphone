# __init__.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ['i2c']
AUTO_LOAD = ['media_player']

es8311_ns = cg.esphome_ns.namespace('es8311')
ES8311Component = es8311_ns.class_('ES8311Component', cg.Component, i2c.I2CDevice)

CONF_SAMPLE_RATE = "sample_rate"
CONF_BITS_PER_SAMPLE = "bits_per_sample"
CONF_ENABLE_PIN = "enable_pin"
CONF_MIC_GAIN = "mic_gain"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ES8311Component),
    cv.Required(CONF_SAMPLE_RATE): cv.int_range(min=8000, max=96000),
    cv.Required(CONF_BITS_PER_SAMPLE): cv.one_of(16, 24, 32, int=True),
    cv.Required(CONF_ENABLE_PIN): cv.int_range(min=0),
    cv.Optional(CONF_MIC_GAIN, default=42): cv.int_range(min=0, max=100),
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x18))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))
    cg.add(var.set_enable_pin(config[CONF_ENABLE_PIN]))
    cg.add(var.set_mic_gain(config[CONF_MIC_GAIN]))

# media_player.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import media_player
from esphome.const import CONF_ID

from . import es8311_ns, ES8311Component

DEPENDENCIES = ['es8311']
AUTO_LOAD = ['es8311']

ES8311MediaPlayer = es8311_ns.class_('ES8311MediaPlayer', media_player.MediaPlayer, cg.Component)

CONF_ES8311_ID = 'es8311_id'

CONFIG_SCHEMA = media_player.MEDIA_PLAYER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ES8311MediaPlayer),
    cv.Required(CONF_ES8311_ID): cv.use_id(ES8311Component),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await media_player.register_media_player(var, config)
    
    parent = await cg.get_variable(config[CONF_ES8311_ID])
    cg.add(var.set_parent(parent))

# ES8311.h
"""
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/media_player/media_player.h"

namespace esphome {
namespace es8311 {

class ES8311Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  
  void set_sample_rate(uint32_t sample_rate) { sample_rate_ = sample_rate; }
  void set_bits_per_sample(uint8_t bits) { bits_per_sample_ = bits; }
  void set_enable_pin(uint8_t pin) { enable_pin_ = pin; }
  void set_mic_gain(uint8_t gain) { mic_gain_ = gain; }
  
  void set_volume(float volume);
  float get_volume() const { return volume_; }
  void set_mute(bool mute);
  
 protected:
  uint32_t sample_rate_{16000};
  uint8_t bits_per_sample_{16};
  uint8_t enable_pin_{0};
  uint8_t mic_gain_{42};
  float volume_{0.5};
  bool is_muted_{false};
};

class ES8311MediaPlayer : public media_player::MediaPlayer, public Component {
 public:
  void set_parent(ES8311Component *parent) { parent_ = parent; }
  void setup() override;
  media_player::MediaPlayerTraits get_traits() override;
  void control(const media_player::MediaPlayerCall &call) override;
  
 protected:
  ES8311Component *parent_;
};

}  // namespace es8311
}  // namespace esphome
"""


