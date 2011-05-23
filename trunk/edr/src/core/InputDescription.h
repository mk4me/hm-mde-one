/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:23
	filename: 	InputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__INPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__INPUTDESCRIPTION_H__

#include "ObjectSource.h"

class InputDescription
{

public:
    typedef ObjectSource::SlotsInfo SlotsInfo;

private:
    boost::scoped_ptr<ObjectSource> source;
public:
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

    //! \see IVisualizer::getInputTypes
    inline const core::TypeInfoList& getInputTypes(int idx) const
    {
        return source->getSlotTypes(idx);
    }
    //! \see IVisualizer::getInputTypes
    inline const std::string& getInputName(int idx) const
    {
        return source->getSlotName(idx);
    }

    //!
    const SlotsInfo& getSourcesTypes() const;

    //! Dodaje niezmienny obiekt na wejœcie.
    //! \param slot
    //! \param object
    void setObject(int slot, const core::ObjectWrapperConstPtr& object)
    {
        source->setObject(slot, object);
    }
    //! Dodaje zmienny obiekt na wejœcie.
    //! \param slot
    //! \param object
    void setObject(int slot, const core::ObjectWrapperPtr& object)
    {
        source->setObject(slot, object);
    }
    //! \return Liczba obiektów.
    int getNumObjects() const
    {
        return source->getNumObjects();
    }
    //!
    core::ObjectWrapperConstPtr getObject(int slot)
    {
        return source->getObject(slot, boost::true_type());
    }

};

#endif