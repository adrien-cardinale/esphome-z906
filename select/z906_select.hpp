#pragma once

#include "esphome/components/select/select.h"
#include "esphome/core/component.h"

namespace esphome {
namespace z906 {

class Z906Component;

enum class SelectChannel {
    INPUT_CHANNEL,
};

class Z906Select : public select::Select,
    public Component {
   public:
    void set_parent(Z906Component *parent) { this->parent_ = parent; }
    void set_channel(SelectChannel channel) { this->channel_ = channel; }

   protected:
    void control(const std::string &value) override;

    Z906Component *parent_{nullptr};
    SelectChannel channel_{SelectChannel::INPUT_CHANNEL};
};

}  // namespace z906
}  // namespace esphome