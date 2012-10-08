/********************************************************************
    created:  2011/12/12
    created:  12:12:2011   9:58
    filename: FlexiTabSectionWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_QT_EXT__FLEXITABSECTIONWIDGET_H__
#define HEADER_GUARD_QT_EXT__FLEXITABSECTIONWIDGET_H__

#include "ui_FlexiTabSectionWidget.h"

class FlexiTabSectionWidget : public QFrame, private Ui::FlexiTabBarSection
{
    Q_OBJECT;

public:
    FlexiTabSectionWidget(const QString & sectionName = "Default Section", const QPicture * picture = 0, QAction * configAction = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~FlexiTabSectionWidget();

    void setSectionName(const QString & sectionName);
    void setCfgButtonAction(QAction * configAction);
    void setPicture(const QPicture * picture);
    void setWidget(QWidget * widget);

private:
    QWidget * currentWidget;
};

#endif HEADER_GUARD_QT_EXT__FLEXITABSECTIONWIDGET_H__

