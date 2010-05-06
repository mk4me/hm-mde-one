#include "ConfigurationService.h"

/*
//Constructors
*/

ConfigurationService::ConfigurationService(void) {
	fileName = "";
}

ConfigurationService::ConfigurationService(const std::string &fileName) {
	this->fileName = fileName;
}

ConfigurationService::ConfigurationService(const ConfigurationService &confService) {
	fileName = confService.getFileName();
	//clear sections
	sections.clear();
	//set sections
	this->setSections(confService.getSections());
}

/*
//Destructor
*/

ConfigurationService::~ConfigurationService(void) {
	sections.clear();
}

/*
//Operators
*/

ConfigurationService& ConfigurationService::operator= (const ConfigurationService &confService) {
	if(this == &confService) {
		return *this;
	}
	fileName = confService.getFileName();
	//clear sections
	sections.clear();
	//set sections
	this->setSections(confService.getSections());
	return *this;
}

const ConfigurationService operator+ (const ConfigurationService &LconfService, const ConfigurationService &RconfService) {
	ConfigurationService confService;

	confService.setFileName("Unnamed");
	confService.addSections(LconfService.getSections());
	confService.addSections(RconfService.getSections());
	return confService;
}

bool ConfigurationService::loadConfiguration(const std::string &fileName) {
	std::string line;
	std::ifstream file(fileName.c_str());

	if(!file.is_open()) {
	//file cannot be opened
		throw ConfigurationException(ConfigurationException::FAIL_OPEN_FILE);
	}

	while(!file.eof()) {
	//reading file
		std::getline(file, line);
		//line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
		//remove " chars
		line.erase(std::remove(line.begin(), line.end(), '"'), line.end());
		//parse file
		parseLine(line);
	}
	file.close();
	return true;
}

//bool ConfigurationService::saveConfiguration(void) const;
//bool ConfigurationService::saveConfiguration(const std::string &fileName) const;

const ConfigurationSection& ConfigurationService::getSection(const std::string &sectionName) const {
	std::map<std::string, ConfigurationSection>::iterator it;
	std::map<std::string, ConfigurationSection> temp;
	temp.insert(sections.begin(), sections.end());

	it = temp.find(sectionName);
	if(it == temp.end()) {
		throw ConfigurationException(ConfigurationException::NO_SECTION);
	}
	return (*it).second;
}

const std::map<std::string, ConfigurationSection>& ConfigurationService::getSections(void) const {
	return sections;
}

const std::string& ConfigurationService::getFileName(void) const {
	return fileName;
}

void ConfigurationService::addSection(const ConfigurationSection &confSection) {
	std::map<std::string, ConfigurationSection>::iterator it;

	for(it = sections.begin(); it != sections.end(); ++it) {
		if(confSection.getSectionName() == (*it).second.getSectionName()) {
			throw ConfigurationException(ConfigurationException::SECTION_ALREADY_EXISTS);
		}	
	}
	//add section
	sections[confSection.getSectionName()] = confSection;
}

void ConfigurationService::addSections(const std::map<std::string, ConfigurationSection> sections) {
	this->sections.insert(sections.begin(), sections.end());
}

void ConfigurationService::deleteSection(const std::string &sectionName) {
	std::map<std::string, ConfigurationSection>::iterator it;
	bool exc = true;

	for(it = sections.begin(); it != sections.end(); ++it) {
		if((*it).second.getSectionName() == sectionName) {
			//delete section
			sections.erase(it);
			exc = false;
			break;
		}	
	}
	if(exc) {
		throw ConfigurationException(ConfigurationException::SECTION_DOESNT_EXIST);
	}
}

void ConfigurationService::setFileName(const std::string &fileName) {
	this->fileName = fileName;
}

void ConfigurationService::setSections(const std::map<std::string, ConfigurationSection> sections) {
	if(this->sections.size() > 0) {
		//exception, not empty properties map
		throw ConfigurationException(ConfigurationException::MAP_NOT_EMPTY);
	}
	addSections(sections);
}

bool ConfigurationService::parseLine(const std::string &line) {
	char skipLineChars[5] = {'#', '/', '\n', ';', ' '};
	char sectionChars[2] = {'[', ']'};
	std::string sectionProperty, sectionValue;

	for(int i = 0; i < 5; i++) {
	//skip lines with first char from skipLineChars and empty lines
		if(line[0] == skipLineChars[i] || line.size() == 0) {
			return false;
		}
	}
	if(line[0] == sectionChars[0]) {
	//adding new section
		if(line[line.size() - 1] != sectionChars[1]) {
		//bad section marks, there is no ']' char at the end
			throw ConfigurationException(ConfigurationException::BAD_SECTION);
		}
		lastSectionName = line.substr(1, line.size() - 2);
#ifdef _DEBUG
		std::cout << "[S]: " << lastSectionName  << "\n";
#endif
		//create pointer and add instance to container
		ConfigurationSection cS(lastSectionName);
		sections[lastSectionName] = cS;
	} else {
	//looking for properties and values
		if(sections.size() <= 0) {
		//there is no sections, bad ini file?
			throw ConfigurationException(ConfigurationException::OPTION_WITHOUT_SECTION);
		}
		//line with sectionProperty/sectionValue
		int pos = line.find("=");
		if(pos == 0xFFFFFFFF) {
		//bad line
			throw ConfigurationException(ConfigurationException::BAD_CONFIG_LINE);
		} else {
			sectionProperty = line.substr(0, pos);
			sectionValue = line.substr(pos + 1, line.size() - 1);
			//trim
			sectionProperty = Trim(sectionProperty);
			sectionValue = Trim(sectionValue);
#ifdef _DEBUG
			std::cout << "\t[P]: " << sectionProperty << "\n\t[V]: " << sectionValue << "\n";
#endif
			//save values type
			if(isInt(sectionValue)) {
				sections[lastSectionName].setValue(sectionProperty, atoi(sectionValue.c_str()));
				return true;
			}
			if(isFloat(sectionValue)) {
				sections[lastSectionName].setValue(sectionProperty, (float)atof(sectionValue.c_str()));
				return true;
			}
			if(isBool(sectionValue)) {
				bool temp = sectionProperty[0] == 'f' ? false : true;
				sections[lastSectionName].setValue(sectionValue, temp);
				return true;
			}
			sections[lastSectionName].setValue(sectionProperty, sectionValue);
		}
	}
	return true;
}

bool ConfigurationService::isInt(const std::string &line) const {
    std::istringstream iss(line);
    int i;
    iss >> i;
    return iss.eof() && !iss.fail(); 
}

bool ConfigurationService::isFloat(const std::string &line) const {
    std::istringstream iss(line);
    float f;
    iss >> f;
    return iss.eof() && !iss.fail(); 
}

bool ConfigurationService::isBool(const std::string &line) const {
	if(line == "true" || line == "false") {
		return true;
	}
	return false;
}

std::string& ConfigurationService::Trim(std::string &line) const {
	int i = 0;

	while(true) {
		if(line[i] != ' ')
			break;
		line.erase(line.begin(), line.begin() + 1);
		i++;
	}

	i = line.size() - 1;

	while(true) {
		if(line[i] != ' ')
			break;
		line.erase(line.end() - 1, line.end());
		i--;
	}
	return line;
}
