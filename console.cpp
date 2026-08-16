#include "console.hpp"

namespace esphome {
namespace z906 {

void Console::update(std::deque<uint8_t> &buffer){
    uint8_t data;
    while (uart->available()) {
        if(uart->read_byte(&data)) {
            buffer.push_back(data);
        }
    }
}

}
}