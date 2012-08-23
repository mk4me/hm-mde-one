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

//! Widget realizuj¹cy prezentacjê danych antropometrycznych
//! Robi to w oparciu o generowanie/formatowanie/stylowanie tabel HTML za pomoc¹ CSS
class AntropometricDataWidget : public QTextEdit
{
	Q_OBJECT

public:
	//! Konstruktor
	AntropometricDataWidget(QWidget * parent = nullptr);
	//! Destruktor
	virtual ~AntropometricDataWidget();

public slots:
	//! \param autUpdate Czy przy zmianie dowolnego parametru danych antropometrycznych widget ma siê odœwie¿yæ - wygenerowaæ tabele HTML, ....
	void setAutoUpdate(bool autoUpdate = true);

	//! Funkcje seterów wartoœci

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

	//! \param val Wartoœæ która zast¹pi wszystkie wpisy w tabelach
	void resetAntropometricData(const QString & val = QString("-"));

private:
	//! Inicjuje tabele HTML
	void initAntropometric();
	//! Inicjuje nag³ówki tabel
	void initAntropometricHeadersStructure();
	//! Inicjuje zawartoœc tabel
	void initAntropometricContent();
	//! Inicjuje style tabel
	void initAntropometricStyles();
	//! Inicjucje atrybuty tabel
	void initAntropometricAtributes();

	//! Odœwie¿a zawartoœc tabel HTML - generuje/formatuje i styluje od zera
	void updateContent();
	//! Próbuje odœwie¿aæ zawartoœæ tabel jeœli jest to konieczne i mo¿liwe
	void tryUpdateContent();
	// Próbuje zmieniæ zawartoœc tabeli i odœwie¿yæ j¹
	void tryChangeContent();

private:
	//! Struktura do opisu tabeli z danymi
	struct AntropometricTabel {
		//! Zawartoœæ tabeli
		htmlGen::HtmlDataTableContent antropometricContent;
		//! Uk³¹d nag³óków tabeli
		htmlGen::TableHeadersStructureDescriptor antropometricHeadersStructure;
		//! Atrybuty poszczególnych komórek tabeli
		htmlGen::HtmlDataTableCellAttributes antropometricCellsAttributes;
		//! Style tabeli
		htmlGen::HtmlDataTableStyles antropometricStyles;
	};

private:

	//! Czy nale¿y przy ka¿dej zmianie aktualizowaæ
	bool autoUpdate_;
	//! Czy nast¹pi³a zmiana zawartoœci
	bool changed_;
	//! Opis danych antropometrycznych ogólnych
	AntropometricTabel antropometricGeneralTable;
	//! Opis danych antropometrycznych z podzia³em na lew¹ i praw¹ stronê
	AntropometricTabel antropometricLeftRightTable;
};

#endif // HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__