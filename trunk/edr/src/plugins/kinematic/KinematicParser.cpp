#include "PCH.h"
#include "KinematicParser.h"
#include <boost/filesystem.hpp>
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/KinematicModel.h>
#include <core/C3DParserEx.h>
#include <plugins/kinematic/Wrappers.h>
#include <core/C3DChannels.h>
#include "ISchemeDrawer.h"

using namespace kinematic;
using namespace boost::filesystem;

KinematicParser::KinematicParser()
{
    kinematicMarkers = core::ObjectWrapper::createWrapper<KinematicModel>();
    kinematicSkeleton = core::ObjectWrapper::createWrapper<KinematicModel>();
    kinematicMarkersSkeleton = core::ObjectWrapper::createWrapper<KinematicModel>();
    schemeMarkers = core::ObjectWrapper::createWrapper<SkeletalVisualizationScheme>(); 
    schemeSkeleton = core::ObjectWrapper::createWrapper<SkeletalVisualizationScheme>();
    schemeMarkersSkeleton = core::ObjectWrapper::createWrapper<SkeletalVisualizationScheme>();
}

KinematicParser::~KinematicParser()
{
}

void KinematicParser::parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path)
{
    SkeletalModelPtr modelPtr(new SkeletalModel);

    bool fromC3D = false;
    
    if(extension(path) == ".amc") {
        AmcParser amc;
        AsfParser asf;
        std::string amcFilename = path.string();
        std::string asfFilename = amcFilename.substr(0, amcFilename.size() - 3);
        asfFilename += "asf";
        asf.parse(modelPtr, asfFilename);
        amc.parse(modelPtr, amcFilename);
    } else if (extension(path) == ".bvh")  {
        BvhParser bvh;
        bvh.parse(modelPtr, path.string());
    } else if (extension(path) == ".c3d") {
        fromC3D = true;
    }

    std::vector<MarkerSetPtr> markers = core::queryDataPtr(dataManager);
    VskParserPtr vsk(new VskParser);

    if (markers.size() > 0) {
        MarkerSetConstPtr ms = markers[0];
        switch(ms->getMarkersCount()) {
        case 39:
            vsk->parse("data/resources/trial/M39.vsk");
            break;
        case 53:
            vsk->parse("data/resources/trial/M53.vsk");
        default:
            VskParserPtr nullVsk;
            vsk = nullVsk;
        }
    }

    if (markers.size() > 0 && fromC3D) {
        KinematicModelPtr kin(new KinematicModel);
        // hack , co jak dostaniemy wiecej markerow?
        kin->setMarkersData(markers[0]);
        
        kinematicMarkers->set<KinematicModel>(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        if (vsk) {
            scheme->setMarkersDataFromVsk(vsk);
        }
        schemeMarkers->set<SkeletalVisualizationScheme>(scheme);
        schemeMarkers->setName(path.filename() + " - markers");
    }

    if (modelPtr && modelPtr->getFrames().size() > 0) {
        KinematicModelPtr kin(new KinematicModel);
        kin->setSkeletalData(modelPtr);
        kinematicSkeleton->set<KinematicModel>(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        schemeSkeleton->set<SkeletalVisualizationScheme>(scheme);
        schemeSkeleton->setName(path.filename() + " - skeleton");
    }

    if (markers.size() > 0 && modelPtr && modelPtr->getFrames().size() > 0) {
        KinematicModelPtr kin(new KinematicModel);
        kin->setSkeletalData(modelPtr);
        kin->setMarkersData(markers[0]);
        kinematicMarkersSkeleton->set<KinematicModel>(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        if (vsk) {
            scheme->setMarkersDataFromVsk(vsk);
        }
        schemeMarkersSkeleton->set<SkeletalVisualizationScheme>(scheme);
        schemeMarkersSkeleton->setName(path.filename() + " - skeleton + markers");
    }
}

core::IParser* KinematicParser::create()
{
    return new KinematicParser();
}

std::string KinematicParser::getSupportedExtensions() const
{
    return "amc;bvh;c3d";
}

void KinematicParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
    objects.push_back(kinematicMarkers);
    objects.push_back(kinematicSkeleton);
    objects.push_back(kinematicMarkersSkeleton);
    objects.push_back(schemeMarkers); 
    objects.push_back(schemeSkeleton);
    objects.push_back(schemeMarkersSkeleton);
}