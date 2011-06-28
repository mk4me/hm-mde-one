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
    //! Faktyczna implementacja elementu przetwarzaj�cego.
    core::scoped_ptr<core::IDataProcessor> impl;
    //! Dane wej�ciowe.
    core::scoped_ptr<ObjectSource> source;
    //! Dane wyj�ciowe.
    core::scoped_ptr<ObjectOutput> output;

public:
    //! \param impl Obiekt przejmowany na w�asno��.
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

    //! \return Liczba wej��.
    int getNumInput() const
    {
        return source->getNumObjects();
    }

    //! \return Liczba wyj��.
    int getNumOutput() const
    {
        return output->getNumObjects();
    }

    //! \return Informacja o i-tym wej�ciu.
    const InputInfo& getInputInfo(int i) const
    {
        return source->getSlotInfo(i);
    }

    //! \return Informacja o i-tym wyj�ciu.
    const OutputInfo& getOutputInfo(int i) const
    {
        return output->getSlotInfo(i);
    }

    //! \param outputNo Numer wyj�cia.
    //! \return Obiekt na wyj�ciu.
    core::ObjectWrapperPtr getOutputObject(int outputNo)
    {
        return output->getWrapper(outputNo);
    }
    //! Gdy wyj�cie/wej�cie s� niedopasowane zostanie rzucony wyj�tek.
    //! \param inputNo Numer wej�cia.
    //! \param object Obiekt na wej�ciu.
    void setInputObject(int inputNo, const core::ObjectWrapperPtr& object)
    {
        source->setObject(inputNo, object);
    }
    //! Gdy wyj�cie/wej�cie s� niedopasowane zostanie rzucony wyj�tek.
    //! \param inputNo Numer wej�cia.
    //! \param object Obiekt na wej�ciu.
    void setInputObject(int inputNo, const core::ObjectWrapperConstPtr& object)
    {
        source->setObject(inputNo, object);
    }

    //! \return Wska�nik do implementacji.
    const core::IDataProcessor* getProcessor() const
    {
        return impl.get();
    }

    //! Przetwarza wej�cia i ustawia wyj�cia. Mo�e rzuca� wyj�tkami.
    void process()
    {
        impl->process(source.get(), output.get());
    }

    //! \return false je�eli przetwarzanie zako�czy�o si� b��dem (wyj�tkiem)
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