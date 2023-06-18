#include <LiquidCrystal.h>
#include "AsyncTaskLib.h"
#define DHT11_PIN       5

#include "DHTStable.h"

DHTStable DHT;
const int photocellPin = A0;
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);int outputValue = 0;

#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);
void readtemperature();
void showLight();
AsyncTask asyncTask(2000, true, []() { DEBUG("Expired"); });
AsyncTask asynctaskTemp(2000, true, readtemperature);
AsyncTask asynctaskLight(4000, true, showLight);
void setup()
{
	Serial.begin(9600);
	Serial.println("Starting");
  asynctaskTemp.Start();
	asyncTask.Start();
  asynctaskLight.Start();
  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop()
{
  asynctaskTemp.Update();
	asyncTask.Update();
  asynctaskLight.Update();
}
void readtemperature(){
 Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
lcd.setCursor(0,0) ;
 lcd.print("H:");
 lcd.print(DHT.getHumidity());
  
  
  Serial.print(DHT.getHumidity(), 1);
  Serial.print(",\t");
  Serial.println(DHT.getTemperature(), 1);

  
} 

void showLight(){

outputValue = analogRead(photocellPin);
lcd.setCursor(0, 1);
lcd.print("L:");
lcd.print(outputValue);//print the temperature on lcd1602
Serial.println(outputValue);
lcd.setCursor(11, 0);
lcd.print(" ");  
}