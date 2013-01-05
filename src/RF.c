/*
 * RF.c
 *
 * Created: 11/24/2012 11:41:07 AM
 *  Author: Xilin
 */ 

#include <asf.h>
#include "avr_compiler.h"
#include "spi_driver.h"
#include "clksys_driver.h"
#include "RF.h"
#include "rf_def.h"
#include "SD.h"
#include "port_driver.h"


bool success = true;

void delay_ms(uint16_t val)
{
	for (uint16_t i = 0; i<val; i++)
	{
		delay_us(1000);
	}
}


uint8_t rf_SPI_RW_master(uint8_t val)
{
	return SPI_MasterTransceiveByte(&spiMasterF, val);
}

uint8_t rf_SPI_RW_slave(uint8_t val)
{
	return SPI_MasterTransceiveByte(&spiMasterC, val);
}

uint8_t rf_writereg_master(uint8_t reg, uint8_t val)
{
	uint8_t status;
	SPI_MasterSSLow(&PORTF, PIN4_bm);	
	status = SPI_MasterTransceiveByte(&spiMasterF, reg);
	SPI_MasterTransceiveByte(&spiMasterF, val);
	SPI_MasterSSHigh(&PORTF, PIN4_bm);	
	return status;
}

uint8_t rf_writereg_slave(uint8_t reg, uint8_t val)
{
	uint8_t status;
	SPI_MasterSSLow(&PORTC, PIN4_bm);
	status = SPI_MasterTransceiveByte(&spiMasterC, reg);
	SPI_MasterTransceiveByte(&spiMasterC, val);
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	return status;
}

uint8_t rf_readreg_master(uint8_t reg)
{
	uint8_t val;
	
	SPI_MasterSSHigh(&PORTF, PIN4_bm);
	delay_us(20);
	SPI_MasterSSLow(&PORTF, PIN4_bm);
	delay_us(20);
	
	SPI_MasterTransceiveByte(&spiMasterF, reg);
	val = SPI_MasterTransceiveByte(&spiMasterF, 0xff);
	
	SPI_MasterSSHigh(&PORTF, PIN4_bm);
	return val;
}

uint8_t rf_readreg_slave(uint8_t reg)
{
	uint8_t val;
	
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	delay_us(20);
	SPI_MasterSSLow(&PORTC, PIN4_bm);
	delay_us(20);
	
	SPI_MasterTransceiveByte(&spiMasterC, reg);
	val = SPI_MasterTransceiveByte(&spiMasterC, 0xff);
	
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	return val;
}


uint8_t rf_readbuf_master(uint8_t reg, uint8_t readbuf[TX_PLOAD_WIDTH], uint8_t bytes)
{
	uint8_t status;
	uint16_t i;
	SPI_MasterSSLow(&PORTF, PIN4_bm);
	status = SPI_MasterTransceiveByte(&spiMasterF, reg);
	for (i=0; i<bytes; i++)
	{
		readbuf[i] = SPI_MasterTransceiveByte(&spiMasterF, 0xff);
	}
	SPI_MasterSSHigh(&PORTF, PIN4_bm);
	return(status);  
}

uint8_t rf_readbuf_slave(uint8_t reg, uint8_t readbuf[TX_PLOAD_WIDTH], uint8_t bytes)
{
	uint8_t status;
	uint16_t i;
	SPI_MasterSSLow(&PORTC, PIN4_bm);
	status = SPI_MasterTransceiveByte(&spiMasterC, reg);
	for (i=0; i<bytes; i++)
	{
		readbuf[i] = SPI_MasterTransceiveByte(&spiMasterC, 0xff);
	}
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	return(status);
}

uint8_t rf_writebuf_master(uint8_t reg, uint8_t * writebuf, uint8_t bytes)
{
	uint8_t status;
	uint16_t i;
	SPI_MasterSSLow(&PORTF, PIN4_bm);
	status = SPI_MasterTransceiveByte(&spiMasterF, reg);
	for (i=0; i<bytes; i++)
	{
		SPI_MasterTransceiveByte(&spiMasterF, writebuf[i]);
	}
	SPI_MasterSSHigh(&PORTF, PIN4_bm);
	return(status);
}


uint8_t rf_writebuf_slave(uint8_t reg, uint8_t * writebuf, uint8_t bytes)
{
	uint8_t status;
	uint16_t i;
	SPI_MasterSSLow(&PORTC, PIN4_bm);
	status = SPI_MasterTransceiveByte(&spiMasterC, reg);
	for (i=0; i<bytes; i++)
	{
		SPI_MasterTransceiveByte(&spiMasterC, writebuf[i]);
	}
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	return(status);
}


void nrf24l01_RX_config_master(void)
{
	ioport_set_pin_low(nrf24l01M_CE);
	
	SPI_MasterSSHigh(&PORTF, PIN4_bm);
	delay_us(20);
	SPI_MasterSSLow(&PORTF, PIN4_bm);	
	delay_us(20);
	
	rf_writebuf_master(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	rf_writereg_master(WRITE_REG + EN_AA, 0x01);//enable autoactive 0x01
	rf_writereg_master(WRITE_REG + EN_RXADDR, 0x01);
	rf_writereg_master(WRITE_REG + RF_CH, 40);
	rf_writereg_master(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);
	//rf_writereg(WRITE_REG + RF_SETUP, 0x07);//1Mbps, 0dBm
	rf_writereg_master(WRITE_REG + RF_SETUP, 0x09);
	rf_writereg_master(WRITE_REG + CONFIG, 0x0f);
	
	//SPI_MasterSSLow(ssPort, PIN4_bm);	
	ioport_set_pin_high(nrf24l01M_CE);
	delay_us(150);//at least 130us
}

void nrf24l01_RX_config_slave(void)
{
	ioport_set_pin_low(nrf24l01S_CE);
	
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	delay_us(20);
	SPI_MasterSSLow(&PORTC, PIN4_bm);
	delay_us(20);
	
	rf_writebuf_slave(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	rf_writereg_slave(WRITE_REG + EN_AA, 0x01);//enable autoactive 0x01
	rf_writereg_slave(WRITE_REG + EN_RXADDR, 0x01);
	rf_writereg_slave(WRITE_REG + RF_CH, 40);
	rf_writereg_slave(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);
	rf_writereg_slave(WRITE_REG + RF_SETUP, 0x09);
	rf_writereg_slave(WRITE_REG + CONFIG, 0x0f);
	
	//SPI_MasterSSLow(ssPort, PIN4_bm);
	ioport_set_pin_high(nrf24l01S_CE);
	delay_us(150);//at least 130us
	
	PORT_ConfigurePins( &PORTC,
	0x01,	//set pin PK0 as input 'IRQ';
	false,
	false,
	PORT_OPC_TOTEM_gc,
	PORT_ISC_FALLING_gc );//set falling edge as trigger;
	
	PORT_SetPinsAsInput( &PORTC, 0x01 );
	
	/* Configure Interrupt0 to have medium interrupt level, triggered by pin 0. */
	PORT_ConfigureInterrupt0( &PORTC, PORT_INT0LVL_MED_gc, 0x01 );		
}


void nrf24l01_TX_config_master(uint8_t * writebuf)
{
	ioport_set_pin_low(nrf24l01M_CE);
	
	SPI_MasterSSHigh(&PORTF, PIN4_bm);
	delay_us(20);
	SPI_MasterSSLow(&PORTF, PIN4_bm);
	delay_us(20);	
	
	rf_writebuf_master(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	rf_writebuf_master(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	rf_writebuf_master(WR_TX_PLOAD, writebuf, TX_PLOAD_WIDTH);
	rf_writereg_master(WRITE_REG + EN_AA, 0x01);//enable autoactive 0x01
	rf_writereg_master(WRITE_REG + EN_RXADDR, 0x01);
	rf_writereg_master(WRITE_REG + SETUP_RETR, 0x0a);
	rf_writereg_master(WRITE_REG + RF_CH, 40);
	rf_writereg_master(WRITE_REG + RF_SETUP, 0x09);
	rf_writereg_master(WRITE_REG + CONFIG, 0x0e);
	
	ioport_set_pin_high(nrf24l01M_CE);
	delay_us(12);//at least 10us
}

void nrf24l01_TX_config_slave(uint8_t * writebuf)
{
	ioport_set_pin_low(nrf24l01S_CE);
	
	SPI_MasterSSHigh(&PORTC, PIN4_bm);
	delay_us(20);
	SPI_MasterSSLow(&PORTC, PIN4_bm);
	delay_us(20);
	
	rf_writebuf_slave(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	rf_writebuf_slave(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	rf_writebuf_slave(WR_TX_PLOAD, writebuf, TX_PLOAD_WIDTH);
	rf_writereg_slave(WRITE_REG + EN_AA, 0x01);//enable autoactive 0x01
	rf_writereg_slave(WRITE_REG + EN_RXADDR, 0x01);
	rf_writereg_slave(WRITE_REG + SETUP_RETR, 0x0a);
	rf_writereg_slave(WRITE_REG + RF_CH, 40);
	rf_writereg_slave(WRITE_REG + RF_SETUP, 0x09);
	rf_writereg_slave(WRITE_REG + CONFIG, 0x0e);
	
	ioport_set_pin_high(nrf24l01S_CE);
	delay_us(12);//at least 10us
}

uint8_t Check_ACK_master(uint8_t clear)
{
//	while(ioport_get_value(nrf24l01M_IRQ));//IRQ is active low, so if there is no event, in the loop
	//sta = SPI_MasterTransceiveByte(&spiMasterC, 0xff);
	sta = rf_readreg_master(STATUSREG);
	/*
				do{
				}while(!USART_IsTXDataRegisterEmpty(&USART));
				USART_PutChar(&USART, sta);
	*/			
	SPI_MasterTransceiveByte(&spiMasterF, FLUSH_TX);

	rf_writereg_master(WRITE_REG + STATUSREG, sta);
	
	SPI_MasterTransceiveByte(&spiMasterF, FLUSH_TX);
	
/*	//IRQ = 1;
	ioport_configure_pin(nrf24l01M_IRQ, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_set_pin_high(nrf24l01M_IRQ);
	ioport_configure_pin(nrf24l01M_IRQ, IOPORT_DIR_INPUT);	
	*/
	if (TX_DS)
		return 0x00;
	else
		return 0xff;
}

uint8_t Check_ACK_slave(uint8_t clear)
{
	//while(ioport_get_value(nrf24l01S_IRQ));//IRQ is active low, so if there is no event, in the loop
	//sta = SPI_MasterTransceiveByte(&spiMasterC, 0xff);
	sta = rf_readreg_slave(STATUSREG);
				
	SPI_MasterTransceiveByte(&spiMasterC, FLUSH_TX);

	rf_writereg_slave(WRITE_REG + STATUSREG, sta);
	
	SPI_MasterTransceiveByte(&spiMasterC, FLUSH_TX);
	
	//IRQ = 1;
	//ioport_configure_pin(nrf24l01S_IRQ, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	//ioport_set_pin_high(nrf24l01S_IRQ);
	//ioport_configure_pin(nrf24l01S_IRQ, IOPORT_DIR_INPUT);	
	
	if (TX_DS)
		return 0x00;
	else
		return 0xff;
}


void CheckButtons(void)
{
	uint8_t statmp;
	uint8_t checkack;
	if (!ioport_get_value(GPIO_PUSH_BUTTON_4))
	{
		delay_ms(20);
		if (!ioport_get_value(GPIO_PUSH_BUTTON_4))
		{
			ioport_set_pin_low(LED4_GPIO);
			TX_BUF[0] = DATA;
			nrf24l01_TX_config_master(TX_BUF);
			Check_ACK_master(1); 

			delay_ms(100);
			ioport_set_pin_high(LED4_GPIO);
			//nrf24l01_RX_config();
			
			while(!ioport_get_value(GPIO_PUSH_BUTTON_4));	//wait the button the release
			DATA <<= 1;
			if(!DATA)
			   DATA = 0x01;
			   
		}
	}
}
