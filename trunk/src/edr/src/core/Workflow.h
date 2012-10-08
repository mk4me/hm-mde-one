/********************************************************************
    created:  2011/05/24
    created:  24:5:2011   14:24
    filename: Workflow.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___WORKFLOW_H__
#define HEADER_GUARD___WORKFLOW_H__

#include <core/SmartPtr.h>
#include <core/IWorkflow.h>
#include <OpenThreads/Mutex>

class QWidget;
class QObject;

class Workflow : public core::IWorkflow
{
private:
    //! Faktyczny widget.
    QWidget* widget;
    OpenThreads::Mutex synchMutex;

public:
    Workflow();
    ~Workflow();

public:
    //! \return Lista akcji ogólnych wizualizatora.
    const std::vector<QObject*>& getGenericActions() const;
    //! \return Lista akcji ogólnych wizualizatora.
    const std::vector<QObject*>& getOrCreateGenericActions();

    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Może być nullptr, jeżeli jeszcze nie jest stworzony.
    QWidget* getWidget();

};

typedef core::shared_ptr<Workflow> WorkflowPtr;
typedef core::shared_ptr<const Workflow> WorkflowConstPtr;

#endif  //  HEADER_GUARD___WORKFLOW_H__
