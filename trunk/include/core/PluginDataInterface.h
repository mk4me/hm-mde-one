#ifndef PLUGIN_DATA_INTERFACE_H
#define PLUGIN_DATA_INTERFACE_H

#include "Enumerators.h"
#include <string>

class IPluginData
{
public:
    virtual void InicializeData() = 0;
    virtual POSITION GetPosition() = 0;
    virtual std::string GetName() = 0;
    virtual std::string GetType() = 0;

protected:
private:
};



#endif //PLUGIN_DATA_INTERFACE_H
