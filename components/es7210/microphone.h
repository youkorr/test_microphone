#pragma once

#include "esphome/components/microphone/microphone.h"  // Inclure la classe de base
#include "es7210.h"

namespace esphome {
namespace es7210 {

class ES7210Microphone : public microphone::Microphone {
 public:
  bool is_ready_() const override { return true; }  // Implémentation de la méthode
  int read_chunk_(int16_t *buffer, size_t length) override;  // Déclaration de la méthode

 private:
  ES7210Component *es7210_{nullptr};
};

}  // namespace es7210
}  // namespace esphome







