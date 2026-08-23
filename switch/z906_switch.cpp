#include "z906_switch.hpp"
#include "../z906.hpp"

namespace esphome {
namespace z906 {

void Z906Switch::write_state(bool state) {
    if (this->parent == nullptr) return;

    this->parent->updateStatus();
}

}
}  // namespace esphome