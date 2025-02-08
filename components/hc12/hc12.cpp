#include "hc12.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hc12 {

static const char* TAG = "hc12";

void HC12Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HC-12 component...");
}

void HC12Component::loop() {
  while (available()) {
    char c = read();
    
    // Skip any control characters or invalid bytes at the start of message
    if (buffer_.empty() && !is_valid_char(c)) {
      ESP_LOGV(TAG, "Skipping invalid start character: 0x%02X", static_cast<uint8_t>(c));
      continue;
    }
    
    if (c == MESSAGE_TERMINATOR) {
      process_buffer();
    } else {
      buffer_ += c;
    }
    
    // Prevent buffer overflow
    if (buffer_.length() >= MAX_BUFFER_SIZE) {
      ESP_LOGW(TAG, "Buffer overflow, clearing buffer");
      buffer_.clear();
    }
  }
}

bool HC12Component::is_valid_char(char c) const {
  // Allow only printable ASCII and valid UTF-8 continuation bytes
  return (c >= 32 && c <= 126) || (c >= 0x80);
}

void HC12Component::process_buffer() {
  if (!buffer_.empty()) {
    std::string clean_data = sanitize_message(buffer_);
    
    if (!clean_data.empty()) {
      ESP_LOGD(TAG, "Received: %s", clean_data.c_str());
      
      if (callback_) {
        callback_(clean_data);
      }
    }
    
    buffer_.clear();
  }
}

std::string HC12Component::sanitize_message(const std::string& message) const {
  std::string clean_message;
  clean_message.reserve(message.length());
  bool found_invalid = false;
  
  for (char c : message) {
    if (is_valid_char(c)) {
      clean_message += c;
    } else {
      found_invalid = true;
      ESP_LOGV(TAG, "Filtered invalid character: 0x%02X", static_cast<uint8_t>(c));
    }
  }
  
  if (found_invalid) {
    ESP_LOGD(TAG, "Original message contained invalid characters");
  }
  
  return clean_message;
}

void HC12Component::send_message(const std::string& message) {
  if (message.empty()) {
    ESP_LOGW(TAG, "Attempted to send empty message");
    return;
  }
  
  std::string clean_message = sanitize_message(message);
  if (clean_message.empty()) {
    ESP_LOGW(TAG, "Message contained no valid characters");
    return;
  }
  
  clean_message += MESSAGE_TERMINATOR;
  write_array(reinterpret_cast<const uint8_t*>(clean_message.c_str()), 
              clean_message.length());
  ESP_LOGD(TAG, "Sent: %s", clean_message.c_str());
}

}  // namespace hc12
}  // namespace esphome
