/********************************************************************
    created:  2011/04/28
    created:  28:4:2011   16:34
    filename: EDRDockWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__EDRDOCKWIDGET_H__
#define HEADER_GUARD_CORE__EDRDOCKWIDGET_H__

#include <QtGui/QDockWidget>
#include "EDRTitleBar.h"
#include "EDRInnerWidget.h"

class EDRDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit EDRDockWidget(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit EDRDockWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = 0);

    ~EDRDockWidget();

public slots:
    //! Zmienia stan dokowania.
    void toggleFloating();
    //! \param floating Czy widget ma byæ oddokowany?
    void setFloating(bool floating);

    EDRInnerWidget * getInnerWidget();
    const EDRInnerWidget * getInnerWidget() const;

    EDRTitleBar * getTitleBar();
    const EDRTitleBar * getTitleBar() const;

private:
    void init();

private:
    EDRTitleBar * titleBar;
    EDRInnerWidget * innerWidget;
};

#endif  //  HEADER_GUARD_CORE__EDRDOCKWIDGET_H__

