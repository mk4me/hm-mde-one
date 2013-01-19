#include "C3DPCH.h"
#include "C3DParser.h"

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <c3dlib/C3DParser.h>
#include <core/Filesystem.h>
#include <plugins/c3d/C3DChannels.h>
#include "C3DParser.h"
#include "ForcePlatform.h"

C3DParser::C3DParser()
{
    // sztywne stworzenie obiektów, zachowanie kompatybilności
	for(int i = 0; i < 4; ++i){
		GRFChannels.push_back(core::ObjectWrapper::create<GRFChannel>());
	}
	for(int i = 0; i < 16; ++i){
		EMGChannels.push_back(core::ObjectWrapper::create<EMGChannel>());
	}

    // GRF i EMG są dosepne tez poprzez standardowe kolekcje
	GRFs = core::ObjectWrapper::create<GRFCollection>();
	EMGs = core::ObjectWrapper::create<EMGCollection>();
    // reszta kolekcji już bez udziwnien
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

    std::vector<std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parser->importFrom(files, importWarnings);

    // wczytanie danych analogowych
    GRFCollectionPtr grfs(new GRFCollection());
    EMGCollectionPtr e(new EMGCollection());
    if (parser->getNumAnalogs() == 28)
    {
        for (int i = 0; i < 4; ++i) {
            GRFChannelPtr ptr(new GRFChannel(*parser , i));
            GRFChannels[i]->set(ptr);
            //TODO
			//metadane
			//GRFChannels[i]->setName(ptr->getName());
            //GRFChannels[i]->setSource(path.string());
            grfs->addChannel(ptr);
        }

		//TODO
		//metadane
        //GRFs->set(grfs, path.filename().string(), path.string());
        //GRFs->setSource(path.string());
		GRFs->set(grfs);

        for (int i = 12; i < 28; ++i) {
            EMGChannelPtr ptr(new EMGChannel(*parser , i));
            EMGChannels[i-12]->set(ptr);
			//TODO
			//metadane
			//EMGChannels[i-12]->setName(ptr->getName());
            //EMGChannels[i-12]->setSource(path.string());
            e->addChannel(ptr);
        }
        
		//TODO
		//metadane
		//EMGs->set(e, path.filename().string(), path.string());
        //EMGs->setSource(path.string());
		EMGs->set(e);
    }

    // wczytanie eventów
	int count = parser->getNumEvents();
    EventsCollectionPtr allEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; ++i) {
		c3dlib::C3DParser::IEventPtr event = parser->getEvent(i);
        C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
	}
    
	//TODO
	//metadane
	//this->allEvents->set(allEventsCollection, path.filename().string(), path.string());
	this->allEvents->set(allEventsCollection);

    // wczytanie plików *vsk, które dostarczaja opis do markerów
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

	int markersCount = parser->getNumPoints();
	for (int i = 0; i < markersCount; ++i) {
		switch (parser->getPoint(i)->getType()) {
			case c3dlib::C3DParser::IPoint::Angle: {
                AngleChannelPtr ptr(new AngleChannel(*parser, i));
                angles->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Marker: {
                MarkerChannelPtr ptr(new MarkerChannel(*parser, i));
                markers->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Force: {
                ForceChannelPtr ptr(new ForceChannel(*parser, i));
                forces->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Moment: {
                MomentChannelPtr ptr(new MomentChannel(*parser, i));
                moments->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Power: {
                PowerChannelPtr ptr(new PowerChannel(*parser, i));
                powers->addChannel(ptr);
            } break;
		}
	}

	//TODO
	//metadane
	/*markerChannels->set(markers, path.filename().string(), path.string());
	forceChannels->set(forces, path.filename().string(), path.string());
	angleChannels->set(angles, path.filename().string(), path.string());
	momentChannels->set(moments, path.filename().string(), path.string());
	powerChannels->set(powers, path.filename().string(), path.string());*/

	markerChannels->set(markers);
	forceChannels->set(forces);
	angleChannels->set(angles);
	momentChannels->set(moments);
	powerChannels->set(powers);

    try {
        IForcePlatformCollection platforms;
        auto parsedPlatforms = parser->getForcePlatforms();
        int count = static_cast<int>(parsedPlatforms.size());
        for (int i = 0; i < count;  ++i) {
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

plugin::IParser* C3DParser::create()
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
