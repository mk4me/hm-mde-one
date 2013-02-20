#include "CoreUiPCH.h"
#include <coreui/CoreWidgetAction.h>

using namespace coreUI;

CoreWidgetAction::CoreWidgetAction(QObject *parent, const QString & sectionName, CoreTitleBar::SideType side)
	: QWidgetAction(parent), sectionName_(sectionName), side_(side)
{

}

CoreWidgetAction::~CoreWidgetAction()
{

}

CoreTitleBar::SideType CoreWidgetAction::side() const
{
	return side_;
}

const QString CoreWidgetAction::section() const
{
	return sectionName_;
}