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
	void request_dev_id();
	void request_dev_id_callback(int narg);

	uint32_t dev_id;
	uint8_t tx_buffer[TX_BUFFER_LENGTH];
    uint8_t rx_buffer[RX_BUFFER_LENGTH];

public:
	DW1000(SPI& spi, DigitalIn& irq, DigitalOut& sReset);
	uint32_t get_dev_id();

};

#endif
