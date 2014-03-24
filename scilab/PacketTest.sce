//
// ScilabSerialLib (SiSeLi) PACKETDEMO, V0.2
// ASkr, 01/2013, 03/2014
// www.askrprojects.net
//
//
// READ THIS (PLEASE?)
//
//-----
// 1) CHANGE YOUR COM-PORT HERE:
COMPORT = 5
//
//-----
// 2) YOU DON'T NEED TO LOAD ANYTHING EXTRA
//    In contrast to the "LoopDemo.sce" demonstration, you
//    don't need to load the SiSeLi DLL via exec("slLoadLib.sci").
//    This was already built in here (see below), but will fail
//    if you previously loaded the DLL manually.
//    To unload it use:
//       slUnload()
//    assuming you already closed the COM port and "unmounted"
//    the COM port handle:
//       slClose(1)
//       slUMount(1)
//    
//
// CHANGES V0.2:
//   - added invalid COM port message
//   - made output of functions visible (mode(1))
//   - added text
//

exec("slLoadLib.sci")
slMount(1)
ret = slOpen(1,COMPORT)
if ret == 0 then
    printf("unable to open COM port %f\n", COMPORT)
    mode(-1)
else
    mode(1)
end
a = [1,2,3,16]

slSendByte(1,65)
slSendByte(1,65)
slSendByte(1,65)

slSendPacket(1,a,3)
slSendPacket(1,a,4)
slSendPacket(1,a,3)

slSendByte(1,65)
slSendByte(1,65)

slSendPacket(1,a,3)
slSendPacket(1,a,4)

sleep(1000)

slCountPacket(1)

slReadPacket(1,5)
slReadPacket(1,5)

slCountPacket(1)

slReadPacket(1,5)
slReadPacket(1,5)
slReadPacket(1,5)
slReadPacket(1,5)

slClose(1)
slUMount(1)
slUnload()

mode(2)
