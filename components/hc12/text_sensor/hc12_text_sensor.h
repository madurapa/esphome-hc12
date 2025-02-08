#pragma once

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "../hc12.h"

namespace esphome {
namespace hc12 {

class HC12TextSensor : public text_sensor::TextSensor, public Component {
 public:
  void setup() override {
    if (this->hc12_component_ != nullptr) {
      this->hc12_component_->set_callback([this](std::string message) {
        this->publish_state(message);
      });
    }
  }

  void set_hc12_component(HC12Component *hc12) { this->hc12_component_ = hc12; }

 protected:
  HC12Component *hc12_component_;
};

}  // namespace hc12
}  // namespace esphome

