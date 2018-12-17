#include <Wire.h>
#include <PinExpander.h>
#include "ButtonControl.h"
#define _OPUZNIENIE 10000

//SimpleButton b8(9,50);

//---- Przyciski -- //
ButtonCounter b_pokojDrzwi(2);
ButtonLoop b_opkojCiemniej(1);
ButtonLoop b_opkojJasniej(0);

ButtonCounter b_pokojLozko(3);
ButtonCounter b_pokojBiurko(4);
ButtonCounter b_opkojJasniejCounter(0);

ButtonStatus b_pokojOkno(5);
ButtonStatus b_pokojSzafa(6);
ButtonStatus b_pokojCzujnikRuchu(7);


//---- PWM -- //
PwmGoTo pwm_led1(5);//halogen
PwmGoTo pwm_led2(4);//taśma
PwmGoTo pwm_biurko(0);
PwmGoTo pwm_lozko(1);
PwmGoTo pwm_szafa(2);


void setup()
{    
   Serial.begin(115200);
   Serial.print ("Init ... "); 
   delay(50);

   addExpander(MCP23017, 255, 255);
   addExpander(PCA9635, 0, 0);    

   delay(100);
   Serial.println("ok !");
}

byte wynik = 0;
int timeDelay = 0;
int swiatoLevel = 1035;
int czujnikDelay = 1;
//bool pasek = false;
byte pasek = 0;

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

/* 
// ---------------- Czujnik ruchu -----------------//  
  wynik = b_pokojCzujnikRuchu.checkStatus();
  
  //Serial.println(wynik);
  // jeśli wykryje ruch
  // jeśli jest ciemniej niż 1000
  // jeśli opuźnienie jest na poziomie 1     
  
  if(wynik==1
    //&& swiatoLevel < 1000
    &&(
        (pwm_led1.GetGoToLevel()==0 && !pasek)
      ||(pwm_led2.GetGoToLevel()==0 && pasek))
    && czujnikDelay == 1
    ){      
    pwm_led1.GoTo(180);
    czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
  }
 */
// ---------------- Wlącznik przy dżwiach -----------------//  
  
  wynik = b_pokojDrzwi.buttonPressedCounter(300);  
  if (wynik){
    Serial.print("wynik drzwi : ");
    Serial.println(wynik);
    if(wynik == 1){
      if(pwm_led1.GetGoToLevel() > 0 || pwm_led2.GetGoToLevel() > 0){
        pwm_led1.SetLevel(0);
        pwm_led2.SetLevel(0);
        czujnikDelay = _OPUZNIENIE;  // ustawia opuźnienie dla czujnika ruchu
        pasek = 0;
      }else{
        pwm_led1.GoTo(180);
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
        pasek = 1;
      }
    }
    if (wynik == 2){
      wylonczSwiatla();
      czujnikDelay = _OPUZNIENIE;    // ustawia opuźnienie dla czujnika ruchu
      pasek = 0;
    }  
 
    if (wynik == 200){               // długie przyciśnięcie gasi wszystkie światła i wylącza czujnik
      czujnikDelay = 0;
      wylonczSwiatla();
      pasek = 0;
    }
  }

// ---------------- Wlącznik przy dżwiach - jaśniej -----------------// 

  wynik = b_opkojJasniejCounter.buttonPressedCounter(300);
  
  if (wynik == 2) {
    // pasek = 0 wyłączone wszystko
    
    // pasek = 1 wlączone ledy
    // pasek = 2 włączone ledy go to 
    
    // pasek = 3 właczona taśma
    // pasek = 4 włączona tasma go to
    
    Serial.print ("Pasek jest ");
    
    Serial.println (pasek);
    if (pasek == 1){
      pasek = 4;
      Serial.println ("Pasek set to 4");
    }
    if (pasek == 3){
      pasek = 2;
      Serial.println ("Pasek set to 2");
    }
    
  /*  
    Serial.print("Pasek ");
    pasek = !pasek;
    Serial.println(pasek);
    
    
    if(pasek)
    {
     // byte lv2 =pwm_led2.GetCurrentLevel();
      pwm_led2.GoTo(180);  
      pwm_led1.GoTo(0);
    }
    else
    {
      //byte lv1 =pwm_led1.GetCurrentLevel();
      pwm_led1.GoTo(180);  
      pwm_led2.GoTo(0);  
      
    }
    */
  }

  wynik = b_opkojJasniej.buttonPressedLoop(50);

   if( wynik == 1
      && (
        (pwm_led1.GetCurrentLevel() < 253)
        || (pwm_led2.GetCurrentLevel() < 253)
        )  
      //&& (pasek == 1 || pasek == 3)      
     ){
      
       //Serial.println("jasniej ");   
        if(pasek == 3)
        {
          // Serial.println(pwm_led2.GetCurrentLevel());
          pwm_led2.GoTo(pwm_led2.GetCurrentLevel()+2);    // pasek led      
        }
        if(pasek == 1)
        {
         //  Serial.println(pwm_led1.GetCurrentLevel());
          pwm_led1.GoTo(pwm_led1.GetCurrentLevel()+2);   // ledy 
        }   
        
  }
   
// ---------------- Wlącznik przy dżwiach - ciemniej -----------------// 

  wynik = b_opkojCiemniej.buttonPressedLoop(50);

  if( wynik == 1
    && (
    (pwm_led1.GetCurrentLevel() > 9)
    ||(pwm_led2.GetCurrentLevel() >9)
    //&& (pasek == 1 || pasek == 3)
    )
  )
  {    
    if(pasek == 3)
    {
      pwm_led2.GoTo(pwm_led2.GetCurrentLevel()-2);
          
    }
    if(pasek == 1)
    {
      pwm_led1.GoTo(pwm_led1.GetCurrentLevel()-2);    
    }
          
    //pwm_led1.SetLevel(pwm_led1.GetCurrentLevel()-2);
    //Serial.print("ciemniej ");
    //Serial.println(pwm_led1.GetCurrentLevel());
  }   

  
// ---------------- Wlącznik biurko -----------------//  
  
  wynik = b_pokojBiurko.buttonPressedCounter(300);  
  if (wynik){
    Serial.print("wynik biurko : ");
    Serial.println(wynik);
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
    Serial.print("wynik lozko : ");
    Serial.println(wynik);
    if(wynik == 1){
      if(pwm_lozko.GetGoToLevel() == 0
        && wynik){
        pwm_lozko.GoTo(50);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }    
      if(pwm_lozko.GetGoToLevel() == 50
        && wynik){
        pwm_lozko.GoTo(90);
        wynik = 0;
        czujnikDelay = 1;            // uaktywnia czujnik ruchu
      }
      if(pwm_lozko.GetGoToLevel() == 90
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
  }

// ---------------- Odczyty w delay -----------------//  


if(timeDelay >= 2000)
{
  timeDelay = 0;
/*
  // ---------------- Pomiar światła -----------------//  
  swiatoLevel = analogRead(A0);
  Serial.println(swiatoLevel); 
  
  // ---------------- Sprawdzenie okna -----------------//  

   wynik = b_pokojOkno.checkStatus();
  //wynik = expanderRead(5);

  Serial.print("Okno                : "); Serial.println(wynik);
  Serial.print("Led 1 poziom        : "); Serial.println(pwm_led1.GetCurrentLevel());
  Serial.print("Opuznienie czujnika : "); Serial.println(czujnikDelay);
  Serial.print("Czujnik             : "); Serial.println(expanderRead(7));
*/
}


    if(pasek == 4)
    {
     // byte lv2 =pwm_led2.GetCurrentLevel();
      pwm_led1.GoTo(0);
      pwm_led2.GoTo(180);       
      pasek = 3;
      Serial.println("Pasek 3");
    }
    if(pasek == 2)
    {
      //byte lv1 =pwm_led1.GetCurrentLevel();
      pwm_led1.GoTo(180);  
      pwm_led2.GoTo(0);  
      pasek = 1;   
     Serial.println("Pasek 1");   
    }


// ---------------- PWM GO TO ---------------- //
pwm_led1.PwmLoop(15);
pwm_led2.PwmLoop(15);
pwm_biurko.PwmLoop(15);
pwm_lozko.PwmLoop(15);
pwm_szafa.PwmLoop(15);
}

void wylonczSwiatla(){
  pasek = 0;
  pwm_led1.SetLevel(0);
  pwm_led2.SetLevel(0);
  pwm_biurko.SetLevel(0); 
  pwm_lozko.SetLevel(0);
}
