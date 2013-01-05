/*
 * SD.c
 *
 * Created: 11/23/2012 9:06:36 PM
 * Author: Xilin
 */ 
#include "SD.h"
#include "conf_board.h"

SPI_Master_t spiMasterC;		//brief SPI master module on PORT C.
SPI_Master_t spiMasterF;		//brief SPI master module on PORT F.
SPI_DataPacket_t dataPacket;	//brief SPI Data packet
PORT_t *ssPort = &PORTC;		//Instantiate pointer to ssPort.

uint8_t SD_Header[512] = {
	0x43,0x6F,0x70,0x79,0x72,0x69,0x67,0x68, 0x74,0x20,0x55,0x50,0x65,0x6e,0x6e,0x20,	//Copyright UPenn
	0x32,0x30,0x31,0x32,0x20,0x20,0x20,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,	//2012
	0x4e,0x65,0x72,0x61,0x6c,0x20,0x52,0x65, 0x63,0x6f,0x72,0x64,0x69,0x6e,0x67,0x20,	//Neral Recording
	0x44,0x65,0x76,0x69,0x63,0x65,0x20,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,	//Device
	0x52,0x65,0x76,0x69,0x73,0x69,0x6f,0x6e, 0x20,0x31,0x2e,0x30,0x20,0x20,0x20,0x20,	//Revision 1.0
	0x58,0x69,0x6c,0x69,0x6e,0x20,0x4c,0x69, 0x75,0x20,0x20,0x20,0x20,0x20,0x20,0x20,	//Xilin Liu
	0x78,0x69,0x6c,0x69,0x6e,0x6c,0x69,0x75, 0x40,0x73,0x65,0x61,0x73,0x2e,0x75,0x70,	//xilinliu@seas.up
	0x65,0x6e,0x6e,0x2e,0x65,0x64,0x75,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,	//enn.edu
	0x44,0x65,0x63,0x2e,0x20,0x32,0x31,0x2c, 0x20,0x32,0x30,0x31,0x32,0x20,0x20,0x20,	//Dec. 21, 2012
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void SPI_Low_init(void)
{
	//set SPI to 250kbps with sys clk 32MHz
	PORTC.DIRSET = PIN4_bm;	//Init SS pin as output with wired AND and pull-up.
	PORTC.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTC.OUTSET = PIN4_bm;	//Set SS output to high
	
	PORTF.DIRSET = PIN4_bm;	//Init SS pin as output with wired AND and pull-up.
	PORTF.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTF.OUTSET = PIN4_bm;	//Set SS output to high	
	
	// Initialize SPI master on port C.
	SPI_MasterInit(	&spiMasterC,
					&SPIC,
					&PORTC,
					false,
					SPI_MODE_0_gc,
					SPI_INTLVL_OFF_gc,
					false,
					SPI_PRESCALER_DIV128_gc);

	SPI_MasterInit(	&spiMasterF,
					&SPIF,
					&PORTF,
					false,
					SPI_MODE_0_gc,
					SPI_INTLVL_OFF_gc,
					false,
					SPI_PRESCALER_DIV128_gc);		
	
	SPI_MasterSSLow(ssPort, PIN4_bm);	
	SPI_MasterSSLow(&PORTF, PIN4_bm);	
}


void SPI_High_init(void)
{
	//set SPI to 8Mbps
	PORTC.DIRSET = PIN4_bm;	//Init SS pin as output with wired AND and pull-up.
	PORTC.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTC.OUTSET = PIN4_bm;	//Set SS output to high
	
	PORTF.DIRSET = PIN4_bm;	//Init SS pin as output with wired AND and pull-up.
	PORTF.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTF.OUTSET = PIN4_bm;	//Set SS output to high	
	
	
	// Initialize SPI master on port C.
	SPI_MasterInit(	&spiMasterC,
					&SPIC,
					&PORTC,
					false,
					SPI_MODE_0_gc,
					SPI_INTLVL_OFF_gc,
					false,
					SPI_PRESCALER_DIV4_gc);
					
	SPI_MasterInit(	&spiMasterF,
					&SPIF,
					&PORTF,
					false,
					SPI_MODE_0_gc,
					SPI_INTLVL_OFF_gc,
					false,
					SPI_PRESCALER_DIV4_gc);			
	
	SPI_MasterSSLow(ssPort, PIN4_bm);
	SPI_MasterSSLow(&PORTF, PIN4_bm);				
}



uint8_t SD_writebyte(uint8_t val)
{
	uint8_t response;
	response = SPI_MasterTransceiveByte(&spiMasterC, val);
	return response;
}


int SD_sendCMD(uint8_t cmd, uint32_t arg)
{
	uint8_t r1;
	uint8_t k = 0; // retry times 

	SD_writebyte(0xff);

	SD_writebyte(cmd | 0x40);
	SD_writebyte(arg>>24);
	SD_writebyte(arg>>16);
	SD_writebyte(arg>>8);
	SD_writebyte(arg);
	SD_writebyte(0x95);
	
	do 
	{
		r1 = SPI_MasterTransceiveByte(&spiMasterC, 0xff);	
		k++;	
	} while ((r1 == 0xff) && (k < 100));
	//while((r1 = SPI_MasterTransceiveByte(&spiMasterC, 0xff))==0xff);
	
	return r1;
}


void SD_init(void)
{
	uint8_t r1;
	//use low speed SPI for command
	SPI_Low_init();
	
	//send idle command
	do
	{
		r1 = SD_sendCMD(0, 0);
	} while (r1 != 0x01);
	
	//send active command
	do
	{
		r1 = SD_sendCMD(1, 0);
	} while (r1 != 0x00);
	
	SPI_High_init();
	
	//disable CRC
	do
	{
		r1 = SD_sendCMD(59, 0);
	} while (r1);
	
	//set sector size to be 512 bytes
	do
	{
		r1 = SD_sendCMD(16, 512);
	} while (r1);
	
}


//write one sector
uint8_t SD_writeBlock(uint32_t sector, uint8_t buffer[512])
{
	uint8_t r1;
	uint16_t i;
	
	//SPI_High_init();
	
	//write command 24
	do
	{
		r1 = SD_sendCMD(24, sector<<9);
	} while (r1);
		
	
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}
	
	//send start token
	SD_writebyte(0xfe);
	
	for (i=0; i<512; i++)
	{
		SD_writebyte(buffer[i]);
	}
	
	do
	{
		r1 = SD_writebyte(0xff);
	} while ((r1&0x1f) != 0x05);
	
	while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);
	//while((r1 = SPI_MasterTransceiveByte(&spiMasterC, 0xff))==0xff);
	
	return 0;
}


uint8_t SD_writeMultiBlock_init(uint32_t sector, uint8_t buffer[512])
{
	uint8_t r1;
	uint16_t i;
	
	do
	{
		r1 = SD_sendCMD(25, sector<<9);
	} while (r1);
	
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}	
		
	ioport_set_pin_low(LED2_GPIO);
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}
		
	//send start token
	SD_writebyte(0xfc);
	
	for (i=0; i<512; i++)
	{
		SD_writebyte(buffer[i]);
	}
		
	do
	{
		r1 = SD_writebyte(0xff);
	} while ((r1&0x1f) != 0x05);
		
	while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);
	ioport_set_pin_high(LED2_GPIO);	
}

void SD_writeBlock_continue(uint32_t sector, uint8_t buffer[512])
{
	uint8_t r1;
	uint8_t k = 0; //retry times
	uint16_t i;
	
	ioport_set_pin_low(LED2_GPIO);
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}
		
	//send start token
	SD_writebyte(0xfc);
	
	for (i=0; i<512; i++)
	{
		SD_writebyte(buffer[i]);
	}
	
	do
	{
		r1 = SD_writebyte(0xff);
	} while ((r1&0x1f) != 0x05);
	
	while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);
	ioport_set_pin_high(LED2_GPIO);
	
	//return 0;
}

uint8_t SD_writeBlock_EZ(uint32_t sector, uint8_t buffer[512])
{
	uint8_t r1;
	uint8_t k = 0; //retry times
	uint16_t i;
	
	SPI_High_init();
	
	//write command 24
	do
	{
		r1 = SD_sendCMD(24, sector<<9);
		k++;
	} while ((r1 != 0) && (k < 100));
	
	for (i=0; i<2; i++)
	{
		SD_writebyte(0xff);
	}
	
	//send start token
	SD_writebyte(0xfe);
	
	for (i=0; i<512; i++)
	{
		SD_writebyte(buffer[i]);
	}
/*	
	do
	{
		r1 = SD_writebyte(0xff);
	} while ((r1&0x1f) != 0x05); // judge if it successful
	
	//while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);	//wait no busy
	//while((r1 = SPI_MasterTransceiveByte(&spiMasterC, 0xff))==0xff);
*/	
	return 0;
}


uint8_t SD_writeMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count)
{
	uint8_t r1;
	uint16_t i;
	
	//SPI_High_init();
	
	//write command 24
	do
	{
		r1 = SD_sendCMD(25, sector<<9);
	} while (r1);
	
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}
	
	do
	{
		//single block start 0xfe, start block token 0xfc;
		SD_writebyte(0xfc);
		
		for (i=0; i<512; i++)
		{
			SD_writebyte(*buffer++);
		}
		
		do
		{
			r1 = SD_writebyte(0xff);
		} while ((r1&0x1f) != 0x05);
		
		while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);
		
	} while (--count);

	SPI_MasterTransceiveByte(&spiMasterC, 0xfd);	// send stop token.

	//while((r1 = SPI_MasterTransceiveByte(&spiMasterC, 0xff))==0xff);
	while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);
	return 0;
}

/**************************************************************************************
* FunctionName   : SD_writeMultiBlock_EZ()
* Description    : Write continuous multi-blocks, using the simplest way
* EntryParameter : NO
* ReturnValue    : NO
**************************************************************************************/
uint8_t SD_writeMultiBlock_EZ(uint32_t sector, uint8_t *buffer, uint8_t count)
{
	uint8_t r1;
	uint16_t i;
	uint8_t k1 = 0;
	uint8_t k2 = 0;
	
	SPI_High_init();
	
	//write command 25
	do
	{
		r1 = SD_sendCMD(25, sector<<9);
		k1++;
	} while ((r1 != 0) && (k1 < 100));
	
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}
	
	do
	{
		//single block start 0xfe, start block token 0xfc;
		SD_writebyte(0xfc);
		
		for (i=0; i<512; i++)
		{
			SD_writebyte(*buffer++);
		}
		
		/*
		do
		{
			r1 = SD_writebyte(0xff);
		} while ((r1&0x1f) != 0x05);
		*/
		//while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00); //wait the flash finish.
		
	} while (--count);

	SPI_MasterTransceiveByte(&spiMasterC, 0xfd);	// send stop token.
	
	do
	{
		r1 = SPI_MasterTransceiveByte(&spiMasterC, 0xff);
		k2++;
	} while ((r1 == 0) && (k2 < 100));
		
	//while(SPI_MasterTransceiveByte(&spiMasterC, 0xff) == 0x00);
	return 0;
}


//read one sector
uint8_t SD_readBlock(uint32_t sector, uint8_t buffer[512])
{
	uint8_t r1;
	uint16_t i;
	
	//SPI_High_init();
	//write command 24
	do
	{
		r1 = SD_sendCMD(17, sector<<9);	//512 bytes is 9 bit
	} while (r1);
	
	//check start byte token
	while(SD_writebyte(0xff) != 0xfe);
	
	for(i=0; i<512; i++)	//read 512 bytes
	{
		buffer[i] = SD_writebyte(0xff);
	}
	
	for (i=0; i<3; i++)
	{
		SD_writebyte(0xff);
	}
	return 0;
}


uint32_t SD_ReadCapacity(void)
{
	uint8_t r1;
	uint16_t i;
	uint16_t temp;
	uint8_t buffer[16];
	uint32_t Capacity;
	
	do
	{
		r1 = SD_sendCMD(9, 0);
	} while (r1);
	
	//check start byte token
	while(SD_writebyte(0xff) != 0xfe);
	
	for (i=0; i<16; i++)
	{
		buffer[i] = SD_writebyte(0xff);
	}
	
	for (i=0; i<10; i++)
	{
		SD_writebyte(0xff);
	}
	
	//	C_SIZE
	i = buffer[6]&0x03;
	i<<=8;
	i += buffer[7];
	i<<=2;
	i += ((buffer[8]&0xc0)>>6);

	//  C_SIZE_MULT
	r1 = buffer[9]&0x03;
	r1<<=1;
	r1 += ((buffer[10]&0x80)>>7);
	
	// BLOCKNR
	r1+=2;
	temp = 1;
	while(r1)
	{
		temp*=2;
		r1--;
	}
	Capacity = ((uint32_t)(i+1))*((uint32_t)temp);

	// READ_BL_LEN
	i = buffer[5]&0x0f;
	
	//BLOCK_LEN
	temp = 1;
	while(i)
	{
		temp*=2;
		i--;
	}
	
	//The final result
	Capacity *= (uint32_t)temp;
	return Capacity;
}
