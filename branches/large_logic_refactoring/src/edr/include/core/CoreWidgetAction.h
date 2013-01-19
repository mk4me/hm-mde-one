/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   16:59
    filename: CoreWidgetAction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREWIDGETACTION_H__
#define HEADER_GUARD___COREWIDGETACTION_H__

#include <QtGui/QWidgetAction>
#include <core/CoreActionSide.h>

class CoreWidgetAction : public QWidgetAction, public CoreActionSide
{
public:
	explicit CoreWidgetAction(QObject *parent, CoreActionSide::SideType side = CoreActionSide::Left);
};

#endif	//	HEADER_GUARD___COREWIDGETACTION_H__
