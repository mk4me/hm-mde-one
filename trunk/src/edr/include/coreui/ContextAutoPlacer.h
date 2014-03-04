/********************************************************************
	created:	2014/02/21
	created:	21:2:2014   11:10
	filename: 	ContextAutoPlacer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__CONTEXTAUTOPLACER_H__
#define HEADER_GUARD_COREUI__CONTEXTAUTOPLACER_H__

#include <QtGui/QWidget>
#include <QtCore/QList>
#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <coreui/CoreFlexiToolBar.h>
#include "IAppUsageContextManager.h"

namespace coreUI {

//! klasa, która automatycznie rozmieszcza akcje w widgetcie flexi bara
class COREUI_EXPORT ContextAutoPlacer
{
public:
    //! Automatycznie rozmieszcza akcje w widgecie.
    //! \param actions rozmieszczane akcje
    //! \return stworzony widget z rozmieszczonymi akcjami
    static QWidget* create(const QList<QAction*>& actions, bool skipSettings = true, bool skipUtils = true);
};


}

#endif
