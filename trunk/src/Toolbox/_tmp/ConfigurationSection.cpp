#include "ConfigurationSection.h"

/*
//Constructors
*/

ConfigurationSection::ConfigurationSection(void) {
	sectionName = "";
}

ConfigurationSection::ConfigurationSection(const std::string &sectionName) {
	this->sectionName = sectionName;
}

ConfigurationSection::ConfigurationSection(const ConfigurationSection &confSection) {
	sectionName = confSection.getSectionName();
	//clear all maps
	stringMap.clear();
	intMap.clear();
	floatMap.clear();
	boolMap.clear();
	//copy maps
	this->addMap(confSection.getStringMap());
	this->addMap(confSection.getIntMap());
	this->addMap(confSection.getFloatMap());
	this->addMap(confSection.getBoolMap());
}

/*
//Destructor
*/

ConfigurationSection::~ConfigurationSection(void) {
	stringMap.clear();
	intMap.clear();
	floatMap.clear();
	boolMap.clear();
}

/*
//Operators
*/

ConfigurationSection& ConfigurationSection::operator= (const ConfigurationSection &confSection) {
	if (this == &confSection) {
		return *this;
	}
	sectionName = confSection.getSectionName();
	//clear all maps
	stringMap.clear();
	intMap.clear();
	floatMap.clear();
	boolMap.clear();
	//copy maps
	this->addMap(confSection.getStringMap());
	this->addMap(confSection.getIntMap());
	this->addMap(confSection.getFloatMap());
	this->addMap(confSection.getBoolMap());
	return *this;
}

const ConfigurationSection operator+ (const ConfigurationSection &LconfSection, const ConfigurationSection &RconfSection) {
	ConfigurationSection confSection;

	confSection.setSectionName("Unnamed");
	//adding string maps
	confSection.addMap(LconfSection.getStringMap());
	confSection.addMap(RconfSection.getStringMap());
	//adding int maps
	confSection.addMap(LconfSection.getIntMap());
	confSection.addMap(RconfSection.getIntMap());
	//adding float maps
	confSection.addMap(LconfSection.getFloatMap());
	confSection.addMap(RconfSection.getFloatMap());
	//adding bool maps
	confSection.addMap(LconfSection.getBoolMap());
	confSection.addMap(RconfSection.getBoolMap());

	return confSection;
}

/*
//Adding maps
*/

void ConfigurationSection::addMap(const std::map<std::string, std::string> &stringMap) {
	this->stringMap.insert(stringMap.begin(), stringMap.end());
}

void ConfigurationSection::addMap(const std::map<std::string, int> &intMap) {
	this->intMap.insert(intMap.begin(), intMap.end());
}

void ConfigurationSection::addMap(const std::map<std::string, float> &floatMap) {
	this->floatMap.insert(floatMap.begin(), floatMap.end());
}

void ConfigurationSection::addMap(const std::map<std::string, bool> &boolMap) {
	this->boolMap.insert(boolMap.begin(), boolMap.end());
}

/*
//Setters
*/

void ConfigurationSection::setSectionName(const std::string &sectionName) {
	this->sectionName = sectionName;
}

void ConfigurationSection::setValue(const std::string &optionName, const std::string &optionValue) {
	stringMap[optionName] = optionValue;
}

void ConfigurationSection::setValue(const std::string &optionName, const int &optionValue) {
	intMap[optionName] = optionValue;
}

void ConfigurationSection::setValue(const std::string &optionName, const float &optionValue) {
	floatMap[optionName] = optionValue;
}

void ConfigurationSection::setValue(const std::string &optionName, const bool &optionValue) {
	boolMap[optionName] = optionValue;
}

void ConfigurationSection::setMap(const std::map<std::string, std::string> &stringMap) {
	if(this->stringMap.size() > 0) {
		//exception, not empty properties map
		throw ConfigurationException(ConfigurationException::MAP_NOT_EMPTY);
	}
	addMap(stringMap);
}

void ConfigurationSection::setMap(const std::map<std::string, int> &intMap) {
	if(this->intMap.size() > 0) {
		//exception, not empty properties map
		throw ConfigurationException(ConfigurationException::MAP_NOT_EMPTY);
	}
	addMap(intMap);
}

void ConfigurationSection::setMap(const std::map<std::string, float> &floatMap) {
	if(this->floatMap.size() > 0) {
		//exception, not empty properties map
		throw ConfigurationException(ConfigurationException::MAP_NOT_EMPTY);
	}
	addMap(floatMap);
}

void ConfigurationSection::setMap(const std::map<std::string, bool> &boolMap) {
	if(this->boolMap.size() > 0) {
		//exception, not empty properties map
		throw ConfigurationException(ConfigurationException::MAP_NOT_EMPTY);
	}
	addMap(boolMap);
}

/*
//Getters
*/

const std::string& ConfigurationSection::getSectionName(void) const {
	return sectionName;
}

const std::string& ConfigurationSection::getStringValue(const std::string &optionName) {
	std::map<std::string, std::string>::iterator it;

	it = stringMap.find(optionName);
	if(it == stringMap.end()) {
		throw ConfigurationException(ConfigurationException::NO_OPTION);
	}
	return it->second;
}

const int ConfigurationSection::getIntValue(const std::string &optionName) {
	std::map<std::string, int>::iterator it;

	it = intMap.find(optionName);
	if(it == intMap.end()) {
		throw ConfigurationException(ConfigurationException::NO_OPTION);
	}
	return it->second;
}

const float ConfigurationSection::getFloatValue(const std::string &optionName) {
	std::map<std::string, float>::iterator it;

	it = floatMap.find(optionName);
	if(it == floatMap.end()) {
		throw ConfigurationException(ConfigurationException::NO_OPTION);
	}
	return it->second;
}

const bool ConfigurationSection::getBoolValue(const std::string &optionName) {
	std::map<std::string, bool>::iterator it;

	it = boolMap.find(optionName);
	if(it == boolMap.end()) {
		throw ConfigurationException(ConfigurationException::NO_OPTION);
	}
	return it->second;
}

const std::map<std::string, std::string>& ConfigurationSection::getStringMap(void) const {
	return stringMap;
}

const std::map<std::string, int>& ConfigurationSection::getIntMap(void) const {
	return intMap;
}

const std::map<std::string, float>& ConfigurationSection::getFloatMap(void) const {
	return floatMap;
}

const std::map<std::string, bool>& ConfigurationSection::getBoolMap(void) const {
	return boolMap;
}
