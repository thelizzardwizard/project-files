
#include <NewTone.h> 
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include "RTClib.h"
#include <LiquidCrystal.h>


LiquidCrystal lcd(6, 7, 8, 9, 10, 11);
RTC_DS3231 rtc; 
const int RECV_PIN = 12; //ir remote pin
IRrecv irrecv(RECV_PIN); //creates ir objext attached to pin
decode_results results; //creates results objext from decode results
//for ir hex codes, i list the corresponded button commented, specific codes will depend on your remote protocol
unsigned long key_value = 0;

byte ledPin= 4;
int motorPin = 3;
int buz= 2;
long rep = 0XFFFFFFFF;//ir repeat code
//values for alarm calculation
byte ah10=0;
byte ah0=0;
byte am10=0;
byte am0=0;
long ah;
long am;
//place holder numbers. sometimes resetting the ir remote disrupts the saved values.
byte t = 0;//setting
byte c = 0; //place for alarm
byte n; //numbers for alarm
byte p1; //placeholders 
byte p2;
byte p3;
byte p4;
byte randNumber;
long array1[5];
long array2[5];
byte yy;


void changeDigit(void);
void showTime(void);
void showAlarm(void);
void makeArray(void); 
void showArray(void);


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buz, OUTPUT);
  ah = (ah10*10) + ah0;
  am = (am10*10) + am0;
  Serial.begin(9600);
 
  
  irrecv.enableIRIn(); // sets up ir 
  irrecv.blink13(true);// blinks when signal reciever
  delay(3000); // wait for console opening
  lcd.begin(16, 2);
  lcd.blink();
//-----------------------rtc set up for issues----------
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // If the RTC have lost power it will sets the RTC to the date & time this sketch was compiled in the following line
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

}
//-------------functions-------------------------------------
void showTime(){
   DateTime now = rtc.now();
   lcd.setCursor(0, 1);
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   lcd.setCursor(0, 1);
   if(now.hour()<10){
    lcd.print('0');
   }
   lcd.print(now.hour());
   lcd.print(':');
   if(now.minute()<10){
    lcd.print('0');
   }
   lcd.print(now.minute());
}
void showAlarm(){
  ah = (ah10*10) + ah0;
  am = (am10*10) + am0;
  Serial.print("TIME");
  lcd.setCursor(0, 1);
  if(ah<10){
    lcd.print('0');
    lcd.print(ah);
   }else{
    lcd.print(ah);
   }
  lcd.print(':');
  if(am<10){
    lcd.print('0');
    lcd.print(am);
   }else{
    lcd.print(am);
   }
   lcd.setCursor(c, 1);
   delay(3000);
    if(ah<10){
    Serial.print('0');
    Serial.print(ah);
   }else{
    Serial.print(ah);
   }
  Serial.print(':');
  if(am<10){
    Serial.print('0');
    Serial.print(am);
   }else{
    Serial.print(am);
   }
 }

void changeDigit(){ // changes the digit place when setting the alarm
     switch(c){
      case 0:
      ah10 = n;
      p1=ah10;
      Serial.println("P");
      Serial.println(ah10);
      break;
      case 1:
      ah0 = n;
      p2=ah0;
      Serial.println("P");
      Serial.println(ah0);
      break;
      case 3:
      am10 =n;
      p3=am10;
      Serial.println("P");
      Serial.println(am10);
      break;
      case 4:
      am0 =n;
      p4=am0;
      Serial.println("P");
      Serial.println(am0);
      break;
     }
}
void makeArray(){
  //makes alarm array, to change and have more digits, change the 3 in randNumber = random(3); to another number, remember to also add the ir remote codes in the alarm loop to correspond

  lcd.clear();
  lcd.setCursor(0,0);
  for (int i = 0; i < 4; i = i + 1){
    randomSeed(analogRead(0));
    randNumber = random(3);
    array1[i]= randNumber;
    Serial.println(i);
    lcd.print(array1[i]);
    delay(100);
  } 
}
void showArray(){
  lcd.clear();
  lcd.setCursor(0,0);
  for (int i = 0; i < 4; i = i + 1){
    Serial.println("ARRAY");
    Serial.println(i);
    lcd.print(array1[i]);
  }
  delay(3000);
}

//--------------------------loop--------------------    
void loop() {
  delay(1000);
  DateTime now = rtc.now();
  ah = (ah10*10) + ah0;
  am = (am10*10) + am0;
  //used to test alarm function, comment out top two lines and uncomment bottom two to test
  //ah=now.hour(); 
 //am=now.minute();
//--------ring alarm loops-----------------------  
  if(ah==now.hour()&& am==now.minute()){
     Serial.println("RINGALARM");
     t=2;
     digitalWrite(ledPin, HIGH);
     digitalWrite(buz, HIGH);
     digitalWrite(motorPin, HIGH);
     delay(1000);
     digitalWrite(motorPin, LOW);
   }
 
  while(t==2){
    makeArray();
    lcd.setCursor(0,1);
    for (int y = 0; y < 4; y = y){
      results.value = 200;
      Serial.print(results.value);
      NewTone(buz, 2000, 1000);//feel free to experiment and play with different tones for the alarm
      delay(500);
      if (irrecv.decode(&results)){ //reurns true if code recieved, stored in results.value
        switch(results.value){
          case 0xFF6897://0
          lcd.setCursor(y,1);
          Serial.print(y);
          array2[y] = 0;
          lcd.print(array2[y]);
          if(array1[y] == array2[y]){
            yy=y+1;
            Serial.println(yy);
          }else{
          showArray();
          yy = 0;   
          }
          break;
          
          case 0xFF30CF://1
          lcd.setCursor(y,1);
          Serial.print(y);
          array2[y] = 1;
          lcd.print(array2[y]);
          if(array1[y] == array2[y]){
            yy=y+1;
            Serial.println(yy);
          }else{
          showArray();
          yy = 0; 
          //lcd.setCursor(0,1);  
          }
          break;

           case 0xFF18E7://2
          lcd.setCursor(y,1);
          Serial.print(y);
          array2[y] = 2;
          lcd.print(array2[y]);
          if(array1[y] == array2[y]){
            yy=y+1;
            Serial.println(yy);
          }else{
          showArray();
          yy = 0; 
           
          }
          break;
        }
        // key_value = results.value; //stores previous code as global varriable
         irrecv.resume();//reset reciever
         y=yy;
         Serial.println(y);
        }
        
    }
    t=0;
    while(ah==now.hour()&& am==now.minute()){
      lcd.clear();
      lcd.print("ALARM OFF");  
      delay(3000);
      digitalWrite(ledPin, LOW);
      noNewTone(buz);
      digitalWrite(buz, LOW);
    }
  }
  
//-------------------------time loop---------------
  while(t==0){
    lcd.setCursor(0, 1);
    showTime();
    delay(3000);
    if (irrecv.decode(&results)){ //reurns true if code recieved, stored in results.value
      if (results.value == rep){
       results.value = key_value;  // if repete key, uses global var stored as pervious code
      }
      if(results.value== 0xFFA25D){// this code for the ch- button switches to the alarm setting
        Serial.print("CH-");
        t=1;
        lcd.clear();
        lcd.setCursor(0,1);
       }
       key_value = results.value; //stores previous code as global varriable
      irrecv.resume();  //reset reciever
      break;
   } 
 }
 //--------------------------alarm set loop----------------
 while(t==1){//alarm loop
  showAlarm();
   if (irrecv.decode(&results)){ //reurns true if code recieved, stored in results.value
      if (results.value == rep){
       results.value = key_value;  // if repeat key, uses global var stored as pervious code
      }
      if(results.value== 0xFF629D){ // pressing ch takes you back to the time loop
        Serial.print("CH");
        t=0; 
       }
       switch(results.value){// setting the alarm time
          case 0xFF6897://0
          n=0;
          changeDigit();
          break;
          case 0xFF30CF://1
          n=1;
          changeDigit();
          break;
          case 0xFF18E7://2
          n=2;
          changeDigit();
          break;
          case 0xFF7A85://3
          n=3;
          changeDigit();
          break;
          case 0xFF10EF://4
          n=4;
          changeDigit();
          break;
          case 0xFF38C7://5
          n=5;
          changeDigit();
          break;
          case 0xFF5AA5://6
          n=6;
          changeDigit();
          break;
          case 0xFF42BD://7
          n=7;
          changeDigit();
          break;
          case 0xFF4AB5://8
          n=8;
          changeDigit();
          break;
          case 0xFF52AD://9
          n=9;
          changeDigit();
          break;
       }
       
     //changing didgit places while setting alarm  
       if(results.value==0xFFA857 && -1<c<5){//+ key
         switch(c){
          case 0:
            c=1;
            lcd.setCursor(1, 1);
            lcd.blink();
            break;
          case 1:
            c=2;
            lcd.setCursor(2, 1);
            lcd.blink();
          break;
            case 2:
          c=3;
            lcd.setCursor(3, 1);
            lcd.blink();
            break;
          case 3:
          case 4:
            c=4;
            lcd.setCursor(4, 1);
            lcd.blink();
            break;
          }
          Serial.println(c);
          irrecv.resume();
          break;
       }
       if(results.value==0xFFE01F && -1<c<5){//- key
         switch(c){
          case 0:
          case 1:
            c=0;
            lcd.setCursor(0, 1);
            lcd.blink();
            break;
          case 2:
            c=1;
            lcd.setCursor(1, 1);
            lcd.blink();
            break;
          case 3:
            c=2;
            lcd.setCursor(2, 1);
            lcd.blink();
            break;
          case 4:
            c=3;
            lcd.setCursor(3, 1);
            lcd.blink();
            break;
           }
           Serial.println(c);
           irrecv.resume();
           break;
       }
      
       key_value = results.value; //stores previous code as global varriable
       irrecv.resume();//reset reciever
       ah10=p1;
       ah0=p2;
       am10=p3;
       am0=p4;
    
       break;
     }
   }
}
 
