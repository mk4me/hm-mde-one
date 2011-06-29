#include <string>
#include <iostream>
#include <map>
#include <core/Parameter.h>

class ProcessorReader;

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

	const std::string& getDescription() const;
    Parameter* getParameter(int index);
    Parameter* getParameter(const std::string& name);
    const Parameter* getParameter(int index) const;
    const Parameter* getParameter(const std::string& name) const;

	void setName(std::string name);
	void setId(short id);
	void setDescription(std::string description);
	void addParameter(std::string name, Parameter* param);

	////////////////////////////////////////

	int saveGroup(std::ofstream& out, ProcessorReader* proc);
	std::string toString();

};