//
// Alternate.c
//
// 
//

#include <Arduino.h>
//#include <SD.h>

#include "define.h"
#include "common.h"
#include "escape.h"

// COMMANDS:
//
#define ALT_CMD_JOYPAD_IN       ','
#define ALT_CMD_COMPUTER_IN     '.'
#define ALT_CMD_HELP            '?'
#define ALT_CMD_SWITCHMODE      '|'

#define ALT_CMD_SAVE            'S'
#define ALT_CMD_LCASE_SAVE      's'
#define ALT_CMD_VIEW            'V'
#define ALT_CMD_LCASE_VIEW      'v'

// SCREEN POSITIONS:
//
#define POS_TITLE               "1;42"
#define POS_BAUD                "1;2"
#define POS_FILENAME            "1;80"

#define POS_REG_HEADING         "4;4"
#define POS_REG_A               "6;6"
#define POS_REG_X               "7;6"
#define POS_REG_Y               "8;6"
#define POS_REG_S               "9;6"
#define POS_REG_F               "10;6"
#define POS_REG_PC              "12;5"
#define POS_MPR_ROW             14
#define POS_MPR_COL             3

#define POS_HEX_HEADING         "4;50"
#define POS_HEX_HEADING2        "5;31"
#define POS_HEX_ROW             6
#define POS_HEX_COL             20

#define POS_CHR_HEADING         "4;87"
#define POS_CHR_ROW             6
#define POS_CHR_COL             81

#define POS_CMD_HEADING         "27;6"
#define POS_SUBCMD_LIST         "28;15"

#define STATUS_LINE             30

// MODES:
//
#define DISP_MAIN_MEM            1
#define DISP_VIDEO_MEM           2

// Thoughts on diffrent display modes (not yet implemented)
//
//#define MAIN_MEMORY        1
//#define VRAM_MEMORY        1
//#define BRAM_MEMORY        1
//#define ADPCM_MEMORY       1
//#define VCE_MEMORY         1



// Globals:
//
int  disp_addr;
char disp_mode = DISP_MAIN_MEM;

char reg_a  = 0;
char reg_f  = 0;
char reg_x  = 0;
char reg_y  = 0;
char reg_s  = 0;
int  reg_pc = 0;
char reg_mpr[8];
bool reg_flg[8];
const char flg_true[] = "NVTBDIZC";

char cmd_buffer[32];    // for sending to PCE
char line_buffer[129];  // for reading from PCE
char hex_buffer[8193];  // most fetches will be 256 bytes;
                        // 2048 for BRAM, 8192 for a bank

char disp_buffer[129];  // for on-screen display


///////////////////////////////////////////////////
// Data Fetch (from PCE) functions
///////////////////////////////////////////////////

// All of these commands are expecting echo off
//

// This will query PCEmon for registers
//
void getRegs()
{
  switchPCEInput(COMPUTER_IN);
  PCE.println("R");            // get registers
  readLinetoBuf(line_buffer);  // first line is headings - ignore this !!
  readLinetoBuf(line_buffer);  // second line is register values/needs parsing

  reg_pc = hextoint16(line_buffer + 2);
  reg_a = hextobyte(line_buffer + 7);
  reg_x = hextobyte(line_buffer + 10);
  reg_y = hextobyte(line_buffer + 13);
  reg_s = hextobyte(line_buffer + 27);

  reg_flg[0] = (line_buffer[17] == '1') ? true : false;
  reg_flg[1] = (line_buffer[18] == '1') ? true : false;
  reg_flg[2] = (line_buffer[19] == '1') ? true : false;
  reg_flg[3] = (line_buffer[20] == '1') ? true : false;
  reg_flg[4] = (line_buffer[21] == '1') ? true : false;
  reg_flg[5] = (line_buffer[22] == '1') ? true : false;
  reg_flg[6] = (line_buffer[23] == '1') ? true : false;
  reg_flg[7] = (line_buffer[24] == '1') ? true : false;

  reg_mpr[0] = hextoint16(line_buffer + 32);
  reg_mpr[1] = hextoint16(line_buffer + 35);
  reg_mpr[2] = hextoint16(line_buffer + 41);
  reg_mpr[3] = hextoint16(line_buffer + 44);
  reg_mpr[4] = hextoint16(line_buffer + 47);
  reg_mpr[5] = hextoint16(line_buffer + 50);
  reg_mpr[6] = hextoint16(line_buffer + 53);
  reg_mpr[7] = hextoint16(line_buffer + 56);
}

// This will query PCEmon for memory; currently only
// main memory, but this can be improved later
//
void getMemoryScreen(int addr)
{
  switchPCEInput(COMPUTER_IN);
  strcpy(cmd_buffer, "D           ");

  if (disp_mode == DISP_MAIN_MEM) {
    hex4tobuf( addr, cmd_buffer + 3 );
    hex4tobuf( addr + 0x100, cmd_buffer + 8 );
  }
  else if (disp_mode == DISP_VIDEO_MEM) {
    cmd_buffer[1] = 'V';
    hex4tobuf( addr, cmd_buffer + 3 );
    hex4tobuf( addr + 0x80, cmd_buffer + 8 );
  }
  cmd_buffer[12] = 0;

  PCE.println(cmd_buffer);
  readBinarytoBuf(256, hex_buffer);
}

// untested
void getMemoryBank(char bank)
{
  switchPCEInput(COMPUTER_IN);
  strcpy(cmd_buffer, "D   :");
    hex2tobuf( bank, cmd_buffer + 2 );
  cmd_buffer[5] = 0;
  
  PCE.println(cmd_buffer);
  readBinarytoBuf(8192, hex_buffer);
}

// untested
void getBRAM()
{
  switchPCEInput(COMPUTER_IN);
  PCE.println("<");
  PCE.println("S");
  readBinarytoBuf(2048, hex_buffer);
}

// untested
void getPalettes()
{
  switchPCEInput(COMPUTER_IN);
  PCE.println("(");
  PCE.println("S");
  readBinarytoBuf(1024, hex_buffer);
}


///////////////////////////////////////////////////
// Display-related functions
///////////////////////////////////////////////////

void altDispRegs()
{
int i;
char c;

  printat(POS_REG_HEADING);
  Serial.print(THM_REG_TITLE "REGISTERS");
  printat(POS_REG_A);
  Serial.print(THM_REGS "A = ");
  printhex2(reg_a);
  printat(POS_REG_X);
  Serial.print(THM_REGS "X = ");
  printhex2(reg_x);
  printat(POS_REG_Y);
  Serial.print(THM_REGS "Y = ");
  printhex2(reg_y);
  printat(POS_REG_S);
  Serial.print(THM_REGS "S = ");
  printhex2(reg_s);
  printat(POS_REG_PC);
  Serial.print(THM_REGS "PC = ");
  printhex2(reg_pc >> 8);
  printhex2(reg_pc & 0xff);

  printat(POS_REG_F);
  Serial.print(THM_REGS "F = ");
  for (i = 0; i < 8; i++) {
    if (reg_flg[i] == true) {
      c = flg_true[i];
      Serial.print(THM_REGS);
      Serial.print(c);
    } else {
      c = flg_true[i];
      Serial.print(THM_REGS_BKGND);
      Serial.print(c);
    }
  }
  
  for (i = 0; i < 8; i++) {
    printat(POS_MPR_ROW + i, POS_MPR_COL);
    Serial.print(THM_REGS "MPR");
    Serial.print(i);
    Serial.print(" = ");
    printhex2(reg_mpr[i]);
  }
}

void altDispHex()
{
int i, j;

  switch (disp_mode) {

    case DISP_MAIN_MEM:
      // print hex area
      for (i = 0; i < 16; i++) {
        printat(POS_HEX_ROW + i, POS_HEX_COL);
        Serial.print(CLEAR_EOL THM_ADDRESS);
        Serial.print("(");
        printhex2( reg_mpr[(disp_addr + (i*16)) >>13] );
        Serial.print("):");
        printhex4(disp_addr + (i*16));
        Serial.print(": ");
        Serial.print(THM_HEXDATA);
        for (j = 0; j < 16; j++) {
          printhex2(hex_buffer[(i*16) + j]);
          Serial.print(" ");
        }
      }
      break;
  
    case DISP_VIDEO_MEM: 
      // print hex area
      for (i = 0; i < 16; i++) {
        printat(POS_HEX_ROW + i, POS_HEX_COL);
        Serial.print(CLEAR_EOL THM_ADDRESS);
        Serial.print("     ");
        printhex4(disp_addr + (i*8));
        Serial.print(":  ");
        Serial.print(THM_HEXDATA);
        for (j = 0; j < 8; j++) {
          printhex4( (hex_buffer[(i*16) + (j*2) + 1]<<8) | hex_buffer[(i*16) + (j*2)]  );
          Serial.print("  ");
        }
      }
      break;
  }
}

void altDispChar()
{
int i, j;

  // print char area
  Serial.print(THM_CHARDATA);
  for (i = 0; i < 16; i++) {
    printat(POS_CHR_ROW + i, POS_CHR_COL);
    for (j = 0; j < 16; j++) {
      disp_buffer[j] = hex_buffer[(i*16) + j];
      if ((disp_buffer[j] < 0x20) || (disp_buffer[j] > 0x7f))
         disp_buffer[j] = '.';
    }
    disp_buffer[16] = 0;
    Serial.print(disp_buffer);
  }
}

void altScreen()
{
int i, j;

  Serial.print(CLEAR HOME);
  printat(POS_TITLE);
  Serial.print(THM_TITLE "BUG MONITOR MODE");

  // print registers section
  printat(POS_REG_HEADING);
  Serial.print(THM_REG_TITLE "REGISTERS");

  altDispRegs();

  if (disp_mode == DISP_MAIN_MEM) {
    // print hex area
    printat(POS_HEX_HEADING);
    Serial.print(THM_HEX_TITLE "MAIN MEMORY");
    printat(POS_HEX_HEADING2);
    Serial.print(THM_HEX_TITLE " 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
  }
  else if (disp_mode == DISP_VIDEO_MEM) {
    // print hex area
    printat(POS_HEX_HEADING);
    Serial.print(THM_HEX_TITLE "VIDEO MEMORY");
    printat(POS_HEX_HEADING2);
    Serial.print(THM_HEX_TITLE "  0/8   1/9   2/A   3/B   4/C   5/D   6/E   7/F");
  }
  
  altDispHex();
  
  // print char area
  printat(POS_CHR_HEADING);
  Serial.print(THM_CHAR_TITLE "CHAR");

  altDispChar();

  // print status line
  printat(POS_BAUD);
  Serial.print(THM_STATUS "BAUD RATE = 19200");

  // clear subcommand line
  printat(POS_SUBCMD_LIST);
  Serial.print(CLEAR_EOL);

  // print command line
  printat(POS_CMD_HEADING);
  Serial.print(THM_CMD_TITLE "COMMAND: " THM_COMMAND);
}


///////////////////////////////////////////////////
// Command-related functions
///////////////////////////////////////////////////

void altHelp()
{
char c;

  Serial.print(CLEAR HOME THM_TITLE "      HELP TEXT     " THM_NORMAL);
  printat(3, 1);
  Serial.println("  Commands available:");
  Serial.println("");
  Serial.println(" Instant Operation Commands (one-key operation):");
  Serial.println(" -----------------------------------------------");
  Serial.println(" ?   - Help (this screen)");
  Serial.println(" |   - Switch command mode back to original (PCEmon orig)");
  Serial.println(" , . - Switch PC Engine input to use Joypad/microcontroller");
  Serial.println(" .   - Switch PC Engine input to use microcontroller");
  Serial.println(" up/dn/pgup/pgdn - scroll through displayed data");
  Serial.println("");
  Serial.println(" V   - View memory (Main/VRAM); soon will accept address input");
  Serial.println("");
  Serial.println(" (Note that unassigned keys will just show their hex code values)");
  Serial.println("");
  Serial.println(" (Another Command)");
  Serial.println(" (Another Command)");
  Serial.println("    .");
  Serial.println("    .");
  Serial.println("    .");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("    Hit any key to return to the monitor");
  
  while (!Serial.available());
  c = Serial.read();   // dispose of the keystroke
  altScreen();
}

//////
void setAltMode()
{
  // first, turn ECHO off
  if (PCE_Echo == true) {
    switchPCEInput(COMPUTER_IN);
    PCE.println("O E");        // turn off (or on) echo
    PCE_Echo = false;
    drainSerialQueue();
  }

  getRegs();

  // first, form the command to invoke:
  // D command:  Dx aaaa bbbb, where x = ' '/'V'

  disp_mode = DISP_MAIN_MEM;
  
  if (disp_mode == DISP_MAIN_MEM) {
    disp_addr = 0xe000;
  }
  else if (disp_mode == DISP_VIDEO_MEM) {
    disp_addr = 0x0000;
  }

  getMemoryScreen(disp_addr);

  Serial.println("AlternateMode is Set");

//  delay (2000);
  altScreen();
}

//////
void setNormalMode()
{
  // first, turn ECHO on
  switchPCEInput(COMPUTER_IN);
  PCE.println("O E");        // turn off (or on) echo
  PCE_Echo = true;

  Serial.println("Normal Mode is Set");
}

//////
void showData(int addr)
{
  disp_addr = addr & 0xffff;
  getMemoryScreen(disp_addr);
  altDispHex();
  altDispChar();
  // go back to command line
  printat(POS_CMD_HEADING);
  Serial.print(THM_CMD_TITLE "COMMAND: " THM_COMMAND);
}

//////
//void submenuSave()
//{
//char c;
//
//  Serial.print("Save ");   // in Command slot
//
//  printat(POS_SUBCMD_LIST);
//  Serial.print(THM_SUBCMD_HILITE "<B>" THM_SUBCOMMAND "RAM  ");
//  Serial.print(THM_SUBCMD_HILITE "<P>" THM_SUBCOMMAND "allete data  ");
//  Serial.print(THM_SUBCMD_HILITE "<M>" THM_SUBCOMMAND "emory Bank  ");
//  
//  while (!Serial.available());
//  c = Serial.read();   // dispose of the keystroke
//  altScreen();
//}

//////
void submenuView()
{
char c;
char cmdList[] = "MmVv";
char temp_disp_mode;

  printat(POS_SUBCMD_LIST);
  Serial.print(THM_SUBCMD_HILITE "<M>" THM_SUBCOMMAND "ain Memory  ");
  Serial.print(THM_SUBCMD_HILITE "<V>" THM_SUBCOMMAND "RAM  ");

  printat(POS_CMD_HEADING);
  Serial.print(THM_CMD_TITLE "COMMAND: " THM_COMMAND);

  Serial.print("View ");   // in Command slot

  c = getKeyFromList(cmdList, true);  // command list, and beep if wrong keys

  temp_disp_mode = disp_mode;

  switch (c) {
    case KEY_ESCAPE:
      break;
      
    case 'm':
    case 'M':
      temp_disp_mode = DISP_MAIN_MEM;
      Serial.print("Main Memory");
      break;

    case 'v':
    case 'V':
      temp_disp_mode = DISP_VIDEO_MEM;
      Serial.print("VRAM");
      break;
  }

  // only allow Enter or ESCAPE
  //    --->  this should probably be a common routine
  //
  c = waitKeyEnterEscape(true);  // beep if wrong key
  
  if (c == KEY_ENTER) {
    disp_mode = temp_disp_mode;
  }
}

int scrollLineAmount(char mode)
{
int amount;

  amount = 0x10;
  if (mode == DISP_VIDEO_MEM)
    amount = 0x08;

  return(amount);
}

int scrollPageAmount(char mode)
{
int amount;

  amount = 0x100;
  if (mode == DISP_VIDEO_MEM)
    amount = 0x80;

  return(amount);
}

///////////////////////////////////////////////////
// Command fetch loop (for 'monitor' mode)
///////////////////////////////////////////////////

void altLoop()
{
int c;

  if (checkForKey()) {      // If anything comes in Serial (USB),
    c = fetchKeyInput();

    switch(c) {
//      case ALT_CMD_SAVE:
//      case ALT_CMD_sAVE:
//        submenuSave();
//        break;

      case ALT_CMD_VIEW:
      case ALT_CMD_LCASE_VIEW:
        submenuView();
        altScreen();
        break;

      case KEY_PAGEDOWN:
        showData(disp_addr + scrollPageAmount(disp_mode));
        break;

      case KEY_PAGEUP:
        showData(disp_addr - scrollPageAmount(disp_mode));
        break;

      case KEY_DOWNARROW:
        showData(disp_addr + scrollLineAmount(disp_mode));
        break;

      case KEY_UPARROW:
        showData(disp_addr + scrollLineAmount(disp_mode));
        break;

      case ALT_CMD_SWITCHMODE:
        modeMonitor = false;
        setNormalMode();
        return;

      case ALT_CMD_JOYPAD_IN:
        switchPCEInput(JOYPAD_IN);
        break;

      case ALT_CMD_COMPUTER_IN:
        switchPCEInput(COMPUTER_IN);
        break;

      case ALT_CMD_HELP:
        altHelp();
        break;

      case KEY_ENTER:             // just display the screen again
//      case KEY_ESCAPE:
        altScreen();
        break;

      default:                    // unknown key: print hex value(s)
        if (c < 0x100)    // not a special code
          printhex2(c);
        else
          printhex4(c);   // special code - identified as an ESC-sequence
        Serial.print(" ");
        break;
    }
  }

  // spool returned data from PCE back to screen
  // (if not consumed by commands above)
  // -> In Bug Monitor mode, this should be rare if at all
  //
  if (PCE.available()) {            // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(PCE.read());   // read it and send it out Serial (USB)
  }
}
