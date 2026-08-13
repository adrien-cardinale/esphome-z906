#pragma once

#include <cstdarg>
#include <deque>
#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

namespace esphome {
namespace z906 {

class Z906Component : public Component {
    public:
        void setup() override;
        void loop() override;

        void set_adc_pins(int console_presence_pin, int amplifier_presence_pin, int alim_pin);

        void set_console_uart(uart::UARTComponent *console_uart) { this->uartConsole = console_uart; }
        void set_amp_uart(uart::UARTComponent *amp_uart) { this->uartAmplifier = amp_uart; }

    protected:

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

        adc_oneshot_unit_handle_t get_adc_unit_handle_(adc_unit_t unit);
        void init_adc_channel_(AdcChannel &adc, int gpio_pin);
        float read_adc_voltage_(AdcChannel &adc);

        int console_presence_pin_{-1};
        int amplifier_presence_pin_{-1};
        int alim_pin_{-1};

        AdcChannel console_presence_adc_{};
        AdcChannel amplifier_presence_adc_{};
        AdcChannel alim_adc_{};

        adc_oneshot_unit_handle_t adc1_handle_{nullptr};
        adc_oneshot_unit_handle_t adc2_handle_{nullptr};

        uint32_t last_presence_update_{0};
};

}
}
