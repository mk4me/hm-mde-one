/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   18:02
    filename: DataSourcePerspectiveManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEPERSPECTIVEMANAGER_H__
#define HEADER_GUARD___DATASOURCEPERSPECTIVEMANAGER_H__

#include <plugins/newCommunication/IDataSourcePerspectiveManager.h>
#include "DataSourcePerspective.h"

//! Implementacja managera perspektyw na potrzeby DataSource
class DataSourcePerspectiveManager : public communication::IDataSourcePerspectiveManager
{
public:
	DataSourcePerspectiveManager();
	~DataSourcePerspectiveManager();

	//! \param perspective Perspektywa do zarejestrowania
	virtual int registerPerspective(communication::IDataSourcePerspective * perspective);
	//! \return ilośc zarejestrowanych perspektyw
	virtual int perspectivesCount() const;
	//! \param idx Index aktualnej perspektywy
	virtual void setCurrentPerspective(int idx);
	//! \return Aktualna perspektywa
	virtual communication::DataSourcePerspectivePtr currentPerspective() const;
	//! \return Index aktualnej perspektywy
	virtual int currentPerspectiveIndex() const;
	//! Unieważnia poprzednie perspektywy - np z powodu zmiany filtra
	void invalidatePerspectives();
	//! \param shallowCopy Płytka kopia którą zasilam perspektywę
	//! \param localStatusManager Status danych w takiej formie jakiej są
	//! \param globalStatusManager Status danych w pełnej formie
	void rebuildCurrentPerspective(const communication::ShallowCopy & shallowCopy);
	//! \return Drzewo danych perspektywy
	QTreeWidget * perspectiveWidget(int idx);
	//! \return Drzewo danych perspektywy
	QTreeWidget * currentPerspectiveWidget();

private:

	typedef utils::shared_ptr<DataSourcePerspective> InnerDataSourcePerspectivePtr;

private:
	//! Perspektywy danych
	std::vector<InnerDataSourcePerspectivePtr> perspectives;
	//! Aktualna perspektywa
	int currentPerspective_;

};

#endif	//	HEADER_GUARD___DATASOURCEPERSPECTIVEMANAGER_H__
