#include "QtWidget.h"
#include "OsgControlWidget.h"


//--------------------------------------------------------------------------------------------------
QtWidget::QtWidget()
{
    _controlWidget = new OsgControlWidget(); 
}

//--------------------------------------------------------------------------------------------------
QtWidget::~QtWidget()
{
    Clear();
}

//--------------------------------------------------------------------------------------------------
void QtWidget::SetData( osg::Node *scene )
{
    if(!_controlWidget) return; 
    _controlWidget->SetScene(scene); 
}

//--------------------------------------------------------------------------------------------------
void QtWidget::SetData( osg::Node *scene, IServiceManager *pServiceManager )
{
    if(!_controlWidget) return; 
    _controlWidget->SetScene(scene, pServiceManager); 
}

//--------------------------------------------------------------------------------------------------
void QtWidget::Clear()
{
    if(_controlWidget != NULL)
    {
//         _controlWidget->parent()->removeChild(_controlWidget);
//         delete _controlWidget; 
        _controlWidget = 0; 
    }
}

void* QtWidget::GetWidget()
{
    return _controlWidget;
}