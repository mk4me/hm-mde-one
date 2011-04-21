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
#include "ObjectSource.h"

class ObjectOutput : public core::IObjectOutput
{
public:
    virtual int getNumObjects()
    {
        return 0;
    }
    virtual core::ObjectWrapperPtr getWrapper(int idx)
    {
        return core::ObjectWrapperPtr(nullptr);
    }
};

class DataProcessor
{
public:
    typedef ObjectSlots::SlotInfo InputInfo;
    typedef ObjectSlots::SlotInfo OutputInfo;
private:
    //! Faktyczna implementacja elementu przetwarzaj�cego.
    core::scoped_ptr<core::IDataProcessor> impl;
    //! Dane wej�ciowe.
    ObjectSource source;
    //! Dane wyj�ciowe.
    ObjectOutput output;

public:
    //! \param impl Obiekt przejmowany na w�asno��.
    DataProcessor(core::IDataProcessor* impl) : 
    impl(impl), 
    source( DataProcessorManager::getInstance()->getInputInfo(impl->getID()) ),
    output( DataProcessorManager::getInstance()->getOutputInfo(impl->getID()) )
    {}

public:

    //! \return Liczba wej��.
    int getNumInput() const
    {
        return source.getNumObjects();
    }

    //! \return Liczba wyj��.
    int getNumOutput() const
    {
        return output.getNumObjects();
    }

    //! \return Informacja o i-tym wej�ciu.
    const InputInfo& getInputInfo(int i) const
    {
        return source.getSlotInfo(i);
    }

    //! \return Informacja o i-tym wyj�ciu.
    const OutputInfo& getOutputInfo(int i) const
    {
        return output.getSlotInfo(i);
    }

    //! \param outputNo Numer wyj�cia.
    //! \return Obiekt na wyj�ciu.
    const ObjectWrapperPtr& getOutputObject(int outputNo)
    {
        return output.getWrapper(outputNo);
    }
    //! Gdy wyj�cie/wej�cie s� niedopasowane zostanie rzucony wyj�tek.
    //! \param inputNo Numer wej�cia.
    //! \param object Obiekt na wej�ciu.
    void setInputObject(int inputNo, const ObjectWrapperPtr& object)
    {
        source.setObject(inputNo, object);
    }

    //! \return Wska�nik do implementacji.
    const core::IDataProcessor* getProcessor() const
    {
        return impl.get();
    }

    //! Przetwarza wej�cia i ustawia wyj�cia. Mo�e rzuca� wyj�tkami.
    void process()
    {
        impl->process();
    }

    //! \return false je�eli przetwarzanie zako�czy�o si� b��dem (wyj�tkiem)
    bool tryProcess()
    {
        try {
            impl->process(&source, &output);
            return true;
        } catch (const std::exception& ex) {
            LOG_ERROR("Error during processing: " << ex.what());
            return false;
        }
    }

};


#endif  // HEADER_GUARD_CORE__DATAPROCESSOR_H__