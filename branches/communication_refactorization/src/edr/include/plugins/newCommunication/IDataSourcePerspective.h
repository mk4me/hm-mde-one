/********************************************************************
	created:  2012/02/16
	created:  16:2:2012   13:47
	filename: IDataSourcePerspective.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEPERSPECTIVE_H__
#define HEADER_GUARD___IDATASOURCEPERSPECTIVE_H__

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <plugins/newCommunication/IDataSourceStatusManager.h>
#include <plugins/newCommunication/DataSourceShallowCopy.h>
#include <hmdbserviceslib/Entity.h>
#include <plugins/newCommunication/IDataSourceContent.h>

namespace communication
{
	//! Interfejs perspektywy danych, ich hierarchi
	class IDataSourcePerspective : public IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourcePerspective() {}
		//! Nazwa perspektywy
		virtual const std::string name() const = 0;
		//! \param treeWidget Drzewo danych dla którego budujemy perspektywę
		//! \param shallowCopy Płytka kopia danych dla której generujemy perspektywę
		virtual void rebuildPerspective(QTreeWidget * treeWidget, const ShallowCopy & shallowCopy) = 0;

		//! Pusta implementacja funkcjonalności contentu - perspektywa może generować własne itemy-agregaty danych o których content nic nie wie

		//! \param item Element który wypełniamy zawartością
		//! \param shallowCopy Płytka kopia na bazie której wypełniamy item
		//! \return Czy wypełnilismy item
		virtual bool fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const { return false; }
		//! \param item Element dla którego chcemy pobrać nagłówek
		//! \param headers [out] Nagłowek dla zadanego elementu
		//! \return Czy wypełnilismy nagłówek
		virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const { return false; }
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy jego status
		//! \reuturn Numer pola ze statusem
		virtual int statusField(const QTreeWidgetItem * item) const { return -1; }
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy info o aktualnym pobieraniu
		//! \reuturn Numer pola z info o pobieraniu
		virtual int downloadField(const QTreeWidgetItem * item) const { return -1; }
		//! Czyścimy dane contentu (np. bo zmieniamy perspektywę)
		virtual void release() {}
	};
}

#endif	//	HEADER_GUARD___IDATASOURCEPERSPECTIVE_H__
