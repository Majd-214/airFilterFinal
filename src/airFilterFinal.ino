#include <Arduino.h>
#include <Wire.h>

//initialize CCS811 CO2 and Toxic gas sensor
#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;

//initialize temperature and humidity sensor
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//initialize OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

//initialize led pins
int ledRed = 5;
int ledGreen = 4;
int ledBlue = 3;

//initialize buzzer pin
int buzzerPin = 6;

//initialize button pin
int buttonPin = 7;

//Initialize gas sensor rating scheme
String airQuality;

//Initialize MOSFET pin for fan load
int switchPin = 8;

//Initialize fan PWM pin
int fanPin = 9;

void setup() 
{
  //start I2C bus
  Wire.begin();

  //Initialize temperature and humidity sensor
  dht.begin();

  //set RGB led to output
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  //set buzzer pin to output
  pinMode(buzzerPin, OUTPUT);
  
  //set button pin to input
  pinMode(buttonPin, INPUT);

  //set fan pin to output
  pinMode(fanPin, OUTPUT);

  //set MOSFET pin to output
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);
  
  //Initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(5,20);
  display.print("Loading...");
  display.display();
  delay(2000);
  display.clearDisplay();

  if(!ccs.begin())
  {
    display.print("Fatal error!");
    while(1);
  }
  
  //wait for CCS811 sensor to be ready
  while(!ccs.available());
}

//mian loop
void loop() 
{
  //read gas sensor
  if(ccs.available())
  {
    if(!ccs.readData())
    {
      gasMode();
    }
    else
    {
      while(1);
    }
  }  
  //display temperature and humidity when button is pressed
  if(digitalRead(buttonPin) == HIGH)
  {
      tempMode();
  }
}

void gasMode()
{
      int CO2 = ccs.geteCO2();
      int TVOC = ccs.getTVOC();
      
      if(TVOC < 330)
      {
          airQuality = "Great";
          clearLed();
          digitalWrite(ledGreen, HIGH);
          fanOff();
      }
      
      else if(TVOC >= 330 && TVOC < 660)
      {
          airQuality = "Good";
          clearLed();
          digitalWrite(ledGreen, HIGH);
          digitalWrite(ledBlue, HIGH);
          fanOn(1);
      }
      
      else if(TVOC >= 660 && TVOC < 1000)
      {
          airQuality = "Decent";
          clearLed();
          digitalWrite(ledRed, HIGH);
          digitalWrite(ledGreen, HIGH);
          digitalWrite(ledBlue, HIGH);
          fanOn(2);
      }

      else if(TVOC >= 1000 && TVOC < 1330)
      {
          airQuality = "Poor";
          clearLed();
          digitalWrite(ledGreen, HIGH);
          digitalWrite(ledRed, HIGH);
          fanOn(3);
      }

      else if(TVOC >= 1330 && TVOC < 1800)
      {
          airQuality = "Warning";
          clearLed();
          digitalWrite(ledRed, HIGH);
          digitalWrite(ledBlue, HIGH);
          fanOn(4);
      }

      else if(TVOC >= 2000)
      {
          airQuality = "DANGER!";
          clearLed();
          digitalWrite(ledRed, HIGH);
          fanOn(5);
          alarm();
      } 
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(0,0);
        display.print("CO2:");
        display.print(CO2);
        display.setCursor(64,0);
        display.print("VOC:");
        display.print(TVOC);
        display.setCursor(0,20);
        display.print("Air Quality: ");
        display.setTextSize(3);
        display.setCursor(0,30);
        display.print(airQuality);
        display.display();
  }
  
//clears LEDs
void clearLed()
{
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledBlue, LOW);
}

//sounds alarm
void alarm()
{
  tone(buzzerPin, 1568, 500);
  digitalWrite(ledRed, HIGH);
  delay(250);
  digitalWrite(ledRed, LOW);
  delay(250);
  digitalWrite(ledRed, HIGH);
  delay(250);
  digitalWrite(ledRed, LOW);
  delay(250);
}

//displays temp
void tempMode()
{
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //int f = dht.readTemperature(true);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(5,15);
  display.print("Humid:");
  display.print(h);
  display.print("%");
  display.setCursor(5,35);
  display.print("Temp:");
  display.print(t);
  display.print((char)247);
  display.print("C");
  /*display.print(f);
  display.print((char)247);
  display.print("F");*/
  display.display();
  delay(5000);
}

void fanOff()
{
  digitalWrite(switchPin, LOW);
  analogWrite(fanPin, 0);
}

void fanOn(int speed)
{
  int range = map(speed, 1, 5, 0 ,255);
  digitalWrite(switchPin, HIGH);
  analogWrite(fanPin, range);
}