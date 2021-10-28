#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <math.h>

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
Adafruit_SSD1325 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// this is for hardware SPI, fast! but fixed oubs
//Adafruit_SSD1325 display(OLED_DC, OLED_RESET, OLED_CS);

double distance;
float prevLat;
float prevLng;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  ss.begin(GPSBaud);
  display.begin();

  distance = 0.0;
  prevLat = gps.location.lat();
  prevLng = gps.location.lng();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      testTemplate();
      distance += (haversine(gps.location.lat(), gps.location.lng(), prevLat, prevLng)/(364000));
      prevLat = gps.location.lat();
      prevLng = gps.location.lng();
    }
  }
}

void testTemplate(void) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  
  display.print(gps.speed.mph());
  display.print(" mph");
  display.setTextSize(1);
  display.setCursor(0, 17);
  display.print("Heading: ");
  display.print(gps.course.deg());
  display.setCursor(0, 25);
  display.print("Distance: ");
  display.print(distance);
  display.setCursor(0, 33);
  display.print("Time: ");
  display.print(gps.time.value());
  display.setCursor(0, 41);
  display.print("Lat: ");
  display.print(gps.location.lat(), 6);
  display.setCursor(0, 49);
  display.print("Long: ");
  display.print(gps.location.lng(), 6);
  display.display();
}

double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double rEarth = 6371000.0; // in meters
    double x = pow( sin( ((lat2 - lat1)*M_PI/180.0) / 2.0), 2.0 );
    double y = cos(lat1*M_PI/180.0) * cos(lat2*M_PI/180.0);
    double z = pow( sin( ((lon2 - lon1)*M_PI/180.0) / 2.0), 2.0 );
    double a = x + y * z;
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
    double d = rEarth * c;
    // Serial.printlnf("%12.9f, %12.9f, %12.9f, %12.9f, %12.9f, %12.9f", x, y, z, a, c, d);
    return d; // in meters
}
