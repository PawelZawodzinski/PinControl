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


 //----------------------------- Obsługa rolety ----------------------------//

RollerBlind::RollerBlind(uint8_t pinUp, uint8_t pinDown, unsigned long moveTime){
	_status = 0b11000100;		// ustawia status na była podnoszona, roleta po zainiciowaniu będzie opuszczana
	_pinUp = pinUp;
	_pinDown = pinDown;
	_time = moveTime;
	//expanderWrite(_pinUp,0);	// dla pewności wyłacza ruch w obu kierunkach
	//expanderWrite(_pinDown,0);	// dla pewności wyłacza ruch w obu kierunkach
}

void RollerBlind::goUp(){
	/*
	 *  1100 0000 - GÓRA 
     *  1100 0001 - przygotowanie do przejścia w górę
     *  1100 0011 - ruch w górę
     * 	1100 0100 - roleta zatrzymana była podnoszona
     */
    _status = 0b11000001;
    _moveTime = millis() + _switchTime;
    
    expanderWrite(_pinUp,0);	// dla pewności wyłacza ruch w obu kierunkach
	expanderWrite(_pinDown,0);	// dla pewności wyłacza ruch w obu kierunkach
}

void RollerBlind::goDown(){
	/*
	 *  1000 0000 - DÓŁ 
     *  1000 0001 - przygotowanie do przejścia w dół
     *  1000 0011 - ruch w dół
     *  1000 0100 - roleta zatrzymana była opuszczana
     */
    _status = 0b10000001;
    _moveTime = millis() + _switchTime; 
    
    expanderWrite(_pinUp,0);	// dla pewności wyłacza ruch w obu kierunkach
	expanderWrite(_pinDown,0);	// dla pewności wyłacza ruch w obu kierunkach
}

void RollerBlind::goOpposite(){
	/*
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
     */ 
	if ((_status & 0b11000000) == 0b11000000){	// ruch był w górę przygotowanie do przejścia w dół 
		RollerBlind::goDown();
		return;		
	}
	if ((_status & 0b11000000) == 0b10000000){	// ruch był w dół przygotowanie do przejścia w górę 
		RollerBlind::goUp();
		return;
	}
}

void RollerBlind::stop(){
	// najpierw zatrzymaj ruch
	expanderWrite(_pinUp,0);
	expanderWrite(_pinDown,0);	
	
	if (_status & 0b11000000 == 0b10000000){ 	// maska wyciąga kierunek ruchu rolety
		_status = 0b10000100; 					// ustala jaki był kierunek ruchu
	}
	
	if (_status & 0b11000000 == 0b11000000){	// maska wyciąga kierunek ruchu rolety
		_status = 0b11000100;					// ustala jaki był kierunek ruchu
	}
}

uint8_t RollerBlind::getStatus(){
	return _status;
}

void RollerBlind::loop(){
	
	// 0 - roleta zatrzymana L1 = 0 L2 = 0
    // 1 - przygotowanie do przejścia na podnoszenie L1 = 0 L2 = 0
    // 2 - podnoszenie L1 = 1 L2 = 0
    // 3 - ZATRZYMANIE PO PODNOSZENIU USTAWIA STAN 100
    // 100 - ROLETA BYŁA PODNOSZONA 
    // 4 - przygotowanie do przejścia na opuszczanie L1 = 0 L2 = 0
    // 5 - opuszczanie L1 = 0 L2 = 1
    // 6 - ZATRZYMANIE PO OPUSZCZENIU USTAWIA STAN 200
    // 200 - ROLETA BYŁA OPUSZCZANA 
    
    // 10 - zatrzymanie - ROLETA BYŁA PODNOSZONA
    // 20 - zatrzymanie - ROLETA BYŁA OPUSZCZANA  
    /*
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
     */ 
	
	if(_status & 0b10000001 == 0b10000001){ 	// sprawdza czy zaplanowany jest ruch w kturymś kierunku
		
		if(millis() > _moveTime){ 				// odczekuje pomiędzy przełączeniami czas _switchTime 
												// jeśli ustawiono ruch w którymś kierunku dodatkowo oczekuje czas _time
			
			if(_status == 0b10000011){			// jeśli roleta była opuszczona zostanie zatrzymana
				_status = 0b10000100;			// ustawia status rolety na była opuszczana
				expanderWrite(_pinUp,0);
				expanderWrite(_pinDown,0);
				return;
			}
			
			if(_status == 0b10000001){			// roleta ma iść w dół			
				_status = 0b10000011;
				expanderWrite(_pinUp,0);
				expanderWrite(_pinDown,1);	
				_moveTime = millis() + _time;	// ustawia czas jak długo ma trwać ruch
				return;
			}
			
			if(_status == 0b11000011){			// jeśli roleta była podnoszona zostanie zatrzymana
				_status = 0b11000100;			// ustawia status rolety na była podnoszona
				expanderWrite(_pinUp,0);
				expanderWrite(_pinDown,0);
				return;
			}
			
			if(_status == 0b11000001){			// roleta ma iść w górę			
				_status = 0b11000011;
				expanderWrite(_pinDown,0);
				expanderWrite(_pinUp,1);					
				_moveTime = millis() + _time;	// ustawia czas jak długo ma trwać ruch
				return;
			}
			
		}
		
	}
	
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

void PwmGoTo::LevelUp(){
	if(_gotoLevel < 253){
		_gotoLevel = _gotoLevel + 2;
	}else{
		_gotoLevel = _gotoLevel = 255;
	}
}

void PwmGoTo::LevelDown(){
	if(_gotoLevel > 2){
		_gotoLevel = _gotoLevel - 2;
	}else{
		_gotoLevel = _gotoLevel = 0;
	}
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

 //----------------------------- LevelChange ----------------------------//

LevelChange::LevelChange(uint8_t pin ,uint8_t* levels,uint8_t size){
	_pin = pin;
	_levels = levels;
	_size = size-1;	
	_level = 0;
}
uint8_t LevelChange::ChangeUp(){	
	//Serial.print("level "); Serial.println(_level);
	//Serial.print("size  "); Serial.println(_size);
	if(_level < _size){
		_level++;		
	}
	else
	{
		_level = 0;
	}
	expanderAnalogWrite(_pin, _levels[_level]);
	return _levels[_level];
}
uint8_t LevelChange::ChangeDown(){
	if(_level > 0 ){
		_level--;		
	}
	else
	{
		_level = _size;
	}
	expanderAnalogWrite(_pin, _levels[_level]);
	return _levels[_level];	
}

void LevelChange::SetOff(){
	expanderAnalogWrite(_pin, 0);
}

