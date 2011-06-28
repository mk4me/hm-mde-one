
#pragma once
#ifndef CONFIGBUFFER_H
#define CONFIGBUFFER_H

#include "ConfigurationGroup.h"
#include <string>
#include <list>


class ConfigBuffer
{
public:
    ConfigBuffer():m_name("") { m_ConfigurationGroupList.clear(); };
    ~ConfigBuffer() {};

    ConfigurationGroup* GetConfigurationGroup(std::string name);
    void UpdateConfigurationGroup(ConfigurationGroup& cGroup);


    void AddNewConfigurationGroup(std::string name);
    void AddNewConfigurationGroup(ConfigurationGroup &cGroup);
    void AddNewConfigurationGroup(ConfigurationGroup *cGroup);

    void DeleteConfigurationGroup(std::string name);
    void DeleteConfigurationGroup(ConfigurationGroup& cGroup);

    bool IsExist(std::string name);

    std::string GetName();
    void SetName(std::string name);
    void Clear();

    std::list<ConfigurationGroup*>& GetConfigurationGroupList();

private:
    std::string m_name;
    std::list<ConfigurationGroup*> m_ConfigurationGroupList;

};


#endif // CONFIGBUFFER_H