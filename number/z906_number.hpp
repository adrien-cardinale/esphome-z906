#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace z906 {

class Z906Component;

enum class NumberChannel {
  VOLUME,
};

class Z906Number : public number::Number, public Component {
 public:
  void set_parent(Z906Component *parent) { this->parent_ = parent; }
  void set_channel(NumberChannel channel) { this->channel_ = channel; }

 protected:
  void control(float value) override;

  Z906Component *parent_{nullptr};
  NumberChannel channel_{NumberChannel::VOLUME};
};

}
}
