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
#include "ObjectSource.h"
#include "ObjectOutput.h"

class DataProcessor : public InputOutputProcesItem<IDataProcessor>
{
public:
    typedef ObjectSlots::SlotInfo InputInfo;
    typedef ObjectSlots::SlotInfo OutputInfo;
private:
    //! Faktyczna implementacja elementu przetwarzającego.
    core::scoped_ptr<core::IDataProcessor> impl;
    //! Dane wejściowe.
    core::scoped_ptr<ObjectSource> source;
    //! Dane wyjściowe.
    core::scoped_ptr<ObjectOutput> output;

public:
    //! \param impl Obiekt przejmowany na własność.
    DataProcessor(core::IDataProcessor* impl);

public:

    //! \return
    ObjectSource& getSource()
    { 
        return *source;
    }
    //! \return
    ObjectOutput& getOutput() 
    { 
        return *output;
    }

    //! \return Liczba wejść.
    int getNumInput() const
    {
        return source->getNumObjects();
    }

    //! \return Liczba wyjść.
    int getNumOutput() const
    {
        return output->getNumObjects();
    }

    //! \return Informacja o i-tym wejściu.
    const InputInfo& getInputInfo(int i) const
    {
        return source->getSlotInfo(i);
    }

    //! \return Informacja o i-tym wyjściu.
    const OutputInfo& getOutputInfo(int i) const
    {
        return output->getSlotInfo(i);
    }

    //! \param outputNo Numer wyjścia.
    //! \return Obiekt na wyjściu.
    core::ObjectWrapperPtr getOutputObject(int outputNo)
    {
        return output->getWrapper(outputNo);
    }
    //! Gdy wyjście/wejście są niedopasowane zostanie rzucony wyjątek.
    //! \param inputNo Numer wejścia.
    //! \param object Obiekt na wejściu.
    void setInputObject(int inputNo, const core::ObjectWrapperPtr& object)
    {
        source->setObject(inputNo, object);
    }
    //! Gdy wyjście/wejście są niedopasowane zostanie rzucony wyjątek.
    //! \param inputNo Numer wejścia.
    //! \param object Obiekt na wejściu.
    void setInputObject(int inputNo, const core::ObjectWrapperConstPtr& object)
    {
        source->setObject(inputNo, object);
    }

    //! \return Wskaźnik do implementacji.
    const core::IDataProcessor* getProcessor() const
    {
        return impl.get();
    }

    //! Przetwarza wejścia i ustawia wyjścia. Może rzucać wyjątkami.
    void process()
    {
        impl->process(source.get(), output.get());
    }

    //! \return false jeżeli przetwarzanie zakończyło się błędem (wyjątkiem)
    bool tryProcess()
    {
        try {
            impl->process(source.get(), output.get());
            return true;
        } catch (const std::exception& ex) {
            LOG_ERROR("Error during processing: " << ex.what());
            return false;
        }
    }

    static void test();

};


#endif  // HEADER_GUARD_CORE__DATAPROCESSOR_H__
