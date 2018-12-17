/*
  PinControl.h - Library for drive pin PinControl code.
  Created by Paweł Zawodziński, September 17, 2017.
  Released into the public domain.
*/
#ifndef ButtonControl_h
#define ButtonControl_h

#include "Arduino.h"
#define BDELAY 30
#define ULONG_MAX 4294967295
#define UINT8_MAX 255
#define UINT8_MIN 0

class SimpleButton;
class ButtonLoop;
class ButtonCounter;

/*
class ButtonControl {	
  public:	
	ButtonControl();
  private:

};
*/

class SimpleButton {	
  public:	
    SimpleButton(uint8_t pin);
    uint8_t buttonPressed();
  private:
	uint8_t _pin;
    unsigned long _time;   
};

class ButtonStatus{
	public:
		ButtonStatus(uint8_t pin);
		uint8_t checkStatus();
	private:
		uint8_t _pin;
		unsigned long _time;		
};

class ButtonLoop {	
  public:	
    ButtonLoop(uint8_t pin);
    uint8_t buttonPressedLoop(unsigned long delay);
  private:
	uint8_t _pin;
    unsigned long _delayLoop;
    unsigned long _time;   
};

class ButtonCounter {	
  public:	
	ButtonCounter(uint8_t pin);
    uint8_t buttonPressedCounter(unsigned long delay,bool ignoreFlag);
    uint8_t buttonPressedCounter(unsigned long delay);
    
  private:
	uint8_t _pin;
	uint8_t _pinState;
	uint8_t _longPushFlag;
    unsigned long _delayLoop;
    unsigned long _time;    
};

class PwmGoTo {
	public:
		PwmGoTo(uint8_t pin);
		void SetLevel(uint8_t level);	
		void PwmLoop(uint8_t delay);
		void GoTo(uint8_t level);
		uint8_t GetCurrentLevel();
		uint8_t GetGoToLevel();
	private:
		uint8_t _pin;
		uint8_t _delay;
		uint8_t _currentLevel;
		uint8_t _gotoLevel;
};	

#endif
