/**
@author Marek Daniluk
*/

#include "Performer.h"

using namespace EDRCM;

Performer::Performer(int performerID, const std::string& name, const std::string& surname):
Entity(performerID), name(name), surname(surname) { }

Performer::~Performer() { }

void Performer::setName(const std::string& name) {
	this->name = name;
}

void Performer::setSurname(const std::string& surname) {
	this->surname = surname;
}

const std::string& Performer::getName() const {
	return name;
}

const std::string& Performer::getSurname() const {
	return surname;
}

const std::string Performer::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->name << " " << this->surname;
	return sstream.str();
}
