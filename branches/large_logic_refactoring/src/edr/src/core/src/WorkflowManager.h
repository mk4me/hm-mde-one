/********************************************************************
    created:  2011/05/24
    created:  24:5:2011   14:06
    filename: WorkflowManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___WORKFLOWMANAGER_H__
#define HEADER_GUARD___WORKFLOWMANAGER_H__

#include <vector>
#include <utils/ObserverPattern.h>
#include <core/IWorkflowManager.h>
#include "Workflow.h"

class Workflow;

class WorkflowManager : public core::IWorkflowManager
{
    friend class Workflow;
public:
    typedef std::list<Workflow*> Workflows;

private:

    //! Lista śledząca wizualizatory.
    Workflows workflows;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    WorkflowManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~WorkflowManager();

public:
    static WorkflowManager* getInstance()
    {
        return static_cast<WorkflowManager*>(core::getWorkflowManager());
    }

    //! Tworzy instancję wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    WorkflowPtr createWorkflow();

private:

    void notifyCreated(Workflow* workflow);
    void notifyDestroyed(Workflow* workflow);
};



#endif  // HEADER_GUARD___WORKFLOWMANAGER_H__
