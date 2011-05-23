/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:27
	filename: 	WorkflowItemEncapsulator.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__WORKFLOWITEMENCAPSULATOR_H__
#define HEADER_GUARD_CORE__WORKFLOWITEMENCAPSULATOR_H__

#include <core/IWorkflowItemBase.h>
#include <core/ObjectSlots.h>
#include <boost/shared_ptr.hpp>
#include "ObjectSource.h"

class WorkflowItem 
{
public:
    virtual ~WorkflowItem() 
    { 
    }
public:
    virtual QWidget* getConfigurationWidget() = 0;
    virtual void run() = 0;
    bool tryRun() 
    {
        try {
            run();
        }
        catch (const std::runtime_error&) {
            return false;
        } catch (const std::exception&) {
        	return false;
        } catch (...) {
            return false;
        }
        return true;
    }
};

template <class T>
class WorkflowItemEncapsulator : public WorkflowItem
{
public:
    typedef ObjectSlots::SlotInfo InputInfo;
    typedef ObjectSlots::SlotInfo OutputInfo;

private:
    boost::scoped_ptr<T> impl;
protected:

    const T* getImplementation() const { return impl.get(); }
    T* getImplementation() { return impl.get(); }

    ObjectOutput createOutput()
    {
        std::vector<T::OutputInfo> coreOutputInfos;
        getImplementation()->getOutputInfo(coreOutputInfos);
        std::vector<OutputInfo> outputInfos;
        outputInfos.reserve(coreOutputInfos.size());
        std::for_each(coreOutputInfos.begin(), coreOutputInfos.end(), [&](T::OutputInfo& info) {
            auto inserted = outputInfos.insert(outputInfos.end(), std::vector<OutputInfo>::value_type());
            inserted->name.swap(info.name);
            inserted->types.push_back(info.type);
        });
        ObjectOutput output(outputInfos);
        ObjectOutput* outPtr = &output;
        // stworzenie instancji ObjectWrapperów dla ka¿dego z wyjœæ
        for (int i = 0; i < output.getNumSlots(); ++i) {
            ObjectWrapperPtr wrp = DataManager::getInstance()->createWrapper( outPtr->getSlotTypes(i).front() );
            static_cast<ObjectSlots*>(outPtr)->setObject(i, wrp);
        }
        return output;
    }

    ObjectSource createSource()
    {
        std::vector<InputInfo> inputInfos;
        T::InputInfo inputInfo;
        for (int i = 0; i < T::maxNumSources; ++i) {
            inputInfo.name.clear();
            inputInfo.types.clear();
            getImplementation()->getInputInfo(i, inputInfo);
            if ( inputInfo.types.empty() ) {
                break;
            } else {
                // zamiast push_backa mo¿na zrobiæ bardziej optymalnie i nie kopiowaæ wektora...
                // sourcesTypes.push_back(info);
                auto inserted = inputInfos.insert(inputInfos.end(), InputInfo());
                inserted->name.swap(inputInfo.name);
                inserted->types.swap(inputInfo.types);
            }
        }
        //core::shared_ptr<ObjectSource> source(new ObjectSource(inputInfos));
        ObjectSource source(inputInfos);
        return source;
    }

public:
    WorkflowItemEncapsulator(T* base) : impl(base) {}
    virtual ~WorkflowItemEncapsulator()
    {
    }

public:
    virtual UniqueID getID() const { return impl->getID(); }
    virtual std::string getDescription() const { return impl->getDescription(); }
    virtual QWidget* getConfigurationWidget() { return impl->getConfigurationWidget(); }
};


#endif