//
// Escape.h
//
// Definitions file for escape codes for various terminal formatting codes
//


#define FG_NORMAL       "\x1b[0m"
#define FG_BOLD         "\x1b[1m"
#define FG_UNDERLINE    "\x1b[4m"
#define FG_INVERSE      "\x1b[7m"

#define FG_BLACK        "\x1b[30m"
#define FG_RED          "\x1b[31m"
#define FG_GREEN        "\x1b[32m"
#define FG_YELLOW       "\x1b[33m"
#define FG_BLUE         "\x1b[34m"
#define FG_MAGENTA      "\x1b[35m"
#define FG_CYAN         "\x1b[36m"
#define FG_WHITE        "\x1b[37m"
#define FG_DEFAULT      "\x1b[39m"

#define FG_GRAY         "\x1b[90m"
#define FG_BRT_RED      "\x1b[91m"
#define FG_BRT_GREEN    "\x1b[92m"
#define FG_BRT_YELLOW   "\x1b[93m"
#define FG_BRT_BLUE     "\x1b[94m"
#define FG_BRT_MAGENTA  "\x1b[95m"
#define FG_BRT_CYAN     "\x1b[96m"
#define FG_BRT_WHITE    "\x1b[97m"

#define BG_BLACK        "\x1b[40m"
#define BG_RED          "\x1b[41m"
#define BG_GREEN        "\x1b[42m"
#define BG_YELLOW       "\x1b[43m"
#define BG_BLUE         "\x1b[44m"
#define BG_MAGENTA      "\x1b[45m"
#define BG_CYAN         "\x1b[46m"
#define BG_WHITE        "\x1b[47m"
#define BG_DEFAULT      "\x1b[49m"

#define BG_GRAY         "\x1b[100m"
#define BG_BRT_RED      "\x1b[101m"
#define BG_BRT_GREEN    "\x1b[102m"
#define BG_BRT_YELLOW   "\x1b[103m"
#define BG_BRT_BLUE     "\x1b[104m"
#define BG_BRT_MAGENTA  "\x1b[105m"
#define BG_BRT_CYAN     "\x1b[106m"
#define BG_BRT_WHITE    "\x1b[107m"

#define THM_TITLE         FG_BRT_WHITE
#define THM_BAUD          FG_BRT_RED
#define THM_REG_TITLE     FG_BRT_WHITE
#define THM_REGS          FG_GREEN
#define THM_REGS_BKGND    FG_BLUE
//#define THM_ADDR_TITLE   FG_BRT_WHITE
#define THM_ADDRESS       FG_WHITE
#define THM_HEX_TITLE     FG_BRT_WHITE
#define THM_HEXDATA       FG_CYAN
#define THM_CHAR_TITLE    FG_BRT_WHITE
#define THM_CHARDATA      FG_YELLOW
#define THM_CMD_TITLE     FG_WHITE
#define THM_COMMAND       FG_BRT_WHITE
#define THM_SUBCOMMAND    FG_YELLOW
#define THM_SUBCMD_HILITE FG_BRT_YELLOW
#define THM_STATUS        FG_BRT_RED
#define THM_NORMAL        FG_WHITE

#define HOME              "\x1b[1;1H"
#define CLEAR             "\x1b[2J"
#define CLEAR_EOL         "\x1b[0K"
#define PRINTAT_OPEN      "\x1b["
#define PRINTAT_SEPARATOR ";"
#define PRINTAT_CLOSE     "f"

#define LF                0x0a
#define ESCAPE            0x1b
#define BELL              0x07


//
// set terminal sizing - not in use at this time
//
#define RESIZE_WINDOW "\x1b[8;24;80t"    // This sets 80x24...
#define LEFT_RIGHT "\x1b[1;80s"          // This sets 80 char wide
#define TOP_BOTTOM "\x1b[1;24r"          // This sets 24 lines tall
