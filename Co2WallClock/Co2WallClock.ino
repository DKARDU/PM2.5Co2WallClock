
#include <Wire.h>

#include "Buzzer.h"
#include "Timer.h"
#include "LDR.h"
#include "LEDRing.h"
#include "DuoLED.h"
#include "CO2Sensor.h"
//PINS

const byte PIN_BUZZER = 13;
const byte PIN_LDR = A7;
const byte PIN_LEDRING = 4;
const byte LEDRING_PM = 6;
const byte PIN_BUTTON = 3;  //Use a pin that works for interrupts 
int measurePin = A1; //Connect dust sensor to Arduino A0 pin
int ledPower = 12; 
//const byte PIN_RED_LED = 10;
//const byte PIN_GREEN_LED = 9;
//const byte gasPin = A0;
//const byte PM = A1;
int gasSensor = 0; 
int PM2_5 = 0; 

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;


//CO2 concentration is affected by altitude 
//Altitude compensation for CO2 measurement - update for your setting 
const word altitude = 250;
//Force calibration based on a reliable and known reference; only use if better than factory default

const word refCO2Value = 0; //0:No calibration; 400-2000: use trusted reference value 
                           

//Setting a flag when inerrupt is detected is an effective way to debounce a button

//volatile bool buttonInterrupt = false;

//Buzzer is active if true
//如果为true，则蜂鸣器处于活动状态
//bool buzzerOn = true;

//Keep track of last CO2 readout  
//Not every loop cylce updates CO2 - sensor delivery new data only every 2s

word co2 = 0;
CO2Sensor co2Sensor(A0, 0.99, 100);
//Timer timer;
//SCD30 sensor;
LDR ldr(PIN_LDR);

//DuoLED duoLed(PIN_RED_LED, PIN_GREEN_LED);
LEDRing ledRing(28, PIN_LEDRING, 65536 * 2 / 4.5); //Hue is a 16 bit number; value = blue  
LEDRing ledRing_PM(28, LEDRING_PM, 65536 * 2 / 3); //Hue is a 16 bit number; value = blue
//Arduino specific - initializes and sets the initial values


void setup() {
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);
  pinMode(PIN_BUZZER,OUTPUT);
  digitalWrite(PIN_BUZZER,LOW);
  co2Sensor.calibrate();
  //Show LED0 for 1s 
  ledRing.setBrightness(DayPeriod::DAY);
  ledRing.setLed(0);
  ledRing_PM.setBrightness(DayPeriod::DAY);
  ledRing_PM.setLed(0);
  delay(1000);

  //Button as interrupt 
  
  //Button pulls to GND - detect falling edge as interrupt
  
   Serial.println("=== Initialized ===");
   delay(600000); //Wait ten minutes
 
}

//Used by attachInterrupt() to track status 



void PM_25(){
  
    digitalWrite(ledPower,LOW);
    delayMicroseconds(samplingTime);
    PM2_5 = analogRead(measurePin);
    delayMicroseconds(deltaTime);
    digitalWrite(ledPower,HIGH); // turn the LED off
    delayMicroseconds(sleepTime);
  
    if (PM2_5 > 2800) PM2_5 = 2800;
    if (PM2_5 < 0) PM2_5 = 0;
    
    Serial.print("PM2.5 : ");
    Serial.print(PM2_5);
    Serial.print("\n");

    byte activeLed_PM = map(PM2_5, 0, 1023, 0, 27);
    ledRing_PM.setLed(activeLed_PM);
    delay(10);
        if (PM2_5 < 650){
      digitalWrite(PIN_BUZZER,LOW);
        }
     else{
      digitalWrite(PIN_BUZZER,HIGH);
    }
}

void co2_gas(){
  
   gasSensor = co2Sensor.read();
  
  //get co2 sensor data, range ristrict it, active corresponding LED


    //limit co2 to available range on watch face

    if (gasSensor > 1000) gasSensor = 1000;
    if (gasSensor < 0) gasSensor = 0;

   Serial.print("CO2 : ");
   Serial.print(gasSensor);
   Serial.print("\n");

    //Map co2 onto 24 LED range

    byte activeLed = map(gasSensor, 0, 1023, 0, 27);

    //Update led brightness
    ledRing.setLed(activeLed);
    delay(10);

        if (gasSensor < 700){
      digitalWrite(PIN_BUZZER,LOW);
    } else {
      digitalWrite(PIN_BUZZER,HIGH);
    }
}
void loop() {


  //Update LED status; outside so respective LED active even without any interrupts

  co2_gas();
  PM_25();

  //Set buzzer timer and set number of beeps
 

  delay(1000);
}
