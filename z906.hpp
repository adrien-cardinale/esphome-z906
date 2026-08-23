#pragma once

#include <cstdarg>
#include <deque>
#include <string>
#include <vector>

#include "amplifier.hpp"
#include "components/binary_sensor/binary_sensor.h"
#include "console.hpp"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"
#include "esphome/core/component.h"
#include "number/z906_number.hpp"
#include "serial.hpp"
#include "switch/z906_switch.hpp"

namespace esphome {
namespace z906 {

class Z906Component : public Component {
   public:
    void setup() override;
    void loop() override;

    // ESPHome Config
    void set_adc_pins(int console_presence_pin, int amplifier_presence_pin,
                      int alim_pin);
    void set_console_uart(uart::UARTComponent *uart) { console.setUart(uart); }
    void set_amp_uart(uart::UARTComponent *uart) { amplifier.setUart(uart); }
    void set_volume_number(Z906Number *number) {
        amplifier.globalVolumeNumber = number;
    }
    void set_status_switch(Z906Switch *status_switch) {
        amplifier.statusSwitch = status_switch;
    }
    void set_power_binary_sensor(binary_sensor::BinarySensor *power_binary_sensor) {
        this->powerBinarySensor = power_binary_sensor;
    }

    void updateStatus() { amplifier.updateStatus(); }

    void setVolume(float percent) { amplifier.setVolume(percent); }

   protected:
    Amplifier amplifier;
    Console console;

    struct AdcChannel {
        adc_unit_t unit{ADC_UNIT_1};
        adc_channel_t channel{ADC_CHANNEL_0};
        adc_oneshot_unit_handle_t unit_handle{nullptr};
        adc_cali_handle_t cali_handle{nullptr};
    };

    bool stable{false};

    void flush_uarts_();

    void updateConsole();
    void updateAmplifier();
    void updatePresence();
    void publishStates();

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
    bool powerStatus{false};

    binary_sensor::BinarySensor *powerBinarySensor{nullptr};
};

}  // namespace z906
}  // namespace esphome
