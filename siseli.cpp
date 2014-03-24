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


// TODO: replace by STL
// I know, I know. Shut up... ;)
static asSerial *cPort[ MAXPORTS ];



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void init(int *OK)
{
	int i;
	for(i=0;i<MAXPORTS;i++)
		cPort[i]=NULL;
	*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void version(int *version)
{
	*version = SISELI_VER;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void mount (int *handle, int *OK)
{
	*OK=0;
	
	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	
	if(cPort[(*handle)-1]==NULL)
	{
		class asSerial *pas = new asSerial;
		cPort[(*handle)-1]=pas;
		if(pas==NULL)
			return;
	}
	
	*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void umount (int *handle, int *OK)
{
	*OK=0;
	
	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	
	if(cPort[(*handle)-1]!=NULL)
	{
		delete cPort[(*handle)-1];
		cPort[(*handle)-1]=NULL;
	}
		
	*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void check (int *handle, int *port, int *OK)
{
	*OK=0;

	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;

	*OK=cPort[(*handle)-1]->Check(*port);
}





//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void config(int *handle, int *baud, int *bits, int *parity, int *stop, int *OK)
{
	*OK = 0;

	if( ( *handle < 1 ) || ( *handle > MAXPORTS ) )
		return;
		
	if( cPort[(*handle)-1] == NULL )
		return;

	if( cPort[(*handle)-1]->ConfigComm(*baud, *bits, *parity, *stop) == ERROR_SUCCESS )
		*OK = 1;
	
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void open(int *handle, int *port, int *OK)
{
	*OK = 0;

	if( ( *handle < 1 ) || ( *handle > MAXPORTS ) )
		return;
	if( cPort[(*handle)-1] == NULL )
		return;

	if( cPort[(*handle)-1]->Check(*port) == 0 )
		return;
	
	if( cPort[(*handle)-1]->Open(*port) == ERROR_SUCCESS )
		*OK = 1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void close (int *handle, int *OK)
{
	*OK=0;

	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;

	if(cPort[(*handle)-1]->Close()==ERROR_SUCCESS)
		*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void sendb (int *handle, int *byte, int *OK)
{
	*OK=0;

	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;

	if(cPort[(*handle)-1]->SendRawByte(*byte) == ERROR_SUCCESS	)
		*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void senda (int *handle, int *bytes, int *length, int *OK)
{
	int i;

	*OK=0;

	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;

	unsigned char *tmp = new unsigned char[(*length)+8];
	unsigned char *org=tmp;
	int *pj=bytes;
	
	if(tmp!=NULL)
	{
		for(i=0;i<*length;i++)
			*(tmp++)=*(pj++)&0xff;
	
		tmp=org;
	
		if(cPort[(*handle)-1]->SendRaw((int*)org,*length) == ERROR_SUCCESS	)
			*OK=1;
	
		delete[] tmp;
	}
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void count (int *handle, int *count)
{
	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;
	
	*count=cPort[(*handle)-1]->BufferCount();
}




//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void recvb (int *handle, int *block, int *byte)
{
	*byte=-1;
	
	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;
	if(*block!=BLOCKING)
		*block=NONBLOCKING;

	*byte=cPort[(*handle)-1]->RecvRawByte(*block);
}


//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void flush  (int *handle, int *OK)
{
	*OK=0;
	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;
	cPort[(*handle)-1]->BufferFlush();
	*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void recva (int *handle, int *bytes)
{
	int i,j=0;
	int *tmp;
	
	if( (*handle < 1) || ( *handle > MAXPORTS ) )
		return;
	if( cPort[ (*handle) - 1 ] == NULL )
		return;

	tmp = bytes++;

	for( i=0; i < 32; i++ )
	{
		if( ( *bytes++ = cPort[(*handle)-1]->RecvRawByte(0) ) >= 0 )
			j++;
	}
	*tmp=j;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void recvan (int *handle, int *length, int *bytes)
{
	int i,j=0;
	int *tmp;
	
	if( (*handle<1) || (*handle>MAXPORTS) )
		return;
	if( cPort[(*handle)-1] == NULL )
		return;
	if( *length < 1 )
		return;
  
	tmp = bytes++;

	for( i=0; i<(*length); i++ )
	{
		if( (*bytes++ = cPort[ (*handle)-1 ] -> RecvRawByte(0) ) >= 0 )
			j++;
	}
	*tmp = j;
}



//************************************************************************************************
//*** 
//***
//***
//************************************************************************************************
DLLIMPORT void recvap (int *handle, int *length, int *bytes)
{
	int i;
	int *tmp;
	
	if( (*handle<1) || (*handle>MAXPORTS) )
		return;
	if( cPort[(*handle)-1] == NULL )
		return;
	if( *length < 1 )
		return;
  
	tmp = bytes++;

	memset( (int *)bytes, 0xff, (*length) * sizeof(int) );

	*tmp = cPort[(*handle)-1]->RecvRawPacketByte( bytes, *length );
}



//************************************************************************************************
//*** countp
//***
//*** Count number of complete packets in buffer.
//************************************************************************************************
DLLIMPORT void countp (int *handle, int *count)
{
	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;
	
	*count = cPort[(*handle)-1]->BufferCountPackets();
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void sendp (int *handle, int *bytes, int *length, int *OK)
{
	int i;

	*OK=0;

	if( (*handle<1)||(*handle>MAXPORTS))
		return;
	if(cPort[(*handle)-1]==NULL)
		return;

	if(cPort[(*handle)-1]->SendPacket(bytes,*length) == ERROR_SUCCESS	)
		*OK=1;

}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void packs (int *handle, int *packstart, int *OK)
{
	int i;

	*OK=0;

	if( ( *handle < 1 ) || ( *handle > MAXPORTS ) )
		return;
		
	if( cPort[ (*handle) -1 ] == NULL )
		return;

	if( cPort[ (*handle) - 1 ]->ConfigPacketStart( packstart ) == ERROR_SUCCESS	)
		*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void packe (int *handle, int *packend, int *OK)
{
	int i;

	*OK=0;

	if( ( *handle < 1 ) || ( *handle > MAXPORTS ) )
		return;
		
	if( cPort[ (*handle) - 1 ] == NULL )
		return;

	if( cPort[ (*handle) - 1 ]->ConfigPacketEnd( packend ) == ERROR_SUCCESS	)
		*OK=1;
}



//************************************************************************************************
//***
//***
//***
//************************************************************************************************
DLLIMPORT void packc (int *handle, int *packchar, int *OK)
{
	*OK=0;

	if( ( *handle < 1 ) || ( *handle > MAXPORTS ) )
		return;
		
	if( cPort[ (*handle) - 1 ] == NULL )
		return;

	if( cPort[ (*handle) - 1 ]->ConfigPacketChar( *packchar ) == ERROR_SUCCESS	)
		*OK=1;
}



//************************************************************************************************
//***
//***
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


