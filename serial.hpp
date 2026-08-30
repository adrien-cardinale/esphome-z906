#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

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
    
    EFFECT_3D = 0x14,
    EFFECT_4_1 = 0x15,
    EFFECT_2_1 = 0x16,
    EFFECT_DISABLED = 0X35,
    EFFECT_NO = 0X3f,

    GET_STATUS = 0x34,
    MULTIBYTE_MESSAGE = 0xAA,
};

enum class SerialMultiByteType : uint8_t {
    AMP_STATUS = 0x0A,
};

struct Effect {
    std::string_view effect;
    SerialCommand command;
};

inline constexpr std::array<Effect, 4> effects = {{
    {"3D", SerialCommand::EFFECT_3D},
    {"2.1", SerialCommand::EFFECT_2_1},
    {"4.1", SerialCommand::EFFECT_4_1},
    {"None", SerialCommand::EFFECT_DISABLED},
}};

constexpr std::optional<SerialCommand> getEffectCommand(std::string_view s) {
    auto it = std::ranges::find(effects, s, &Effect::effect);
    return it != effects.end() ? std::optional{it->command} : std::nullopt;
}

}
}  // namespace esphome