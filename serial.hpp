#pragma once

namespace esphome {
namespace z906 {

enum class SerialCommand : uint8_t {
    VOLUME_UP = 0x08,
    VOLUME_DOWN = 0x09,
    GET_STATUS = 0x34,
    MULTIBYTE_MESSAGE = 0xAA,
};

enum class SerialMultiByteType : uint8_t {
    AMP_STATUS = 0x0A,
};

}
}  // namespace esphome