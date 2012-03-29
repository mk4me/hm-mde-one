#include "CommunicationPCH.h"
#include "DisordersDataWidget.h"
#include <qthtmlgenlib/DocumentGenerator.h>
#include <qthtmlgenlib/TableGenerator.h>

DisordersDataWidget::DisordersDataWidget(QWidget * parent) : QTextEdit(parent)
{
	initDisorders();
	setReadOnly(true);
}

DisordersDataWidget::~DisordersDataWidget()
{

}

void DisordersDataWidget::setAutoUpdate(bool autoUpdate)
{
	autoUpdate_ = autoUpdate;
	tryUpdateContent();
}

void DisordersDataWidget::setDisordersCount(int count)
{
	disordersTable.content.setRows(count + 1);
	clearDisorders();
}

void DisordersDataWidget::clearDisorders()
{
	bool tmpAutoUpdate = autoUpdate_;
	setAutoUpdate(false);
	auto last = disordersTable.content.rows() - 1;
	for(int i = 0; i < last; ++i){
		setDisorder(i, QString(), QString(), QString());
	}

	setAutoUpdate(tmpAutoUpdate);
}

void DisordersDataWidget::setDisorder(int idx, const QString & name, const QString & diagnosisDate, const QString & focus)
{
	int row = idx + 1;
	disordersTable.content.setCell(row, 0, QString::number(row));
	disordersTable.content.setCell(row, 1, name);
	disordersTable.content.setCell(row, 2, diagnosisDate);
	disordersTable.content.setCell(row, 3, focus);

	tryChangeContent();
}

void DisordersDataWidget::initDisorders()
{
	initDisordersContent();
	initDisordersAtributes();
	initDisordersHeaderStructure();
	initDisordersStyles();
}

void DisordersDataWidget::initDisordersHeaderStructure()
{
	disordersTable.headersStructure.headerRows = 1;
	disordersTable.headersStructure.headerColumns = 1;
}

void DisordersDataWidget::initDisordersContent()
{
	//wymiary
	disordersTable.content.setDimensions(3, 4);

	//zawarto�� tabeli z danymi og�lnymi
	//! wiersz nag��wkowy - pierwsza kom�rka pusta - ��cznik z nag��wkiem pionowym
	disordersTable.content.setCell(0, 1, tr("Disorder"));
	disordersTable.content.setCell(0, 2, tr("Diagnosis date"));
	disordersTable.content.setCell(0, 3, tr("Focus"));
	//! kolumna nag��wkowa - pierwsza kom�rka pusta - ��cznik z nag��wkiem poziomym
	disordersTable.content.setCell(1, 0, QString::number(1));
	disordersTable.content.setCell(2, 0, QString::number(2));	
}

void DisordersDataWidget::initDisordersStyles()
{
	disordersTable.styles.tableStyle_ = "width: 300px; border-style: solid; border-color: rgb(0, 0, 0); border-radius: 4px;";
	disordersTable.styles.headerRowStyle_[Single] = "color: white; background: rgb(41, 41, 41);";

	//wiersz nag��wkowy
	disordersTable.styles.headersStyles[Single][Middle] = "color: white; border-style: none; border-color: white;";
	// kolumna nag��wkowa - musze doda� kolor, nie da si� kolumnom przypisywa� w�a�ciwo�ci
	disordersTable.styles.headersStyles[Middle][Single] = "color: white; border-style: none; border-color: white; background: rgb(41, 41, 41);";
	// ostatni element kolumny nag��wkowej - tylko kolor
	disordersTable.styles.headersStyles[Last][Single] = "color: white; background: rgb(41, 41, 41);";

	//style contentu - tutaj chcemy uzyska� efekt kratownicy bez ramki
	disordersTable.styles.contentStyles[First][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	disordersTable.styles.contentStyles[Last][First] = "border-style: none; border-color: rgb(41, 41, 41);";
	disordersTable.styles.contentStyles[Last][Last] = "border-style: none; border-color: rgb(41, 41, 41);";
}

void DisordersDataWidget::initDisordersAtributes()
{

}

void DisordersDataWidget::updateContent()
{
	QString html;

	DocumentGenerator::openDocument(html);
	DocumentGenerator::openBody(html);

	QString disTable;

	HtmlDataTableGenerator::generateHtmlTable(disTable, disordersTable.content, disordersTable.headersStructure,
		disordersTable.cellsAttributes, disordersTable.styles);

	html += disTable;

	DocumentGenerator::closeBody(html);
	DocumentGenerator::closeDocument(html);

	setHtml(html);
}

void DisordersDataWidget::tryUpdateContent()
{
	if(autoUpdate_ == true && changed_ == true){
		updateContent();
		changed_ = false;
	}
}

void DisordersDataWidget::tryChangeContent()
{
	if(autoUpdate_ == true){
		updateContent();
		changed_ = false;
	}else{
		changed_ = true;
	}
}