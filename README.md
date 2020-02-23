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
which input is active, with the touch of a pad on the microcontroller board
- Currently, it powers up with a shell pass-thru interface with the original
PCEmon commands; touching the 'PLUS mode' touchpad on the board will bring up
an alternate interface which is being built, but can currently view and scroll
through main memory and VRAM (with more to come)


NOTES:
------
Recommended Teraterm settings by Setup menu area:
Terminal:
- New-line: Transmit - change to LF

Font:
- If you use Teraterm on Windows, I found that the default font is thin and wispy; a better
  font to use is "HGSoeiKakuGothicUB" (Regular)

Serial Port:
- Start with default 19200

Additional Settings, 'General' tab:
- Unset "Clear display when window resized"
- Set "Display serial port speed"

TODOs:
------
- Add interactive memory edit
- Add ZMODEM transfers for easier data handling
- Add disassembler (and save to SDCard/computer)
- Add more clever management of input control (i.e. PCE can signal back with
extended /CLR=high signal, that PCE is ready for PCEmoninput)

This is prototype software, and as such has been minimally tested.
Use at your own risk.
