/*
  PinControl.h - Library for drive pin PinControl code.
  Created by Paweł Zawodziński, September 17, 2017.
  Released into the public domain.
*/

#include "Arduino.h"
#include "PinExpander.h"
#include <Wire.h>

uint8_t MCP23017_COUNTER;
uint8_t PCA9635_COUNTER;
uint8_t pinState[8];
uint8_t pinDir[8];

void addExpander(EXPANDER_TYPE type, uint8_t portADir, uint8_t portBDir){
	if (type == MCP23017){
		if (MCP23017_COUNTER < 7){
		// portXDir 1 - in ; 0 - out
		Wire.begin();
		Wire.beginTransmission(MCP23017_ADRESS | MCP23017_COUNTER);
		Wire.write(MCP23017_IODIRA); 									// IODIRA register
		Wire.write(portADir); 											// set all of bank A 
		Wire.endTransmission();
		pinDir[MCP23017_COUNTER*2] = portADir;							// zapisanie portu do tablicy ; MCP23017_COUNTER = 0 * 2 = rejestr 0 
		
		Wire.begin();
		Wire.beginTransmission(MCP23017_ADRESS | MCP23017_COUNTER);
		Wire.write(MCP23017_IODIRB); 									// IODIRB register
		Wire.write(portBDir); 											// set all of port B 
		Wire.endTransmission();
		pinDir[(MCP23017_COUNTER*2)+1] = portBDir;						// zapisanie portu do tablicy ; MCP23017_COUNTER = ( 0 * 2 ) + 1 = rejestr 1 		
		
		// MCP23017_COUNTER = 0 ;porty pinDir 0,1 (0*2 , 0*2+1)
		// MCP23017_COUNTER = 1 ;porty pinDir 2,3 (1*2 , 1*2+1)
		// MCP23017_COUNTER = 2 ;porty pinDir 4,5 (2*2 , 2*2+1)
		
		// set state to off
		Wire.beginTransmission(MCP23017_ADRESS | MCP23017_COUNTER);
	    Wire.write(MCP23017_GPIOA);
	    Wire.write(0b0);  						// value to send 
        Wire.endTransmission();
		// set state to off
		Wire.beginTransmission(MCP23017_ADRESS | MCP23017_COUNTER);
	    Wire.write(MCP23017_GPIOB);
	    Wire.write(0b0);  						// value to send 
        Wire.endTransmission();
        
        MCP23017_COUNTER ++;
		}
	}
	
	if (type == PCA9635){
		if (PCA9635_COUNTER < 7){
		Wire.begin();
		_send_byte(PCA9635_ADRESS | PCA9635_COUNTER ,0x00 , 0xc1);
		_send_byte(PCA9635_ADRESS | PCA9635_COUNTER ,0x01 , 0x15);
		_send_byte(PCA9635_ADRESS | PCA9635_COUNTER ,0x14 , 0xff);
		_send_byte(PCA9635_ADRESS | PCA9635_COUNTER ,0x15 , 0xff);
		_send_byte(PCA9635_ADRESS | PCA9635_COUNTER ,0x16 , 0xff);
		_send_byte(PCA9635_ADRESS | PCA9635_COUNTER ,0x17 , 0xff);
        
        Wire.beginTransmission(PCA9635_ADRESS | PCA9635_COUNTER);
		Wire.write((0x02) | 0x80); // first adress + autoincrement on
		for(uint8_t i = 0; i < 16; i++)
		{
			Wire.write(0);
		}
		Wire.endTransmission();
        
        PCA9635_COUNTER ++;
		}
	}
}

void _send_byte(uint8_t device, uint8_t adres, uint8_t data)
{
    Wire.beginTransmission(device);
    Wire.write(adres); //
    Wire.write(data); //
    Wire.endTransmission();
}

void expanderReadLoop(){
	// Serial.print ("kontroler : ");
	// Serial.println (pin>>4);
	//
	// Serial.print ("Port A/B : ");
	// Serial.println ((pin >> 3)  & 0x01);
 
    uint8_t port = 0;
    for(uint8_t mcp = 0 ; mcp < MCP23017_COUNTER ; mcp++){
		
		Wire.beginTransmission(MCP23017_ADRESS | mcp);	
        Wire.write(MCP23017_GPIOA);                		// address PORT B
        Wire.endTransmission();
        Wire.requestFrom(MCP23017_ADRESS | mcp, 1);     // request one byte of data
        pinState[port++]=Wire.read();                 	// store incoming byte into "input"
        
        Wire.beginTransmission(MCP23017_ADRESS | mcp);	
        Wire.write(MCP23017_GPIOB);                		// address PORT B
        Wire.endTransmission();
        Wire.requestFrom(MCP23017_ADRESS | mcp, 1);     // request one byte of data
        pinState[port++]=Wire.read();                 	// store incoming byte into "input"
	}	
}

uint8_t expanderRead(uint8_t pin){
	// [pin>>3] 									- wyciąga w której tablicy jest wynik
	// pinState[pin>>3]								- wyciąga wartośc z tablicy
	// pin & 0b00000111								- maska wyciąga o który pin portu chodzi 0 do 7
	// (pinState[pin>>3] >> (pin & 0b00000111)) 	- z tablicy 11111111 przsuwa interesujący pin na koniec 
	// & 0b00000001									- nakłada maskę wyciagając ostatni pin
	
	return (pinState[pin>>3] >> ((pin & 0b00000111))) & 0b00000001;
}

void expanderWrite(uint8_t pin, uint8_t state){
	// (pin & 0b00000111)							- wyciąga numer portu od 0 do 7
	// (0b1 << (pin & 0b00000111))					- ustala 1 na odpowiednie miejsce w łańcuchu  
	
	// Set bit: num = num | (1 << i)
	// Clear bit : num = num & ~(1 << i)
	// Toggle bit: num = num ^ (1 << i)
	// Get bit: num = (num >> i) & 1
	
	uint8_t localTab = pin >> 3;
	
	if (!(pinDir[localTab] & (0b00000001 << (pin & 0b00000111)))){
	    Wire.beginTransmission(MCP23017_ADRESS | (pin>>4));
	    
	    if(((localTab)  & 0x01) == 0){
			Wire.write(MCP23017_GPIOA);
		}else{
			Wire.write(MCP23017_GPIOB);
		}
		
		if (state){
			pinState[localTab] = pinState[localTab] | (1 << (pin & 0b00000111));
		}else{
			pinState[localTab] = pinState[localTab] & ~(1 << (pin & 0b00000111));
		}
		
        Wire.write(pinState[localTab]);  						// value to send 
        Wire.endTransmission();
	}
}

void expanderAnalogWrite (uint8_t pin, uint8_t level){
	_send_byte(PCA9635_ADRESS | (pin>>4), (pin & 0b00001111) + 2, level);
}
