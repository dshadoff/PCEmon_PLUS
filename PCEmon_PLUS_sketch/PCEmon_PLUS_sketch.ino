/*
  PCEmon_accel sketch

  On most boards (including the Adafruit M0 Adalogger), "Serial" refers to the USB
  Serial port attached to the Serial Monitor, and "Serial1" refers to the hardware
  serial port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  by David Shadoff
*/

// Strictly speaking, "Serial1" is used to communicate to the PCE side
// However, when written in code, that is nearly identical to "Serial"
// which is the Terminal side, so confusion can result.
//
// I am implementing an alias of "PCE" to help reduce this confusion
// even though syntax highlighting doesn't show it in the same way as
// Serial1
//


#include <SD.h>

#include "define.h"
#include "common.h"
#include "escape.h"

extern void altLoop();
extern void setAltMode();

File dataFile;


#define CMD_JOYPAD_IN       ','
#define CMD_COMPUTER_IN     '.'
#define CMD_SWITCHMODE      '|'

void setup()
{
  int i;
  char c;

  // setup Serial ports
  Serial.begin(19200);  // computer side
  PCE.begin(19200); // PCE Side

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

  drainSerialQueue();
  modeMonitor = false;

  // detect echo on PCE side
  PCE_Echo = false;
  PCE.print("O");
  delay(100);

  // This implements a timeout; the above letter is sent,
  // and may or may not be echoed... we briefly listen for
  // the echo in order to determine whether echo is ON at
  // the time of microcontroller startup
  
  while (i < 100) {
    if (PCE.available()) {
      c = PCE.read();
      if (c == 'O') {
        PCE_Echo = true;
        break;
      }
    }
    i++;
    delay(1);
  }

  // if echo is off, turn it on
  if (PCE_Echo == false)
    PCE.println(" E");
  else
    PCE.println("");

  PCE_Echo = true;
}


//
// Add some instructions to 'accelerate' the grab/download of BRAM data
// as an example of what can be done as part of acceleration
//
void getbram()
{
int i;
int c;

  if (cardPresent) {
    if (SD.exists("BRAM.sav")) {
      Serial.println("mb128.sav exists; removing");
      SD.remove("BRAM.sav");
    }

    Serial.println("Opening BRAM.sav");
    dataFile = SD.open("BRAM.sav", FILE_WRITE);
  }

  Serial.println("Get BRAM:");
  switchPCEInput(COMPUTER_IN);
  
  // if echo was on, temporarily disable echo
  //
  if (PCE_Echo == true) {
    PCE.println("O E");
    drainSerialQueue();  // consume the response
  }

  PCE.println("<");
  delay(200);
  PCE.println("S");

  i = 0;
  while (i < 2048) {
    if (PCE.available()) {
      c = PCE.read();
      if (cardPresent)
        dataFile.write(c);
      else
        Serial.write(c);
      i++;
    }
  }

  if (cardPresent)
    dataFile.close();
    
  // if echo was originally on, re-enable echo
  //
  if (PCE_Echo == true) {
    PCE.println("O E");
    drainSerialQueue();  // consume the response
  }

  Serial.println("done");
}


void loop() {
int c;

  if (modeMonitor == true) {
    altLoop();
    return;
  }
  
  if (Serial.available()) {      // If anything comes in Serial (USB),
    c = Serial.read();

    if (c == CMD_SWITCHMODE) {
      modeMonitor = true;
      setAltMode();
    }
    else if (c == 'Z')
      getbram();
    else if (c == CMD_JOYPAD_IN) {           // Note: these keys may be changed in the future
      switchPCEInput(JOYPAD_IN);
    }
    else if (c == CMD_COMPUTER_IN) {           // Note: these keys may be changed in the future
      switchPCEInput(COMPUTER_IN);
    }
//    else if (c == '?') {           // Note: these keys may be changed in the future
//      Serial.write(FG_CYAN "HELP ME" THM_ADDRESS);
//      PCE.write(c);
//    }
//    else if (c == '\x0a') {
//      Serial.println("");
//    }
    else

//      printhex2(c);
//      Serial.print(" ");
      PCE.write(c);   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (PCE.available()) {            // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(PCE.read());       // read it and send it out Serial (USB)
  }
}
