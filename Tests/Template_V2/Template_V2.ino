#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>
#include <NMEAGPS.h>
#include <NeoSWSerial.h>
#include <EEPROM.h>

// If using software SPI, define CLK and MOSI
#define OLED_CLK 7
#define OLED_MOSI 6

// These are neede for both hardware & softare SPI
#define OLED_CS 5
#define OLED_RESET 4
#define OLED_DC 8

#define RESET_BUTTON 10

#define ALS_IN 2
#define ALS_OUT 3

#define TURN_IN_X 11
#define TURN_OUT 12
#define TURN_IN_Y 13

//#define ss Serial

static const int RXPin = 0, TXPin = 1;
static const uint32_t GPSBaud = 9600;


// The GPS object
NMEAGPS gps;
gps_fix fix;

// The serial connection to the GPS device
NeoSWSerial ss(RXPin, TXPin);

// this is software SPI, slower but any pins
Adafruit_SSD1325 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

float distance = 0.0f;
float avgSpeed = 0.0f;
uint32_t rideTime = 0;
float prevCount = 0.0f;
NeoGPS::Location_t prevLoc;
uint32_t onTimeStamp;
int buttonState;
int buttonState2 = HIGH;
int turnStateX;
int turnStateY;
int turnLEDState;
int reInitTime = 1;
uint32_t prevTime;
uint32_t newTime;
uint32_t alsTimeStamp;
float tmpHeading;
int prevState;
int alsPower;
long interval = 500;
long prevMillis = 0;
char direction[17][4] = {"N","NNE","NE","ENE","E","ESE","SE","SSE","S","SSW","SW","WSW","W","WNW","NW","NNW","N"};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);

  ss.begin(GPSBaud);
  pinMode(RESET_BUTTON, INPUT);
  pinMode(ALS_IN, INPUT);
  pinMode(TURN_IN_X, INPUT);
  pinMode(TURN_IN_Y, INPUT);
  pinMode(TURN_OUT, OUTPUT);
  pinMode(ALS_OUT, OUTPUT);
  display.begin();
  display.clearDisplay();
  reInitTime = 1;
  digitalWrite(ALS_OUT, HIGH);
  prevState = HIGH;
  EEPROM.get(0, distance);
  EEPROM.get(4, avgSpeed);
  EEPROM.get(8, rideTime);
  EEPROM.get(12, prevCount);
  EEPROM.get(16, prevTime);
  //stateInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (gps.available(ss)) { 
    processLoop();
  }
  turnCheck();
}

void stateInit(void) {
  digitalWrite(ALS_OUT, LOW);
  delay(40);
  digitalWrite(ALS_OUT, HIGH);
}

void processLoop(void) {
  buttonState = digitalRead(RESET_BUTTON);
  buttonState2 = digitalRead(ALS_IN);
  fix = gps.read();
  buttonCheck();
  testTemplate();
  EEPROM.put(0, distance);
  EEPROM.put(4, avgSpeed);
  EEPROM.put(8, rideTime);
  EEPROM.put(12, prevCount);
}

void turnCheck(void) {
  turnStateX = digitalRead(TURN_IN_X);
  turnStateY = digitalRead(TURN_IN_Y);
  turnLEDState = digitalRead(TURN_OUT);
  if (turnStateX == HIGH || turnStateY == HIGH) {
    unsigned long currMillis = millis();
    if (currMillis - prevMillis > interval) {
      prevMillis = currMillis;
      if (turnLEDState == LOW) {
        digitalWrite(TURN_OUT, HIGH);
      } else {
        digitalWrite(TURN_OUT, LOW);
      }
    }
  }
  else {
    digitalWrite(TURN_OUT, LOW);
  }
}

void buttonCheck(void) {
  if (buttonState == LOW) { // RESET_BUTTON
    resetButton();
  } else {
    newSpeedAvg();
    newTotalDistance();
    newRideTime();
  }

/*
  if (buttonState2 == HIGH) { // ALS
    if (prevState == LOW) {
      alsTimeStamp = (NeoGPS::clock_t) fix.dateTime;
      alsPower = 1;
    } else if (((NeoGPS::clock_t) fix.dateTime) - alsTimeStamp >= 5 && alsPower) {
      digitalWrite(ALS_OUT, LOW);
      delay(40);
      digitalWrite(ALS_OUT, HIGH);
      alsPower = 0;
    }
  } else {
    if (prevState == HIGH) {
      digitalWrite(ALS_OUT, LOW);
      delay(40);
      digitalWrite(ALS_OUT, HIGH);
      delay(40);
      digitalWrite(ALS_OUT, LOW);
      delay(40);
      digitalWrite(ALS_OUT, HIGH);
      delay(40);
      digitalWrite(ALS_OUT, LOW);
      delay(40);
      digitalWrite(ALS_OUT, HIGH);
      delay(40);
      digitalWrite(ALS_OUT, LOW);
      delay(40);
      digitalWrite(ALS_OUT, HIGH);
    }
  }
  prevState = buttonState2;
  */

  if (buttonState2 == HIGH) { // ALS
    if (prevState == LOW && !alsPower) {
      digitalWrite(ALS_OUT, LOW);
      delay(75);
      digitalWrite(ALS_OUT, HIGH);
    }
  } else {
    if (prevState == HIGH ) {
      alsTimeStamp = (NeoGPS::clock_t) fix.dateTime;
      alsPower = 1;
    } else if (((NeoGPS::clock_t) fix.dateTime) - alsTimeStamp >= 5 && alsPower) {
      digitalWrite(ALS_OUT, LOW);
      delay(75);
      digitalWrite(ALS_OUT, HIGH);
      delay(75);
      digitalWrite(ALS_OUT, LOW);
      delay(75);
      digitalWrite(ALS_OUT, HIGH);
      delay(75);
      digitalWrite(ALS_OUT, LOW);
      delay(75);
      digitalWrite(ALS_OUT, HIGH);
      delay(75);
      digitalWrite(ALS_OUT, LOW);
      delay(75);
      digitalWrite(ALS_OUT, HIGH);
      alsPower = 0;
    }
  }
  prevState = buttonState2;
}

void newRideTime(void) {
  if (reInitTime && ((NeoGPS::clock_t) fix.dateTime != 0)) {
    onTimeStamp = (NeoGPS::clock_t) fix.dateTime;
    reInitTime = 0;
    prevTime = rideTime;
    EEPROM.put(16, prevTime);
  } else if (!reInitTime && ((NeoGPS::clock_t) fix.dateTime != 0)) {
    newTime = ((NeoGPS::clock_t) fix.dateTime) - onTimeStamp;
    rideTime = newTime + prevTime;
  }
}

void resetButton(void) {
  distance = 0.0f;
  avgSpeed = 0.0f;
  rideTime = 0;
  prevCount = 0.0f;
  prevTime = 0;
  onTimeStamp = (NeoGPS::clock_t) fix.dateTime;
  EEPROM.put(0, 0.0f);
  EEPROM.put(4, 0.0f);
  EEPROM.put(8, 0.0f);
  EEPROM.put(12, 0.0f);
}

void newTotalDistance(void) {
  if (fix.valid.location && fix.speed_mph() >= 1.0f) {
    if (prevLoc.latF() != 0.0f && prevLoc.lonF() != 0.0f) {
      distance += fix.location.DistanceMiles(prevLoc);
      Serial.print(fix.location.DistanceMiles(prevLoc));
    }
    prevLoc = fix.location;
  }
}

void newSpeedAvg(void) {
  if (fix.speed_mph() >= 0.5f) {
    avgSpeed = ((prevCount * avgSpeed + fix.speed_mph()) / (prevCount + 1.0f));
    prevCount++;
  }
}

String printHeading(float heading) {
  if (heading <= 22.5f && heading >= 337.5f) {
    return "N";
  } else if (heading > 22.5f && heading < 67.5f) {
    return "NE";
  } else if (heading >= 67.5f && heading <= 112.5f) {
    return "E";
  } else if (heading > 112.5f && heading < 157.5f) {
    return "SE";
  } else if (heading >= 157.5f && heading <= 202.5f) {
    return "S";
  } else if (heading > 202.5f && heading < 247.5f) {
    return "SW";
  } else if (heading >= 247.5f && heading <= 292.5f) {
    return "W";
  } else if (heading >292.5f && heading < 337.5f) {
    return "NW";
  } else {
    return "";
  }
}

void testTest(void) {
  display.clearDisplay();
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.print("Test");
  display.display();
}

void testTemplate(void) {
  display.clearDisplay();
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  

  display.setTextSize(1);
  display.setCursor(0, 0);
  if (fix.heading() != 0.0f) display.print(printHeading(fix.heading()));
  //display.print(fix.heading(), 2);
  //display.print(" deg N");
  
  display.setCursor(80, 0);
  if (((fix.dateTime.hours + 6) % 12) < 10 && ((fix.dateTime.hours + 6) % 12) != 0) display.print(F("0"));
  if ((fix.dateTime.hours + 6) % 12 == 0) display.print("12");
  else display.print(((fix.dateTime.hours + 6) % 12));
  display.print(F(":"));
  if (fix.dateTime.minutes < 10) display.print(F("0"));
  display.print(fix.dateTime.minutes);
  display.print(F(":"));
  if (fix.dateTime.seconds < 10) display.print(F("0"));
  display.print(fix.dateTime.seconds);
  


  
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.print(fix.speed_mph());
  display.setTextSize(1);
  display.print(avgSpeed);
  display.print(" mph");
  
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(distance);
  display.print(" miles");
  display.setCursor(0, 48);
  if ((rideTime / 3600) < 10) display.print(F("0"));
  display.print(rideTime / 3600);
  display.print(F(":"));
  if ((rideTime / 60) < 10) display.print(F("0"));
  display.print(rideTime / 60);
  display.print(F(":"));
  if ((rideTime % 60) < 10) display.print(F("0"));
  display.print(rideTime % 60);

  display.setCursor(0, 56);
  if (fix.latitude() != 0.0f && fix.longitude() != 0.0f) {
    display.print(F("("));
    display.print(fix.latitude(), 4);
    display.print(F(","));
    display.print(" ");
    display.print(fix.longitude(), 4);
    display.print(F(")"));
  }
  display.display();
}
