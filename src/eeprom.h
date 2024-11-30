#pragma once
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define EEPROM_SIZE 1023

// Basic error codes
enum{
	EEPROM_OK,
	EEPROM_WRITE_FAIL,
	EEPROM_INVALID_ADDR	
};


// EEAR -> EEPROM Address Register
// EEDR -> EEPROM Data Register
// EECR -> EEPROM Control Register

uint8_t EEPROM_read(uint16_t uiAddress, uint8_t *data);
uint8_t EEPROM_write(uint16_t uiAddress, uint8_t ucData);
uint8_t EEPROM_read_batch(uint16_t uiAddress, void *data,uint16_t len);


