/********************************************************************
	created:  2012/03/12
	created:  12:3:2012   13:06
	filename: DefaultPatientCardWidget.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DEFAULTPATIENTCARDWIDGET_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DEFAULTPATIENTCARDWIDGET_H__

#include <plugins/hmdbCommunication/IPatientCard.h>
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

//! domyślny widget obsługujący karte pacjenta
class DefaultPatientCardWidget : public QFrame
{
	Q_OBJECT

public:
	//! domyślny konstruktor
	//! \param parent Rodzic widgeta
	DefaultPatientCardWidget(QWidget * parent = nullptr);
	//! Wirtualny destruktor
	virtual ~DefaultPatientCardWidget();

	public slots:

	//! \param patient Pacjent którego należy sformatować w karcie pacjenta - dla nullptr należy zresetować kartę pacjenta
	//! \param subject Subject którego sesje będziemy czytać
	//! \param photo Zdjęcie pacjenta
	//! \param userData Dane użytkownika (na potrzeby notatek!!)
	void setPatient(const hmdbServices::MedicalShallowCopy::Patient * patient = nullptr,
		const hmdbServices::MotionShallowCopy::Performer * subject = nullptr,
		const QPixmap & photo = QPixmap());

	void innerSetPatient(const hmdbServices::MedicalShallowCopy::Patient * patient, const QPixmap & photo, const hmdbServices::MotionShallowCopy::Performer * subject);

	private slots:

	void setPersonalData(const hmdbServices::MedicalShallowCopy::Patient * patient = nullptr, const QPixmap & photo = QPixmap());
	void setDisordersData(const hmdbServices::MedicalShallowCopy::PatientDisorders & patientDisorders);
	void setSessionsData(const hmdbServices::MotionShallowCopy::PerformerConfs & subjectConfigurations);
	void setNotes(const hmdbServices::MedicalShallowCopy::Patient * patient,
		const hmdbServices::MotionShallowCopy::Performer * subject);
	void setAntropometricData(const hmdbServices::MotionShallowCopy::Attrs & attributes);

	void currentSessionChanged(QTreeWidgetItem * prev, QTreeWidgetItem * current);

private:

	static QString getAttribute(const hmdbServices::MotionShallowCopy::Attrs & attributes, const std::string & name, const QString & defaultVal = QString("-"));

	static QTreeWidgetItem * createBranch(const QString & name, const std::map<int, const hmdbServices::MotionShallowCopy::Session *> & sessions);

private:

	QTreeWidget * sessionsWidget;

	PersonalDataWidget * personalDataWidget;

	DisordersDataWidget * disordersDataWidget;

	AntropometricDataWidget * antropomtricDataWidget;

	NotesWidget * notesWidget;
};

class DefaultPatientCard : public hmdbCommunication::IPatientCard
{
public:

	DefaultPatientCard();
	virtual ~DefaultPatientCard();

	//! \return Nazwa schematu karty pacjenta
	virtual const std::string name();
	//! \return Widget z kartą pacjenta
	virtual QWidget * widget();
	//! \param patient Pacjent którego należy sformatować w karcie pacjenta - dla nullptr należy zresetować kartę pacjenta
	//! \param subject Subject którego sesje będziemy czytać
	//! \param photo Zdjęcie pacjenta
	//! \param userData Dane użytkownika (na potrzeby notatek!!)
	virtual void setPatient(const hmdbServices::MedicalShallowCopy::Patient * patient = nullptr,
		const hmdbServices::MotionShallowCopy::Performer * subject = nullptr,
		const QPixmap & photo = QPixmap());

private:

	DefaultPatientCardWidget * patientCardWidget;
};

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DEFAULTPATIENTCARDWIDGET_H__
