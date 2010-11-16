/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/Trial.h>

using namespace communication;

Trial::Trial(int trialID, const std::string& trialDescription, int duration) :
Entity(trialID), trialDescription(trialDescription), duration(duration) { }

Trial::~Trial() { }

void Trial::setTrialDescription(const std::string& trialDescription) {
	this->trialDescription = trialDescription;
}

void Trial::setDuration(int duration) {
	this->duration = duration;
}

const std::string& Trial::getTrialDescription() const {
	return this->trialDescription;
}

const int Trial::getDuration() const {
	return this->duration;
}

const std::string Trial::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->trialDescription << " (" << this->duration << ")";
	return sstream.str();
}

std::vector<File>& Trial::getFiles()
{
	return this->files;
}

void Trial::setFiles(std::vector<File>& files)
{
	this->files = files;
}
