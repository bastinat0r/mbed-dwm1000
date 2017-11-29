#pragma once
#ifndef DW1000_SPI_H
#define DW1000_SPI_H

#include "mbed.h"
#include "inttypes.h"
#include "libdw1000.h"

#define TX_BUFFER_LENGTH (1024 + 3)
#define RX_BUFFER_LENGTH 1024


class DW1000 {
private:
	SPI& spi;
	DigitalIn& irq;
	DigitalOut& sReset;
	uint32_t dev_id;
	uint64_t eui;

public:
	DW1000(SPI& spi, DigitalIn& irq, DigitalOut& sReset);
	dwDevice_t dwm_device;

	bool dev_id_ok();

};

#endif
