#include "mcp4xxx.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mcp4xxx {

static const char *const TAG = "mcp4xxx";

void MCP4XXX::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MCP4XXX...");

  this->spi_setup();

  // Set initial value
  if (!this->write_wiper_value(this->initial_value_)) {
    ESP_LOGE(TAG, "Failed to set initial value");
    this->mark_failed();
    return;
  }

  this->current_value_ = this->initial_value_;
  this->publish_state(this->current_value_);
}

void MCP4XXX::dump_config() {
  ESP_LOGCONFIG(TAG, "MCP4XXX Digital Potentiometer:");
  LOG_PIN("  CS Pin: ", this->cs_);
  ESP_LOGCONFIG(TAG, "  Initial Value: %d", this->initial_value_);
  ESP_LOGCONFIG(TAG, "  Current Value: %d", this->current_value_);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with MCP4XXX failed!");
  }
}

uint8_t MCP4XXX::create_command_byte(uint8_t address, uint8_t command, uint8_t data_bits) {
  // Command byte format: AD3 AD2 AD1 AD0 C1 C0 D9 D8
  // Address bits: 7-4, Command bits: 3-2, Data bits: 1-0
  return (address << 4) | (command & 0x0C) | (data_bits & 0x03);
}

bool MCP4XXX::write_wiper_value(uint8_t value) {
  if (value > MCP4XXX_MAX_VALUE) {
    ESP_LOGE(TAG, "Invalid wiper value: %d (max: %d)", value, MCP4XXX_MAX_VALUE);
    return false;
  }

  this->enable();

  // Create command byte: address=0 (wiper0), write command, D8=0
  uint8_t command_byte = this->create_command_byte(MCP4XXX_WIPER0_ADDRESS, MCP4XXX_WRITE_COMMAND, 0);

  // Send 16-bit command: command byte + data byte
  this->write_byte(command_byte);
  this->write_byte(value);

  this->disable();

  ESP_LOGVV(TAG, "Wrote wiper value: command=0x%02X, data=0x%02X", command_byte, value);
  return true;
}

bool MCP4XXX::write_tcon_register(uint8_t value) {
  this->enable();

  // Create command byte: TCON address, write command, D8=0
  uint8_t command_byte = this->create_command_byte(MCP4XXX_TCON_ADDRESS, MCP4XXX_WRITE_COMMAND, 0);

  // Send 16-bit command: command byte + data byte
  this->write_byte(command_byte);
  this->write_byte(value);

  this->disable();

  ESP_LOGVV(TAG, "Wrote TCON register: command=0x%02X, data=0x%02X", command_byte, value);
  return true;
}

bool MCP4XXX::set_terminal_connection(bool connect_a, bool connect_w, bool connect_b) {
  uint8_t tcon_value = MCP4XXX_TCON_DEFAULT;  // Start with all bits set

  // Clear bits for terminals that should be disconnected
  if (!connect_a) {
    tcon_value &= ~MCP4XXX_TCON_R0A;
  }
  if (!connect_w) {
    tcon_value &= ~MCP4XXX_TCON_R0W;
  }
  if (!connect_b) {
    tcon_value &= ~MCP4XXX_TCON_R0B;
  }

  ESP_LOGD(TAG, "Setting terminal connections - A:%s, W:%s, B:%s (TCON=0x%02X)",
           connect_a ? "ON" : "OFF", connect_w ? "ON" : "OFF", connect_b ? "ON" : "OFF", tcon_value);

  return this->write_tcon_register(tcon_value);
}

}  // namespace mcp4xxx
}  // namespace esphome
