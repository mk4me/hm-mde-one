/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   17:45
    filename: DataSourcePerspective.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEPERSPECTIVE_H__
#define HEADER_GUARD___DATASOURCEPERSPECTIVE_H__

#include <plugins/newCommunication/IDataSourcePerspective.h>
#include <plugins/newCommunication/IDataSourcePerspectiveManager.h>
#include <QtWidgets/QTreeWidget>
#include "DataSourceStatusManager.h"

//! Klasa widgeta drzewa, gdzie na prawy przycisk myszy nie ma zaznaczania danych w drzewie
class PerspectiveTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:

	PerspectiveTreeWidget(QWidget * parent = nullptr);
	virtual ~PerspectiveTreeWidget();

protected:

	virtual void mousePressEvent(QMouseEvent *event);
};

//! Obiekt zarządzający perspektywą po stornie DataSource
//! Kontroluje czy perspektywa danych jest jeszcze ważna, jeśli nie to odświeża ją w razie potrzeby (czyści i przebudowuje)
class DataSourcePerspective
{
public:
	//! Konstruktor
	//! \param perspective Perspektywa danych
	DataSourcePerspective(const communication::DataSourcePerspectivePtr & perspective);
	//! Destruktor
	~DataSourcePerspective();

	//! \return Zarejestrowana perspektywa
	const communication::DataSourcePerspectivePtr & perspective() const;

	//! Uniewaznia perspektywę
	void invalidate();

	//! \return Czy perspektywa jest ważna
	bool invalid() const;

	//! \return Drzewo danych perspektywy
	QTreeWidget * widget();

	//! \param shallowCopy Płytka kopia bazy danych którą odświeżamy perspektywę
	void rebuildPerspective(const communication::ShallowCopy & shallowCopy);
	//! Czyści perspektywę
	void clearPerspective();

private:
	//! Zarejestroana perspektywa
	communication::DataSourcePerspectivePtr perspective_;
	//! Drzewo danych na których operuje zarejestrowana perspektywa
	QTreeWidget * treeWidget;
	//! Czy perspektywa jest ważna
	bool invalid_;
};

//! Perspektywa pacjenta pełna
class DataSourcePatientPerspective : public communication::IDataSourcePerspective
{
public:
	DataSourcePatientPerspective();
	virtual ~DataSourcePatientPerspective();

	virtual const std::string name() const;

	virtual void rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy);
};

//! Perspektywa medusy - tylko sesje
class DataSourceMedusaPerspective : public communication::IDataSourcePerspective
{
public:
    DataSourceMedusaPerspective();
    virtual ~DataSourceMedusaPerspective();

    virtual const std::string name() const;

    virtual void rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy);

    virtual bool headers( const QTreeWidgetItem * item, QStringList & headers ) const;

};

//! Perspektywa chorób - pacjenci pojawiają się kilka razy w zależności od choroby
class DataSourceDisorderPerspective : public communication::IDataSourcePerspective
{
public:
	DataSourceDisorderPerspective();
	virtual ~DataSourceDisorderPerspective();

	virtual const std::string name() const;

	virtual void rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy);
};

//! Perspektywa płci
class DataSourceGenderPerspective : public communication::IDataSourcePerspective
{
public:
	DataSourceGenderPerspective();
	virtual ~DataSourceGenderPerspective();

	virtual const std::string name() const;

	virtual void rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy);

	virtual bool fillContent(QTreeWidgetItem * item, const communication::ShallowCopy & shallowCopy) const;
	virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const;

	virtual void release();

private:

	std::map<const QTreeWidgetItem*, webservices::xmlWsdl::Gender::Type> contentMap;
};

#endif	//	HEADER_GUARD___DATASOURCEPERSPECTIVE_H__
