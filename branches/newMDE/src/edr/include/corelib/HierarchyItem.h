/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   14:18
	filename: 	HierarchyItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYITEM_H__
#define HEADER_GUARD_CORE__HIERARCHYITEM_H__

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <utils/ObjectWrapper.h>
#include <corelib/IHierarchyItem.h>

namespace core {

class CORELIB_EXPORT HierarchyItem : public IHierarchyItem, public boost::enable_shared_from_this<HierarchyItem>
{
public:
    HierarchyItem(const QString& name, const QIcon& icon = QIcon() );
    virtual ~HierarchyItem();

public:
    virtual void appendChild(IHierarchyItemConstPtr child);
    virtual void removeChild(IHierarchyItemConstPtr child);
    virtual IHierarchyItemConstPtr getChild(int row) const;
    virtual int getNumChildren() const;
    virtual int getIndex() const;
    virtual int getChildIndex(IHierarchyItemConstPtr child) const;
    virtual IHierarchyItemConstPtr getParent() const;
    virtual QString getName() const;
    virtual QIcon getIcon() const;
    virtual void setParent(IHierarchyItemConstPtr parent);

private:
    QList<IHierarchyItemConstPtr> childItems;
    QString name;
    QIcon icon;
    IHierarchyItemConstWeakPtr parentItem;
};
DEFINE_SMART_POINTERS(HierarchyItem)

}

#endif
