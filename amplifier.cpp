#include "amplifier.hpp"

#include "esphome/core/log.h"

namespace esphome {
namespace z906 {

void Amplifier::update(std::deque<uint8_t> &buffer) {
    uint8_t data;
    while (uart->available()) {
        if (uart->read_byte(&data)) {
            buffer.push_back(data);
            onMessage(static_cast<SerialHeader>(data));
            ESP_LOGD("z906.amplifier", "Received byte: 0x%02X", data);
        }
    }
}

void Amplifier::onMessage(SerialHeader header) {
    switch (header) {
        case SerialHeader::VOLUME_UP:
            if (volume < MAX_VOLUME) {
                volume++;
            }
            globalVolumeNumber->publish_state(getVolume());
            break;
        case SerialHeader::VOLUME_DOWN:
            if (volume > 0) {
                volume--;
            }
            globalVolumeNumber->publish_state(getVolume());
            break;
    }
}

void Amplifier::setVolume(float volume) {
    uint8_t target =
        static_cast<uint8_t>(lroundf(volume / 100.0f * MAX_VOLUME));

    const SerialHeader step =
        (target > volume) ? SerialHeader::VOLUME_UP : SerialHeader::VOLUME_DOWN;
    const uint8_t delta =
        (target > volume) ? (target - volume) : (volume - target);
    for (uint8_t i = 0; i < delta; i++) {
        writeByte(static_cast<uint8_t>(SerialHeader::RESET_IDLE_TIME));
        writeByte(static_cast<uint8_t>(step));
        writeByte(static_cast<uint8_t>(SerialHeader::RESET_IDLE_TIME));
    }
}

}  // namespace z906
}  // namespace esphome