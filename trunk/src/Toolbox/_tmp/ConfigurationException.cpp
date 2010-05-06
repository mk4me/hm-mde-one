
#include "ConfigurationException.h"

//-----------------------------------------
//static exceptions
//-----------------------------------------
const std::string ConfigurationException::NO_OPTION = "Cannot find option in section.";
const std::string ConfigurationException::NO_SECTION = "Cannot find section in configuration.";
const std::string ConfigurationException::FAIL_OPEN_FILE = "Cannot open file.";
const std::string ConfigurationException::BAD_SECTION = "Section is bad marked. No ']' at the end.";
const std::string ConfigurationException::OPTION_WITHOUT_SECTION = "Line with option is before section line.";
const std::string ConfigurationException::SECTION_ALREADY_EXISTS = "Section you want to add already exists.";
const std::string ConfigurationException::SECTION_DOESNT_EXIST = "Section you want to delete doesn't exist.";
const std::string ConfigurationException::BAD_CONFIG_LINE = "There is bad line in config file.";
const std::string ConfigurationException::MAP_NOT_EMPTY = "Map is not empty.";

//-----------------------------------------
//constructor of an exception
//-----------------------------------------
ConfigurationException::ConfigurationException(const std::string sError) {
	this->sError = sError;
}

//-----------------------------------------
//destructor
//-----------------------------------------
ConfigurationException::~ConfigurationException(void) {}

//-----------------------------------------
//return error message
//-----------------------------------------
const std::string ConfigurationException::PrintException(void) const {
	return sError;
}
