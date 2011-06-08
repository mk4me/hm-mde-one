/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:26
	filename: 	OutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__OUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__OUTPUTDESCRIPTION_H__

#include "ObjectOutput.h"

class OutputDescription
{

public:
    typedef ObjectOutput::SlotsInfo SlotsInfo;
    typedef ObjectSlots::DependentSlots RequiredInputs;

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

    inline const core::TypeInfo& getOutputTypes(int outputNo) const
    {
        return output->getSlotType(outputNo);
    }

    inline const std::string& getOutputName(int outputNo) const
    {
        return output->getSlotName(outputNo);
    }

    ////!
    //const SlotsInfo& getOutputsTypes() const;

    //! \return Liczba obiektów.
    int getNumOutputs() const
    {
        return output->getNumOutputs();
    }
    ////!
    //core::ObjectWrapperCollectionConstPtr getObjects(int slot)
    //{
    //    return output->getObjects(slot);
    //}

    //! \param outputNo Numer wyjœcia.
    //! \return Obiekt na wyjœciu.
    core::ObjectWrapperCollectionPtr getOutputObjects(int outputNo)
    {
        return output->getObjects(outputNo);
    }

    const RequiredInputs & getRequiredInputs(int outputNo) const
    {
        return output->getSlotInfo(outputNo).dependentSlots;
    }
};


#endif