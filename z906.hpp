#pragma once

#include <cstdarg>
#include <deque>
#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "number/z906_number.hpp"

namespace esphome {
namespace z906 {

enum class SerialHeader : uint8_t {
  SELECT_INPUT_1 = 0x02,
  SELECT_INPUT_2 = 0x05,
  SELECT_INPUT_3 = 0x03,
  SELECT_INPUT_4 = 0x04,
  SELECT_INPUT_5 = 0x06,
  SELECT_INPUT_6 = 0x07,
  VOLUME_UP = 0x08,
  VOLUME_DOWN = 0x09,
  VOLUME_BASS_UP = 0x0A,
  VOLUME_BASS_DOWN = 0x0B,
  VOLUME_CENTER_UP = 0x0C,
  VOLUME_CENTER_DOWN = 0x0D,
  VOLUME_REAR_UP = 0x0E,
  VOLUME_REAR_DOWN = 0x0F,
  POWER_OFF = 0x10,
  POWER_ON = 0x11,
  SELECT_EFFECT_3D = 0x14,
  SELECT_EFFECT_4_1 = 0x15,
  SELECT_EFFECT_2_1 = 0x16,
  AMP_MUTED = 0x20,
  AMP_UNMUTED = 0x21,
  START_SPEAKER_TEST = 0x22,
  READ_AMP_TEMPERATURE = 0x25,
  READ_INPUT_VOLUME = 0x2F,
  RESET_IDLE_TIME = 0x30,
  READ_IDLE_TIME = 0x31,
  STOP_SPEAKER_TEST = 0x33,
  READ_STATUS = 0x34,
  SELECT_EFFECT_NONE = 0x35,
  SAVE = 0x36,
  UNKNOWN = 0x37,
  MUTE_ON = 0x38,
  MUTE_OFF = 0x39,
  HEADPHONES = 0x3F,
  MULTIBYTE_REPLY = 0xAA,
};

class Z906Component : public Component {
    public:
        void setup() override;
        void loop() override;

        void set_adc_pins(int console_presence_pin, int amplifier_presence_pin, int alim_pin);

        void set_console_uart(uart::UARTComponent *console_uart) { this->uartConsole = console_uart; }
        void set_amp_uart(uart::UARTComponent *amp_uart) { this->uartAmplifier = amp_uart; }

        void set_volume_number(Z906Number *number) { this->volume_number_ = number; }

        void controlVolume(float percent);
    protected:
        static constexpr uint8_t MAX_VOLUME = 43;

        struct AdcChannel {
            adc_unit_t unit{ADC_UNIT_1};
            adc_channel_t channel{ADC_CHANNEL_0};
            adc_oneshot_unit_handle_t unit_handle{nullptr};
            adc_cali_handle_t cali_handle{nullptr};
        };

        bool stable{false};

        uart::UARTComponent *uartConsole{nullptr};
        uart::UARTComponent *uartAmplifier{nullptr};

        void flush_uarts_();

        void updateConsole();
        void updateAmplifier();
        void updatePresence();
        void publishStates();

        adc_oneshot_unit_handle_t get_adc_unit_handle_(adc_unit_t unit);
        void init_adc_channel_(AdcChannel &adc, int gpio_pin);
        float read_adc_voltage_(AdcChannel &adc);
        void onAmplifierMessage(SerialHeader header);

        int console_presence_pin_{-1};
        int amplifier_presence_pin_{-1};
        int alim_pin_{-1};

        AdcChannel console_presence_adc_{};
        AdcChannel amplifier_presence_adc_{};
        AdcChannel alim_adc_{};

        adc_oneshot_unit_handle_t adc1_handle_{nullptr};
        adc_oneshot_unit_handle_t adc2_handle_{nullptr};

        uint32_t last_presence_update_{0};

        Z906Number *volume_number_{nullptr};

        uint8_t volume_{9};
};

}
}
