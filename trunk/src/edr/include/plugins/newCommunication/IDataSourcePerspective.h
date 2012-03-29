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
#include <webserviceslib/Entity.h>
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
	//! \param treeWidget Drzewo danych dla kt�rego budujemy perspektyw�
	//! \param shallowCopy P�ytka kopia danych dla kt�rej generujemy perspektyw�
    virtual void rebuildPerspective(QTreeWidget * treeWidget, const ShallowCopy & shallowCopy) = 0;

	//! Pusta implementacja funkcjonalno�ci contentu - perspektywa mo�e generowac w�asne itemy-agregaty danych o kt�rych content nic nie wie

	//! \param item Element kt�ry wype�niamy zawarto�ci�
	//! \param shallowCopy P�ytka kopia na bazie kt�rej wype�niamy item
	//! \return Czy wype�nilismy item
	virtual bool fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const { return false; }
	//! \param item Element dla kt�rego chcemy pobra� nag��wek
	//! \param headers [out] Nag�owek dla zadanego elementu
	//! \return Czy wype�nilismy nag��wek
	virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const { return false; }
	//! \param item Element dla kt�rego chcemy pobra� pole w kt�rym zamie�cimy jego status
	//! \reuturn Numer pola ze statusem
	virtual int statusField(const QTreeWidgetItem * item) const { return -1; }
	//! \param item Element dla kt�rego chcemy pobra� pole w kt�rym zamie�cimy info o aktualnym pobieraniu
	//! \reuturn Numer pola z info o pobieraniu
	virtual int downloadField(const QTreeWidgetItem * item) const { return -1; }
	//! Czy�cimy dane contentu (np. bo zmieniamy perspektyw�)
	virtual void release() {}
};

}

#endif	//	HEADER_GUARD___IDATASOURCEPERSPECTIVE_H__
