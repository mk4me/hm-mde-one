#ifndef _PARAMETERS_DEF_
#define _PARAMETERS_DEF_

#include <iostream>
#include "ProcessorReader.h"
#include "Header.h"
#include <string>
#include <map>
#include "GroupData.h"

/*

Class responsible for holding data from parameters section.
It contains map o parameters indexed by their name. Each GroupName object contains a vector of parameters inside the group.

*/
class Parameters {

private:

	ProcessorReader* proc;
	short numberOfParamBlocks;
	unsigned char processorType;

	std::map<short,GroupData*> groups;
	std::map<std::string,GroupData*> groupsByName;


public: 

	Parameters(ProcessorReader* proc);

	/////////////////////////////////////

	std::map<short,GroupData*> getGroups();
	std::map<std::string,GroupData*> getGroupsByName();
	void addGroup(short key, std::string name, GroupData* value);

	short getNumberOfParamBlocks();
	void setNumberOfParamBlocks(short number);

	unsigned char getProcessorType();
	void setProcessorType(unsigned char type);

	/////////////////////////////////////

	void saveParameters(std::ofstream& out, MarkerHeader* header);
	bool parseParameters(short startData);
	~Parameters();

	////////////////////////////////////

	std::string toString();

};

#endif //_PARAMETERS_DEF_