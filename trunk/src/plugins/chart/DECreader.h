#include "ProcessorReader.h"
#include <fstream>

/////////////////////////////

/*

Class of binary file reader. This reader is specialized in reading DEC processor encoded binary files.

*/
class DECreader : public ProcessorReader {

public:

	DECreader(char* mem, unsigned int maxPos);
	~DECreader();

	char readNextByte();

	float readNextFloat();

	short readNextWord();

	void seek(int offset, bool direction); // true - forward, false - backward 
	void seek(int offset);  // absolute position

	unsigned int getCurPos();

};