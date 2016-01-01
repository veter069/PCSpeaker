#ifdef WIN32
	#include <conio.h>
#else
	#include <sys/io.h>
	#include <unistd.h>
#endif
//
#include <cstdio>
#include <iostream>

#include <thread>
#include <chrono>

using namespace std;

int port_timer2a = 0x43;
int port_timer2b = 0x42;
int port_speaker = 0x61;

const int  OctaveBases[8] = {125, 250, 500, 1000, 2000, 4000, 8000, 16000};
const int UsedOctave = 2;
const double ST = 1.0594630943593;	// pow(2, 1/12)
const int Octaves[13]  = {
			OctaveBases[UsedOctave], 
			(int)(OctaveBases[UsedOctave]*ST),
			(int)(OctaveBases[UsedOctave]*(ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST*ST*ST*ST*ST)),
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST*ST*ST*ST*ST*ST)), // BECAUSE FUCK YOU THAT'S WHY!!!!11onewonewoneo/
			(int)(OctaveBases[UsedOctave]*(ST*ST*ST*ST*ST*ST*ST*ST*ST*ST*ST*ST))
		};
const int PeriodBase = 500;
const int Periods[7] = {	PeriodBase, PeriodBase/2, 
			PeriodBase/4, PeriodBase/8, 
			PeriodBase/16, PeriodBase/32,
			PeriodBase/64};

bool TakePermissions() {
	#ifndef WIN32
	if(ioperm(port_timer2b, 0x02, 1))
		return false;
	if(ioperm(port_speaker, 0x20, 1))
		return false;
	#endif
	return true;
}
void writeByteToPort(unsigned char value, unsigned char port){
	#ifdef WIN32
		_outp(port, value);
	#else
		outb(value, port);
	#endif
}
unsigned char readByteFromPort(unsigned char port){
	#ifdef WIN32
		return _inp(port);
	#else 
		return inb(port);
	#endif
}

void GetHLBytes(int inumber, unsigned char* low, unsigned char* high){
	low[0] = 	(inumber & 0xff);	
	high[0] = 	(inumber >> 8);
}

void OpenSpeaker(){
	writeByteToPort(readByteFromPort(0x61)|0x3, 0x61);
}
void CloseSpeaker(){
	writeByteToPort(readByteFromPort(0x61)&0xfc, 0x61);
}

void SetFreq(int freq){
	int countdown = 1193180 / freq;
	unsigned char *low = new unsigned char[1];
	unsigned char *high = new unsigned char[1];
	GetHLBytes(countdown, low, high);
	writeByteToPort(0xb6, port_timer2a);
	writeByteToPort(*low, port_timer2b);
	writeByteToPort(*high, port_timer2b);
}

void Beep(int ms){
	OpenSpeaker();
	this_thread::sleep_for(std::chrono::milliseconds(ms));
	CloseSpeaker();
}
void Bee(int ms){
	OpenSpeaker();
	this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void PlayNote(int freq, int ms){
	SetFreq(freq);
	Beep(ms);
}

void PlayMusic(int *freqs, int *mss, int length){
	for(int i=0; i<length; i++){
		PlayNote(freqs[i], mss[i]);
	}
} 

void PlayOctave(int octave, int period){
	for(int i=0; i<13; i++){
		cout<<Octaves[i]<<"\n";
		PlayNote(Octaves[i], Periods[period]);
	}
}

int main(int argc, char *argv[]){
	cout 	<< "Permissions taking: "<< TakePermissions() << "\n";
	//SimpleTest();
		
	for(int i=0; i<13; i++){
		cout<<Octaves[i]<<"\n";
		PlayNote(Octaves[i], Periods[3]);
	}
	/*	
	int freqs[20] = {};
	int mss[20] = {};
	// Mozart's skills uncovering time!

	PlayNote(20, 500);

	
	int count = 50;
	for (int i = 0; i < count; ++i){
		Beep()
	}
*/
}
