#include "z906.hpp"

#include <algorithm>
#include <cmath>
#include <deque>

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace z906 {

static const char *const TAG = "z906";

static const uint32_t PRESENCE_UPDATE_INTERVAL_MS = 1000;

void Z906Component::set_adc_pins(int console_presence_pin,
                                 int amplifier_presence_pin, int alim_pin) {
    this->console_presence_pin_ = console_presence_pin;
    this->amplifier_presence_pin_ = amplifier_presence_pin;
    this->alim_pin_ = alim_pin;
}

adc_oneshot_unit_handle_t Z906Component::get_adc_unit_handle_(adc_unit_t unit) {
    adc_oneshot_unit_handle_t &handle =
        (unit == ADC_UNIT_1) ? this->adc1_handle_ : this->adc2_handle_;
    if (handle == nullptr) {
        adc_oneshot_unit_init_cfg_t init_cfg = {};
        init_cfg.unit_id = unit;
        if (adc_oneshot_new_unit(&init_cfg, &handle) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize ADC unit %d", unit);
            return nullptr;
        }
    }
    return handle;
}

void Z906Component::init_adc_channel_(AdcChannel &adc, int gpio_pin) {
    if (gpio_pin < 0) return;

    if (adc_oneshot_io_to_channel(gpio_pin, &adc.unit, &adc.channel) !=
        ESP_OK) {
        ESP_LOGE(TAG, "GPIO%d is not a valid ADC pin", gpio_pin);
        return;
    }

    adc.unit_handle = this->get_adc_unit_handle_(adc.unit);
    if (adc.unit_handle == nullptr) return;

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_oneshot_config_channel(adc.unit_handle, adc.channel, &chan_cfg) !=
        ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC channel for GPIO%d", gpio_pin);
        return;
    }

    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = adc.unit,
        .chan = adc.channel,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_cfg, &adc.cali_handle) !=
        ESP_OK) {
        ESP_LOGW(TAG,
                 "ADC calibration unavailable for GPIO%d, falling back to raw "
                 "scaling",
                 gpio_pin);
        adc.cali_handle = nullptr;
    }
}

float Z906Component::read_adc_voltage_(AdcChannel &adc) {
    if (adc.unit_handle == nullptr) return NAN;

    int raw;
    if (adc_oneshot_read(adc.unit_handle, adc.channel, &raw) != ESP_OK)
        return NAN;

    if (adc.cali_handle != nullptr) {
        int millivolts;
        if (adc_cali_raw_to_voltage(adc.cali_handle, raw, &millivolts) ==
            ESP_OK)
            return millivolts / 1000.0f;
    }

    return raw / 4095.0f * 3.3f;
}

void Z906Component::setup() {
    this->init_adc_channel_(this->console_presence_adc_,
                            this->console_presence_pin_);
    this->init_adc_channel_(this->amplifier_presence_adc_,
                            this->amplifier_presence_pin_);
    this->init_adc_channel_(this->alim_adc_, this->alim_pin_);
    powerBinarySensor->publish_state(false);
}

void Z906Component::loop() {
    updatePresence();
    if (stable) {
        if (!powerStatus) {
            powerStatus = true;
            ESP_LOGI(TAG, "Z906 system is stable and powered on");
            powerBinarySensor->publish_state(true);
        }
        std::deque<uint8_t> console_to_amp_buffer, amp_to_console_buffer;
        console.update(console_to_amp_buffer);
        amplifier.update(amp_to_console_buffer);
        for (uint8_t b : console_to_amp_buffer) amplifier.writeByte(b);
        for (uint8_t b : amp_to_console_buffer) console.writeByte(b);
    } else {
        if (powerStatus) {
            powerStatus = false;
            ESP_LOGW(TAG, "Z906 system is unstable or powered off");
            powerBinarySensor->publish_state(false);
        }
    }
}

void Z906Component::updatePresence() {
    const uint32_t now = millis();
    if (now - this->last_presence_update_ < PRESENCE_UPDATE_INTERVAL_MS) return;
    this->last_presence_update_ = now;

    const float voltage_console_presence =
        this->read_adc_voltage_(this->console_presence_adc_);
    const float voltage_amplifier_presence =
        this->read_adc_voltage_(this->amplifier_presence_adc_);
    const float voltage_alim = this->read_adc_voltage_(this->alim_adc_);

    if (voltage_console_presence < 0.1f && voltage_amplifier_presence < 0.1f &&
        voltage_alim > 3.0f) {
        this->stable = true;
    } else {
        this->stable = false;
    }
}

}  // namespace z906
}  // namespace esphome
