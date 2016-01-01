#ifdef __BORLANDC__
	#include <dos.h>
	#include <stdlib.h>
	#include <iostream.h>
	#include <stdio.h>
	#include <conio.h>
#else
	#include <sys/io.h>
	#include <unistd.h>
	#include <iostream>

	#include <thread>
	#include <chrono>
#endif


#include <math.h>

#ifndef __BORLANDC__
	using namespace std;
#endif

int port_timer2a = 0x43;
int port_timer2b = 0x42;
int port_speaker = 0x61;

const int FirstOctaveC = 262;
const double ST = 1.0594630943593;	// pow(2, 1/12)
const int PeriodBase = 3520;
const int Periods[7] = {	PeriodBase, PeriodBase/2,
							PeriodBase/4, PeriodBase/8,
							PeriodBase/16, PeriodBase/32,
							PeriodBase/64};

int NotesInOctaves[10][12];
void GenerateNotes(){
	for(int o=0; o<10; o++){
		for(int n=0; n<12; n++){
			NotesInOctaves[o][n] = FirstOctaveC*pow(2, o-3) * pow(ST, n);
		}
	}
}


int TakePermissions() {
	#ifndef __BORLANDC__
	if(ioperm(port_timer2b, 0x02, 1))
		return 0;
	if(ioperm(port_speaker, 0x20, 1))
		return 0;
	#else
	cout << "Fuck permissions, we are in a DOS country" << endl;
	#endif
	return 1;
}
inline void writeByteToPort(unsigned char value, unsigned char port){
	#ifdef __BORLANDC__
		outportb(port, value);
	#else
		outb(value, port);
	#endif
}
inline unsigned char readByteFromPort(unsigned char port){
	#ifdef __BORLANDC__
		return inportb(port);
	#else
		return inb(port);
	#endif
}
#ifndef __BORLANDC__
void delay(int ms){
	this_thread::sleep_for(std::chrono::milliseconds(ms));
}
#endif

void GetHLBytes(int inumber, unsigned char* low, unsigned char* high){
	low[0] = 	(inumber & 0xff);
	high[0] = 	(inumber >> 8);
}

inline void OpenSpeaker(){
	writeByteToPort(readByteFromPort(0x61)|0x3, 0x61);
}
inline void CloseSpeaker(){
	writeByteToPort(readByteFromPort(0x61)&0xfc, 0x61);
}

inline void SetFreq(int freq){
	int countdown = 1193180 / freq;
	unsigned char *low = new unsigned char[1];
	unsigned char *high = new unsigned char[1];
	GetHLBytes(countdown, low, high);
	writeByteToPort(0xb6, port_timer2a);
	writeByteToPort(*low, port_timer2b);
	writeByteToPort(*high, port_timer2b);
}

inline void Beep(int ms){
	OpenSpeaker();
	delay(ms);
	CloseSpeaker();
}
inline void Bee(int ms){
	OpenSpeaker();
	delay(ms);
}

void PlayNote(int octave, int note, int period){
	if (note<0){
		delay(Periods[period]);
		return;
	}
	SetFreq(NotesInOctaves[octave][note]);
	Beep(Periods[period]/8*7);
	delay(Periods[period] - Periods[period]/8*7);
}

void PlayMusic(int* octaves, int* notes, int* periods, int length){
	for(int i=0; i<length; i++){
		cout << octaves[i] << " " << notes[i] << " " << periods[i] << endl;
		PlayNote(octaves[i], notes[i], periods[i]);
	}
}

void PlayOctave(int octave, int period){
	for(int n=0; n<12; n++){
		PlayNote(octave, n, period);
	}
}

const int Mixdelay = 40;
int ChordI;
int ChordJ;
void PlayChord(int* octaves, int* notes, int period, int notesnumber)
{
	ChordI = 0;
//	int playp = Periods[period]* 7 / 8 - Mixdelay/notesnumber;
	OpenSpeaker();
	while (ChordI < Periods[period] *7 /8 - Mixdelay)
	{
		for (ChordJ = 0; ChordJ < notesnumber; ChordJ++)
		{
			SetFreq(NotesInOctaves[octaves[ChordJ]][notes[ChordJ]]);
			delay(Mixdelay/notesnumber);
			ChordI+=Mixdelay/notesnumber;
		}
	}
	CloseSpeaker();
	delay(Periods[period] - ChordI);
}

void PlayMusicChords(int** octaves, int** notes, int* periods, int* notesnumber, int length){
	for(int i=0; i<length; i++){
		cout 	<< octaves[i][0] << " " << notes[i][0]<< " "
				<< octaves[i][1] << " " << notes[i][1]<< " "
				<< octaves[i][2] << " " << notes[i][2]<< " "
				<< octaves[i][3] << " " << notes[i][3]<< " " 
				<< periods[i] << " chp"<< notesnumber[i] << endl;
	//	if (notesnumber[i]<2)
	//		PlayNote(octaves[i][0], notes[i][0], periods[i]);
	//	else
			PlayChord(octaves[i], notes[i], periods[i], notesnumber[i]);
	}
}


//const int NotesNumber = 4;
int main(int argc, char *argv[]){
	if(argc < 2){
		cout<< "USAGE:\npcsplayer <music file name>\n";
		return 1;}
	if(!TakePermissions()){
		cout<<"Couldn't take permissions for ports direct usage.\n"
			<<"Maybe should run as root or use DOS?\n";
		return 1;}

	GenerateNotes();
	cout << "Here came our note frequencies" << endl;

	//int octaves[3] = {3,3,3};
	//int notes[3] = {0,4,7};
	//int period = 2;

	//PlayChord(octaves, notes, period, 3);
	FILE* mFile;
	mFile = fopen(argv[1], "r");
	int lines;
	fscanf(mFile, "%d", &lines);

	int** octaves = new int*[lines];
	int** notes = new int*[lines];
	for (int l=0; l<lines; l++)
	{
		octaves[l] = new int[4];
		notes[l] = new int[4];
	}
	int* periods = new int[lines];
	int* chordsizes = new int[lines];

	cout << "File opened! " << lines << " notes!" << endl;
	for (int i=0; i < lines; i++){
		    fscanf(mFile, "%d %d %d %d %d %d %d %d %d", &octaves[i][0], &notes[i][0], &octaves[i][1], &notes[i][1], &octaves[i][2], &notes[i][2], &octaves[i][3], &notes[i][3], &periods[i]);
		    chordsizes[i] = 0;
		    for (int j = 0; j<4;j++)
			if (notes[i][j] >= 0)
				chordsizes[i]++;
	}
	cout << "File read! Press any key to play" << endl;
//	getchar();

	PlayMusicChords(octaves, notes, periods, chordsizes, lines);

//	getchar();
	return 0;

	
}
