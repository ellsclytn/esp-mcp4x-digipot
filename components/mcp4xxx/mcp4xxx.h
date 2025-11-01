#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace mcp4xxx {

// Memory addresses for MCP4XXX commands
static const uint8_t MCP4XXX_WIPER0_ADDRESS = 0x00;
static const uint8_t MCP4XXX_WIPER1_ADDRESS = 0x01;  // For dual potentiometers
static const uint8_t MCP4XXX_EEPROM0_ADDRESS = 0x02;
static const uint8_t MCP4XXX_EEPROM1_ADDRESS = 0x03;  // For dual potentiometers
static const uint8_t MCP4XXX_TCON_ADDRESS = 0x04;    // Terminal Control register

// Command bits for MCP4XXX SPI interface
static const uint8_t MCP4XXX_WRITE_COMMAND = 0x00;  // C1=0, C0=0
static const uint8_t MCP4XXX_READ_COMMAND = 0x0C;   // C1=1, C0=1
static const uint8_t MCP4XXX_INCREMENT_COMMAND = 0x04;  // C1=0, C0=1
static const uint8_t MCP4XXX_DECREMENT_COMMAND = 0x08;  // C1=1, C0=0

// Maximum wiper value (7-bit: 0-128, 129 steps)
static const uint8_t MCP4XXX_MAX_VALUE = 128;

// TCON register bit masks
static const uint8_t MCP4XXX_TCON_R0A = 0x08;  // Terminal A connect bit (bit 3)
static const uint8_t MCP4XXX_TCON_R0W = 0x04;  // Wiper connect bit (bit 2)
static const uint8_t MCP4XXX_TCON_R0B = 0x02;  // Terminal B connect bit (bit 1)
static const uint8_t MCP4XXX_TCON_DEFAULT = 0xFF;  // All terminals connected

class MCP4XXX : public number::Number, public Component, public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_200KHZ> {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  void set_initial_value(uint8_t value) { this->initial_value_ = value; }

  // Public methods to allow other components (like fan) to control the wiper
  bool write_wiper_value(uint8_t value);
  bool set_terminal_connection(bool connect_a, bool connect_w, bool connect_b);
  bool enable_terminals() { return set_terminal_connection(true, true, true); }
  bool disable_terminals() { return set_terminal_connection(false, false, false); }

 protected:
  bool write_tcon_register(uint8_t value);

  uint8_t create_command_byte(uint8_t address, uint8_t command, uint8_t data_bits = 0);

  uint8_t initial_value_{0};
  uint8_t current_value_{0};
};

}  // namespace mcp4xxx
}  // namespace esphome
