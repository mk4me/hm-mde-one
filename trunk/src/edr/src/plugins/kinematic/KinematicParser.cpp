#include "PCH.h"
#include <core/Filesystem.h>
#include "KinematicParser.h"
#include <plugins/kinematic/Wrappers.h>
#include <kinematiclib/SkeletalModel.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/kinematic/Wrappers.h>
#include "ISchemeDrawer.h"


KinematicParser::KinematicParser()
{
    kinematicMarkersSkeleton = core::ObjectWrapper::create<KinematicModel>();
    schemeMarkersSkeleton = core::ObjectWrapper::create<SkeletalVisualizationScheme>();
    kinematicMarkers = core::ObjectWrapper::create<KinematicModel>();
    schemeMarkers = core::ObjectWrapper::create<SkeletalVisualizationScheme>();
    skeleton = core::ObjectWrapper::create<kinematic::JointAnglesCollection>();
}

KinematicParser::~KinematicParser()
{
}

void KinematicParser::parseFile(core::IDataManager* dataManager, const core::Filesystem::Path& path)
{
	using namespace kinematic;
	using kinematic::AsfParser;
    SkeletalModelPtr modelPtr(new SkeletalModel);
	SkeletalDataPtr dataPtr(new SkeletalData());
    bool fromC3D = false;
    
    if(core::Filesystem::fileExtension(path).compare(".amc") == 0) {
        AmcParser amc;
        AsfParser asf;
        std::string amcFilename = path.string();
        std::string asfFilename = amcFilename.substr(0, amcFilename.size() - 3);
        asfFilename += "asf";
        asf.parse(modelPtr, asfFilename);
        amc.parse(dataPtr, amcFilename);
    } else if (core::Filesystem::fileExtension(path).compare(".bvh") == 0)  {
        BvhParser bvh;
        bvh.parse(modelPtr, dataPtr, path.string());
    } else if (core::Filesystem::fileExtension(path).compare(".c3d") == 0) {
        fromC3D = true;

        // hack - trzeba to jakos sprytniej rozwiazac! o_O
        core::Filesystem::Path p = path.parent_path();
        if (core::Filesystem::isDirectory(p) == true) {
            bool hasSkeletonFile = false;

            for (auto it = core::Filesystem::Iterator(p); it != core::Filesystem::Iterator(); it++) {

                std::string ext = core::Filesystem::fileExtension(*it);
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

    std::vector<MarkerCollectionPtr> markers = core::queryDataPtr(dataManager);
    VskParserPtr vsk(new VskParser);

    if (markers.size() > 0) {
        MarkerCollectionConstPtr ms = markers[0];
        switch(ms->getNumChannels()) {
       case 39:
            vsk->parse(core::getResourceString("trial/M39.vsk"));
            break;
        case 53:
            vsk->parse(core::getResourceString("trial/M53.vsk"));
        default:
            VskParserPtr nullVsk;
            vsk = nullVsk;
        }
    }

    if (markers.size() > 0 && fromC3D) {
        KinematicModelPtr kin(new KinematicModel);
        // hack , co jak dostaniemy wiecej markerow?
        kin->setMarkers(markers[0]);

        //kinematicMarkers->set(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        if (vsk) {
            scheme->setMarkersDataFromVsk(vsk);
        }
        schemeMarkers->set(scheme);
        schemeMarkers->setName(path.filename().string() + " - markers");
    }

    if (markers.size() > 0 && modelPtr && dataPtr && dataPtr->getFrames().size() > 0) {
        KinematicModelPtr kin(new KinematicModel);
        JointAnglesCollectionPtr kinematicSkeleton(new JointAnglesCollection);
        kinematicSkeleton->setSkeletal(modelPtr, dataPtr);
        skeleton->set(kinematicSkeleton);
        kin->setSkeleton(kinematicSkeleton);
        kin->setMarkers(markers[0]);
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

void AsfParser::parseFile( core::IDataManager* dataManager, const core::Filesystem::Path& path )
{
	kinematic::SkeletalModelPtr modelPtr(new kinematic::SkeletalModel);
	kinematic::AsfParser asf;
	asf.parse(modelPtr, path.string());
	skeletalModel->set(modelPtr);
}

core::IParser* AsfParser::create()
{
	return new AsfParser();
}

std::string AsfParser::getSupportedExtensions() const
{
	return ".asf";
}

void AsfParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
	objects.push_back(this->skeletalModel);
}

AsfParser::AsfParser()
{
	this->skeletalModel = core::ObjectWrapper::create<kinematic::SkeletalModel>();
}

AsfParser::~AsfParser()
{

}
