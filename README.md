#### WARNING ####

The "DevCPP2014" branch is not fully tested and will not yet compile on x86
machines because of some bugs in the TDM-GCC release.
  
The x64 version is not affected.


----------------------------------------------------------------------------------


SiSeLi
======

A serial port library for [Scilab][1].

    FEATURES:
        - Supports all kinds of serial ports, including USB, Bluetooth or virtual connections.      
        - baud rates up to 3MBit/s
        - unlimited amount of simultaneous connections
        - supports DLE sequenced packet mechanism
        - ...


These sources come with all necessary project files for building a
x86 or x64 DLL with either [Dev-C++][4] or [Orwell Dev-C++][5].


For binaries, documentation and examples, go [this way][2].  
As an alternative, some versions of this library is available
via [Scilab Atoms][3] too.


----------------------------------------------------------------------------------
## TODO
  - auto load x86/x64 libs
  - fix x86 build problems
  - buy more beer


----------------------------------------------------------------------------------
## CHANGES

### V0.1 initial release
    
### V0.1a mini-fix
    - NEW: added slInit() function
    
### V0.2 next steps
    - FIX: removed some debug lines
    - CHG: increased default RX buffer size to 30kB

### V0.3 never released
    - NOP: bummer
    
### V0.4 tiny add-on
    - NEW: added slReadArrayN() function

### V0.5 baudrate upgrade
    - NEW: added baudrates 256000, up to 3MBit/s
    - FIX: error handling did not work for some functions; always reported "OK"

### V0.6 implemented packet read stuff
    - NEW: added slReadPacket()
    - NEW: added slCountPacket()
    - CHG: reworked complete packet send/write algorithm
    - CHG: increased simultaneously usable serial ports to 16

### V0.7, refactoring and minor improvements
    - NEW: started to remove old C-style stuff
    - NEW: unlimited amount of serial ports (previously limited to 16)
    - CHG: slMount() does not require the handle argument any longer.
    - CHG: slReadByte() does not require the 2nd argument if NONBLOCKING


Have fun  
FMMT666(ASkr)


[1]: http://www.scilab.org
[2]: http://www.askrprojects.net/software/siseli/index.html
[3]: http://atoms.scilab.org/toolboxes/siseli
[4]: http://www.bloodshed.net/download.html
[5]: http://orwelldevcpp.blogspot.de/
