/********************************************************************
	created:	2011/10/07
	created:	7:10:2011   9:59
	filename: 	FilterCommand.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMMLIB__FILTERCOMMAND_H__
#define HEADER_GUARD_HMMLIB__FILTERCOMMAND_H__

#include <set>
#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <utils/SmartPtr.h>
#include <corelib/IFilterCommand.h>
#include <corelib/HierarchyItem.h>
#include <utils/TypeInfo.h>
#include <plugins/subject/Types.h>
#include <plugins/subject/SubjectDataFilters.h>


//! komenda bez konfiguratora, która tworzy przefiltrowane drzewo na podstawie filtra danych
class AbstractFilterCommand : public core::IFilterCommand
{
public:
    //! Konstruktor
    AbstractFilterCommand(const QString& name, const QIcon& icon);
    
    virtual core::IHierarchyItemPtr getFilteredTree( core::IHierarchyItemConstPtr root );


    virtual QWidget* getConfigurationWidget() { return nullptr; }
    virtual void reset() {}

    virtual QString getName() const { return name; }
    virtual QIcon getIcon() const { return icon; }

protected:
    virtual bool checkItem(core::IHierarchyItemConstPtr item) = 0;

private:
    void traverse(core::IHierarchyItemConstPtr source, core::HierarchyItemPtr parent);

private:
    //! filtr danych, który będzie użyty do tworzenia drzewa
    QString name;
    QIcon icon;
};
DEFINE_SMART_POINTERS(AbstractFilterCommand);

//! komenda bez konfiguratora, która tworzy przefiltrowane drzewo na podstawie filtra danych
class SimpleFilterCommand : public AbstractFilterCommand
{
public:
    //! Konstruktor
    SimpleFilterCommand(const std::set<utils::TypeInfo>& typeinfos, const QString& name, const QIcon& icon);

    SimpleFilterCommand(const utils::TypeInfo& typeinfo, const QString& name, const QIcon& icon);

protected:
    virtual bool checkItem( core::IHierarchyItemConstPtr item );

private:
    std::set<core::TypeInfo> typeinfos;
};
typedef boost::shared_ptr<SimpleFilterCommand> SimpleFilterCommandPtr;
typedef boost::shared_ptr<const SimpleFilterCommand> SimpleFilterCommandConstPtr;


#endif
