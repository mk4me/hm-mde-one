/**
@author Marek Daniluk
*/

#include "Session.h"

using namespace EDRCM;

Session::Session() {
	id = 0;
	motionKindID = 0;
	performerID = 0;
	sessionDate;
	sessionDescription = "";
}

Session::Session(int sessionID, int motionKindID, int performerID, DateTime& sessionDate, const std::string& sessionDescription) {
	this->id = sessionID;
	this->motionKindID = motionKindID;
	this->performerID = performerID;
	this->sessionDate = sessionDate;
	this->sessionDescription = sessionDescription;
}

Session::~Session() { }

void Session::setmotionKindID(int motionKindID) {
	this->motionKindID = motionKindID;
}

void Session::setPerformerID(int performerID) {
	this->performerID = performerID;
}

void Session::setSessionDate(DateTime sessionDate) {
	this->sessionDate = sessionDate;
}

void Session::setSessionDescription(const std::string& sessionDescription) {
	this->sessionDescription = sessionDescription;
}

const int Session::getmotionKindID() const {
	return motionKindID;
}

const int Session::getPerformerID() const {
	return performerID;
}

const DateTime& Session::getSessionDate() const {
	return sessionDate;
}

const std::string& Session::getSessionDescription() const {
	return sessionDescription;
}

const std::string Session::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->sessionDescription << " [" << this->sessionDate << "]";
	return sstream.str();
}

std::vector<Trial>& Session::getTrials()
{
	return this->trials;
}

void Session::setTrials(std::vector<Trial>& trials)
{
	this->trials = trials;
}
