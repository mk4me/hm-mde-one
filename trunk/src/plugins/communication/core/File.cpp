/**
@author Marek Daniluk
*/

#include "File.h"

using namespace EDRCM;

File::File(int fileID, const std::string& filename, const std::string& fileDescription) :
Entity(fileID), filename(filename), fileDescription(fileDescription) { }

File::~File() { }

void File::setFilename(const std::string& filename) {
	this->filename = filename;
}

void File::setFileDescription(const std::string& fileDescription) {
	this->fileDescription = fileDescription;
}

const std::string& File::getFilename() const {
	return this->filename;
}

const std::string& File::getFileDescription() const {
	return this->fileDescription;
}

const std::string File::toString() const {
	std::ostringstream sstream;
	sstream << this->id << ": " << this->filename << " (" << this->fileDescription << ")";
	return sstream.str();
}
