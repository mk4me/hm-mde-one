#include <timelinelib/Controller.h>

namespace timeline {

Controller::Controller(const ViewPtr & view) : IController(&dirtyState),
    model(new Model()), constModel(model), view(view), constView(view),
    playbackDirection(PlayForward), timer(new Timer(40)),
    timeGenerator(&Controller::forwardTimeUpdate)
{
    UTILS_ASSERT((view != nullptr), "B³êdny vidok dla timeline");
    timer->setController(this);
}

Controller::~Controller()
{

}

const ModelConstPtr & Controller::getModel() const
{
    return constModel;
}

void Controller::setView(const ViewPtr & view)
{
    ScopedLock lock(stateMutex);

    if(this->view != nullptr){
        this->view->setController(nullptr);
    }

    this->view = view;
    view->setController(this);
    view->refreshAll();
}

const ViewPtr & Controller::getView()
{
    return view;
}

const ViewConstPtr & Controller::getView() const
{
    return constView;
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
    setState(state);

    pauseMutex.unlock();
}

void Controller::pause()
{
    ScopedLock lock(stateMutex);

    State state = getState();

    if(state.isPlaying == false){
        return;
    }

    pauseMutex.lock();
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
    bool ret = false;

    if ( OpenThreads::Thread::CurrentThread() == timer.get() || model->getState().isPlaying == false || timeUpdateMode == SynchTimeUpdate ) {
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

    view->updateViewChannelsStructure();
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

    view->updateViewChannelsStructure();
}


double Controller::forwardTimeUpdate(Controller * controller, unsigned long int realTimeDeltaMS)
{
    return std::min(controller->getLength(), controller->getTime() + (double)realTimeDeltaMS * controller->getTimeScale() / 1000.0);
}

double Controller::backwardTimeUpdate(Controller * controller, unsigned long int realTimeDeltaMS)
{
    return std::max(0.0, controller->getTime() - (double)realTimeDeltaMS * controller->getTimeScale() / 1000.0);
}

bool Controller::setNextTime(unsigned long int realTimeDeltaMS)
{
    setTime(timeGenerator(this, realTimeDeltaMS));
    double time = getTime();
    double length = getLength();
    if( (time == 0.0 && playbackDirection == PlayBackward) ||
        (time == length && playbackDirection == PlayForward)){

        return true;
    }

    return false;
}

}