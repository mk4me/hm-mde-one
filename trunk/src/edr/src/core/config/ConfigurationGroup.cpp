#include "ConfigurationGroup.h"




//------------------------------------------------------------------------------------------------
bool ConfigurationGroup::GetParametrVal( std::string pName, std::string &val)
{
    for(std::list<ConfigParameter*>::iterator it = m_ParameterList.begin(); it != m_ParameterList.end(); it++)
    {
        if((*it)->GetName() == pName)
        {
            val = (*it)->GetValue();
            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------------------------
void ConfigurationGroup::SetParametrVal( std::string pName, std::string val )
{
    for(std::list<ConfigParameter*>::iterator it = m_ParameterList.begin(); it != m_ParameterList.end(); it++)
    {
        if((*it)->GetName() == pName)
        {
            (*it)->SetValue(val);
        }
    }
}

//------------------------------------------------------------------------------------------------
void ConfigurationGroup::AddNewParametr( std::string name, std::string val )
{
	ConfigParameter *parameter = new ConfigParameter(name, val);
	m_ParameterList.push_back(parameter);
}

//------------------------------------------------------------------------------------------------
void ConfigurationGroup::AddNewParamter(ConfigParameter& parameter)
{
    m_ParameterList.push_back(&parameter);
}

//------------------------------------------------------------------------------------------------
void ConfigurationGroup::DeleteParametr( std::string name )
{
    for(std::list<ConfigParameter*>::iterator it = m_ParameterList.begin(); it != m_ParameterList.end(); it++)
    {
        if((*it)->GetName() == name)
        {
            m_ParameterList.erase(it);
            break;
        }
    }
}

//------------------------------------------------------------------------------------------------
void ConfigurationGroup::DeleteParametr( ConfigParameter& parametr )
{
    for(std::list<ConfigParameter*>::iterator it = m_ParameterList.begin(); it != m_ParameterList.end(); it++)
    {
        if((*it)->GetName() == parametr.GetName())
        {
            m_ParameterList.erase(it);
            break;
        }
    }
}

//------------------------------------------------------------------------------------------------
bool ConfigurationGroup::IsExistParameter(std::string pName)
{
    for (std::list<ConfigParameter*>::iterator it = m_ParameterList.begin(); it != m_ParameterList.end(); it++)
    {
        if((*it)->GetName() == pName)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------
std::string ConfigurationGroup::GetName()
{
    return m_name;
}

//------------------------------------------------------------------------------------------------
void ConfigurationGroup::SetName(std::string name)
{
    m_name = name;
}

//------------------------------------------------------------------------------------------------
std::list<ConfigParameter*>& ConfigurationGroup::GetParametersList()
{
    return m_ParameterList;
}

//------------------------------------------------------------------------------------------------
ConfigParameter* ConfigurationGroup::GetParameter( std::string pName )
{
    for (std::list<ConfigParameter*>::iterator it = m_ParameterList.begin(); it != m_ParameterList.end(); it++)
    {
        if((*it)->GetName() == pName)
        {
            return (*it);
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------
void ConfigurationGroup::Clear()
{

}