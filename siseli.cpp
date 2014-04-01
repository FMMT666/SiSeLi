//
// siseli; ScilabSerialLibrary
// FMMT666(ASkr), 2010, 2012, 2013, 2014; www askrprojects.net
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


#include "siseli.h"
#include <windows.h>

#include "asSerial.h"

#include <map>


using namespace std;


// A Container for the <asSerial> classes.
// Numbering starts at 1 (equal to the Scilab handles)
static map<int,asSerial*> mPort;



//************************************************************************************************
//*** init()
//*** Called by Scilab's slInit()
//***
//************************************************************************************************
DLLIMPORT void init( int *OK )
{
	*OK = 1;
}



//************************************************************************************************
//*** version()
//*** Called by Scilab's slVersion()
//***
//************************************************************************************************
DLLIMPORT void version( int *version )
{
	*version = SISELI_VER;
}



//************************************************************************************************
//*** mount()
//*** Called by Scilab's slMount()
//***
//************************************************************************************************
DLLIMPORT void mount ( int *handle, int *OK )
{
	*OK = 0;
	
	// In the past, the <handle>, given by the Scilab call, created exactly this handle and
	// returned 1 to <OK> if everything went fine (0 otherwise).
	// Now, we return either 0, on failure or the number of the handle created or
	// any number >0, representing the handle itself.
	// Notice that there is _no_ (practical) limit to the amount of handles serial interface
	// classes ano more. The code limit is set to 32000 8-)

	// New behaviour, 4/2014: find next free number (the hard way, for now ;-) and
	// return it as the new handle.
	// For compatibility, we first check if the requested handle number (if any)
	// is available...
	
	if(  ( *handle != 0 ) && ( mPort.find( *handle ) == mPort.end() )  )
	{
		// compatibility mode; check if requested handle is available
		mPort.emplace( *handle, new asSerial() );
		*OK = *handle;
	}
	else
	{
		// find first, free handle number >0
		// I guess, 32000 interfaces are sufficient ;-)
		for( int i=1; i<32000; ++i )
		{
			if( mPort.find( i ) == mPort.end() )
			{
				mPort.emplace( i, new asSerial() );
				*OK = i;
				break;
			}
		}// END for all possible handles
	}//END else

}// END DLLIMPORT void mount




//************************************************************************************************
//*** umount()
//*** Called by Scilab's slUMount()
//***
//************************************************************************************************
DLLIMPORT void umount ( int *handle, int *OK )
{
	*OK = 0;
	
	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		delete el->second;       // destroy the asSerial object
		mPort.erase( *handle );  // delete the container entry
		*OK = 1;
	}
	
}



//************************************************************************************************
//*** check()
//*** Called by Scilab's slCheck()
//***
//************************************************************************************************
DLLIMPORT void check ( int *handle, int *port, int *OK )
{
	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
		*OK = el->second->Check( *port );
	
}



//************************************************************************************************
//*** config()
//*** Called by Scilab's slConfig()
//***
//************************************************************************************************
DLLIMPORT void config( int *handle, int *baud, int *bits, int *parity, int *stop, int *OK )
{
	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->ConfigComm( *baud, *bits, *parity, *stop ) == ERROR_SUCCESS )
			*OK = 1;
	}
	
}



//************************************************************************************************
//*** open()
//*** Called by Scilab's slOpen()
//***
//************************************************************************************************
DLLIMPORT void open( int *handle, int *port, int *OK )
{
	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->Check( *port ) == 0 )
			return;
			
		if( el->second->Open( *port ) == ERROR_SUCCESS )
			*OK = 1;
	}

}



//************************************************************************************************
//*** close()
//*** Called by Scilab's slClose()
//***
//************************************************************************************************
DLLIMPORT void close ( int *handle, int *OK )
{
	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->Close() == ERROR_SUCCESS )
			*OK = 1;
	}
	
}



//************************************************************************************************
//*** sendb()
//*** Called by Scilab's slSendByte()
//***
//************************************************************************************************
DLLIMPORT void sendb ( int *handle, int *byte, int *OK )
{
	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->SendRawByte(*byte) == ERROR_SUCCESS	)
			*OK = 1;
	}

}



//************************************************************************************************
//*** senda()
//*** Called by Scilab's slSendArray()
//***
//************************************************************************************************
DLLIMPORT void senda ( int *handle, int *bytes, int *length, int *OK )
{
	int i;

	*OK = 0;

	//*********************
	// TODO: NEEDS CLEANUP
	//*********************

	unsigned char *tmp = new unsigned char[(*length)+8];
	unsigned char *org = tmp;
	int *pj = bytes;
	
	if( tmp != NULL )
	{
		for( i = 0; i < *length; i++ )
			*(tmp++) = *(pj++) & 0xff;
	
		tmp = org;

		auto el = mPort.find( *handle );
		if( el != mPort.end() )
		{
			if( el->second->SendRaw( (int*)org, *length ) == ERROR_SUCCESS )
				*OK = 1;
		}
	
		delete[] tmp;
	}

}



//************************************************************************************************
//*** count
//*** Called by Scilab's slCount()
//***
//************************************************************************************************
DLLIMPORT void count ( int *handle, int *count )
{
	// default is an overflow/error
	*count = -1;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
		*count = el->second->BufferCount();
	
}




//************************************************************************************************
//*** recvb()
//*** Called by Scilab's slReadByte()
//***
//************************************************************************************************
DLLIMPORT void recvb (int *handle, int *block, int *byte )
{
	*byte = -1;
	
	if( *block != BLOCKING )
		*block = NONBLOCKING;
	
	auto el = mPort.find( *handle );
	if( el != mPort.end() )
		*byte = el->second->RecvRawByte( *block );
	
	
}


//************************************************************************************************
//*** flush
//*** Called by Scilab's slFlush()
//***
//************************************************************************************************
DLLIMPORT void flush (int *handle, int *OK)
{
	*OK = 0;
	
	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		el->second->BufferFlush();
		*OK = 1;
	}
	
}



//************************************************************************************************
//*** recva()
//*** Called by Scilab's slReadArray()
//***
//************************************************************************************************
DLLIMPORT void recva ( int *handle, int *bytes )
{
	int i, j = 0;
	int *tmp;

	//*********************
	// TODO: NEEDS CLEANUP
	//*********************
	
	// first byte contains the length of the answer ( "0" aka "nothing" is the default)
	*bytes = 0;
	
	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		tmp = bytes++;
	
		for( i=0; i < 32; i++ )
		{
			if( ( *bytes++ = el->second->RecvRawByte(0) ) >= 0 )
				j++;
		}
		*tmp = j;
	
	}
	
}



//************************************************************************************************
//*** recvan()
//*** Called by Scilab's slReadArrayN()
//***
//************************************************************************************************
DLLIMPORT void recvan ( int *handle, int *length, int *bytes )
{
	int i, j = 0;
	int *pFirstByte;

	//*********************
	// TODO: NEEDS CLEANUP
	//*********************

	// first byte contains the length of the answer ( "0" aka "nothing" is the default)
	*bytes = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		// remember the address of the first byte (will later contain # of bytes read)
		pFirstByte = bytes++;
	
		for( i = 0; i < (*length); ++i )
		{
			if( (*bytes++ = el->second->RecvRawByte(0) ) >= 0 )
				j++;
		}
		
		// store the number of received bytes to the first entry of the array
		*pFirstByte = j;
	}
  
}



//************************************************************************************************
//*** recvap()
//*** Called by Scilab's slReadPacket()
//***
//************************************************************************************************
DLLIMPORT void recvap ( int *handle, int *length, int *bytes )
{
	int i;
	int *pFirstByte;
		
	//*********************
	// TODO: NEEDS CLEANUP
	//*********************

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{

		// The first address (pFirstByte) will contain the number of bytes received.
		pFirstByte = bytes++;

		// holy mackerel...	
		memset( (int *)bytes, 0xff, (*length) * sizeof(int) );
	
		// Results are written to <bytes>
		*pFirstByte = el->second->RecvRawPacketByte( bytes, *length );
	}	
	
}



//************************************************************************************************
//*** countp()
//*** Called by Scilab's slCount()
//***
//*** Count number of complete packets in buffer.
//************************************************************************************************
DLLIMPORT void countp ( int *handle, int *count )
{
	*count = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
		*count = el->second->BufferCountPackets();
		
}



//************************************************************************************************
//*** sendp()
//*** Called by Scilab's slSendPacket()
//***
//************************************************************************************************
DLLIMPORT void sendp ( int *handle, int *bytes, int *length, int *OK )
{
	int i;

	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->SendPacket( bytes, *length ) == ERROR_SUCCESS )
			*OK = 1;
	}

}



//************************************************************************************************
//*** packs()
//*** Called by Scilab's slSetPacketStart()
//***
//************************************************************************************************
DLLIMPORT void packs ( int *handle, int *packstart, int *OK )
{
	int i;

	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->ConfigPacketStart( packstart ) == ERROR_SUCCESS	)
			*OK = 1;
	}


}



//************************************************************************************************
//*** packe()
//*** Called by Scilab's slSetPacketEnd()
//***
//************************************************************************************************
DLLIMPORT void packe (int *handle, int *packend, int *OK )
{
	int i;

	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->ConfigPacketEnd( packend ) == ERROR_SUCCESS	)
			*OK = 1;
	}

}



//************************************************************************************************
//*** packc()
//*** Called by Scilab's slSetPacketChar()
//***
//************************************************************************************************
DLLIMPORT void packc ( int *handle, int *packchar, int *OK )
{
	*OK = 0;

	auto el = mPort.find( *handle );
	if( el != mPort.end() )
	{
		if( el->second->ConfigPacketChar( *packchar ) == ERROR_SUCCESS	)
			*OK = 1;
	}

}



//************************************************************************************************
//*** DllMain
//*** Called by nothing
//***
//************************************************************************************************
BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	// Scilab never enters DllMain...
	
	switch (reason)
	{
    case DLL_PROCESS_ATTACH:
      break;

    case DLL_PROCESS_DETACH:
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;
	}

	return TRUE;
}


