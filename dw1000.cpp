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
	for(uint8_t i = 0; i<cmd_length; ++i) {
		tx_buffer[i] = 0;
	}
	cmd_length = 0;
}

void DW1000::cmd_set_byte0(uint8_t value){
	tx_buffer[0] &= 0b11000000; // set all bits except RW/subregister to 0
	tx_buffer[0] |= ( ~0b11000000 & value ); // set all bits accept RW/subregister
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

bool DW1000::cmd_is_write() {
	return tx_buffer[0] & (1 << 7);
}

void DW1000::cmd_execute(size_t bytes, const event_callback_t& cb) {
	size_t tx_length = cmd_length;
	size_t rx_length = 0;
	if(cmd_is_write()) {
		tx_length += bytes;
	} else {
		rx_length += bytes;
	}
	spi.transfer(tx_buffer, tx_length * 8,
		rx_buffer, rx_length*8,
		cb, SPI_EVENT_COMPLETE);
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
	dev_id = *((uint32_t*) (rx_buffer+1));
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
	*((uint64_t*) (tx_buffer + cmd_length)) = eui;
	write_no_callback(8+cmd_length);
}

void DW1000::write_no_callback(size_t bytes) {
	spi.transfer(tx_buffer, bytes*8,
				 rx_buffer, 0,
				 event_callback_t(this, &DW1000::no_callback),
				 SPI_EVENT_COMPLETE);
}

void DW1000::request_eui() {
	cmd_reset();
	cmd_set_byte0(0x00);

	spi.transfer(tx_buffer, 1*8,
		rx_buffer, 8*8,
		event_callback_t(this, &DW1000::request_dev_id_callback),
		SPI_EVENT_COMPLETE);
}

void DW1000::request_eui_callback(int narg) {
	(void)narg;
	dev_id = *((uint32_t*) (rx_buffer+1));
}

bool DW1000::dev_id_ok() {
	if(dev_id == 0) {
		request_dev_id();
		wait(0.1f);
	}
	return dev_id == 0xdeca0130;
}
