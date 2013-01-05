/**
 * \file
 *
 * \12.11.13 add USB ASF and set up environment.
 * \12.11.23 add SD card functions.
 * \12.11.24 add rf, using SPI 12MHz, correct delay_us
 * \12.12.31 modify SD multi-sector write.
 */

#include <asf.h>
#include "conf_usb.h"
#include "avr_compiler.h"
#include "spi_driver.h"
#include "rf_def.h"
#include "SD.h"
#include "RF.h"
#include "port_driver.h"
#include "crc.h"
#include "string.h"
#include "adc_driver.h"

#define SECTER_LENGTH 512

// Define variable for SD card
uint8_t received_byte;
uint8_t response;
uint32_t Capacity;
volatile uint8_t SD_buff1[SECTER_LENGTH];
volatile uint8_t SD_buff1_ready = 0;
volatile uint8_t SD_buff2[SECTER_LENGTH];
volatile uint8_t SD_buff2_ready = 0;
volatile uint16_t SD_counter = 0;	// SD block postion counter;
volatile uint32_t SD_addr = 1;
uint8_t myvalue;
uint32_t myaddr= 10;
uint8_t mycmd;
uint16_t i;
uint8_t j=0;
uint8_t k=0;
uint8_t a=0;
uint8_t tx_data;
uint8_t rx_data;
uint8_t usart_flag;
uint8_t send;


// Define variable for nrf24l01
volatile uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01}; 
volatile uint8_t RX_BUF[TX_PLOAD_WIDTH];
volatile uint8_t TX_BUF[TX_PLOAD_WIDTH];
volatile uint8_t sta;
volatile uint8_t sta2;
volatile uint8_t flag;
volatile uint8_t DATA = 0x01;
volatile uint8_t butten_flag;
volatile uint8_t RX_DR = 0x00;	//sta^6
volatile uint8_t TX_DS = 0x00;	//sta^5
volatile uint8_t MAX_RT = 0x00;	//sta^4
volatile uint8_t rf_counter = 0;	// rf block postion counter;
volatile uint8_t rf_buf_ready = 0;
volatile uint8_t ADC_data[18];

volatile uint16_t ADC_result0 = 0x00;
volatile uint16_t ADC_result1 = 0x00;
volatile uint16_t ADC_result2 = 0x00;
volatile uint16_t ADC_result3 = 0x00;

#define ADC_CH_GAINFAC_gm  0x1C  // Gain Factor group mask. 
static int8_t offset;// Variable for use when we read the result from an ADC channel
static int8_t offset1;


user_callback_vbus_action(b_vbus_high){
	if (b_vbus_high)
	{
		udc_attach();
	}
	else
	{
		udc_detach();
	}
}


void InitADC( ADC_t *ADC_Pointer )
{
	// Move stored calibration values to ADC A
	ADC_CalibrationValues_Load(&ADCA);
	
	// Initialize sweep for channel 0, 1, 2 and 3
	ADC_SweepChannels_Config( ADC_Pointer, ADC_SWEEP_0123_gc );

	// Setup event to start synchronized sweep, event channel 0 trigger the sweep
	ADC_Events_Config( ADC_Pointer, ADC_EVSEL_0123_gc, ADC_EVACT_SYNCHSWEEP_gc );

	// Initialize the four channels to convert in single ended mode
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH0, ADC_CH_INPUTMODE_DIFFWGAIN_gc, ADC_CH_GAIN_2X_gc );
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH1, ADC_CH_INPUTMODE_DIFFWGAIN_gc, ADC_CH_GAIN_2X_gc );
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH2, ADC_CH_INPUTMODE_DIFFWGAIN_gc, ADC_CH_GAIN_2X_gc );
	ADC_Ch_InputMode_and_Gain_Config( &ADC_Pointer->CH3, ADC_CH_INPUTMODE_DIFFWGAIN_gc, ADC_CH_GAIN_2X_gc );
	
	// Route the channels to different pins
	// Note that in Single Ended Mode, there is no negative input
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH0, ADC_CH_MUXPOS_PIN0_gc, ADC_CH_MUXNEG_PIN4_gc );
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH1, ADC_CH_MUXPOS_PIN1_gc, ADC_CH_MUXNEG_PIN5_gc );
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH2, ADC_CH_MUXPOS_PIN2_gc, ADC_CH_MUXNEG_PIN6_gc );
	ADC_Ch_InputMux_Config( &ADC_Pointer->CH3, ADC_CH_MUXPOS_PIN3_gc, ADC_CH_MUXNEG_PIN7_gc );
	
	// Sample rate is CPUFREQ / 32. @ 2 MHz this equals 62,5ksps
	ADC_Prescaler_Config( ADC_Pointer, ADC_PRESCALER_DIV16_gc);
	
	// Set up ADC A to have signed conversion mode and 12 bit resolution.
	ADC_ConvMode_and_Resolution_Config( ADC_Pointer, true, ADC_RESOLUTION_12BIT_gc );
	
	// Set reference voltage on ADCx to be VCC/1.6 V
	ADC_Reference_Config( ADC_Pointer, ADC_REFSEL_VCC_gc );
	
	
	// Wait until common mode voltage is stable. Default clk is 2MHz and
	// therefore within the maximum frequency to use this function.
	ADC_Wait_8MHz( ADC_Pointer );
	

	// Setup Interrupt Mode Above Compare Value (ADCB.CMP)
	ADC_Ch_Interrupts_Config(&ADCA.CH3,
	ADC_CH_INTMODE_COMPLETE_gc, //Interrupt on conversion complete
	ADC_CH_INTLVL_MED_gc);

	// Enable the Medium Level interrupts needed in this task
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;

	// In the while(1) loop, a conversion is started on CH0 and the 8 MSB of the result is
	// ouput on the LEDPORT when the conversion is done

	// Enable free running mode
	//ADC_FreeRunning_Enable(&ADCA);
	
	// Enable the ADC
	ADC_Enable( ADC_Pointer );

	// Get offset value for ADC A.
	offset = ADC_Offset_Get_Signed(&ADCA, &(ADCA.CH0), true);
	offset1 = ADC_Offset_Get_Signed(&ADCA, &(ADCA.CH1), true);
}


int main (void)
{
	//irq_initialize_vectors();
	//cpu_irq_enable();
	//sleepmgr_init();		
	//sysclk_init();	
	board_init();
	//udc_start();
	
	/*	
	if (!udc_include_vbus_monitoring()) {
		user_callback_vbus_action(true);
	}	
	udc_attach();
	*/
	
	SD_init();
	SD_writeMultiBlock_init(0, SD_Header);
	
	//SPI_High_init();//PORTC for rf pairs
		
	/*
	nrf24l01_RX_config_master();		
	
	PORT_ConfigurePins( &PORTF,
						0x01,	//set pin PK0 as input 'IRQ';
						false,
						false,
						PORT_OPC_TOTEM_gc,
						PORT_ISC_FALLING_gc );//set falling edge as trigger;
	
	PORT_SetPinsAsInput( &PORTF, 0x01 );
	
	// Configure Interrupt0 to have medium interrupt level, triggered by pin 0. 
	PORT_ConfigureInterrupt0( &PORTF, PORT_INT0LVL_LO_gc, 0x01 );	
	*/
	/*	
	// Set up Timer/Counter 0.
	// SYSCLK_PSADIV_1 DIV64£º  12000 -> 16ms (accurate)  
	//							50000 -> 66.8ms
	//
	// 300 --> 50us
	TCC0.PER = 300;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV8_gc; // Work from CPUCLK/64.
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_MED_gc; // Enable overflow interrupt.--
	//sysclk = 48MHz --> syslck/8
	*/

	// initialize clock source to be 32MHz internal oscillator (no PLL)
	OSC.CTRL = OSC_RC32MEN_bm | OSC_RC32KEN_bm; // enable 32MHz and 32KHz oscillators
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
	CCP = CCP_IOREG_gc; //Security Signature to modify clock
	CLK.CTRL = 0x01; //system clock = 32MHz selected
		
	// Set up the ADC_Pointer to the ADC module to use
	ADC_t *ADC_Pointer;
	ADC_Pointer = &ADCA;	
	
	InitADC( ADC_Pointer );
	
	// We will use TCC0 overflows to trigger events on channel 0
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	
	// Start Timer/Counter C0 with main clock (2MHz) divided by 8
	TCC0.CTRLA = (TCC0.CTRLA & ~TC0_CLKSEL_gm) | TC_CLKSEL_DIV4_gc;
	//TCC0.PER = 0x00C7;
	TCC0.PER = 0x0197;
	//TCC0.CTRLA = TC_CLKSEL_DIV8_gc;		
	
	/*
	// Enable medium level interrupts in the PMIC.
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	*/
		
	// Enable the global interrupt flag. 
	sei();	
	ioport_set_pin_low(LED0_GPIO);	
	
	SD_addr = 1;
	
	while (true)
	{
		if (SD_buff1_ready)
		{
			ioport_set_pin_high(LED3_GPIO);
			SD_writeBlock_continue(SD_addr, SD_buff1);
			SD_buff1_ready = 0;
			SD_addr = SD_addr+1;
			ioport_set_pin_low(LED3_GPIO);
			
			/*
			delay_us(100);
			for (j = 0; j<16; j++)	//send 20 times
			{
				ioport_set_pin_high(LED2_GPIO);
				for (k = 0; k<32; k++)
				{
					TX_BUF[k] = SD_buff1[j*32 + k];
				}
				nrf24l01_TX_config_master(TX_BUF);
				Check_ACK_master(1);
				ioport_set_pin_low(LED2_GPIO);
				delay_us(20);
			}
			*/						
		}
		
		
		else if (SD_buff2_ready)
		{
			ioport_set_pin_high(LED3_GPIO);
			SD_writeBlock_continue(SD_addr, SD_buff2);
			SD_buff2_ready = 0;
			SD_addr = SD_addr+1;
			ioport_set_pin_low(LED3_GPIO);
			
			/*
			delay_us(100);
			for (j = 0; j<16; j++)	//send 20 times
			{
				ioport_set_pin_high(LED2_GPIO);
				for (k = 0; k<32; k++)
				{
					TX_BUF[k] = SD_buff2[j*32 + k];
				}
				nrf24l01_TX_config_master(TX_BUF);
				Check_ACK_master(1);
				ioport_set_pin_low(LED2_GPIO);
				delay_us(20);
			}		
			*/		
		}
	}
}


ISR(PORTF_INT0_vect)
{		

}

ISR(ADCA_CH3_vect)
{
	ioport_set_pin_high(LED1_GPIO);
	
	ADC_result0 = ADC_ResultCh_GetWord_Signed(&ADCA.CH0, 0);
	ADC_result1 = ADC_ResultCh_GetWord_Signed(&ADCA.CH1, 0);
	ADC_result2 = ADC_ResultCh_GetWord_Signed(&ADCA.CH2, 0);
	ADC_result3 = ADC_ResultCh_GetWord_Signed(&ADCA.CH3, 0);	
	//ADC_result1 = 0;
	//ADC_result2 = 0;
	//ADC_result3 = 0;

	if (SD_counter < 64)
	{
		SD_buff1[8*SD_counter] = ADC_result0 >> 8;
		SD_buff1[8*SD_counter+1] = ADC_result0;
		SD_buff1[8*SD_counter+2] = ADC_result1 >> 8;
		SD_buff1[8*SD_counter+3] = ADC_result1;
		SD_buff1[8*SD_counter+4] = ADC_result2 >> 8;
		SD_buff1[8*SD_counter+5] = ADC_result2;
		SD_buff1[8*SD_counter+6] = ADC_result3 >> 8;
		SD_buff1[8*SD_counter+7] = ADC_result3;
	}
	else if ((SD_counter < 128) && (SD_counter > 63))
	{
		SD_buff2[8*SD_counter - 512] = ADC_result0 >> 8;
		SD_buff2[8*SD_counter - 511] = ADC_result0;
		SD_buff2[8*SD_counter - 510] = ADC_result1 >> 8;
		SD_buff2[8*SD_counter - 509] = ADC_result1;
		SD_buff2[8*SD_counter - 508] = ADC_result2 >> 8;
		SD_buff2[8*SD_counter - 507] = ADC_result2;
		SD_buff2[8*SD_counter - 506] = ADC_result3 >> 8;
		SD_buff2[8*SD_counter - 505] = ADC_result3;
	}
	
	SD_counter ++;
	
	if (SD_counter == 64)
	{
		SD_buff1_ready = 1;
	}
	
	else if (SD_counter == 128)
	{
		SD_buff2_ready = 1;
		SD_counter = 0;
	}
	ioport_set_pin_low(LED1_GPIO);		
	//ADC_result = ADC_result >> 4;
	//LEDPORT.OUT = ~ADC_result;
	//LEDPORT.OUT = ~LEDPORT.OUT;
	//PORTB.OUT = ~PORTB.OUT;
}