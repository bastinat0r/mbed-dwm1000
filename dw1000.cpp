#include "dw1000.h"
#include "mbed.h"

DW1000::DW1000(SPI& spi, DigitalIn& irq, DigitalOut& sReset) : 
	spi(spi),
	irq(irq),
	sReset(sReset) {
		dev_id = 0;
	
}

uint32_t DW1000::get_dev_id() {
	return dev_id;
}


void DW1000::request_dev_id() {
	tx_buffer[0] = 0;

	spi.transfer(tx_buffer, 1,
		rx_buffer, 4,
		event_callback_t(this, &DW1000::request_dev_id_callback),
		SPI_EVENT_COMPLETE);
}

void DW1000::request_dev_id_callback(int narg) {
	(void)narg;
	dev_id = *((uint32_t*) (rx_buffer+1));
}

