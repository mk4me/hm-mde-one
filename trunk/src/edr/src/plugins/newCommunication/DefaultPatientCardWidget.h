/********************************************************************
    created:  2012/03/12
    created:  12:3:2012   13:06
    filename: DefaultPatientCardWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DEFAULTPATIENTCARDWIDGET_H__
#define HEADER_GUARD___DEFAULTPATIENTCARDWIDGET_H__

#include <plugins/newCommunication/IPatientCard.h>
#include "AntropometricDataWidget.h"
#include "PersonalDataWidget.h"
#include "DisordersDataWidget.h"
#include "NotesWidget.h"
#include "QMultiToolBox.h"

#include <QtGui/QFrame>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QListWidget>

//! domyœlny widget obs³uguj¹cy karte pacjenta
class DefaultPatientCardWidget : public QFrame
{
	Q_OBJECT

public:
	//! domyœlny konstruktor
	//! \param parent Rodzic widgeta
	DefaultPatientCardWidget(QWidget * parent = nullptr);
	//! Wirtualny destruktor
	virtual ~DefaultPatientCardWidget();

public slots:

	//! \param patient Pacjent którego nale¿y sformatowac w karcie pacjenta - dla nullptr nale¿y zresetowaæ kartê pacjenta
	//! \param subject Subject którego sesje bêdziemy czytaæ
	//! \param photo Zdjêcie pacjenta
	//! \param userData Dane u¿ytkownika (na potrzeby notatek!!)
	void setPatient(const webservices::MedicalShallowCopy::Patient * patient = nullptr,
		const webservices::MotionShallowCopy::Performer * subject = nullptr,
		const QPixmap & photo = QPixmap(), const communication::IUserData & userData = communication::IUserData());

	void innerSetPatient( const webservices::MedicalShallowCopy::Patient * patient, const QPixmap & photo, const webservices::MotionShallowCopy::Performer * subject, const communication::IUserData & userData );

private slots:
		
	void setPersonalData(const webservices::MedicalShallowCopy::Patient * patient = nullptr, const QPixmap & photo = QPixmap());
	void setDisordersData(const webservices::MedicalShallowCopy::PatientDisorders & patientDisorders);
	void setSessionsData(const webservices::MotionShallowCopy::PerformerConfs & subjectConfigurations);
	void setNotes(const webservices::MedicalShallowCopy::Patient * patient,
		const webservices::MotionShallowCopy::Performer * subject,
		const communication::IUserData & userData);
	void setAntropometricData(const webservices::MotionShallowCopy::Attrs & attributes);

	void currentSessionChanged(QTreeWidgetItem * prev, QTreeWidgetItem * current);

private:

	static QString getAttribute(const webservices::MotionShallowCopy::Attrs & attributes, const std::string & name, const QString & defaultVal = QString("-"));

	static QTreeWidgetItem * createBranch(const QString & name, const std::map<int, const webservices::MotionShallowCopy::Session *> & sessions);

private:

	QTreeWidget * sessionsWidget;

	PersonalDataWidget * personalDataWidget;

	DisordersDataWidget * disordersDataWidget;

	AntropometricDataWidget * antropomtricDataWidget;

	NotesWidget * notesWidget;
};


class DefaultPatientCard : public communication::IPatientCard
{
public:

	DefaultPatientCard();
	virtual ~DefaultPatientCard();

	//! \return Nazwa schematu karty pacjenta
	virtual const std::string name();
	//! \return Widget z kart¹ pacjenta
	virtual QWidget * widget();
	//! \param patient Pacjent którego nale¿y sformatowac w karcie pacjenta - dla nullptr nale¿y zresetowaæ kartê pacjenta
	//! \param subject Subject którego sesje bêdziemy czytaæ
	//! \param photo Zdjêcie pacjenta
	//! \param userData Dane u¿ytkownika (na potrzeby notatek!!)
	virtual void setPatient(const webservices::MedicalShallowCopy::Patient * patient = nullptr,
		const webservices::MotionShallowCopy::Performer * subject = nullptr,
		const QPixmap & photo = QPixmap(), const communication::IUserData & userData = communication::IUserData());

private:

	DefaultPatientCardWidget * patientCardWidget;
};

#endif	//	HEADER_GUARD___DEFAULTPATIENTCARDWIDGET_H__
