#include "amplifier.hpp"

#include "esphome/core/log.h"

namespace esphome {
namespace z906 {

void Amplifier::update(std::deque<uint8_t> &buffer) {
    uint8_t data;
    while (uart->available()) {
        if (uart->read_byte(&data)) {
            buffer.push_back(data);
            onMessage(static_cast<SerialCommand>(data));
            ESP_LOGD("z906.amplifier", "Received byte: 0x%02X", data);
            if (data == static_cast<uint8_t>(SerialCommand::MULTIBYTE_MESSAGE)) {
                readMultiByteMessage(buffer);
            }
        }
    }
}

void Amplifier::readMultiByteMessage(std::deque<uint8_t> &buffer) {
    uint8_t type, length;
    if (!uart->read_byte(&type) || !uart->read_byte(&length)) {
        ESP_LOGE("z906.amplifier", "Failed to read multi-byte message header");
        return;
    }
    buffer.push_back(type);
    buffer.push_back(length);

    std::vector<uint8_t> data_bytes;

    for (uint8_t i = 0; i < length; ++i) {
        uint8_t data;
        if (!uart->read_byte(&data)) {
            ESP_LOGE("z906.amplifier", "Failed to read multi-byte message data");
            return;
        }
        buffer.push_back(data);
        data_bytes.push_back(data);
    }
    ESP_LOGD("z906.amplifier", "Received multi-byte message: type=0x%02X, length=%d", type, length);

    switch (static_cast<SerialMultiByteType>(type)) {
        case SerialMultiByteType::AMP_STATUS:
            ESP_LOGD("z906.amplifier", "AMP_STATUS data: %s",
                     std::string(data_bytes.begin(), data_bytes.end()).c_str());
            break;
        default:
            ESP_LOGW("z906.amplifier", "Unknown multi-byte message type: 0x%02X", type);
            break;
    }
}

void Amplifier::onMessage(SerialCommand header) {
    switch (header) {
        case SerialCommand::VOLUME_UP:
            if (volume < MAX_VOLUME) {
                volume++;
            }
            globalVolumeNumber->publish_state(getVolume());
            break;
        case SerialCommand::VOLUME_DOWN:
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

    const SerialCommand direction =
        (target > volume) ? SerialCommand::VOLUME_UP : SerialCommand::VOLUME_DOWN;
    const uint8_t delta =
        (target > volume) ? (target - volume) : (volume - target);
    for (uint8_t i = 0; i < delta; i++) {
        writeByte(static_cast<uint8_t>(direction));
    }
}

void Amplifier::updateStatus() {
    writeByte(static_cast<uint8_t>(SerialCommand::GET_STATUS));
}

}  // namespace z906
}  // namespace esphome