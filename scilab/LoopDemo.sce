//
// ScilabSerialLib (SiSeLi) DEMO, V0.3
// ASkr, 10/2012, 03/2014
// www.askrprojects.net
//
//
// READ THIS (PLEASE?)
//
//-----
// 1) CHANGE YOUR COM-PORT HERE
COMPORT = 5
//
//-----
// 3) TYPE THIS IN YOUR SCILAB CONSOLE
//      exec("slLoadLib.sci")
//
//-----
// 4) CREATE A SERIAL LOOP
//    Make an RX<->TX loop (connect them together)
//    to receive the already sent bytes.
//
//
//
// NOTICE:
//   This is just a demonstration...
//   None of the function calls evaluates the return argument (OK/ERROR)!
//   You can do better...
//
//
//
// CHANGES V0.3:
//  - added a COM port variable
//  - added some text
//
// CHANGES V0.2:
//  - added new function "slReadArrayN()"
//



mode(1)

// print library version
printf("SISELI Version %3.1f\n",slVersion());

// mount library on handle "1"
slMount(1);

// handle "1": check availability of specified COM port
slCheck(1,COMPORT);

// handle "1": configure port
slConfig(1,19200,8,0,1);

// handle "1": open port
slOpen(1,COMPORT);

mode(2);

// handle "1": send some data
for i=0:25
    slSendByte(1,ascii('A')+i);
end

// handle "1": check number of bytes in RX buffer
// It takes some time for the threads to complete...
sleep(100);
count=slCount(1);
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
count=slCount(1);
printf("Bytes received: %d\n",count);

// handle "1": read (non-blocking) and print complete receive buffer
for i=1:count
    printf("%d ",slReadByte(1,0));
    if ~modulo(i,10) then
        printf("\n");
    end
end
printf("\n")

// handle "1": overflow buffer (might take some time)
n=0;
while slCount(1) >= 0 then
    slSendByte(1,int(rand()*26+65));
    i=i+1;
    if ~modulo(i,100) then
        printf("%d\n",i);
    end
    if i>32000 then
        printf("Did you connect RX and TX??\naborting...\n\n");
        abort;
    end
end
printf("On overflow, slCount() reports: %d\n",slCount(1));
printf("flush buffer via slFlush()\n");
sleep(100); // wait a little to give data a chance to arrive
slFlush(1);
printf("slCount() now reports: %d\n",slCount(1));

// handle "1": Send 3*26=78 characters and receive them via slReadArray()
printf("sending 78 characters...\n");
for n=1:3
    for i=0:25
        slSendByte(1,ascii('A')+i);
    end
end
sleep(100);
printf("slCount() now reports: %d\n",slCount(1));

tmp=[];
data=[];
for i=1:4
    // slReadArray() always returns 33 bytes.
    // The first byte is the number of valid bytes following.
    tmp=slReadArray(1);
    // add valid data to tmp
    data=[data tmp(1,2:1+tmp(1))];
end
printf("We received: ")
a=ascii(data)
printf("%s\n",a);
printf("length: %d\n",length(data));
disp(data);
printf("\n");

// handle 1, send a packet
// Use ASCII characters:
printf("Send 210KEKS012 in packet mode\n")
printf("ASCII numbers for packet start, end and character:\n")
printf("  DLE=0, STX=8, ETX=9\n")
slSetPacketStart(1,[ascii("0") ascii("8")]);
slSetPacketEnd(1,[ascii("0") ascii("9")]);
slSetPacketChar(1,ascii("0"));
slSendPacket(1,ascii("210KEKS012"),10);
sleep(100);
tmp=slReadArray(1);
data=tmp(1,2:1+tmp(1));
printf("We received: ");
ascii(data)


// *** NEW V0.2 *** slReadArrayN()
// create an 5x5 array 
a=ones(1,5);
for i=1:5
    a(i,:)=[i:i+4]+65-i+(i-1)*5;
end

// clear buffer
slFlush(1)

// send array 20 times
printf("Writing 500 bytes to serial.\n");
for i=1:20
  slSendArray(1,a,length(a));
end

sleep(100); // wait a little to give data a chance to arrive
printf("slCount() now reports: %d\n",slCount(1));

// try to read 1000 bytes:
a1 = slReadArrayN(1,1000);

printf("Read %d bytes from serial, via slReadArrayN().\n", a1(1) );


// uncomment the following line to unload everything...

// handle "1": close port (frees COM port for other tasks)
//slClose(1);

// handle "1": unmount libs and free internal objects
//slUMount(1);

// unload all libraries
//slUnload();

