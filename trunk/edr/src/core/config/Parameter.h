#pragma once
#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>

class Parameter
{
public:
    Parameter():m_name(""), m_value("") {};
	Parameter(std::string name, std::string val):m_name(name), m_value(val) {};
    ~Parameter() {};

    std::string GetName();
    std::string GetValue();

    void SetName(std::string name);
    void SetValue(std::string val);

    void Clear();

private:
    std::string m_name;
    std::string m_value;
};

#endif // PARAMETER_H