#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/ImageStream>
#include <osg/Timer>
#include <osg/Notify>
#include <OpenThreads/Thread>

#include <utils/Utils.h>
#include <utils/ObserverPattern.h>
#include <utils/Profiler.h>

#include "Model.h"
#include "Controller.h"

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

Controller::Controller() : timeMutex(), stateMutex()
{
  model = new Model();
}

Controller::~Controller()
{
  // quit
  if ( isRunning() ) {
    // zmieniamy stan...
    model->setPlaying(false);
    join();
  }
  delete model;
}

void Controller::pause()
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
  model->setPlaying(false);
}

void Controller::play()
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
  // czy zrestartowaæ zegar?
  bool restartClock = !model->getPlaying() || !isRunning();
  // ustawiamy flagê
  model->setPlaying(true);
  // resetujemy zegar
  if ( restartClock ) {
    timer.setStartTick();
  }
  // uruchamiamy w¹tek
  if ( !isRunning() ) {
    start();
  }
}

void Controller::setTime( double time )
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock(timeMutex);
  // ustawiamy czas
  model->setTime(time);
  if ( isRunning() ) {
    // synchronizujemy zegar
    timer.setStartTick();
  }
}

void Controller::setUITime( double time )
{
  // ustawiamy czas
  model->setUITime(time);
  // jeœli w¹tek nie dzia³a, uruchamiamy go
  if ( !isRunning() ) {
    // dla bezpieczeñstwa sprawdzamy jeszcze raz...
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    if (time != model->getTime() && !isRunning()) {
      // ok, uruchamiamy w¹tek który bêdzie odœwie¿a³ obraz
      start();
    }
  }
}

double Controller::getLength() const
{
  return model->getLength();
}

double Controller::getTime() const
{
  return model->getTime();
}

void Controller::setStreamOffset( int idx, double offset )
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
  model->getStream(idx)->setStartOffset(offset);
}

void Controller::run()
{
  UTILS_PROFILER_THREAD_SCOPE;
  try
  {
    osg::Timer frameLength;
    bool done = false;
    do {
      // zerujemy czas ramki
      frameLength.setStartTick();
      {
        //cout << "entering mutex\n";
        // odtwarzamy
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(timeMutex);
        if ( model->getPlaying() )  {
          done = autoPlayback();
        } else {
          done = asyncPlayback();
        }
        //cout << "leaving mutex\n";
      }
      if (!done) {
        // jak d³ugo to trwa³o?
        double waitTime = model->getRefreshPeriod() - frameLength.time_s();
        if ( waitTime > 0.0 ) {
          // czekamy
          OpenThreads::Thread::microSleep( waitTime * 1000000 );
        }
      } else {
        // koniec pêtli
        break;
      }
    } while (true);
  }
  catch (const std::exception & error)
  {
    OSG_WARN<< "VideoImageStream::run : " << error.what() << std::endl;
  }
  catch (...)
  {
    OSG_WARN<< "VideoImageStream::run : unhandled exception" << std::endl;
  }
  OSG_NOTICE<<"Finished VideoImageStream::run()"<<std::endl;
}

bool Controller::asyncPlayback()
{
  if (model->getUITime() != model->getTime()) {
    model->setTime(model->getUITime());
    return false;
  } else {
    // skoñczyliœmy
    return true;
  }
}

bool Controller::autoPlayback()
{
  bool usingUITime = false;
  // bie¿¹ce tykniêcie
  osg::Timer_t tick = timer.tick();
  double delta = timer.delta_s(timer.getStartTick(), tick);
  timer.setStartTick(tick);
  // czy pobieramy normalny czas czy te¿ czas UI (zmodyfikowany przez UI)?
  double time = model->getTime() + delta * getTimeScale();
  if ( model->getUITime() != model->getTime() ) {
    //std::cout << model->getUITime() << "\n";
    time = model->getUITime();
    usingUITime = true;
  }
  // ograniczamy czas
  time = utils::clamp(time, 0.0, model->getLength());
  model->setTime(time);

  if ( usingUITime ) {
    timer.setStartTick();
  }

  // jeszcze nie koniec
  return false;
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////
