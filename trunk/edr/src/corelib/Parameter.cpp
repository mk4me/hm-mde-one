#include <core/Parameter.h>
#include <iostream>


Parameter::Parameter(){

	data = 0;

}

std::string Parameter::getName(){ 

	return this->name;

}

char Parameter::getDataType() {

	return this->dataType;

}

void Parameter::setDataType(char type) {

	this->dataType = type;

}

std::string Parameter::getDescription(){

	return this->description;

}

std::vector<short> Parameter::getDimensions() const {

	return this->dimensions;

}


float* Parameter::getData(){

	return this->data;

}

////////////////////////////////////////////////////

void Parameter::setName(std::string name){

	this->name = name;

}

void Parameter::setDescription(std::string description){

	this->description = description;

}

void Parameter::addDimension(short dim){

	this->dimensions.push_back(dim);

}

void Parameter::setData(float* data){

	this->data = data;

}

////////////////////////////////////////////////////

Parameter::~Parameter(){

	if(data) {
		delete[] data;
		data = 0;
	}

}

////////////////////////////////////////////////////

int Parameter::saveParam(std::ofstream& out, ProcessorReader* proc, char id) {

	int res = 0;

	//////////////////

	proc->writeByte(name.length(),out); // always unlocked

	proc->writeByte(-id,out);

	res += 2;
	////////// param name

	const char* ptr = 0;
	ptr = name.data();

	for(int i=0; i < name.length() ; i++) 
	{

		proc->writeByte( ptr[i], out);

	}

	res += name.length();
	/////////

	std::vector<short>::const_iterator iter; //// dimensions calculations
	int sumDim = 1;

	for (iter = this->dimensions.begin();
		iter != this->dimensions.end();
		iter++)
	{
		sumDim *= *iter;
	}

	/////////

	int dataLength = 0;

	if(dataType < 0){
		dataLength = -dataType;
	}
	else
	{
		dataLength = dataType;
	}

	/////////////

	if(sumDim == 1)  // scalar data ..
	{

		proc->writeWord( (short) 1 + 1 + dataLength + 1 + description.length() + 2,out); // param length + 2 extra space

		res += 2;

		proc->writeByte( dataType, out); // data type

		res += 1;

		proc->writeByte( (char) 0, out); // 0 - Scalar data
		res += 1;

		///// Parameters dimensions skipped

		switch((short) ((signed char)dataType)) {

					case -1: {
						proc->writeByte((char) data[0] ,out);
						res += 1;
						break;
							 }
					case 1: {
						proc->writeByte((unsigned char) data[0] ,out);
						res += 1;
						break;
							}
					case 2: {
						proc->writeWord((short) data[0] ,out);
						res += 2;
						break;
							}
					case 4: {
						proc->writeFloat( data[0], out);
						res += 4;
						break;
							}
		}

	}
	else
	{

		proc->writeWord( (short) 1 + 1 + dimensions.size()+ (sumDim*dataLength) + 1 + description.length() + 2,out); // param length + 2 extra space

		res += 2;

		proc->writeByte( dataType, out); // data type

		res += 1;

		proc->writeByte( (char) dimensions.size(), out); // number of dimensions
		res += 1;

		for (iter = this->dimensions.begin();
			iter != this->dimensions.end();
			iter++)
		{
			proc->writeByte((char) *iter, out); // dimensions values
		}

		res += dimensions.size();

		///////////// param data

		if(sumDim != 0){

			for(int i=0; i < sumDim-1; i++){

				switch((short) ((signed char)dataType)) {

					case -1: {
						proc->writeByte((char) data[i] ,out);
						break;
							 }
					case 1: {
						proc->writeByte((unsigned char) data[i] ,out);
						break;
							}
					case 2: {
						proc->writeWord((short) data[i] ,out);
						break;
							}
					case 4: {
						proc->writeFloat( data[i], out);
						break;
							}
				}

			}

			switch((short) ((signed char)dataType)) {

					case -1: {
						proc->writeByte((char) data[sumDim-1] ,out);
						res += sumDim*1; // bytes size
						break;
							 }
					case 1: {
						proc->writeByte((unsigned char) data[sumDim-1] ,out);
						res += sumDim*1; // bytes size
						break;
							}
					case 2: {
						proc->writeWord((short) data[sumDim-1] ,out);
						res += sumDim*2; // word size
						break;
							}
					case 4: {
						proc->writeFloat( data[sumDim-1], out);
						res += sumDim*4; // float size
						break;
							}
			}

		}

	}
	////////////

	proc->writeByte(description.length(), out); // nof description characters
	res += 1;

	/////////// description

	ptr = description.data();

	for(int i=0; i < description.length() ; i++) 
	{

		proc->writeByte( ptr[i], out);

	}

	res += description.length();

	///////////

	//proc->writeWord((short) 0, out); // 2 extra space
	//res += 2;

	///////////////

	//std::cout << "PARAMETER RESULT: " << res << "\n";
	return res;

}
std::string Parameter::getDataAsStringsArray(int i) const
{

	std::string result ;


	//////////////////////////
	result.reserve(this->dimensions[0]);


	//create string data
	for(int j=0; j < this->dimensions[0];j++)
	{
		result.push_back( static_cast<char>(data[i * this->dimensions[0] + j]) );	

	}


	return result;
}

// for 1D data e.g. SCALE,OFFSET etc.
float* Parameter::getDataAsFloatArray()
{



	return static_cast<float*> (data);
}

std::string Parameter::toString(){

	std::cout << this->name << " " << this->description << " DATA-TYPE: " << (short) this->dataType << "\n";


	std::vector<short>::const_iterator iter;
	int sumDim = 1;

	for (iter = this->dimensions.begin();
		iter != this->dimensions.end();
		iter++)
	{
		std::cout << *iter << " ";
		sumDim *= *iter;
	}
	std::cout << "\n";

	for(int i=0; i < sumDim-1; i++){

		switch((short) ((signed char)dataType)) {

					case -1: {
						std::cout << (char) data[i] << ",";
						break;
							 }
					case 1: {
						std::cout << (unsigned char) data[i] << ",";
						break;
							}
					case 2: {
						std::cout << (short) data[i] << ",";
						break;
							}
					case 4: {
						std::cout << data[i] << ",";
						break;
							}
		}

	}

	switch((short) ((signed char)dataType)) {

					case -1: {
						std::cout << (char)data[sumDim-1] << "\n";
						break;
							 }
					case 1: {
						std::cout << (unsigned char) data[sumDim-1] << "\n";
						break;
							}
					case 2: {
						std::cout << (short) data[sumDim-1] << "\n";
						break;
							}
					case 4: {
						std::cout << data[sumDim-1] << "\n";
						break;
							}
	}

	return "";
}