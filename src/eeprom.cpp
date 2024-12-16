
#include <eeprom.h>

uint8_t EEPROM_read(uint16_t uiAddress, uint8_t *data)
{
	if(uiAddress > EEPROM_SIZE){
		return EEPROM_INVALID_ADDR;
	}
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address register */
	EEARH = (uiAddress & 0xFF00) >> 8;
	EEARL = (uiAddress & 0x00FF);
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from Data Register */
	*data = EEDR;
	return EEPROM_OK;
}


uint8_t EEPROM_write(uint16_t uiAddress, uint8_t ucData)
{
	if(uiAddress > EEPROM_SIZE){
		return EEPROM_INVALID_ADDR;
	}
	/* Wait for completion of previous write */
	while(EECR & (1<<EEPE));
	/* Set up address and Data Registers */
	EEARH = (uiAddress & 0xFF00) >> 8;
	EEARL = (uiAddress & 0x00FF);
	EEDR = ucData;
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	/* Start eeprom write by setting EEPE */
	// EEPE = EEPROM Write Enable
	// EEMPE = EEPROM Master Write Enable
	EECR |= (1<<EEPE);
	return EEPROM_OK;
}

// reads a len of data from the eeprom
uint8_t EEPROM_read_batch(uint16_t uiAddress, void *data,uint16_t len){
	uint16_t i = 0;
	uint8_t err = EEPROM_OK;
	uint8_t *data_cast = (uint8_t *)data;
	
	for(i = 0; i < len; i++){
		err = EEPROM_read(uiAddress + i,&data_cast[i]);
		if(err != EEPROM_OK){
			return err;
		}
	}

	return EEPROM_OK;
}