#ifdef WIN32
	#include <conio.h>
#else
	#include <sys/io.h>
	#include <unistd.h>
#endif

#include <cstdio>
#include <iostream>
//
#include <thread>
#include <chrono>

using namespace std;

int port_timer2a = 0x43;
int port_timer2b = 0x42;
int port_speaker = 0x61;


bool TakePermissions() {
	#ifndef WIN32
	if(ioperm(port_timer2b, 0x02, 1))
		return false;
	if(ioperm(port_speaker, 0x20, 1))
		return false;
	#endif
	return true;
	}

void GetHLBytes(int inumber, unsigned char* low, unsigned char* high){
	low[0] = 	(inumber & 0xff);	
	high[0] = 	(inumber >> 8);
}

int main(int argc, char *argv[]){
//	cout << argv[1];
//	cout << "\n";
	cout 	<< "Permissions taking: "<< TakePermissions() << "\n";

	cout<<"\n\n"<<(short int)inb(0x61)<<"\n";
	outb(inb(0x61)|0x3, 0x61);
	cout<<(short int)inb(0x61)<<'\n';
	this_thread::sleep_for(std::chrono::seconds(1));
	outb(inb(0x61)&0xfc, 0x61);
	cout<<(short int)inb(0x61)<<'\n';

//	cout << *((int*)low) << "\n";
//	cout << *((int*)high) << "\n";
	
}
