/*
 * SD.h
 *
 * Created: 11/23/2012 9:06:50 PM
 *  Author: Xilin
 */ 
#include "avr_compiler.h"
#include "spi_driver.h"
#include <asf.h>
#include "conf_usb.h"
#include "spi_driver.h"
#include "rf_def.h"
#include "port_driver.h"


extern SPI_Master_t spiMasterC;		//brief SPI master module on PORT C.
extern SPI_Master_t spiMasterF;		//brief SPI master module on PORT F.
extern SPI_DataPacket_t dataPacket;	//brief SPI Data packet
extern PORT_t *ssPort;		//Instantiate pointer to ssPort.
extern uint8_t SD_Header[512];

void SPI_Low_init(void);

void SPI_High_init(void);

uint8_t SD_writebyte(uint8_t val);

int SD_sendCMD(uint8_t cmd, uint32_t arg);

void SD_init(void);

uint8_t SD_writeBlock(uint32_t sector, uint8_t buffer[512]);

uint8_t SD_writeBlock_EZ(uint32_t sector, uint8_t buffer[512]);

uint8_t SD_readBlock(uint32_t sector, uint8_t buffer[512]);

uint32_t SD_ReadCapacity(void);

uint8_t SD_writeMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count);

uint8_t SD_writeMultiBlock_EZ(uint32_t sector, uint8_t *buffer, uint8_t count);

uint8_t SD_writeMultiBlock_init(uint32_t sector, uint8_t buffer[512]);

void SD_writeBlock_continue(uint32_t sector, uint8_t buffer[512]);