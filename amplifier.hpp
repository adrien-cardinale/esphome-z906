#pragma once
#include "deque"
#include "esphome/components/uart/uart.h"
#include "number/z906_number.hpp"
#include "serial.hpp"

namespace esphome {
namespace z906 {

class Amplifier {
   public:
    void update(std::deque<uint8_t> &buffer);
    void onMessage(SerialHeader header);
    void writeByte(uint8_t data) {
        if (uart) uart->write_byte(data);
    }
    void setUart(uart::UARTComponent *uart) { this->uart = uart; }

    void setVolume(float volume);
    float getVolume() const {
        return static_cast<float>(volume) / static_cast<float>(MAX_VOLUME) *
               100.0f;
    }

    friend class Z906Component;

   protected:
    uart::UARTComponent *uart{nullptr};

    static constexpr uint8_t MAX_VOLUME = 43;

    uint8_t volume{9};

    // ESPHome Component
    Z906Number *globalVolumeNumber{nullptr};
};

}  // namespace z906
}  // namespace esphome