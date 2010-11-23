#include <plugins/chart/Header.h>
#include <plugins/chart/Parameters.h>
#include <plugins/chart/Data.h>
#include <plugins/chart/ProcessorReader.h>
#include <core/C3D_Data.h>

#include <string>
#include <fstream>
#include <iostream>

///////////////////////////////////////////

/*

Main class of the C3D Parser class. Parser input is a string type path to the file. 
The result of parsing is C3D Data object pointer holding all read data from the path file.

*/
class c3dParser {

private:

	Header* header;
	Parameters* parameters;
	Data* data;
	ProcessorReader* procRead;
	char* memContent;

public:

	c3dParser();

///////////////////////////////////////////

	C3D_Data* parseData(std::string path);
	bool saveData(std::string path, C3D_Data* data);

	Header* getHeader() { return header; }
	Parameters* getParameters() { return parameters; }
	Data* getData() { return data; }
	ProcessorReader* getProcessorReader() { return procRead; }

///////////////////////////////////////////

	~c3dParser();

};