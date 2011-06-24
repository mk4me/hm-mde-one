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
    utils::Observable<State>(&dirtyState),
    modelMutex(), 
    stateMutex(), 
    asynchronous(false),
    timeDirty(false),
    busy(false),
    dirty(false),
    paused(true)
{
  model = new Model();
  dirtyState = model->getState();
  // pauzuj watek kontrolera
  pauseMutex.lock();

  // uruchom watek kontrolera
  start();
}

Controller::~Controller()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    killThread = true;
    if(isPlaying() == true){
        model->setPlaying(false);
        pauseMutex.unlock();        
        join();
    }
    delete model;
}

void Controller::pause()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);

    State state = getState();
    
    if(state.isPlaying == false){
        return;
    }
    paused = true;
    pauseMutex.lock();    
    state.isPlaying = false;
    setState(state);
}

void Controller::play()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);

    State state = getState();

    if(state.isPlaying == true){
        return;
    }

    paused = false;
    state.isPlaying = true;
    setState(state);

    pauseMutex.unlock();
}

void Controller::addStream( StreamPtr stream )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock2(modelMutex);
    model->addStream(stream);
    dirtyState.length = model->getLength();
}

void Controller::removeStream( StreamPtr stream )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock2(modelMutex);
    model->removeStream(stream);
    State state = getState();
    state.length = model->getLength();
    setState(state);
}

double Controller::getNormalizedTime() const
{
    return getTime() / getLength();
}

void Controller::setNormalizedTime( double time )
{
    setTime( time * getLength() );
}

double Controller::getLength() const
{
    return getState().length;
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
    State state = getState();
    state.time = time;
    state.normalizedTime = time  / getLength();
    setState(state);
    timeDirty = true;
}

double Controller::getTimeScale() const
{
    return getState().timeScale;
}

void Controller::setTimeScale( double timeScale )
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    State state = getState();
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
            {
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(pauseMutex);
                if(killThread == true){
                    return;
                }
            }
            // zerujemy czas ramki
            frameLength.setStartTick();

            compute();

            // jak d³ugo to wszystko trwa³o?
            double waitTime = 0.02 - frameLength.time_s();
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



State Controller::getState() const
{
    if ( isWriteEnabled() ) {
        return model->getState();
    } else {
        return dirtyState;
    }
}

void Controller::setState( const State& state )
{
    if ( isWriteEnabled() ) {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(modelMutex);
        busy = true;
        model->setState(state);
        dirty = false;
        busy = false;
    } else {
        dirty = true;
    }
    dirtyState = state;
    notify();
}

bool Controller::isWriteEnabled() const
{
    if ( OpenThreads::Thread::CurrentThread() == this || paused == true ) {
        return true;
    } else {
        return !asynchronous;
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

bool Controller::isTimeDirty() const
{
    return timeDirty;
}

bool Controller::isAsynchronous() const
{
    return asynchronous;
}

void Controller::setAsynchronous( bool asynchronous )
{
    this->asynchronous = asynchronous;
}

bool Controller::compute()
{
    // ustalenie i modyfikacja bie¿¹cego stanu
    bool resetTimer = false;
    State appliedState;
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
        // czy jest sens dalej trzymaæ w¹tek?
        if ( !isDirty() && !isPlaying() ) {
            pauseMutex.lock();
            return true;
        }
        // zaakceptowano stan
        appliedState = dirtyState;
        timeDirty = dirty = false;
        busy = true;
    }

    // nadanie bie¿¹cego stanu
    {
        // blokada modyfikacji czasu
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(modelMutex);
        // nadanie stanu modelowi - miejsce wykonania w³aœciwej logiki
        model->setState(appliedState);
        // ile trzeba bêdzie czekaæ?
        busy = false;
    }
    return true;
}



////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////