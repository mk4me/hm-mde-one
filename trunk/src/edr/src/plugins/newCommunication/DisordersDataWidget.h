/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   9:05
    filename: DisordersDataWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DISORDERSDATAWIDGET_H__
#define HEADER_GUARD___DISORDERSDATAWIDGET_H__

#include <qthtmlgenlib/TableGenerator.h>
#include <QtGui/QTextEdit>

class DisordersDataWidget : public QTextEdit
{
	Q_OBJECT

public:

	DisordersDataWidget(QWidget * parent = nullptr);
	virtual ~DisordersDataWidget();

public slots:

	void setAutoUpdate(bool autoUpdate = true);
	
	void setDisordersCount(int count);
	void clearDisorders();

	void setDisorder(int idx, const QString & name, const QString & diagnosisDate, const QString & focus);

private:

	void initDisorders();
	void initDisordersHeaderStructure();
	void initDisordersContent();
	void initDisordersStyles();
	void initDisordersAtributes();

	void updateContent();
	void tryUpdateContent();
	void tryChangeContent();

private:

	struct DisordersTabel {
		HtmlDataTableContent content;
		TableHeadersStructureDescriptor headersStructure;
		HtmlDataTableCellAttributes cellsAttributes;
		HtmlDataTableStyles styles;
	};

private:

	bool autoUpdate_;
	bool changed_;
	DisordersTabel disordersTable;
};


#endif	//	HEADER_GUARD___DISORDERSDATAWIDGET_H__