/*
 * RF.h
 *
 * Created: 11/24/2012 11:41:17 AM
 *  Author: Xilin
 */ 
#include "avr_compiler.h"
#include "spi_driver.h"

//#define nrf24l01M_IRQ		IOPORT_CREATE_PIN(PORTF, 2)
#define nrf24l01M_CE		IOPORT_CREATE_PIN(PORTF, 3)

//#define nrf24l01S_IRQ		IOPORT_CREATE_PIN(PORTC, 2)
#define nrf24l01S_CE		IOPORT_CREATE_PIN(PORTC, 3)

#define TX_ADR_WIDTH	5
#define TX_PLOAD_WIDTH	32

extern volatile uint8_t TX_ADDRESS[TX_ADR_WIDTH]; 
extern volatile uint8_t TX_BUF[TX_PLOAD_WIDTH];
extern volatile uint8_t RX_BUF[TX_PLOAD_WIDTH];
extern volatile uint8_t sta;
extern volatile uint8_t TX_DS;
extern volatile uint8_t DATA;


void delay_ms(uint16_t val);

uint8_t rf_SPI_RW_master(uint8_t val);

uint8_t rf_SPI_RW_slave(uint8_t val);

uint8_t rf_writereg_master(uint8_t reg, uint8_t val);

uint8_t rf_writereg_slave(uint8_t reg, uint8_t val);

uint8_t rf_readreg_master(uint8_t reg);

uint8_t rf_readreg_slave(uint8_t reg);

uint8_t rf_readbuf_master(uint8_t reg, uint8_t readbuf[TX_PLOAD_WIDTH], uint8_t bytes);

uint8_t rf_readbuf_slave(uint8_t reg, uint8_t readbuf[TX_PLOAD_WIDTH], uint8_t bytes);

uint8_t rf_writebuf_master(uint8_t reg, uint8_t * writebuf, uint8_t bytes);

uint8_t rf_writebuf_slave(uint8_t reg, uint8_t * writebuf, uint8_t bytes);

void nrf24l01_RX_config_master(void);

void nrf24l01_RX_config_slave(void);

void nrf24l01_TX_config_master(uint8_t * writebuf);

void nrf24l01_TX_config_slave(uint8_t * writebuf);

uint8_t Check_ACK_master(uint8_t clear);

uint8_t Check_ACK_slave(uint8_t clear);

void CheckButtons(void);

