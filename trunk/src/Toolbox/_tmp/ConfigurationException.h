#ifndef _CONFIGURATIONEXCEPTIONS_
#define _CONFIGURATIONEXCEPTIONS_

#include <iostream>
#include <string>
#include "ConfigurationException.h"

class ConfigurationException {
public:

	//-----------------------------------------
	//static exceptions
	//-----------------------------------------
	static const std::string NO_OPTION;
	static const std::string NO_SECTION;
	static const std::string FAIL_OPEN_FILE;
	static const std::string BAD_SECTION;
	static const std::string OPTION_WITHOUT_SECTION;
	static const std::string SECTION_ALREADY_EXISTS;
	static const std::string SECTION_DOESNT_EXIST;
	static const std::string BAD_CONFIG_LINE;
	static const std::string MAP_NOT_EMPTY;

	//-----------------------------------------
	//constructor of an exception
	//-----------------------------------------
	ConfigurationException(const std::string sError);

	//-----------------------------------------
	//return error message
	//-----------------------------------------
	const std::string PrintException(void) const;

	//-----------------------------------------
	//destructor
	//-----------------------------------------
	virtual ~ConfigurationException(void);
protected:

	//-----------------------------------------
	//error
	//-----------------------------------------
	std::string sError;
};

#endif