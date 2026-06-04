#include "hc12.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace hc12
  {

    static const char *TAG = "hc12";
    const uint32_t ONLINE_TIMEOUT = 2000;    // Drop offline if silence lasts > 2 seconds
    const uint32_t INTER_CHAR_TIMEOUT = 150; // Clear broken partial buffers after 150ms

    void HC12Component::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up HC-12 component...");
      buffer_.reserve(max_buffer_size_);
      last_packet_time_ = millis();
    }

    void HC12Component::dump_config()
    {
      ESP_LOGCONFIG(TAG, "HC-12:");
      ESP_LOGCONFIG(TAG, "  Max Buffer Size: %u", max_buffer_size_);
      ESP_LOGCONFIG(TAG, "  Message Terminator: '%s'", terminator_.c_str());
    }

    void HC12Component::loop()
    {
      uint32_t now = millis();

      // 1. Read incoming UART streams
      while (available() > 0)
      {
        char c = read();
        buffer_ += c;
        last_char_time_ = now;

        // Overflow safety mechanism
        if (buffer_.length() >= max_buffer_size_)
        {
          ESP_LOGW(TAG, "Buffer overflow safety tripped, dumping contents.");
          buffer_.clear();
          continue;
        }

        // Delimiter parsing verification
        if (buffer_.length() >= terminator_.length())
        {
          size_t pos = buffer_.find(terminator_);
          if (pos != std::string::npos)
          {
            std::string message = buffer_.substr(0, pos);
            buffer_.erase(0, pos + terminator_.length());
            
            hc12_online_ = true;
            last_packet_time_ = now; // Lock tracking timestamp precisely on successful parse
            
            this->process_buffer(message);
          }
        }
      }

      // 2. Buffer cleanup for partial or corrupt data frames
      if (!buffer_.empty() && (now - last_char_time_ > INTER_CHAR_TIMEOUT))
      {
        ESP_LOGD(TAG, "Stale un-terminated data cleared from buffer.");
        buffer_.clear();
      }

      // 3. Heartbeat Watchdog evaluation block
      if (hc12_online_ && (now - last_packet_time_ > ONLINE_TIMEOUT))
      {
        ESP_LOGW(TAG, "HC-12 Heartbeat lost. Switching to Offline state.");
        hc12_online_ = false;
      }
    }

    void HC12Component::process_buffer(const std::string &message)
    {
      if (!message.empty())
      {
        ESP_LOGD(TAG, "Received complete packet: %s", message.c_str());
        if (callback_)
          callback_(message);
      }
    }

    void HC12Component::send_message(const std::string &message)
    {
      std::string to_send = message + terminator_;
      write_array(reinterpret_cast<const uint8_t *>(to_send.c_str()), to_send.length());
      ESP_LOGD(TAG, "Sent encoded packet: %s", to_send.c_str());
    }

    bool HC12Component::is_available()
    {
      return hc12_online_;
    }

  } // namespace hc12
} // namespace esphome
