// =========================================================================
// RESUME PROJECT: Hutemp Data Logger
// VERSION: 1.2.3
// DATE: August 15, 2026
// AUTHOR: Tj Johnston, K4TFJ
// 
// HARDWARE COMPONENTS:
// - Microcontroller: Arduino Duemilanove (ATmega328P)
// - Temperature/Humidity Sensor: HiLetgo DHT11
// - Real Time Clock: HiLetgo DS3231 RTC
// - SD Card Module: HiLetgo SD TF Card Adapter
// - Display: 128x64 I2C OLED (Origin/Brand Unknown)
//
// HARDWARE RESTRAINTS: 
// Limited ATmega328P dynamic RAM. Standard graphics libraries cause allocation 
// failures when paired with the SD library buffer. Uses the text-only 
// SSD1306Ascii library to eliminate the 1024-byte frame buffer requirement.
//
// STATUS: 
// Standard integer baseline is stable. Current breadboard jumper wire links 
// are suspect; final deployment will be permanently soldered on a custom PCB. 
// User will manually adjust final logging interval configuration on Monday.
// =========================================================================

//#include <SPI.h>               // Core
#include <SD.h>                // Core
//#include <Wire.h>              // Core

#include <DHT.h>               // DHT sensor library by Adafruit (v1.4.7)
#include <RTClib.h>            // RTClib by Adafruit (v2.1.4)
//#include "SSD1306Ascii.h"      // SSD1306Ascii by Bill Greiman (v1.3.5)
#include "SSD1306AsciiWire.h"  // SSD1306Ascii by Bill Greiman (v1.3.5)

// --- LOGGING INTERVAL CONFIGURATION --- 
#define LOG_INTERVAL_SECONDS 600 
#define SCREEN_ADDRESS 0x3C 
SSD1306AsciiWire display; 

#define DHTPIN 2 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 

const int chipSelect = 4; 
File myFile; 
RTC_DS1307 rtc; // Compatible with DS3231 hardware addresses

char timestamp[25]; 
char screenTime[20]; 
char temperature_c[5]; 
char temperature_f[5]; 
char humidity[5]; 

// Global variables to track Min and Max values 
int min_c = 999, max_c = -999; 
int min_f = 999, max_f = -999; 
int min_h = 999, max_h = -999; 

bool firstReadingCaptured = false; 
bool sdError = false; // Flag to track if the SD card logging fails 

void setup() { 
  Serial.begin(9600); 
  while (!Serial); 
  
  Wire.begin(); 
  Wire.setClock(400000L); 
  display.begin(&Adafruit128x64, SCREEN_ADDRESS); 
  display.setFont(System5x7); 
  
  display.clear(); 
  display.println(F("Hutemp logging")); 
  
  dht.begin(); 
  
  if(!rtc.begin()) { 
    Serial.println(F("Couldn't find RTC")); 
    while (1); 
  } else { 
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  } 
  
  if(!rtc.isrunning()) { 
    Serial.println(F("RTC is NOT running!")); 
  } 
  
  Serial.print(F("Initializing SD card...")); 
  if(!SD.begin(chipSelect)) { 
    Serial.println(F("initialization failed!")); 
    sdError = true; // Flag the error if card fails initialization at startup 
    return; 
  } 
  Serial.println(F("initialization done.")); 
  
  myFile = SD.open("DATA.csv", FILE_WRITE); 
  if (myFile) { 
    Serial.println(F("File opened ok")); 
    myFile.println(F("Date,Time,Temperature C,Temperature F,Humidity")); 
    myFile.close(); 
    sdError = false; 
  } else { 
    sdError = true; 
  } 
} 

void loop() { 
  // 1. Get Time 
  DateTime now = rtc.now(); 
  sprintf(timestamp, "%02d/%02d/%04d, %02d:%02d:%02d", now.month(), now.day(), now.year(), now.hour(), now.minute(), now.second()); 
  sprintf(screenTime, "%02d/%02d/%04d %02d:%02d", now.month(), now.day(), now.year(), now.hour(), now.minute()); 

  // 2. Get Sensor Data 
  float t_raw = dht.readTemperature(); 
  float f_raw = dht.readTemperature(true); 
  float h_raw = dht.readHumidity(); 
  
  if (isnan(t_raw) || isnan(f_raw) || isnan(h_raw)) { 
    Serial.println(F("Failed to read from DHT sensor! Skipping this loop...")); 
    delay(LOG_INTERVAL_SECONDS * 1000UL); 
    return; 
  } 
  
  // Convert floats to integers immediately 
  int t = (int)(t_raw + 0.5); 
  int f = (int)(f_raw + 0.5); 
  int h = (int)(h_raw + 0.5); 
  
  // 3. Track Min and Max Records safely using integers 
  if (!firstReadingCaptured) { 
    min_c = max_c = t; 
    min_f = max_f = f; 
    min_h = max_h = h; 
    firstReadingCaptured = true; 
  } else { 
    if (t < min_c) min_c = t; 
    if (t > max_c) max_c = t; 
    if (f < min_f) min_f = f; 
    if (f > max_f) max_f = f; 
    if (h < min_h) min_h = h; 
    if (h > max_h) max_h = h; 
  } 
  
  // Render text arrays cleanly from the rounded integers 
  itoa(t, temperature_c, 10); 
  itoa(f, temperature_f, 10); 
  itoa(h, humidity, 10); 
  
  // 4. Print detailed logs to Serial 
  Serial.print(timestamp); 
  Serial.print(F(" | C: ")); Serial.print(t); Serial.print(F(" (Min: ")); Serial.print(min_c); Serial.print(F(", Max: ")); Serial.print(max_c); Serial.print(F(")")); 
  Serial.print(F(" | F: ")); Serial.print(f); Serial.print(F(" (Min: ")); Serial.print(min_f); Serial.print(F(", Max: ")); Serial.print(max_f); Serial.print(F(")")); 
  Serial.print(F(" | Hum: ")); Serial.print(h); Serial.print(F("% (Min: ")); Serial.print(min_h); Serial.print(F("%, Max: ")); Serial.print(max_h); Serial.println(F("%)")); 

  // 5. Log to SD Card with real-time error tracking 
  myFile = SD.open("DATA.csv", FILE_WRITE); 
  if (myFile) { 
    myFile.print(timestamp); myFile.print(F(", ")); 
    myFile.print(t); myFile.print(F(", ")); 
    myFile.print(f); myFile.print(F(", ")); 
    myFile.println(h); 
    myFile.close(); 
    sdError = false; // Write succeeded, system healthy! 
  } else { 
    Serial.println(F("SD Write Error detected!")); 
    sdError = true; // Write failed, toggle error flag 
  } 
  
  // 6. Update OLED Display 
  display.clear(); 
  
  // Hand-centered double height header with dynamic asterisk alert flag 
  display.set2X(); 
  if (sdError) { 
    display.println(F(" LIVE DATA*")); // Error state: Shows asterisk indicator 
  } else { 
    display.println(F(" LIVE DATA ")); // Healthy state: Keeps space layout clean 
  } 
  display.set1X(); 
  
  display.println(F("       NOW  MIN  MAX")); 
  display.println(F("----------------------")); 
  
  // C Row 
  display.print(F("C:     ")); display.print(t); 
  display.print(F("   ")); display.print(min_c); 
  display.print(F("   ")); display.println(max_c); 
  
  // F Row 
  display.print(F("F:     ")); display.print(f); 
  display.print(F("   ")); display.print(min_f); 
  display.print(F("   ")); display.println(max_f); 
  
  // Humidity Row 
  display.print(F("Hum:   ")); display.print(h); 
  display.print(F("   ")); display.print(min_h); 
  display.print(F("   ")); display.print(max_h); display.println(F("%")); 
  
  display.print(F("Log: ")); display.println(screenTime); 
  
  delay(LOG_INTERVAL_SECONDS * 1000UL); 
}
