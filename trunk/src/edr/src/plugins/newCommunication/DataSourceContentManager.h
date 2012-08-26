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

	//! \param content Generator zawartości który rejestrujemy
	//! \return indeks zarejestrowanego generatora zawartości
	virtual int registerContent(communication::IDataSourceContent * content);
	//! \return Indeks aktualnego generatora zawartości
	virtual int currentContentIndex() const;
	//! \return Aktualny generator zawartości
	virtual communication::DataSourceContentPtr currentContent() const;
	//! \param idx Indeks nowego generatora zawartości
	virtual void setCurrentContent(int idx);
	//! \return Czy jest zarejestrowany jakiś generator zawartości
	virtual bool empty() const;
	//! \return Ilośc zarejestrowanych generatorów zawartości
	virtual int size() const;

	//! \param treeWidget Drzewo z danymi którego zawartość odświeżamy
	//! \param shallowCopy Płytka kopia dla której odśiweżamy zawartość
	//! \param perspective Aktualna perspektywa danych
	//! \patam localStatusManager Manager danych w lokalnej formie (przefiltrowana shallowCopy usera)
	//! \patam globalStatusManager Manager danych w globalnej formie (pełne shallowCopy usera)
	//! \param perspectiveFirst Czy w przypadku kolizji contentu pierwszeństwo ma perspektywa
	void refreshCurrentContent(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy, communication::IDataSourcePerspective * perspective,
		const communication::IDataSourceStatusManager * localStatusManager, const communication::IDataSourceStatusManager * globalStatusManager,
		bool perspectiveFirst = false);

private:
	//! Metoda rekurencyjna
	//! \param item Element drzewa którego status odświeżamy
	//! \param shallowCopy Płytka kopia dla której odśiweżamy zawartość
	//! \param perspective Aktualna perspektywa danych
	//! \patam localStatusManager Manager danych w lokalnej formie (przefiltrowana shallowCopy usera)
	//! \patam globalStatusManager Manager danych w globalnej formie (pełne shallowCopy usera)
	//! \param perspectiveFirst Czy w przypadku kolizji pierwszeństwo ma perspektywa
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
