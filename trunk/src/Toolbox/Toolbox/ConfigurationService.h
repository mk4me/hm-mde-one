#ifndef CONFIGURATION_SERVICE_H
#define CONFIGURATION_SERVICE_H

#include <string>
#include <vector>

struct lua_State; 

class ConfigurationEntry
{
public: 
  ConfigurationEntry(); 
  virtual ~ConfigurationEntry(); 
};

class ConfigurationTable: ConfigurationEntry
{
public: 
  ConfigurationTable(void); 
  virtual ~ConfigurationTable(void); 

  
};

class ConfigurationService
{
public:
  ConfigurationService(void);
  virtual ~ConfigurationService(void);

  bool loadConfiguration(std::string fileName); 
  void parseTable(lua_State *L, int idx);
};

#endif