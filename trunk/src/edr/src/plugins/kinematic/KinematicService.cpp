#include "PCH.h"

//#include <plugins/timeline/Stream.h>
//#include <plugins/timeline/ITimeline.h>

#include <core/IServiceManager.h>

#include <core/IDataManager.h>
#include <core/IVisualizer.h>
#include <plugins/c3d/C3DChannels.h>
//#include <core/C3DChannels.h>
//#include <plugins/timeline/Stream.h>
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

