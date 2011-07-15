#include "PCH.h"

#include <plugins/timeline/Stream.h>
#include <plugins/timeline/ITimeline.h>

#include <core/IServiceManager.h>

#include <core/IDataManager.h>
#include <core/IVisualizer.h>
#include <plugins/c3d/C3DChannels.h>


#include <core/ILog.h>
#include <core/IService.h>
#include <boost/foreach.hpp>
#include <core/ObjectWrapper.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/kinematic/skeletalVisualizationScheme.h>

#include "SchemeDrawerContainer.h"
#include "KinematicService.h"


using namespace core;
using namespace osg;


KinematicService::KinematicService() :
    name("KinematicService")
{

}

void KinematicService::loadData(IServiceManager* serviceManager, core::IDataManager* dataManager)
{
    ITimelinePtr timeline = core::queryServices<ITimeline>(serviceManager);
    if ( timeline ) {
        for (int i = stream.size() - 1; i >= 0; --i) {
            timeline->removeStream(stream[i]);
        }
        std::vector<SkeletalVisualizationSchemePtr> schemes = core::queryDataPtr(dataManager);
        for (int i = schemes.size() - 1; i >= 0; --i) {
            timeline->addStream(timeline::StreamPtr(new KinematicTimeline(schemes[i])));
        }
    } else {
        OSG_WARN<<"ITimeline not found."<<std::endl;
    }
}

