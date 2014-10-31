/*
 * TestowanieNaPlytce.c
 *
 * Created: 2014-05-06 16:40:19
 *  Author: Radek
 */ 


#include <avr/io.h>
#include <display.h>
#include <I2Cmoja.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

#define BUTTON_ONE 0b00000001 // Monostable buttons
#define BUTTON_TWO 0b10000000
#define BUTTON_THREE 0b01000000 
							    
#define LED_0 0b00010000 //PD4 LED's for indicating which reading is being shown on the display
#define LED_1 0b00100000 //PD5
#define LED_2 0b01000000 //PB6  
#define LED_3 0b10000000 //PB7

#define LED_ON 0b00001000 //PD3
#define LED_USART 0b00000100 //PD2
										 
int main(void)
{
	DDRB = 0b11111110;
	PORTB = 0b00000001;
	
	DDRC = 0xff;
	
	DDRD = 0b00111111;
	PORTD = 0b11000000;
	
	PORTD |= LED_ON;
	
	const uint8_t PCF8591_CONTROL_BYTE = 0b00000100;		/* Initial control byte configuration. Starting from LSB:
																- 0,1 = combination of both determines the choose of the input to be read. 
																- 2   = auto-increment flag.
																- 3   = according to the datasheet, always zeroed
																- 4,5 = combination of those determines: single-ended or differential inputs.
																		In this app - always zeroed.
																- 6   = enables the analog output of the converter which isn't used here. Always zeroed.
																- 7   = according to the datasheet, always zeroed
														    */		

   while(1)
    {		
		uint8_t selector = 0;  // determines an ADC channel to be read. Depends on the state of BUTTON_ONE and BUTTON_TWO and loopAux_0
		uint8_t loopAux_0;
		
		for(loopAux_0 = 0 ; loopAux_0 < 2 ; loopAux_0++) //This sub-main loop does two runs because there are two displays(1) to handle.
		{											     // (1)---> The 'display' term is understood as a set of four 7-segment digits put into one package.
			switch(loopAux_0)
			{
				case 0:
					if(PINB & BUTTON_ONE)
					{	
						selector = 0; 
						PORTD |= LED_0;
						PORTD &= ~LED_1;
					}
						
					else
					{
						selector = 1; 
						PORTD |= LED_1;
						PORTD &= ~LED_0;
					}
					
					break;
				
				case 1:
					if(PIND & BUTTON_TWO)
					{
						selector = 2;
						PORTB |= LED_2;
						PORTB &= ~LED_3;
					}
					else
					{
						selector = 3;
						PORTB |= LED_3;
						PORTB &= ~LED_2;
					}
					
					break;
			}
			
			uint8_t dataPacket[4] = {0};
			uint8_t finalValue[4] = {0};
			
			I2C_PCF8591(PCF8591ADR, PCF8591_CONTROL_BYTE, dataPacket); 
			
			if(dataPacket[selector]>253)
			{
				uint8_t digitForError = 1;
				if(loopAux_0 == 0) digitForError = 1;
				else digitForError = 16;
				displayChar('E', I2C_PCF8574, digitForError);
			}
			
			else
			{
				binaryToVoltageConverter(dataPacket[selector], selector, finalValue);
				
				display4DigitNumber(finalValue, I2C_PCF8574, loopAux_0);
			}
			
		}
    }
}