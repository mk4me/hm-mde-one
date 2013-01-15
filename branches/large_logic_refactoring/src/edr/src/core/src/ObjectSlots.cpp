#include "CorePCH.h"
#include <core/ILog.h>
#include <core/PluginCommon.h>
#include "ObjectSlots.h"
#include "DataHierarchyManager.h"

using namespace core;

ObjectSlots::ObjectSlots( const SlotsInfo& info ) :
objects(info.size()), info(info)
{

}

ObjectSlots::ObjectSlots( SlotsInfo&& info ) :
objects(info.size()), info(info)
{

}

ObjectSlots::ObjectSlots( const ObjectSlots& objectSlots ) :
objects(objectSlots.objects), info(objectSlots.info)
{

}

bool ObjectSlots::isAssignable( int slotNo, const TypeInfo & type ) const
{
    try{
        return getDataHierarchyManager()->isTypeCompatible(getSlotType(slotNo), type);
    }
    catch(std::runtime_error & e){
        CORE_LOG_ERROR("ObjectSlot: Request for unsupported type: " << type.name() << " " << e.what());
    }
    catch(std::invalid_argument & e){
        CORE_LOG_ERROR("ObjectSlot: Request for unsupported type: " << type.name() << " " << e.what());
    }
    catch(std::exception & e){
        CORE_LOG_ERROR("ObjectSlot: Request for unsupported type: " << type.name() << " " << e.what());
    }
    catch(...){
        CORE_LOG_ERROR("ObjectSlot: Unknown error");
    }

    return false;
}

void ObjectSlots::setObjects( int slotNo, const ObjectWrapperCollectionPtr& objects )
{
    if (objects != nullptr && !isAssignable(slotNo, objects->getTypeInfo())) {
        throw std::runtime_error("Assign error");
    }
    Slot& slot = this->objects[slotNo];
    slot.objects = objects;
    slot.constObjects = ObjectWrapperCollectionConstPtr(objects);
}

void ObjectSlots::setObjects( int slotNo, const ObjectWrapperCollectionConstPtr& objects )
{
    if ( !isAssignable(slotNo, objects->getTypeInfo()) ) {
        throw std::runtime_error("Assign error");
    }
    Slot& slot = this->objects[slotNo];
    slot.objects.reset();
    slot.constObjects = objects;
}
