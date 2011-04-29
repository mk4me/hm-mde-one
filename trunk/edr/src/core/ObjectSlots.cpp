#include "CorePCH.h"
#include "ObjectSlots.h"

ObjectSlots::ObjectSlots( const std::vector<SlotInfo>& info ) :
info(info), objects(info.size())
{

}

ObjectSlots::ObjectSlots( std::vector<SlotInfo>&& info ) :
info(info), objects(info.size())
{

}

ObjectSlots::ObjectSlots( const ObjectSlots& objectSlots ) : 
info(objectSlots.info), objects(objectSlots.objects)
{

}

bool ObjectSlots::isAssignable( int slotNo, const core::ObjectWrapper* object ) const
{
    if ( !object ) {
        // nulla zawsze mo¿na przypisaæ
        return true;
    } else {
        // sprawdzenie obiekt wspiera któryœ z typów
        const core::TypeInfoList& types = info[slotNo].types;
        auto found = std::find_if(types.begin(), types.end(), [&](const core::TypeInfo& type) { 
            return object->isSupported(type); 
        });
        return found != types.end();
    }
}

void ObjectSlots::setObject( int slotNo, const core::ObjectWrapperPtr& object )
{
    if ( !isAssignable(slotNo, object.get()) ) {
        throw std::runtime_error("Assign error");
    }
    Slot& slot = objects[slotNo];
    slot.object = object;
    slot.constObject = core::ObjectWrapperConstPtr(object);
}

void ObjectSlots::setObject( int slotNo, const core::ObjectWrapperConstPtr& object )
{
    if ( !isAssignable(slotNo, object.get()) ) {
        throw std::runtime_error("Assign error");
    }
    Slot& slot = objects[slotNo];
    slot.object.reset();
    slot.constObject = object;
}