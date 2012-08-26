#include "hmmPCH.h"
#include "ContextEventFilter.h"
#include "HmmMainWindow.h"

ContextEventFilter::ContextEventFilter( HmmMainWindow * mainWindow ) 
    : mainWindow(mainWindow) 
{}

void ContextEventFilter::registerPermamentContextWidget( QWidget * widget )
{
    permamentWidgets.insert(widget);
    widget->installEventFilter(this);
}

void ContextEventFilter::registerClosableContextWidget( QWidget * widget )
{
    closableWidgets.insert(widget);
}

void ContextEventFilter::unregisterPermamentContextWidget( QWidget * widget )
{
    permamentWidgets.erase(widget);
}

void ContextEventFilter::unregisterClosableContextWidget( QWidget * widget )
{
    closableWidgets.erase(widget);
}

bool ContextEventFilter::eventFilter( QObject *obj, QEvent *event )
{
    auto w = (QWidget*)obj;
    auto eType = event->type();
    switch(event->type())
    {
    case QEvent::Close:
        {
            auto it = closableWidgets.find(w);
            if(it != closableWidgets.end()){
                mainWindow->deactivateContext(w);
            }
        }
        break;

    case QEvent::MouseButtonPress:
        {
            auto it = closableWidgets.find(w);
            if(it != closableWidgets.end()){
                mainWindow->activateContext(w);
            }
        }
        break;

    case QEvent::FocusIn:
        {
            auto it = permamentWidgets.find(w);
            if(it != permamentWidgets.end()){
                mainWindow->activateContext(w);
            }
        }
        break;
    }

    return QObject::eventFilter(obj, event);
}
