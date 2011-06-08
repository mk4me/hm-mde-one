/********************************************************************
    created:  2011/05/20
    created:  20:5:2011   11:43
    filename: EDRDFNode.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___EDRDFNODE_H__
#define HEADER_GUARD___EDRDFNODE_H__

#include <dfmlib/DFNode.h>
#include "WorkflowItemEncapsulator.h"

class EDRDFNode : public dflm::DFNode
{
public:
    EDRDFNode(const WorkflowItemPtr & workflowItem, const std::string & nodeName = std::string());

    ~EDRDFNode();

    const WorkflowItemPtr & getWorkflowItem() const;

    //! Metoda pozwalaj¹ca podpi¹æ metody konfiguracji wêz³a
    virtual void configure();

protected:

    //! Inicjalizacja do implementacji przez klienta
    virtual void doInitialization(const dflm::Node::PinsAdderPtr & pinsAdder);

    //! Przetwarza dane w³aœciwe
    virtual void processData();

private:
    WorkflowItemPtr workflowItem;

};

#endif  //  HEADER_GUARD___EDRDFNODE_H__

