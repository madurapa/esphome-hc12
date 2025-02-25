#include "hc12.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace hc12
  {

    static const char *TAG = "hc12";

    void HC12Component::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up HC-12 component...");
      buffer_.reserve(max_buffer_size_);
    }

    void HC12Component::dump_config()
    {
      ESP_LOGCONFIG(TAG, "HC-12:");
      ESP_LOGCONFIG(TAG, "  Max Buffer Size: %u", max_buffer_size_);
      ESP_LOGCONFIG(TAG, "  Message Terminator: '%s'", terminator_.c_str());
    }

    void HC12Component::loop()
    {
      bool received_data = false;
      while (available() > 0)
      {
        char c = read();
        received_data = true;
        buffer_ += c;

        // Overflow protection
        if (buffer_.length() >= max_buffer_size_)
        {
          ESP_LOGW(TAG, "Buffer overflow, clearing (length: %u)", buffer_.length());
          buffer_.clear();
          continue;
        }

        // Check for configurable terminator
        if (buffer_.length() >= terminator_.length())
        {
          size_t pos = buffer_.find(terminator_);
          if (pos != std::string::npos)
          {
            std::string message = buffer_.substr(0, pos);
            buffer_.erase(0, pos + terminator_.length());
            process_buffer(message);
          }
        }
      }
      if (received_data)
      {
        hc12_online_ = true;
      }
      else
      {
        static uint32_t last_check = millis();
        if (millis() - last_check > 10000)
        {
          hc12_online_ = false;
          last_check = millis();
        }
      }
    }

    void HC12Component::process_buffer(std::string message)
    {
      if (!message.empty())
      {
        ESP_LOGD(TAG, "Received: %s", message.c_str());
        if (callback_)
          callback_(message);
      }
    }

    void HC12Component::send_message(const std::string &message)
    {
      // Ensure message is terminated
      std::string to_send = message + terminator_;

      write_array(reinterpret_cast<const uint8_t *>(to_send.c_str()), to_send.length());

      ESP_LOGD(TAG, "Sent: %s", to_send.c_str());
    }

    bool HC12Component::is_available()
    {
      ESP_LOGD(TAG, "HC-12 is %s", hc12_online_ ? "Online" : "Offline");
      return hc12_online_;
    }

  } // namespace hc12
} // namespace esphome