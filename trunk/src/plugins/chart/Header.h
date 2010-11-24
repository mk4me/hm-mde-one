#ifndef _MARKERHEADER_DEF_
#define _MARKERHEADER_DEF_

#include <string>
#include "ProcessorReader.h"
#include <iostream>
#include <fstream>
#include <bitset>
#include <limits>

/*

Class responsible for holding data from the header section of c3d file. 
It contains mirrored values from the most important values from the parameters section.

*/
class MarkerHeader {

private:

	unsigned char paramStartBlock;
	int numberOfC3DPoints;
	int numberOfAllAnalogMesPerFrame;
	int firstDataFrame;
	int lastDataFrame;
	int maxInterpolation;

	float scaleFactor;

	int dataStartBlockNumber;
	unsigned short numberOfAnalogSamplesPerFrame;
	float frameRate;

	bool labelAndRangeDataIsPresent;
	bool fourOrTwoLettersLabels;

	int numberOfEvents;
	float* eventTimes;
	std::string* eventLabels;
	bool* eventStatus;

	ProcessorReader* proc;

public:

	MarkerHeader(ProcessorReader* proc);

	unsigned char getParamStartBlock() { return paramStartBlock; }
	int getNumberOfC3DPoints() { return numberOfC3DPoints; }
	int getNumberOfAllAnalogMesPerFrame(){ return numberOfAllAnalogMesPerFrame; }
	int getFirstDataFrame(){ return firstDataFrame; }
	int getLastDataFrame(){ return lastDataFrame; }
	int getMaxInterpolation(){ return maxInterpolation; }

	float getScaleFactor(){ return scaleFactor; }
	int getdDtaStartBlockNumber(){ return dataStartBlockNumber; }
	int getNumberOfAnalogSamplesPerFrame(){ return numberOfAnalogSamplesPerFrame; }
	float getFrameRate(){ return frameRate; }

	bool getLabelAndRangeDataIsPresent(){ return labelAndRangeDataIsPresent; }
	bool getFourOrTwoLettersLabels(){ return fourOrTwoLettersLabels; }

	int getNumberOfEvents(){ return numberOfEvents; }
	float* getEventTimes(){ return eventTimes; }
	std::string* getEventLabels(){ return eventLabels; }
	bool* getEventStatus(){ return eventStatus; }

	///////////////////////////////////////

	bool parseHeader();
	void saveHeader(std::ofstream& out);
	~MarkerHeader();

	/////////////////////////////////

	std::string toString();

};

#endif //_MARKERHEADER_DEF_