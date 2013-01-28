/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   16:59
    filename: CoreWidgetAction.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREWIDGETACTION_H__
#define HEADER_GUARD___COREWIDGETACTION_H__

#include <utils/Export.h>
#include <QtGui/QWidgetAction>
#include <coreui/CoreTitleBar.h>
#include <coreui/ICoreActionSection.h>

namespace coreUI {

class UTILS_EXPORT CoreWidgetAction : public QWidgetAction, public CoreTitleBar::ICoreTitleBarAction, public ICoreActionSection
{
public:
	explicit CoreWidgetAction(QObject *parent, const QString & sectionName, CoreTitleBar::SideType side = CoreTitleBar::Left);

	virtual CoreTitleBar::SideType side() const;
	virtual const QString section() const;

private:
	CoreTitleBar::SideType side_;
	QString sectionName_;
};

}

#endif	//	HEADER_GUARD___COREWIDGETACTION_H__
