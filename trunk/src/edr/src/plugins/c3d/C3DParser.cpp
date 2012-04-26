#include "C3DPCH.h"

#include "c3dParser.h"
#include <string>
#include <vector>
#include <c3dlib/c3dparser.h>
#include <plugins/c3d/C3DChannels.h>
#include <list>
#include <boost/tuple/tuple.hpp>
#include "ForcePlatform.h"
#include <core/Filesystem.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

C3DParser::C3DParser()
{
	for(int i = 0; i < 4; i++){
		GRFChannels.push_back(core::ObjectWrapper::create<GRFChannel>());
	}
	for(int i = 0; i < 16; i++){
		EMGChannels.push_back(core::ObjectWrapper::create<EMGChannel>());
	}

	GRFs = core::ObjectWrapper::create<GRFCollection>();
	EMGs = core::ObjectWrapper::create<EMGCollection>();
	markerChannels = core::ObjectWrapper::create<MarkerCollection>();
	forceChannels  = core::ObjectWrapper::create<ForceCollection>();
	angleChannels  = core::ObjectWrapper::create<AngleCollection>();
	momentChannels = core::ObjectWrapper::create<MomentCollection>();
	powerChannels  = core::ObjectWrapper::create<PowerCollection>();
	allEvents = core::ObjectWrapper::create<C3DEventsCollection>();
}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile( const core::Filesystem::Path& path )
{
	ParserPtr parser(new c3dlib::C3DParser());
	parserPtr = parser;
    
    std::vector<const std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parser->importFrom(files, importWarnings);
    
    
    GRFCollectionPtr grfs(new GRFCollection());
    EMGCollectionPtr e(new EMGCollection());
    if (parser->getNumAnalogs() == 28)
    {
        for (int i = 0; i < 4; ++i) {
            GRFChannelPtr ptr(new GRFChannel(*parser , i));
            GRFChannels[i]->set(ptr);
            GRFChannels[i]->setName(ptr->getName());
            GRFChannels[i]->setSource(path.string());
            grfs->addChannel(ptr);
        }

        GRFs->set(grfs, path.filename().string(), path.string());
        GRFs->setSource(path.string());
        
        for (int i = 12; i < 28; ++i) {
            EMGChannelPtr ptr(new EMGChannel(*parser , i));
            EMGChannels[i-12]->set(ptr);
            EMGChannels[i-12]->setName(ptr->getName());
            EMGChannels[i-12]->setSource(path.string());
            e->addChannel(ptr);
        }
        EMGs->set(e, path.filename().string(), path.string());
        EMGs->setSource(path.string());
    }
	int count = parser->getNumEvents();
    
    EventsCollectionPtr allEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; i++) {
		c3dlib::C3DParser::IEventPtr event = parser->getEvent(i);
        C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
	}
    this->allEvents->set(allEventsCollection, path.filename().string(), path.string());
    core::Filesystem::Path dir = path.parent_path();
    std::vector<std::string> vskFiles = core::Filesystem::listFiles(dir, false, ".vsk");
    kinematic::VskParserPtr vsk;
    if (vskFiles.size() == 1) {
        vsk = kinematic::VskParserPtr(new kinematic::VskParser());
        vsk->parse(*vskFiles.begin());
    }
	MarkerCollectionPtr markers(new MarkerCollection(vsk)); 
	ForceCollectionPtr forces(new ForceCollection);
	AngleCollectionPtr angles(new AngleCollection);
	MomentCollectionPtr moments(new MomentCollection);
	PowerCollectionPtr powers(new PowerCollection);
	
#define CHANNEL_CASE(name, collection)						  	 \
	case c3dlib::C3DParser::IPoint::##name:					  	 \
		{														 \
			name##ChannelPtr ptr(new name##Channel(*parser, i)); \
			collection->addChannel(ptr);						 \
		}														 \
		break;

	int markersCount = parser->getNumPoints();
	for (int i = 0; i < markersCount; i++) {
		switch (parser->getPoint(i)->getType()) {
			CHANNEL_CASE(Angle, angles);
			CHANNEL_CASE(Marker, markers);
			CHANNEL_CASE(Force, forces);
			CHANNEL_CASE(Moment, moments);
			CHANNEL_CASE(Power, powers);
		}
	}
	markerChannels->set(markers, path.filename().string(), path.string());
	forceChannels->set(forces, path.filename().string(), path.string());
	angleChannels->set(angles, path.filename().string(), path.string());
	momentChannels->set(moments, path.filename().string(), path.string());
	powerChannels->set(powers, path.filename().string(), path.string());

    try {
        IForcePlatformCollection platforms;
        auto parsedPlatforms = parser->getForcePlatforms();
        int count = parsedPlatforms.size();
        for (unsigned int i = 0; i < count;  i++) {
            ForcePlatformPtr fp(new ForcePlatform(parsedPlatforms[i]));
            // HACK ! POWINIEN BYC KANAL NA PODST. NR PLATFORMY!
            fp->setForceChannel(grfs->getGRFChannel(i == 0 ? GRFChannel::F1 : GRFChannel::F2));
            fp->setMomentChannel(grfs->getGRFChannel(i == 0 ? GRFChannel::M1 : GRFChannel::M2));
            fp->computeSteps(markers, allEventsCollection);
            platforms.push_back(fp);
        }
		grfs->setPlatforms(platforms);
	} catch(...) {}
}

core::IParser* C3DParser::create()
{
    return new C3DParser();
}

void C3DParser::getSupportedExtensions(core::IParser::Extensions & extensions) const
{
    core::IParser::ExtensionDescription extDesc;
    extDesc.description = "C3D format";

    extDesc.types.insert(typeid(GRFChannel));
    extDesc.types.insert(typeid(EMGChannel));
    extDesc.types.insert(typeid(GRFCollection));
    extDesc.types.insert(typeid(EMGCollection));
    extDesc.types.insert(typeid(MarkerCollection));
    extDesc.types.insert(typeid(ForceCollection));
    extDesc.types.insert(typeid(AngleCollection));
    extDesc.types.insert(typeid(MomentCollection));
    extDesc.types.insert(typeid(PowerCollection));
    extDesc.types.insert(typeid(C3DEventsCollection));

    extensions["c3d"] = extDesc;
}

void C3DParser::getObjects( core::Objects& objects )
{
	objects.insert(GRFChannels.begin(), GRFChannels.end());
	objects.insert(EMGChannels.begin(), EMGChannels.end());
	objects.insert(markerChannels);    objects.insert(allEvents);
	objects.insert(EMGs);
	objects.insert(GRFs);
	objects.insert(forceChannels );
	objects.insert(angleChannels );
	objects.insert(momentChannels);
	objects.insert(powerChannels );
	//objects.insert(c3dMisc);
}

void C3DParser::saveFile( const core::Filesystem::Path& path )
{
	if (parserPtr) {
		parserPtr->save(path.string());
	}
}
