#pragma once
#include "esphome/components/uart/uart.h"
#include "deque"

namespace esphome {
namespace z906 {

class Console {
public:
    void update(std::deque<uint8_t> &buffer);
    void writeByte(uint8_t data) { if(uart) uart->write_byte(data); }

    void setUart(uart::UARTComponent *uart) { this->uart = uart; }

protected:
    uart::UARTComponent *uart{nullptr};
};

}
}