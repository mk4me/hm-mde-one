/********************************************************************
	created:	2011/10/07
	created:	7:10:2011   9:59
	filename: 	FilterCommand.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMMLIB__FILTERCOMMAND_H__
#define HEADER_GUARD_HMMLIB__FILTERCOMMAND_H__

#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <utils/SmartPtr.h>
#include <corelib/IFilterCommand.h>
#include <plugins/subject/Types.h>
#include <plugins/subject/SubjectDataFilters.h>



//! komenda bez konfiguratora, która tworzy przefiltrowane drzewo na podstawie filtra danych
class SimpleFilterCommand : public core::IFilterCommand
{
public:
    //! Konstruktor
    //! \param dataFilter filtr danych, który będzie użyty do tworzenia drzewa
    SimpleFilterCommand(SubjectHierarchyFilterPtr dataFilter);

    virtual core::IHierarchyItemPtr getFilteredTree( core::IHierarchyItemConstPtr root );

    virtual void reset();

    virtual QWidget* getConfigurationWidget();

private:
    //! filtr danych, który będzie użyty do tworzenia drzewa
    SubjectHierarchyFilterPtr dataFilter;
};
typedef boost::shared_ptr<SimpleFilterCommand> SimpleFilterCommandPtr;
typedef boost::shared_ptr<const SimpleFilterCommand> SimpleFilterCommandConstPtr;


#endif
