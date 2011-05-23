/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   16:45
    filename: DataProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__DATAPROCESSOR_H__
#define HEADER_GUARD_CORE__DATAPROCESSOR_H__

#include <core/IDataProcessor.h>
#include <core/ObjectWrapper.h>
#include "WorkflowItemEncapsulator.h"
#include "InputOutputItem.h"

class DataProcessor : public InputOutputItem<core::IDataProcessor>
{
//public:
//    typedef ObjectSlots::SlotInfo InputInfo;
//    typedef ObjectSlots::SlotInfo OutputInfo;
//private:
//    //! Dane wejœciowe.
//    core::shared_ptr<ObjectSource> source;
//    //! Dane wyjœciowe.
//    core::scoped_ptr<ObjectOutput> output;

public:
    //! \param impl Obiekt przejmowany na w³asnoœæ.
    DataProcessor(core::IDataProcessor* impl);

public:

    ////! \return
    //ObjectSource& getSource()
    //{ 
    //    return *source;
    //}
    ////! \return
    //ObjectOutput& getOutput() 
    //{ 
    //    return *output;
    //}

    ////! \return Liczba wejœæ.
    //int getNumInput() const
    //{
    //    return source->getNumObjects();
    //}

    ////! \return Liczba wyjœæ.
    //int getNumOutput() const
    //{
    //    return output->getNumObjects();
    //}

    ////! \return Informacja o i-tym wejœciu.
    //const InputInfo& getInputInfo(int i) const
    //{
    //    return source->getSlotInfo(i);
    //}

    ////! \return Informacja o i-tym wyjœciu.
    //const OutputInfo& getOutputInfo(int i) const
    //{
    //    return output->getSlotInfo(i);
    //}

    ////! \param outputNo Numer wyjœcia.
    ////! \return Obiekt na wyjœciu.
    //core::ObjectWrapperPtr getOutputObject(int outputNo)
    //{
    //    return output->getWrapper(outputNo);
    //}
    ////! Gdy wyjœcie/wejœcie s¹ niedopasowane zostanie rzucony wyj¹tek.
    ////! \param inputNo Numer wejœcia.
    ////! \param object Obiekt na wejœciu.
    //void setInputObject(int inputNo, const core::ObjectWrapperPtr& object)
    //{
    //    source->setObject(inputNo, object);
    //}
    ////! Gdy wyjœcie/wejœcie s¹ niedopasowane zostanie rzucony wyj¹tek.
    ////! \param inputNo Numer wejœcia.
    ////! \param object Obiekt na wejœciu.
    //void setInputObject(int inputNo, const core::ObjectWrapperConstPtr& object)
    //{
    //    source->setObject(inputNo, object);
    //}

      

    //! Przetwarza wejœcia i ustawia wyjœcia. Mo¿e rzucaæ wyj¹tkami.
    void process()
    {
        getImplementation()->process(getSource(), getOutput());
    }

    //! \return false je¿eli przetwarzanie zakoñczy³o siê b³êdem (wyj¹tkiem)
    bool tryProcess()
    {
        try {
            getImplementation()->process(getSource(), getOutput());
            return true;
        } catch (const std::exception& ex) {
            LOG_ERROR("Error during processing: " << ex.what());
            return false;
        }
    }

    virtual void run() 
    {
        process();
    }

    static void test();

};


#endif  // HEADER_GUARD_CORE__DATAPROCESSOR_H__