#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

/* ############################## INFORMATION ############################
	
	  The 'display' term means a set of four digits mounted in one package. In this voltmeter there are two displays and eight digits
	  This means, that despite the fact that the device has four separate channels (so it can be connected to four different voltages) it is only
	  possible to show two voltages at one time. Switching between readings to be shown depends
	  on setting of the switches.

   ####################################################################### */



/*
	____________________
	| 1 || 2 || 3 || 4 | <--- display nr 1 
	___________________
	
	____________________
	| 5|| 6 || 7 || 8 |	 <--- display nr 2
    ___________________
	
	The 'dispIndex' 8-bit integer, used below, represents which of eight digits is on. 
	
	The binary representation shows it much clearer: 
	when the dispIndex == 8 (0b00010000), digit number 4 is ON. When it's equal to 64 (0b00000010), digit number 7 is ON. And so on...

*/
 
#define PCF8574ADR 0x40 //The older nibble contains a built-in device's address. The younger one consists of 3 user defined bits (set to zero in this case) and an R/W bit.
#define PCF8591ADR 0x90

#define REFERENCEVOLTAGE 2560       // A reference voltage of 2.56v 

#define CH0_DIV_FACTOR 40	        // 'Channel 0 division factor'. 
#define CH1_DIV_FACTOR 40
#define CH2_DIV_FACTOR 20
#define CH3_DIV_FACTOR 20

uint8_t displayChar(char charToDisplay, uint8_t (*I2C_PCF8574)(uint8_t deviceAdr, uint8_t dispIndex), uint8_t dispIndex)
	/* Displays one char (charToDisplay) on a desired display selected by dispIndex. The switch-case instructions may be
	   changed regarding to the pinout of 7-seg. display.                                                                   */

{	
		I2C_PCF8574(PCF8574ADR, dispIndex); // Turns on the desired display.
		
		switch(charToDisplay)
		{
			case 0:
			PORTB = ((~( _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2) );
			break;
			
			case 1:
			PORTB = ((~(_BV(PB3) | _BV(PB4))) & 0b00111111)  | (PORTB & 0b11000000);
			PORTC = (_BV(PC0) | _BV(PC1) | _BV(PC2));
			break;
			
			case 2:
			PORTB = ((~( _BV(PB2) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC2) );
			break;
			
			case 3:
			PORTB = ((~( _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) );
			break;
			
			case 4:
			PORTB = ((~( _BV(PB3) | _BV(PB2) | _BV(PB4))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC1) );
			break;
			
			case 5:
			PORTB = ((~( _BV(PB2) | _BV(PB3)  | _BV(PB5))) & 0b00111111 ) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) );
			break;
			
			case 6:
			PORTB = ((~( _BV(PB2) | _BV(PB3)  | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2) );
			break;
			
			case 7:
			PORTB = ((~(_BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = (_BV(PC1) | _BV(PC0) | _BV(PC2));
			break;
			
			case 8:
			PORTB = ((~( _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2) );
			break;
			
			case 9:
			PORTB = ((~( _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1)  );
			break;
			
			case 'E':
			PORTB = (~( _BV(PB2) | _BV(PB5)) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2));
			
		}
		return 0;
}

//CONSIDER REBULIDING THE display4DigitNumber TO USE THE POINTER FOR displayChar FUNCION 
uint8_t display4DigitNumber(uint8_t *finalValue, uint8_t (*I2C_PCF8574)(uint8_t deviceAdr, uint8_t dispIndex), uint8_t loopAux_0)
{
	uint8_t loopAux_1, dispIndex;
	
	if(loopAux_0 == 0) dispIndex = 1; // These two lines are responsible for observing which display is being edited (nr 1 or nr 2). 
	else dispIndex = 16;			  // ## See line 17 for a more detailed description ##
	
	 
	
	for(loopAux_1 = 0, dispIndex; loopAux_1 < 4 ; loopAux_1++, dispIndex = dispIndex*2)
	{
		
		I2C_PCF8574(PCF8574ADR, dispIndex); // Turns on the desired display.
		
		if(dispIndex == 2 || dispIndex == 32)
		{
			PORTB = (~( _BV(PB1)) & 0b00111111) | (PORTB & 0b11000000);
			_delay_us(500);
		}
	
		switch(finalValue[loopAux_1])
		{
			case 0:
			PORTB = ((~( _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000); 
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2) );
			break;
			
			case 1:
			PORTB = ((~(_BV(PB3) | _BV(PB4))) & 0b00111111)  | (PORTB & 0b11000000);
			PORTC = (_BV(PC0) | _BV(PC1) | _BV(PC2));
			break;
			
			case 2:
			PORTB = ((~( _BV(PB2) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC2) );
			break;
			
			case 3:
			PORTB = ((~( _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) );
			break;
			
			case 4:
			PORTB = ((~( _BV(PB3) | _BV(PB2) | _BV(PB4))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC1) );
			break;
			
			case 5:
			PORTB = ((~( _BV(PB2) | _BV(PB3)  | _BV(PB5))) & 0b00111111 ) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) );
			break;
			
			case 6:
			PORTB = ((~( _BV(PB2) | _BV(PB3)  | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2) );
			break;
			
			case 7:
			PORTB = ((~(_BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = (_BV(PC1) | _BV(PC0) | _BV(PC2));
			break;
			
			case 8:
			PORTB = ((~( _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1) | _BV(PC2) );
			break;
			
			case 9:
			PORTB = ((~( _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5))) & 0b00111111) | (PORTB & 0b11000000);
			PORTC = ~( _BV(PC0) | _BV(PC1)  );
			break;
		}
		_delay_ms(2);
		PORTB |= (~(_BV(PB6) | _BV(PB7)) ) & 0b11111111;
		PORTC = (_BV(PC0) | _BV(PC1) | _BV(PC2));
	}
	
	return 0;
}  

void binaryToVoltageConverter(uint8_t dataPacket, uint8_t channelSelector, uint8_t *finalValue) 
	/* Turns a byte of data (dataPacket) into array of chars making them easier to process.
	   Function uses channelSelector to decide which division factor needs to be used for the 
	   appropriate reading. DecimalPointShifter allows to avoid computing float type numbers.
	   The position of a decimal point on the displays doesn't change.		 */


{
	uint8_t currentDivFactor = 1;
	uint8_t decimalPointShifter = 100; // Used to avoid calculating float type integers 
	
	switch(channelSelector) // Sets a proper division factor (currentDivFactor) depending on the channel being processed.
		{
			case 0x00:
			currentDivFactor = CH0_DIV_FACTOR;
			break;
				
			case 0x01:
			currentDivFactor = CH1_DIV_FACTOR;
			break;
			
			case 0x02:
			currentDivFactor = CH2_DIV_FACTOR;
			break;
			
			case 0x03:
			currentDivFactor = CH3_DIV_FACTOR;
			break;
		}
	 
	
	uint32_t resultVoltage = (  ( dataPacket*decimalPointShifter  / 255UL ) * REFERENCEVOLTAGE * currentDivFactor ); 
								/*	-datapacket = 1 byte of data received from a specific channel of ADC. 
									-decimalPointShifter = a multiplier of a value of 100 which shifts the 
									 decimal point by 2 places to the right, preventing from calculating 
									 float type numbers.
									-deviding by 256, which is the decimal representation of an 8bit ADC resolution
									-REFERENCEVOLTAGE = a stable voltage delivered to the ADC by hardware
									-currentDivFactor = a factor used to calculate voltage for different voltage ranges.
								*/
	uint8_t loop;
	uint32_t factor;
	
	for(loop = 0, factor = 10000000; loop < 4 ; loop++, resultVoltage = (resultVoltage % factor), factor = factor/10 )
	{	
		finalValue[loop] = resultVoltage/factor; // separates digits from a number (e.g. 2351 --> {2,3,5,1})
	}
}
