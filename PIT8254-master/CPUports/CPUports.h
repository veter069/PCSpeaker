#ifdef __BORLANDC__
    #include <dos.h>
    #include <conio.h>
#else
    #include <sys/io.h>
    #include <unistd.h>
#endif

 class CPUports {

public: // PUBLIC // PUBLIC  // PUBLIC  // PUBLIC  
	
	/// Returns 1 if everything is OK. 0 if something goes wrong.
	static int TakePermission(int portnumber, int portscount) {
	#ifndef __BORLANDC__
		if(ioperm(portnumber, portscount, 1))
			return 0;
	#else
	//	cout << "Fuck permissions, we are in a DOS country!" << endl;
	#endif
		return 1;
	}
	
	// Ohlol, why do i write this?
	static int DropPermission(int portnumber, int portscount){
	#ifndef __BORLANDC__
		if(ioperm(portnumber, portscount, 0))
		return 0;
	#else
	//	cout << "Fuck permissions, we are in a DOS country!" << endl;
	#endif
		return 1;
	}

	inline void WriteByteToPort(unsigned char value, unsigned char port){
	#ifdef __BORLANDC__
		outportb(port, value);
	#else
		outb(value, port);
	#endif
	}
	
	inline unsigned char ReadByteFromPort(unsigned char port){
	#ifdef __BORLANDC__
		return inportb(port);
	#else
		return inb(port);
	#endif
	}
};

