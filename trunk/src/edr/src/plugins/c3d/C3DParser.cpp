#include "C3DPCH.h"

#include "c3dParser.h"
#include <string>
#include <vector>
#include <c3dlib/c3dparser.h>
#include <plugins/c3d/C3DChannels.h>

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
	
	leftEvents = core::ObjectWrapper::create<C3DEventsCollection>();
	rightEvents = core::ObjectWrapper::create<C3DEventsCollection>();

	//c3dMisc = core::ObjectWrapper::create<C3DMisc>();
}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile( core::IDataManager* dataManager, const core::Filesystem::Path& path )
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
        grfs->setPlatforms(parser->getForcePlatforms());
        GRFs->set(grfs, path.filename().string(), path.string());

        
        for (int i = 12; i < 28; ++i) {
            EMGChannelPtr ptr(new EMGChannel(*parser , i));
            EMGChannels[i-12]->set(ptr);
            EMGChannels[i-12]->setName(ptr->getName());
            EMGChannels[i-12]->setSource(path.string());
            e->addChannel(ptr);
        }
        EMGs->set(e, path.filename().string(), path.string());
    }
	int count = parser->getNumEvents();
	EventsCollectionPtr leftEventsCollection(new C3DEventsCollection());
	EventsCollectionPtr rightEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; i++) {
		c3dlib::C3DParser::IEventPtr event = parser->getEvent(i);
		if (event->getContext() == "Left") {
			leftEventsCollection->addEvent(parser->getEvent(i));
		} else if (event->getContext() == "Right") {
			rightEventsCollection->addEvent(parser->getEvent(i));
		} else {
			LOG_WARNING("Unknown event context - skipping event");
		}
	}
	this->leftEvents->set(leftEventsCollection, path.filename().string(), path.string());
	this->rightEvents->set(rightEventsCollection, path.filename().string(), path.string());

	MarkerCollectionPtr markers(new MarkerCollection); 
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
		GRFChannelPtr f1 = boost::const_pointer_cast<GRFChannel>(grfs->getGRFChannel(GRFChannel::F1));
		GRFChannelPtr f2 = boost::const_pointer_cast<GRFChannel>(grfs->getGRFChannel(GRFChannel::F2));
		GRFChannelPtr m1 = boost::const_pointer_cast<GRFChannel>(grfs->getGRFChannel(GRFChannel::M1));
		GRFChannelPtr m2 = boost::const_pointer_cast<GRFChannel>(grfs->getGRFChannel(GRFChannel::M2));
		if (f1->hasStartEndData() && f2->hasStartEndData()) {
			MarkerChannelPtr ltoe = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("LTOE"));
			MarkerChannelPtr rtoe = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("RTOE"));
			MarkerChannelPtr lhee = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("LHEE"));
			MarkerChannelPtr rhee = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("RHEE"));
			auto platforms = grfs->getPlatforms();
			float startTime1 = f1->getDataStartTime();
			float endTime1 = f1->getDataEndTime();
			float startTime2 = f2->getDataStartTime();
			float endTime2 = f2->getDataEndTime();

            /*float ltoeDistTo1 = platforms[0]->getDistanceToCenter((*ltoe)[endTime1]);
            float ltoeDistTo2 = platforms[1]->getDistanceToCenter((*ltoe)[endTime1]);
            float rtoeDistTo1 = platforms[0]->getDistanceToCenter((*rtoe)[endTime1]);
            float rtoeDistTo2 = platforms[1]->getDistanceToCenter((*rtoe)[endTime1]);*/
            float ltoeDistTo1 = platforms[0]->getDistanceToCenter(getChannelValue(endTime1, *ltoe));
            float ltoeDistTo2 = platforms[1]->getDistanceToCenter(getChannelValue(endTime1, *ltoe));
            float rtoeDistTo1 = platforms[0]->getDistanceToCenter(getChannelValue(endTime1, *rtoe));
            float rtoeDistTo2 = platforms[1]->getDistanceToCenter(getChannelValue(endTime1, *rtoe));

			float lmin = std::min(ltoeDistTo1, ltoeDistTo2);
			float rmin = std::min(rtoeDistTo1, rtoeDistTo2);

			if (lmin < rmin) {
				if (ltoeDistTo1 < ltoeDistTo2) {
					/*f1->startPoint = osg::Vec3((*lhee)[startTime1]);
					f1->endPoint   = osg::Vec3((*ltoe)[endTime1]);
					f2->startPoint = osg::Vec3((*rhee)[startTime2]);
					f2->endPoint   = osg::Vec3((*rtoe)[endTime2]);*/

                    f1->startPoint = osg::Vec3(getChannelValue(startTime1, *lhee));
                    f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *ltoe));
                    f2->startPoint = osg::Vec3(getChannelValue(startTime2, *rhee));
                    f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *rtoe));

				} else {			
					/*f1->startPoint = osg::Vec3((*rhee)[startTime1]);
					f1->endPoint   = osg::Vec3((*rtoe)[endTime1]);
					f2->startPoint = osg::Vec3((*lhee)[startTime2]);
					f2->endPoint   = osg::Vec3((*ltoe)[endTime2]);*/

                    f1->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
                    f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *rtoe));
                    f2->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
                    f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
				}
			} else {
				if (rtoeDistTo1 < rtoeDistTo2) {
					/*f1->startPoint = (*rhee)[startTime1];
					f1->endPoint   = (*rtoe)[endTime1];
					f2->startPoint = (*lhee)[startTime2];
					f2->endPoint   = (*ltoe)[endTime2];*/

                    f1->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
                    f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *rtoe));
                    f2->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
                    f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));

				} else {				
					/*f1->startPoint = (*lhee)[startTime2];
					f1->endPoint   = (*ltoe)[endTime2];
					f2->startPoint = (*rhee)[startTime1];
					f2->endPoint   = (*rtoe)[endTime2];*/

                    f1->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
                    f1->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
                    f2->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
                    f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *rtoe));
				}
			}

		}
	} catch(...) {}
}

core::IParser* C3DParser::create()
{
    return new C3DParser();
}

std::string C3DParser::getSupportedExtensions() const
{
    return "c3d";
}

void C3DParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
	objects.insert(objects.end(), GRFChannels.begin(), GRFChannels.end());
	objects.insert(objects.end(), EMGChannels.begin(), EMGChannels.end());
	objects.push_back(markerChannels);
	objects.push_back(leftEvents);
	objects.push_back(rightEvents);
	objects.push_back(EMGs);
	objects.push_back(GRFs);
	objects.push_back(forceChannels );
	objects.push_back(angleChannels );
	objects.push_back(momentChannels);
	objects.push_back(powerChannels );
	//objects.push_back(c3dMisc);
}

void C3DParser::saveFile( const core::Filesystem::Path& path )
{
	if (parserPtr) {
		parserPtr->save(path.string());
	}
}

