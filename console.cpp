#include "console.hpp"

#include "esphome/core/log.h"

namespace esphome {
namespace z906 {

void Console::update(std::deque<uint8_t> &buffer) {
    uint8_t data;
    while (uart->available()) {
        if (uart->read_byte(&data)) {
            buffer.push_back(data);
            ESP_LOGD("z906.console", "Received byte: 0x%02X", data);
        }
    }
}

}  // namespace z906
}  // namespace esphome