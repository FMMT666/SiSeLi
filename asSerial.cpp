//
// asSerial, a CSerial wrapper object (for Scilab)
// FMMT666(ASkr), 2010, 2012, 2013; www askrprojects.net
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>

#include "asSerial.h"

// Notice that the CSerial library is loaded as a library...
// Modified source code can be found in the source/cserial directory (distributioon-ZIP)
#include "Serial.h"

// "private" defines
#define BUF_RX_SIZE 		30000
#define BUF_TX_SIZE 		1024
//#define TIME_RX_THREAD	   5	// polling delay of RX thread (if commented out, sleep() is skipped)

#if BUF_RX_SIZE > 31000
#error BUF_RX_SIZE must not exceed 31000 bytes! Check "asSerial.cpp"!
#endif
#if BUF_TX_SIZE > 31000
#error BUF_TX_SIZE must not exceed 31000 bytes! Check "asSerial.cpp"!
#endif

#define CLOSED					0
#define OPENED					1


template <class T>
inline std::string myitoa (const T& t)
{
	std::stringstream strs;
	strs << t;
	return strs.str();
}



//************************************************************************************************
//*** asSerial::Secret
//***
//*** d-pointer style secret stuff of asSerial
//************************************************************************************************
class asSerial::Secret
{
	public:
		Secret();
		~Secret();	
		
		// the main serial interface
		class CSerial serial;

		// port related variables
		int				port;									// COM port 1-99
		long			baudrate;							// 300-256000
		int				bits;									// 5, 6, 7, 8
		int				parity;								// NOPARITY, ODDPARITY, EVENPARITY
		int				stop;									// NOSTOPBIT, ONE5STOPBITS, TWOSTOPBITS

		int				packetMode;						// 1 enables packet mode (only valid for receiving data)
		int				packetStart[2];				// packet start identifier (e.g.: 0x10,0x02)
		int				packetEnd[2];					// packet end identifier (e.g.: 0x10,0x03)
		int				packetChar;						// packet "special character" (e.g.: 0x10)

		// buffer stuff
		int				RXBufWriteByte(int ch);
		int				RXBufReadByte(int blocked);
		int				RXBufSnifByte();			// "read" byte without removing it from the buffer
		void			RXBufSnifReset();			// resets the sniff pointer to the current read index
		void			RXBufFlush();
		int				RXBufCount();					// returns number of bytes in RX buffer; returns -1 on overflow

		// RX Thread
		static DWORD WINAPI RXBufThread(LPVOID param);
		HANDLE	RXBufThreadStart();		// TODO -> may be private?
		void		RXBufThreadStop();		// TODO -> may be private?
		HANDLE	RXBufThreadRunning;		// thread handle
		bool		RXBufThreadFinish;		// thread-running marker

		// even more secret stuff ;)
	private:
		int				portState;								// port state
		
		// port buffers and vars (internal)
		void			RXBufInit();					
		void			RXBufFill(char ch);		// writes new data to RX buffer
		unsigned char RXBuf[BUF_RX_SIZE];	// RX ringbuffer (filled by thread)
		unsigned char TXBuf[BUF_RX_SIZE]; // TX ringbuffer
		volatile unsigned RXBufW;					// write buffer index
		volatile unsigned	RXBufR;					// read buffer index
		volatile unsigned	RXBufC;					// number of bytes in buffer
		volatile unsigned	RXBufO;					// overflow flag
		volatile unsigned	RXBufS;					// snif buffer index
		
};



//************************************************************************************************
//** asSerial::Secret ****************************************************************************
//**                  ****************************************************************************
//**                  ****************************************************************************
//************************************************************************************************

//************************************************************************************************
//*** asSerial::Secret::Secret()
//***
//***
//************************************************************************************************
asSerial::Secret::Secret()
{
	baudrate=	CSerial::EBaudrate(19200);
	bits=			CSerial::EDataBits(8);
	parity=		CSerial::EParity(NOPARITY);
	stop=			CSerial::EStopBits(ONESTOPBIT);

	packetMode=			0;	// only for RX thread; TX can be used all the time
	
	// Those a nice, default values. Saves additional calls in Scilab (for me ;-)
	packetChar=			0x10;
	packetStart[0]=	packetChar;		// to reflect the new packet strategy (V06)
	packetStart[1]=	0x02;
	packetEnd[0]=		packetChar;		// to reflect the new packet strategy (V06)
	packetEnd[1]=		0x03;

	RXBufThreadRunning=NULL;
	RXBufThreadFinish=true;

	RXBufInit();
	
	portState=CLOSED;
}



//************************************************************************************************
//*** asSerial::Secret::~Secret()
//***
//***
//************************************************************************************************
asSerial::Secret::~Secret()
{
}



//************************************************************************************************
//*** asSerial::Secret::RXBufThread()
//***
//***
//************************************************************************************************
DWORD WINAPI asSerial::Secret::RXBufThread(LPVOID param)
{
	int ch;
	DWORD count;

	asSerial::Secret *pt = static_cast<asSerial::Secret *>(param);

	do
	{
		for(;;)
		{
			pt->serial.Read((int *)&ch,1,&count);
			if(count)
				pt->RXBufWriteByte(ch);
			else
				break;
		}

#ifdef TIME_RX_THREAD
		Sleep(TIME_RX_THREAD);
#endif
		
	}	while(!pt->RXBufThreadFinish);
	
	pt->RXBufThreadRunning=NULL;
}




//************************************************************************************************
//*** asSerial::Secret::RXBufThreadStart()
//***
//***
//************************************************************************************************
HANDLE asSerial::Secret::RXBufThreadStart()
{
	// don't try this at home ;)
	RXBufThreadFinish=false;
	return (RXBufThreadRunning = CreateThread(NULL,0,&RXBufThread,static_cast<void*>(this),0,NULL));
}



//************************************************************************************************
//*** asSerial::Secret::RXBufThreadStop()
//***
//***
//************************************************************************************************
void asSerial::Secret::RXBufThreadStop()
{
	// don't try this t home ;-)
	RXBufThreadFinish=true;
}




//************************************************************************************************
//*** asSerial::Secret::RXBufInit()
//***
//***
//************************************************************************************************
void asSerial::Secret::RXBufInit()
{
	RXBufW=0;
	RXBufR=0;
	RXBufC=0;
	RXBufO=0;
	RXBufS=0;
}



//************************************************************************************************
//*** asSerial::Secret::RXBufWriteByte()
//***
//***
//************************************************************************************************
int asSerial::Secret::RXBufWriteByte(int ch)
{
	// if an overflow already occured, do nothing
	if(RXBufO)
 		return 0;
	
	// check end of ringbuffer
	if((RXBufW++) >= BUF_RX_SIZE )
		RXBufW=0;

	// check overflow
	if(RXBufW == RXBufR )
	{
		RXBufInit();
		RXBufO=1;
		return 0;
	}

	RXBuf[RXBufW]=(unsigned char)(ch&0xff);
	RXBufC++;
	
	return 1;
}



//************************************************************************************************
//*** asSerial::Secret::RXBufReadByte()
//***
//*** TODO: no read in here, yet!
//************************************************************************************************
int asSerial::Secret::RXBufReadByte(int blocked)
{
	if(blocked==BLOCKING)
	{
		while (RXBufW == RXBufR)
		{;}
	}
	else
	{
		if(RXBufW == RXBufR)
			return -1;
	}
	
	if((RXBufR++) >= BUF_RX_SIZE )
		RXBufR=0;

	if(RXBufC>0)
		RXBufC--;

	// TODO: Is it wise to reset the sniff-buffer	pointer here?
	RXBufS=RXBufR;
		
	return RXBuf[RXBufR] & 0xff;
}



//************************************************************************************************
//*** asSerial::Secret::RXBufSnifByte()
//***
//*** TODO: no read in here, yet!
//************************************************************************************************
int asSerial::Secret::RXBufSnifByte()
{
	if(RXBufW == RXBufS)
		return -1;
	
	if((RXBufS++) >= BUF_RX_SIZE )
		RXBufS=0;
	
	return RXBuf[RXBufS] & 0xff;
}



//************************************************************************************************
//*** asSerial::Secret::RXBufSnifReset()
//***
//***
//************************************************************************************************
void asSerial::Secret::RXBufSnifReset()
{
	RXBufS=RXBufR;
}



//************************************************************************************************
//*** asSerial::Secret::RXBufFLush()
//***
//***
//************************************************************************************************
void asSerial::Secret::RXBufFlush()
{
	RXBufInit();	// "delete" buffer
}



//************************************************************************************************
//*** asSerial::Secret::RXBufCount()
//***
//***
//************************************************************************************************
int asSerial::Secret::RXBufCount()
{
	if(RXBufO)
		return -1;	// overflow
	else
		return RXBufC;
}






//************************************************************************************************
//** asSerial ************************************************************************************
//**          ************************************************************************************
//**          ************************************************************************************
//************************************************************************************************

//************************************************************************************************
//*** asSerial::asSerial
//***
//***
//************************************************************************************************
asSerial::asSerial() : my (new Secret)
{
}



//************************************************************************************************
//*** asSerial::~asSerial
//***
//***
//************************************************************************************************
asSerial::~asSerial()
{
	delete my;
}



//************************************************************************************************
//*** asSerial::ConfigComm
//***
//***
//************************************************************************************************
int asSerial::ConfigComm(long baud, int bits, int parity, int stop)
{
	// TODO:
	// Actually, these checks do not check ;)
	if( (baud < 300) || (baud > 3000000) )
		return ERROROR;
	if( (bits < 5) || (bits > 8) )
		return ERROROR;
	if( (parity<0) || (parity>2) )  // reduce parity to NONE, ODD and EVEN
		return ERROROR;

	my->baudrate = CSerial::EBaudrate(baud);	
	my->bits =     CSerial::EDataBits(bits);	

	switch(parity)
	{
		case 0: my->parity = CSerial::EParity(NOPARITY); break;
		case 1: my->parity = CSerial::EParity(ODDPARITY); break;
		case 2: my->parity = CSerial::EParity(EVENPARITY); break;
		default: return ERROROR;
	}
	
	switch(stop)
	{
		case 1: my->stop = CSerial::EStopBits(ONESTOPBIT); break;
		case 2: my->stop = CSerial::EStopBits(TWOSTOPBITS); break;
		case 15:my->stop = CSerial::EStopBits(ONE5STOPBITS); break;
		default: return ERROROR;
	}
	
	return ERROR_SUCCESS;
}




//************************************************************************************************
//*** asSerial::Check()
//***
//***
//************************************************************************************************
int asSerial::Check(int port)
{
	std::string tmp="\\\\.\\COM"+myitoa(port);
	if (my->serial.CheckPort(tmp.c_str()) == CSerial::EPortAvailable)
		return 1;
	else
		return 0;
}



//************************************************************************************************
//*** asSerial::Open()
//***
//***
//************************************************************************************************
int asSerial::Open(int port)
{
	std::string tmp="\\\\.\\COM"+myitoa(port);
	LONG res = my->serial.Open(tmp.c_str(),0,0,false);
	if(res==ERROR_SUCCESS)
	{
		my->serial.SetupReadTimeouts(CSerial::EReadTimeout(0));		
		if(my->serial.Setup(
				CSerial::EBaudrate(my->baudrate),
				CSerial::EDataBits(my->bits),
				CSerial::EParity(my->parity),
				CSerial::EStopBits(my->stop))
				== ERROR_SUCCESS)
		{
			if(my->RXBufThreadStart()==NULL)
				res=ERROROR;
		}
		else
			res=ERROROR;
	}
	return res;
}



//************************************************************************************************
//*** asSerial::Close()
//***
//***
//************************************************************************************************
int asSerial::Close(void)
{
	my->RXBufThreadStop();
	return (int)my->serial.Close();
}



//************************************************************************************************
//*** asSerial::SendRaw()
//***
//***
//************************************************************************************************
int asSerial::SendRaw(int *data, int len)
{
	return (int) my->serial.Write(data,len);
}



//************************************************************************************************
//*** asSerial::SendRawByte()
//***
//***
//************************************************************************************************
int asSerial::SendRawByte(int ch)
{
	return (int) my->serial.Write((char *)&ch,1);
}



//************************************************************************************************
//*** asSerial::RecvRawByte()
//***
//***
//************************************************************************************************
int asSerial::RecvRawByte(int blocked=NONBLOCKING)
{
	return my->RXBufReadByte(blocked);
}


//************************************************************************************************
//*** asSerial::BufferCount()
//***
//***
//************************************************************************************************
int asSerial::BufferCount()
{
	return my->RXBufCount();
}


//************************************************************************************************
//*** asSerial::BufferFlush()
//***
//***
//************************************************************************************************
void asSerial::BufferFlush()
{
	my->RXBufFlush();
}


//************************************************************************************************
//*** asSerial::ConfigPacketStart()
//***
//***
//************************************************************************************************
int asSerial::ConfigPacketStart(int *start)
{
	int i;
	
	my->packetStart[1] = *start;
	if( my->packetStart[1] > 255 )
		my->packetStart[1] = 255;
	if( my->packetStart[1] < 0 )
		my->packetStart[1] = 0;

	return ERROR_SUCCESS;
}


//************************************************************************************************
//*** asSerial::ConfigPacketEnd()
//***
//***
//************************************************************************************************
int asSerial::ConfigPacketEnd(int *end)
{
	int i;
	
	my->packetEnd[1] = (*end++);
	if( my->packetEnd[1] > 255 ) 
		my->packetEnd[1] = 255;
	// future upgrade already implemented ;-)		
	if( my->packetEnd[1] < -1 ) 
		my->packetEnd[1] = -1;
		
	return ERROR_SUCCESS;
}


//************************************************************************************************
//*** asSerial::ConfigPacketChar()
//***
//***
//************************************************************************************************
int asSerial::ConfigPacketChar(int ch)
{
	my->packetChar = ch;
	if( my->packetChar > 255 )
		my->packetChar = 255;
	if( my->packetChar < 0 )
		my->packetChar = 0;
	// NEW as of V06
	my->packetStart[0] = my->packetChar;
	my->packetEnd[0] = my->packetChar;
		
	return ERROR_SUCCESS;	
}



//************************************************************************************************
//*** asSerial::SendPacket()
//***
//***
//************************************************************************************************
int asSerial::SendPacket(int *data, int len)
{
	int i;
	int OK = ERROROR;
	int nlen = 0;
	
	unsigned char *tmp = new unsigned char[ len * 2 + 12]; // worst case
	unsigned char *org = tmp;
	int *pj = data;
	
	if( tmp != NULL )
	{
		// packet start
		for( i = 0; i < 2; i++)
		{
	
//			NEW V06: packet start is always sent
//			if( my->packetStart[i] >= 0 )
//			{
				*tmp++ = my->packetStart[i] & 0xff;
				nlen++;
//			}
//			else
//				break;
		}

		// data			
		for( i = 0; i < len ; i++ )
		{
			*tmp = *( pj++ ) & 0xff;
			// special character
			if( *tmp++ == my->packetChar & 0xff)
			{

//				NEW V06: removed support for supressed special character				
//				if( my->packetChar >= 0x00 )
//				{
					*tmp++ = my->packetChar & 0xff;
					nlen++;
//				}
			}
			nlen++;
		}

		// packet end
		// NEW V06 -> only skip (and completely skip) if <ETX> is <0
		if( my->packetEnd[1] >= 0 )
		{
			for( i = 0; i < 2; i++ )
			{
//				NEW V06: removed support for sending "half" packet markers.
//				if( my->packetEnd[i] >= 0 )
//				{
					*tmp++ = my->packetEnd[i] & 0xff;
					nlen++;
//				}				
//				else
//					break;
			}// END for
		}

		tmp = org;
	
		OK = SendRaw(( int*)org, nlen );
	
		delete[] tmp;
	}

	return OK;
		
}


//************************************************************************************************
//*** asSerial::BufferCountPackets()
//***
//***
//************************************************************************************************
int asSerial::BufferCountPackets()
{
	int i;
	int nPackets = 0;
	int state = 0;

	my->RXBufSnifReset();

	for(;;)
	{
		// check for overflow hack
		if( my->RXBufCount() < 0 )
			return -1;
		
		// "sniff" a byte (read it but do not remove it from the receive buffer)
		i = my->RXBufSnifByte();
		if( i < 0 )
			return nPackets;

		switch( state )
		{
			// ---------------------------------
			// waiting for packet start 1
			case 0:
				if( i == my->packetStart[0] )
					state++;
				break;
				
			// ---------------------------------
			// waiting for packet start 2
			case 1:
				if( i == my->packetStart[1] )
					state++;
				else
					state--;
				break;
			// ---------------------------------
			// waiting for any DLE
			case 2:
				if( i == my->packetChar )
					state++;
				break;
			// ---------------------------------
			// waiting for character, packet end 2 or packet start
			case 3:
				if( i == my->packetEnd[1] )
				{
					nPackets++;
					state = 0;
				}
				else
				{
					// received 2nd DLE as character -> OK
					if( i == my->packetChar )
					{
						state--; break;
					}

					// received a packet start:
					//   OK    -> if packet end is turned off
					//   ERROR -> in all other cases, but treat as 2nd packet
					// TODO: Maybe a better strategy would be discarding that packet?
					if( i == my->packetStart[1] )
					{
						nPackets++;
						state = 1;
					}
					// any other character -> no chance to reconstruct this...
					else
						state = 0;
						
				}// END else
			
				break;
				
		}// END switch
	}// END for
}


//************************************************************************************************
//*** asSerial::RecvRawPacketByte
//***
//***
//*** *data -> pointer to write data to
//***   len -> size of buffer (max writes)
//************************************************************************************************
int asSerial::RecvRawPacketByte(int *data, int len )
{
	int i;
	int state = 0;
	int lenRead = 0;

	if( len < 1 )
		return 0;

	if( BufferCountPackets() < 1 )
		return 0;

	for(;;)
	{
		i = my->RXBufReadByte( NONBLOCKING );
		if( i < 0 )
			return lenRead;

		if( lenRead > len )
			return lenRead;


		switch( state )
		{
			// ---------------------------------
			// waiting for packet start 1
			case 0:
				if( i == my->packetStart[0] )
					state++;
				break;
				
			// ---------------------------------
			// waiting for packet start 2
			case 1:
				if( i == my->packetStart[1] )
					state++;
				else
					state--;
				break;
			// ---------------------------------
			// waiting for any character or DLE
			case 2:
				if( i == my->packetChar )
					state++;
				else
				{
					*data++ = i & 0xff;
					lenRead++;
				}
				break;
			// ---------------------------------
			// waiting for character, packet end 2 or packet start
			case 3:
				if( i == my->packetEnd[1] )
					return lenRead; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
				else
				{
					// received 2nd DLE as character -> OK
					if( i == my->packetChar )
					{
						*data++ = i & 0xff;
						lenRead++;
						state--;
						break;
					}

					return lenRead; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
						
				}// END else
			
				break;
				
		}// END switch
	}// END for

}

