#include "CorePCH.h"
#include <core/CoreWidgetAction.h>

CoreWidgetAction::CoreWidgetAction(QObject *parent, CoreActionSide::SideType side) : QWidgetAction(parent), CoreActionSide(side)
{

}