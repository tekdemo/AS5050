/* 
 * This sketch demonstrates some of the ways to read the angle from the AS5050
 * magnetic encoder. Note, that to use this chip, you require a special 
 * diametrically magnetized magnet, not one of the easy-to-find axially
 * magnetized ones. These are available on quite a few sites, from K+J Magnetics
 * to AMS (who supplies this chip).
 * 
 * Check for updates at the code site
 * https://github.com/tekdemo/AS5050
 */

//I don't know why, but you have to include SPI. before the AS5050.h
#include <SPI.h>
#include <as5050.h>

//Initialize with the chip select pin, and with the clock divider.
#define CHIP_SELECT_PIN 9
#define CLOCK_DIVIDER_VALUE 16
AS5050 mag(CHIP_SELECT_PIN,CLOCK_DIVIDER_VALUE);

void setup(){
  //Configure the serial terminal
  Serial.begin(9600);
}

unsigned long oldmillis=0;
int lastPrintedAngle=-10;
int currentAngle;

void loop(){
  //we need this to update regularly, so call it often 
  //Otherwise, it will lose track of the rotations if we spin things too fast.
  currentAngle=mag.angle();
  
  
  //Print whenever the angle changes a bit, or if it's been a while
  if(abs(lastPrintedAngle-currentAngle)>10 || millis()>oldmillis+1000){
    lastPrintedAngle=mag.angle();
    oldmillis=millis();
    
    //Do a printout of some of the different ways we can read the angle
    Serial.print("Angle:");      Serial.print(mag.angle());
    Serial.print("\tDegrees:");  Serial.print(mag.angleDegrees());
    Serial.print("\tRads:");     Serial.print(mag.angleRad());
    Serial.print("\tdAngle:");   Serial.print(mag.deltaAngle());
    Serial.print("\tdDegrees:"); Serial.print(mag.deltaAngleDegrees());
    Serial.print("\tdRads:");    Serial.print(mag.deltaAngleRad());
    Serial.println("");
  }
}
