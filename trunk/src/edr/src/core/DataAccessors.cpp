#include "CorePCH.h"
#include "core/DataAccessors.h"
#include "DataManager.h"

namespace core {

    /*ObjectWrapperPtr __DataAccessorHelper::getWrapper(void * ptr)
    {
        return DataManager::getInstance()->getWrapper(ptr);
    }*/

    /*void queryData(ObjectWrapperCollection & collection)
    {
        DataManager::getInstance()->getObjects(collection);
    }

    Objects queryData(const TypeInfo & type, bool exact)
    {
        Objects ret;
        DataManager::getInstance()->getObjects(ret, type, exact);

        return ret;
    }

    bool isTypeRegistered(const TypeInfo & type)
    {
        return DataManager::getInstance()->getSupportedTypes().find(type) != DataManager::getInstance()->getSupportedTypes().end();
    }

    const TypeInfoSet & getApplicationRegisteredTypes()
    {
        return DataManager::getInstance()->getSupportedTypes();
    }

    const TypeInfoSet & getTypeBaseTypes(const TypeInfo & type)
    {
        return DataManager::getInstance()->getTypeBaseTypes(type);
    }

    const TypeInfoSet & getTypeDerrivedTypes(const TypeInfo & type)
    {
        return DataManager::getInstance()->getTypeDerrivedTypes(type);
    }*/

}