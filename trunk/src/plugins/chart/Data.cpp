#include "Data.h"

Data::Data(Header* pam, ProcessorReader* proc){

	this->header = pam;
	this->proc = proc;

	this->data = 0;
	this->analog = 0;

}

/////////////////////////////////////////

bool Data::parseData(){

	
	//std::cout << "Scale factor: " << this->header->getScaleFactor() << "\n";

	/*
	if(this->header->getScaleFactor() < 0)
	{
		std::cout << "DATA CODED USING Float32-data\n";
	}
	else
	{
		std::cout << "DATA CODED USING INTEGER DATA\n";
	}
	*/

	//std::cout << "Data start block: " << this->header->getdDtaStartBlockNumber() << " bytes: " << (this->header->getdDtaStartBlockNumber()*512) << "\n";
	//std::cout << "Current index: " << this->proc->curPos << " \n";
	//std::cin.get();

	//std::cout << "Frames number: " << (this->header->getLastDataFrame() - this->header->getFirstDataFrame() + 1) << "\n";

	this->numberOfFrames = (this->header->getLastDataFrame() - this->header->getFirstDataFrame() + 1);

	if(this->header->getNumberOfAnalogSamplesPerFrame() > 0){
	   analogChannelsNumber = (this->header->getNumberOfAllAnalogMesPerFrame() / this->header->getNumberOfAnalogSamplesPerFrame());
	}

/////////////////////////////////////////

	/* 
	
	Data array of size framesNumber. Each frame consists of 4 float values * NumberOfC3DPoints. First three values are x,y,z and the last one should be
	casted to short and separated into 2 bytes. Each bit means camera with such a number caught a marker or not. After marker data there is array of length
	this->header->getNumberOfAnalogSamplesPerFrame() * 4 analog float values.

	*/

////////////////////////////////////////////////////

	this->proc->seek( (this->header->getdDtaStartBlockNumber()-1)*512 ); // !

	this->data = new float[this->numberOfFrames * this->header->getNumberOfC3DPoints() * 4];
	this->analog = new float[this->numberOfFrames * this->header->getNumberOfAnalogSamplesPerFrame() * analogChannelsNumber];

	int dataP = 0;
	int analogP = 0;

	if(this->header->getScaleFactor() < 0)
	{

	for(int i=0; i < this->numberOfFrames; i++){

	//	std::cout << "Frame " << i << "___________________\n";

		for(int j=0; j < this->header->getNumberOfC3DPoints(); j++){
			
			data[dataP++] = this->proc->readNextFloat();
			//std::cout << data[dataP-1] << " X\n";
			data[dataP++] = this->proc->readNextFloat();
			//std::cout << data[dataP-1] << " Y\n";
			data[dataP++] = this->proc->readNextFloat();
			//std::cout << data[dataP-1] << " Z\n";
			data[dataP++] = this->proc->readNextFloat();
			//std::cout << data[dataP-1] << " CHECK\n";

		//	std::cin.get();
		}

		for(int j=0; j < this->header->getNumberOfAnalogSamplesPerFrame(); j++){
		
			for(int x=0; x < analogChannelsNumber; x++){

				this->analog[analogP++] = this->proc->readNextFloat();
			//	std::cout << "Channel " << x << ": " << this->analog[analogP-1] << "\n";
				

			}
		}

	}

	}
	else
	{

	for(int i=0; i < this->numberOfFrames; i++){

	//	std::cout << "Frame " << i << "___________________\n";

		for(int j=0; j < this->header->getNumberOfC3DPoints(); j++){
			
			
			data[dataP++] = (this->proc->readNextWord()*this->header->getScaleFactor());
			data[dataP++] = (this->proc->readNextWord()*this->header->getScaleFactor());
			data[dataP++] = (this->proc->readNextWord()*this->header->getScaleFactor());
			
			data[dataP++] = this->proc->readNextWord(); 

		}

		for(int j=0; j < this->header->getNumberOfAnalogSamplesPerFrame(); j++){
		
			for(int x=0; x < analogChannelsNumber; x++){

				this->analog[analogP++] = this->proc->readNextWord();
			//	std::cout << "Channel " << x << ": " << this->analog[analogP-1] << "\n";

			}
		}

	}

	}

//////////////////////////////////////////////

	return true;

}

void Data::saveData(std::ofstream& out){

	//std::cout << "SAVING DATA !\n";

	//////////////////////////////

	int dataP = 0;
	int analogP = 0;

	if(this->header->getScaleFactor() < 0)
	{

	for(int i=0; i < this->numberOfFrames; i++){

		//std::cout << "Frame " << i << "___________________\n";

		for(int j=0; j < this->header->getNumberOfC3DPoints(); j++){
			
			this->proc->writeFloat(data[dataP++], out);
			this->proc->writeFloat(data[dataP++], out);
			this->proc->writeFloat(data[dataP++], out);
			this->proc->writeFloat(data[dataP++], out);

		}

		for(int j=0; j < this->header->getNumberOfAnalogSamplesPerFrame(); j++){
		
			for(int x=0; x < analogChannelsNumber; x++){

				this->proc->writeFloat(this->analog[analogP++], out);

			}
		}

	}

	}
	else
	{

	for(int i=0; i < this->numberOfFrames; i++){

		//std::cout << "Frame " << i << "___________________\n";

		for(int j=0; j < this->header->getNumberOfC3DPoints(); j++){
			
			this->proc->writeWord((short) data[dataP++], out);		
			this->proc->writeWord((short) data[dataP++], out);	
			this->proc->writeWord((short) data[dataP++], out);	
			this->proc->writeWord((short) data[dataP++], out);	

		}

		for(int j=0; j < this->header->getNumberOfAnalogSamplesPerFrame(); j++){
		
			for(int x=0; x < analogChannelsNumber; x++){

				this->proc->writeWord(this->analog[analogP++], out);

			}
		}

	}

}

///////////////////////////////////

}

Data::~Data(){

//std::cout << "destructor data\n";

	if(this->data) {
		delete[] this->data;
		this->data = 0;
	}

	if(this->analog) {
		delete[] this->analog;
		this->analog = 0;
	}

}

std::string Data::toString() {

	/* test */
	
	std::cout << "First frame: \n" <<
		"X: " << this->data[0] << "\n" <<
		"Y: " << this->data[1] << "\n" <<
		"Z: " << this->data[2] << "\n" <<
		"CHECK: " << this->data[3] << "\n" <<

		"Second frame: \n" <<
		"X: " << this->data[1*this->header->getNumberOfC3DPoints()*4 + 0] << "\n" <<
		"Y: " << this->data[1*this->header->getNumberOfC3DPoints()*4 + 1] << "\n" <<
		"Z: " << this->data[1*this->header->getNumberOfC3DPoints()*4 + 2] << "\n" <<
		"CHECK: " << this->data[1*this->header->getNumberOfC3DPoints()*4 + 3] << "\n" <<

		"Last frame: \n" <<
		"X: " << this->data[(this->numberOfFrames-1)*this->header->getNumberOfC3DPoints()*4 + 0] << "\n" <<
		"Y: " << this->data[(this->numberOfFrames-1)*this->header->getNumberOfC3DPoints()*4 + 1] << "\n" <<
		"Z: " << this->data[(this->numberOfFrames-1)*this->header->getNumberOfC3DPoints()*4 + 2] << "\n" <<
		"CHECK: " << this->data[(this->numberOfFrames-1)*this->header->getNumberOfC3DPoints()*4 + 3] << "\n";


	for(int j=0; j < this->header->getNumberOfAnalogSamplesPerFrame(); j++){
		
			for(int x=0; x < analogChannelsNumber; x++){

			     std::cout << "Channel " << x << ": " << this->analog[j*x + x] << "\n";

			}
	}
	


return "";

}

int Data::getNumberOfFrames() {

	return this->numberOfFrames;

}

int Data::getAnalogChannelsNumber() {

	return this->analogChannelsNumber;

}


float* Data::getData() {

	return this->data;

}

float* Data::getAnalog() {

	return this->analog;

}