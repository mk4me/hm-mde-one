/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	OutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__OUTPUTDESCRIPTION_H__

#include "ObjectSource.h"

class OutputDescription
{

public:
    typedef ObjectSource::SlotsInfo SlotsInfo;

private:
    boost::scoped_ptr<ObjectOutput> output;
public:
    OutputDescription(const OutputDescription& outputDescription) : 
      output(new ObjectOutput(*outputDescription.output))
      {}

      OutputDescription(const ObjectOutput& objOutput) :
      output(new ObjectOutput(objOutput))
      {}
      
      virtual ~OutputDescription() {}

public:
    const ObjectOutput* getOutput() const { return output.get(); }
    ObjectOutput* getOutput() { return output.get(); }

    inline const core::TypeInfoList& getOutputTypes(int idx) const
    {
        return output->getSlotTypes(idx);
    }

    inline const std::string& getOutputName(int idx) const
    {
        return output->getSlotName(idx);
    }

    //!
    const SlotsInfo& getOutputsTypes() const;

    //! \return Liczba obiektów.
    int getNumObjects() const
    {
        return output->getNumObjects();
    }
    //!
    core::ObjectWrapperConstPtr getObject(int slot)
    {
        return output->getObject(slot);
    }

    //! \param outputNo Numer wyjœcia.
    //! \return Obiekt na wyjœciu.
    core::ObjectWrapperPtr getOutputObject(int outputNo)
    {
        return output->getWrapper(outputNo);
    }
};


#endif