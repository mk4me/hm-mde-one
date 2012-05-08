#include "CommunicationPCH.h"
#include "AntropometricDataWidget.h"
#include <qthtmlgenlib/DocumentGenerator.h>


AntropometricDataWidget::AntropometricDataWidget(QWidget * parent) : QTextEdit(parent)
{
	setReadOnly(true);
	initAntropometric();
}

AntropometricDataWidget::~AntropometricDataWidget()
{

}

void AntropometricDataWidget::setAutoUpdate(bool autoUpdate)
{
	autoUpdate_ = autoUpdate;
	tryUpdateContent();
}

void AntropometricDataWidget::setBodyMass(const QString & val)
{
	antropometricGeneralTable.antropometricContent.setCell(1, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setBodyHeight(const QString & val)
{
	antropometricGeneralTable.antropometricContent.setCell(2, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setASIS(const QString & val)
{
	antropometricGeneralTable.antropometricContent.setCell(3, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftLegLength(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(1, 2, val);
	tryChangeContent();	
}

void AntropometricDataWidget::setRightLegLenght(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(1, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftKneeWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(2, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightKneeWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(2, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftAnkleWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(3, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightAnkleWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(3, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftCircuitThigh(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(4, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightCircuitThight(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(4, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftCircuitShank(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(5, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightCircuitShank(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(5, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftShoulderOffset(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(6, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightShoulderOffset(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(6, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftElbowWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(7, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightElbowWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(7, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftWristWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(8, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightWristWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(8, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftWristThickness(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(9, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightWristThickness(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(9, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftHandWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(10, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightHandWidth(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(10, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::setLeftHandThickness(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(11, 2, val);
	tryChangeContent();
}

void AntropometricDataWidget::setRightHandThickness(const QString & val)
{
	antropometricLeftRightTable.antropometricContent.setCell(11, 3, val);
	tryChangeContent();
}

void AntropometricDataWidget::resetAntropometricData(const QString & val)
{
	for(int i = 1; i < 4; ++i){
		antropometricGeneralTable.antropometricContent.setCell(i, 2, val);
	}

	for(int i = 1; i < 12; ++i){
		antropometricLeftRightTable.antropometricContent.setCell(i, 2, val);
		antropometricLeftRightTable.antropometricContent.setCell(i, 3, val);
	}

	tryChangeContent();
}

void AntropometricDataWidget::updateContent()
{
	QString html;

	DocumentGenerator::openDocument(html);
	DocumentGenerator::openBody(html);

	QString genTable;

	HtmlDataTableGenerator::generateHtmlTable(genTable, antropometricGeneralTable.antropometricContent, antropometricGeneralTable.antropometricHeadersStructure,
		antropometricGeneralTable.antropometricCellsAttributes, antropometricGeneralTable.antropometricStyles);

	QString lrTable;

	HtmlDataTableGenerator::generateHtmlTable(lrTable, antropometricLeftRightTable.antropometricContent, antropometricLeftRightTable.antropometricHeadersStructure,
		antropometricLeftRightTable.antropometricCellsAttributes, antropometricLeftRightTable.antropometricStyles);

	html += genTable;

	html += "<p></p>";

	html += lrTable;

	DocumentGenerator::closeBody(html);
	DocumentGenerator::closeDocument(html);

	setHtml(html);
}

void AntropometricDataWidget::tryUpdateContent()
{
	if(autoUpdate_ == true && changed_ == true){
		updateContent();
		changed_ = false;
	}
}

void AntropometricDataWidget::tryChangeContent()
{
	if(autoUpdate_ == true){
		updateContent();
		changed_ = false;
	}else{
		changed_ = true;
	}
}

void AntropometricDataWidget::initAntropometric()
{
	initAntropometricContent();
	initAntropometricHeadersStructure();
	initAntropometricAtributes();
	initAntropometricStyles();
}

void AntropometricDataWidget::initAntropometricHeadersStructure()
{
	antropometricGeneralTable.antropometricHeadersStructure.headerRows = 1;
	antropometricGeneralTable.antropometricHeadersStructure.headerColumns = 1;

	antropometricLeftRightTable.antropometricHeadersStructure.headerRows = 1;
	antropometricLeftRightTable.antropometricHeadersStructure.headerColumns = 1;
}

void AntropometricDataWidget::initAntropometricContent()
{
	//wymiary
	antropometricGeneralTable.antropometricContent.setDimensions(4, 3);
	antropometricLeftRightTable.antropometricContent.setDimensions(12, 4);

	//zawartoœæ tabeli z danymi ogólnymi
	//! wiersz nag³ówkowy - pierwsza komórka pusta - ³¹cznik z nag³ówkiem pionowym
	antropometricGeneralTable.antropometricContent.setCell(0, 1, tr("Unit"));
	antropometricGeneralTable.antropometricContent.setCell(0, 2, tr("Value"));
	//! kolumna nag³ówkowa - pierwsza komórka pusta - ³¹cznik z nag³ówkiem poziomym
	antropometricGeneralTable.antropometricContent.setCell(1, 0, tr("Body Mass"));
	antropometricGeneralTable.antropometricContent.setCell(2, 0, tr("Height"));
	antropometricGeneralTable.antropometricContent.setCell(3, 0, tr("ASIS-ASIS"));
	//! pierwsza kolumna z jednostkami
	antropometricGeneralTable.antropometricContent.setCell(1, 1, "[" + tr("kg") + "]");
	antropometricGeneralTable.antropometricContent.setCell(2, 1, "[" + tr("mm") + "]");
	antropometricGeneralTable.antropometricContent.setCell(3, 1, "[" + tr("mm") + "]");

	//zawartoœæ tabeli z danymi szczegó³owymi dla lewej i prawej storny
	//! wiersz nag³ówkowy - pierwsza komórka pusta - ³¹cznik z nag³ówkiem pionowym
	antropometricLeftRightTable.antropometricContent.setCell(0, 1, tr("Unit"));
	antropometricLeftRightTable.antropometricContent.setCell(0, 2, tr("Left Value"));
	antropometricLeftRightTable.antropometricContent.setCell(0, 3, tr("Right Value"));
	//! kolumna nag³ówkowa - pierwsza komórka pusta - ³¹cznik z nag³ówkiem poziomym
	antropometricLeftRightTable.antropometricContent.setCell(1, 0, tr("Leg Length"));
	antropometricLeftRightTable.antropometricContent.setCell(2, 0, tr("Knee Width"));
	antropometricLeftRightTable.antropometricContent.setCell(3, 0, tr("Anckle Width"));
	antropometricLeftRightTable.antropometricContent.setCell(4, 0, tr("Circuit Thight"));
	antropometricLeftRightTable.antropometricContent.setCell(5, 0, tr("Circuit Shank"));
	antropometricLeftRightTable.antropometricContent.setCell(6, 0, tr("Shoulder Offset"));
	antropometricLeftRightTable.antropometricContent.setCell(7, 0, tr("Elbow Width"));
	antropometricLeftRightTable.antropometricContent.setCell(8, 0, tr("Wrist Width"));
	antropometricLeftRightTable.antropometricContent.setCell(9, 0, tr("Wrist Thickness"));
	antropometricLeftRightTable.antropometricContent.setCell(10, 0, tr("Hand Width"));
	antropometricLeftRightTable.antropometricContent.setCell(11, 0, tr("Hand Thickness"));

	//! pierwsza kolumna z jednostkami
	antropometricLeftRightTable.antropometricContent.setCell(1, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(2, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(3, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(4, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(5, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(6, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(7, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(8, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(9, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(10, 1, "[" + tr("mm") + "]");
	antropometricLeftRightTable.antropometricContent.setCell(11, 1, "[" + tr("mm") + "]");
}

void AntropometricDataWidget::initAntropometricStyles()
{
	antropometricGeneralTable.antropometricStyles.tableAttributes.setCellPadding(4);
	antropometricGeneralTable.antropometricStyles.tableAttributes.setCellSpacing(0);
	antropometricGeneralTable.antropometricStyles.tableAttributes.setWidth(100, WRelative);
	antropometricGeneralTable.antropometricStyles.tableAttributes.setHeight(100, WRelative);
	antropometricGeneralTable.antropometricStyles.tableAttributes.setBorder(1);

	antropometricLeftRightTable.antropometricStyles.tableAttributes.setCellPadding(4);
	antropometricLeftRightTable.antropometricStyles.tableAttributes.setCellSpacing(0);
	antropometricLeftRightTable.antropometricStyles.tableAttributes.setWidth(100, WRelative);
	antropometricLeftRightTable.antropometricStyles.tableAttributes.setHeight(100, WRelative);
	antropometricLeftRightTable.antropometricStyles.tableAttributes.setBorder(1);

	antropometricGeneralTable.antropometricStyles.tableStyle_ = "border-color: rgb(0, 0, 0);";
	antropometricGeneralTable.antropometricStyles.headerRowStyle_[Single] = "color: white; background: rgb(41, 41, 41);";

	//wiersz nag³ówkowy
	antropometricGeneralTable.antropometricStyles.headersStyles[Single][Middle] = "color: white; border-style: none; border-color: white;";
	// kolumna nag³ówkowa - musze dodaæ kolor, nie da siê kolumnom przypisywaæ w³aœciwoœci
	antropometricGeneralTable.antropometricStyles.headersStyles[Middle][Single] = "color: white; border-style: none; border-color: white; background: rgb(41, 41, 41);";
	// ostatni element kolumny nag³ówkowej - tylko kolor
	antropometricGeneralTable.antropometricStyles.headersStyles[Last][Single] = "color: white; background: rgb(41, 41, 41);";

	//style contentu - tutaj chcemy uzyskaæ efekt kratownicy bez ramki
	antropometricGeneralTable.antropometricStyles.contentStyles[First][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricGeneralTable.antropometricStyles.contentStyles[Last][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricGeneralTable.antropometricStyles.contentStyles[Last][Last] = "border-style: none; border-color: rgb(41, 41, 41);";

	// kopiujemy wiêkszoœæ ustawieñ
	antropometricLeftRightTable.antropometricStyles = antropometricGeneralTable.antropometricStyles;
	//poprawiamy style contentu - wiêcej mo¿liwoœci
	antropometricLeftRightTable.antropometricStyles.contentStyles[First][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricLeftRightTable.antropometricStyles.contentStyles[First][Middle] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricLeftRightTable.antropometricStyles.contentStyles[First][Last] = QString();

	antropometricLeftRightTable.antropometricStyles.contentStyles[Middle][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricLeftRightTable.antropometricStyles.contentStyles[Middle][Middle] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricLeftRightTable.antropometricStyles.contentStyles[Middle][Last] = "border-style: none; border-color: rgb(41, 41, 41);";

	antropometricLeftRightTable.antropometricStyles.contentStyles[Last][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricLeftRightTable.antropometricStyles.contentStyles[Last][Middle] = "border-style: none; border-color: rgb(41, 41, 41);";
	antropometricLeftRightTable.antropometricStyles.contentStyles[Last][Last] = "border-style: none; border-color: rgb(41, 41, 41);";
}

void AntropometricDataWidget::initAntropometricAtributes()
{
	antropometricLeftRightTable.antropometricCellsAttributes.setDimensions(antropometricLeftRightTable.antropometricContent.rows(), antropometricLeftRightTable.antropometricContent.columns());
	antropometricGeneralTable.antropometricCellsAttributes.setDimensions(antropometricGeneralTable.antropometricContent.rows(), antropometricGeneralTable.antropometricContent.columns());

	HtmlCellAttributes attributes;
	attributes.setHAlign(HACenter);
	attributes.setVAlign(VAMiddle);

	//centrujemy dane
	for(int i = 0; i < antropometricLeftRightTable.antropometricCellsAttributes.rows(); ++i){
		for(int j = 0; j < antropometricLeftRightTable.antropometricCellsAttributes.columns(); ++j){
			antropometricLeftRightTable.antropometricCellsAttributes.setCell(i, j, attributes);
		}
	}

	for(int i = 0; i < antropometricGeneralTable.antropometricCellsAttributes.rows(); ++i){
		for(int j = 0; j < antropometricGeneralTable.antropometricCellsAttributes.columns(); ++j){
			antropometricGeneralTable.antropometricCellsAttributes.setCell(i, j, attributes);
		}
	}

	attributes.setHAlign(HALeft);

	for(int i = 0; i < antropometricLeftRightTable.antropometricCellsAttributes.rows(); ++i){
		for(int j = 0; j < antropometricLeftRightTable.antropometricHeadersStructure.headerColumns; ++j){
			antropometricLeftRightTable.antropometricCellsAttributes.setCell(i, j, attributes);
		}
	}

	for(int i = 0; i < antropometricGeneralTable.antropometricCellsAttributes.rows(); ++i){
		for(int j = 0; j < antropometricGeneralTable.antropometricHeadersStructure.headerColumns; ++j){
			antropometricGeneralTable.antropometricCellsAttributes.setCell(i, j, attributes);
		}
	}

	antropometricLeftRightTable.antropometricCellsAttributes.cell(0,1).setWidth(10, WRelative);
	antropometricLeftRightTable.antropometricCellsAttributes.cell(0,2).setWidth(25, WRelative);
	antropometricLeftRightTable.antropometricCellsAttributes.cell(0,3).setWidth(25, WRelative);

	antropometricGeneralTable.antropometricCellsAttributes.cell(0,1).setWidth(10, WRelative);
	antropometricGeneralTable.antropometricCellsAttributes.cell(0,2).setWidth(50, WRelative);
}