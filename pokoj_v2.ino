#include <Wire.h>
#include <PinExpander.h>
#include "ButtonControl.h"
#define _OPUZNIENIE 10000

//SimpleButton b8(9,50);

//---- Przyciski -- //
ButtonCounter b_pokojDrzwi(2);
ButtonLoop b_opkojCiemniej(1); 
ButtonLoop b_opkojJasniej(0); // 0

ButtonCounter b_pokojLozko(3); 
ButtonCounter b_pokojBiurko(5);
ButtonCounter b_opkojJasniejCounter(0); // 0

ButtonStatus b_pokojOkno(4);
ButtonStatus b_pokojSzafa(6);
ButtonStatus b_pokojCzujnikRuchu(7);


//---- PWM -- //
PwmGoTo pwm_halogen(3);  //halogen
PwmGoTo pwm_led2(4);  //taśma
PwmGoTo pwm_biurko(0);
PwmGoTo pwm_lozko(1);
PwmGoTo pwm_szafa(2);

//--- ROLETA ----/
RollerBlind roleta(14,15,500000);

byte wynik = 0;
int timeDelay = 0;
int swiatoLevel = 1035;
int czujnikDelay = _OPUZNIENIE;
bool pasek = false;
//byte roleta;
long czas;

void setup()
{    
   Serial.begin(115200);
   Serial.print ("Init ... ");    
   czas = 0;
   delay(50);

   addExpander(MCP23017, 255, 0); // porty 0-7 wejścia, porty 8-15 wyjścia
   addExpander(PCA9635, 0, 0);    

   delay(100);
   
   expanderWrite(8,1); // włacza rekuperator
   expanderWrite(9,1); // włacza rekuperator - tryb 2
   
   Serial.println("ok !");
   Serial.println("Bulid : V2 19.01.2019");
   //Bulid : V2 23.12.2018 - wersja startowa
   //Bulid : V2 19.01.2019 
   //  - dodanie obsługi rolety
}



void loop()
{
  timeDelay++;
  expanderReadLoop();
  
  
  if(czujnikDelay > 1) // ustawia delay czujnika ruchu na 1, poziom 0 wyłącza czujnik
  czujnikDelay --;

// ---------------- Czujnik otwarcia szafy -----------------//  
  wynik = b_pokojSzafa.checkStatus();
  // zapis zapobiega wielokrotnemu wchodzeniu w if i ustawianiu poziomu światła
  if(wynik==1
    && pwm_szafa.GetGoToLevel() <= 0
    //&& swiatoLevel < 1000
    ){      
    pwm_szafa.GoTo(200);
  }
  if(wynik==0
    && pwm_szafa.GetGoToLevel() > 0
    //&& swiatoLevel < 1000
    ){      
    pwm_szafa.SetLevel(0);
  }

 
// ---------------- Czujnik ruchu -----------------//  
  wynik = b_pokojCzujnikRuchu.checkStatus();
  
  //Serial.println(wynik);
  // jeśli wykryje ruch
  // jeśli jest ciemniej niż 1000
  // jeśli opuźnienie jest na poziomie 1    
  // jesli światło w łóżku jest wyłączone 
  
  if(wynik==1
    && swiatoLevel > 1000
    &&(
        (pwm_halogen.GetGoToLevel()==0 && !pasek)
      ||(pwm_led2.GetGoToLevel()==0 && pasek))
    && czujnikDelay == 1
    && pwm_lozko.GetGoToLevel( ) == 0
    ){      
    pwm_halogen.GoTo(180);
    czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
  }
 
// ---------------- Wlącznik przy dżwiach -----------------//  
  
  wynik = b_pokojDrzwi.buttonPressedCounter(300);  
  
  //ROLETA W ruchu przycisk nie wylącza światła awaryjnie wyłącza rolete !
  if(wynik == 1 && (roleta.getStatus() & 0b00000001) == 0b00000001 ){ 
      roleta.stop();
      wynik = 0;
  } 
  
  if (wynik){
    //Serial.print("wynik drzwi : ");
    //Serial.println(wynik);
    if(wynik == 1){  // roleta > 100 roleta zatrzymana
      if(pwm_halogen.GetGoToLevel() > 0 || pwm_led2.GetGoToLevel() > 0){
        pwm_halogen.SetLevel(0);
        pwm_led2.SetLevel(0);
        czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
      }else{
        pwm_halogen.GoTo(180);
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }
    }   
    
    if (wynik == 2){
      wylonczSwiatla();
      czujnikDelay = _OPUZNIENIE;    // ustawia opuźnienie dla czujnika ruchu
    }  
 
    if (wynik == 200){               // długie przyciśnięcie gasi wszystkie światła i wylącza czujnik
      czujnikDelay = 0;
      wylonczSwiatla();
    }
    
// ---------------- Wlącznik przy dżwiach - obsługa rolety -----------------// 
    if (wynik == 4){
      roleta.goOpposite();
      wynik = 0;    
    }
  }
  
  
// ---------------- Wlącznik przy dżwiach - ciemniej -----------------// 

  wynik = b_opkojCiemniej.buttonPressedLoop(50);

  if( wynik == 1
    && (
    (pwm_halogen.GetCurrentLevel() > 9)
    ||(pwm_led2.GetCurrentLevel() >9)
    )
  )
  {
        if(pasek)
        {
          pwm_led2.LevelDown();          
        }
        else
        {
          pwm_halogen.LevelDown();    
        }
  } 
  
  
// ---------------- Wlącznik przy dżwiach - jaśniej -----------------// 

  wynik = b_opkojJasniejCounter.buttonPressedCounter(300);
  
  if (wynik == 2) {
    //Serial.print("Pasek ");
    pasek = !pasek;
    //Serial.println(pasek);
    if(pasek)
    {
      pwm_led2.GoTo(180);  
      pwm_halogen.GoTo(0);
      //Serial.print("halogeny off pasek : ");Serial.println(pasek);
    }
    else
    {
      pwm_halogen.GoTo(180);  
      pwm_led2.GoTo(0);  
      //Serial.print("halogeny on pasek : ");Serial.println(pasek);      
    }
    //Serial.println("#-------");
  }

  wynik = b_opkojJasniej.buttonPressedLoop(50);

   if( wynik == 1
      && (
           (pwm_halogen.GetCurrentLevel() < 255)
        || (pwm_led2.GetCurrentLevel() < 255)
        )
      && (
           (pwm_halogen.GetCurrentLevel() == pwm_halogen.GetGoToLevel())
        || (pwm_led2.GetCurrentLevel() == pwm_led2.GetGoToLevel())
        )
    )      
 {    
        if(pasek)
        {
          pwm_led2.LevelUp();
        }
        else
        {
          pwm_halogen.LevelUp();    
        }       
      //Serial.print("halogen GetGoToLevel ");Serial.println(pwm_halogen.GetGoToLevel());
      //Serial.print("pasek led GetGoToLevel ");Serial.println(pwm_led2.GetGoToLevel());
      //Serial.println("-----------------------------------------------------------------------------");
  }

   // --- obsługa paska LED --- //

 
  
// ---------------- Wlącznik biurko -----------------//  
  
  wynik = b_pokojBiurko.buttonPressedCounter(300);  
  if (wynik){
    //Serial.print("wynik biurko : ");
    //Serial.println(wynik);
    if(wynik == 1){
      if(pwm_biurko.GetGoToLevel() == 0
        && wynik){
        pwm_biurko.GoTo(50);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }    
      if(pwm_biurko.GetGoToLevel() == 50
        && wynik){
        pwm_biurko.GoTo(90);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }
      if(pwm_biurko.GetGoToLevel() == 90
        && wynik){
        pwm_biurko.GoTo(255);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }
      if(pwm_biurko.GetGoToLevel() == 255
        && wynik){
        pwm_biurko.SetLevel(0);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }     
    }
    if (wynik == 2){
        wylonczSwiatla();
        czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
    }  
    if (wynik == 3){
        pwm_biurko.SetLevel(0);
        czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
    } 
    
    if (wynik == 200){               // długie przyciśnięcie gasi wszystkie światła i wylącza czujnik
      czujnikDelay = 0;
      wylonczSwiatla();
    }
  }
  
// ---------------- Wlącznik łóżko -----------------//  
  
  wynik = b_pokojLozko.buttonPressedCounter(300);  
  if (wynik){
    //Serial.print("wynik lozko : ");
    //Serial.println(wynik);
    if(wynik == 1){
      if(pwm_lozko.GetGoToLevel() == 0
        && wynik){
        pwm_lozko.GoTo(40);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }    
      if(pwm_lozko.GetGoToLevel() == 40
        && wynik){
        pwm_lozko.GoTo(60);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }
      if(pwm_lozko.GetGoToLevel() == 60
        && wynik){
        pwm_lozko.GoTo(255);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }
      if(pwm_lozko.GetGoToLevel() == 255
        && wynik){
        pwm_lozko.SetLevel(0);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }     
    }
      
      if (wynik == 2){
          wylonczSwiatla();
          czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
      }  
      if (wynik == 3){
          pwm_lozko.SetLevel(0);
          czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
      } 
      
      if (wynik == 200){               // długie przyciśnięcie gasi wszystkie światła i wylącza czujnik
        czujnikDelay = 0;
        wylonczSwiatla();
      }
      
      // ---------------- Wlącznik łóżko - obsługa rolety -----------------// 
      if (wynik == 4){
        roleta.goOpposite();
      }  
}




// ---------------- Odczyty w delay -----------------//  


if(timeDelay >= 2000)
{
  timeDelay = 0;

  // ---------------- Pomiar światła -----------------//  
  swiatoLevel = analogRead(A0);
  //Serial.println(swiatoLevel); 
  
  // ---------------- Sprawdzenie okna -----------------//  

   wynik = b_pokojOkno.checkStatus();
  //wynik = expanderRead(5);
/*
  Serial.print("Okno                : "); Serial.println(wynik);
  Serial.print("Led 1 poziom        : "); Serial.println(pwm_halogen.GetCurrentLevel());
  Serial.print("Opuznienie czujnika : "); Serial.println(czujnikDelay);
  Serial.print("Czujnik             : "); Serial.println(expanderRead(7));
*/
}



// ---------------- PWM GO TO ---------------- //
pwm_halogen.PwmLoop(15);
pwm_led2.PwmLoop(15);
pwm_biurko.PwmLoop(15);
pwm_lozko.PwmLoop(15);
pwm_szafa.PwmLoop(15);

roleta.loop();
}

void wylonczSwiatla(){
  pwm_halogen.SetLevel(0);
  pwm_led2.SetLevel(0);
  pwm_biurko.SetLevel(0); 
  pwm_lozko.SetLevel(0);
}
