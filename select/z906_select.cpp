#include "z906_select.hpp"

#include "../z906.hpp"

namespace esphome {
namespace z906 {

void Z906Select::control(const std::string &value) {
    if (this->parent_ == nullptr) return;

    switch (this->channel_) {
        case SelectChannel::INPUT_CHANNEL:
            this->parent_->controlInput(value);
            break;
        case SelectChannel::EFFECT_CHANNEL:
            this->parent_->controlEffect(value);
            break;
    }
}

}  // namespace z906
}  // namespace esphome