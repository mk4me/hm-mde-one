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

    //! \see IVisualizer::getInputType
    inline const core::TypeInfo& getInputType(int idx) const
    {
        return source->getSlotType(idx);
    }
    //! \see IVisualizer::getInputType
    inline const std::string& getInputName(int idx) const
    {
        return source->getSlotName(idx);
    }

    //! Dodaje niezmienny obiekt na wej�cie.
    //! \param slot
    //! \param object
    void setObjects(int slot, const core::ObjectWrapperCollectionConstPtr& objects)
    {
        source->setObjects(slot, objects);
    }
    //! Dodaje zmienny obiekt na wej�cie.
    //! \param slot
    //! \param object
    void setObjects(int slot, const core::ObjectWrapperCollectionPtr& objects)
    {
        source->setObjects(slot, objects);
    }
    //! \return Liczba obiekt�w.
    int getNumInputs() const
    {
        return source->getNumSources();
    }
    //!
    core::ObjectWrapperCollectionPtr getObjects(int slot)
    {
        return source->getObjects(slot, boost::false_type());
    }

    core::ObjectWrapperCollectionConstPtr getObjects(int slot) const
    {
        return source->getObjects(slot, boost::true_type());
    }

    bool isRequired(int slot)
    {
        return source->getSlotInfo(slot).required;
    }

};

#endif