#include "ConfigBuffer.h"


//------------------------------------------------------------------------------------------------
ConfigurationGroup* ConfigBuffer::GetConfigurationGroup( std::string name )
{
	for(std::list<ConfigurationGroup*>::iterator it = m_ConfigurationGroupList.begin(); it != m_ConfigurationGroupList.end(); it++)
	{
		if((*it)->GetName() == name)
		{
			return *it;
		}	
	}
	return NULL;
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::UpdateConfigurationGroup(ConfigurationGroup& cGroup)
{
	for(std::list<ConfigurationGroup*>::iterator it = m_ConfigurationGroupList.begin(); it != m_ConfigurationGroupList.end(); it++)
	{
		if(cGroup.GetName() == (*it)->GetName())
		{
			*it = &cGroup;
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::AddNewConfigurationGroup( std::string name )
{
    ConfigurationGroup *cGroup = new ConfigurationGroup(name);
    m_ConfigurationGroupList.push_back(cGroup);
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::AddNewConfigurationGroup(ConfigurationGroup& cGroup)
{
	m_ConfigurationGroupList.push_back(&cGroup);
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::AddNewConfigurationGroup( ConfigurationGroup *cGroup )
{
    m_ConfigurationGroupList.push_back(cGroup);
}
//------------------------------------------------------------------------------------------------
void ConfigBuffer::DeleteConfigurationGroup( std::string name )
{
	for(std::list<ConfigurationGroup*>::iterator it = m_ConfigurationGroupList.begin(); it != m_ConfigurationGroupList.end(); it++)
	{
		if((*it)->GetName() == name)
		{
			m_ConfigurationGroupList.erase(it);
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::DeleteConfigurationGroup( ConfigurationGroup& cGroup )
{
	for(std::list<ConfigurationGroup*>::iterator it = m_ConfigurationGroupList.begin(); it != m_ConfigurationGroupList.end(); it++)
	{
		if((*it)->GetName() == cGroup.GetName())
		{
			m_ConfigurationGroupList.erase(it);
			break;
		}
	}
}

//------------------------------------------------------------------------------------------------
bool ConfigBuffer::IsExist( std::string name )
{
	for(std::list<ConfigurationGroup*>::iterator it = m_ConfigurationGroupList.begin(); it != m_ConfigurationGroupList.end(); it++)
	{
		if((*it)->GetName() == name)
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------
std::string ConfigBuffer::GetName()
{
    return m_name;
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::SetName( std::string name )
{
    m_name = name;
}

//------------------------------------------------------------------------------------------------
std::list<ConfigurationGroup*>& ConfigBuffer::GetConfigurationGroupList()
{
    return m_ConfigurationGroupList;
}

//------------------------------------------------------------------------------------------------
void ConfigBuffer::Clear()
{
    m_ConfigurationGroupList.clear();
}
