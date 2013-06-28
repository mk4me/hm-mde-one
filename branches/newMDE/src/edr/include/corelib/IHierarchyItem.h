/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   14:19
	filename: 	IHierarchyItem.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IHIERARCHYITEM_H__
#define HEADER_GUARD_CORE__IHIERARCHYITEM_H__

#include <utils/SmartPtr.h>
#include <QtCore/QString>
#include <QtGui/QIcon>

namespace core {
class IHierarchyItem;
DEFINE_SMART_POINTERS(IHierarchyItem);


//! Postawowy interfejs, na kt�rym opiera si� hierarchiczna struktura danych
class IHierarchyItem
{
public:
	virtual ~IHierarchyItem() {}

public:
    virtual void appendChild(IHierarchyItemConstPtr child) = 0;
    virtual void removeChild(IHierarchyItemConstPtr child) = 0;
    virtual IHierarchyItemConstPtr getChild(int index) const = 0;
    virtual int getNumChildren() const = 0;
    virtual int getIndex() const = 0;
    virtual int getChildIndex(IHierarchyItemConstPtr child) const = 0;
    virtual IHierarchyItemConstPtr getParent() const = 0;
    virtual QString getName() const = 0;
    virtual QIcon getIcon() const = 0;
    virtual void setParent(IHierarchyItemConstPtr parent) = 0;
    virtual QString getDescription() const = 0;
};
}

#endif
