/*
  PinControl.h - Library for drive pin PinControl code.
  Created by Paweł Zawodziński, September 17, 2017.
  Released into the public domain.
*/
#ifndef PinExpander_h
#define PinExpander_h

#define MCP23017_ADRESS 0x20

// MCP23017 registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14


#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

//----------------------- PCA -----------------------//
#define PCA9635_ADRESS 0x18 // - na sztywno zwarte piny A4, A5

#include "Arduino.h"

typedef enum EXPANDER_TYPE {
	//   Pin defined:
	MCP23017 		= 0,
	PCA9635			= 1
};


extern "C"{
	void addExpander(EXPANDER_TYPE type, uint8_t portADir, uint8_t portBDir);
	void expanderReadLoop();
	uint8_t expanderRead(uint8_t pin);
	void expanderWrite(uint8_t pin, uint8_t state);
	void _send_byte(uint8_t device, uint8_t adres, uint8_t data);
	void expanderAnalogWrite (uint8_t pin, uint8_t level);
};	


#endif
