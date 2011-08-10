#include <timelinelib/Controller.h>

namespace timeline {

Controller::Controller() : IController(&dirtyState),
    model(new Model()), constModel(model),
    playbackDirection(PlayForward), timer(new Timer(40000)),
    timeGenerator(&Controller::forwardTimeUpdate), paused(false)
{
    timer->setController(this);
}

Controller::~Controller()
{

}

const ModelConstPtr & Controller::getModel() const
{
    return constModel;
}

void Controller::setTimeUpdateMode(TimeUpdateMode timeUpdateMode)
{
    ScopedLock lock(stateMutex);
    this->timeUpdateMode = timeUpdateMode;
}

Controller::TimeUpdateMode Controller::getTimeUpdateMode() const
{
    ScopedLock lock(stateMutex);
    return timeUpdateMode;
}

void Controller::setPlaybackDirection(PlaybackDirection playbackDirection)
{
    ScopedLock lock(stateMutex);
    this->playbackDirection = playbackDirection;
    timeGenerator = (playbackDirection == PlayForward) ? &forwardTimeUpdate : &backwardTimeUpdate;
}

Controller::PlaybackDirection Controller::getPlaybackDirection() const
{
    ScopedLock lock(stateMutex);
    return playbackDirection;
}

void Controller::play()
{
    ScopedLock lock(stateMutex);

    State state = getState();

    if(state.isPlaying == true){
        return;
    }

    state.isPlaying = true;

    if(paused == false){
        if(timer->isRunning() == true){
            timer->cancel();
            timer->join();
        }

        timer->start();

    }else{
        paused = false;
        pauseMutex.unlock();
    }

    setState(state);
}

void Controller::pause()
{
    ScopedLock lock(stateMutex);

    State state = getState();

    if(state.isPlaying == false){
        return;
    }

    pauseMutex.lock();
    paused = true;
    state.isPlaying = false;
    setState(state);
}

bool Controller::isPlaying() const
{
    return getState().isPlaying;
}

double Controller::getLength() const
{
    return getState().length;
}

double Controller::getTime() const
{
    return getState().time;
}

double Controller::getNormalizedTime() const
{
    return getState().normalizedTime;
}

double Controller::getTimeScale() const
{
    return getState().timeScaleFactor;
}

void Controller::setTimeScale(double timeScale)
{
    ScopedLock lock(stateMutex);

    // nowy stan
    State state = getState();
    state.timeScaleFactor = timeScale;
    setState(state);
}

void Controller::setTime(double time)
{
    ScopedLock lock(stateMutex);

    // nowy stan
    State state = getState();
    state.time = time;
    state.normalizedTime = time  / getLength();
    setState(state);
    timeDirty = true;
}

void Controller::setNormalizedTime(double normTime)
{
    setTime( normTime * getLength() );
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
        ScopedLock lock(modelMutex);
        busy = true;
        model->setState(state);
        dirty = false;
        busy = false;
        timeDirty = false;
    } else {
        dirty = true;
    }
    dirtyState = state;
    notify();
}

bool Controller::isWriteEnabled() const
{
    bool ret = false;

    if ( OpenThreads::Thread::CurrentThread() == timer.get() || model->getState().isPlaying == false || timeUpdateMode == AsynchTimeUpdate ) {
        ret = true;
    }

    return ret;
}

bool Controller::isBusy() const
{
    ScopedLock lock(stateMutex);
    return busy;
}

bool Controller::isDirty() const
{
    ScopedLock lock(stateMutex);
    return dirty;
}

bool Controller::isTimeDirty() const
{
    ScopedLock lock(stateMutex);
    return timeDirty;
}

void Controller::addChannel(const std::string & path, const IChannelPtr & channel)
{
    ScopedLock lock(stateMutex);
    ScopedLock lock2(modelMutex);
    model->addChannel(path, channel);
    double newLength = model->getLength();
    if(dirtyState.length != newLength){
        dirtyState.length = newLength;
        setState(dirtyState);
    }
}

void Controller::addChannels(const std::map<std::string, IChannelPtr> & channels)
{
    ScopedLock lock(stateMutex);
    ScopedLock lock2(modelMutex);
    model->addChannels(channels);
    double newLength = model->getLength();
    if(dirtyState.length != newLength){
        dirtyState.length = newLength;
        setState(dirtyState);
    }
}


void Controller::removeChannel(const std::string & path)
{
    ScopedLock lock(stateMutex);
    ScopedLock lock2(modelMutex);
    model->removeChannel(path);
    double newLength = model->getLength();
    if(dirtyState.length != newLength){
        dirtyState.length = newLength;
        setState(dirtyState);
    }
}

void Controller::removeChannels(const std::set<std::string> & paths)
{
    ScopedLock lock(stateMutex);
    ScopedLock lock2(modelMutex);
    model->removeChannels(paths);
    double newLength = model->getLength();
    if(dirtyState.length != newLength){
        dirtyState.length = newLength;
        setState(dirtyState);
    }
}


double Controller::forwardTimeUpdate(Controller * controller, unsigned long int realTimeDeltaMS)
{
    return std::min(controller->getLength(), controller->getTime() + (double)realTimeDeltaMS / 1000000.0);
}

double Controller::backwardTimeUpdate(Controller * controller, unsigned long int realTimeDeltaMS)
{
    return std::max(0.0, controller->getTime() - (double)realTimeDeltaMS / 1000000.0);
}

bool Controller::setNextTime(unsigned long int realTimeDeltaMS)
{    
    ScopedLock lock(stateMutex);

    bool ret = false;

    State state = getState();

    state.time = timeGenerator(this, realTimeDeltaMS);
    if(state.length > 0){
        state.normalizedTime = state.time / state.length;
    }

    if(getPlaybackDirection() == PlayForward){
        if(getTime() == model->getEndTime()){
            ret = true;
        }
    }else if(getTime() == model->getBeginTime()){
        ret = true;
    }

    if(ret == true){
        state.isPlaying = false;
    }

    setState(state);

    return ret;
}

void Controller::setChannelActive(const std::string & path, bool active)
{
    ScopedLock lock(stateMutex);
    model->setChannelActive(path, active);
}

}