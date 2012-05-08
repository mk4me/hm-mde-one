#include "TimelinePCH.h"
//#include <qwt/qwt_scale_draw.h>
#include <plugins/newTimeline/TimeSliderWidget.h>

QTime convertToQTime(double timeInSeconds)
{
    int seconds = 0;

    if(time >= 0){
        seconds = floor(timeInSeconds);
    }else{
        seconds = ceil(timeInSeconds);
    }

    return QTime(0,0, seconds, (timeInSeconds - seconds) * 1000.0);
}

double convertToTime(const QTime & time)
{
    return (double)(time.hour() * 3600 + time.minute() * 60 + time.second()) + time.msec() / 1000.0;
}

class TimeScaleDraw : public QwtScaleDraw
{
public:

    TimeScaleDraw() {}
    virtual ~TimeScaleDraw() {}

    virtual QwtText label( double value) const
    {
        return QwtText(convertToQTime(value).toString(QString("mm:ss:zzz")));
    }
};

TimeSliderWidget::TimeSliderWidget(QWidget *parent, Qt::Orientation orientation,
    ScalePos scalePos, BackgroundStyles bgStyle)
    : QwtSlider(parent, orientation, scalePos, bgStyle), leftMargin(0), rightMargin(0)
{
    setScaleDraw(new TimeScaleDraw());
}

TimeSliderWidget::~TimeSliderWidget()
{

}

QwtScaleDraw * TimeSliderWidget::scaleDraw()
{
    return QwtSlider::scaleDraw();
}

void TimeSliderWidget::drawSlider(QPainter *painter, const QRect &sliderRect ) const
{
    QwtSlider::drawSlider(painter, sliderRect);

    bool changed = false;

    if(leftMargin != sliderRect.left() + handleSize().width() / 2){
        leftMargin = sliderRect.left() + handleSize().width() / 2;
        changed = true;
    }

    if(rightMargin != this->rect().right() - sliderRect.right() + handleSize().width() / 2){
        rightMargin = this->rect().right() - sliderRect.right() + handleSize().width() / 2;
        changed = true;
    }

    if(changed == true){
        emit marginsChanged(leftMargin, rightMargin);
    }
}

double TimeSliderWidget::getLeftMargin() const
{
    return leftMargin;
}

double TimeSliderWidget::getRightMargin() const
{
    return rightMargin;
}