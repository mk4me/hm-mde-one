#include <core/c3dParser.h>
#include <plugins/chart/INTELreader.h>
#include <plugins/chart/DECreader.h>
#include <plugins/chart/MIPSreader.h>
#include <stdio.h>
#include <iostream>

////////////////////////

c3dParser::c3dParser(){

	this->parameters = 0;
	this->header = 0;
	this->data = 0;
	this->procRead = 0;

}

c3dParser::~c3dParser() {

	//std::cout << "\ndestructor c3dparser\n";

	if(this->procRead){
		delete this->procRead;
		this->procRead = 0;
	}

}

bool c3dParser::saveData(std::string filePath, C3D_Data* data) {

//	std::cout << " saving file into " << filePath << "\n";

	std::ofstream file;
	std::ofstream::pos_type size;

	bool ret = true;

	file.open(filePath.c_str(), std::ios::out  |  std::ios::binary); // |app if there is a need to append data to the file

	if (file.is_open())
	{

		/////////////////////////////////////////

		data->getHeader()->saveHeader(file);
		data->getParameters()->saveParameters(file, data->getHeader());
		data->getData()->saveData(file);

		////////////////////////////////////////
		file.close();
	}
	else
	{
		ret = false;
	}

	////////////////////////////////////////////

	return ret;
}

C3D_Data* c3dParser::parseData(std::string filePath) {

	std::ifstream file;
	std::ifstream::pos_type size;
	int firstParamBlockNumber = -1;

////////////////////////////////////////////////

	file.open(filePath.c_str(), std::ios::in  |  std::ios::binary  |  std::ios::ate);

////////////////////////////////////////////////

	if (file.is_open())
  {

	char* memCont = 0;
	memCont = new char [2];
	file.seekg (0, std::ios::beg);
    file.read (memCont, 2);

	////std::cout << "byte 1: " << (short) memCont[0] << "\n";	
		////std::cout << std::bitset<std::numeric_limits<short>::digits>((short) memCont[0]);
//	std::cout << "byte 2: " << (short) memCont[1] << "\n";
		//std::cout << std::bitset<std::numeric_limits<short>::digits>((short) memCont[1]);

	firstParamBlockNumber = memCont[0];

	if(memCont[1] != 80)
	{
		std::cout << "INVALID FILE !\n\n";
		delete[] memCont;
		return NULL;
	}

	delete[] memCont;
	//std::cout << "FILE OK - PROCESSING START !\n\n";

///////////////////////////////////////

	file.seekg (0, std::ios::end);
	size = file.tellg();
    this->memContent = new char [size];
	file.seekg (0, std::ios::beg);
    file.read (this->memContent, size);

///////////////////////////////////////

    file.close();

	if(firstParamBlockNumber > -1){
		//std::cout << "First param " << firstParamBlockNumber << "\n";
	}else {
		return NULL;
	}

	//std::cout << "parsing data start !\n";

/////////////////////////////////////////////////

	/////////////////////////////////////// Processor type from the parameter section shortcut

	/*

	//Read - test
	
	//firstParamBlockNumber = ((firstParamBlockNumber-1)*512);

	int start = 000;

	for(int i=0; i < 500; i++){
		std::cout << "byte " << (start+i) << "word(" << (start+i)/2 << ") " <<  (short) this->memContent[start+i] << " " << (char) this->memContent[start+i] << "\n";
	}
	*/

	firstParamBlockNumber = ((firstParamBlockNumber-1)*512)+3;

	unsigned char singleByte = this->memContent[firstParamBlockNumber];
	//std::cout << "proc value = " << (short) singleByte << "\n";
	singleByte -= 83;

	ProcessorReader* proc = 0;

	//std::cout << (short) singleByte << "\n";

	switch(singleByte) {

	case 1: { 
		//std::cout << "1- INTEL\n"; 
		proc = new INTELreader(this->memContent, size); 
		break;
			}
	case 2: { 
		//std::cout << "2- DEC\n"; 
		proc = new DECreader(this->memContent, size); 
		break;
			}
	case 3: { 
		//std::cout << "3- MIPS\n"; 
		proc = new MIPSreader(this->memContent, size); 
		break;
			}

default: {std::cout << "Unknown Processor - Parser Fatal error !"; return NULL; }

	}

	////////////////// header

	proc->readStart(); // !

	this->header = new MarkerHeader(proc);

	if(!this->header->parseHeader())
	{
		if(proc) delete proc;
		proc = 0;

		if(this->header) delete this->header;
		this->header = 0;

		return NULL;
	}

	///////////////// params

	this->parameters = new Parameters(proc);

	if(!this->parameters->parseParameters(this->header->getParamStartBlock()))
	{
		if(proc) delete proc;
		proc = 0;

		if(this->header) delete this->header;
		this->header = 0;

		if(this->parameters) delete this->parameters;
		this->parameters = 0;

		return NULL;
	}

	///////////////// data

	this->data = new Data(this->header, proc);	

	if(!this->data->parseData())
	{
		if(proc) delete proc;
		proc = 0;

		if(this->header) delete this->header;
		this->header = 0;

		if(this->parameters) delete this->parameters;
		this->parameters = 0;

		if(this->data) delete this->data;
		this->data = 0;

		return NULL;
	}

/////////////////////////////////////////////////

	C3D_Data* res = new C3D_Data(this->header, this->parameters, this->data);
///////////////////////////////////////////////////

	return res;

	}
	else
	{
		return NULL;
	}

}

