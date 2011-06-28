/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   11:47
    filename: IWorkflow.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IWORKFLOW_H__
#define HEADER_GUARD___IWORKFLOW_H__

#include <core/ISerializable.h>
#include <core/IWorkflowItemBase.h>
#include <core/IInputProcessItem.h>
#include <core/IOutputProcessItem.h>
#include <core/IInputOutputProcessItem.h>

#include <core/IDataProcessor.h>
#include <core/IVisualizer.h>

namespace core {

class Slot
{
public:

    Slot(const Slot & slot) : object(slot.object), slotNo(slot.slotNo) {}
    Slot(const IWorkflowItemBasePtr & object, int slotNo) : object(object), slotNo(slotNo) {}
    ~Slot() {}

    const IWorkflowItemBasePtr & getObjcet() const { return object; }
    int getSlotNum() const { return slotNo; }

private:

    IWorkflowItemBasePtr object;
    int slotNo;
};

class Connection
{
public:

    Connection(const Slot & src, const Slot & dest) : source(src), destination(dest) {}
    ~Connection() {}

    const Slot & getSourceSlot() const { return source; }
    const Slot & getDestinationSlot() const { return destination; }

private:

    Slot source;
    Slot destination;

};

typedef shared_ptr<Connection> ConnPtr;
typedef shared_ptr<const Connection> ConnConstPtr;

class IWorkflow : public ISerializable
{
    /*virtual void addElement() = 0;
    virtual void removeElement() = 0;
    virtual bool canConnect() = 0;
    virtual ConnPtr connect() = 0;
    virtual ConnPtr disconnect() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual bool isModified() const = 0;
    virtual const std::string & getName() const = 0;

    */

};

typedef shared_ptr<IWorkflow> IWorkflowPtr;
typedef shared_ptr<const IWorkflow> IWorkflowConstPtr;

}

#endif  //  HEADER_GUARD___IWORKFLOW_H__
