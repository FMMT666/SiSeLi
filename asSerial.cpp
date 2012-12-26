//
// asSerial, a CSerial wrapper object (for Scilab)
// ASkr, 2010, 2012; www askrprojects.net
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

		// (limited) packet mode
		// I once had the idea to have more than two bytes for packet start and end...
		// I skipped this, but the relics are still in here.
		// Maybe, in the future...
		int				packetMode;						// 1 enables packet mode (only valid for receiving data)
		int				packetStart[3];				// packet start identifier, 0-terminnated (e.g.: 0x10,0x02,0x00)
		int				packetEnd[3];					// packet end identifier, 0-terminnated (e.g.: 0x10,0x03,0x00)
		int				packetChar;						// packet "special character" (e.g.: 0x10)

		// buffer stuff
		int				RXBufWriteByte(int ch);
		int				RXBufReadByte(int blocked);
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
	packetStart[0]=	0x10;
	packetStart[1]=	0x02;
	packetStart[2]=	0x00;
	packetEnd[0]=		0x10;
	packetEnd[1]=		0x03;
	packetEnd[2]=		0x00;
	packetChar=			0x10;

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
	return RXBuf[RXBufR] & 0xff;
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
	
	for(i=0;i<2;i++)
		my->packetStart[i]=(*start++) & 0xff;
	// just in case...
	my->packetStart[2]=0;

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
	
	for(i=0;i<2;i++)
		my->packetEnd[i]=(*end++) & 0xff;
	// just in case...
	my->packetEnd[2]=0;

	return ERROR_SUCCESS;

}




//************************************************************************************************
//*** asSerial::ConfigPacketChar()
//***
//***
//************************************************************************************************
int asSerial::ConfigPacketChar(int ch)
{
	my->packetChar=ch & 0xff;
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
	int OK=ERROROR;
	int nlen=0;
	
	unsigned char *tmp = new unsigned char[len*2+12]; // worst case
	unsigned char *org=tmp;
	int *pj=data;
	
	if(tmp!=NULL)
	{
		// packet start
		for(i=0;i<2;i++)
		{
			if(my->packetStart[i]!=0)
			{
				*tmp++=my->packetStart[i] & 0xff;
				nlen++;
			}
			else
				break;
		}

		// data			
		for(i=0;i<len;i++)
		{
			*tmp=*(pj++) & 0xff;
			// special character
			if(*tmp++ == my->packetChar & 0xff)
			{
				// skip "0" DLE character (turned off)
				// TOTHINK:
				// Maybe, "-1" would be a better "skip" choice...
				if(my->packetChar > 0x00)
				{
					*tmp++=my->packetChar & 0xff;
					nlen++;
				}
			}
			nlen++;
		}

		// packet end
		for(i=0;i<2;i++)
		{
			if(my->packetEnd[i]!=0)
			{
				*tmp++=my->packetEnd[i] & 0xff;
				nlen++;
			}				
			else
				break;
		}

		tmp=org;
	
		OK=SendRaw((int*)org,nlen);
	
		delete[] tmp;
	}

	return OK;
		
}

