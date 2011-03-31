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
    model = core::ObjectWrapper::createWrapper<kinematic::KinematicModel>();
    scheme = core::ObjectWrapper::createWrapper<SkeletalVisualizationScheme>();
}

KinematicParser::~KinematicParser()
{
}

void KinematicParser::parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path)
{
    SkeletalModelPtr modelPtr(new SkeletalModel);

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
    }

    KinematicModelPtr kinematic(new KinematicModel);
    kinematic->setSkeletalData(modelPtr);

    std::vector<MarkerSetPtr> markers = core::queryDataPtr(dataManager);
    // todo : co jesli kolekcja != 1
    if (markers.size() > 0) {
        kinematic->setMarkersData(markers[0]);
    }
    
    model->set<kinematic::KinematicModel>(kinematic);
    model->setName(path.filename());

    SkeletalVisualizationSchemePtr schemePtr = SkeletalVisualizationScheme::create();
    schemePtr->setKinematicModel(kinematic);
    scheme->set<SkeletalVisualizationScheme>(schemePtr);
    scheme->setName(path.filename());
}

core::IParser* KinematicParser::create()
{
    return new KinematicParser();
}

std::string KinematicParser::getSupportedExtensions() const
{
    return "amc;bvh";
}

void KinematicParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
    objects.push_back(model);
    objects.push_back(scheme);
}