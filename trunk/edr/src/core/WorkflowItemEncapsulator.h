/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   11:27
	filename: 	WorkflowItemEncapsulator.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__WORKFLOWITEMENCAPSULATOR_H__
#define HEADER_GUARD_CORE__WORKFLOWITEMENCAPSULATOR_H__

#include <core/IWorkflowItemBase.h>
#include <boost/shared_ptr.hpp>

namespace core {

class WorkflowItem 
{
public:
    virtual ~WorkflowItem() 
    { 
    }
public:
    virtual QWidget* configure() = 0;
    virtual void run() = 0;
    bool tryRun() 
    {
        try {
            run();
        }
        catch (const std::runtime_error& re) {
            return false;
        } catch (const std::exception& e) {
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
private:
    boost::scoped_ptr<T> impl;
protected:

    const T* getImplementation() const { return impl.get(); }
    T* getImplementation() { return impl.get(); }

public:
    WorkflowItemEncapsulator(T* base) : impl(base) {}
    virtual ~WorkflowItemEncapsulator()
    {
    }

public:
    virtual UniqueID getID() const { return impl->getID(); }
    virtual std::string getDescription() const { return impl->getDescription(); }
    virtual QWidget* configure() { return impl->configure(); }
};

}

#endif