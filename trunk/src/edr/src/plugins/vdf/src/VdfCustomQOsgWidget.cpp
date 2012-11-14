#include "VdfCustomQOsgWidget.h"

VdfCustomQOsgWidget::VdfCustomQOsgWidget(QWidget * parent, const osg::GraphicsContext::Traits* traits, Qt::WindowFlags f)
    : osgui::QOsgDefaultWidget(parent, traits, f)
{

}

void VdfCustomQOsgWidget::mousePressEvent(QMouseEvent * event)
{
    emit customContextMenuRequested(event->pos());
    osgui::QOsgDefaultWidget::mousePressEvent(event);
}
