#include "z906_number.hpp"
#include "../z906.hpp"

namespace esphome {
namespace z906 {

void Z906Number::control(float value) {
  if (this->parent_ == nullptr)
    return;

  switch (this->channel_) {
    case NumberChannel::VOLUME:
      this->parent_->controlVolume(value);
      break;
  }
}

}
}