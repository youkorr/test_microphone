#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2s_audio/i2s_audio.h"

namespace esphome {
namespace s3_box_audio {

class S3BoxI2SAudio : public i2s_audio::I2SAudio {
 public:
  void setup() override;
  void dump_config() override;
};

}  // namespace s3_box_audio
}  // namespace esphome
