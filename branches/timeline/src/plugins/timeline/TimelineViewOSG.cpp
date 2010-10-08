#include "VideoPCH.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/ImageStream>
#include <osg/Timer>
#include <osg/Notify>
#include <OpenThreads/Thread>
#include <osgWidget/Box>
#include <osgWidget/Widget>
#include <osgWidget/Window>
#include <osgWidget/WindowManager>
#include <osgWidget/Label>

#include <utils/PtrPolicyOSG.h>
#include <utils/ObserverPattern.h>
#include "Callback.h"

#include "Model.h"
#include "Controller.h"
#include "View.h"


#include "Button.h"
#include "HorizontalScrollBar.h"
#include "WidgetFunctors.h"
#include "Potentiometer.h"
#include "TimelineViewOSG.h"


using namespace osgUI;

//------------------------------------------------------------------------------

const float TimelineViewOSG::BUTTON_WIDTH = 64.0f;
const float TimelineViewOSG::BUTTON_HEIGHT = 64.0f;
const std::string TimelineViewOSG::DATA_ROOT = "data/resources/player/";

//------------------------------------------------------------------------------


TimelineViewOSG::TimelineViewOSG( osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags )
: WindowManager( view, width, height, mask, flags ), supressCallbacks(false)
{
  // g³ówny box
  osgWidget::Box * mainBox = new osgWidget::Box("mainBox", osgWidget::Box::HORIZONTAL);
  mainBox->getBackground()->setColor(0.0,0.0,0.0,0.0);
  mainBox->addUpdateCallback( new PercentWindowResizer(100.0f) );
  mainBox->setStrata(osgWidget::Window::STRATA_FOREGROUND);

  // guziki nawigacyjne
  osgWidget::Widget* play = createButton("play");
  osgWidget::Widget* pause = createButton("pause");
  osgWidget::Widget* stop = createButton("stop");
  mainBox->addWidget(createMargin());
  mainBox->addWidget(play);
  mainBox->addWidget(pause);
  mainBox->addWidget(stop);

  // potencjometr
  potentiometer = new Potentiometer("potentiometer", 64, 64);
  potentiometer->setImage(DATA_ROOT + "potentiometer.png", true);
  potentiometer->setTextureTileWidth(0.5f);
  potentiometer->setSensetivity(50);
  potentiometer->setMomentOfInteria(1);
  potentiometer->setAirFriction(25);
  potentiometer->setClickFriction(150);
  potentiometer->setMinValue( -FLT_MAX );
  potentiometer->setMaxValue(  FLT_MAX );
  potentiometer->setMomentOfInteria(1.0f);
  potentiometer->addCustomCallback(new Callback(Potentiometer::EventIDValueChanged, &TimelineViewOSG::onPotentiometerChanged, this));
  mainBox->addWidget(potentiometer);

  // pasek nawigacyjny
  navigationBar = createSlider("navigation");
  mainBox->addWidget(embed(navigationBar));

  // skala czasowa
  timescaleBar = createSlider("timescale");
  timescaleBar->setTrackBackground(osgWidget::Color(1.0, 0.0, 0.0, 1.0));
  osgWidget::Widget * widget = embed(timescaleBar);
  widget->setCanFill(false);
  widget->setWidth(200);
  widget->setHeight(64);
  mainBox->addWidget(widget);

  labelTime = createLabelBox();
  labelTime->setVisibilityMode(osgWidget::Window::VM_ENTIRE);
  labelTime->setStrata(osgWidget::Window::STRATA_FOREGROUND);
  labelTimeScale = createLabelBox();
  labelTimeScale->setVisibilityMode(osgWidget::Window::VM_ENTIRE);
  labelTime->setStrata(osgWidget::Window::STRATA_FOREGROUND);

  // tymczasowy box
  /*osgWidget::Box * tempBox = new osgWidget::Box("tempBox", osgWidget::Box::HORIZONTAL);
  tempBox->getBackground()->setColor(0.0,0.0,0.0,0.0);
  tempBox->addUpdateCallback( new PercentWindowResizer(100.0f) );
  tempBox->setStrata(osgWidget::Window::STRATA_FOREGROUND);
  tempBox->setOrigin(196+16, 64);*/

  addChild(mainBox);
  addChild(labelTime);
  addChild(labelTimeScale);
  //addChild(tempBox);

//   osgWidget::Box * test = new osgWidget::Box("TEST");
//   osgWidget::Widget* space = new osgWidget::Widget("SPACE", 250, 250);
//
//   test->addWidget(space);
//   addChild(test);

  navigationBar->setTooltipWindow(labelTime);
  timescaleBar->setTooltipWindow(labelTimeScale);
}

// osgWidget::Box* TimelineViewOSG::createNavigationBox()
// {
//   osgWidget::Box* result = new osgWidget::Box("streamBox", osgWidget::Box::VERTICAL);
//   result->getBackground()->setColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//   // tworzymy kursor nawigacyjyn
//   navigationCursor = new osgWidget::Widget("navigationCursor", 10.0f, 10.0f);
//   // przesuwamy
//   WidgetColorer::attach( navigationCursor.get(), new WidgetDragger(
//     osgWidget::Color(1.0f, 0.0f, 0.0f, 0.3f),
//     osgWidget::Color(1.0f, 0.0f, 0.0f, 0.7f),
//     osgWidget::Color(1.0f, 0.0f, 0.0f, 1.0f))
//   );
//   // dodatkowy callback
//   navigationCursor->addCallback( new osgWidget::Callback(&TimelineViewOSG::onNavigationBarDragged, this, osgWidget::EVENT_MOUSE_DRAG) );
//
//   // rêczne sterowanie eventami
//   result->setEventMask(osgWidget::EVENT_MASK_MOUSE_CLICK);
//   result->addCallback( new osgWidget::Callback(&TimelineViewOSG::onNavigationClicked, this, osgWidget::EVENT_MOUSE_PUSH));
//   result->addWidget(navigationCursor.get());
//
//   return result;
// }

osgWidget::Widget* TimelineViewOSG::createStreamBar()
{
  osgWidget::Box* box = new osgWidget::Box("streamBar", osgWidget::Box::VERTICAL);
  box->getBackground()->setColor(0.0f, 0.0f, 0.0f, 0.0f);

  osgWidget::Widget* space = new osgWidget::Widget("space1", 0.0f, 1.0f);
  space->setColor(0.0f, 0.0f, 0.0f, 0.0f);

  Button* bar = new Button("bar", osgWidget::Color(0.0, 0.75, 0.0, 0.3), 10.0f, 10.0f);
  bar->setHooverColor(osgWidget::Color(0.0f, 1.0f, 0.0f, 0.5f));
  bar->setClickedColor(osgWidget::Color(0.0f, 1.0f, 0.0f, 0.7f));
  //bar->setCanFill(true);

  box->addWidget(space);
  box->addWidget(bar);
  box->addWidget(osg::clone(space, "space2", osg::CopyOp::DEEP_COPY_ALL));

  osgWidget::Widget* result = box->embed();
  result->setColor(0.0f, 0.0f, 0.0f, 0.0f);

//   // callbacki
//   WidgetDragger* functor = new WidgetDragger(
//     osgWidget::Color(0.0f, 0.75f, 0.0f, 0.3f),
//     osgWidget::Color(0.0f, 1.0f, 0.0f, 0.5f),
//     osgWidget::Color(0.0f, 1.0f, 0.0f, 0.7f)
//     );
//   functor->attachTo(bar);
//   // dodatkowy callback
//   bar->addCallback( new osgWidget::Callback(&TimelineViewOSG::onStreamBarDragged, this, osgWidget::EVENT_MOUSE_DRAG) );
//   //
//   bar->addCallback(new osgWidget::Callback(&TimelineViewOSG::onStreamClicked, this, osgWidget::EVENT_MOUSE_PUSH));

  return result;
}

osgWidget::Box* TimelineViewOSG::createLabelBox()
{
  osgWidget::Box* result = new osgWidget::Box("labelBox");
  osgWidget::Label* label = new osgWidget::Label("label");

  //label->setFont("fonts/arial.ttf");
  label->setFontSize(50);
  label->setFontColor(1.0f, 1.0f, 1.0f, 0.7f);
  //label->setAlignHorizontal(osgWidget::Widget::HA_LEFT);
  //label->setAlignVertical(osgWidget::Widget::VA_BOTTOM);
  //label->setLabel("00:00/00:00");
  label->setUpdateCallback(new LabelFunctor());

  result->addWidget(label);
  result->getBackground()->setColor(0.0f, 0.0f, 0.0f, 0.5f);
  return result;
}

osgWidget::Widget* TimelineViewOSG::createButton( const std::string& name )
{
  Button * result = new Button(name, osgWidget::Color(1.0,1.0,1.0,1.0), BUTTON_WIDTH, BUTTON_HEIGHT);
  result->setHooverColor(osgWidget::Color(0.0,0.0,1.0,1.0));
  result->setClickedColor(osgWidget::Color(0.7,0.7,0.7,0.7));
  result->setImage(DATA_ROOT + name + ".png", true);
  result->addCallback(new osgWidget::Callback(&TimelineViewOSG::onButtonClicked, this, osgWidget::EVENT_MOUSE_PUSH));
  result->setLayer(osgWidget::Widget::LAYER_HIGH, 10);
  return result;
}

void TimelineViewOSG::update( const Model * subject )
{
  supressCallbacks = true;
  //Model::size_type streamCount = subject->getStreamCount();
  //// czy musimy dodaæ strumienie?
  //int i =0;
  //if ( streamCount > streams.size() ) {
  //  // dodajemy brakuj¹ce s³upki
  //  for ( Model::const_iterator it = subject->begin() + streams.size(); it != subject->end(); ++it ) {
  //    osg::ref_ptr<osgWidget::Widget> bar = createStreamBar();
  //    bar->setUpdateCallback( new BarFunctor() );
  //    streamBox->addWidget(bar);
  //    streams.push_back(bar);
  //  }
  //  resizeAllWindows();
  //} else if ( streamCount < streams.size() ) {
  //  // usuwamy zbêdne s³upki
  //  for ( size_t i = streamCount; i < streams.size(); ++i ) {
  //    streamBox->removeWidget( streams[i].get() );
  //  }
  //  streams.resize(streamCount);
  //  resizeAllWindows();
  //}

  //// aktualizacja strumieni
  //for ( Model::size_type i = 0; i < subject->getStreamCount(); ++i ) {
  //  // pobieramy kontrolkê strumienia
  //  osg::ref_ptr<osgWidget::Widget> bar = streams[i];
  //  // nadanie nazwy
  //  std::stringstream converter;
  //  converter<<"stream"<<i;
  //  bar->setName(converter.str());
  //  // aktualizacja danych strumienia
  //  BarFunctor * functor = dynamic_cast<BarFunctor*>(bar->getUpdateCallback());
  //  functor->totalDuration = subject->getLength();
  //  functor->startOffset = subject->getStream(i)->getStartOffset();
  //  functor->duration = subject->getStream(i)->getLength();
  //  functor->streamIdx = i;
  //}

  double normTime = subject->getNormalizedUITime();
  if ( navigationBar ) {
    if ( !navigationBar->getCursor()->isPushed() ) {
      navigationBar->setValue( normTime );
    } else {
      normTime = navigationBar->getValue();
      if (potentiometer) {
        potentiometer->zeroPhysics();
      }
    }
  }
  if ( potentiometer ) {
    //potentiometer->setMaxValue( subject->getLength() * 2 * osg::PI );
    /*if ( !potentiometer->isPushed() ) {
      potentiometer->setNormalizedValue(normTime);
    } else {
      normTime = potentiometer->getNormalizedValue();
    }*/
  }

  // aktualizacja labelki z czasem
  LabelFunctor * label = dynamic_cast<LabelFunctor*>(labelTime->getByName("label")->getUpdateCallback());
  label->text = std::string("Time: ") + formatTime(subject->getLength() * normTime) + "/" + formatTime(subject->getLength());

  label = dynamic_cast<LabelFunctor*>(labelTimeScale->getByName("label")->getUpdateCallback());
  label->text = std::string("Scale: ") + formatTimeScale(subject->getTimeScale());



  if ( timescaleBar ) {
    timescaleBar->setValue( subject->getTimeScale() / 20.0 + 0.5 );
  }

  // aktualizacja guzika
  //LabelFunctor * functor = dynamic_cast<LabelFunctor*>(mainBox->getByName("play")->getUpdateCallback());
  //functor->text = ( subject->getPlaying() == osg::ImageStream::PLAYING ) ? "PAUSE" : "PLAY";

  // aktualizacja pozycji draga
  //if ( navigationCursor ) {
  //  // dziwnym trafem wskaŸnik potrafi siê wyzerowaæ przy zamykaniu
  //  WidgetDragger * dragger = dynamic_cast<WidgetDragger*>(navigationCursor->getUpdateCallback());
  //  dragger->x = streamBox->getWidth() * subject->getNormalizedTime();
  //}
  supressCallbacks = false;
}

void TimelineViewOSG::setController( osg::ref_ptr<timeline::Controller> controller )
{
  this->controller = controller;
  const Model * model = controller->getModel();
  update(model);
}

std::string TimelineViewOSG::formatTime( double seconds )
{
  int integral = static_cast<int>(seconds);
  double decimal = seconds - integral;

  int decyPart = static_cast<int>(decimal * 100);
  int secondsPart = integral % 60;
  int minutesPart = ( integral / 60 ) % 60;
  int hoursPart = integral / 3600;

  std::stringstream converter;

  converter.fill('0');
  converter << hoursPart << ":" << std::setw(2) << minutesPart << ":" << std::setw(2) << secondsPart << "." << std::setw(2) << decyPart;

  //return "aaa";
  return converter.str();
}


std::string TimelineViewOSG::formatTimeScale( double scale )
{
  std::stringstream converter;
  converter << std::fixed << std::setprecision(2) << scale;

  //return "aaa";
  return converter.str();
}



bool TimelineViewOSG::onButtonClicked( osgWidget::Event& event )
{
  const std::string & name = event.getWidget()->getName();
  if ( name == "play" ) {
    controller->play();
  } else if ( name == "pause" ) {
    controller->pause();
  } else if ( name == "stop" ) {
    controller->pause();
    controller->setTime(0.0);
  }
  return true;
}

osgWidget::Widget* TimelineViewOSG::createMargin()
{
  return new osgWidget::Widget("space", 16, 0);
}

bool TimelineViewOSG::onSliderValueChanged(osgUI::Event& event)
{
  if (supressCallbacks) {
    return false;
  }

  HorizontalScrollBar * sender = event.getSender<HorizontalScrollBar>();
  double value = event.getValue<double>();
  if ( sender == navigationBar ) {
    //controller->getModel()->setNormalizedUITime( value );
    controller->setNormalizedUITime(value);
  } else if ( sender == timescaleBar ) {
    // przeskalowanie 0:1 na -10:10
    controller->setTimeScale( (value - 0.5) * 20 );
  }
  return true;
}

bool TimelineViewOSG::onSliderUpdateTooltip(osgUI::Event& event)
{
  if (supressCallbacks) {
    return false;
  }
  HorizontalScrollBar * sender = event.getSender<HorizontalScrollBar>();
  if ( sender == navigationBar ) {

  } else if ( sender == timescaleBar ) {

  }
  return true;
}


bool TimelineViewOSG::onPotentiometerChanged( osgUI::Event& event )
{
  if (supressCallbacks) {
    return false;
  }
  Potentiometer * p = event.getSender<Potentiometer>();
  //std::cout << p->getDelta() << std::endl;
  float value = p->getDelta() / (2 * osg::PI); /*p->getMaxValue()*/;
  value = controller->getModel()->getUITime() + value;
  value = utils::clamp<float>(value, 0.0f, controller->getLength());
  controller->setUITime(value);
  return true;
}




HorizontalScrollBar * TimelineViewOSG::createSlider( const std::string& name )
{
  HorizontalScrollBar * result = new HorizontalScrollBar(name);
  result->getCursor()->setNormalColor(osgWidget::Color(1.0, 1.0, 1.0, 1.0));
  result->getCursor()->setHooverColor(osgWidget::Color(0.0f, 0.0f, 1.0f, 0.75));
  result->getCursor()->setClickedColor(osgWidget::Color(0.75, 0.75, 0.75, 0.75));
  result->getCursor()->setSize(64, 64);
  result->getCursor()->setImage(DATA_ROOT + "slider.png", true);
  result->getBorderLeft()->setSize(32, 64);
  result->getBorderLeft()->setImage(DATA_ROOT + "slider_left_edge.png", true);
  result->getBorderRight()->setSize(32, 64);
  result->getBorderRight()->setImage(DATA_ROOT + "slider_right_edge.png", true);
  result->getTrack()->setImage(DATA_ROOT + "slider_bcg.png", true);
  result->getTrack()->setTexCoordWrapHorizontal();
  result->getBackground()->setColor(0.0, 0.0, 0.0, 0.0);
  result->setTrackExtension( 18.0 );
  result->addCustomCallback( new Callback(HorizontalScrollBar::EventIDValueChanged, &TimelineViewOSG::onSliderValueChanged, this) );
  result->addCustomCallback( new Callback(HorizontalScrollBar::EventIDUpdateTooltip, &TimelineViewOSG::onSliderUpdateTooltip, this) );
  return result;
}

osgWidget::Widget * TimelineViewOSG::embed( osgWidget::Window * window, const std::string& name /*= ""*/ )
{
  osgWidget::Widget * result = window->embed(name);
  result->setColor(0.0, 0.0, 0.0, 0.0);
  return result;
}

