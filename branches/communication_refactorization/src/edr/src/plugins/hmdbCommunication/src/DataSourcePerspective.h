/********************************************************************
	created:  2012/03/14
	created:  14:3:2012   17:45
	filename: DataSourcePerspective.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEPERSPECTIVE_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEPERSPECTIVE_H__

#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <QtWidgets/QTreeWidget>

namespace hmdbCommunication
{

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
		DataSourcePerspective(const IDataSourcePerspectivePtr & perspective);
		//! Destruktor
		~DataSourcePerspective();

		//! \return Zarejestrowana perspektywa
		const IDataSourcePerspectivePtr & perspective() const;

		//! Uniewaznia perspektywę
		void invalidate();

		//! \return Czy perspektywa jest ważna
		bool invalid() const;

		//! \return Drzewo danych perspektywy
		QTreeWidget * widget();

		//! \param shallowCopy Płytka kopia bazy danych którą odświeżamy perspektywę
		void rebuildPerspective(const ShallowCopy & shallowCopy);
		//! Czyści perspektywę
		void clearPerspective();

	private:
		//! Zarejestroana perspektywa
		IDataSourcePerspectivePtr perspective_;
		//! Drzewo danych na których operuje zarejestrowana perspektywa
		QTreeWidget * treeWidget;
		//! Czy perspektywa jest ważna
		bool invalid_;
	};

	//! Perspektywa pacjenta pełna
	class DataSourcePatientPerspective : public IDataSourcePerspective
	{
	public:
		DataSourcePatientPerspective();
		virtual ~DataSourcePatientPerspective();

		virtual const QString name() const;

		virtual void rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const ShallowCopy & shallowCopy);
	};

	//! Perspektywa medusy - tylko sesje
	class DataSourceMedusaPerspective : public IDataSourcePerspective
	{
	public:
		DataSourceMedusaPerspective();
		virtual ~DataSourceMedusaPerspective();

		virtual const QString name() const;
		virtual void defaultHeaders(QStringList & headers) const;
		virtual void rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const ShallowCopy & shallowCopy);

		virtual const bool headers(const QTreeWidgetItem * item, QStringList & headers) const;
	};

	//! Perspektywa chorób - pacjenci pojawiają się kilka razy w zależności od choroby
	class DataSourceDisorderPerspective : public IDataSourcePerspective
	{
	public:
		DataSourceDisorderPerspective();
		virtual ~DataSourceDisorderPerspective();

		virtual const QString name() const;

		virtual void rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const ShallowCopy & shallowCopy);
	};

	//! Perspektywa płci
	class DataSourceGenderPerspective : public IDataSourcePerspective
	{
	public:
		DataSourceGenderPerspective();
		virtual ~DataSourceGenderPerspective();

		virtual const QString name() const;

		virtual void rebuildPerspective(QTreeWidgetItem * treeWidgetItem, const ShallowCopy & shallowCopy);

		virtual const bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const;
		virtual const bool headers(const QTreeWidgetItem * item, QStringList & headers) const;

		virtual void release();

	private:

		std::map<const QTreeWidgetItem*, hmdbServices::xmlWsdl::Gender::Type> contentMap;
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEPERSPECTIVE_H__
