#include "CorePCH.h"
#include <core/Log.h>
#include "ServiceManager.h"
#include "ComputeThread.h"


ComputeThread::ComputeThread( ServiceManager* serviceManager, double refreshRate ) 
:   serviceManager(serviceManager), 
    refreshRate(refreshRate), 
    done(false)
{
}


ComputeThread::~ComputeThread()
{
    // quit
    if ( isRunning() ) {
        // zmieniamy stan...
        setDone(true);
        join();
    }
}

void ComputeThread::setDone( bool done )
{
    this->done = done;
}

void ComputeThread::run()
{
    try
    {
        osg::Timer frameLength;
        while (!done) {
            // zerujemy czas ramki
            frameLength.setStartTick();

            //!
            serviceManager->computePass();

            // jak dlugo to trwa³o?
            double waitTime = refreshRate - frameLength.time_s();
            if ( waitTime > 0.0 ) {
                OpenThreads::Thread::microSleep( waitTime * 1000000 );
            }
        } 
    } catch (const std::exception & error) {
        LOG_ERROR("ComputeThread::run : " << error.what());
    } catch (...) {
        LOG_ERROR("ComputeThread::run : unhandled exception");
    }
}
