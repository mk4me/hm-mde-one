#pragma once
#ifndef CONFIG_PARAMETER_H
#define CONFIG_PARAMETER_H

#include <string>

class ConfigParameter
{
public:
    ConfigParameter():m_name(""), m_value("") {};
	ConfigParameter(std::string name, std::string val):m_name(name), m_value(val) {};
    ~ConfigParameter() {};

    std::string GetName();
    std::string GetValue();

    void SetName(std::string name);
    void SetValue(std::string val);

    void Clear();

private:
    std::string m_name;
    std::string m_value;
};

#endif // CONFIG_PARAMETER_H