/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/Segment.h>

using namespace communication;

Segment::Segment(int segmentID, int trialID, const std::string& segmentName, int startTime, int endTime):
Entity(segmentID), trialID(trialID), segmentName(segmentName), startTime(startTime), endTime(endTime) { }

Segment::~Segment() { }

void Segment::setTrialID(int trialID) {
	this->trialID = trialID;
}

void Segment::setSegmentName(const std::string& segmentName) {
	this->segmentName = segmentName;
}

void Segment::setStartTime(int startTime) {
	this->startTime = startTime;
}

void Segment::setEndTime(int endTime) {
	this->endTime = endTime;
}

const int Segment::getTrialID() const {
	return this->trialID;
}

const std::string& Segment::getSegmentName() const {
	return this->segmentName;
}

const int Segment::getStartTime() const {
	return this->startTime;
}

const int Segment::getEndTime() const {
	return this->endTime;
}

const std::string Segment::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->segmentName << " (" << this->startTime << "-" << this->endTime << ")";
	return sstream.str();
}
