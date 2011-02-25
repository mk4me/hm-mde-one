#ifndef _MARKERHEADER_DEF_
#define _MARKERHEADER_DEF_

#include <string>
#include <plugins/chart/ProcessorReader.h>
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

	unsigned char getParamStartBlock() const { return paramStartBlock; }
	int getNumberOfC3DPoints() const { return numberOfC3DPoints; }
	int getNumberOfAllAnalogMesPerFrame() const { return numberOfAllAnalogMesPerFrame; }
	int getFirstDataFrame() const { return firstDataFrame; }
	int getLastDataFrame() const { return lastDataFrame; }
	int getMaxInterpolation() const { return maxInterpolation; }

	float getScaleFactor() const { return scaleFactor; }
	int getdDtaStartBlockNumber() const { return dataStartBlockNumber; }
	int getNumberOfAnalogSamplesPerFrame() const { return numberOfAnalogSamplesPerFrame; }
	float getFrameRate() const { return frameRate; }

	bool getLabelAndRangeDataIsPresent() const { return labelAndRangeDataIsPresent; }
	bool getFourOrTwoLettersLabels() const { return fourOrTwoLettersLabels; }

	int getNumberOfEvents() const { return numberOfEvents; }
	float* getEventTimes() const { return eventTimes; }
	std::string* getEventLabels() const { return eventLabels; }
	bool* getEventStatus() const { return eventStatus; }

	///////////////////////////////////////

	bool parseHeader();
	void saveHeader(std::ofstream& out);
	~MarkerHeader();

	/////////////////////////////////

	std::string toString();

};

#endif //_MARKERHEADER_DEF_