
#include <core/IServiceManager.h>
#include <plugins/chart/ChartService.h>
#include "ChartWidget.h"


ChartService::ChartService()
: name("Chart")
{
    widget = new ChartWidget(this);
 
}


ChartService::~ChartService()
{
}

IWidget* ChartService::getWidget()
{
	//TODO klasa widgetu
    return reinterpret_cast<IWidget*>(widget);
}

AsyncResult ChartService::init( IServiceManager* serviceManager, osg::Node* sceneRoot )
{
    return AsyncResult_Complete;
}

AsyncResult ChartService::compute()
{
  
    return AsyncResult_Complete;
}

AsyncResult ChartService::update( double time, double timeDelta )
{
    
    return AsyncResult_Complete;
}

AsyncResult ChartService::lateUpdate( double time, double timeDelta)
{
   
    return AsyncResult_Complete;
}



const std::string& ChartService::getName() const
{
    return name;
}