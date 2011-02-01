#include "ProcessorReader.h"
#include <fstream>
/////////////////////////////

/*

Class of binary file reader. This reader is specialized in reading MIPS processor encoded binary files.
This class is NOT YET READY !!

*/
class MIPSreader : public ProcessorReader {

public:

	MIPSreader(char* mem, unsigned int maxPos);
	~MIPSreader();

	char readNextByte();

	float readNextFloat();

	short readNextWord();

	void seek(int offset, bool direction); // true - forward, false - backward 
	void seek(int offset);  // absolute position

	unsigned int getCurPos();

};