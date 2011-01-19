#include "Parameters.h"

Parameters::Parameters(ProcessorReader* proc) {

	this->proc = proc;

////////////////////////////////////////
}

std::map<short,GroupData*> Parameters::getGroups(){

	return this->groups;

}

std::map<std::string,GroupData*> Parameters::getGroupsByName(){

	return this->groupsByName;

}

void Parameters::addGroup(short key, std::string name, GroupData* value){

	this->groups[key] = value;
	this->groupsByName[name] = value;

}

short Parameters::getNumberOfParamBlocks(){

	return this->numberOfParamBlocks;

}
void Parameters::setNumberOfParamBlocks(short number){

	this->numberOfParamBlocks = number;

}

unsigned char Parameters::getProcessorType(){

	return this->processorType;

}
void Parameters::setProcessorType(unsigned char type){

	this->processorType = type;

}

void Parameters::saveParameters(std::ofstream& out, MarkerHeader* header){

	/// param header 

   this->proc->writeByte((unsigned char) 0, out); // 1
   this->proc->writeByte((unsigned char) 0, out); // 2
   this->proc->writeByte((unsigned char) this->getNumberOfParamBlocks(), out); // 3
   this->proc->writeByte((unsigned char) 84, out);  ///4 - processor type INTEL

   //std::cout << "NUMBER OF PARAM BLOCKS: " << this->getNumberOfParamBlocks() << "\n";

   int numberOfBytesToWrite = this->getNumberOfParamBlocks() * 512;

	//std::cout << "NUMBER OF BYTES TO WRITE: " << numberOfBytesToWrite << "\n";

   numberOfBytesToWrite -= 4; // 4 already wrote

  // std::cin.get();
	//////////////////////// end of header

	for( std::map<short, GroupData*>::iterator ii=this->groups.begin(); ii!=this->groups.end(); ++ii)
	   {

		  numberOfBytesToWrite -= ((*ii).second)->saveGroup(out, this->proc);

	  }

	//std::cout << "Bytes unused till the end of param section (if standard was satisfied):" << numberOfBytesToWrite << "\n";
	//std::cin.get();

	//std::cout << "DataStartNumber:" << header->getdDtaStartBlockNumber() << "\n";
	//std::cout << "RealBytesDataStart:" << (header->getdDtaStartBlockNumber()-1)*512 << "\n";
	//std::cout << "Current position:" << this->proc->getCurPos() << "\n";
	//std::cin.get();

	int realDistance =  ((header->getdDtaStartBlockNumber()-1)*512)-this->proc->getCurPos();
	//std::cout << "Real number of bytes till the end of param section: " << realDistance << "\n";
	//std::cin.get();

	for(int i=0; i < realDistance; i++)
	{

		this->proc->writeByte((unsigned char) 0, out); // empty space till the data section

	}

}



bool Parameters::parseParameters( short startData){

	unsigned char singleByte;
	short numberOfBytesToScan = 0;
	short bytesAlreadyScanned = 0;

	//////////////////////////

	//std::cout << " start param data: " << startData << "\n";


	this->proc->seek( ((startData-1)*512)+2 ); // start of param section

	///////////////////////////

	numberOfBytesToScan = this->proc->readNextByte();
	this->numberOfParamBlocks = numberOfBytesToScan;

	//std::cout << "number of param blocks = " << this->numberOfParamBlocks << "\n";

	numberOfBytesToScan *= 512; //size of block
	numberOfBytesToScan -= 4; // already read;

	bytesAlreadyScanned += 4;

	//std::cout << "number of bytes to scan = " << numberOfBytesToScan << "\n";
	//std::cin.get();

	this->processorType = (this->proc->readNextByte()-83);
	//std::cout << "procType: " << (short) this->processorType << "\n";

		short groupNumber = 0;
		short groupNameLength = 0;

		singleByte = this->proc->readNextByte();
		groupNameLength = singleByte;
		//std::cout << "group name length : " << groupNameLength << " " << singleByte << "\n";

		singleByte = this->proc->readNextByte();
		groupNumber = singleByte;
		//std::cout << "group number : " << (short) ((signed char)singleByte) << " " << singleByte << "\n";

		bytesAlreadyScanned++;

		char* tmpBuf = 0;
		short nextDataOffset = -1;
		short whereToJump = -1;
		GroupData* group = 0;
		Parameter* param = 0;

		while(groupNameLength)
		//for(int xx=0; xx < 6; xx++)
		if(groupNameLength)  // there is a name - may be -value if group is locked
		{

			if((short) ((signed char)groupNameLength) < 0 ) { // group / param locked !
				groupNameLength = -groupNameLength;
				//std::cout << "Group/Param locked! Length = " << (short) ((signed char)groupNameLength) << "____________________________\n";
				//std::cin.get();
			}

			tmpBuf = new char[(short) ((signed char)groupNameLength)+1]; // mem declaration - > +1 for \0

			////////////////////////////////////

			if( ((short) ((signed char)groupNumber)) < 0) // GROUP ____________________________________________________________
			{
				//std::cout << "GROUP \n";

				for(int i=0; i < groupNameLength; i++){
				tmpBuf[i] = this->proc->readNextByte(); 
				bytesAlreadyScanned++;
				//std::cout << "letter " << i << ": " << tmpBuf[i] << "\n";
				}

			tmpBuf[(short) ((signed char)groupNameLength)] = '\0';

			std::string groupName = std::string(tmpBuf); 
			//std::cout << "Group name: " << groupName << "\n";

			///////////////////////////////////////////////////////////////////////////////////////////

			group = new GroupData();
			group->setName(groupName);
			group->setId((short) ((signed char)groupNumber));

			addGroup(group->getId(), group->getName(), group);
			//this->groups[group->getId()] = group; //////////////////////////////////////////////////

			/////////////////////////////////////////////////////////////////////////////////////////////

			nextDataOffset = this->proc->readNextWord();

			bytesAlreadyScanned += 2;

			//std::cout << "Next data is supposed to be after next " << nextDataOffset << " bytes.\n";
			//std::cout << "Already read (more-less): " << bytesAlreadyScanned << "\n";

			whereToJump = this->proc->getCurPos() + nextDataOffset - 1;
			//std::cout << "where to jump? : " << whereToJump << "\n";

			singleByte = this->proc->readNextByte();
			groupNameLength = singleByte;
			//std::cout << "Group description length : " << (short) ((signed char)singleByte) << "\n"; 

			bytesAlreadyScanned++;

			////////////////////////////////////

			delete[] tmpBuf; // mem free
			tmpBuf = new char[groupNameLength+1]; // mem declaration - > +1 for \0

			////////////////////////////////////

			for(int i=0; i < groupNameLength; i++){
				tmpBuf[i] = this->proc->readNextByte(); 
				bytesAlreadyScanned++;
				//std::cout << "letter " << i << ": " << tmpBuf[i] << "\n";
			}
			tmpBuf[groupNameLength] = '\0';

			std::string groupDescription = std::string(tmpBuf); 
			//std::cout << "Group description: " << groupDescription << "\n";

			group->setDescription(groupDescription);

			///////////////////////////////////

			delete[] tmpBuf; // mem free

		     //std::cout << "jumping to the next data\n";
		    this->proc->seek(whereToJump);

			//////////////////////////////////

			singleByte = this->proc->readNextByte();
			groupNameLength = singleByte;
			//std::cout << "next data length : " << groupNameLength << " " << singleByte << "\n";

			bytesAlreadyScanned++;
			//std::cout << "Already read: " << bytesAlreadyScanned << "\n";

			singleByte = this->proc->readNextByte();
			bytesAlreadyScanned++;
			groupNumber = singleByte;
			//std::cout << "next data id number : " << (short) ((signed char)singleByte) << " " << singleByte << "\n";

			/////////////////////////////////

			}
			else // PARAMETERS ____________________________________________________
			{

				//std::cout << " PARAMETER !!!! \n";
				//std::cout << "Belongs to group with ID: " << groupNumber << "\n";

				//std::cout << "Param length: " << groupNameLength << "  " << (short) ((signed char)groupNameLength) << "\n";

				for(int i=0; i < (short) ((signed char)groupNameLength); i++){
				tmpBuf[i] = this->proc->readNextByte(); 
				bytesAlreadyScanned++;

				//std::cout << "letter " << i << ": " << tmpBuf[i] << "\n";
				}

			    tmpBuf[(short) ((signed char)groupNameLength)] = '\0';

			    std::string groupName = std::string(tmpBuf); 
			    //std::cout << "Parameter name: " << groupName << "\n";

				////////////////////////////////////////////////////////////////////////////////

				param = new Parameter();
				param->setName(groupName);

				///////////////////////////////////////////////////////////////////////////////

				nextDataOffset = this->proc->readNextWord();
				bytesAlreadyScanned += 2;
				//std::cout << "Next data is supposed to be after next " << nextDataOffset << " bytes.\n";

				whereToJump = this->proc->getCurPos() + nextDataOffset - 1;
				//std::cout << "where to jump? : " << whereToJump << "\n";
				
				//std::cout << "Already read: " << bytesAlreadyScanned << "\n";

				char paramType = 0;
				paramType = this->proc->readNextByte();
				bytesAlreadyScanned++;
				//std::cout << "PARAM DATA TYPE: " << (short) ((signed char)paramType) << " "; 
					
				//////////////////////////////////////////////////////////////////////////////

				param->setDataType(paramType);

				//////////////////////////////////////////////////////////////////////////////

				/*
				switch((short) ((signed char)paramType)) {

					case -1: {std::cout << "CHAR";break;}
					case 1: {std::cout << "BYTE";break;}
					case 2: {std::cout << "INTEGER";break;}
					case 4: {std::cout << "FLOAT";break;}
					default: { std::cout << "ERROR DATA TYPE !";break;}
				}
				*/

				char numberOfDimensions = -1;
				numberOfDimensions = this->proc->readNextByte();
				bytesAlreadyScanned++;
				//std::cout << "\nNUMBER OF DIMENSIONS: " << (short) ((signed char)numberOfDimensions) << " "; 
			
				if(numberOfDimensions > 0){
			
					//std::cout << "DIMENSIONS > 0 - allocating memory\n";

					tmpBuf = new char[numberOfDimensions];
					short dim1D = 1;

					for(int i=0; i < numberOfDimensions; i++)
					{
						tmpBuf[i] = this->proc->readNextByte();
						bytesAlreadyScanned++;
						//std::cout << "Dimension " << i << " value=" << (short)( (unsigned char)tmpBuf[i]) << "\n";

						///////////////////////////////////////////////////////////////////////////

						param->addDimension((short)( (unsigned char)tmpBuf[i]));

						/////////////////////////////////////////////////////////////////////////////

						dim1D *= (short)( (unsigned char)tmpBuf[i]); // unrolling the m.d. arrays
					}
		            
					//std::cout << "Unpacked 1D array of data is of length: " << dim1D << "____________________________________\n";
					

				switch((short) ((signed char)paramType)) {

					case -1: {

						//CHAR

						float* Data = new float[dim1D];
						char temp = 0;

						for(int i=0; i < dim1D; i++){
							
							temp = this->proc->readNextByte();
							bytesAlreadyScanned++;
							Data[i] = temp;
							//std::cout << (char) Data[i] << " ";

						}

						///////////////////////////////////////////////////////////////////////////

						param->setData(Data);

						/////////////////////////////////////////////////////////////////////////////
						//std::cout << "\n";

						break;						 						 
							 }
					case 1: {
						
						//BYTE

						float* Data = new float[dim1D];

						char temp = 0;

						for(int i=0; i < dim1D; i++){
							
							temp = this->proc->readNextByte();
							bytesAlreadyScanned++;
							Data[i] = temp;
							//std::cout << Data[i] << " ";				

						}

						///////////////////////////////////////////////////////////////////////////

						param->setData(Data);

						/////////////////////////////////////////////////////////////////////////////

						//std::cout << "\n";

						break;							
							}
					case 2: {
						
						//INTEGER

						float* Data = new float[dim1D];
						short temp = -1;

						for(int i=0; i < dim1D; i++){
							
							temp = this->proc->readNextWord();
							bytesAlreadyScanned += 2;
							Data[i] = temp;
							//std::cout << Data[i] << " ";

						}

						///////////////////////////////////////////////////////////////////////////

						param->setData(Data);

						/////////////////////////////////////////////////////////////////////////////

						//std::cout << "\n";

						break;			
							}
					case 4: {

						//FLOAT

						float* Data = new float[dim1D];

						float temp = -1;

						for(int i=0; i < dim1D; i++){
							
							temp = this->proc->readNextFloat();
							bytesAlreadyScanned += 4;
							Data[i] = temp;
							//std::cout << Data[i] << " ";

						}

						///////////////////////////////////////////////////////////////////////////

						param->setData(Data);

						/////////////////////////////////////////////////////////////////////////////

						//std::cout << "\n";

						break;	
							}
				}

					////////////////////////////////////////////

					delete[] tmpBuf;

				}
				else
				{

					///////////////////////////////////////////////////////////////////////////

						param->addDimension( 1 );

					/////////////////////////////////////////////////////////////////////////////

					//std::cout << "SCALAR VALUE\n";

					switch((short) ((signed char)paramType)) {

					case -1: {
						
						//CHAR	
						float* Data = new float[1];

							Data[0]  = this->proc->readNextByte();
							bytesAlreadyScanned++;
							//std::cout << Data[0] << "\n";
							
							param->setData(Data);

						break;
							 }

					case 1: {
						
						//BYTE
						float* Data = new float[1];

							Data[0]  = (short) ((unsigned char)this->proc->readNextByte());
							bytesAlreadyScanned++;
							//std::cout << Data[0] << "\n";
							
							param->setData(Data);

						break;
							}

					case 2: {
						
						//INTEGER
							float* Data = new float[1];

							Data[0]  = (unsigned short) this->proc->readNextWord();
							bytesAlreadyScanned += 2;
							//std::cout << Data[0] << "\n";
							
							param->setData(Data);

						break;
							}

					case 4: {

						//FLOAT
						float* Data = new float[1];

							Data[0]  = this->proc->readNextFloat();
							bytesAlreadyScanned += 4;
							//std::cout << Data[0] << "\n";
							
							param->setData(Data);
						
						break;

							}
				}

			}

			///////////// parameter description /////////////

			short descrLenght = (short)((unsigned char)this->proc->readNextByte());
			//std::cout << "description length: " << descrLenght << "\n";

			bytesAlreadyScanned++;

			tmpBuf = new char[(short)((unsigned char)descrLenght)+1]; // mem declaration - > +1 for \0

				for(int i=0; i < (short)((unsigned char)descrLenght); i++){
				tmpBuf[i] = this->proc->readNextByte(); 
				bytesAlreadyScanned++;
				//std::cout << "letter " << i << ": " << tmpBuf[i] << " " << (short) ((signed char) tmpBuf[i]) << "\n";
				}

			    tmpBuf[descrLenght] = '\0';

			    std::string groupDescr = std::string(tmpBuf); 
			    //std::cout << "Parameter description: " << groupDescr << "\n";

			///////////////////////////////////////////////////////////////////

				param->setDescription(groupDescr);
		
				group = this->groups[-groupNumber];
				if(group != NULL) {

					this->groups[-groupNumber]->addParameter(param->getName(),param); // adding new param to its group

				}
			
			//////////////////////////////////////////////////////////////////

			delete[] tmpBuf;

			//////////////////////////////////

			//std::cout << "\njumping to the next data\n";
		    this->proc->seek(whereToJump);

			//std::cout << "Already read: " << bytesAlreadyScanned << "\n";

			//////////////////////////////////

			singleByte = this->proc->readNextByte();
			bytesAlreadyScanned++;
			groupNameLength = singleByte;
			//std::cout << "next data length : " << (short) ((signed char)groupNameLength) << " " << singleByte << "\n";

			singleByte = this->proc->readNextByte();
			bytesAlreadyScanned++;
			groupNumber = singleByte;
			//std::cout << "next data id number : " << (short) ((signed char)singleByte) << " " << singleByte << "\n";

			/////////////////////////////////

			}

			//std::cin.get();
		}
	    ///////////////////////////////////////////////

	return true;

}

std::string Parameters::toString(){

	std::cout << "numberOfParamBlocks " << (unsigned short)numberOfParamBlocks << "\n" <<
                 "processorType " << (unsigned short)processorType << "\n";

	for( std::map<short, GroupData*>::iterator ii=this->groups.begin(); ii!=this->groups.end(); ++ii)
	   {

		   ((*ii).second)->toString();

	  }

	return "";
}

Parameters::~Parameters() {

	//std::cout << "destructor parameters\n";


	for( std::map<short, GroupData*>::iterator ii=this->groups.begin(); ii!=this->groups.end(); ++ii)
	   {

	       delete (*ii).second;

	  }

	this->groups.clear();

}