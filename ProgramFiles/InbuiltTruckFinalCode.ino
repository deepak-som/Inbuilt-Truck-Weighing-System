//Include LCD and hx711 and other required libraries

#include <MPU6050_tockn.h>
#include <LiquidCrystal.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4  ;

const int buzz=3, grn=8, yel=9, red=10;                           // declare pins for indication unit

const int data = A2, clk = A1;                                    // pins for hx711
int frequency_r = 20, frequency_y = 40, frequency_g = 75;       //frequency declaration for each color
int buzz_t;

const int eepromAdress = 0;

long t;

//HX711 constructor (dout pin, sck pin):
//LiquidCrystal lcd(pins)


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

HX711_ADC LoadCell(data, clk);




MPU6050 mpu6050(Wire);

float x,y,z;                                 // variables for each axis representation

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  lcd.begin(16, 2);
  mpu6050.calcGyroOffsets(true);


 //indication unit pins
  pinMode(yel, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(grn, OUTPUT);
  //tone(buzz,50);

 lcd.print("MPU INTIALIZE");
 delay(1000);
 lcd.setCursor(0,2);
 lcd.print("DO NOT! MOVE MPU");
 delay(5000);

 lcd.clear();


float calValue;                             // calibration value
  calValue = 110.0;                         // uncomment this if you want to set this value in the sketch 
  #if defined(ESP8266) 
  //EEPROM.begin(512);                      // uncomment this if you use ESP8266 and want to fetch the value from eeprom
  #endif
  //EEPROM.get(eepromAdress, calValue);     // uncomment this if you want to fetch the value from eeprom
  
  //Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  lcd.print("Starting Load");
  LoadCell.begin();
  long stabilisingtime = 2000;              // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  if(LoadCell.getTareTimeoutFlag()) {
    Serial.println("Tare timeout, check MCU>HX711 wiring and pin designations");
    lcd.print("Check Wiring");
  }
  else {
    LoadCell.setCalFactor(calValue);        // set calibration value (float)
    Serial.println("Startup + tare is complete");
  }
  

}


void loop() {
  mpu6050.update();
  Serial.print("angleX : ");

  x = mpu6050.getAngleX();
  Serial.print(x);
  Serial.print("\tangleY : ");

 y = mpu6050.getAngleY();
  Serial.print(y);
  Serial.print("\tangleZ : ");
  z = mpu6050.getAngleZ();
  Serial.println(z);

  //improper level part
if((x>15 || x<-15) || (y>15 || y<-15))
  {
    Serial.println("improper angles");
    lcd.clear();
    lcd.print("IMPROPER LEVEL!");
     digitalWrite(yel,HIGH);
tone(buzz,frequency_y,100);
delay(100);
    noTone(buzz);
    digitalWrite(yel,LOW);
    //buzz_y();
    //delay(1000);
    //lcd.clear();
    //lcd.print("x is:" );
    //lcd.setCurser(6,1);
    //lcd.print(x);


    //lcd.print("y is:" );
    //lcd.setCurser(6,2);
    //lcd.print(y);
 }


 else {

  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //use of delay in sketch will reduce effective sample rate (be carefull with use of delay() in the loop)
  LoadCell.update();

  //get smoothed value from data set
  if (millis() > t + 250) {
    
    float i = LoadCell.getData();
    float load_threshold = 2000;
    Serial.print("Load_cell output val: ");
    Serial.println(i);

    lcd.clear();
    //lcd.setCursor(1,1);
    lcd.print("Load value: ");
    lcd.setCursor(0,2);
    lcd.print(i);
    lcd.setCursor(11,2);
    lcd.print("gram");
    t = millis();
    
    //buzz_g();
  digitalWrite(grn,HIGH);
tone(buzz,frequency_g,100);
delay(100);
    noTone(buzz);
    digitalWrite(grn,LOW);
  }



//check if overload or  not
float i = LoadCell.getData();
float load_threshold = 2000; 
if(i > load_threshold){
  lcd.clear();
  Serial.print("OVERLOAD");
  lcd.print("Overload");
  lcd.setCursor(0,2);
  lcd.print(i);
   lcd.setCursor(11,2);
    lcd.print("gram");
    t = millis();

  
   digitalWrite(red,HIGH);
tone(buzz,frequency_r,100);
delay(100);
    noTone(buzz);
    digitalWrite(red,LOW);
    //buzz_r();
  }


//  //receive from serial terminal
//  if (Serial.available() > 0) {
//    float i;
//    char inByte = Serial.read();
//    if (inByte == 't') LoadCell.tareNoDelay();
//  }
//
//  //check if last tare operation is complete
//  if (LoadCell.getTareStatus() == true) {
//    Serial.println("Tare complete");
//  }
  
  }
  
   
}

 void buzz_r()
  {
     digitalWrite(red,HIGH);
tone(buzz,frequency_r,100);
delay(100);
    noTone(buzz);
    digitalWrite(red,LOW);
    
  }


 void buzz_y()
  {
  
  digitalWrite(yel,HIGH);
tone(buzz,frequency_y,100);
delay(100);
    noTone(buzz);
    digitalWrite(yel,LOW);
    
  }


 void buzz_g()
  {
   digitalWrite(grn,HIGH);
tone(buzz,frequency_g,100);
delay(100);
    noTone(buzz);
    digitalWrite(grn,LOW);
    
  }
