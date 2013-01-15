/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:23
	filename: 	InputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__INPUTDESCRIPTION_H__

#include "ObjectSource.h"

namespace core {

class InputDescription
{

public:
    typedef ObjectSource::SlotsInfo SlotsInfo;

private:
    boost::scoped_ptr<ObjectSource> source;
public:

	//TODO
	//do usunięcia
	InputDescription() {}

    InputDescription(const InputDescription& inputDescription) : 
        source(new ObjectSource(*inputDescription.source))
     {}

    InputDescription(const ObjectSource& objSource) :
        source(new ObjectSource(objSource))
    {}

    virtual ~InputDescription() {}
    
public:

    const ObjectSource* getSource() const { return source.get(); }
    ObjectSource* getSource() { return source.get(); }

    inline const TypeInfo& getInputType(int idx) const
    {
        return source->getSlotType(idx);
    }

    inline const std::string& getInputName(int idx) const
    {
        return source->getSlotName(idx);
    }

    //! Dodaje niezmienny obiekt na wejście.
    //! \param slot
    //! \param object
    void setObjects(int slot, const ObjectWrapperCollectionConstPtr& objects)
    {
        source->setObjects(slot, objects);
    }
    //! Dodaje zmienny obiekt na wejście.
    //! \param slot
    //! \param object
    void setObjects(int slot, const ObjectWrapperCollectionPtr& objects)
    {
        source->setObjects(slot, objects);
    }
    //! \return Liczba obiektów.
    int getNumInputs() const
    {
        return source->getNumSources();
    }
    //!
    const ObjectWrapperCollectionPtr & getObjects(int slot) const
    {
        return source->ObjectSlots::getObjects(slot);
    }

    bool isRequired(int slot)
    {
        return source->getSlotInfo(slot).required;
    }

};

}

#endif
