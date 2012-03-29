/********************************************************************
    created:  2012/03/12
    created:  12:3:2012   17:49
    filename: NotesWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___NOTESWIDGET_H__
#define HEADER_GUARD___NOTESWIDGET_H__

#include "ui_NotesWidget.h"
#include <QtGui/QFrame>
#include <QtGui/QDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtCore/QDateTime>
#include <core/SmartPtr.h>

class NotesWidget : public QFrame, private Ui::NotesWidget
{
	Q_OBJECT

public:

	NotesWidget(QWidget * parent = nullptr);
	virtual ~NotesWidget();

public slots:

	//! \param patientID Identyfikator aktualnego pacjenta
	void setCurrentPatient(int patientID);

	//! \param patientID Identyfikator pacjenta kt�emu dodajemy notatk�
	//! \param created Czas utworzenia notatki
	//! \param title Tytu� notatki
	//! \param text Tre�� notatki
	void addNote(int patientID, const QDateTime & created, const QString & title, const QString & text);
	//! \param created Czas utworzenia notatki
	//! \param title Tytu� notatki
	//! \param text Tre�� notatki
	void addNote(const QDateTime & created, const QString & title, const QString & text);

	//! Usuwa aktualn� notatk�
	void removeNote();

	//! Wywo�uje dialog dodawania notatki
	void addNoteDialog();
	//! Wywo�uje dialog edycji notatki
	void editNoteDialog();

	//! Czy�ci zawarto�� pola notatki
	void clearNote();

	//! Wype�nia list� notatek (tabel�)
	void fillNotesList();
	//! Czy�ci liste notatek (tabel�)
	void clearNotesList();

private slots:

	//! �aduje text aktualnej notatki
	void loadNote();
	//! \param created Czas utworzenia notatki
	//! \param title Tytu� notatki
	//! \param text Tre�� notatki
	void updateNote(const QDateTime & modified, const QString & title, const QString & text);
	//! Prze�adowywuje notatki po zmianie u�ytkownika
	void reloadPatientNotes();

	//! Reakcja na zmian� notatki
	void onCurrentNoteChange();
	//! Weryfikuje dane dialogu edycji i nowej notatki
	void noteDialogConfirm();

private:
	//! \return Unikalne ID notatki
	int generateNoteUniqueID();

private:
	//! Struktura opisuj�ca notatk�
	struct NoteData {
		//! Tytu�
		QString title;
		//! Tre��
		QString text;
		//! Lokalne ID w obr�bie pacjenta
		int localID;
		//! Globalne ID
		int globalID;
		//! ID pacjenta
		int patientID;
		//! Czas utworzenia
		QDateTime created;
		//! Czas modyfikacji
		QDateTime modified;
	};

	//! Agregat indeksowanych notatek
	typedef std::map<int, core::shared_ptr<NoteData>> Notes;
	//! Notatki wg pacjent�w
	typedef std::map<int, Notes> PatientNotes;

private:
	//! ID Aktualnego pacjenta
	int currentPatientID;
	//! Aktualna notatka
	core::shared_ptr<NoteData> currentNote;
	//! Notatki
	Notes notes;
	//! Notatki wg pacjent�w
	PatientNotes patientNotes;
};

class NoteDialog : public QDialog
{
	Q_OBJECT

public:

	NoteDialog(QWidget * parent = nullptr);
	virtual ~NoteDialog();

	QLineEdit * titleEdit;
	QLineEdit * textEdit;

	QPushButton * acceptButton;
	QPushButton * cancelButton;

signals:

	void acceptRequest();

private slots:

	void buttonAccepted();
};

#endif	//	HEADER_GUARD___NOTESWIDGET_H__
