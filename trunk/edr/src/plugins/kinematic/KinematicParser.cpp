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
    kinematicMarkersSkeleton = core::ObjectWrapper::create<KinematicModel>();
    schemeMarkersSkeleton = core::ObjectWrapper::create<SkeletalVisualizationScheme>();
    kinematicMarkers = core::ObjectWrapper::create<KinematicModel>();
    schemeMarkers = core::ObjectWrapper::create<SkeletalVisualizationScheme>();
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

        // hack - trzeba to jakos sprytniej rozwiazac! o_O
        boost::filesystem::path p = path.parent_path();
        if (is_directory(p)) {
            bool hasSkeletonFile = false;
            for (auto it = directory_iterator(p); it != directory_iterator(); it++) {

                std::string ext = extension(*it);
                if (ext == ".amc" || ext == ".asf" || ext == ".bvh") {
                    hasSkeletonFile = true;
                    break;
                }
            }

            if (hasSkeletonFile) {
                return;
            }
        }
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

        kinematicMarkers->set(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        if (vsk) {
            scheme->setMarkersDataFromVsk(vsk);
        }
        schemeMarkers->set(scheme);
        schemeMarkers->setName(path.filename().string() + " - markers");
    }

    if (markers.size() > 0 && modelPtr && modelPtr->getFrames().size() > 0) {
        KinematicModelPtr kin(new KinematicModel);
        kin->setSkeletalData(modelPtr);
        kin->setMarkersData(markers[0]);
        kinematicMarkersSkeleton->set(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        if (vsk) {
            scheme->setMarkersDataFromVsk(vsk);
        }
        schemeMarkersSkeleton->set(scheme);
        schemeMarkersSkeleton->setName(path.filename().string());
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
    objects.push_back(schemeMarkers);
    objects.push_back(kinematicMarkersSkeleton);
    objects.push_back(schemeMarkersSkeleton);
}