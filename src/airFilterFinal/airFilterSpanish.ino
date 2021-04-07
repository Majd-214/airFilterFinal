#include <Arduino.h>
#include <Wire.h>
#include <LED.h>

//initialize CCS811 CO2 and Toxic gas sensor
#include "Adafruit_CCS811.h"
Adafruit_CCS811 ccs;

//initialize temperature and humidity sensor
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//initialize OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

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

//Initialize led
LED led;

//Initialize Serial Communications Data Variables
bool farenheit = false;
bool mute = false;

void setup()
{
  //Initialize Serial Communication
  Serial.begin(9600);

  //start I2C bus
  Wire.begin();

  //Initialize temperature and humidity sensor
  dht.begin();

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
  display.setCursor(48, 15);
  display.print("...");
  display.display();
  delay(2000);
  display.clearDisplay();

  if (!ccs.begin())
  { 
    Serial.print("Error Fatal!");
    while (1);
  }

  //wait for CCS811 sensor to be ready
  while (!ccs.available());
}

//sounds alarm
void alarm()
{
  if(mute == false){
    tone(buzzerPin, 1568, 500);
  }
  led.Flash('R', 250);
}

//controls fan
void fan(int speed)
{
  int range = map(speed, 1, 5, 0, 255);
  digitalWrite(switchPin, HIGH);
  analogWrite(fanPin, range);
  if(speed == 0)
  {
    digitalWrite(switchPin, LOW);
    analogWrite(fanPin, 0);
  }
}

void gasMode()
{
  int CO2 = ccs.geteCO2();
  int TVOC = ccs.getTVOC();
  if (TVOC < 330)
  {
    airQuality = "Genial";
    led.Color('G');
    fan(0);
  }

  else if (TVOC >= 330 && TVOC < 660)
  {
    airQuality = "Buena";
    led.Color('C');
    fan(1);
  }

  else if (TVOC >= 660 && TVOC < 1000)
  {
    airQuality = "Decente";
    led.Color('W');
    fan(2);
  }

  else if (TVOC >= 1000 && TVOC < 1330)
  {
    airQuality = "Mala";
    led.Color('Y');
    fan(3);
  }

  else if (TVOC >= 1330 && TVOC < 1800)
  {
    airQuality = "Alarma";
    led.Color('M');
    fan(4);
  }

  else if (TVOC >= 2000)
  {
    airQuality = "PELIGRO!";
    led.Color('R');
    fan(5);
    alarm();
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("CO2:");
  display.print(CO2);
  display.setCursor(64, 0);
  display.print("COV:");
  display.print(TVOC);
  display.setCursor(0, 20);
  display.print("Calidad del aire:");
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(airQuality);
  display.display(); 
}

//displays temp
void tempMode()
{
  // Read humidity from DHT sensor
  int h = dht.readHumidity();
  // Initiate Temperature in Celsius
  int c = dht.readTemperature();
  // Initiate Temperature in Farenheit
  int f = dht.readTemperature(true);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(5, 15);
  display.print("Humid:");
  display.print(h);
  display.print("%");
  display.setCursor(5, 35);
  display.print("Temp:");

  if(farenheit == false)
  {
    display.print(c);
    display.print((char)247);
    display.print('C');
  }
  else if(farenheit == true)
  {
    display.print(f);
    display.print((char)247);
    display.print('F');
  }

  display.display();
  delay(5000);
}

//mian loop
void loop()
{ 
  int input = Serial.read();
  switch(input)
  {
    case 'C': farenheit = false; break;
    case 'F': farenheit = true; break;
    case 'M': mute = true; break;
    case 'B': mute = false; break;
  }

  

  //read gas sensor
  if (ccs.available())
  {
    if (!ccs.readData())
    {
      gasMode();
    }
    else
    {
      while (1);
    }
  }
  //display temperature and humidity when button is pressed
  if (digitalRead(buttonPin) == HIGH)
  {
    tempMode();
  }
}
