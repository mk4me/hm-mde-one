#include "TimelinePCH.h"
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

#include <plugins/timeline/Model.h>
#include <plugins/timeline/Controller.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

Controller::Controller() : 
    utils::Observable<Model::State>(&dirtyState),
    modelMutex(), 
    stateMutex(), 
    asynchronousMode(true)
{
  model = new Model();
  dirtyState = model->getState();
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
//   OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
//   model->setPlaying(false);
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    Model::State state = getState();
    state.isPlaying = false;
    setState(state);
}

void Controller::play()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);

    Model::State state = getState();
    state.isPlaying = true;
    setState(state);

    timeRequested = true;
    if ( !isRunning() ) {
        start();
    }
}

// void Controller::setUITime( double time )
// {
//   // ustawiamy czas
//   model->setUITime(time);
//   // jeœli w¹tek nie dzia³a, uruchamiamy go
//   if ( !isRunning() ) {
//     // dla bezpieczeñstwa sprawdzamy jeszcze raz...
//     OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
//     if (time != model->getTime() && !isRunning()) {
//       // ok, uruchamiamy w¹tek który bêdzie odœwie¿a³ obraz
//       start();
//     }
//   }
// }

double Controller::getLength() const
{
    return model->getLength();
    //return getState().length;
}


bool Controller::isPlaying() const
{
    return getState().isPlaying;
}

double Controller::getTime() const
{
    return getState().time;
}

void Controller::setTime( double time )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    
    // nowy stan
    Model::State state = getState();
    state.time = time;
    state.normalizedTime = time  / getLength();
    setState(state);

    // wyzerowanie licznika
    timeRequested = true;
    if ( !isRunning() && asynchronousMode ) {
        start();
    }
}

double Controller::getTimeScale() const
{
    return getState().timeScale;
}

void Controller::setTimeScale( double timeScale )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    Model::State state = getState();
    state.timeScale = timeScale;
    setState(state);
}

void Controller::setStreamOffset( int idx, double offset )
{
    throw std::runtime_error("Not implemented");
}

void Controller::run()
{
    //UTILS_PROFILER_THREAD_SCOPE;
    try
    {
        osg::Timer frameLength;
        while (true) {
            // zerujemy czas ramki
            frameLength.setStartTick();

            // ustalenie i modyfikacja bie¿¹cego stanu
            bool resetTimer = false;
            Model::State appliedState;
            {
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
                // czy jest sens dalej trzymaæ w¹tek?
                if ( !isDirty() && !isPlaying() ) {
                    OSG_NOTICE<<"Finished Controller::run()"<<std::endl;
                    return;
                }

                // obliczenie delty
                osg::Timer_t tick = timer.tick();
                double delta = timer.delta_s(timer.getStartTick(), tick);
                timer.setStartTick(tick);

                // je¿eli podano inny czas to znaczy, ¿e nie mo¿na siê automatycznie przesun¹æ dalej
                double time = dirtyState.time;
                if ( timeRequested ) {
                    // czas podany rêcznie, nie modyfikujemy go
                    resetTimer = true;
                    timeRequested = false;
                } else if ( dirtyState.isPlaying ) {
                    // automatyczna inkrementacja
                    //time += delta * dirtyState.timeScale;
                    //time = utils::clamp(time, 0.0, model->getLength());
                }
                dirtyState.time = time;

                // zaakceptowano stan
                appliedState = dirtyState;
                dirty = false;
                //notify();
            }

            // nadanie bie¿¹cego stanu
            double waitTime;
            {
                // blokada modyfikacji czasu
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(modelMutex);
                busy = true;
                // nadanie stanu modelowi - miejsce wykonania w³aœciwej logiki
                model->setState(appliedState);
                // ile trzeba bêdzie czekaæ?
                waitTime = appliedState.refreshPeriod - frameLength.time_s();
                busy = false;
            }

            // je¿eli czas podano rêcznie - wyzerowanie timera
            if ( resetTimer ) {
                timer.setStartTick();
            }
            // jak d³ugo to wszystko trwa³o?
            if ( waitTime > 0.0 ) {
                OpenThreads::Thread::microSleep( waitTime * 1000000 );
            }
        } 
    } catch (const std::exception & error) {
        OSG_WARN<< "Controller::run : " << error.what() << std::endl;
    } catch (...) {
        OSG_WARN<< "Controller::run : unhandled exception" << std::endl;
    }
}



Model::State Controller::getState() const
{
    if ( isWriteEnabled() ) {
        return model->getState();
    } else {
        return dirtyState;
    }
}

void Controller::setState( const Model::State& state )
{
    if ( isWriteEnabled() ) {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(modelMutex);
        busy = true;
        model->setState(state);
        busy = false;
    } else {
        dirty = true;
    }
    dirtyState = state;
    // HACK
    dirtyState.length = model->getLength();
    notify();
}

bool Controller::isWriteEnabled() const
{
    if ( OpenThreads::Thread::CurrentThread() == this ) {
        return true;
    } else if ( const_cast<Controller*>(this)->isRunning() ) {
        return false;
    } else {
        return !asynchronousMode;
    }
}

bool Controller::isBusy() const
{
    return busy;
}

bool Controller::isDirty() const
{
    return dirty;
}

bool Controller::isTimeRequested() const
{
    return timeRequested;
}
////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////
