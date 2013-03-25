#include "C3DPCH.h"
#include "C3DParser.h"

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <c3dlib/C3DParser.h>
#include <corelib/Filesystem.h>
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

void C3DParser::parse( const std::string & source  )
{
	core::Filesystem::Path path(source);

	parserPtr.reset(new c3dlib::C3DParser());

    std::vector<std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parserPtr->importFrom(files, importWarnings);

	std::vector<double> test = parserPtr->getMovieDelays();

    // wczytanie danych analogowych
    GRFCollectionPtr grfs(new GRFCollection());
    EMGCollectionPtr e(new EMGCollection());
    if (parserPtr->getNumAnalogs() == 28)
    {
        for (int i = 0; i < 4; ++i) {
            GRFChannelPtr ptr(new GRFChannel(*parserPtr , i));
            GRFChannels[i]->set(ptr);
			(*(GRFChannels[i]))["core/name"]  = ptr->getName();
            (*(GRFChannels[i]))["core/source"] = path.string();
            grfs->addChannel(ptr);
        }

		GRFs->set(grfs);
        (*GRFs)["core/source"] = path.string();

        for (int i = 12; i < 28; ++i) {
            EMGChannelPtr ptr(new EMGChannel(*parserPtr , i));
            EMGChannels[i-12]->set(ptr);
			(*(EMGChannels[i-12]))["core/name"]  = ptr->getName();
			(*(EMGChannels[i-12]))["core/source"]  = path.string();
            e->addChannel(ptr);
        }
        
		EMGs->set(e);
        (*EMGs)["core/name"] = path.string();
    }

    // wczytanie eventów
	int count = parserPtr->getNumEvents();
    EventsCollectionPtr allEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; ++i) {
		c3dlib::C3DParser::IEventPtr event = parserPtr->getEvent(i);
        C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
	}
    
	allEvents->set(allEventsCollection);
	(*allEvents)["core/source"] = path.string();
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

	powerChannels->set(powers);
	(*powerChannels)["core/source"] = path.string();

	markerChannels->set(markers);
	(*markerChannels)["core/source"] = path.string();
	forceChannels->set(forces);
	(*forceChannels)["core/source"] = path.string();
	angleChannels->set(angles);
	(*angleChannels)["core/source"] = path.string();
	momentChannels->set(moments);
	(*momentChannels)["core/source"] = path.string();

	int markersCount = parserPtr->getNumPoints();
	for (int i = 0; i < markersCount; ++i) {
		switch (parserPtr->getPoint(i)->getType()) {
			case c3dlib::C3DParser::IPoint::Angle: {
                AngleChannelPtr ptr(new AngleChannel(*parserPtr, i));
                angles->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Marker: {
                MarkerChannelPtr ptr(new MarkerChannel(*parserPtr, i));
                markers->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Force: {
                ForceChannelPtr ptr(new ForceChannel(*parserPtr, i));
                forces->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Moment: {
                MomentChannelPtr ptr(new MomentChannel(*parserPtr, i));
                moments->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Power: {
                PowerChannelPtr ptr(new PowerChannel(*parserPtr, i));
                powers->addChannel(ptr);
            } break;
		}
	}

    try {
        IForcePlatformCollection platforms;
        auto parsedPlatforms = parserPtr->getForcePlatforms();
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

plugin::IParser* C3DParser::create() const
{
    return new C3DParser();
}

void C3DParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "C3D format";

    expDesc.types.insert(typeid(GRFChannel));
    expDesc.types.insert(typeid(EMGChannel));
    expDesc.types.insert(typeid(GRFCollection));
    expDesc.types.insert(typeid(EMGCollection));
    expDesc.types.insert(typeid(MarkerCollection));
    expDesc.types.insert(typeid(ForceCollection));
    expDesc.types.insert(typeid(AngleCollection));
    expDesc.types.insert(typeid(MomentCollection));
    expDesc.types.insert(typeid(PowerCollection));
    expDesc.types.insert(typeid(C3DEventsCollection));

    expressions.insert(Expressions::value_type(".*\.c3d$", expDesc));
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
