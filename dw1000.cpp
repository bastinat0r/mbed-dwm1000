#include "dw1000.h"
#include "mbed.h"


DW1000::DW1000(SPI& spi, DigitalIn& irq, DigitalOut& sReset) : 
	spi(spi),
	irq(irq),
	sReset(sReset),
	dev_id(0),
	eui(0),
	cmd_length(0)
{
		spi.format(8, 0);
		spi.frequency(1000000);
		sReset = 1;
		cmd_reset();
}

void DW1000::cmd_reset() {
	for(uint8_t i = 0; i<4; ++i) {
		tx_buffer[i] = 0;
	}
	cmd_length = 0;
}

void DW1000::cmd_set_byte0(uint8_t value){
	tx_buffer[0] &= 0b11000000; // set all bits except RW/subregister to 0
	tx_buffer[0] |= ( ~0b11000000 & value ); // set all bits except RW/subregister
	if(cmd_length < 1)
		cmd_length = 1;
} 

void DW1000::cmd_set_byte1(uint8_t value){
	tx_buffer[1] &= 0b10000000; // set all bits except subregister to 0
	tx_buffer[1] |= ( ~0b10000000 & value ); // set all bits except subregister
	tx_buffer[0] |= 0b01000000;
	if(cmd_length < 2)
		cmd_length = 2;
} 

void DW1000::cmd_set_byte2(uint8_t value){
	tx_buffer[2] = value;
	tx_buffer[0] |= 0b01000000; // in case someone omits to set byte1
	tx_buffer[1] |= 0b10000000;
	cmd_length = 3;
} 

void DW1000::cmd_set_write_bit() {
	tx_buffer[0] |= (1 << 7);
}

bool DW1000::cmd_is_write() {
	return tx_buffer[0] & (1 << 7);
}

void DW1000::cmd_execute(size_t bytes, const event_callback_t& cb) {
	size_t tx_length = cmd_length;
	size_t rx_length = 0;
	if(cmd_is_write()) { 
		tx_length += bytes;
	} else {
		rx_length += bytes + cmd_length; // the client transmits while we send cmd
	}
	spi.transfer(tx_buffer, tx_length * 8,
		rx_buffer, rx_length*8,
		cb, SPI_EVENT_COMPLETE);
}

void DW1000::cmd_execute(size_t bytes) {
	cmd_execute(bytes, event_callback_t(this, &DW1000::no_callback));
}

uint32_t DW1000::get_dev_id() {
	return dev_id;
}

void DW1000::request_dev_id() {
	cmd_reset();
	cmd_set_byte0(0x00);
	cmd_execute(4, event_callback_t(this, &DW1000::request_dev_id_callback));
}

void DW1000::request_dev_id_callback(int narg) {
	(void)narg;
	dev_id = rx_get_payload4(1);
}

// extended unique identifier
uint64_t DW1000::get_eui() {
	return eui;
}

void DW1000::set_eui(uint64_t new_eui) {
	cmd_reset();
	cmd_set_write_bit();
	cmd_set_byte0(0x01);
	eui = new_eui;
	tx_set_payload8(eui);
	cmd_execute(8);
}

void DW1000::request_eui() {
	cmd_reset();
	cmd_set_byte0(0x01);

	cmd_execute(8, event_callback_t(this, &DW1000::request_eui_callback));
}

void DW1000::request_eui_callback(int narg) {
	(void)narg;
	eui = rx_get_payload8(1);
}

bool DW1000::dev_id_ok() {
	if(dev_id == 0) {
		request_dev_id();
		wait(0.1f);
	}
	return dev_id == 0xdeca0130;
}

void DW1000::led_control(bool enable, uint8_t blinknow) {
	cmd_reset();
	cmd_set_write_bit();
	cmd_set_byte0(0x36);
	cmd_set_byte1(0x28);
	tx_buffer[cmd_length + 0] = 0x00;
	tx_buffer[cmd_length + 1] = blinknow & 0x0F;
	tx_buffer[cmd_length + 2] = enable ? 0x01 : 0x00;
	tx_buffer[cmd_length + 3] = 0x20; // the default blink length

	cmd_execute(4);
}

void DW1000::gpio_mode_led() {
	cmd_reset();
	cmd_set_write_bit();
	cmd_set_byte0(0x26);
	cmd_set_byte1(0x00);
	tx_buffer[cmd_length + 0] = 0x00;
	tx_buffer[cmd_length + 1] = 0x00;
	tx_buffer[cmd_length + 2] = 0b00010101;
	tx_buffer[cmd_length + 3] = 1<<6;

	cmd_execute(4);
}


union Payload {
	uint64_t value8;
	uint32_t value4;
	uint8_t bytes[8];
};

uint32_t DW1000::rx_get_payload4(uint8_t offset) {
	Payload payload;
	for(size_t i = 0; i<4; ++i) {
		payload.bytes[i] = rx_buffer[i+offset];
	}
	return payload.value4;
}

uint64_t DW1000::rx_get_payload8(uint8_t offset) {
	Payload payload;
	for(size_t i = offset; i<8; ++i) {
		payload.bytes[i] = rx_buffer[i+offset];
	}
	return payload.value8;
}
void DW1000::tx_set_payload4(uint32_t value) {
	Payload payload;
	payload.value4 = value;
	for(size_t i = 0; i<4; ++i) {
		tx_buffer[i + cmd_length] = payload.bytes[i];
	}
}

void DW1000::tx_set_payload8(uint64_t value) {
	Payload payload;
	payload.value8 = value;
	for(size_t i = 0; i<8; ++i) {
		tx_buffer[i + cmd_length] = payload.bytes[i];
	}
}
