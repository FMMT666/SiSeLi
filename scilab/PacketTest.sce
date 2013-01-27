

COMPORT = 38


exec("slLoadLib.sci")
slMount(1)
slOpen(1,COMPORT)
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
