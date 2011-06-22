#include <core/GroupData.h>
#include <plugins/chart/ProcessorReader.h>

	GroupData::GroupData() {

		

	}

	GroupData::~GroupData(){

	std::vector<Parameter*>::const_iterator iter;

	for (iter = this->parameters.begin();
	        iter != this->parameters.end();
	        iter++)
	   {
	      delete *iter;
	   }
	 
	   this->parameters.clear();

	}

	std::string GroupData::getName(){

		return this->name;

	}

	short GroupData::getId(){

		return this->id;

	}

	const std::string& GroupData::getDescription() const
    {
		return this->description;
	}

// 	std::vector<Parameter*>& GroupData::getParameters()
//     {
// 		return this->parameters;
// 	}
// 
//     std::map<std::string,Parameter*>& GroupData::getParameterByName()
//     {
// 		return this->parametersByName;
// 	}

    Parameter* GroupData::getParameter( int index )
    {
        return parameters[index];
    }

    Parameter* GroupData::getParameter( const std::string& name )
    {
        return parametersByName[name];
    }

    const Parameter* GroupData::getParameter( int index ) const
    {
        return parameters[index];
    }

    const Parameter* GroupData::getParameter( const std::string& name ) const
    {
        return parametersByName.find(name)->second;
    }

    ////////////////////////////////////////////////////

	void GroupData::setName(std::string name){

		this->name = name;

	}

	void GroupData::setId(short id){

		this->id = id;

	}

	void GroupData::setDescription(std::string description){

		this->description = description;

	}

	void GroupData::addParameter(std::string name, Parameter* param){

		this->parameters.push_back(param);
		this->parametersByName[name] = param;

	}

	int GroupData::saveGroup(std::ofstream& out, ProcessorReader* proc) {

		int res = 0;

		proc->writeByte(name.length(), out); // nchar

		proc->writeByte((char) id, out); // id

		res += 2;

		///////// group name 

	    const char* ptr = 0;
		ptr = name.data();

		for(unsigned int i=0; i < name.length() ; i++) 
		{

			proc->writeByte( ptr[i], out);

		}

		res += name.length();
		////////

		proc->writeWord((short)(description.length() + 3), out); // length of description, description

		proc->writeByte(description.length(), out); // descr length

		res += 3;
	
		///////// group description

		ptr = description.data();

		for(unsigned int i=0; i < description.length(); i++) 
		{

			proc->writeByte( ptr[i], out);

		}

		res += description.length();

		std::vector<Parameter*>::const_iterator iter;

		for (iter = this->parameters.begin();
	        iter != this->parameters.end();
	        iter++)
	   {
		   res += (*iter)->saveParam(out, proc, (char) id);
	   }

		/////////////////////////////

		//std::cout << "GROUP RESULT: " << res << "\n";

		return res;
	}

	std::string GroupData::toString(){

		std::cout << name <<  " " << id << " " << description << "\n PARAMETERS: \n";

		std::vector<Parameter*>::const_iterator iter;

		for (iter = this->parameters.begin();
	        iter != this->parameters.end();
	        iter++)
	   {
		   (*iter)->toString();
		   std::cout << "\n";
		  // std::cin.get();
	   }

	    std::cout << "\n";

		return "";
    }

