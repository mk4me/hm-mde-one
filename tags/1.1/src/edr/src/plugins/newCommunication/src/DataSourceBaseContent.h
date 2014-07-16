/********************************************************************
    created:  2012/03/24
    created:  24:3:2012   10:34
    filename: DataSourceBaseContent.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEBASECONTENT_H__
#define HEADER_GUARD___DATASOURCEBASECONTENT_H__

#include <plugins/newCommunication/IDataSourceContent.h>

class DataSourceBaseContent : public communication::IDataSourceContent
{
public:
	DataSourceBaseContent();
	~DataSourceBaseContent();

	virtual const std::string name() const;

	virtual bool fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const;
	virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const;
	virtual int statusField(const QTreeWidgetItem * item) const;
	virtual int downloadField(const QTreeWidgetItem * item) const;
	virtual void release();
};

#endif	//	HEADER_GUARD___DATASOURCEBASECONTENT_H__
