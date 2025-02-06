#pragma once

#include "esphome/components/microphone/microphone.h"
#include "es7210.h"
#include "driver/i2s.h"

namespace esphome {
namespace es7210 {

class ES7210Microphone : public microphone::Microphone {
 public:
  void set_es7210(ES7210Component *es7210) { this->es7210_ = es7210; }

 protected:
  // Supprimer le `override` pour is_ready_() si la fonction n'est pas virtuelle dans la classe parente
  bool is_ready_() const { return true; }

  // Correction de la signature de `read_chunk_()`, elle doit être correctement déclarée
  int read_chunk_(int16_t *buffer, size_t length) override;

 private:
  ES7210Component *es7210_{nullptr};
  i2s_port_t i2s_port_ = I2S_NUM_0;
};

}  // namespace es7210
}  // namespace esphome

