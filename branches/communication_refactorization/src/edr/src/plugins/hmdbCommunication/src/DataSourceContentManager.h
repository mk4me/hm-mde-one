/********************************************************************
    created:  2012/03/24
    created:  24:3:2012   10:11
    filename: DataSourceContentManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCECONTENTMANAGER_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCECONTENTMANAGER_H__

#include <vector>
#include <plugins/hmdbCommunication/IDataSourceContentManager.h>
#include <plugins/hmdbCommunication/DataStatus.h>

namespace hmdbCommunication
{
	class IDataSourcePerspective;
	class IHMDBStatusManager;

	class DataSourceContentManager : public IDataSourceContentManager
	{
	public:

		DataSourceContentManager();
		virtual ~DataSourceContentManager();

		//! \param content Generator zawartości który rejestrujemy
		//! \return indeks zarejestrowanego generatora zawartości
		virtual int registerContent(hmdbCommunication::IDataSourceContent * content);
		//! \return Indeks aktualnego generatora zawartości
		virtual int currentContentIndex() const;
		//! \return Aktualny generator zawartości
		virtual hmdbCommunication::DataSourceContentPtr currentContent() const;
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
		void refreshCurrentContent(QTreeWidget * treeWidget, const ShallowCopy & shallowCopy, IDataSourcePerspective * perspective,
			const IHMDBStatusManager * statusManager, bool perspectiveFirst = false);

		QIcon statusIcon(const DataStatus status) const;

	private:
		//! Metoda rekurencyjna
		//! \param item Element drzewa którego status odświeżamy
		//! \param shallowCopy Płytka kopia dla której odśiweżamy zawartość
		//! \param perspective Aktualna perspektywa danych
		//! \patam localStatusManager Manager danych w lokalnej formie (przefiltrowana shallowCopy usera)
		//! \patam globalStatusManager Manager danych w globalnej formie (pełne shallowCopy usera)
		//! \param perspectiveFirst Czy w przypadku kolizji pierwszeństwo ma perspektywa
		//! \return Status danych
		DataStatus refrshItemContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy, IDataSourcePerspective * perspective,
			const IHMDBStatusManager * statusManager, bool perspectiveFirst);

	private:
		//! Zarejestrowane filtry
		std::vector<DataSourceContentPtr> contents;
		//! Indeks aktualnego filtra
		int currentContent_;

		mutable std::map<DataStatus, QIcon> icons;
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCECONTENTMANAGER_H__
