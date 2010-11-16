/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/MotionKind.h>

using namespace communication;

MotionKind::MotionKind(int motionKindID, const std::string& motionKindName) :
Entity(motionKindID), motionKindName(motionKindName) { }

MotionKind::~MotionKind() { }

void MotionKind::setMotionKindName(const std::string& motionKindName) {
	this->motionKindName = motionKindName;
}

const std::string& MotionKind::getMotionKindName() const {
	return this->motionKindName;
}

const std::string MotionKind::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->motionKindName;
	return sstream.str();
}
