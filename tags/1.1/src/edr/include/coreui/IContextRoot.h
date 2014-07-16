/********************************************************************
	created:	2013/04/08
	created:	8:4:2013   19:46
	filename: 	IContextRoot.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__ICONTEXTROOT_H__
#define HEADER_GUARD_COREUI__ICONTEXTROOT_H__

#include <QtGui/QTabWidget>
#include <coreui/IAppUsageContext.h>

namespace coreUI {

//! Kontekst bazowy dla kontekst�w aplikacji medycznej
class MdeEmptyContext : public coreUI::IAppUsageContext
{
public:
    MdeEmptyContext() {}
    virtual ~MdeEmptyContext() {}

public:
    //! wywo�ywane, po aktywowaniu si� kontekstu
    //! \param contextWidget wiget, kt�ry wywo�a� kontekst
    virtual void activateContext(QWidget * contextWidget) {}
    //! wywo�ywane, gdy kontekst przestaje by� aktywny
    //! \param nextContextWidget 
    //! \param refresh 
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh) {}
};
DEFINE_SMART_POINTERS(MdeEmptyContext);


class IContextRoot
{
public:
	virtual ~IContextRoot() {}
    virtual void registerConxtext(coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget* flexiWidget ) = 0;
};
DEFINE_SMART_POINTERS(IContextRoot);

}
#endif
