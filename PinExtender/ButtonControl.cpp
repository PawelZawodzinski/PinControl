/*
  PinControl.h - Library for drive pin PinControl code.
  Created by Paweł Zawodziński, September 17, 2017.
  Released into the public domain.
*/


#include "Arduino.h"
#include "ButtonControl.h" 
#include <PinExpander.h>


 //----------------------------- Button ----------------------------//

SimpleButton::SimpleButton(uint8_t pin){
	_pin = pin;
	_time = 0;
}
 
/* Sprawdza czy pin jest wciśnięty 
 * Jeśli czas _time przekroczy wartość ustawioną w _bounceDelay
 * funkcja zwracja 1. Przy kolejnych przejściach pentli zwruci 0.
 * return Funkcja zwruci 1 jeśli czas bounceDelay zruwna się z _time po tym bedzie zwracać 0 mimo że przycisk jest wcisnięty
 */
uint8_t SimpleButton::buttonPressed(){
	if (expanderRead(_pin) == 1 ){
		if(_time<ULONG_MAX)
		{
			_time++;
		}
	}else {_time=0;}	

	// oszczędny zapis 
	if (_time == BDELAY){		
		return 1;
	}
	return 0;
}

 //----------------------------- ButtonStatus ----------------------------//
 
ButtonStatus::ButtonStatus(uint8_t pin){
	_pin = pin;
	_time = 0;
}
/* Zwykły debounce zwraca 1 tak długo jak przycisk jest wciśnięty
 * Po przekroczeniu BDELAY zaczyna zwracać 1
 */
uint8_t ButtonStatus::checkStatus(){

	if (expanderRead(_pin) == 1 ){
		if(_time<ULONG_MAX)
		{
			_time++;
		}
	}
	else {_time=0;}	

	// oszczędny zapis 
	if (_time > BDELAY){
		_time=BDELAY+1;		
		return 1;
	}	
	return 0;
}


 //----------------------------- Button Loop ----------------------------//
 
 ButtonLoop::ButtonLoop(uint8_t pin){
	_pin = pin;
	_time = 0;
}
 
/* Sprawdza czy pin jest wciśnięty.
 * Jeśi czas _time przekroczy _bounceDelay oraz delay
 * funkcja zwruci jednorazowo 1. Przy następnych przejściach pentli fukkcja zwrui 1
 * jeśli zmienna _delayLoop przekroczy wartość delay, po tym _delayLoop = 0
 * return Będzie zwracać 1 co okreslony czas "unsigned long delay" tak długo jak wciśnięty będzie przycisk
 */
uint8_t ButtonLoop::buttonPressedLoop(unsigned long delay){
	if (expanderRead(_pin) == 1 ){
		if(_time<ULONG_MAX)
		{
			_time++;
		} 
		if(_delayLoop<ULONG_MAX)
		{
			_delayLoop++;
		}

	}else {_time=0; _delayLoop=0;}	

	if (_time > BDELAY) {
		if (_delayLoop > delay){		
		_delayLoop = 0;
		return 1;
		}				
	}
	return 0;		
}

 //----------------------------- Button Counter ----------------------------//

ButtonCounter::ButtonCounter(uint8_t pin){
	_pin=pin;
	_pinState=0;
	_longPushFlag=0;
    _delayLoop=0;
    _time=0;  
}

/* Zlicza ilość przyciśnięcia guzika
 * Jeśi czas _time przekroczy _bounceDelay do _pinState doda jedno wciśnięcie guzika
 * Przy zwolnieniu guzika czas _time zliczany jest od nowa po przekroczeniu _bounceDelay dodane jest kolejne wciśnięcie guzika
 * Jeśli po ostatnim zakwalifikowanym wciśnięciu czas _delayLoop przekroczy delay -1 (_delayLoop == delay -1) funkcja zwruci zliczoną liczbe wciśnięć 
 * Przy następnym przejściu delayLoop == delay stan _pinState zostanie wyczyszczony
 * return Bedzie zwracać liczbę przyciśnięć guzika w serii
 */
uint8_t ButtonCounter::buttonPressedCounter(unsigned long delay){
	return  ButtonCounter::buttonPressedCounter( delay,false);
}
uint8_t ButtonCounter::buttonPressedCounter(unsigned long delay,bool ignoreFlag){
	
	if (expanderRead(_pin) == 1 ){
		if(_time<ULONG_MAX)
		{
			_time++;
		} 
	 _delayLoop = 0;
	}else {
		_time=0; 
		if(_delayLoop<ULONG_MAX)
		{
			_delayLoop++;
		} 

		if(_longPushFlag > 0){					 // obsługa długiego przytrzymania guzika
			_pinState = 0;
			_time = 0;
			_longPushFlag = 0;
			return 0;
			}		
		}
	
	if (_time == BDELAY)
	{
		if(_pinState<UINT8_MAX)
		{
			_pinState++;
		}
	}
	
	if (_time > delay << 2 && (_longPushFlag == 0 || ignoreFlag)){ // długie przyciśnięcie guzika zwraca 200		
		_longPushFlag = 1;
		return 200;
	}
	
	//po przekroczeniu delay sprawdza jednego if zamiast dwa
	if (_delayLoop <= delay && _longPushFlag == 0)
	{
		if(_delayLoop == delay){
			_pinState = 0;
		}
		if(_delayLoop == delay -1){
			return _pinState;
		}
	}	
	return 0;
}

 //----------------------------- PWM GoTo ----------------------------//

PwmGoTo::PwmGoTo(uint8_t pin){
	_pin = pin;
	_delay = 0;
}

void PwmGoTo::SetLevel(uint8_t level){	
	_gotoLevel = level;
	_currentLevel = level;
	expanderAnalogWrite(_pin, level);
}

void PwmGoTo::GoTo(uint8_t level){
	_gotoLevel = level;
}

uint8_t PwmGoTo::GetCurrentLevel(){
	return _currentLevel;
}

uint8_t PwmGoTo::GetGoToLevel(){
	return _gotoLevel;
}

void PwmGoTo::PwmLoop(uint8_t delay){
	if(_gotoLevel != _currentLevel){		
		if(_delay<UINT8_MAX)
		{
			_delay++;
		}
		if (_delay == delay){
			_delay = 0;		
			if (_gotoLevel > _currentLevel){
				if(_currentLevel<UINT8_MAX)		
				{	
					expanderAnalogWrite(_pin, _currentLevel++);
				}
				else
				{
					expanderAnalogWrite(_pin, _currentLevel);
				}
			}else{
				if(_currentLevel>UINT8_MIN)		
				{	
					expanderAnalogWrite(_pin, _currentLevel--);
				}
				else
				{
					expanderAnalogWrite(_pin, _currentLevel);
				}
			}
		}
	}
}
