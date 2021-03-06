# REVISIONs

20200222:
- Added 'rev. C' board design to the repository
- modified code to accept touch input from teh board instead of keyboard
  commands for switching inputs or modes
- Added recommended Teraterm settings for best results to the README.md file

20200118:
- added scroll left/right (1-byte movement)
- added "Address" entry command
- fix bugs; minor formatting changes

20200105:
- added flag display
- added VRAM display
- started implementing commands ("View" in this case)
- cleaned up the key fetch framework a bit
- various minor changes
- added help text 

20191231:
- bugs and improvements
- add some (but not enough) help text
- add some color to theme
- add communication flip to PCE-to-MCU during debug 

20191229:
- added more framework
- split interface between traditional and new interaction modes
- program now grabs downloads into buffers
- memory view/scroll now working

20191219:
- check in Rev. B boards
- bom.csv and assembly.csv were required by JLCPCB in order to do the SMT assembly

20191216:
- sync up echo
- add 1-button BRAM fetch (temporary framing)
- Make direction of data clearer for PCE communications

20191214:
- Initial check-in of rev. A boards.  Fair power isolation between PC and PCE, but
boot-up of MCU causes garbage input on the PCE side.
- Currently, only a shell pass-thru interface with trivial commands are implemented
(including input-swapping), with more to come once the rev. B boards are received
and verified

This is prototype software, and as such has been minimally tested.
Use at your own risk.
