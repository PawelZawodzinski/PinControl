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
class RollerBlind;
class PwmGoTo;

/*
typedef enum ROLLERBIND_STATE{
	 *
     *  0000 0000 - STOP
     *  
     *  1000 0000 - DÓŁ 
     *  1000 0001 - przygotowanie do przejścia w dół
     *  1000 0011 - ruch w dół
     *  1000 0100 - roleta zatrzymana była opuszczana
     * 
     *  1100 0000 - GÓRA 
     *  1100 0001 - przygotowanie do przejścia w górę
     *  1100 0011 - ruch w górę
     * 	1100 0100 - roleta zatrzymana była podnoszona
     *
	
	STOP 		= 0b00000000,

	WILLGODOWN	= 0b10000001, 	
	GODOWN		= 0b10000011,	 	
	WNETDOWN	= 0b10000100, 
	
	WILLGOUP	= 0b11000001,
	GOUP		= 0b11000011,
	WENTUP		= 0b11000100,	
	
	};
*/

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

class RollerBlind {
	public:
		RollerBlind(uint8_t pinUp, uint8_t pinDown, unsigned long time);
		void goUp();
		void goDown();
		void goOpposite();
		void stop();
		void loop();	
		uint8_t getStatus();
	private:
		uint8_t _status;
		uint8_t _pinUp;
		uint8_t _pinDown;
		unsigned long _time;
		unsigned long _moveTime;	
		unsigned long _switchTime = 500;
};

class PwmGoTo {
	public:
		PwmGoTo(uint8_t pin);
		void SetLevel(uint8_t level);	
		void PwmLoop(uint8_t delay);
		void GoTo(uint8_t level);
		uint8_t GetCurrentLevel();
		uint8_t GetGoToLevel();
		void LevelDown();
		void LevelUp();
	private:
		uint8_t _pin;
		uint8_t _delay;
		uint8_t _currentLevel;
		uint8_t _gotoLevel;
};	

class LevelChange {
	public:
		LevelChange(uint8_t pin ,uint8_t* levels,uint8_t size);
		uint8_t ChangeUp();
		uint8_t ChangeDown();
		void SetOff();
	private:
		uint8_t _size;
		uint8_t _level;
		uint8_t _pin;
		uint8_t* _levels;
	
};

#endif
