#pragma once
#ifndef DW1000_SPI_H
#define DW1000_SPI_H

#include "mbed.h"
#include "inttypes.h"

#define TX_BUFFER_LENGTH (1024 + 3)
#define RX_BUFFER_LENGTH 1024


class DW1000 {
private:
	SPI& spi;
	DigitalIn& irq;
	DigitalOut& sReset;
	uint32_t dev_id;
	uint64_t eui;
	uint8_t cmd_length;

	void request_dev_id_callback(int narg);
	void request_eui_callback(int narg);
	void no_callback(int narg){};

	void cmd_set_write_bit();
	void cmd_reset();
	void cmd_set_byte0(uint8_t value);
	void cmd_set_byte1(uint8_t value);
	void cmd_set_byte2(uint8_t value);
	void cmd_execute(size_t bytes, const event_callback_t& cb);
	void cmd_execute(size_t bytes);
	bool cmd_is_write();
	uint32_t rx_get_payload4(uint8_t offset);
	uint64_t rx_get_payload8(uint8_t offset);
	void tx_set_payload4(uint32_t value);
	void tx_set_payload8(uint64_t value);

	uint8_t tx_buffer[TX_BUFFER_LENGTH];
    uint8_t rx_buffer[RX_BUFFER_LENGTH];

public:
	DW1000(SPI& spi, DigitalIn& irq, DigitalOut& sReset);

	void request_dev_id();
	uint32_t get_dev_id();
	void request_eui();
	uint64_t get_eui();
	void set_eui(uint64_t eui);
	bool dev_id_ok();
	void gpio_mode_led();
	void led_control(bool enable, uint8_t blinknow=0);

};

#endif
