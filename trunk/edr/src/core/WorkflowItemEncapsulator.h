/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:27
	filename: 	WorkflowItemEncapsulator.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__WORKFLOWITEMENCAPSULATOR_H__
#define HEADER_GUARD_CORE__WORKFLOWITEMENCAPSULATOR_H__

#include "DataManager.h"
#include <core/IWorkflowItemBase.h>
#include <core/ObjectSlots.h>
#include "ObjectSource.h"
#include "ObjectOutput.h"

class WorkflowItem 
{
public:
    virtual ~WorkflowItem() 
    { 
    }
public:
    virtual const std::string & getName() const = 0;
    virtual QWidget* getConfigurationWidget() = 0;
    virtual void run() = 0;
    bool tryRun() 
    {
        try {
            run();
        }
        catch (const std::runtime_error& e) {
            LOG_ERROR(std::string("Error during execution of module main functionality: ") + e.what());
            return false;
        } catch (const std::exception& e) {
            LOG_ERROR(std::string("Error during execution of module main functionality: ") + e.what());
        	return false;
        } catch (...) {
            LOG_ERROR("Unrecognised error during execution of module main functionality!");
            return false;
        }
        return true;
    }
};

typedef core::shared_ptr<WorkflowItem> WorkflowItemPtr;
typedef core::shared_ptr<const WorkflowItem> WorkflowItemConstPtr;

template <class T>
class WorkflowItemEncapsulator : public WorkflowItem
{
    UTILS_STATIC_ASSERT((boost::is_base_of<core::IWorkflowItemBase, T>::value), "Template class should inherit from core::IWorkflowItemBase");

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
        std::vector<core::IOutputDescription::OutputInfo> coreOutputInfos;
        getImplementation()->getOutputInfo(coreOutputInfos);
        ObjectSlots::SlotsInfo outputInfos(getSlotsInfoFromOutputInfo(coreOutputInfos));

        ObjectOutput output(outputInfos);
        // stworzenie instancji ObjectWrapper�w dla ka�dego z wyj��
        for (int i = 0; i < output.getNumSlots(); ++i) {
            core::ObjectWrapperCollectionPtr wrp = DataManager::getInstance()->createWrapperCollection( output.getSlotType(i) );
            output.ObjectSlots::setObjects(i, wrp);
        }
        return output;
    }

    ObjectSource createSource()
    {
        std::vector<core::IInputDescription::InputInfo> coreInputInfos;
        getImplementation()->getInputInfo(coreInputInfos);

        ObjectSlots::SlotsInfo inputInfos(getSlotsInfoFromInputInfo(coreInputInfos));

        ObjectSource source(inputInfos);

        // stworzenie instancji ObjectWrapper�w dla ka�dego z wej��
        for (int i = 0; i < source.getNumSlots(); ++i) {
            core::ObjectWrapperCollectionPtr wrp = DataManager::getInstance()->createWrapperCollection( source.getSlotType(i) );
            source.ObjectSlots::setObjects(i, wrp);
        }

        return source;
    }

public:
    WorkflowItemEncapsulator(T* base) : impl(base) {}
    WorkflowItemEncapsulator(const WorkflowItemEncapsulator & workflowItem) : impl(dynamic_cast<T*>(workflowItem.impl->createClone())) {}

    virtual ~WorkflowItemEncapsulator()
    {
    }

public:
    virtual UniqueID getID() const { return impl->getID(); }
    virtual std::string getDescription() const { return impl->getDescription(); }
    virtual QWidget* getConfigurationWidget() { return impl->getConfigurationWidget(); }
    virtual const std::string & getName() const { return impl->getName(); }

public:

    static ObjectSlots::SlotsInfo getSlotsInfoFromInputInfo(const std::vector<core::IInputDescription::InputInfo> & inputDescription)
    {
        ObjectSlots::SlotsInfo ret;

        std::for_each(inputDescription.begin(), inputDescription.end(), [&](const core::IInputDescription::InputInfo& info) {
            auto inserted = ret.insert(ret.end(), ObjectSlots::SlotsInfo::value_type());
            inserted->name = info.name;
            inserted->type = info.type;
            inserted->required = info.required;
            inserted->modify = info.modify;
        });

        return ret;
    }

    static ObjectSlots::SlotsInfo getSlotsInfoFromOutputInfo(const std::vector<core::IOutputDescription::OutputInfo> & outputDescription)
    {
        ObjectSlots::SlotsInfo ret;

        std::for_each(outputDescription.begin(), outputDescription.end(), [&](const core::IOutputDescription::OutputInfo& info) {
            auto inserted = ret.insert(ret.end(), ObjectSlots::SlotsInfo::value_type());
            inserted->name = info.name;
            inserted->type = info.type;
            inserted->dependentSlots = info.dependentInput;
            inserted->required = false;
            inserted->modify = false;
        });

        return ret;
    }

};


#endif