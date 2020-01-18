//
// common.c
//
// This file contains all sorts of common utility routines, such as
// string/hex conversion, serial I/O and buffer control, etc.
//
// (c) 2019, 2020 by David Shadoff


#include <Arduino.h>
#include <SD.h>

#include "define.h"
#include "escape.h"


bool cardPresent;

bool PCE_Echo = false;
bool modeMonitor;


bool checkForKey()
{
  return(Serial.available());
}

char waitKeyEnterEscape(bool beep)
{
char c;

  while (1) {
    while (!checkForKey());
    
    c = Serial.read();
    if ((c == KEY_ENTER) || (c == KEY_ESCAPE))
      break;

    if (beep == true) {
      Serial.write(BELL);
    }
  }

  return(c);
}

char getKeyFromList(char * list, bool beep)
{
int i;
char c;
bool exit;

  exit = false;
  while (exit == false) {
    while (!checkForKey());

    c = Serial.read();

    if (c == KEY_ESCAPE) {
      exit = true;
      break;
    }

    for (i = 0 ; i < strlen(list); i++) {
      if ( c == *(list + i) ) {
        exit = true;
        break;
      }
    }

    if ((exit != true) && (beep == true)) {
      Serial.write(BELL);
    }
  }

  return(c);
}

// single-code keystrokes:
//#define KEY_BACKSPACE    0x08
//#define KEY_TAB          0x09
//#define KEY_ESCAPE       0x1B
//#define KEY_DELETE       0x7F

// multi-code keystrokes:
//#define KEY_F1           0xFF01    // 0x1B 0x5B 0x31 0x31 0x7E via serial
//#define KEY_F2           0xFF02    // 0x1B 0x5B 0x31 0x32 0x7E via serial
//#define KEY_F3           0xFF03    // 0x1B 0x5B 0x31 0x33 0x7E via serial
//#define KEY_F4           0xFF04    // 0x1B 0x5B 0x31 0x34 0x7E via serial
//#define KEY_F5           0xFF05    // 0x1B 0x5B 0x31 0x35 0x7E via serial
//#define KEY_F6           0xFF06    // 0x1B 0x5B 0x31 0x37 0x7E via serial
//#define KEY_F7           0xFF07    // 0x1B 0x5B 0x31 0x38 0x7E via serial
//#define KEY_F8           0xFF08    // 0x1B 0x5B 0x31 0x39 0x7E via serial
//#define KEY_F9           0xFF09    // 0x1B 0x5B 0x32 0x30 0x7E via serial
//#define KEY_F10          0xFF0A    // 0x1B 0x5B 0x32 0x31 0x7E via serial
//#define KEY_F11          0xFF0B    // 0x1B 0x5B 0x32 0x33 0x7E via serial
//#define KEY_F12          0xFF0C    // 0x1B 0x5B 0x32 0x34 0x7E via serial
//
//#define KEY_HOME         0xFF31    // 0x1B 0x5B 0x31 0x7E via serial
//#define KEY_INSERT       0xFF32    // 0x1B 0x5B 0x32 0x7E via serial
//#define KEY_END          0xFF34    // 0x1B 0x5B 0x34 0x7E via serial
//#define KEY_PAGEUP       0xFF35    // 0x1B 0x5B 0x35 0x7E via serial
//#define KEY_PAGEDOWN     0xFF36    // 0x1B 0x5B 0x36 0x7E via serial
//
//#define KEY_UPARROW      0xFF41    // 0x1B 0x5B 0x41 via serial
//#define KEY_DOWNARROW    0xFF42    // 0x1B 0x5B 0x42 via serial
//#define KEY_RIGHTARROW   0xFF43    // 0x1B 0x5B 0x43 via serial
//#define KEY_LEFTARROW    0xFF44    // 0x1B 0x5B 0x44 via serial
//
//#define KEY_NUMLOCK      0xFF50    // 0x1B 0x4F 0x50 via serial
//#define KEY_BACKTAB      0xFF5A    // 0x1B 0x5B 0x5A via serial
//


// I don't like the way this is written but
// it basically works... I'll fix it later...
//
int fetchKeyInput()
{
int keycode;
char c1, c2, c3, c4, c5;

  c1 = Serial.read();
  if (c1 != KEY_ESCAPE)
  {
    keycode = c1;
    return(keycode);
  }

  keycode = KEY_ESCAPE;  // at least until we check further...

  // Now we differentiate between various codes
  //
  if (!Serial.available())    // nothing in buffer - return (but this may not
  {                           // be completely sufficient - may need timeout)
    return(keycode);
  }

  c2 = Serial.read();  // get code #2
  if (c2 == 0x4f) {
    c2 = Serial.read();
    if (c2 == 0x50) {
      keycode = KEY_NUMLOCK;
      return(keycode);
    }
    else {
      return(keycode);  // ESCAPE
    }
  }
  else if (c2 != 0x5B)
    return(keycode);

  //OK, now we have 0x1B 0x5B ...
  c3 = Serial.read();  // get code #3
  switch(c3) {
    case 0x41:
      keycode = KEY_UPARROW;
      return(keycode);
    case 0x42:
      keycode = KEY_DOWNARROW;
      return(keycode);
    case 0x43:
      keycode = KEY_RIGHTARROW;
      return(keycode);
    case 0x44:
      keycode = KEY_LEFTARROW;
      return(keycode);
    case 0x50:
      keycode = KEY_NUMLOCK;
      return(keycode);
    case 0x5A:
      keycode = KEY_BACKTAB;
      return(keycode);
  }

  c4 = Serial.read();
  if (c4 == 0x7E) {
    switch(c3) {
      case 0x31:
        keycode = KEY_HOME;
        return(keycode);
      case 0x32:
        keycode = KEY_INSERT;
        return(keycode);
      case 0x34:
        keycode = KEY_END;
        return(keycode);
      case 0x35:
        keycode = KEY_PAGEUP;
        return(keycode);
      case 0x36:
        keycode = KEY_PAGEDOWN;
        return(keycode);
    }
    return(keycode);
  }
}

//
// char enterValue(int size, int base, char *buf)
//   Fetch an n-digit hex value into a supplied buffer
// size = number of digits
// base = base system (8, 10, or 16)
// buf  = buffer to write entereed keys into (must be adequately-sized)
// return value = KEY_ESCAPE for abort, KEY_ENTER for accept
//
char enterValue(int size, int base, char *buf)
{
char c;
char cmdList[32];
int i;

  switch (base) {
    case 16:
      strcpy(cmdList, "1234567890AaBbCcDdEeFf\x8\x7f\x0a");
      break;

    case 10:
      strcpy(cmdList, "1234567890\x8\x7f\x0a");
      break;

    case 8:
      strcpy(cmdList, "12345670\x8\x7f\x0a");
      break;

    default:
      return(0);
  }

  i = 0;
  while (1) {
    c = getKeyFromList(cmdList, true);  // command list, and beep if wrong keys
    
    if (c == KEY_ESCAPE)
      return(c);

    if (c == KEY_ENTER) {
      if (i > (size-1)) {
        break;
      }
      else {
        Serial.write(BELL);
        continue;
      }
    }

    if ((c == KEY_BACKSPACE) || (c == KEY_DELETE)) {      // backspace
      *(buf + i) = 0x00;
      if (i > 0) {
        i--;
        Serial.write(CURSORLEFT " " CURSORLEFT);
      }
      else {
        Serial.write(BELL);
      }
      continue;
    }

    if (i > (size-1)) {
      Serial.write(BELL);
      continue;
    }

    if ((c >= 'a') && (c <= 'f')) {    // change lowercase to uppercase
      c -= 0x20;
    }

    Serial.write(c);
    *(buf + i) = c;

    i += 1;
  }
  return(c);
}

//
// printat(row, col)
// printat(char * rowcol)
//
// This function suppresses all returned data from the PCE being
//
void printat(int row, int col)
{
  Serial.print(PRINTAT_OPEN);
  Serial.print(row);
  Serial.print(PRINTAT_SEPARATOR);
  Serial.print(col);
  Serial.print(PRINTAT_CLOSE);
}

void printat(char * rowcol)
{
  Serial.print(PRINTAT_OPEN);
  Serial.print(rowcol);
  Serial.print(PRINTAT_CLOSE);
}

//
// Note: Make sure to have buffer large enough
//       as it is not bounds-protected !!!
//
void readLinetoBuf(char * buffer)
{
char c;

  delay(20);           // wait 20 milliseconds
  while (PCE.available())
  {
    c = PCE.read();

    if (c == 0x0a) {
      *buffer = 0x00;
      break;
    } else {
      *buffer = c;
      buffer++;
    }
    delay(1);
  }
}

//
// Notes: Echo must be off
//
// Also, First character coming back must be first hex character
//
void readBinarytoBuf(int size, char * buffer)
{
int i;
char c;

  i = 0;
  while (i < size) {
    if (PCE.available()) {
      c = PCE.read();
      *buffer = c;
      buffer++;
      i++;
    }
  }
}


//
// drainSerialQueue()
//
// This function suppresses all returned data from the PCE being
// sent basically continuously.  This is because the response is
// unwanted
//
void drainSerialQueue()
{
  char c;

  //drain the queue
  PCE.println("");      // in case there's junk on the command line
  delay(20);           // wait 20 milliseconds
  while (PCE.available()) {
    c = PCE.read();
    delay(1);
  }
}

//
// switchPCEInput(int)
// 
// Switches between sending joypad inputs to the PCE, versus sending
// microcontroller input to the PCE
//
void switchPCEInput(int input)
{
  if (input == JOYPAD_IN) {
    digitalWrite(joypadPin,HIGH);
    digitalWrite(computerPin,LOW);
  }
  else {
    digitalWrite(joypadPin,LOW);
    digitalWrite(computerPin,HIGH);
  }
}


//
// hex conversion routines
//
// char hex2digit(char *buffer)
//   - take a single char of buffer, return val (0-15) in char
//
// char hex2digit(char *buffer)
//   - take 2 chars of buffer, turn it into a char
//
// int hextoint16(char *buffer)
//   - take 4 chars of buffer, turn it into an int
//
// hex2tobuf(char c, char *buffer)
//   - take char and 'sprintf' it as 2-bute hex string into buffer
//
// hex4tobuf(int i, char *buffer)
//   - take int and 'sprintf' it as 4-bute hex string into buffer
//
char hex2digit(char *buffer) {
char c, n;

  c = *buffer;

  if ((c >= '0') && (c <= '9')) {
    n = c - '0';
  }
  else if ((c >= 'A') && (c <= 'F')) {
    n = c - 'A' + 10;
  }
  else if ((c >= 'a') && (c <= 'f')) {
    n = c - 'a' + 10;
  }
  else
    n = 0;

  return(n);
}

char hextobyte(char *buffer) {
char n;

  n = (hex2digit(buffer) << 4);
  buffer++;
  n |= hex2digit(buffer);

  return(n);
}

int hextoint16(char *buffer) {
int n;

  n = (hex2digit(buffer) << 12);
  buffer++;
  n |= (hex2digit(buffer) << 8);
  buffer++;
  n |= (hex2digit(buffer) << 4);
  buffer++;
  n |= hex2digit(buffer);

  return(n);
}


//
// printhex2(char) / printhex4(int)
// 
// Print a char variable as a 2-/4-digit hex number
//
// Arduino has a print(... , HEX) function, but it trims leading zeroes
// which is not desirable.  Have to break it down to character-by-character
//
void printhex2(char c)
{
int c1, c2;

  c1 = (c >> 4) & 0x0f;
  c2 = c & 0x0f;

  Serial.print(c1, HEX);
  Serial.print(c2, HEX);
}

void printhex4(int i)
{
int c1, c2, c3, c4;

  c1 = (i >> 12) & 0x0f;
  c2 = (i >> 8) & 0x0f;
  c3 = (i >> 4) & 0x0f;
  c4 = i & 0x0f;
  
  Serial.print(c1, HEX);
  Serial.print(c2, HEX);
  Serial.print(c3, HEX);
  Serial.print(c4, HEX);
}


void hex2tobuf(char c, char *buffer)
{
  int c1, c2;
  c1 = c / 16;
  c2 = c % 16;

  if (c1 < 0x0a)
    *buffer = c1 + '0';
  else
    *buffer = (c1 - 10) + 'A';

  buffer++;

  if (c2 < 0x0a)
    *buffer = c2 + '0';
  else
    *buffer = (c2 - 10) + 'A';
}

void hex4tobuf(int i, char *buffer)
{
char c;

  c = i >> 8;
  hex2tobuf(c, buffer);

  buffer += 2;

  c = i & 0xff;
  hex2tobuf(c, buffer);
}
