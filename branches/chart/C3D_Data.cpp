#include "C3D_Data.h"
#include <iostream>

///////////////////////////////////////////

	C3D_Data::C3D_Data(Header* header, Parameters* parameters, Data* data){

		this->header = header;
		this->parameters = parameters;
		this->data = data;

	}

	C3D_Data::~C3D_Data(){

		if(this->header) {
		delete this->header;
		this->header = 0;
	}

	if(this->parameters) {
		delete this->parameters;
		this->parameters = 0;
	}

	if(this->data){
		delete this->data;
		this->data = 0;
	}

	}

	std::string C3D_Data::toString() {

	this->header->toString();
	//this->parameters->toString();
	//this->data->toString();

	return "";

	}