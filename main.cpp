#include "mbed.h"
#include "dw1000_spi.h"
 
PwmOut led1(LED1);
DigitalOut led2(LED2);
SPI spi(SPI_MISO, SPI_MOSI, SPI_SCK, SPI_CS);
DigitalIn sIRQ(PA_0);
DigitalOut sReset(PA_1);
DW1000 dw1000(spi, sIRQ, sReset);


// main() runs in its own thread in the OS
int main() {
	printf("Hello World!\n");
	uint32_t dev_id = dw1000.get_dev_id();


	printf("%lx\n", (unsigned long int) dev_id);

	if(dev_id != 0) {
		led2 != led2;
	}


	led1.period(2.0f);
	led1.write(0.3f);
    while (true) {
    }
}
