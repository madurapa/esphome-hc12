#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include <memory>

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
      void set_callback(std::function<void(const std::string &)> callback)
      {
        callback_ = std::move(callback);
      }

      float get_setup_priority() const override
      {
        return setup_priority::LATE;
      }

      void set_max_buffer_size(size_t size) { max_buffer_size_ = size; }
      void set_message_terminator(const char *terminator) { message_terminator_ = terminator[0]; }

    protected:
      bool is_valid_char(char c) const;
      void process_buffer();
      std::string sanitize_message(const std::string &message) const;

    private:
      std::string buffer_;
      bool hc12_online_ = false;
      std::function<void(const std::string &)> callback_{nullptr};
      char message_terminator_ = '\n';
      size_t max_buffer_size_ = 256;
    };

  } // namespace hc12
} // namespace esphome
