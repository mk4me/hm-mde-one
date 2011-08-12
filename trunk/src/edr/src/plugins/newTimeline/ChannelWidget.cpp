#include "TimelinePCH.h"
#include "ChannelWidget.h"

ChannelWidget::ChannelWidget(QWidget* parent, Qt::WindowFlags f, double begin, double end,
    double currentTimeHalfWidth, double currentTime)
    : QFrame(parent, f), begin(begin), end(end), currentTime(currentTime),
    currentTimeHalfWidth(currentTimeHalfWidth), fillColor("black"), backgroundColor("gray"),
    markerColor("white")
{
    UTILS_ASSERT((end <= 1.0 && begin <= end && begin >= 0.0), "Bledne krance kanalu w wizualizacji");
    UTILS_ASSERT((currentTime >= 0.0 && currentTime <= 1.0), "Bledny czas kanalu w wizualizacji");
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    setLineWidth ( 1 );
    setMidLineWidth ( 1 );
    setFrameShape ( QFrame::Box );
}

double ChannelWidget::getBegin() const
{ 
    return begin;
}

void ChannelWidget::setBegin(double begin) 
{ 
    UTILS_ASSERT((0.0 <= begin && begin <= end), "Bledny poczatek kanalu w wizualizacji");

    if(this->begin == begin){
        return;
    }

    this->begin = begin;
    update();
}

double ChannelWidget::getEnd() const
{ 
    return end;
}

void ChannelWidget::setEnd(double end) 
{ 
    UTILS_ASSERT((end <= 1.0 && begin <= end), "Bledny koniec kanalu w wizualizacji");
    if(this->end == end){
        return;
    }

    this->end = end;
    update();
}

double ChannelWidget::getCurrentTime() const
{
    return currentTime;
}

void ChannelWidget::setCurrentTime(double currentTime)
{
    UTILS_ASSERT((currentTime <= 1.0 && 0.0 <= currentTime), "Bledny czas kanalu");
    if(this->currentTime == currentTime){
        return;
    }

    this->currentTime = currentTime;
    update();
}

void ChannelWidget::setRange(double begin, double end)
{
    UTILS_ASSERT((end <= 1.0 && begin <= end && begin >= 0.0), "Bledne krance kanalu w wizualizacji");

    bool different = false;

    if(this->begin != begin){
        this->begin = begin;
        different = true;
    }

    if(this->end != end){
        this->end = end;
        different = true;
    }

    if(different == true){
        update();
    }
}

void ChannelWidget::setFillColor(const QColor & fillColor)
{
    if(this->fillColor != fillColor){
        this->fillColor = fillColor;
        update();
    }
}

void ChannelWidget::setBackgorundColor(const QColor & backgroundColor)
{
    if(this->backgroundColor != backgroundColor){
        this->backgroundColor = backgroundColor;
        update();
    }
}

void ChannelWidget::setMarkerColor(const QColor & markerColor)
{
    if(this->markerColor != markerColor){
        this->markerColor = markerColor;
        update();
    }
}

void ChannelWidget::paintEvent(QPaintEvent * pEvent)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);
    QPen pen(Qt::black,0); // zero width pen is cosmetic pen
    //pen.setCosmetic(true);
    painter.setPen(pen);
    // We want to work with floating point, so we are considering
    // the rect as QRectF
    QRectF rect = this->rect();
    // at first fill the rect
    //wype³niamy t³o
    painter.fillRect(rect,backgroundColor);

    QRectF channelRect = rect;

    qreal top = channelRect.top() + 2;
    qreal bottom = channelRect.bottom() - 2;

    if(top < bottom){
        channelRect.setTop(top);
        channelRect.setBottom(bottom);
    }
    
    //wype³aniamy kana³
    channelRect.setLeft(this->size().width() * begin);
    channelRect.setRight(this->size().width() * end);
    painter.fillRect(channelRect,fillColor);

    //rysujemy marker aktualnego czasu
    QRectF channelCurrentTime = rect;
    //channelCurrentTime.setLeft(std::max((currentTime-currentTimeHalfWidth) * this->size().width(), 0.0));
    //channelCurrentTime.setRight(std::min((currentTime+currentTimeHalfWidth) * this->size().width(), (qreal)this->size().width()));
    channelCurrentTime.setLeft(std::max(currentTime * this->size().width() - 1, 0.0));
    channelCurrentTime.setRight(std::min(currentTime * this->size().width() + 1, (qreal)this->size().width()));

    painter.fillRect(channelCurrentTime,markerColor);

    //ramka widgeta!!
    //QFrame::paintEvent(pEvent);
}