//
// define.h
//
// Common Definitions file used by everybody
//
// (c) 2019, 2020 by David Shadoff

#define PCE     Serial1


#define JOYPAD_IN    0
#define COMPUTER_IN  1

// names of keys
// Note that any multi-code key has special code defined here
//
// single-code keystrokes:
#define KEY_BACKSPACE    0x08
#define KEY_TAB          0x09
#define KEY_ENTER        0x0A
#define KEY_ESCAPE       0x1B
#define KEY_DELETE       0x7F

// multi-code keystrokes:

#define KEY_F1           0xFF01    // 0x1B 0x5B 0x31 0x31 0x7E via serial
#define KEY_F2           0xFF02    // 0x1B 0x5B 0x31 0x32 0x7E via serial
#define KEY_F3           0xFF03    // 0x1B 0x5B 0x31 0x33 0x7E via serial
#define KEY_F4           0xFF04    // 0x1B 0x5B 0x31 0x34 0x7E via serial
#define KEY_F5           0xFF05    // 0x1B 0x5B 0x31 0x35 0x7E via serial
#define KEY_F6           0xFF06    // 0x1B 0x5B 0x31 0x37 0x7E via serial
#define KEY_F7           0xFF07    // 0x1B 0x5B 0x31 0x38 0x7E via serial
#define KEY_F8           0xFF08    // 0x1B 0x5B 0x31 0x39 0x7E via serial
#define KEY_F9           0xFF09    // 0x1B 0x5B 0x32 0x30 0x7E via serial
#define KEY_F10          0xFF0A    // 0x1B 0x5B 0x32 0x31 0x7E via serial
#define KEY_F11          0xFF0B    // 0x1B 0x5B 0x32 0x33 0x7E via serial
#define KEY_F12          0xFF0C    // 0x1B 0x5B 0x32 0x34 0x7E via serial

#define KEY_HOME         0xFF31    // 0x1B 0x5B 0x31 0x7E via serial
#define KEY_INSERT       0xFF32    // 0x1B 0x5B 0x32 0x7E via serial
#define KEY_END          0xFF34    // 0x1B 0x5B 0x34 0x7E via serial
#define KEY_PAGEUP       0xFF35    // 0x1B 0x5B 0x35 0x7E via serial
#define KEY_PAGEDOWN     0xFF36    // 0x1B 0x5B 0x36 0x7E via serial

#define KEY_UPARROW      0xFF41    // 0x1B 0x5B 0x41 via serial
#define KEY_DOWNARROW    0xFF42    // 0x1B 0x5B 0x42 via serial
#define KEY_RIGHTARROW   0xFF43    // 0x1B 0x5B 0x43 via serial
#define KEY_LEFTARROW    0xFF44    // 0x1B 0x5B 0x44 via serial

#define KEY_NUMLOCK      0xFF50    // 0x1B 0x4F 0x50 via serial
#define KEY_BACKTAB      0xFF5A    // 0x1B 0x5B 0x5A via serial



// This sketch is made specifically for the Adafruit M0 Feather Adalogger

const int chipSelect   = 4;   // Pin used for chip select for on-board SDCard

const int joypadPin    = 11;  // if set high, triggers joypad passthru to port
const int computerPin  = 10;  // This should be the inverse of joypadPin (status LED)

const int attnPin      = 12;  // joypad CLR pin.
                              // Future proposed functionality:
                              //   if PC-E sets this high for >16ms,
                              //   it is a signal to switch back to MCU mode
