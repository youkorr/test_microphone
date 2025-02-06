#ifndef MICROPHONE_H
#define MICROPHONE_H

namespace esphome {
namespace es7210 {

class ES7210Microphone : public esphome::sensor::Sensor {
public:
  int read_chunk_(int16_t *buffer, size_t length) override;  // Déclaration de la méthode
};

}  // namespace es7210
}  // namespace esphome

#endif  // MICROPHONE_H

