/**
@author Marek Daniluk
*/

#include "SessionGroup.h"

using namespace EDRCM;

SessionGroup::SessionGroup(int sessionGroupID, const std::string& sessionGroupName) :
Entity(sessionGroupID), sessionGroupName(sessionGroupName) { }

SessionGroup::~SessionGroup() { }

void SessionGroup::setSessionGroupName(const std::string& sessionGroupName) {
	this->sessionGroupName = sessionGroupName;
}

const std::string& SessionGroup::getSessionGroupName() const {
	return this->sessionGroupName;
}

const std::string SessionGroup::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->sessionGroupName;
	return sstream.str();
}
