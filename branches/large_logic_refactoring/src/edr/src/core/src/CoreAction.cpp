#include "CorePCH.h"
#include <core/CoreAction.h>

CoreAction::CoreAction(const QString &text, QObject* parent, CoreActionSide::SideType side) : QAction(text, parent), CoreActionSide(side)
{

}

CoreAction::CoreAction(const QIcon &icon, const QString &text, QObject* parent, CoreActionSide::SideType side) : QAction(icon, text, parent), CoreActionSide(side)
{

}