/*
  PCEmon_accel sketch

  On most boards (including the Adafruit M0 Adalogger), "Serial" refers to the USB
  Serial port attached to the Serial Monitor, and "Serial1" refers to the hardware
  serial port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  by David Shadoff
*/

// Strictly speaking, we should be using "Serial1" to communicate to
// the PCE side. However, when written in code, that is nearly identical
// to "Serial" which is the Terminal side, so confusion results.
//
// I am implementing an alias of "PCE" to help reduce this confusion
// even though it causes problems with syntax highlighting
//
#define PCE     Serial1


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

bool PCE_Echo;


void drainSerialQueue()
{
  char c;

  //drain the queue
  PCE.println("");      // in case there's junk on the command line
  delay(100);           // wait 100 milliseconds
  while (PCE.available()) {
    c = PCE.read();
    delay(3);
  }
}

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

  if (Serial.available()) {      // If anything comes in Serial (USB),
    c = Serial.read();

    if (c == 'Z')
      getbram();
    else if (c == ',') {           // Note: these keys may be changed in the future
      digitalWrite(joypadPin,HIGH);
      digitalWrite(computerPin,LOW);
    }
    else if (c == '.') {           // Note: these keys may be changed in the future
      digitalWrite(joypadPin,LOW);
      digitalWrite(computerPin,HIGH);
    }
    else

      PCE.write(c);   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (PCE.available()) {            // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(PCE.read());   // read it and send it out Serial (USB)
  }
}
