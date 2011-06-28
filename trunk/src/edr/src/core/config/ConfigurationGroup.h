#pragma once
#ifndef CONFIGURATIONGROUP_H
#define CONFIGURATIONGROUP_H

#include <string>
#include <list>
#include "ConfigParameter.h"

#include <list>
#include <string>

class ConfigurationGroup
{
public:
    ConfigurationGroup():m_name("") { m_ParameterList.clear(); };
    ConfigurationGroup(std::string name):m_name(name) { m_ParameterList.clear(); };
    ~ConfigurationGroup() {};

    bool GetParametrVal(std::string pName, std::string &val);
    void SetParametrVal(std::string pName, std::string val);

    void AddNewParametr(std::string name, std::string val);
    void AddNewParamter(ConfigParameter& parameter);

    void DeleteParametr(std::string name);
    void DeleteParametr(ConfigParameter& parametr);

    bool IsExistParameter(std::string pName);

    std::string GetName();
    void SetName(std::string name);

    ConfigParameter* GetParameter(std::string pName);

    std::list<ConfigParameter*>& GetParametersList();
    void Clear();

private:
    std::string m_name;
    std::list<ConfigParameter*> m_ParameterList;
};

#endif // CONFIGURATIONGROUP_H