#include "ProcessorReader.h"
#include <fstream>

/////////////////////////////

/*

Class of binary file reader. This reader is specialized in reading INTEL processor encoded binary files.

*/
class INTELreader : public ProcessorReader{

public:

	
	INTELreader(char* mem, unsigned int maxPos);
	~INTELreader();

	char readNextByte();

	float readNextFloat();

	short readNextWord();

	void seek(int offset, bool direction); // true - forward, false - backward 
	void seek(int offset);  // absolute position

	unsigned int getCurPos();
	

};