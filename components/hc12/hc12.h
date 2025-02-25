#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include <functional>

namespace esphome
{
  namespace hc12
  {

    class HC12Component : public Component, public uart::UARTDevice
    {
    public:
      explicit HC12Component(uart::UARTComponent *parent) : uart::UARTDevice(parent) {}

      void setup() override;
      void loop() override;
      void dump_config() override;

      void send_message(const std::string &message);
      bool is_available();
      void set_callback(std::function<void(const std::string &)> callback) { callback_ = std::move(callback); }
      void set_max_buffer_size(size_t size) { max_buffer_size_ = size; }
      void set_message_terminator(const std::string &terminator) { terminator_ = terminator; }

      float get_setup_priority() const override { return setup_priority::LATE; }

    private:
      std::string buffer_;
      bool hc12_online_ = false;
      std::function<void(const std::string &)> callback_{nullptr};
      size_t max_buffer_size_{64};
      std::string terminator_{"\r\n"};

      void process_buffer(std::string message);
    };

  } // namespace hc12
} // namespace esphome