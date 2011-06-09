#include "TimelinePCH.h"
#include "TimelineWidget.h"
#include <plugins/timeline/State.h>
// TODO
// - slider QSlider nie wspiera przeskakiwania do miejsca klikniêcia,
//   zamiast tego przeskakuje o "page value"

TimelineWidget::TimelineWidget(TimelineService* service, timeline::Controller * controller)
:   service(service), controller(controller)
{
    setupUi(this);
    controller->attach(this);
}

TimelineWidget::~TimelineWidget()
{
    dynamic_cast<utils::Observable<timeline::State>*>(controller)->detach(this);
}

void TimelineWidget::timeSliderChanged(int value)
{
    service->getController()->setNormalizedTime(getSliderValue(timeSlider));
    service->setSeekRequested(true);
}

void TimelineWidget::timeSliderPressed()
{
    //service->setSeekRequested(true);
}

void TimelineWidget::timeSliderReleased()
{
    //service->setSeekRequested(false);
}

void TimelineWidget::update( const timeline::State * subject )
{
    // ustawienie wartoœci slidera
    int sliderValue = getSliderValue(timeSlider, subject->normalizedTime);
    if ( sliderValue != timeSlider->value() && !timeSlider->isSliderDown() ) {
        timeSlider->blockSignals(true);
        timeSlider->setValue(sliderValue);
        timeSlider->blockSignals(false);
    }

    currentTimeSpinBox->blockSignals(true);
    currentTimeSpinBox->setMinimum(0.0);
    currentTimeSpinBox->setMaximum( subject->length );
    currentTimeSpinBox->setValue( subject->time );
    currentTimeSpinBox->blockSignals(false);

//     blockSignals(true);
//     if (subject->getPlaying()) {
//         playButton->setChecked(true);
//         pauseButton->setChecked(false);
//     } else {
//         playButton->setChecked(false);
//         pauseButton->setChecked( normalizedUITime == 0.0 );
//     }
//     blockSignals(false);
}

double TimelineWidget::getSliderValue(QSlider* slider) const
{
    int min = slider->minimum();
    int max = slider->maximum();
    int value = slider->value();
    return static_cast<double>( value - min ) / ( max - min );
}

int TimelineWidget::getSliderValue( QSlider* slider, double normalizedValue )
{
    int min = slider->minimum();
    int max = slider->maximum();
    int intValue = static_cast<int>(normalizedValue * (max - min) + min + 0.5);
    return intValue;
}

void TimelineWidget::setSliderValue( double value, QSlider* slider )
{
    int min = slider->minimum();
    int max = slider->maximum();
    int intValue = static_cast<int>(value * (max - min) + min + 0.5);
    slider->setValue(intValue);
}

void TimelineWidget::playButtonPressed()
{
    service->setPlaying(true);
}

void TimelineWidget::pauseButtonPressed()
{
    service->setPlaying(false);
}

void TimelineWidget::stopButtonPressed()
{
    service->setPlaying(false);
    service->setTime(0);
}

void TimelineWidget::setBusy( bool busy )
{
    if ( busy ) {
        statusLabel->setText("Working...");
    } else {
        statusLabel->setText("");
    }
}
