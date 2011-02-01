#ifndef _PROCREADER_
#define _PROCREADER_

#include <stdio.h>
#include <fstream>

/*

Class of abstract binary file reader. A super class for more specialized readers.

*/
class ProcessorReader {

protected:

	
	unsigned int maxPos;
	char* memContent;

public:

	unsigned int curPos;

	virtual char readNextByte() = 0;

	virtual float readNextFloat() = 0;

	virtual short readNextWord() = 0;

	virtual void seek(int offset, bool direction) = 0; // true - forward, false - backward 
	virtual void seek(int offset) = 0;  // absolute position


	virtual unsigned int getCurPos() = 0;

	////////////////////////////////////////

	void readStart() {

		curPos = 0;

	}

	void writeStart() {

		curPos = 0;

	}

	void writeByte(char value, std::ofstream& out) {

		out.write( (char*)&value ,1);
		curPos++;

	}

	void writeWord(short value, std::ofstream& out) {

		out.write( (char*)&value ,2);
		curPos += 2;

	}

	void writeFloat(float value, std::ofstream& out) {

		out.write( (char*)&value ,4);
		curPos += 4;

	}

};

#endif //_PROCREADER_


