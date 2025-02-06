#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "esphome/components/microphone/microphone.h"
#include "esphome/core/component.h"

namespace esphome {
namespace es7210 {

class ES7210Component;  // Forward declaration

class ES7210Microphone : public microphone::Microphone {
public:
  void set_es7210(ES7210Component* parent) { parent_ = parent; }
  
protected:
  int read_chunk_(int16_t *buffer, size_t length) override;

private:
  ES7210Component* parent_ = nullptr;
};

}  // namespace es7210
}  // namespace esphome

#endif  // MICROPHONE_H

