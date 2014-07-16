#include "CoreUiPCH.h"
#include <coreui/CoreCursorChanger.h>
#include <QtGui/QApplication>

using namespace coreUI;

CoreCursorChanger::CoreCursorChanger(const Qt::CursorShape cursorShape)
{
	QApplication::setOverrideCursor(cursorShape);
}

CoreCursorChanger::~CoreCursorChanger()
{
	QApplication::restoreOverrideCursor();
}