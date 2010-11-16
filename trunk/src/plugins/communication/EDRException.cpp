/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/EDRException.h>

using namespace communication;

void EDRException::log(const std::string& information) {
	std::ofstream stream;
	time_t date_now;
	tm* t;
	std::string date_format;

	time(&date_now);
	t = localtime(&date_now);
	date_format = asctime(t);
	date_format = date_format.substr(0, date_format.length() - 1);

	stream.open("log.txt", std::fstream::app);
	stream << "[" << date_format << "] " << information << "\n";
	stream.close();
}

EDRException::EDRException() {
	exception = "An exception occurred.";
}

EDRException::EDRException(const std::string& exception) {
	this->exception = exception;
}

EDRException::~EDRException() {
}

void EDRException::setExceptionMessage(const std::string& exception) {
	this->exception = exception;
}

const std::string& EDRException::getExceptionMessage() const {
	return this->exception;
}
