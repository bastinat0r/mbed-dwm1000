#include "mbed.h"
#include "dw1000.h"
 
DigitalOut heartbeat(LED2);
DigitalOut led(LED1);
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS);
DigitalIn sIRQ(PA_0);
DigitalOut sReset(PA_1);
DW1000 dw1000(spi, sIRQ, sReset);

// main() runs in its own thread in the OS
int main() {
	uint64_t dev_id;
	dev_id = dw1000.get_dev_id();
	if(dev_id != 0) {
		led = 1;
	}
	
	dw1000.request_dev_id();
	


    while (true) {
		heartbeat = !heartbeat;
		wait(.5f);
		dev_id = dw1000.get_dev_id();
		if(dev_id != 0) {
			led = 1;
		}
    }
}
