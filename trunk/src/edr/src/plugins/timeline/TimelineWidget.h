/********************************************************************
	created:  2010/10/07
	created:  7:10:2010   13:12
	filename: TimelineWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEWIDGET_H__
#define __HEADER_GUARD__TIMELINEWIDGET_H__

#include <QtGui/QWidget>
#include <plugins/timeline/Controller.h>
#include <plugins/timeline/TimelineService.h>
#include <plugins/timeline/View.h>
#include "ui_TimelineWidget.h"

class TimelineWidget : public QWidget, public Ui::TimelineWidget, public timeline::View
{
    Q_OBJECT

private:
    //! Us�uga tworz�ca widget.
    TimelineService* service;

public:
    //! 
    TimelineWidget(TimelineService* service, timeline::Controller * controller);
    //! 
    virtual ~TimelineWidget();

public slots:
    //! \param 
    void timeSliderChanged(int value);
    void playButtonPressed();
    void pauseButtonPressed();
    void stopButtonPressed();
    void timeSliderPressed();
    void timeSliderReleased();

public:
    //! Aktualizacja obserwatora.
    //! \param subject Obiekt podany obserwacji.
    virtual void update(const timeline::State * subject);
    void setBusy(bool busy);

private:
    double getSliderValue(QSlider* slider) const;
    void setSliderValue(double value, QSlider* slider);

    int getSliderValue(QSlider* slider, double normalizedValue);

private:
    timeline::Controller * controller;
};

#endif  // __HEADER_GUARD__TIMELINEWIDGET_H__