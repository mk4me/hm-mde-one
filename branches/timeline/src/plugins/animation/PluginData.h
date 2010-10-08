#ifndef PLUGIN_DATA_H
#define PLUGIN_DATA_H

#include <core/PluginDataInterface.h>

class PluginData: public IPluginData
{
public:
    PluginData();
    ~PluginData();

    virtual void InicializeData();
    virtual POSITION GetPosition();
    virtual std::string GetName();
    virtual std::string GetType();

protected:
private:
    POSITION _position;
    std::string _name;
    std::string _type;
};

#endif //PLUGIN_DATA_H
