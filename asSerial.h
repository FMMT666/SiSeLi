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


#define NONBLOCKING 0
#define BLOCKING    1


#ifndef ERROROR
#define ERROROR			-1
#endif


class asSerial
{
	public:
		asSerial();
		~asSerial();

		int ConfigComm(long baud, int bits, int parity, int stop);
		int ConfigPacketStart(int *start);
		int ConfigPacketEnd(int *end);
		int ConfigPacketChar(int ch);

		int Check(int port);											// checks if a port is available
		int Open(int port);												// opens a port; requires a preceeding configuration!
		int Close();															// closes the port
		
		int SendRaw(int *data, int len);					// sends an array to the serial interface
		int SendRawByte(int ch);									// sends a single byte to the serial interface
		int RecvRawByte(int blocked);							// read a single byte from the RX buffer (NONBLOCKING/BLOCKED)
		int RecvRawPacketByte(int *data, int len);// read a single byte from the RX buffer (NONBLOCKING/BLOCKED)
		void BufferFlush();												// empties the content of the RX buffer
		int BufferCount();												// returns the amount of data bytes in the RX buffer; -1 on overflow
		int BufferCountPackets();									// returns the number of packets in the RX buffer
		
		int SendPacket(int *data, int len);				// sends a packet to the serial interface
		int RecvPacket(int *data);								// (not yet) reads a packet from the RX buffer

	private:
		class Secret;
		Secret *my;

};

