/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   8:04
    filename: AntropometricDataWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__
#define HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__

#include <qthtmlgenlib/TableGenerator.h>
#include <QtGui/QTextEdit>

//! Widget realizuj�cy prezentacj� danych antropometrycznych
//! Robi to w oparciu o generowanie/formatowanie/stylowanie tabel HTML za pomoc� CSS
class AntropometricDataWidget : public QTextEdit
{
	Q_OBJECT

public:
	//! Konstruktor
	AntropometricDataWidget(QWidget * parent = nullptr);
	//! Destruktor
	virtual ~AntropometricDataWidget();

public slots:
	//! \param autUpdate Czy przy zmianie dowolnego parametru danych antropometrycznych widget ma si� od�wie�y� - wygenerowa� tabele HTML, ....
	void setAutoUpdate(bool autoUpdate = true);

	//! Funkcje seter�w warto�ci

	void setBodyMass(const QString & val);
	void setBodyHeight(const QString & val);
	void setASIS(const QString & val);
	
	void setLeftLegLength(const QString & val);
	void setRightLegLenght(const QString & val);
	void setLeftKneeWidth(const QString & val);
	void setRightKneeWidth(const QString & val);
	void setLeftAnkleWidth(const QString & val);
	void setRightAnkleWidth(const QString & val);
	void setLeftCircuitThigh(const QString & val);
	void setRightCircuitThight(const QString & val);
	void setLeftCircuitShank(const QString & val);
	void setRightCircuitShank(const QString & val);
	void setLeftShoulderOffset(const QString & val);
	void setRightShoulderOffset(const QString & val);
	void setLeftElbowWidth(const QString & val);
	void setRightElbowWidth(const QString & val);
	void setLeftWristWidth(const QString & val);
	void setRightWristWidth(const QString & val);
	void setLeftWristThickness(const QString & val);
	void setRightWristThickness(const QString & val);
	void setLeftHandWidth(const QString & val);
	void setRightHandWidth(const QString & val);
	void setLeftHandThickness(const QString & val);
	void setRightHandThickness(const QString & val);

	//! \param val Warto�� kt�ra zast�pi wszystkie wpisy w tabelach
	void resetAntropometricData(const QString & val = QString("-"));

private:
	//! Inicjuje tabele HTML
	void initAntropometric();
	//! Inicjuje nag��wki tabel
	void initAntropometricHeadersStructure();
	//! Inicjuje zawarto�c tabel
	void initAntropometricContent();
	//! Inicjuje style tabel
	void initAntropometricStyles();
	//! Inicjucje atrybuty tabel
	void initAntropometricAtributes();

	//! Od�wie�a zawarto�c tabel HTML - generuje/formatuje i styluje od zera
	void updateContent();
	//! Pr�buje od�wie�a� zawarto�� tabel je�li jest to konieczne i mo�liwe
	void tryUpdateContent();
	// Pr�buje zmieni� zawarto�c tabeli i od�wie�y� j�
	void tryChangeContent();

private:
	//! Struktura do opisu tabeli z danymi
	struct AntropometricTabel {
		//! Zawarto�� tabeli
		htmlGen::HtmlDataTableContent antropometricContent;
		//! Uk��d nag��k�w tabeli
		htmlGen::TableHeadersStructureDescriptor antropometricHeadersStructure;
		//! Atrybuty poszczeg�lnych kom�rek tabeli
		htmlGen::HtmlDataTableCellAttributes antropometricCellsAttributes;
		//! Style tabeli
		htmlGen::HtmlDataTableStyles antropometricStyles;
	};

private:

	//! Czy nale�y przy ka�dej zmianie aktualizowa�
	bool autoUpdate_;
	//! Czy nast�pi�a zmiana zawarto�ci
	bool changed_;
	//! Opis danych antropometrycznych og�lnych
	AntropometricTabel antropometricGeneralTable;
	//! Opis danych antropometrycznych z podzia�em na lew� i praw� stron�
	AntropometricTabel antropometricLeftRightTable;
};

#endif // HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__