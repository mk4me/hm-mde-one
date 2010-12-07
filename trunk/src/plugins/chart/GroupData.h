#include <string>
#include <iostream>
#include <map>
#include "Parameter.h"
#include "ProcessorReader.h"

/*

A vector of parameters oraganized in one group. Each group has unique ID value, name and description.
Its parameters can be placed in c3d parameters section in any order.

*/
class GroupData {

private: 

	std::string name;
	short id;
	std::string description;
	std::vector<Parameter*> parameters;
	std::map<std::string,Parameter*> parametersByName;

public:

	GroupData();
	~GroupData();

	std::string getName();
	short getId();
	std::string getDescription();
	std::vector<Parameter*> getParameters();
	std::map<std::string,Parameter*> getParameterByName();

	void setName(std::string name);
	void setId(short id);
	void setDescription(std::string description);
	void addParameter(std::string name, Parameter* param);

	////////////////////////////////////////

	int saveGroup(std::ofstream& out, ProcessorReader* proc);
	std::string toString();

};