#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>
//#include <TinyGPS++.h>
//#include <SoftwareSerial.h>
#include <NMEAGPS.h>
#include <NeoSWSerial.h>

// If using software SPI, define CLK and MOSI
#define OLED_CLK 13
#define OLED_MOSI 11

// These are neede for both hardware & softare SPI
#define OLED_CS 10
#define OLED_RESET 9
#define OLED_DC 8

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
//TinyGPSPlus gps;
NMEAGPS gps;
gps_fix fix;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);
NeoSWSerial ss(RXPin, TXPin);

// this is software SPI, slower but any pins
Adafruit_SSD1325 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// this is for hardware SPI, fast! but fixed oubs
//Adafruit_SSD1325 display(OLED_DC, OLED_RESET, OLED_CS);

double distance;
int i;
int j;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);

  ss.begin(GPSBaud);
  display.begin();

  distance = 0.0;  
  i = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  while (gps.available(ss)) {
    fix = gps.read();
    testTemplate();
  }
}

void testTemplate() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
    if (fix.dateTime.hours < 10) display.print(F("0"));
    display.print(fix.dateTime.hours);
    display.print(F(":"));
    if (fix.dateTime.minutes < 10) display.print(F("0"));
    display.print(fix.dateTime.minutes);
    display.print(F(":"));
    if (fix.dateTime.seconds < 10) display.print(F("0"));
    display.print(fix.dateTime.seconds);

  display.setCursor(0, 8);
  display.print("i is currently: ");
  display.print(i);
  display.setCursor(0, 16);
  display.print("Lat: ");
  display.print(fix.latitudeL());
  display.setCursor(0, 24);
  display.print("Long: ");
  display.print(fix.longitudeL());
  display.setCursor(0, 32);
  display.print(fix.speed_mph());
  display.print(" mph");
  display.setCursor(0, 40);
  display.print(fix.dateTime);
  display.display();
}
