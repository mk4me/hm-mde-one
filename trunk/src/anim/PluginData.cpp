#include "PluginData.h"



//--------------------------------------------------------------------------------------------------
PluginData::PluginData()
{

}

//--------------------------------------------------------------------------------------------------
PluginData::~PluginData()
{

}

//--------------------------------------------------------------------------------------------------
void PluginData::InicializeData()
{

}

//--------------------------------------------------------------------------------------------------
POSITION PluginData::GetPosition()
{
    if(_position)
        return _position;

    return RIGHT;
}

//--------------------------------------------------------------------------------------------------
std::string PluginData::GetName()
{
    return _name;
}

//--------------------------------------------------------------------------------------------------
std::string PluginData::GetType()
{
    return _type;
}