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

//! Widget realizujący prezentację danych antropometrycznych
//! Robi to w oparciu o generowanie/formatowanie/stylowanie tabel HTML za pomocą CSS
class AntropometricDataWidget : public QTextEdit
{
	Q_OBJECT

public:
	//! Konstruktor
	AntropometricDataWidget(QWidget * parent = nullptr);
	//! Destruktor
	virtual ~AntropometricDataWidget();

public slots:
	//! \param autUpdate Czy przy zmianie dowolnego parametru danych antropometrycznych widget ma się odświeżyć - wygenerować tabele HTML, ....
	void setAutoUpdate(bool autoUpdate = true);

	//! Funkcje seterów wartości

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

	//! \param val Wartość która zastąpi wszystkie wpisy w tabelach
	void resetAntropometricData(const QString & val = QString("-"));

private:
	//! Inicjuje tabele HTML
	void initAntropometric();
	//! Inicjuje nagłówki tabel
	void initAntropometricHeadersStructure();
	//! Inicjuje zawartośc tabel
	void initAntropometricContent();
	//! Inicjuje style tabel
	void initAntropometricStyles();
	//! Inicjucje atrybuty tabel
	void initAntropometricAtributes();

	//! Odświeża zawartośc tabel HTML - generuje/formatuje i styluje od zera
	void updateContent();
	//! Próbuje odświeżać zawartość tabel jeśli jest to konieczne i możliwe
	void tryUpdateContent();
	// Próbuje zmienić zawartośc tabeli i odświeżyć ją
	void tryChangeContent();

private:
	//! Struktura do opisu tabeli z danymi
	struct AntropometricTabel {
		//! Zawartość tabeli
		htmlGen::HtmlDataTableContent antropometricContent;
		//! Ukłąd nagłóków tabeli
		htmlGen::TableHeadersStructureDescriptor antropometricHeadersStructure;
		//! Atrybuty poszczególnych komórek tabeli
		htmlGen::HtmlDataTableCellAttributes antropometricCellsAttributes;
		//! Style tabeli
		htmlGen::HtmlDataTableStyles antropometricStyles;
	};

private:

	//! Czy należy przy każdej zmianie aktualizować
	bool autoUpdate_;
	//! Czy nastąpiła zmiana zawartości
	bool changed_;
	//! Opis danych antropometrycznych ogólnych
	AntropometricTabel antropometricGeneralTable;
	//! Opis danych antropometrycznych z podziałem na lewą i prawą stronę
	AntropometricTabel antropometricLeftRightTable;
};

#endif // HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__
