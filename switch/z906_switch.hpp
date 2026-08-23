#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace z906 {

class Z906Component;

enum class SwitchChannel {
    STATUS,
};

class Z906Switch : public switch_::Switch, public Component {
   public:
    void set_parent(Z906Component *parent) { this->parent = parent; }
    void set_channel(SwitchChannel channel) { this->channel = channel; }

   protected:
    void write_state(bool state) override;

    Z906Component *parent{nullptr};
    SwitchChannel channel{SwitchChannel::STATUS};
};

}  // namespace z906
}  // namespace esphome