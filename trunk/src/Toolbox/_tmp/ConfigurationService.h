#ifndef _CONFIGURATIONSERVICE_
#define _CONFIGURATIONSERVICE_

#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include "ConfigurationSection.h"

class ConfigurationService {
public:
	//constructors
	ConfigurationService(void);
	ConfigurationService(const std::string &fileName);
	ConfigurationService(const ConfigurationService &confService);

	//destructor
	virtual ~ConfigurationService(void);

	//operators
	ConfigurationService& operator= (const ConfigurationService &confService);
	friend const ConfigurationService operator+ (const ConfigurationService &LconfService, const ConfigurationService &RconfService);

	//loading ini configuration files
	bool loadConfiguration(const std::string &fileName);

	//saving ini configuration file
	bool saveConfiguration(void) const;

	//savinig as fileName ini configuration file
	bool saveConfiguration(const std::string &fileName) const;

	//getters
	const ConfigurationSection& getSection(const std::string &sectionName) const;
	const std::map<std::string, ConfigurationSection>& getSections(void) const;
	const std::string& getFileName(void) const;

	//add new section
	void addSection(const ConfigurationSection &confSection);
	//add sections
	void addSections(const std::map<std::string, ConfigurationSection> sections);

	//delete section
	void deleteSection(const std::string &sectionName);

	//setters
	void setFileName(const std::string &fileName);
	void setSections(const std::map<std::string, ConfigurationSection> sections);

protected:
	//container for all sections
	std::map<std::string, ConfigurationSection> sections;

	//last section name
	std::string lastSectionName;

	//name of ini file
	std::string fileName;

	//prasing line for sections/properties
	bool parseLine(const std::string &line);

	//value type
	bool isInt(const std::string &line) const;
	bool isFloat(const std::string &line) const;
	bool isBool(const std::string &line) const;

	//trim (delete spaces from begining and ending)
	std::string& Trim(std::string &line) const;
};

#endif
