/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   16:51
    filename: CoreAction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREACTION_H__
#define HEADER_GUARD___COREACTION_H__

#include <QtGui/QAction>
#include <core/CoreActionSide.h>

class CoreAction : public QAction, public CoreActionSide
{
public:
	CoreAction(const QString &text, QObject* parent, CoreActionSide::SideType side = CoreActionSide::Left);
	CoreAction(const QIcon &icon, const QString &text, QObject* parent, CoreActionSide::SideType side = CoreActionSide::Left);
};

#endif	//	HEADER_GUARD___COREACTION_H__
