#include <Wire.h>
#include <PinExpander.h>
#include "ButtonControl.h"
#include "PCA9635.h"

#define BDELAY 50

SimpleButton b8(9,50);
ButtonCounter b1(24,BDELAY);
ButtonCounter b2(25,BDELAY);

void setup()
{    
   Serial.begin(115200);
   Serial.print ("Init ... "); 
   addExpander(MCP23017, 0, 255);
   addExpander(MCP23017, 0, 255); 

   addExpander(PCA9635, 0, 0); 
   expanderAnalogWrite(0,10);
   expanderAnalogWrite(1,20);
   expanderAnalogWrite(2,30);
   expanderAnalogWrite(3,40);
   expanderAnalogWrite(4,50);
   expanderAnalogWrite(5,60);
   expanderAnalogWrite(6,70);
   expanderAnalogWrite(7,80);
   expanderAnalogWrite(8,90);
   expanderAnalogWrite(9,100);
   expanderAnalogWrite(10,110);
   expanderAnalogWrite(11,120);
   expanderAnalogWrite(12,130);
   expanderAnalogWrite(13,140);
   expanderAnalogWrite(14,150);
   expanderAnalogWrite(15,160);
   
    
   delay(50);
   Serial.println("ok !");
}
 
byte wynik; 
byte dyskoteka;
int counter;

void loop()
{    
  
 // read();
  expanderReadLoop();
  counter++;
 // Serial.println(counter);
  
  if (b8.buttonPressed()){
    dyskoteka = dyskoteka ^ (1 << 0);
    Serial.print("Dyskoteka : ");Serial.println(dyskoteka);
    counter = 0;
    if(dyskoteka == 0){
     Serial.print("Dezaktywacja portow ... ");
     expanderWrite(0,0);
     expanderWrite(7,0);
     Serial.println("ok");
    }
  }
  
    
  
  if(dyskoteka){
    if(counter == 200){
     Serial.println(counter);
     Serial.print("Aktywacja portu 0 ... ");
     expanderWrite(0,1);
     Serial.println("ok");
    }
    if(counter == 400){
     Serial.print("Aktywacja portu 7 ... ");
     expanderWrite(7,1);
     Serial.println("ok");
    }
    if (counter == 1400){
     Serial.print("Dezaktywacja portow ... ");
     expanderWrite(0,0);
     expanderWrite(7,0);
     Serial.println("ok");
     counter = 0;
    }
  }

  
  
   wynik = b1.buttonPressedCounter(300);
   if(wynik){
     Serial.print("guzik 24 wcisniety : ");Serial.println(wynik);
   }
   
   if (wynik == 1){
     expanderWrite(22,1); // n-mosfet
     Serial.println("n-mosfet 1");
   }
   if (wynik == 2){
     expanderWrite(22,0); // n-mosfet
     Serial.println("n-mosfet 0");
   }
   
   wynik = b2.buttonPressedCounter(300);
   if(wynik){
     Serial.print("guzik 25 wcisniety : ");Serial.println(wynik);
   }
   
}
