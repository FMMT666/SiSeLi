
  ScilabSerialLib (SiSeLi) Mini Documentation, V0.7
 ===================================================
  Serial Port library for Scilab (Windoze).
  (c)ASkr, 10/2010 .. 04/2014
  www.askrprojects.net

  

1.0 INTRODUCTION

    A serial port library for Scilab and Windoze.

      FEATURES:

        - Supports all kinds of serial ports, including USB, Bluetooth or virtual connections.      
        - baud rates up to 3MBit/s
        - up to 16 simultaneous connections
        - easy to use
        - ...


1.1 FEATURES    

    See Chapter 3 for a complete function reference.
    Scroll down to the end to see a list of changes or new features.
    
    
1.2 REQUIREMENTS/NOTES

    SiSeLi requires at least Scilab version 5.3+ and comes in 32 and 64 bit variants.

    Beside Windows and a running Scilab installation, nothing else is required.
    
    Is it stable?
    Yes. Usually, Scilab crashes before SiSeli does ;)
    
      NOTICE:
      All data transfer Scilab <-> SiSeLi is done with pointer arithmetic.
      The only way of securing memory integrity, would be adding more stuff to the Scilab
      wrapper functions (Scilab's own function definitions), at the cost of speed, so I skipped
      this. Especially the "slSendArray(<handle>, <array>, <size>)" function (and maybe more in
      the future, see "Packet Mode"), relies on what you told it to do.
      If you assign a <size>, exceeding the limit of <array>, a memory exception will occur...
      Be careful !-)

    Can it be used with USB or Bluetooth virtual COM ports?
    Yes, although some extra care may be required. Sometimes, Windows "reserves" a COM port,
    although the device is not attached. In this case, you'll send all the data to nowhere
    and receive from anywhere. Basically this is not a problem until you use a function or
    mechanism that blocks until a byte is received. At this moment, Scilab will hang...
    But all the other time (you attached the device, the driver is loaded and you managed to
    get the right COM port number...) it will cause no (known) problems...
    I tested it with several FTDIs, several off-the-shelf USB <-> RS-232 converters, a few
    mobile phones and a pbLualized Lego NXT.

    
1.3 DEVELOPMENT ENVIRONMENT

    This one was created with the great DEV-CPP, freely available from
      http://www.bloodshed.net/devcpp.html
    or
      http://www.bloodshed.net/dev/    
    
    
1.4 FUTURE DEVELOPMENT, TODO LIST:

    - docs in Markdown notation
    - ...

    
1.5 FILES IN THE DISTRIBUTION

    DOC
    . slReadme.txt          -> what you're reading, right now
    .   ...
    SCILAB
    . siseli.dll            -> ScilabSerialLib
    . LoopDemo.sce          -> example usage
    . LoopDemoAllInOne.sce  -> example usage in one file, including the wrapper functions;
    .   ...                    just for reference
    . slLoadLib.sci         -> a script to load the library and all necessary wrapper functions
    SOURCE
    . SiSeLi_Vxy.zip        -> source code
    . CSERIAL
      . cserial.zip         -> slightly modified (and DEV-CPPed) CSerial object; used by SiSeLi
                               http://www.ramondeklein.nl/Projects/Serial.html

                               
      
2.0 EXAMPLE USAGE

    For an in-depth explanation of (all) the SiSeLi Scilab-wrapper functions, see chapter 3.

    Extract SiSeLi's SCILAB directory, or only "siseli.dll" to a new directory on your hard disk.
    Start Scilab and change the path to there.

    - Load the libary:

        exec("slLoadLib.sci")

      If any error occurs, Scilab script execution is aborted via "abort".
      There is no return variable available, but success or failure can be determined
      by the message that gets print out on the console.

    - Create a handle by mounting a port. A handle can be regarded as a reference number of an
      interface. SiSeLi supports an unlimited amount of interfaces.
      
        port1 = slMount()
      
      NOTICE:
        As of V0.7 (04/2014), slMount() does not require an argument (handle) any more.
        Just call it and it will return the handle.
        For compatibility reasons you still can call it with a parameter, but this is
        not recommended.
      
      Remember that number in "port1". From now on, every function call of the SiSeLi DLL wrapper
      functions require this number as an argument.
      Calling "slMount()" is once required before doing anything else. It internally creates
      all necessary objects an dependencies.

      If everything went fine, a number >0 is returned.
      In case of an error, 0 is returned.

    - Check if a serial port is available (COM x):
      
        ret = slCheck(port1,5)
      
      The first number is the handle, the second one specifies the number of the COM port.
      If COM5 is availabe and free, the function returns "1", otherwise "0" is returned.
    
    - Set COMM parameters:
    
        ret = slConfig(port1,19200,8,0,1)
      
      On handle "port1" (first arg), use "19200" baud, "8" data bits, "0" no parity and
      "1" stop bit. If everything went right, "1" is returned, "0" otherwise.
      Notice that there is no COM port argument!
      
    - Open port:
    
        ret = slOpen(port1,5)
        
      On handle "port1", open "COM5". If successful, guess what, a "1" is returned ("0" on error).
      From now on, your serial interface is ready to send and receive data.
    
    - Sending a byte/character:
    
        ret = slSendByte(port1,65)
        ret = slSendByte(port1,ascii('A'))
        
      On handle "port1", both functions send an "A" out to the port ("ascii('A')" returns 65 too).
      Notice that this function is not limited to ASCII characters. Every value 0..255 is
      supported.
      As from here on, I skip commenting the value of the return arguments...
      I guess you got the clue ;)

    - Sending an array (1D/2D):

        ret = slSendArray(port1,a,length(a));
        ret = slSendArray(port1,a',length(a));
        ret = slSendArray(port1,a(1,:),size(a,2));
        ret = slSendArray(port1,a(:,1),size(a,1));

      Assuming that "a" is an integer array of size(n,m), it can be sent as written above.
      First argument "port1" is the handle, the second "a" an array (even an array of size [1,1]),
      the third and last one is the size of the array.
      
      NOTICE: It is extremely important that the given size DOES NOT EXCEED the length of
      the array (smaller values are OK). Otherwise memory exceptions will occur.

    - Checking bytes in the (receiving) buffer:
    
        bytes = slCount(port1)

      This function returns the number of bytes available in the receive buffer.
      Internally, a thread receives and stores all incoming data bytes. If you (or Scilab)
      do not poll them fast enough and empty the buffer (see next function(s)), an overflow
      might occur. On overflow, slCount(<handle>) reports a "-1".
      The overflow can only be reset by calling slFlush(<handle>).
      
    - Receiving a byte (Actually: reading it from the internal buffer):
    
        databyte = slReadByte(port1)     // NON-BLOCKING (returns immediately)
        databyte = slReadByte(port1,0)   // NON-BLOCKING (returns immediately, same as above)
        databyte = slReadByte(port1,1)   // BLOCKING     (waits until a byte was received)

      If anything is in the receiving buffer, slReadbyte() returns one byte of that data,
      here on handle "port1". If the second argument is "0" or skipped, the function returns
      immediately, even if the buffer is empty. A "1" as a second argument waits until at least
      one byte is received. It "blocks".
      slReadByte returns either a valid data byte (value 0..255) or "-1" if the receive
      buffer is empty, an overflow occurs or the port is not opened.
      Notice that "BLOCKING" really blocks. Scilab will do absolutely nothing until a byte
      arrives (*subject to change in future versions*).

    - Closing the COM port:
    
        ret = slClose(port1);
      
      After this call, the COM port is closed. You can not send or receive data anymore,
      but the handle is still valid for, e.g. slConfig(), which still allows changing
      communication parameters, slOpen(), to re-open the port, etc...
      
    - Unmount handle:
    
        ret = slUMount(port1)
    
      slUMount() frees the internal objects and dependecies. From now on, any access to this
      former handle is invalid, but the library is still loaded and ready to run.
      You still remember the return stuff, right? "0" is bad, "1" means good...
      
    - Unload library:

        ret = slUnload()
        
      Calling slUnload removes the library from memory. No go, until you execute
      exec("slLoadLib.sci") again.
      Do NOT call unload until the port is closed and unmounted!


    
3.0 WRAPPER FUNCTIONS AND VARIABLES

3.1 LIST OF AVAILABLE FUNCTIONS

    In no particular order:

      exec("slLoadLib.sci") Not a function, but required to load the library.
      slLoad                *SEE REF* Loads the library and links functions. Also in "slLoadLib.sci"!
      slInit                *SEE REF* Initializes some internal objects.     Also in "slLoadLib.sci"!
      slVersion             Returns the version of the library.
      slUnload              Unload the library.
      slMount               Returns a handle to a COM port and initializes internal objects.
      slUMount              Deletes the handle and frees internal objects.
      slCheck               Check availability of a COM port.
      slConfig              Configure a COM port (baudrate, databits, parity and stop bits).
      slOpen                Opens a COM port.
      slClose               Closes a COM port.
      slSendByte            Sends a byte to the COM port.
      slSendArray           Sends an array to the COM port.
      slCount               Counts the available bytes in the receive buffer.
      slReadByte            Reads a byte from the receive buffer.
      slFlush               Flushes the receive buffer and resets the overflow flag (if set)
      slReadArray           Reads up to 32 bytes of data from the buffer.
      slReadArrayN          Reads an arbitrary amount of data from the buffer.
      slSendPacket          Sends a <DLE><STX><DATA/DLE><DLE><ETX> packet.
      slReadPacket          Reads a <DLE><STX><DATA/DLE><DLE><ETX> packet.
      slCountPacket         Counts the number of packets in the receive buffer.
      slSetPacketStart      Allows configuration of the 2nd "packet start" bytes <STX>; e.g. <DLE><STX>
      slSetPacketEnd        Allows configuration of the 2nd "packet end" bytes <ETX>; e.g.   <DLE><ETX>
      slSetPacketChar       Allows configuration of the "special character" byte <DLE>; e.g. <DLE>

      
3.2 LIST OF AVAILABLE VARIABLES

      slFuncs               An array that keeps track of linked functions. Must not be deleted!

      
3.3 QUICK REFERENCE      

      NULL      = exec("slLoadLib.sci")
      NULL      = slLoad()
      <ret>     = slInit()
      <version> = slVersion(<handle>)
      <ret>     = slUnload()
      <handle>  = slMount()
      <handle>  = slMount(<handle>) *** DEPRECATED ***
      <ret>     = slUMount(<handle>)
      <ret>     = slCheck(<handle>, <port>)
      <ret>     = slConfig(<handle>, <baud>, <databits>, <parity>, <stopbits>)
      <ret>     = slOpen(<handle>, <port>)
      <ret>     = slClose(<handle>)
      <ret>     = slSendByte(<handle>, <byte>)
      <ret>     = slSendArray(<handle>, <array>, <size>)
      <count>   = slCount(<handle>)
      <byte>    = slReadByte(<handle>)
      <byte>    = slReadByte(<handle>,<blocking>)
      <ret>     = slFlush(<handle>)
      <array>   = slReadArray(<handle>)
      <array>   = slReadArrayN(<handle>, <length>)
      <ret>     = slSendPacket(<handle>, <array>, <size>)
      <array>   = slReadPacket(<handle>, <size>)
      <count>   = slCountPacket(<handle>)
      <ret>     = slSetPacketStart(<handle>, <byte>)
      <ret>     = slSetPacketEnd(<handle>, <byte>)
      <ret>     = slSetPacketChar(<handle>, <byte>)

      
3.4 REFERENCE

3.4.1 slInit

      <ret>     = slInit()

      Notice: Not required if you use "exec('slLoadLib.sci')"!
      
      Initializes some internal objects.
      Needs to be called once before you do anything else.
      
      RETURNS:
        1     = OK
        

3.4.2 slLoad
        
        NULL    = slLoad()
        
        Notice: Not required if you use "exec('slLoadLib.sci')"!
        
        Opens "siseli.dll", which has to be on your path (usually right inside the directory
        Scilab is currently operating in), and links the DLL functions.
        If any error occurs, execution will be aborted via "abort" (Scilab stops) and an
        error message is print out on the console.
        
        RETURNS:
          nothing

          
3.4.3 slVersion

      <version> = slVersion(<handle>)
      
        Returns a floating point representation of the libraries' version.
        E.g. V1.2 will return "1.2".
        Requires a valid <handle> argument.
        
        RETURNS:
          <version> = floating point number of library version

          
3.4.4 slUnload()

      <ret>     = slUnload()
      
        Unloads the library from memory by removing all DLL links.
        Make sure all ports are closed ("slClose(<handle>)") and unmounted ("slUMount(<handle>)")
        before calling this. Otherwise strange things may happen!
        
        Example:
        
          // 2 COM ports are referenced by handle 2 and 5 and opened (communicating).
          // The right way to shut down everything is:
          slClose(2)
          slClose(5)
          slUMount(2)
          slUMount(5)
          slUnload()
        
        RETURNS:
          1     = OK; Even returns "1" if library was previously unloaded (nothing dramatic).
          0     = Something went wrong. may be someone deleted "slFuncs", the link list?
     
     
3.4.5 slMount
      
      <handle>  = slMount()
      <handle>  = slMount(<handle>) *** DEPRECATED ***
      
        Creates internal objects and dependencies. Needs to be called once, before anything else.
        The returned value <handle> is an integer number and references the selected COMM port.
        This way, different COM ports, that are opened the same time, can be easily distinguished.
        
        As of V0.7, slMount() does not need to get called with an argument.
        To remain compatibility, the old function is still available, but its usage
        is not recommended.
        
        Example:
        
          // You already executed "exec('slLoadLib.sci')" to load the library.
          // Execute or type
          handle = slMount()
          // From now on, all the internal objects, necessary for operation, are available
          // and you can start to use the library functions.
          // Remember the "handle", as this is the reference to the comm port(s).
        
        RETURNS:
         >0     = a number (handle) to identify the selected COMM port
          0     = ERROR

          
3.4.6 slUMount
      
      <ret>     = slUMount(<handle>)
      
        Deletes internal objects and dependencies. After this is called, the <handle> is invalid
        and can not be used any more, but the library is still loaded.
        Notice that the COM port, reference by <handle>, needs to be closed via slClose(<handle>),
        prior to calling this!
        
        Example:
        
          // Close COM port, referenced by handle 3 and destroy object:
          slClose(3)
          slUMount(3)
        
        RETURNS:
          1     = OK, everything deleted
          0     = ERROR, something very strange happened (you better restart Scilab ;-)

          
3.4.7 slCheck

      <ret>     = slCheck(<handle>, <port>)
        
        Checks if COM port <port> is available and free.
        
        Example:
        
          // get a handle and check if COM4 is present and free:
          handle = slMount()
          if slCheck(handle,4) then
            printf("Port 4 available\n")
          else
            printf("Port over board!\n")
          end
        
        RETURNS:
          1     = port available, can be opened
          0     = port does not exist or is already opened by an other application


3.4.8 slConfig
      
      <ret>     = slConfig(<handle>, <baud>, <databits>, <parity>, <stopbits>)
      
        Configures a not-yet opened port:

          <baud>     = baud rate; 300..3000000 bits/sec
          <databits> = number of data bits; 5..8
          <parity>   = 0=NONE, 1=ODD, 2=EVEN
          <stopbits> = 1=one stop bit, 2=two stop bits, 15=1.5 stop bits
        
        Example:
        
          // Configure handle "port1" to 4800 bits/s, 8 databits, no parity, one stop bit:
          slConfig(port1, 4800, 8, 0, 1)
          
        Supported baudrates:
          300, 600,	1200,	2400,	4800,	9600,	14400, 19200, 38400, 56000, 57600, 115200,
          128000, 256000, 921600, 1000000, 2000000, 3000000
        
        RETURNS:
          1     = OK
          0     = you entered something stupid...
      
      
3.4.9 slOpen
      
      <ret>     = slOpen(<handle>, <port>)
      
        Opens COM port <port> on handle <handle>.
        After the call, the COM port is ready for usage.
        
        Example:
          
          // example sequence, e.g. after calling "exec('slloadlib.sci')".
          // Notice: No checks done!
          // get a handle
          handle = slMount()
          // configure it (115k2, 7O1)
          slConfig(handle, 115200, 7, 1, 1)
          // open COM4 on this port
          slOpen(handle,4)
        
        RETURNS:
          1     = OK
          0     = ERROR

3.4.10 slClose
      
      <ret>     = slClose(<handle>)
      
        Closes the COM port, referenced by <handle>.
        Note that the handle is still valid and the library still loaded!
        You may still use the handle for everything except for sending bytes or arrays.
        A read from the buffer is still possible, but probably does not make sense ;)
        After the call, the COM port is availabe for other applications.
        
        RETURNS:
          1     = OK
          0     = ERROR
          
      
3.4.11 slSendByte
      
      <ret>     = slSendByte(<handle>, <byte>)
      
        Sends a single byte <byte> 0..255, to the COM port, referenced by <handle>.
        The COM port needs to be configured ("slConfig()"), and open ("slOpen()").
        Notice that it might take "some time" (...) until the data is sent out. Windows ... ;-)

        Example:
        
          // send an "A" to the port, referenced by handle "port1":
          slSendByte(port1,ascii("A"))
          
        RETURNS:
          1     = OK
          0     = ERROR


3.4.12 slSendArray
      
      <ret>     = slSendArray(<handle>, <array>, <size>)
      
        Send an array of integers (although you may *carefully* try something else) <array>,
        of the size <size> (or lesser) to the COM port references by <handle>.
        If the given size argument exceeds the length of the array, a memory exception will
        occur and you have to restart Scilab.
        Notice that the complete Scilab <-> SiSeLi "communication" is pointer arithmetic.
        Unfortunately Scilab does not provide any (fast and failsafe) checks for this, so
        you really should take care of that.
        
        Scilab Tip:
          If you need to send a string (e.g.: "Hello"), you can easily convert it to an
          array by using:
          
            array = ascii("Hello")
            slSendArray(<handle>, array, length(array) )

        Example:
        
          // send "BLOED" to the port, referenced by handle "port1":
          slSendArray(port1, ascii("BLOED"), 5)
          
          // send an array of integers to the port, referenced by handle "port1":
          a=[68 79 79 70]
          slSendArray(port1, a, length(a))
            
        RETURNS:
          1     = OK
          0     = ERROR


3.4.13 slCount
      
      <count>   = slCount(<handle>)
      
        Counts the number of bytes available in the receive buffer, referenced by <handle>.
        
        RETURNS:
          <count> = number of bytes in receive buffer
          <-1>    = A buffer overflow occurred. You have to call slFlush() to reset it.
                    Also returned if called with an invalid handle.
          
          
          
3.4.14 slReadByte

      <byte>    = slReadByte(<handle>)
      <byte>    = slReadByte(<handle>,<blocking>)
      
        Reads (and removes) a byte from the receive buffer, referenced by <handle>.
        The <blocking> parameter, if given, specifies its behaviour:

             empty      -> the function always returns immediately
          <blocking=0>  -> the function always returns immediately
          <blocking=1>  -> the functions waits (blocks all) until at least one byte is available
          
        
        RETURNS:
          <byte>  = a received byte
          <-1>    = ERROR: nothing in buffer, overflow or port not opened or available

          
3.4.15 slFlush          
          
      <ret>     = slFlush(<handle>)
      
        Flushes (empties) the receive buffer, referenced by <handle>.
        If an overflow flag is set, it is reset.
        
        RETURNS
          1     = OK
          0     = ERROR


3.4.16 slReadArray

      <array>   = slReadArray(<handle>)

        To speed up reading, more than a single byte may be read by slReadArray(), on the port
        referenced by <handle>.
        Size of <array> is fixed to 33 bytes. The first byte determines the number of valid bytes
        following. The rest of the array is filled up with "-1"
        E.g.: If the buffer contains 5 received bytes

          65, 66, 67, 68, 69
          
        <array> will be:

          index:   1   2   3   4   5   6   7   8      33
          content: 5, 65, 66, 67, 68, 69, -1, -1, ... -1
          
        WARNING:
          Calling slReadArray() with an invalid handle may return garbage at index 2..33.
          The first byte will contain a valid "0" though...
        
        Example usage in Scilab:

          tmp=[]
          data=[]
          while 1 then
            tmp=slReadArray(1)
            data=[data tmp(1,2:1+tmp(1))]
            if tmp(1) < 32 then
              break
            end
          end
          
        Scilab Tip:
          If <array> contains a string (ascii characters), it can easily be converted via:
          
            str = ascii(<array>)
          
        RETURNS
          <array> = [ <N> <data1> <data2> ... <dataN> ]


3.4.17 slReadArrayN

      <array>   = slReadArray(<handle>, <length>)

        By popular demand:
        This function behaves exactly like "slReadArray" above, except it takes a second
        argument which specifies the length of data to read.

        <length> must be >1 and <31000.
        
        Size of <array> is at least <length+1>.
        The first byte determines the number of valid bytes following.
        The rest of the array is filled up with "-1"
        E.g.: If the buffer contains 5 received bytes

          65, 66, 67, 68, 69

        executing
        
          tmp = slReadArrayN(1, 1000)
          
         will return an <array> like this:

          index:   1   2   3   4   5   6   7   8     1001
          content: 5, 65, 66, 67, 68, 69, -1, -1, ... -1
          
        WARNING:
          Calling slReadArray() with an invalid handle may return garbage after index 1.
          The first byte will contain a valid "0" though...
          
        RETURNS
          <array> = [ <N> <data1> <data2> ... <dataN> ]
          

3.4.18 slSendPacket

      <ret>     = slSendPacket(<handle>, <array>, <size>)

        By default, slSendPacket() creates a
        
          < PSTART ><   DATA  ><  PEND  >

        packet. With
        
          <PSTART> = <DLE><STX>
          <PEND>   = <DLE><ETX>
      
        this results in a
          
          <DLE><STX><array+DLE><DLE><ETX>

        packet, which is then sent out to the com PORT, referenced by <handle>.
        If the given <size> argument exceeds the length of <array>, a memory exception will
        occur and you have to restart Scilab.
        Packet start <PSTART>, packet end <PEND> and the special <DLE> character can be
        configured by:
        
          slSetPacketStart()    // set <STX>; range from 0..255
          slSetPacketEnd()      // set <ETX>; range from 0..255
          slSetPacketChar()     // set <DLE>; range 0..255
          
        Example:
        
          // our array, that needs to be sent
          a = [14 15 16 17 18]
          
          // send it to handle "port1"
          slSendPacket(port1, a, length(a))

        Assuming that <PSTART> = <0x10><0x02>, <PEND> = <0x10><0x03> and <DLE> = <0x10>,
        the sent packet looks like this:
        
          <0x10> <0x02> < 14 > < 15 > < 16 > < 16 > < 17 > < 18 > <0x10> <0x03>
          
        In hexadecimal notation (notice that 16==0x10 ;-):
        
          <0x10> <0x02> <0x0E> <0x0F> <0x10> <0x10> <0x11> <0x12> <0x10> <0x03>
          
        In <array>, every character, that equals <DLE> ("packet character" or data link escape
        "DLE"), is doubled. It can be set via slSetPacketChar().

        RETURNS:
          1     = OK
          0     = ERROR

          
3.4.19 slReadPacket

      <array>   = slSendPacket(<handle>, <size>)

        Reads a packet from the receive buffer and returns it as <array>.
        All packet start and end markers, as well as the "packet character" <DLE> are removed.

        The first byte of <array> determines the number of valid bytes that are following, the
        rest of <array>, up to <size+1> is filled up with "-1".

        If <size> is smaller than the packet in the receive buffer, the rest of the packet is
        discarded.
        
        Example:

          Assuming that <PSTART> = <0x10><0x02>, <PEND> = <0x10><0x03> and <DLE> = <0x10>
          and the receive buffer contains

            <0x34> <0x65> <0x10> <0x02> <0x01> <0x10> <0x10> <0x03> <0x04> <0x10> <0x03> <0x65>
           |...ANYDATA...| PACKETSTART | DAT1 |  DLE DAT 2  | DAT3 | DAT4 | PACKET END  |...ANY
            
          slReadPacket(1, 30) will return:
          
            <0x04> <0x01> <0x10> <0x03> <0x04>
           |AMOUNT| DAT1 | DAT2 | DAT3 | DAT4



3.4.13 slCountPacket
      
      <count>   = slCountPacket(<handle>)
      
        Counts the number of packets available in the receive buffer, referenced by <handle>.
        
        RETURNS:
          <count> = number of packets in receive buffer
          <-1>    = a buffer overflow occured; you have to call slFlush() to reset it
           

          
3.4.21 slSetPacketStart

      <ret>     = slSetPacketStart(<handle>, <byte>)
      
        See slSendPacket() for a description of the packet sending mechanism.
        Sets the 2nd byte of "packet start" string on the COM port referenced by <handle>, to
        the content of <byte>.
        
        Examples:
        
          // Set packet start to <0x10><0x02> (the classic <DLE><STX>), on handle "port1".
          // The raw data array [       14 15 16 17 18 19 ], 6 bytes,
          // will get sent as   [ 16 02 14 15 16 17 18 19 ], 8 bytes.
          slSetPacketStart(port1, 2)
          
        RETURNS
          1     = OK
      

3.4.22 slSetPacketEnd

      <ret>     = slSetPacketEnd(<handle>, <byte>)
      
        See slSendPacket() for a description of the packet sending mechanism.
        Sets the 2nd byte of "packet end" string on the COM port referenced by <handle>, to
        the content of <byte>.
        
        Examples:
        
          // Set packet end to <0x10><0x03> (the classic <DLE><ETX>), on handle "port1".
          // The raw data array [ 14 15 16 17 18 19       ], 6 bytes,
          // will get sent as   [ 14 15 16 17 18 19 16 03 ], 8 bytes.
          slSetPacketEnd(port1, 3)

        RETURNS
          1     = OK


3.4.23 slSetPacketChar
      
      <ret>     = slSetPacketChar(<handle>, <byte>)
      
        Sets the "special character", also known as the classic <DLE> (data link escape), for
        the communication with the COM port referenced by <handle>.
        If <byte> is in the range 0..255, any data byte of the same value is doubled.
        As of SiSeLi V0.6, the special character is automatically used for packet start and
        packet end (<DLE>).
        
        Examples:

          // Use classic <DLE> (0x10) during transmission on handle "port1":
          // After the call, every data byte, with a value of "16" will be doubled:
          // The raw data array [ 14 15 16    17 18 16    ], 6 bytes,
          // will get sent as   [ 14 15 16 16 17 18 16 16 ], 8 bytes.
          slSetPacketChar(port1,16)
          
        RETURNS:
          1     = OK
        
          
    
9.0 CHANGES

    V0.1, initial release
    
    V0.1a, mini-fix
      - *NEW* added slInit() function
    
    V0.2, next steps
      - *FIX* removed some debug lines
      - *CHG* increased default RX buffer size to 30kB

    V0.3, never released
      - *NOP* bummer
    
    V0.4, tiny add-on
      - *NEW* added slReadArrayN() function

    V0.5, baudrate upgrade, fix
      - *NEW* added baudrates 256000, up to 3MBit/s
      - *FIX* error handling did not work for some functions; always reported "OK"

    V0.6, implemented packet read stuff
      - *NEW* added slReadPacket()
      - *NEW* added slCountPacket()
      - *CHG* reworked complete packet send/write algorithm
      - *CHG* increased simultaneously usable serial ports to 16

    V0.7, refactoring
      - *NEW* started to remove old C-style stuff
      - *NEW* unlimited amount of serial ports (previously limited to 16)
      - *CHG* slMount() does not require the handle argument any longer.
      - *CHG* slReadByte() does not require the 2nd argument if NONBLOCKING
