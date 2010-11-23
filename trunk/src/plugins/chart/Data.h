#ifndef _DATA_DEF_
#define _DATA_DEF_

#include "Header.h"
#include "ProcessorReader.h"

/*

Class responsible for holding data section of c3d file.
It stores data and analog data in float arrays for ease of access.

*/
class Data {

private:

	Header* header;
	ProcessorReader* proc;
	int numberOfFrames;
	int analogChannelsNumber;
	float* data;
	float* analog;

public: 

	Data(Header* head, ProcessorReader* proc);

	/////////////////////////////////////

	int getNumberOfFrames();
	int getAnalogChannelsNumber();

	float* getData();
	float* getAnalog();

	bool parseData();
	void saveData(std::ofstream& out);

	Header* getHeader() { return this->header; }
	~Data();

	///////////////////////////////////

	std::string toString();

};

#endif //_DATA_DEF_