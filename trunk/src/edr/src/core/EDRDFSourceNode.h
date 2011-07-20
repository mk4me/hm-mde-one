/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   8:27
    filename: EDRDFSourceNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___EDRDFSOURCENODE_H__
#define HEADER_GUARD___EDRDFSOURCENODE_H__

#include "EDRDFNode.h"
#include <dfmlib/DFSourceNode.h>

class EDRDFSourceNode : public virtual EDRDFNode, public virtual dflm::DFSourceNode
{
public:
    EDRDFSourceNode(const WorkflowItemPtr & workflowItem, const std::string & name = std::string());
    ~EDRDFSourceNode();

protected:

    //! Inicjalizacja do implementacji przez klienta
    virtual void doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder);
    virtual bool hasMoreData() const;

    virtual void processData();
	
private:

    const core::IDataSource* source;

};

#endif  //  HEADER_GUARD___EDRDFSOURCENODE_H__

