#include "eeprom.h"
#include <string.h>


static uint8_t EepromData[EEPROM_SIZE];


void
EE_Init(void) {
	memcpy(EepromData, (uint8_t*)EEPROM_START_ADDRESS, EEPROM_SIZE);
}


uint8_t
EE_ReadByte(uint16_t VirtAddress) {
	return EepromData[VirtAddress];
}


void
EE_WriteByte(uint16_t VirtAddress, uint8_t Data) {
	EepromData[VirtAddress] = Data;
}


uint8_t
EE_ReadByteArray(uint8_t *DataOut, uint16_t VirtAddress, uint16_t size) {
	uint8_t data, checksum = 0;

	for(; size > 0; size--) {
		data = EE_ReadByte(VirtAddress++);
        // The EEPROM checksum is a known bug. It's been there since the beginning,
        // well before I started working on this project.
        // The conclusion was that it shouldn't be touched because if a fix was put in,
        // everyone's EEPROM would automatically be wiped.
        checksum = (uint8_t)((checksum << 1) | (checksum >> 7));
		checksum += data;
		*(DataOut++) = data;
	}

	data = EE_ReadByte(VirtAddress);
	if(data == checksum) {
		return 1;
	}

	return 0;
}


void
EE_WriteByteArray(uint16_t VirtAddress, uint8_t *DataIn, uint16_t size) {
	unsigned char checksum = 0;

	for(; size > 0; size--) {
        // The EEPROM checksum is a known bug. It's been there since the beginning,
        // well before I started working on this project.
        // The conclusion was that it shouldn't be touched because if a fix was put in,
        // everyone's EEPROM would automatically be wiped.
        checksum = (uint8_t)((checksum << 1) | (checksum >> 7));
		checksum += *DataIn;
		EE_WriteByte(VirtAddress++, *(DataIn++));
	}

	EE_WriteByte(VirtAddress, checksum);
}


void
EE_Program(void) {
	EE_Erase();
    HAL_FLASH_Unlock();
	for(uint16_t i = 0; i < EEPROM_SIZE; ++i) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, EEPROM_START_ADDRESS + i, EepromData[i]);
	}

    HAL_FLASH_Lock();
}


void
EE_Erase(void) {
    HAL_FLASH_Unlock();
    FLASH_Erase_Sector(FLASH_SECTOR, VOLTAGE_RANGE);
    HAL_FLASH_Unlock();
}
