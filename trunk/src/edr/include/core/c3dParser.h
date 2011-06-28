#ifndef _C3D_PARSER_H_
#define _C3D_PARSER_H_

#include <core/Header.h>
#include <core/Parameters.h>
#include <core/Data.h>
#include <core/C3D_Data.h>

#include <string>
#include <fstream>
#include <iostream>

///////////////////////////////////////////

class ProcessorReader;

/*

Main class of the C3D Parser class. Parser input is a string type path to the file. 
The result of parsing is C3D Data object pointer holding all read data from the path file.

*/
class c3dParser {

private:

	MarkerHeader* header;
	Parameters* parameters;
	Data* data;
	ProcessorReader* procRead;
	char* memContent;

public:

	c3dParser();

///////////////////////////////////////////

	C3D_Data* parseData(std::string path);
	bool saveData(std::string path, C3D_Data* data);

	MarkerHeader* getHeader() { return header; }
	Parameters* getParameters() { return parameters; }
	Data* getData() { return data; }
	ProcessorReader* getProcessorReader() { return procRead; }

///////////////////////////////////////////

	~c3dParser();

};
#endif
