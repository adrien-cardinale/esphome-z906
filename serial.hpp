#pragma once

namespace esphome {
namespace z906 {

enum class SerialCommand : uint8_t {
    INPUT_1 = 0x02,
    INPUT_2 = 0x05,
    INPUT_3 = 0x03,
    INPUT_4 = 0x04,
    INPUT_5 = 0x06,
    INPUT_6 = 0x07,
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