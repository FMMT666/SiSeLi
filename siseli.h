//
// siseli; Scilab Serial Library
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



#define SISELI_VER  07						// decimal notation


// CHANGES:
//
//		V0.2:
//			- increased default RX buffer size to 30kB
//
//		V0.3:
//			- does not exist
//
//		V0.4:
//			- added new Scilab function "slReadArrayN", recvan()
//
//		V0.5:
//			- added baudrates >256000, up to 3MBit/s
//			- removed an error handling bug in some functions (see "ERROROR")
//
//		V0.6:
//			- experimental packet receive algorithm
//			- reworked the packet algorithm ("DLE" standard):
//				- always two byte packet start     [DLE][STA]
//				- always two byte packet end       [DLE][END]
//				- special character is always DLE  [DLE]
//				- packet end can be skippped (first entry in <packetEnd[]> array set to -1
//				- packet start can not be skipped
//
//		V0.6:
//			- removed the "char *cPort[]" array and replaced it with a "map<int,asSerial>" container
//			- introduced new "slMount()" behaviour without the handle argument (but still compatible to the old version)
//


#ifndef _SISELI_H_
#define _SISELI_H_



#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else
# define DLLIMPORT __declspec (dllimport)
#endif


extern "C"
{
	DLLIMPORT void init   (int *OK);
	DLLIMPORT void version(int *version);
	DLLIMPORT void mount  (int *handle, int *OK);
	DLLIMPORT void umount (int *handle, int *OK);
	DLLIMPORT void check  (int *handle, int *port, int *OK);
	DLLIMPORT void config (int *handle, int *baud, int *bits, int *parity, int *stop, int *OK);
	DLLIMPORT void open   (int *handle, int *port, int *OK);
	DLLIMPORT void close  (int *handle, int *OK);
	DLLIMPORT void sendb  (int *handle, int *byte, int *OK);
	DLLIMPORT void senda  (int *handle, int *bytes, int *length, int *OK);
	DLLIMPORT void count  (int *handle, int *count);
	DLLIMPORT void countp (int *handle, int *count);
	DLLIMPORT void recvb  (int *handle, int *block, int *byte);
	DLLIMPORT void flush  (int *handle, int *OK);
	DLLIMPORT void recva  (int *handle, int *bytes);
	DLLIMPORT void recvan (int *handle, int *length, int *bytes);
	DLLIMPORT void recvap (int *handle, int *length, int *bytes);
	DLLIMPORT void packs  (int *handle, int *packstart, int *OK);
	DLLIMPORT void packe  (int *handle, int *packend, int *OK);
	DLLIMPORT void packc  (int *handle, int *packchar, int *OK);
	DLLIMPORT void sendp  (int *handle, int *bytes, int *length, int *OK);
}


#endif

