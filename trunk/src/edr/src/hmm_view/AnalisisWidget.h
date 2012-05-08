/********************************************************************
	created:	2011/10/21
	created:	21:10:2011   11:09
	filename: 	AnalisisWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__ANALISISWIDGET_H__
#define HEADER_GUARD_HMM__ANALISISWIDGET_H__

#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include "DataFilterWidget.h"
#include "ui_AnalisisWidget.h"

class AnalisisTreeWidget : public QTreeWidget
{
    Q_OBJECT;
public:
    explicit AnalisisTreeWidget(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void contextMenuEvent ( QContextMenuEvent * event );
};


class AnalisisWidget : public QWidget, public Ui::AnalisisWidget
{
    Q_OBJECT;
public:
    AnalisisWidget(QWidget* parent, HmmMainWindow* hmm, int margin = 2, Qt::WindowFlags flags = 0);
	virtual ~AnalisisWidget() {}

public:
    QTreeWidget* getTreeWidget() { return this->treeWidget; }
    QWidget* getRaportsThumbnailList() { return raportsArea; }
    void addDataFilterWidget(DataFilterWidget* filter);
    QWidget* getArea() { return analisisArea; }

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

private slots:
    void setFiltersExpanded(bool expand);
    void filterClicked(FilterEntryWidget* filter);
    void applyClicked();
    void switchToFirstTab();
    void recreateTree(FilterEntryWidget* currentFilter);

private:
    int filterWidth;
    int filterHeight;
    int margin;
    Qt::MouseButton lastMouseButton;
    AnalisisTreeWidget* treeWidget;
    HmmMainWindow* hmm;
    //TODO potrzebne tylko, aby przekazac info miedzy elementami.
    FilterEntryWidget* currentFilter;

};



#endif
