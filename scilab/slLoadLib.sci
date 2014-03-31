mode(0)  // echo off
// #############################
// ### SCILAB LIBRARY LOADER ### V2.0
// #############################
//
// ScilabSerialLib (SiSeLi)
// ASkr, 2010, 2012, 2014
// www.askrprojects.net
//
// Loads ScilabSerialLib into Scilab memory and creates DLL access wrapper functions.
// Requires SiSeLi V0.7+
//
// NOTICE:
//  ---------------------------------------------
//  --> No 100% error checks performed (yet)! <--
//  -->          Watch your steps!            <--
//  ---------------------------------------------
//
// SPECIAL NOTE ON slUnload():
//   I did not find a way to safely remove the Scilab wrapper
//   functions (all those "sl..." stuff) within slUnload().
//   Do not call any "sl...()" function after "slUnload()"
//   was executed!
//
//
// CHANGES V1.0, 10/2012:
//  - added "slReadArrayN()",  "recvan()"
//
// CHANGES V1.1, 10/2012:
//  - added "slReadPacket()",  "recvap()"
//  - added "slCountPacket()", "countp()"
//  - changed 
//      - slSetPacketStart()
//      - slSetPacketEnd()
//      - slSetPacketChar()
//    They use only >>1<< character, now (see documentation)
//
// CHANGES V2.0, 04/2014:
//  - changed "slMount()" behaviour (no argument by default, but remain compatibility)
//  - changed "slReadByte()" behaviour (2nd argument not required; NONBLOCKING by default)
// 


SLLOADERVERSION = "2.0";


// check if lib is already loaded
if mtlb_exist("slFuncs") then
    
    if size(slFuncs)~=[0 0] then
        printf("\n*\n* WARNING: ScilabSerialLib already loaded!\n*\n\n")
        abort
    end
    
else
    global slFuncs
    slFuncs=[];
end



//*****************************************************************************
//*** slLoad
//*****************************************************************************
function slLoad()
    global slFuncs;
    printf("\n---\nSISELI Loader V%s\n\n", SLLOADERVERSION);
    try
        slFuncs(1,1)= link("siseli.dll","init","c");
        slFuncs(1,2)= link("siseli.dll","version","c");
        slFuncs(1,3)= link("siseli.dll","mount","c");
        slFuncs(1,4)= link("siseli.dll","umount","c");
        slFuncs(1,5)= link("siseli.dll","check","c");
        slFuncs(1,6)= link("siseli.dll","config","c");
        slFuncs(1,7)= link("siseli.dll","open","c");
        slFuncs(1,8)= link("siseli.dll","close","c");
        slFuncs(1,9)= link("siseli.dll","sendb","c");
        slFuncs(1,10)=link("siseli.dll","senda","c");
        slFuncs(1,11)=link("siseli.dll","count","c");
        slFuncs(1,12)=link("siseli.dll","recvb","c");
        slFuncs(1,13)=link("siseli.dll","flush","c");
        slFuncs(1,14)=link("siseli.dll","recva","c");
        slFuncs(1,15)=link("siseli.dll","sendp","c");
        slFuncs(1,16)=link("siseli.dll","packs","c");
        slFuncs(1,17)=link("siseli.dll","packe","c");
        slFuncs(1,18)=link("siseli.dll","packc","c");
        slFuncs(1,19)=link("siseli.dll","recvan","c");
        slFuncs(1,20)=link("siseli.dll","recvap","c");
        slFuncs(1,21)=link("siseli.dll","countp","c");
    catch
        printf("\n*\n")
        printf("* ERROR: Unable to load ScilabSerialLibrary\n*\n\n")
        abort
    end
endfunction


//*****************************************************************************
//*** slInit
//*****************************************************************************
function res=slInit()
    res=fort("init","out",[1,1],1,"i")
endfunction    


//*****************************************************************************
//*** slVersion
//*****************************************************************************
function res=slVersion()
    res=fort("version","out",[1,1],1,"i")/10
endfunction    


//*****************************************************************************
//*** slUnload
//*****************************************************************************
function res=slUnload()
    global slFuncs
    try
        for i=1:size(slFuncs,2)
            ulink(slFuncs(i));
        end
        res=1;
        slFuncs=[];
    catch
        printf("\n*\n* WARNING: Nothing to unload!\n*\n\n");
        res=0;
    end
endfunction    


//*****************************************************************************
//*** slMount
//*****************************************************************************
function res=slMount(nHandle)
    // New behaviour (4/2014) does not require an argument, but
    // we remain compatible...
    if exists('nHandle','l') then
        printf("WARNING: Calling slMount() with argument is deprecated.\n")
        res=fort("mount",nHandle,1,"i","out",[1,1],2,"i")
    else
        res=fort("mount",0,      1,"i","out",[1,1],2,"i")
    end
endfunction


//*****************************************************************************
//*** slUMount
//*****************************************************************************
function res=slUMount(nHandle)
    res=fort("umount",nHandle,1,"i","out",[1,1],2,"i")
endfunction


//*****************************************************************************
//*** slCheck
//*****************************************************************************
function res=slCheck(nHandle, nPort)
    res=fort("check",nHandle,1,"i",nPort,2,"i","out",[1,1],3,"i")
endfunction


//*****************************************************************************
//*** slConfig
//*****************************************************************************
function res=slConfig(nHandle, nBaud, nBits, nPar, nStop)
    res=fort("config",nHandle,1,"i",nBaud,2,"i",nBits,3,"i",nPar,4,"i",nStop,5,"i","out",[1,1],6,"i")
endfunction


//*****************************************************************************
//*** slOpen
//*****************************************************************************
function res=slOpen(nHandle, nPort)
    res=fort("open",nHandle,1,"i",nPort,2,"i","out",[1,1],3,"i")
endfunction


//*****************************************************************************
//*** slClose
//*****************************************************************************
function res=slClose(nHandle)
    res=fort("close",nHandle,1,"i","out",[1,1],2,"i")
endfunction


//*****************************************************************************
//*** slSendByte
//*****************************************************************************
function res=slSendByte(nHandle, nByte)
    res=fort("sendb",nHandle,1,"i",nByte,2,"i","out",[1,1],3,"i")
endfunction


//*****************************************************************************
//*** slSendArray
//*****************************************************************************
function res=slSendArray(nHandle, nByte, nLength)
    res=fort("senda",nHandle,1,"i",nByte,2,"i",nLength,3,"i","out",[1,1],4,"i");
endfunction


//*****************************************************************************
//*** slCount
//*****************************************************************************
function res=slCount(nHandle)
    res=fort("count",nHandle,1,"i","out",[1,1],2,"i")
endfunction


//*****************************************************************************
//*** slCountPacket
//*****************************************************************************
function res=slCountPacket(nHandle)
    res=fort("countp",nHandle,1,"i","out",[1,1],2,"i")
endfunction


//*****************************************************************************
//*** slReadByte
//*****************************************************************************
function res=slReadByte(nHandle, nBlock)
    if exists('nBlock','l') then
        res=fort("recvb",nHandle,1,"i",nBlock,2,"i","out",[1,1],3,"i")
    else
        res=fort("recvb",nHandle,1,"i",0     ,2,"i","out",[1,1],3,"i")
    end
endfunction


//*****************************************************************************
//*** slFlush
//*****************************************************************************
function res=slFlush(nHandle)
    res=fort("flush",nHandle,1,"i","out",[1,1],2,"i")
endfunction


//*****************************************************************************
//*** slReadArray
//***  - First byte in the array is the amount of data available
//***  - Maximum data size is 32 bytes
//***  => therefore, array size is always (1,33)
//*****************************************************************************
function res=slReadArray(nHandle)
    tmp=ones(1,33)*-1;
    tmp=fort("recva",nHandle,1,"i","out",[1,33],2,"i");
    res=tmp;
endfunction


//*****************************************************************************
//*** slReadArrayN
//***  - First byte in the array is the amount of data available
//***  => therefore, array size is always <nLength + 1> (+2 for safety ;-)
//***  - 2nd parameter specifies number of bytes to read
//*****************************************************************************
function res=slReadArrayN(nHandle, nLength)
    tmp=ones(1,nLength+1)*-1;
    tmp=fort("recvan",nHandle,1,"i",nLength,2,"i","out",[1,nLength+1],3,"i");
    res=tmp;
endfunction


//*****************************************************************************
//*** slReadPacket
//***  - First byte in the array is the amount of data available
//***  => therefore, array size is always <nLength + 1> (+2 for safety ;-)
//***  - 2nd parameter specifies number of bytes to read
//*****************************************************************************
function res=slReadPacket(nHandle, nLength)
    tmp=ones(1,nLength+1)*-1;
    tmp=fort("recvap",nHandle,1,"i",nLength,2,"i","out",[1,nLength+1],3,"i");
    res=tmp;
endfunction


//*****************************************************************************
//*** slSendPacket
//*****************************************************************************
function res=slSendPacket(nHandle, nByte, nLength)
    res=fort("sendp",nHandle,1,"i",nByte,2,"i",nLength,3,"i","out",[1,1],4,"i");
endfunction


//*****************************************************************************
//*** slSetPacketStart
//*****************************************************************************
function res=slSetPacketStart(nHandle, nByte)
    res=fort("packs",nHandle,1,"i",nByte,2,"i","out",[1,1],3,"i");
endfunction


//*****************************************************************************
//*** slSetPacketEnd
//*****************************************************************************
function res=slSetPacketEnd(nHandle, nByte)
    res=fort("packe",nHandle,1,"i",nByte,2,"i","out",[1,1],3,"i");
endfunction


//*****************************************************************************
//*** slSetPacketChar
//*****************************************************************************
function res=slSetPacketChar(nHandle, nByte)
    res=fort("packc",nHandle,1,"i",nByte,2,"i","out",[1,1],3,"i")
endfunction


//*****************************************************************************


// load library functions
slLoad();

if mtlb_exist("slVersion") then
    printf("\n\nSISELI Version %3.1f loaded\n---\n", slVersion() );
else
    printf("\nERROR: Unable to load library\n---\n")
    abort;
end

slInit();

// echo on
mode(1)

