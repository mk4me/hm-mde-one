/********************************************************************
    created:  2011/05/23
    created:  23:5:2011   17:06
    filename: WorkflowService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___WORKFLOWSERVICE_H__
#define HEADER_GUARD___WORKFLOWSERVICE_H__

#include <core/IService.h>
#include <utils/ObserverPattern.h>
#include "WorkflowWidget.h"

class VisualizerManager;
class DataProcessorManager;

class WorkflowService : public core::IService, utils::Observer<VisualizerManager>, utils::Observer<DataProcessorManager>
{
public:
    WorkflowService();
    ~WorkflowService();

    //---------- core::IService ----------------------------
    UNIQUE_ID("{B0992026-42B3-4E21-8548-2102968D66AB}", "Workflow Service");
public:
    virtual void init(core::IServiceManager* serviceManager, core::IDataManager* dataManager);
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    virtual const std::string & getName() const;

    //---------- utils::Observer ----------------------------
    virtual void update(const VisualizerManager * subject);
    virtual void update(const DataProcessorManager * subject);

private:
    static std::string name;
    WorkflowWidget * workflowWidget;
    std::set<WorkflowWidget*> workflowWidgets;
    std::vector<QObject*> actions;
};


#endif  //  HEADER_GUARD___WORKFLOWSERVICE_H__