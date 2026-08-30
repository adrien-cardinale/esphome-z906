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
            if (data ==
                static_cast<uint8_t>(SerialCommand::MULTIBYTE_MESSAGE)) {
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
            parseState =
                (msgLength == 0) ? ParseState::CHECKSUM : ParseState::PAYLOAD;
            break;
        case ParseState::PAYLOAD:
            msgPayload.push_back(data);
            if (msgPayload.size() >= msgLength)
                parseState = ParseState::CHECKSUM;
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
            ESP_LOGD(TAG, "Received amplifier status message");
            ESP_LOGD(TAG, "Payload: %s", hex.c_str());
            if (!payload.empty() && payload[0] <= MAX_VOLUME) {
                volume = payload[0];
                inputSelect->publish_state("Input " + std::to_string(payload[4] + 1));
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
        case SerialCommand::INPUT_1:
            if (inputSelect) inputSelect->publish_state("Input 1");
            break;
        case SerialCommand::INPUT_2:
            if (inputSelect) inputSelect->publish_state("Input 2");
            break;
        case SerialCommand::INPUT_3:
            if (inputSelect) inputSelect->publish_state("Input 3");
            break;
        case SerialCommand::INPUT_4:
            if (inputSelect) inputSelect->publish_state("Input 4");
            break;
        case SerialCommand::INPUT_5:
            if (inputSelect) inputSelect->publish_state("Input 5");
            break;
        case SerialCommand::INPUT_6:
            if (inputSelect) inputSelect->publish_state("Input 6");
            break;
        default:
            break;
    }
}

void Amplifier::setVolume(float vol) {
    uint8_t target = static_cast<uint8_t>(lroundf(vol / 100.0f * MAX_VOLUME));

    const SerialCommand direction = (target > volume)
                                        ? SerialCommand::VOLUME_UP
                                        : SerialCommand::VOLUME_DOWN;
    const uint8_t delta =
        (target > volume) ? (target - volume) : (volume - target);
    for (uint8_t i = 0; i < delta; i++) {
        writeByte(static_cast<uint8_t>(direction));
    }
}

void Amplifier::controlInput(const std::string &input) {
    ESP_LOGD(TAG, "Controlling input: %s (volume %u)", input.c_str(), volume);

    const uint8_t savedVolume = volume;
    for (uint8_t i = 0; i < savedVolume; i++)
        writeByte(static_cast<uint8_t>(SerialCommand::VOLUME_DOWN));

    static const struct {
        const char *name;
        SerialCommand cmd;
    } INPUTS[] = {
        {"Input 1", SerialCommand::INPUT_1}, {"Input 2", SerialCommand::INPUT_2},
        {"Input 3", SerialCommand::INPUT_3}, {"Input 4", SerialCommand::INPUT_4},
        {"Input 5", SerialCommand::INPUT_5}, {"Input 6", SerialCommand::INPUT_6},
    };

    bool found = false;
    for (const auto &entry : INPUTS) {
        if (input == entry.name) {
            writeByte(static_cast<uint8_t>(entry.cmd));
            found = true;
            break;
        }
    }
    if (!found) {
        ESP_LOGW(TAG, "Unknown input: %s", input.c_str());
    }
    for (uint8_t i = 0; i < savedVolume; i++)
        writeByte(static_cast<uint8_t>(SerialCommand::VOLUME_UP));
}

void Amplifier::controlEffect(const std::string &effect){
    ESP_LOGD(TAG, "Controlling effect: %s ", effect.c_str());

    if (auto cmd = getEffectCommand(effect)) {
        writeByte(static_cast<uint8_t>(cmd.value()));
    } else {
        ESP_LOGW(TAG, "Unknown effect: %s", effect.c_str());
    }
}

void Amplifier::updateStatus() {
    writeByte(static_cast<uint8_t>(SerialCommand::GET_STATUS));
}

}  // namespace z906
}  // namespace esphome
