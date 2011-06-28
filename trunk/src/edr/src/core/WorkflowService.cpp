#include "CorePCH.h"
#include "WorkflowService.h"
#include "DataProcessorManager.h"
#include "VisualizerManager.h"
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QAction>

std::string WorkflowService::name = "Workflow Service";

WorkflowService::WorkflowService()
{
    workflowWidget = new WorkflowWidget(this);
    
    
    
    DataProcessorManager::getInstance()->attach(this);
    VisualizerManager::getInstance()->attach(this);


}

WorkflowService::~WorkflowService()
{

}

void WorkflowService::init(core::IServiceManager* serviceManager, core::IDataManager* dataManager)
{

}

QWidget* WorkflowService::getWidget(std::vector<QObject*>& actions)
{
    actions.insert(actions.end(), this->actions.begin(), this->actions.end());
    return workflowWidget;
}

const std::string & WorkflowService::getName() const
{
    return name;
}

void WorkflowService::update(const VisualizerManager * subject)
{
    LOG_INFO("New visualizer catched by Workflow service");
}
void WorkflowService::update(const DataProcessorManager * subject)
{
    LOG_INFO("New data processor catched by Workflow service");
}