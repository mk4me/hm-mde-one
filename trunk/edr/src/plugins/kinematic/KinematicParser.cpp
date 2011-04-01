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

    if (markers.size() > 0 && fromC3D) {
        KinematicModelPtr kin(new KinematicModel);
        // hack , co jak dostaniemy wiecej markerow?
        kin->setMarkersData(markers[0]);
        kinematicMarkers->set<KinematicModel>(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        schemeMarkers->set<SkeletalVisualizationScheme>(scheme);
        schemeMarkers->setName(path.filename() + " - markers");
    } /*else {
        kinematicMarkers->set<KinematicModel>(KinematicModelPtr(new KinematicModel));
        schemeMarkers->set<SkeletalVisualizationScheme>(SkeletalVisualizationScheme::create());
    }*/

    if (modelPtr && modelPtr->getFrames().size() > 0) {
        KinematicModelPtr kin(new KinematicModel);
        kin->setSkeletalData(modelPtr);
        kinematicSkeleton->set<KinematicModel>(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        schemeSkeleton->set<SkeletalVisualizationScheme>(scheme);
        schemeSkeleton->setName(path.filename() + " - skeleton");
    } /*else {
        kinematicSkeleton->set<KinematicModel>(KinematicModelPtr(new KinematicModel));
        schemeSkeleton->set<SkeletalVisualizationScheme>(SkeletalVisualizationScheme::create());
    }*/

    if (markers.size() > 0 && modelPtr && modelPtr->getFrames().size() > 0) {
        KinematicModelPtr kin(new KinematicModel);
        kin->setSkeletalData(modelPtr);
        kin->setMarkersData(markers[0]);
        kinematicMarkersSkeleton->set<KinematicModel>(kin);
        SkeletalVisualizationSchemePtr scheme = SkeletalVisualizationScheme::create();
        scheme->setKinematicModel(kin);
        schemeMarkersSkeleton->set<SkeletalVisualizationScheme>(scheme);
        schemeMarkersSkeleton->setName(path.filename() + " - skeleton + markers");
    } /*else {
        kinematicMarkersSkeleton->set<KinematicModel>(KinematicModelPtr(new KinematicModel));
        schemeMarkersSkeleton->set<SkeletalVisualizationScheme>(SkeletalVisualizationScheme::create());
    }*/

    //KinematicModelPtr kinematic(new KinematicModel);
    //kinematic->setSkeletalData(modelPtr);
    //

   
    //// todo : co jesli kolekcja != 1
    //if (markers.size() > 0) {
    //    kinematic->setMarkersData(markers[0]);
    //}
    //
    //modelWithMarkers->set<kinematic::KinematicModel>(kinematic);
    //modelWithMarkers->setName(path.filename());

    //SkeletalVisualizationSchemePtr schemePtr = SkeletalVisualizationScheme::create();
    //schemePtr->setKinematicModel(kinematic);
    //schemeWithMarkers->set<SkeletalVisualizationScheme>(schemePtr);
    //schemeWithMarkers->setName(path.filename());


    //KinematicModelPtr kinematic2(new KinematicModel);
    //kinematic2->setSkeletalData(modelPtr);
    //modelWithoutMarkers->set<kinematic::KinematicModel>(kinematic2);
    //modelWithoutMarkers->setName(path.filename() + "2");

    //SkeletalVisualizationSchemePtr schemePtr2 = SkeletalVisualizationScheme::create();
    //schemePtr2->setKinematicModel(kinematic2);
    //schemeWithoutMarkers->set<SkeletalVisualizationScheme>(schemePtr2);
    //schemeWithoutMarkers->setName(path.filename() + "2");
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
    //if (!kinematicMarkers->isNull()) {
        objects.push_back(kinematicMarkers);
    //}
    //if (!kinematicSkeleton->isNull()) {
        objects.push_back(kinematicSkeleton);
    //}
    //if (!kinematicMarkersSkeleton->isNull()) {
        objects.push_back(kinematicMarkersSkeleton);
    //}

    //if (!schemeMarkers->isNull()) {
        objects.push_back(schemeMarkers); 
    //}
    //if (!schemeSkeleton->isNull()) {
        objects.push_back(schemeSkeleton);
    //}
    //if (!schemeMarkersSkeleton->isNull()) {
        objects.push_back(schemeMarkersSkeleton);
    //}
}