/********************************************************************
	created:	2013/04/08
	created:	8:4:2013   20:20
	filename: 	AbstractTab.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ABSTRACTTAB_H__
#define HEADER_GUARD_MDE_VIEW__ABSTRACTTAB_H__

#include "IContextRoot.h"
#include "IMdeTab.h"

class AbstractTab : public IContextRoot, public IMdeTab
{
public:
    AbstractTab(const QIcon& icon, const QString& label);
	virtual ~AbstractTab() {}

public:
    virtual void attachChildContext( IAppUsageContextPtr child );
    virtual void setEnabled( bool enabled );
    virtual bool isEnabled() const;
    virtual QIcon getIcon() const;
    virtual QString getLabel() const;
    virtual bool isActive() const;
    virtual void setActive( bool val );

private:
    QString label;
    QIcon icon;
    bool active;
    bool enabled;
};
DEFINE_SMART_POINTERS(AbstractTab);

#endif
