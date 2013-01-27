SiSeLi
======

A Serial Port library for [Scilab][1].

    FEATURES:
        - Supports all kinds of serial ports, including USB, Bluetooth or virtual connections.      
        - baud rates up to 3MBit/s
        - up to 16 simultaneous connections
        - easy to use
        - ...

For binaries, documentation and examples, go [this way][2].  
As an alternative, some versions of this library is available
via [Scilab Atoms][3] too.


---


## CHANGES

### V0.1 initial release
    
### V0.1a mini-fix
    - NEW: added slInit() function
    
### V0.2 next steps
    - removed some debug lines
    - increased default RX buffer size to 30kB

### V0.3 never released
    
### V0.4 tiny add-on
    - NEW: added slReadArrayN() function

### V0.5 baudrate upgrade
    - NEW: added baudrates 256000, up to 3MBit/s
    - FIX: error handling did not work for some functions; always reported "OK"

### V0.6 implemented packet read stuff
    - *NEW* added slReadPacket()
    - *NEW* added slCountPacket()
    - *CHG* reworked complete packet send/write algorithm
    - *CHG* increased simultaneously usable serial ports to 16


Have fun  
FMMT666(ASkr)


[1]: http://www.scilab.org
[2]: http://www.askrprojects.net/software/siseli/index.html
[3]: http://atoms.scilab.org/toolboxes/siseli
