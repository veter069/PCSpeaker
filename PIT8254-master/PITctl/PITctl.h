#include "CPUports.h"

const int port_PITCW = 0x43;
const int port_PIT0 = 0x40;
const int port_PIT1 = 0x41;
const int port_PIT2 = 0x42;
const int port_speakerGate = 0x61;

// Here is a pretty good explanation of how does those counters work:
// https://en.wikipedia.org/wiki/Intel_8253
// Another one is found in OSDev wiki:
// http://wiki.osdev.org/Programmable_Interval_Timer
// A pretty good documentation is also provided by Intel.
class PIT2ctl{
private:
	CPUports ports;	


	void GetHLBytes(int inumber, unsigned char* low, unsigned char* high){
		low[0] = 	(inumber & 0xff);
		high[0] = 	(inumber >> 8);
	}

public:

	PIT2ctl(){
			// Taking 0x42 and 0x43. Second argument is for count of ports after the first.
		bool rez = ports.TakePermission(port_PIT2, 2) 
				&& ports.TakePermission(port_speakerGate, 1); // And 0x61 - the speaker gate.
		if(!rez){
			throw "Couldn't take direct port usage permissions!\r\nMaybe should run as root or use DOS?\r\n";
		}
	}


	inline void SetSquareWaveMode(int freq){
		int countdown = 1193180 / freq;
		unsigned char *low = new unsigned char[1];
		unsigned char *high = new unsigned char[1];
		GetHLBytes(countdown, low, high);
		ports.WriteByteToPort(0xb6, port_PITCW); 	// SC1 SC0 RW1 RW0 M2 M1 M0 BCD
													//	1	0	1	1	0  1  1	 0	
		ports.WriteByteToPort(*low, port_PIT2);
		ports.WriteByteToPort(*high, port_PIT2);
	}

	short int GetCurrentCount(){
		ports.WriteByteToPort(128, port_PITCW);
		return ports.ReadByteFromPort(port_PIT2);
	}

	short int GetStatusByte(){
		return  ports.ReadByteFromPort(port_PIT2);
	}

	bool OutIsHigh(){
		return (ports.ReadByteFromPort(port_PIT2)>>7 == 1 );
	}

	inline void OpenSpeaker(){
		ports.WriteByteToPort(ports.ReadByteFromPort(port_speakerGate)|3, port_speakerGate);
	}
	inline void CloseSpeaker(){
		ports.WriteByteToPort(ports.ReadByteFromPort(port_speakerGate)&0xfc, port_speakerGate);
	}

};

