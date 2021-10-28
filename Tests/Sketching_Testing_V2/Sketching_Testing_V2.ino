#include <SPI.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1325.h>
#include "lcdgfx.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

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
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// this is software SPI, slower but any pins
//Adafruit_SSD1325 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// this is for hardware SPI, fast! but fixed oubs
//Adafruit_SSD1325 display(OLED_DC, OLED_RESET, OLED_CS);

DisplaySSD1325_128x64_SPI display(OLED_RESET,{-1, OLED_CS, OLED_DC, 0,-1,-1}); 

double distance;
int i;
int j;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ss.begin(GPSBaud);
  display.begin();

  distance = 0.0;  
  i = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      textDemo();
      i++;
    }
  }
}

static void textDemo()
{
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    //display.setColor(GRAY_COLOR4(255));
    display.printFixed(0,  8, "Normal text", STYLE_NORMAL);
    //display.setColor(GRAY_COLOR4(192));
    display.printFixed(0, 16, "Bold text", STYLE_BOLD);
    //display.setColor(GRAY_COLOR4(128));
    display.printFixed(0, 24, "Italic text", STYLE_ITALIC);
    delay(3000);
}

/*void testTemplate() {
  display.clear();
  
    if (gps.time.hour() < 10) display.print("0");
    display.print(gps.time.hour());
    display.print(":");
    if (gps.time.minute() < 10) display.print("0");
    display.print(gps.time.minute());
    display.print(":");
    if (gps.time.second() < 10) display.print("0");
    display.print(gps.time.second());
    display.print(".");
    if (gps.time.centisecond() < 10) display.print("0");
    display.print(gps.time.centisecond());

  display.print("i is currently: ");
  display.print(i);
  display.print("Lat: ");
  display.print(gps.location.lat());
  display.print("Long: ");
  display.print(gps.location.lng());
}*/
