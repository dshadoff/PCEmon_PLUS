//
// common.h
//
// Common functions
//


extern bool cardPresent;

extern bool PCE_Echo;
extern bool modeMonitor;


extern bool checkForKey();
extern int  fetchKeyInput();

extern void printat(int row, int col);
extern void printat(char *rowcol);

extern void readLinetoBuf(char * buffer);
extern void readBinarytoBuf(int size, char * buffer);

extern void drainSerialQueue();

extern void switchPCEInput(int input);

extern char hex2digit(char *buffer);
extern char hextobyte(char *buffer);
extern int  hextoint16(char *buffer);
extern void printhex2(char c);
extern void printhex4(int i);
extern void hex2tobuf(char c, char*buffer);
extern void hex4tobuf(int i, char*buffer);
