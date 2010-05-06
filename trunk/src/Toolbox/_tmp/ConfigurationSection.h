#ifndef _CONFIGURATIONSECTION_
#define _CONFIGURATIONSECTION_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "ConfigurationException.h"

class ConfigurationSection {
public:
	//constructors
	ConfigurationSection(void);
	ConfigurationSection(const std::string &sectionName);
	ConfigurationSection(const ConfigurationSection &confSection);

	//destructor
	virtual ~ConfigurationSection(void);

	//operators
	ConfigurationSection& operator= (const ConfigurationSection &confSection);
	friend const ConfigurationSection operator+ (const ConfigurationSection &LconfSection, const ConfigurationSection &RconfSection);

	//adding maps to existing maps
	void addMap(const std::map<std::string, std::string> &stringMap);
	void addMap(const std::map<std::string, int> &intMap);
	void addMap(const std::map<std::string, float> &floatMap);
	void addMap(const std::map<std::string, bool> &boolMap);

	//setters
	void setSectionName(const std::string &sectionName);
	void setValue(const std::string &optionName, const std::string &optionValue);
	void setValue(const std::string &optionName, const int &optionValue);
	void setValue(const std::string &optionName, const float &optionValue);
	void setValue(const std::string &optionName, const bool &optionValue);
	void setMap(const std::map<std::string, std::string> &stringMap);
	void setMap(const std::map<std::string, int> &intMap);
	void setMap(const std::map<std::string, float> &floatMap);
	void setMap(const std::map<std::string, bool> &boolMap);

	//getters
	const std::string& getSectionName(void) const;
	const std::string& getStringValue(const std::string &optionName);
	const int getIntValue(const std::string &optionName);
	const float getFloatValue(const std::string &optionName);
	const bool getBoolValue(const std::string &optionName);
	const std::map<std::string, std::string>& getStringMap(void) const;
	const std::map<std::string, int>& getIntMap(void) const;
	const std::map<std::string, float>& getFloatMap(void) const;
	const std::map<std::string, bool>& getBoolMap(void) const;

protected:
	//section name
	std::string sectionName;

	//conteners with properties/values
	std::map<std::string, std::string> stringMap;
	std::map<std::string, int> intMap;
	std::map<std::string, float> floatMap;
	std::map<std::string, bool> boolMap;
};

#endif
