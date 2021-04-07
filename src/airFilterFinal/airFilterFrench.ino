#include <Arduino.h>
#include <Wire.h>
#include <LED.h>

//initialize CCS811 CO2 and Toxic gas sensor

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
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
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
  //set MOSFET pin to output
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, LOW);

  //Setup LED
  led.Setup(5,4,3,true);
  
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

  pinMode(A0, INPUT);

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
}

//sounds alarm
void alarm()
{
  if(mute == false){
    tone(buzzerPin, 1568, 500);
    delay(250);
  }
  led.Flash('R', 250);
}

//controls fan
void fan(int speed)
{
  int range = map(speed, 0, 5, 0, 255);
  digitalWrite(switchPin, HIGH);
  analogWrite(fanPin, range);
}

void gasMode(bool disp)
{
  //When using 5V pot value 0-1023, 3.3V pot value 0-640
  int TVOC = map(analogRead(A0), 0, 1023, 0, 2250);
  if (TVOC < 330)
    {
      airQuality = "Geniale";
      led.Color('G');
      fan(0);
    }

    else if (TVOC >= 330 && TVOC < 660)
    {
      airQuality = "Bonne";
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
      airQuality = "Mal";
      led.Color('Y');
      fan(3);
    }

    else if (TVOC >= 1330 && TVOC < 1800)
    {
      airQuality = "Alerte";
      led.Color('M');
      fan(4);
    }

    else if (TVOC >= 2000)
    {
      airQuality = "DANGER!";
      led.Color('R');
      fan(5);
      alarm();
    }

    if(disp == true)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0, 12);
      display.print("CO2:");
      display.print(TVOC);
      display.setCursor(64, 12);
      display.print("COV:");
      display.print(TVOC);
      display.setCursor(0, 30);
      display.print("Qualite de l'air:");
      display.setTextSize(3);
      display.setCursor(0, 40);
      display.print(airQuality);
      if(mute == true)
      {
        display.setTextSize(1);
        display.setCursor(0, 1);
        display.print("-----En Sourdine-----");
      }
      display.display();
    }
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
  display.setCursor(7, 17);
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

  display.setCursor(7, 37);
  display.print("Humid:");
  display.print(h);
  display.print("%");
  
  if(mute == true)
  {
    display.setTextSize(1);
    display.setCursor(5, 1);
    display.print("----En Sourdine----");
  }

  display.display();
}

//mian loop
void loop()
{ 
  int input = Serial.read();
  switch(input)
  {
    case 'C': if(farenheit == false){farenheit = true;}else if(farenheit == true){farenheit = false;}; break;
    case 'M': if(mute == false){mute = true;}else if(mute == true){mute = false;}; break;
  }
  
  //reads gas sensor when button is pressed
  if (digitalRead(buttonPin) == HIGH)
  {
    
    for(int i = 0; i < 50; i++)
    {
      gasMode(true);
      delay(200);
    }
  }
  
  tempMode();
  gasMode(false);
}