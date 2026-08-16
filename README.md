=========================================================================
RESUME PROJECT: Hutemp Data Logger
AUTHOR: Tj Johnston, K4TFJ

HARDWARE COMPONENTS:
 - Microcontroller: Arduino Duemilanove (ATmega328P)
 - Temperature/Humidity Sensor: HiLetgo DHT11
 - Real Time Clock: HiLetgo DS3231 RTC
 - SD Card Module: HiLetgo SD TF Card Adapter
 - Display: 128x64 I2C OLED (Origin/Brand Unknown)

HARDWARE RESTRAINTS: 
Limited ATmega328P dynamic RAM. Standard graphics libraries cause allocation 
failures when paired with the SD library buffer. Uses the text-only 
SSD1306Ascii library to eliminate the 1024-byte frame buffer requirement.

STATUS: 
Standard integer baseline is stable. Current breadboard jumper wire links 
are suspect; final deployment will be permanently soldered on a custom PCB. 
User will manually adjust final logging interval configuration on Monday.
=========================================================================
