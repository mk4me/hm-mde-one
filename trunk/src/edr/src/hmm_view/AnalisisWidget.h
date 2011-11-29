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
public:
    AnalisisWidget(QWidget* parent, int margin = 2, Qt::WindowFlags flags = 0);
	virtual ~AnalisisWidget() {}

public:
    QTreeWidget* getTreeWidget() { return this->treeWidget; }

    void setActivePixmapAndText(const QPixmap& pixmap, const QString& name) 
    {
        label->setText(name);
        picture->setPixmap(pixmap);
    }
    void addDataFilterWidget(DataFilterWidget* filter);

    QWidget* getArea() { return analisisArea; }

protected:
    virtual bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
            lastMouseButton = mouse->button();
        }

        return false;
    }


private:
    int filterWidth;
    int filterHeight;
    int margin;
    Qt::MouseButton lastMouseButton;
    AnalisisTreeWidget* treeWidget;
};



#endif