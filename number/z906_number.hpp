#pragma once

#include "esphome/components/number/number.h"
#include "esphome/core/component.h"

namespace esphome {
namespace z906 {

class Z906Component;

enum class NumberChannel {
    VOLUME,
};

class Z906Number : public number::Number, public Component {
   public:
    void set_parent(Z906Component *parent) { this->parent = parent; }
    void set_channel(NumberChannel channel) { this->channel = channel; }

   protected:
    void control(float value) override;

    Z906Component *parent{nullptr};
    NumberChannel channel{NumberChannel::VOLUME};
};

}  // namespace z906
}  // namespace esphome
