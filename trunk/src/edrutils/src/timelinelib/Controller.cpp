#include <timelinelib/Controller.h>
#include <OpenThreads/ScopedLock>
#include <osg/Timer>
#include <osg/Notify>

namespace timeline {

Controller::Controller() : //utils::Observable<State>(&dirtyState),
        modelMutex(), 
        stateMutex(), 
        asynchronous(false),
        timeDirty(false),
        busy(false),
        dirty(false),
        model(new Model())//,
        //dirtyState(model->getState())
{

}

Controller::~Controller(void)
{
    // konczymy
    if ( isRunning() ) {
        // zmiana stanu
//        model->setPlaying(false);
        join();
    }
}

void Controller::play()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    model->lockModel(this);

    State state = getState();
    state.isPlaying = true;
    setState(state);

    //if ( !isRunning() ) {
    //    //start();
    //}
}

void Controller::pause()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    State state = getState();
    state.isPlaying = false;
    setState(state);
}

double Controller::getTime() const
{
    return getState().time;
}

void Controller::setTime(double time)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);

    // nowy stan
    State state = getState();
    state.time = time;
    timeDirty = true;
    model->breakTimeUpdate();
    setState(state);

    //// wyzerowanie licznika
    //if ( !isRunning() && asynchronous ) {
    //    //start();
    //}
}

double Controller::getNormalizedTime() const
{
    return getTime() / model->getLength();
}

void Controller::setNormalizedTime(double normalizedTime)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    setTime( normalizedTime * model->getLength() );
}

const ModelPtr & Controller::getModel()
{
    return model;
}

const ModelConstPtr & Controller::getModel() const
{
    return constModel;
}

bool Controller::isBusy() const
{
    return busy;
}

bool Controller::isDirty() const
{
    return dirty;
}

bool Controller::isPlaying() const
{
    return getState().isPlaying;
}

bool Controller::isTimeDirty() const
{
    return timeDirty;
}

bool Controller::isAsynchronous() const
{
    return asynchronous;
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
            //OSG_NOTICE<<"Finished Controller::run()"<<std::endl;
            return false;
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

void Controller::run()
{
    try
    {
        osg::Timer frameLength;
        while (true) {
            // zerujemy czas ramki
            frameLength.setStartTick();

            if (!compute()) {
                return;
            }

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

bool Controller::isWriteEnable() const
{
    if ( OpenThreads::Thread::CurrentThread() == this ) {
        return true;
    } else if ( const_cast<Controller*>(this)->isRunning() ) {
        return false;
    } else {
        return !asynchronous;
    }
}

const State & Controller::getState() const
{
    if ( isWriteEnable() ) {
        return model->getState();
    } else {
        return dirtyState;
    }
}

void Controller::setState(const State & state)
{
    if ( isWriteEnable() ) {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(modelMutex);
        busy = true;
        model->setState(state);
        dirty = false;
        busy = false;
    } else {
        dirty = true;
    }
    dirtyState = state;
    //notify();
}

}