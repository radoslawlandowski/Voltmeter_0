
#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <i2cmaster.h>

/*  Function takes an address of a device and sends the data to it. */


uint8_t I2C_PCF8574(uint8_t deviceAdr, uint8_t dispIndex)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) != TW_START)
	{
		return TWSR & 0xF8; 
	}
	
	TWDR = deviceAdr;	 //Load the pcf8574 adress to TWDR
	
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) !=TW_MT_SLA_ACK)
	{
		return TWSR & 0xF8;
	}
	
	TWDR = ~dispIndex; // dispIndex is a byte of data to configure outputs of the expander. Negation of this byte is caused by
					   // the usage of pnp transistors on the board
	
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) !=TW_MT_DATA_ACK)
	{
		return TWSR & 0xF8;
	}
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	return 0;
	 
}

void I2C_PCF8591(uint8_t deviceAdr, uint8_t controlByte, uint8_t *dataPacket)
{
	
	 i2c_init();       

	 i2c_start_wait(deviceAdr+I2C_WRITE);    
	 i2c_write(controlByte);                                      
	 i2c_stop();
	                           
	 i2c_rep_start(deviceAdr+I2C_READ);
	 _delay_us(300);
	 
	 dataPacket[3] = i2c_readAck(); // Since each transmission delivers a previously done reading, I assign the 4th place of the array in the beginning.
	 
	 uint8_t aux;
	 for(aux = 0; aux < 3 ; aux++)
	 {
		 
		 if(aux == 2)
		 {
			 _delay_us(300);
			 dataPacket[2] = i2c_readNak();
		 }
								  // the transmission will be ended by sending NOTACKNOWLEDGE to ADC.
		 else
		 {	
			 _delay_us(300);
			 dataPacket[aux] = i2c_readAck();
		 }
	 }              
	 
	 i2c_stop();
}
