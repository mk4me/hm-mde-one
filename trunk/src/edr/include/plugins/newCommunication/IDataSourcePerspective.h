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
	//! \param treeWidget Drzewo danych dla którego budujemy perspektywê
	//! \param shallowCopy P³ytka kopia danych dla której generujemy perspektywê
    virtual void rebuildPerspective(QTreeWidget * treeWidget, const ShallowCopy & shallowCopy) = 0;

	//! Pusta implementacja funkcjonalnoœci contentu - perspektywa mo¿e generowac w³asne itemy-agregaty danych o których content nic nie wie

	//! \param item Element który wype³niamy zawartoœci¹
	//! \param shallowCopy P³ytka kopia na bazie której wype³niamy item
	//! \return Czy wype³nilismy item
	virtual bool fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const { return false; }
	//! \param item Element dla którego chcemy pobraæ nag³ówek
	//! \param headers [out] Nag³owek dla zadanego elementu
	//! \return Czy wype³nilismy nag³ówek
	virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const { return false; }
	//! \param item Element dla którego chcemy pobraæ pole w którym zamieœcimy jego status
	//! \reuturn Numer pola ze statusem
	virtual int statusField(const QTreeWidgetItem * item) const { return -1; }
	//! \param item Element dla którego chcemy pobraæ pole w którym zamieœcimy info o aktualnym pobieraniu
	//! \reuturn Numer pola z info o pobieraniu
	virtual int downloadField(const QTreeWidgetItem * item) const { return -1; }
	//! Czyœcimy dane contentu (np. bo zmieniamy perspektywê)
	virtual void release() {}
};

}

#endif	//	HEADER_GUARD___IDATASOURCEPERSPECTIVE_H__
