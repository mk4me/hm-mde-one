/********************************************************************
	created:	2013/04/17
	created:	17:4:2013   10:19
	filename: 	AnalysisTreeItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MDE_VIEW__ANALYSISTREEITEM_H__
#define HEADER_GUARD_MDE_VIEW__ANALYSISTREEITEM_H__

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <utils/ObjectWrapper.h>
#include "IAnalysisTreeItem.h"
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>

class AnalysisTreeItem : public IAnalysisTreeItem, public boost::enable_shared_from_this<AnalysisTreeItem>
{
public:
    AnalysisTreeItem(const QString& name, const QIcon& icon = QIcon() );
    virtual ~AnalysisTreeItem();

public:
    virtual void appendChild(IAnalysisTreeItemPtr child);
    virtual void removeChild(IAnalysisTreeItemPtr child);
    virtual IAnalysisTreeItemConstPtr getChild(int row) const;
    virtual int getNumChildren() const;
    virtual int getIndex() const;
    virtual int getChildIndex(IAnalysisTreeItemConstPtr child) const;
    virtual IAnalysisTreeItemConstPtr getParent() const;
    virtual QString getName() const;
    virtual QIcon getIcon() const;
    virtual void setParent(IAnalysisTreeItemPtr parent);

private:
    QList<IAnalysisTreeItemPtr> childItems;
    QString name;
    QIcon icon;
    IAnalysisTreeItemConstWeakPtr parentItem;
};

#endif
