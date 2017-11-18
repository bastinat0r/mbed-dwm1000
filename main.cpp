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
	heartbeat = 1;
	led = dw1000.dev_id_ok();
	wait(4.0f);
	dw1000.request_eui();
	wait(0.1f);
	uint64_t uid = dw1000.get_eui();
	wait(0.1f);
	dw1000.set_eui(0x01);
	wait(0.1f);
	dw1000.request_eui();
	wait(0.1f);
	uid = dw1000.get_eui();
	led = (uid == 0x1);


	dw1000.gpio_mode_led();
    while (true) {
		heartbeat = !heartbeat;
		dw1000.led_control(true, 0x0F);
		wait(.5f);
    }
}
