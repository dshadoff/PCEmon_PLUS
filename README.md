# PCEmon_PLUS

This project is meant to enhance Chris Covell's PCEmon, which can be found here:
https://chrismcovell.com/PCEmon/index.html

This board implements a microcontroller (Adafruit Feather M0 Adalogger), to
simplify some operations and to generally make life easier for the PCEmon user.
Software is written using the Arduino IDE.

I am hoping that others find this useful, and are able to contribute to the
project in order to add new functionality.  I am open to improving the hardware
in future, if it would be helpful for the functionality added.

Features:
- Instead of swapping cables between joypad and computer, this board can change
which input is active, without interrupting the communications between computer
and microcontroller.
- Currently, only a shell pass-thru interface with trivial commands are implemented
(including input-swapping), with more to come once the rev. B boards are received
and verified


NOTES:
- If you use Teraterm on Windows, I found that the default font is thin and wispy; a better
  font to use is "HGSoeiKakuGothicUB" (Regular)

TODOs:
- Add interactive memory edit
- Add ZMODEM transfers for easier data handling
- Add disassembler (and save to SDCard/computer)
- Add more clever management of input control (i.e. PCE can signal back with
extended /CLR=high signal, that PCE is ready for PCEmoninput)

This is prototype software, and as such has been minimally tested.
Use at your own risk.
