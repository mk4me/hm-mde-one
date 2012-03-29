/********************************************************************
    created:  2012/03/24
    created:  24:3:2012   10:11
    filename: DataSourceContentManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCECONTENTMANAGER_H__
#define HEADER_GUARD___DATASOURCECONTENTMANAGER_H__

#include <vector>
#include <plugins/newCommunication/IDataSourceContentManager.h>
#include <plugins/newCommunication/IDataSourcePerspective.h>
#include <plugins/newCommunication/IDataSourceStatusManager.h>
#include "DataSourceStatus.h"

class DataSourceContentManager : public communication::IDataSourceContentManager
{
public:

	DataSourceContentManager();
	virtual ~DataSourceContentManager();

	//! \param content Generator zawarto�ci kt�ry rejestrujemy
	//! \return indeks zarejestrowanego generatora zawarto�ci
	virtual int registerContent(communication::IDataSourceContent * content);
	//! \return Indeks aktualnego generatora zawarto�ci
	virtual int currentContentIndex() const;
	//! \return Aktualny generator zawarto�ci
	virtual communication::DataSourceContentPtr currentContent() const;
	//! \param idx Indeks nowego generatora zawarto�ci
	virtual void setCurrentContent(int idx);
	//! \return Czy jest zarejestrowany jaki� generator zawarto�ci
	virtual bool empty() const;
	//! \return Ilo�c zarejestrowanych generator�w zawarto�ci
	virtual int size() const;

	//! \param treeWidget Drzewo z danymi kt�rego zawarto�� od�wie�amy
	//! \param shallowCopy P�ytka kopia dla kt�rej od�iwe�amy zawarto��
	//! \param perspective Aktualna perspektywa danych
	//! \patam localStatusManager Manager danych w lokalnej formie (przefiltrowana shallowCopy usera)
	//! \patam globalStatusManager Manager danych w globalnej formie (pe�ne shallowCopy usera)
	//! \param perspectiveFirst Czy w przypadku kolizji contentu pierwsze�stwo ma perspektywa
	void refreshCurrentContent(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy, communication::IDataSourcePerspective * perspective,
		const communication::IDataSourceStatusManager * localStatusManager, const communication::IDataSourceStatusManager * globalStatusManager,
		bool perspectiveFirst = false);

private:
	//! Metoda rekurencyjna
	//! \param item Element drzewa kt�rego status od�wie�amy
	//! \param shallowCopy P�ytka kopia dla kt�rej od�iwe�amy zawarto��
	//! \param perspective Aktualna perspektywa danych
	//! \patam localStatusManager Manager danych w lokalnej formie (przefiltrowana shallowCopy usera)
	//! \patam globalStatusManager Manager danych w globalnej formie (pe�ne shallowCopy usera)
	//! \param perspectiveFirst Czy w przypadku kolizji pierwsze�stwo ma perspektywa
	//! \return Status danych
	DataStatus refrshItemContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy, communication::IDataSourcePerspective * perspective,
		const communication::IDataSourceStatusManager * localStatusManager, const communication::IDataSourceStatusManager * globalStatusManager,
		bool perspectiveFirst);

private:
	//! Zarejestrowane filtry
	std::vector<communication::DataSourceContentPtr> contents;
	//! Indeks aktualnego filtra
	int currentContent_;

};

#endif	//	HEADER_GUARD___DATASOURCECONTENTMANAGER_H__
