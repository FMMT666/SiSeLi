//
// ScilabSerialLib (SiSeLi) ALL-IN-ONE-DEMO, V0.1
// ASkr, 10/2010
// www.askrprojects.net
//
// This is an all-in-one-file demonstration of ScilabSerialLib's usage.
// It is just provided for a better insight of the DLL's usage.
// Usually, you just should use the provided loader script "slLoadLib.sci",
// by executing:
//
//   exec("slLoadLib.sci")
//
// After doing so, only the stuff (right here in this file) after the
// >>> ********************************************
// >>> *** exec("slLoadLib.sci") starting point ***
// >>> ***           (see above)                ***
// >>> ********************************************
// marker is required (including an example on how to unload the library).
// Just scroll down...
//
// *
// * NOTICE THAT THIS FILE MIGHT NOT CONTAIN THE NEWEST VERSIONS
// * OF THE WRAPPER FUNCTIONS. IF IN DOUBT, ALWAYS LOOK AT
// *   "slLoadLib.sci" or
// *   "LoopDemo.sce"
// *
//
//
// Make an RX<->TX loop (connect them together)
// to receive the already sent bytes.
//



//*****************************************************************************
//*** slLoad
//*****************************************************************************
function slLoad()
    slFuncs(1,1)= link("siseli.dll","version","c");
    slFuncs(1,2)= link("siseli.dll","mount","c");
    slFuncs(1,3)= link("siseli.dll","umount","c");
    slFuncs(1,4)= link("siseli.dll","check","c");
    slFuncs(1,5)= link("siseli.dll","config","c");
    slFuncs(1,6)= link("siseli.dll","open","c");
    slFuncs(1,7)= link("siseli.dll","close","c");
    slFuncs(1,8)= link("siseli.dll","sendb","c");
    slFuncs(1,9)= link("siseli.dll","senda","c");
    slFuncs(1,10)=link("siseli.dll","count","c");
    slFuncs(1,11)=link("siseli.dll","recvb","c");
    slFuncs=resume(slFuncs)
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
function slUnload()
    for i=1:size(slFuncs,2)
        ulink(slFuncs(i))
    end
    clear slFuncs
endfunction    


//*****************************************************************************
//*** slMount
//*****************************************************************************
function res=slMount(nHandle)
    res=fort("mount",nHandle,1,"i","out",[1,1],2,"i")
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
    res=fort("senda",nHandle,1,"i",nByte,2,"i",nLength,3,"i","out",[1,1],4,"i")
endfunction


//*****************************************************************************
//*** slCount
//*****************************************************************************
function res=slCount(nHandle)
    res=fort("count",nHandle,1,"i","out",[1,1],2,"i")
endfunction


//*****************************************************************************
//*** slCount
//*****************************************************************************
function res=slReadByte(nHandle, nBlock)
    res=fort("recvb",nHandle,1,"i",nBlock,2,"i","out",[1,1],3,"i")
endfunction



//*****************************************************************************
//*** DEMO APP ****************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

// load library functions
slLoad();



// ********************************************
// *** exec("slLoadLib.sci") starting point ***
// ***           (see above)                ***
// ********************************************


// print library version
printf("SISELI Version %3.1f\n",slVersion());

// mount library on handle "1"
slMount(1);

// handle "1": check availability of COM17
slCheck(1,17);

// handle "1": configure port
slConfig(1,19200,8,0,1);

// handle "1": open port
slOpen(1,17);

// handle "1": send some data
for i=0:25
    slSendByte(1,ascii('A')+i);
end

// handle "1": check number of bytes in RX buffer
sleep(100);
count=slCount(1)
printf("Bytes received: %d\n",count);

// handle "1": send an array
a=ones(1,5);
for i=1:5
    a(i,:)=[i:i+4]+65-i+(i-1)*5;
end
slSendArray(1,a(1,:),size(a,2));
slSendArray(1,a(:,1),size(a,1));
slSendArray(1,a,length(a));
slSendArray(1,a',length(a));
slSendByte(1,ascii('Z'));        // to make this one complete ;-)

// handle "1": check number of bytes in RX buffer
sleep(100);
count=slCount(1)
printf("Bytes received: %d\n",count);

// handle "1": read (non-blocking) and print complete receive buffer
for i=1:count
    printf("%d",slReadByte(1,0))
end
printf("\n")



// handle "1": close port (frees COM port for other tasks)
slClose(1);

// handle "1": unmount libs and free internal objects
slUMount(1);

// unload all libraries
slUnload();


