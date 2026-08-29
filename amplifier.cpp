#include "amplifier.hpp"

#include <cmath>

#include "esphome/core/log.h"

namespace esphome {
namespace z906 {

static const char *const TAG = "z906.amplifier";

void Amplifier::update(std::deque<uint8_t> &buffer) {
    uint8_t data;
    while (uart->available()) {
        if (uart->read_byte(&data)) {
            buffer.push_back(data);
            feedByte(data);
        }
    }
}

void Amplifier::feedByte(uint8_t data) {
    switch (parseState) {
        case ParseState::IDLE:
            if (data == static_cast<uint8_t>(SerialCommand::MULTIBYTE_MESSAGE)) {
                parseState = ParseState::TYPE;
            } else {
                ESP_LOGD(TAG, "Received byte: 0x%02X", data);
                onSingleByte(static_cast<SerialCommand>(data));
            }
            break;
        case ParseState::TYPE:
            msgType = data;
            parseState = ParseState::LENGTH;
            break;
        case ParseState::LENGTH:
            msgLength = data;
            msgPayload.clear();
            msgPayload.reserve(msgLength);
            parseState = (msgLength == 0) ? ParseState::CHECKSUM
                                          : ParseState::PAYLOAD;
            break;
        case ParseState::PAYLOAD:
            msgPayload.push_back(data);
            if (msgPayload.size() >= msgLength) parseState = ParseState::CHECKSUM;
            break;
        case ParseState::CHECKSUM:
            onMultiByteMessage(msgType, msgPayload, data);
            parseState = ParseState::IDLE;
            break;
    }
}

void Amplifier::onMultiByteMessage(uint8_t type,
                                   const std::vector<uint8_t> &payload,
                                   uint8_t checksum) {
    std::string hex;
    char tmp[4];
    for (uint8_t b : payload) {
        snprintf(tmp, sizeof(tmp), "%02X ", b);
        hex += tmp;
    }
    ESP_LOGD(TAG, "Multi-byte message: type=0x%02X len=%u checksum=0x%02X [%s]",
             type, static_cast<unsigned>(payload.size()), checksum,
             hex.c_str());

    switch (static_cast<SerialMultiByteType>(type)) {
        case SerialMultiByteType::AMP_STATUS:
            if (!payload.empty() && payload[0] <= MAX_VOLUME) {
                volume = payload[0];
                if (globalVolumeNumber)
                    globalVolumeNumber->publish_state(getVolume());
            }
            break;
        default:
            ESP_LOGW(TAG, "Unknown multi-byte message type: 0x%02X", type);
            break;
    }
}

void Amplifier::onSingleByte(SerialCommand header) {
    switch (header) {
        case SerialCommand::VOLUME_UP:
            if (volume < MAX_VOLUME) volume++;
            if (globalVolumeNumber)
                globalVolumeNumber->publish_state(getVolume());
            break;
        case SerialCommand::VOLUME_DOWN:
            if (volume > 0) volume--;
            if (globalVolumeNumber)
                globalVolumeNumber->publish_state(getVolume());
            break;
        default:
            break;
    }
}

void Amplifier::setVolume(float vol) {
    uint8_t target =
        static_cast<uint8_t>(lroundf(vol / 100.0f * MAX_VOLUME));

    const SerialCommand direction = (target > volume)
                                        ? SerialCommand::VOLUME_UP
                                        : SerialCommand::VOLUME_DOWN;
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
