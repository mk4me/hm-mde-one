#include "TimelinePCH.h"
#include "TimelineWidget.h"

// TODO
// - slider QSlider nie wspiera przeskakiwania do miejsca klikniêcia,
//   zamiast tego przeskakuje o "page value"

TimelineWidget::TimelineWidget(TimelineService* service)
:   service(service)
{
    setupUi(this);
}

TimelineWidget::~TimelineWidget()
{

}

void TimelineWidget::timeSliderChanged(int value)
{
    service->getController()->setNormalizedTime(getSliderValue(timeSlider));
    service->setSeekRequested(true);
}

void TimelineWidget::update( const timeline::Model::State * subject )
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
    service->getController()->play();
}

void TimelineWidget::pauseButtonPressed()
{
    service->getController()->pause();
}

void TimelineWidget::stopButtonPressed()
{
    service->getController()->pause();
    service->getController()->setTime(0);
}