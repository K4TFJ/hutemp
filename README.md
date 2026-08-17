
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

COMMENTS:
Asterisks to right of LIVE DATA indicates issue with SD card


Wiring Information

-------------------------------

ARD     HiLetgo DS3231 RTC

nc      1 32K

nc      2 SQW

A5      3 SCL

A4      4 SDA

VCC     5 VCC

GND     6 GND

HiLetgo SD TF Card Adapter

GND     1 GND

VCC     2 VCC

D12     3 MISO

D11     4 MOSI

D13     5 SCK

D4      6 CS

HiLetgo DHT11 Temp/Humidity Sensor

VCC     1 VCC

D2      2 OUT

GND     3 GND

128x64 I2C OLED (0x3c)

GND     1 GND

VCC     2 VCC

A5      3 SCL

A4      4 SDA

