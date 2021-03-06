#ifndef ESPHOMELIB_SPI_COMPONENT_H
#define ESPHOMELIB_SPI_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_SPI

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"

ESPHOMELIB_NAMESPACE_BEGIN

class SPIComponent : public Component {
 public:
  SPIComponent(GPIOPin *clk, GPIOPin *miso, GPIOPin *mosi);

  void setup() override;

  void dump_config() override;

  uint8_t read_byte();

  void read_array(uint8_t *data, size_t length);

  void write_byte(uint8_t data);

  void write_array(uint8_t *data, size_t length);

  void enable(GPIOPin *cs, bool msb_first, bool high_speed);

  void disable();

  float get_setup_priority() const override;

  void set_miso(const GPIOInputPin &miso);

  void set_mosi(const GPIOOutputPin &mosi);

 protected:
  GPIOPin *clk_;
  GPIOPin *miso_;
  GPIOPin *mosi_;
  GPIOPin *active_cs_{nullptr};
  bool msb_first_{true};
  bool high_speed_{false};
};

class SPIDevice {
 public:
  SPIDevice(SPIComponent *parent, GPIOPin *cs);

  void spi_setup();

  void enable();

  void disable();

  uint8_t read_byte();

  void read_array(uint8_t *data, size_t length);

  void write_byte(uint8_t data);

  void write_array(uint8_t *data, size_t length);

 protected:
  virtual bool msb_first() = 0;

  virtual bool high_speed();

  SPIComponent *parent_;
  GPIOPin *cs_;
};



ESPHOMELIB_NAMESPACE_END

#endif //USE_SPI

#endif //ESPHOMELIB_SPI_COMPONENT_H
