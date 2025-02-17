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
      void set_callback(std::function<void(const std::string &)> callback)
      {
        callback_ = std::move(callback);
      }

      float get_setup_priority() const override
      {
        return setup_priority::LATE;
      }

    protected:
      bool is_valid_char(char c) const;
      void process_buffer();
      std::string sanitize_message(const std::string &message) const;

    private:
      std::string buffer_;
      std::function<void(const std::string &)> callback_{nullptr};
      static constexpr char MESSAGE_TERMINATOR = '\n';
      static constexpr size_t MAX_BUFFER_SIZE = 256;
    };

  } // namespace hc12
} // namespace esphome
