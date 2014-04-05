
SiSeLi
======

A serial port library for [Scilab][1].

    FEATURES:
        - no installation required; Just copy SiSeLi into your Scilab project directory.
        - Supports all kinds of serial ports, including USB, Bluetooth or virtual connections.      
        - baud rates up to 3MBit/s
        - unlimited amount of simultaneous connections
        - supports DLE sequenced packet mechanism
        - ...


These sources come with all necessary project files for building a
x86 or x64 DLL with [Code::Blocks][4] and the [TDM-GCC][5] compiler.


For binaries, documentation and examples, go [this way][2].  
As an alternative, some versions of this library is available
via [Scilab Atoms][3] too.


----------------------------------------------------------------------------------
## TODO
  - buy more beer
  - ...


----------------------------------------------------------------------------------
## BUILDING

SiSeLi's source code comes with two separate Code::Blocks project files for
32- or 64-bit DLL compilation.

Due to some bugs in the TDM64-GCC, you can (currently) NOT build 32 bit DLLs
with it, hence, the 32 bit project file requires an installation of the
32 bit TDM-GCC edition.


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
    - NEW: switched over to TDM-GCC and Code::Blocks
    - NEW: slLoadLib.sci now auto-selects the right DLL (x86/x64)
    - NEW: started to remove old C-style stuff
    - NEW: unlimited amount of serial ports (previously limited to 16)
    - CHG: slMount() does not require the handle argument any longer.
    - CHG: slReadByte() does not require the 2nd argument if NONBLOCKING


Have fun  
FMMT666(ASkr)


[1]: http://www.scilab.org
[2]: http://www.askrprojects.net/software/siseli/index.html
[3]: http://atoms.scilab.org/toolboxes/siseli
[4]: http://www.codeblocks.org/
[5]: http://tdm-gcc.tdragon.net/
