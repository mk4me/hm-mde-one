/********************************************************************
    created:  2011/04/19
    created:  19:4:2011   7:56
    filename: VdfWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___VDFWIDGET_H__
#define HEADER_GUARD___VDFWIDGET_H__

#include <QtGui/QWidget>
#include <plugins/vdf/VdfService.h>
#include <vdfmlib/osgVDFModel.h>
#include <osgui/QOsgWidgets.h>
#include "ui_VdfWidget.h"

class VdfWidget : public QWidget, public Ui::VdfWidget
{
    Q_OBJECT

private:

    typedef std::set<osg::ref_ptr<osgui::StyleEx> > StylesSet;

private:
    //! Usługa tworząca widget.
    VdfService* service;

    QWidget* actualWidget;

    //! Widget z widokiem OSG.
    osgui::QOsgDefaultWidget* vdfQWidget;
    osg::ref_ptr<osgVDF::osgVDFBaseModel> vdfModel;

public:
    //! 
    VdfWidget(VdfService* service);
    //! 
    virtual ~VdfWidget();

protected:
    //virtual void mousePressEvent(QMouseEvent * event);

private:
    static std::string colorToStr(const osgWidget::Color & color);
    StylesSet generateRequiredStyles();
};

#endif  // HEADER_GUARD___VDFWIDGET_H__
