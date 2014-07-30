/********************************************************************
    created:  2012/03/24
    created:  24:3:2012   10:34
    filename: DataSourceBaseContent.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEBASECONTENT_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEBASECONTENT_H__

#include <plugins/hmdbCommunication/IDataSourceContent.h>

namespace hmdbCommunication
{
	class DataSourceBaseContent : public hmdbCommunication::IDataSourceContent
	{
	public:
		DataSourceBaseContent();
		~DataSourceBaseContent();

		virtual const std::string name() const;

		virtual const bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const;
		virtual const bool headers(const QTreeWidgetItem * item, QStringList & headers) const;
		virtual const int statusField(const QTreeWidgetItem * item) const;
		virtual const int downloadField(const QTreeWidgetItem * item) const;
		virtual void release();
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEBASECONTENT_H__
