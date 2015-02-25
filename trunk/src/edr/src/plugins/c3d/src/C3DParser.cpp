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
    
}

C3DParser::~C3DParser()
{

}

void C3DParser::initObjects(utils::ObjectsVector & objects)
{
	// sztywne stworzenie obiektów, zachowanie kompatybilności
	for (int i = 0; i < 4; ++i){
		objects.push_back(utils::ObjectWrapper::create<GRFChannel>());
	}
	for (int i = 0; i < 16; ++i){
		objects.push_back(utils::ObjectWrapper::create<EMGChannel>());
	}

	// GRF i EMG są dosepne tez poprzez standardowe kolekcje
	objects.push_back(utils::ObjectWrapper::create<GRFCollection>());
	objects.push_back(utils::ObjectWrapper::create<EMGCollection>());

	objects.push_back(utils::ObjectWrapper::create<C3DEventsCollection>());
	objects.push_back(utils::ObjectWrapper::create<MovieDelays>());

	// reszta kolekcji już bez udziwnien
	objects.push_back(utils::ObjectWrapper::create<MarkerCollection>());
	objects.push_back(utils::ObjectWrapper::create<ForceCollection>());
	objects.push_back(utils::ObjectWrapper::create<AngleCollection>());
	objects.push_back(utils::ObjectWrapper::create<MomentCollection>());
	objects.push_back(utils::ObjectWrapper::create<PowerCollection>());
}

void C3DParser::parse( const std::string & source  )
{
	core::Filesystem::Path path(source);
	parserPtr = utils::make_shared<c3dlib::C3DParser>();

    std::vector<std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parserPtr->importFrom(files, importWarnings);

	utils::ObjectsVector localData;

	initObjects(localData);

    // wczytanie danych analogowych
    GRFCollectionPtr grfs(new GRFCollection());
    EMGCollectionPtr e(new EMGCollection());
    if (parserPtr->getNumAnalogs() == 28)
    {
        for (int i = 0; i < 4; ++i) {
            GRFChannelPtr ptr(new GRFChannel(*parserPtr , i));
            localData[i]->set(ptr);			
            grfs->addChannel(ptr);
        }

		localData[20]->set(grfs);        

        for (int i = 12; i < 28; ++i) {
            EMGChannelPtr ptr(new EMGChannel(*parserPtr , i));
            localData[4+i-12]->set(ptr);			
            e->addChannel(ptr);
        }
        
		localData[21]->set(e);        
    }

    // wczytanie eventów
	int count = parserPtr->getNumEvents();
    EventsCollectionPtr allEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; ++i) {
		c3dlib::C3DParser::IEventPtr event = parserPtr->getEvent(i);
        C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
	}
    
	localData[22]->set(allEventsCollection);	

	MovieDelaysPtr delays = utils::make_shared<MovieDelays>(parserPtr->getMovieDelays());
	localData[23]->set(delays);	

    // wczytanie plików *vsk, które dostarczaja opis do markerów
    core::Filesystem::Path dir = path.parent_path();
    auto vskFiles = core::Filesystem::listFiles(dir, false, ".vsk");
    vicon::VskPtr vsk;
    if (vskFiles.size() == 1) {
        vsk = utils::make_shared<vicon::Vsk>();
        vicon::VskParser::parse(vskFiles.front().string(), *vsk);
    }
	MarkerCollectionPtr markers(new MarkerCollection(vsk));
	ForceCollectionPtr forces(new ForceCollection);
	AngleCollectionPtr angles(new AngleCollection);
	MomentCollectionPtr moments(new MomentCollection);
	PowerCollectionPtr powers(new PowerCollection);

	localData[24]->set(markers);
	localData[25]->set(forces);
	localData[26]->set(angles);
	localData[27]->set(moments);
	localData[28]->set(powers);	

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
	this->path = source;
	data.swap(localData);
}

void C3DParser::reset()
{
	std::string().swap(path);
	utils::ObjectsVector().swap(data);
}

void C3DParser::getObject(core::Variant & object, const unsigned int idx) const
{
	object.set(data[idx]);
	object.setMetadata("core/source", path);
	if (0 <= idx && idx < 4){
		GRFChannelPtr ptr = data[idx]->get();
		if (ptr) {
			object.setMetadata("core/name", ptr->getName());
		}
	}
	else if(idx < 20){
		EMGChannelPtr ptr = data[idx]->get();
		if (ptr) {
			object.setMetadata("core/name", ptr->getName());
		}
	}
	else if (idx < 22){
		object.setMetadata("core/name", path);
	}
}

plugin::IParser* C3DParser::create() const
{
    return new C3DParser();
}

void C3DParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "C3D format";

	utils::ObjectsVector localData;
	initObjects(localData);
	expDesc.objectsTypes.reserve(localData.size());

	for (auto it = localData.begin(); it != localData.end(); ++it){
		expDesc.objectsTypes.push_back((*it)->getTypeInfo());
	}	

    expressions.insert(Expressions::value_type(".*\\.c3d$", expDesc));
}

void C3DParser::saveFile( const core::Filesystem::Path& path )
{
	if (parserPtr) {
		parserPtr->save(path.string());
	}
}
