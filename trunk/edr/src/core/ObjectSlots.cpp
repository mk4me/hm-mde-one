#include "CorePCH.h"
#include "ObjectSlots.h"
#include "DataManager.h"

ObjectSlots::ObjectSlots( const SlotsInfo& info ) :
info(info), objects(info.size())
{

}

ObjectSlots::ObjectSlots( SlotsInfo&& info ) :
info(info), objects(info.size())
{

}

ObjectSlots::ObjectSlots( const ObjectSlots& objectSlots ) : 
info(objectSlots.info), objects(objectSlots.objects)
{

}

bool ObjectSlots::isAssignable( int slotNo, const core::TypeInfo & type ) const
{
    //if ( !object ) {
    //    // nulla zawsze mo¿na przypisaæ
    //    return true;
    //} else {
    //    // sprawdzenie obiekt wspiera któryœ z typów
    //    const core::TypeInfo& types = info[slotNo].type;
    //    auto found = std::find_if(types.begin(), types.end(), [&](const core::TypeInfo& type) { 
    //        return object->isSupported(type); 
    //    });
    //    return found != types.end();
    //}

    try{
        core::ObjectWrapper::Types supportedTypes;
        DataManager::getInstance()->getTypePrototype(type)->getSupportedTypes(supportedTypes);

        if(std::find(supportedTypes.begin(), supportedTypes.end(), getSlotType(slotNo)) != supportedTypes.end()){
            return true;
        }
    }
    catch(std::exception e){
        LOG_ERROR(std::string("ObjectSlot: Request for unsupported type: ") + std::string(type.name()) + e.what());
    }
    catch(std::runtime_error e){
        LOG_ERROR(std::string("ObjectSlot: Request for unsupported type: ") + std::string(type.name()) + e.what());
    }
    catch(std::invalid_argument e){
        LOG_ERROR(std::string("ObjectSlot: Request for unsupported type: ") + std::string(type.name()) + e.what());
    }

    return false;
}

void ObjectSlots::setObjects( int slotNo, const core::ObjectWrapperCollectionPtr& objects )
{
    if ( !isAssignable(slotNo, objects->getTypeInfo())) {
        throw std::runtime_error("Assign error");
    }
    Slot& slot = this->objects[slotNo];
    slot.objects = objects;
    slot.constObjects = core::ObjectWrapperCollectionConstPtr(objects);
}

void ObjectSlots::setObjects( int slotNo, const core::ObjectWrapperCollectionConstPtr& objects )
{
    if ( !isAssignable(slotNo, objects->getTypeInfo()) ) {
        throw std::runtime_error("Assign error");
    }
    Slot& slot = this->objects[slotNo];
    slot.objects.reset();
    slot.constObjects = objects;
}