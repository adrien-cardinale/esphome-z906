#pragma once
#include "deque"
#include "esphome/components/uart/uart.h"
#include "number/z906_number.hpp"
#include "select/z906_select.hpp"
#include "serial.hpp"
#include "switch/z906_switch.hpp"
#include "vector"

namespace esphome {
namespace z906 {

class Amplifier {
   public:
    void setUart(uart::UARTComponent *uart) { this->uart = uart; }
    void update(std::deque<uint8_t> &buffer);
    void writeByte(uint8_t data) {
        if (uart) uart->write_byte(data);
    }

    void setVolume(float vol);
    float getVolume() const {
        return static_cast<float>(volume) / static_cast<float>(MAX_VOLUME) *
               100.0f;
    }
    void controlInput(const std::string &input);
    void updateStatus();

    friend class Z906Component;

   protected:
    enum class ParseState : uint8_t { IDLE, TYPE, LENGTH, PAYLOAD, CHECKSUM };

    void feedByte(uint8_t data);
    void onSingleByte(SerialCommand header);
    void onMultiByteMessage(uint8_t type, const std::vector<uint8_t> &payload,
                            uint8_t checksum);

    uart::UARTComponent *uart{nullptr};

    static constexpr uint8_t MAX_VOLUME = 43;

    uint8_t volume{9};

    ParseState parseState{ParseState::IDLE};
    uint8_t msgType{0};
    uint8_t msgLength{0};
    std::vector<uint8_t> msgPayload;

    // ESPHome Component
    Z906Number *globalVolumeNumber{nullptr};
    Z906Switch *statusSwitch{nullptr};
    Z906Select *inputSelect{nullptr};
};

}  // namespace z906
}  // namespace esphome
