/*
  PCEmon_PLUS sketch

  On most boards (including the Adafruit M0 Adalogger), "Serial" refers to the USB
  Serial port attached to the Serial Monitor, and "Serial1" refers to the hardware
  serial port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  This version of the sketch is intended for "rev. C" of the board

  (c) 2019, 2020 by David Shadoff
*/

// Strictly speaking, "Serial1" is used to communicate to the PCE side
// However, when written in code, that is nearly identical to "Serial"
// which is the Terminal side, so confusion can result.
//
// I am using an alias of "PCE" to help reduce this confusion even
// though syntax highlighting doesn't show it in the same way as Serial1
//


#include <SD.h>

#include "define.h"
#include "common.h"
#include "escape.h"

extern void altLoop();
extern void setAltMode();

File dataFile;


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

  digitalWrite(plusModeLEDPin, LOW);  // switch PCE to original mode 
  digitalWrite(origModeLEDPin, HIGH); // 

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
// This function will be relocated to the "alternate commands" side soon
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
int a1, a2, a4;
const int touchThreshold = 100;
int numbytes, readsize, bytesread;
boolean flush = false;
char buffer[49];

  if (modeMonitor == true) {
    altLoop();
    return;
  }
 
  a1 = analogRead(joyTouchPin);
  a2 = analogRead(compTouchPin);
  a4 = analogRead(plusModeTouchPin);

  // deal with touchpad inputs
  //
  if (a1 < touchThreshold)
    switchPCEInput(JOYPAD_IN);

  else if (a2 < touchThreshold)
    switchPCEInput(COMPUTER_IN);

  else if (a4 < touchThreshold) {
    modeMonitor = true;
    setAltMode();
  }

  if (Serial.available()) {      // If anything comes in Serial (USB),
    c = Serial.read();

//  Filtering inputs was found to be a problem in a previous version,
//  so all input filtering is now removed, and the job of the touchpads
//  on the PC board
//
//    if (c == 'Z')
//      getbram();
//    else {
//    }
    switchPCEInput(COMPUTER_IN);
    PCE.write(c);   // read it and send it out Serial1 (pins 0 & 1)
    return;
  }

  delay(4);                               // pause to prevent more than 250 USB packets per second

  numbytes = PCE.available();
  if (numbytes > 0) {                     // If anything comes in Serial1 (pins 0 & 1)
    readsize = min(48, numbytes);
    bytesread = PCE.readBytes(buffer, readsize);
    Serial.write(buffer, bytesread);      // read it and send it out Serial (USB)
    flush = true;
  }
  else if (flush == true) {
    Serial.flush();
    flush = false;
  }
}
