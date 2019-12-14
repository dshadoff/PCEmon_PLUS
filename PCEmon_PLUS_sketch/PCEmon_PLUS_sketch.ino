/*
  PCEmon_plus sketch

  On most boards (including the Adafruit M0 Adalogger), "Serial" refers to the USB
  Serial port attached to the Serial Monitor, and "Serial1" refers to the hardware
  serial port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  by David Shadoff
*/

#include <SD.h>

// This sketch is made specifically for the Adafruit M0 Feather Adalogger

const int chipSelect   = 4;   // Pin used for chip select for on-board SDCard

const int joypadPin    = 11;  // if set high, triggers joypad passthru to port
const int computerPin  = 10;  // This should be the inverse of joypadPin (status LED)

const int attnPin      = 12;  // joypad CLR pin.
                              // Future proposed functionality:
                              //   if PC-E sets this high for >16ms,
                              //   it is a signal to switch back to MCU mode

bool cardPresent;
File dataFile;


void setup()
{
  // setup Serial ports
  Serial.begin(19200);  // computer side
  Serial1.begin(19200); // PCE Side

  // setup GPIO pins
  pinMode(joypadPin,   OUTPUT);
  pinMode(computerPin, OUTPUT);
  pinMode(attnPin,     INPUT);

  digitalWrite(joypadPin,   LOW);  // switch PCE to computer input (not joypad)
  digitalWrite(computerPin, HIGH); // 

  // Detect and initialze SDCard access hardware
  cardPresent = true;
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed or not present");
    cardPresent = false;
  }
}



void loop() {
int c;

  if (Serial.available()) {      // If anything comes in Serial (USB),
    c = Serial.read();
    
    if (c == ',') {              // Note: these keys may be changed in the future
      digitalWrite(joypadPin,HIGH);
      digitalWrite(computerPin,LOW);
    }
    else if (c == '.') {         // Note: these keys may be changed in the future
      digitalWrite(joypadPin,LOW);
      digitalWrite(computerPin,HIGH);
    }
    else
      Serial1.write(c);   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial1.available()) {        // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }
}
