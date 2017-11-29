#include "dw1000.h"
#include "mbed.h"
#include "libdw1000.h"


DW1000::DW1000(SPI& spi, DigitalIn& irq, DigitalOut& sReset) : 
	spi(spi),
	irq(irq),
	sReset(sReset),
	dev_id(0),
	eui(0)
{
		spi.format(8, 0);
		spi.frequency(1000000);
		sReset = 1;
}

bool DW1000::dev_id_ok() {
	return dev_id == 0xdeca0130;
}
