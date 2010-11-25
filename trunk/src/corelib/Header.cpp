#include "Header.h"
#include "ProcessorReader.h"


//////////////////////

MarkerHeader::MarkerHeader(ProcessorReader* proc) {

	this->proc = proc;

	eventTimes = 0;
	eventLabels = 0;
	eventStatus = 0;

	this->numberOfEvents = 0;

////////////////////////////////////////
}

MarkerHeader::~MarkerHeader() {

	//std::cout << "destructor header\n";

	if(eventTimes) {
		delete[] eventTimes;
		eventTimes = 0;
	}

	if(eventLabels) {
		delete[] eventLabels;
		eventLabels = 0;
	}

	if(eventStatus) {
		delete[] eventStatus;
		eventStatus = 0;
	}

}

void MarkerHeader::saveHeader(std::ofstream& out){

	this->proc->writeStart(); // !!!!!

	////////////////////////////////////////

	unsigned char singleByte = 10;

	
	this->proc->writeByte( getParamStartBlock() , out);
	this->proc->writeByte( (unsigned char) 80 , out); // C3D key value // word1

	/////////////////////////////////////////

	this->proc->writeWord( (short) getNumberOfC3DPoints() , out); // word 2

	this->proc->writeWord( (short) getNumberOfAllAnalogMesPerFrame(), out ); // word 3

	this->proc->writeWord( (short) getFirstDataFrame(), out ); // word 4

	this->proc->writeWord( (short) getLastDataFrame(), out ); // word 5

	this->proc->writeWord( (short) getMaxInterpolation(), out ); // wor 6

	this->proc->writeFloat( getScaleFactor(), out ); // word 7-8

	this->proc->writeWord( (short) getdDtaStartBlockNumber(), out ); // word 9

	this->proc->writeWord( (short) getNumberOfAnalogSamplesPerFrame(), out ); // word 10

	this->proc->writeFloat( getFrameRate(), out ); // word 11-12

	//////////////////////////////

	for(int i=13; i <= 147; i++){

		this->proc->writeWord( (short) 0, out ); // unused space

	}

	///////////////////////////////

	if(this->getLabelAndRangeDataIsPresent()) // event section present ..
	{

		this->proc->writeWord( (short) 12345, out); //148

		this->proc->writeWord( (short) 0, out ); // 149 - ?

		this->proc->writeWord( (short) 12345, out ); // key value //150
 
		this->proc->writeWord( (short) this->getNumberOfEvents(), out ); // correct way // 151

		this->proc->writeWord( (short) 0, out ); // 152 - 0

		for(int i=0; i < this->getNumberOfEvents(); i++) // 153++
		{
			this->proc->writeFloat( this->getEventTimes()[i], out); // event times
		}

				for(int i=0; i < (18 - this->getNumberOfEvents()); i++) // to finish the section
				{
					this->proc->writeFloat( (float) 0, out); 
				}

		for(int i=0; i < this->getNumberOfEvents(); i++)
		{
			this->proc->writeByte( this->getEventStatus()[i], out); // event on off
		}

				for(int i=0; i < (18 - this->getNumberOfEvents()); i++) // to finish the section
				{
					this->proc->writeByte( (char) 0, out); 
				}

				this->proc->writeWord(0,out); // 

		const char* ptr = 0;
		for(int i=0; i < this->getNumberOfEvents(); i++)
		{

			ptr = this->getEventLabels()[i].data();

			for(int j=0; j < 4; j++){
				this->proc->writeByte( ptr[j], out); // event labels
			}

		}

				for(int i=0; i < (18 - this->getNumberOfEvents()); i++) // to finish the section
				{
					this->proc->writeFloat( (float) 0, out);  // same length as 4 byte letters
				}

		for(int i=234; i < 256; i++)
		{
			this->proc->writeWord( (short) 0, out); // empty space reserved for future use
		}

	} 
	else // event section not available
	{

		//std::cout << " bez eventow" << "\n";
		int counter = 0;

		for(int i=0; i < (((this->getParamStartBlock()-1)*256)-147); i++) {

			this->proc->writeWord( (short) 0, out); // 0 in the empty space..
			counter++;

		}

	}

}

bool MarkerHeader::parseHeader(){

	unsigned short word;
	unsigned char singleByte;
	//short word;

////////////////////////////////////////

	//std::cout << "Start parsing header !! " << "\n";

	this->proc->seek(0); // start of the header

	singleByte = proc->readNextByte();
	//std::cout << std::bitset<std::numeric_limits< unsigned short>::digits>(singleByte);
	//std::cout << "byte 1: " << ( short) singleByte << "\n";

	this->paramStartBlock = singleByte;

	singleByte = proc->readNextByte();
	//std::cout << std::bitset<std::numeric_limits< unsigned short>::digits>(singleByte);
	//std::cout << "byte 2: " << ( short) singleByte << "\n";


	
    // byte = 3,4; word = 2 
	word = proc->readNextWord();

			//std::cout << "word 2(number of c3d points in file): ";
			//std::cout << std::bitset<std::numeric_limits<unsigned short>::digits>(word);
			//std::cout << "  " << word << " sizeof " << sizeof(word) << "\n";

	this->numberOfC3DPoints = word;





	// byte = 5,6; word = 3
	word = proc->readNextWord();
	//std::cout << "word 3(total number of analog mes): " << word << "\n";

	this->numberOfAllAnalogMesPerFrame = word;




	// byte = 7,8; word = 4
	word = proc->readNextWord();
	//std::cout << "word 4(first data frame): " << word << "\n";

	this->firstDataFrame = word;


	// byte = 9,10; word = 5
	word = proc->readNextWord();
	//std::cout << "word 5(number of last data frame): " << (signed short) word << "\n";
	//std::cout << std::bitset<std::numeric_limits< unsigned short>::digits>(word);

	this->lastDataFrame = word;




	// byte = 11,12; word = 6
	word = proc->readNextWord();
	//std::cout << "word 6(interpolation gap): " << word << "\n";

	this->maxInterpolation = word;



    // byte = 13,14,15,16; word = 7-8
	this->scaleFactor = proc->readNextFloat(); // 4 bytes into 1 float
	//std::cout << "scaleFactor: " << this->scaleFactor << "\n";

	if(this->scaleFactor < 0)
	{
		//std::cout << "DATA CODED USING Float32-data";
	}
	else
	{
		//std::cout << "DATA CODED USING INTEGER DATA";
	}
	//std::cin.get();

	// byte = 17,18; word = 9
	word = proc->readNextWord();
	//std::cout << "word 9(data start block): " << word << "\n";
	//std::cin.get();
	this->dataStartBlockNumber = word;




	// byte = 19,20; word = 10
	word = proc->readNextWord();
	//std::cout << "word 10(number of analog samples per frame): " << word << "\n";

	this->numberOfAnalogSamplesPerFrame = word;

	if(this->numberOfAnalogSamplesPerFrame != 0){
	//std::cout << "analog channels number : " << (this->numberOfAllAnalogMesPerFrame / this->numberOfAnalogSamplesPerFrame) << "\n";
	}

	// byte = 21,22,23,24; word = 11-12
	this->frameRate = proc->readNextFloat();
	//std::cout << "FrameRate: " << this->frameRate << "\n";

	//std::cin.get();

	// words 13-147 nothing
	// byte = 295,296; word = 148
	this->proc->seek(298);

	word = proc->readNextWord();
	//std::cout << "word 148: (should be 12345) =  " << word << "\n";




	// byte = 297,298; word = 149
	word = proc->readNextWord();
	
	if(word > 0 && word < 19) {

	//std::cout << "word 149: (number of events in incorrect file)" << word << "\n";
	this->numberOfEvents = word;
	this->labelAndRangeDataIsPresent = true;

	}
	else
	{
	//	std::cout << "word 149: " << word << "\n";
		this->labelAndRangeDataIsPresent = false;
	}

	// byte = 299,300; word = 150
	word = proc->readNextWord();
	//std::cout << "word 150: " << word << "\n";

	if(word != 12345)
	{
		this->fourOrTwoLettersLabels = false;
	}
	else
	{
		this->fourOrTwoLettersLabels = true;
	}


	// byte 301-302; word 151
	char tmp[5];
	bool floatToearly = false;

		tmp[0] = proc->readNextByte(); 
		//std::cout << "byte 301: " << ( short) tmp[0] << "\n";

		tmp[1] = proc->readNextByte(); 
		//std::cout << "byte 302: " << ( short) tmp[1] << "\n";
	
		word = *(unsigned short*)tmp;

		if(  word > 0 && word < 19  ) // number of events
		{
			this->numberOfEvents = word; // probably the right value...

			//std::cout << "NUMBER OF EVENTS: " << this->numberOfEvents << "\n";
		}
		else if (this->numberOfEvents != 0) // incorrect data - previously set
		{
			floatToearly = true;
			this->labelAndRangeDataIsPresent = true;
			this->fourOrTwoLettersLabels = true; // probably also 4 instead of 2..
		}
		
		//std::cout << "number of events: " << this->numberOfEvents << "\n";



			if(floatToearly) // incorrect values - first event starts in word 151 instead of 153..
			{

				if (this->numberOfEvents > 0){

					eventTimes = new float[this->numberOfEvents];
					eventLabels = new std::string[this->numberOfEvents];
					eventStatus = new bool[this->numberOfEvents];

					//////////////////////////////////////////////

					tmp[2] = proc->readNextByte(); 
				//	std::cout << "byte 303: " << ( short) tmp[2] << "\n";

					tmp[3] = proc->readNextByte(); 
				//	std::cout << "byte 304: " << ( short) tmp[3] << "\n";

					this->eventTimes[0] = *(float*)tmp;
				//	std::cout << "event " << 0 << ": " << this->eventTimes[0] << "\n";

					for(int i=1; i < this->numberOfEvents; i++){ // first implemented above
	
						this->eventTimes[i] = proc->readNextFloat();
				//		std::cout << "event " << i << ": " << this->eventTimes[i] << "\n";

					}


					this->proc->seek(376);

					for(int i=0; i < this->numberOfEvents; i++)
					{
			
						eventStatus[i] = proc->readNextByte();
				//		std::cout << "event " << i << " status: " << (eventStatus[i] ? "ON" : "OFF") << "\n";

					}


					this->proc->seek(396);

					/*
					for(int i=0; i < 20; i++)
					{

						char bytes = proc->readNextByte();
						std::cout << (396+i) << " " << (char) bytes << " " << (short) bytes << "\n";  

					}
					*/

					///*
					for(int i=0; i < this->numberOfEvents; i++)
					{
			
						if(this->fourOrTwoLettersLabels) { // new file format 4 letters

						tmp[0] = proc->readNextByte(); // 1st letter
						tmp[1] = proc->readNextByte(); // 2nd letter
						tmp[2] = proc->readNextByte(); // 3rd letter
						tmp[3] = proc->readNextByte(); // 4th letter

							tmp[4] = '\0'; // string termination 
							eventLabels[i] = std::string(tmp);

				//		std::cout << eventLabels[i] << "\n";

						}
						else // old file format - 2 letters
						{

						tmp[0] = proc->readNextByte(); // 1st letter
						tmp[1] = proc->readNextByte(); // 2nd letter
						tmp[2] = 0; // 3rd letter
						tmp[3] = 0; // 4th letter

						eventLabels[i] = std::string(tmp);

				//		std::cout << eventLabels[i] << "\n";

						}

					}

					//*/

				}

			}
			else
			{

				//std::cout << "nie niepoprawnie, czyli\n";

				if (this->numberOfEvents > 0){

					//std::cout << "w events poprawnych\n";

					eventTimes = new float[this->numberOfEvents];
					eventLabels = new std::string[this->numberOfEvents];
					eventStatus = new bool[this->numberOfEvents];

					//////////////////////////////////////////////

					proc->readNextWord(); // skip 152 word reserved for future use by c3d

					for(int i=0; i < this->numberOfEvents; i++){ // first implemented above
	
						this->eventTimes[i] = proc->readNextFloat();
						//std::cout << "event " << i << ": " << this->eventTimes[i] << "\n";

					}

					this->proc->seek(376);

					for(int i=0; i < this->numberOfEvents; i++)
					{
			
						eventStatus[i] = proc->readNextByte();
						//std::cout << "event " << i << " status: " << (eventStatus[i] ? "ON" : "OFF") << "\n";

					}

                    this->proc->seek(396);

					for(int i=0; i < this->numberOfEvents; i++)
					{
			
						if(this->fourOrTwoLettersLabels) { // new file format 4 letters

						tmp[0] = proc->readNextByte(); // 1st letter
						tmp[1] = proc->readNextByte(); // 2nd letter
						tmp[2] = proc->readNextByte(); // 3rd letter
						tmp[3] = proc->readNextByte(); // 4th letter

							tmp[4] = '\0'; // string termination 
							eventLabels[i] = std::string(tmp);

					//	std::cout << eventLabels[i] << "\n";

						}
						else // old file format - 2 letters
						{

						tmp[0] = proc->readNextByte(); // 1st letter
						tmp[1] = proc->readNextByte(); // 2nd letter
						tmp[2] = 0; // 3rd letter
						tmp[3] = 0; // 4th letter

						eventLabels[i] = std::string(tmp);

					//	std::cout << eventLabels[i] << "\n";

						}

					}

				}

			}
		
	//////////////////////////////////////////

	//std::cin.get();
	/////////////////////////////////////////
	return true;
}

std::string MarkerHeader::toString(){

	std::cout << "StartParamBlock: " << (short) paramStartBlock << "\n" <<
	             "numberOfC3DPoints: " << numberOfC3DPoints << "\n" <<
	             "numberOfAllAnalogMesPerFrame: " << numberOfAllAnalogMesPerFrame << "\n" <<
					  "firstDataFrame: " << firstDataFrame << "\n" <<
	                  "lastDataFrame: " << lastDataFrame << "\n" <<
	                  "maxInterpolation: " << maxInterpolation  << "\n"<<
				      "scaleFactor: " << scaleFactor << "\n" <<
	                  "dataStartBlockNumber: " << dataStartBlockNumber  << "\n" <<
	                  "numberOfAnalogSamplesPerFrame: " << numberOfAnalogSamplesPerFrame <<  "\n" <<
	                  "frameRate: " << frameRate << "\n" <<
	                  "labelAndRangeDataIsPresent: " << labelAndRangeDataIsPresent  << "\n" <<
	                  "fourOrTwoLettersLabels: " << fourOrTwoLettersLabels << "\n" <<
	                  "numberOfEvents: " << numberOfEvents << "\n";

	for(int i=0; i < numberOfEvents; i++){

		std::cout << eventLabels[i] << " " << eventStatus[i] << " " << eventTimes[i] << "\n";

	}

	return "";

}