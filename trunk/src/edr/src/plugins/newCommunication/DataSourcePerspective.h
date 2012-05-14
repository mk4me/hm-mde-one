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
#include <QtGui/QTreeWidget>
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

//! Obiekt zarz¹dzaj¹cy perspektyw¹ po stornie DataSource
//! Kontroluje czy perspektywa danych jest jeszcze wa¿na, jeœli nie to odœwie¿a j¹ w razie potrzeby (czyœci i przebudowuje)
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

	//! Uniewaznia perspektywê
	void invalidate();

	//! \return Czy perspektywa jest wa¿na
	bool invalid() const;

	//! \return Drzewo danych perspektywy
	QTreeWidget * widget();

	//! \param shallowCopy P³ytka kopia bazy danych któr¹ odœwie¿amy perspektywê
	void rebuildPerspective(const communication::ShallowCopy & shallowCopy);
	//! Czyœci perspektywê
	void clearPerspective();

private:
	//! Zarejestroana perspektywa
	communication::DataSourcePerspectivePtr perspective_;
	//! Drzewo danych na których operuje zarejestrowana perspektywa
	QTreeWidget * treeWidget;
	//! Czy perspektywa jest wa¿na
	bool invalid_;
};

//! Perspektywa pacjenta pe³na
class DataSourcePatientPerspective : public communication::IDataSourcePerspective
{
public:
	DataSourcePatientPerspective();
	virtual ~DataSourcePatientPerspective();

	virtual const std::string name() const;

	virtual void rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy);
};

//! Perspektywa chorób - pacjenci pojawiaj¹ siê kilka razy w zale¿noœci od choroby
class DataSourceDisorderPerspective : public communication::IDataSourcePerspective
{
public:
	DataSourceDisorderPerspective();
	virtual ~DataSourceDisorderPerspective();

	virtual const std::string name() const;

	virtual void rebuildPerspective(QTreeWidget * treeWidget, const communication::ShallowCopy & shallowCopy);
};

//! Perspektywa p³ci
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

	std::map<const QTreeWidgetItem*, char> contentMap;
};

#endif	//	HEADER_GUARD___DATASOURCEPERSPECTIVE_H__