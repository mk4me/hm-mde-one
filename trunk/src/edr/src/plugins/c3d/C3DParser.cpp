#include "C3DPCH.h"

#include "c3dParser.h"
#include <string>
#include <vector>
#include <c3dlib/c3dparser.h>
#include <plugins/c3d/C3DChannels.h>
#include <list>
#include <boost/tuple/tuple.hpp>

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
	
	//leftEvents = core::ObjectWrapper::create<C3DEventsCollection>();
	//rightEvents = core::ObjectWrapper::create<C3DEventsCollection>();
    allEvents = core::ObjectWrapper::create<C3DEventsCollection>();
	//c3dMisc = core::ObjectWrapper::create<C3DMisc>();
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
        grfs->setPlatforms(parser->getForcePlatforms());
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
	/*EventsCollectionPtr leftEventsCollection(new C3DEventsCollection());
	EventsCollectionPtr rightEventsCollection(new C3DEventsCollection());*/
    EventsCollectionPtr allEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; i++) {
		c3dlib::C3DParser::IEventPtr event = parser->getEvent(i);
        C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
		//if (event->getContext() == "Left") {
		//	leftEventsCollection->addEvent(parser->getEvent(i));
		//} else if (event->getContext() == "Right") {
		//	rightEventsCollection->addEvent(parser->getEvent(i));
		//} else {
		//	LOG_WARNING("Unknown event context - skipping event");
		//}
	}
	//this->leftEvents->set(leftEventsCollection, path.filename().string(), path.string());
	//this->rightEvents->set(rightEventsCollection, path.filename().string(), path.string());
    this->allEvents->set(allEventsCollection, path.filename().string(), path.string());
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
        computeDataForPlatform(f1, markers, grfs->getPlatforms()[0], allEventsCollection);
        computeDataForPlatform(f2, markers, grfs->getPlatforms()[1], allEventsCollection);

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
	objects.insert(markerChannels);
	/*objects.insert(leftEvents);
	objects.insert(rightEvents);*/
    objects.insert(allEvents);
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

bool C3DParser::isInsideXY( const osg::Vec3& v, c3dlib::ForcePlatformConstPtr fp ) const
{
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = -std::numeric_limits<float>::max();
    for (int i = 0; i < 4; i++) {
        if (fp->corners[i].x() < minx) {
            minx = fp->corners[i].x();
        }
        if (fp->corners[i].y() < miny) {
            miny = fp->corners[i].y();
        }
        if (fp->corners[i].x() > maxx) {
            maxx = fp->corners[i].x();
        }
        if (fp->corners[i].y() > maxy) {
            maxy = fp->corners[i].y();
        }
    }
    if (v.x() >= minx && v.x() <= maxx && v.y() >= miny && v.y() <= maxy) {
        return true;
    }
    return false;
}

void C3DParser::computeDataForPlatform( GRFChannelPtr f1, MarkerCollectionPtr markers, c3dlib::ForcePlatformConstPtr platform, EventsCollectionConstPtr events )
{
    MarkerChannelPtr ltoe = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("LTOE"));
    MarkerChannelPtr rtoe = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("RTOE"));
    MarkerChannelPtr lhee = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("LHEE"));
    MarkerChannelPtr rhee = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("RHEE"));


    
    typedef c3dlib::C3DParser::IEvent IEvent;
    typedef c3dlib::C3DParser::IEvent::Context Context;
    typedef boost::tuple<double, double, Context> Step;
    Step left = boost::make_tuple(-1.0, -1.0, IEvent::Left);
    Step right = boost::make_tuple(-1.0, -1.0, IEvent::Right);
    auto stepTest = [&](const Step& s) { return s.get<0>() >= 0.0 && s.get<1>() >= 0.0; };
    auto getAndRemoveLastStep = [&](std::list<Step>& steps, Context context, Step& ret) -> bool
    {
        for (auto it = steps.begin(); it != steps.end(); it++) {
            if (it->get<2>() == context && (it->get<0>() >= 0.0 && it->get<1>() < 0.0)) {
                ret = *it;
                steps.erase(it);
                return true;
            }
        }

        return false;
    };

    std::list<Step> possibleSteps;
    for (auto it = events->cbegin(); it != events->cend(); it++) {
        const auto e = *it;
        double time = e->getTime();
        osg::Vec3 ltoePos = getChannelValue(time, *ltoe);
        osg::Vec3 rtoePos = getChannelValue(time, *rtoe);
        osg::Vec3 lheePos = getChannelValue(time, *lhee);
        osg::Vec3 rheePos = getChannelValue(time, *rhee);
        Context context = e->getContext();
        if (context == IEvent::Left && isInsideXY(ltoePos, platform) && isInsideXY(lheePos, platform)) {
            std::string l = e->getLabel();
            if (l == "Foot Strike") {
                Step step = boost::make_tuple(e->getTime(), -1.0, context);
                possibleSteps.push_back(step);
            } else if (l == "Foot Off") {
                Step step;
                if (getAndRemoveLastStep(possibleSteps, context, step)) {
                    possibleSteps.push_back(boost::make_tuple(step.get<0>(), e->getTime(), context));
                } else {
                    possibleSteps.push_back(boost::make_tuple(-1.0, e->getTime(), context));
                }
            }
        } 
        
        if (!stepTest(left) && context == c3dlib::C3DParser::IEvent::Right && isInsideXY(rtoePos, platform) && isInsideXY(rheePos, platform)) {
            std::string l = e->getLabel();
            if (l == "Foot Strike") {
                Step step = boost::make_tuple(e->getTime(), -1.0, context);
                possibleSteps.push_back(step);
            } else if (l == "Foot Off") {
                Step step;
                if (getAndRemoveLastStep(possibleSteps, context, step)) {
                    possibleSteps.push_back(boost::make_tuple(step.get<0>(), e->getTime(), context));
                } else {
                    possibleSteps.push_back(boost::make_tuple(-1.0, e->getTime(), context));
                }
            }
        }
        /*if (!stepTest(right) && context == IEvent::Left && isInsideXY(ltoePos, platform) && isInsideXY(lheePos, platform)) {
            std::string l = e->getLabel();
            if (l == "Foot Strike") {
                left.first = true;
                f1->startPoint = lheePos;
                f1->dataStart = e->getTime();
            } else if (l == "Foot Off") {
                left.second = true;
                f1->endPoint = ltoePos;
                f1->dataEnd = e->getTime();
            }
        } 
        
        if (!stepTest(left) && context == c3dlib::C3DParser::IEvent::Right && isInsideXY(rtoePos, platform) && isInsideXY(rheePos, platform)) {
            std::string l = e->getLabel();
            if (l == "Foot Strike") {
                right.first = true;
                f1->startPoint = rheePos;
                f1->dataStart = e->getTime();
            } else if (l == "Foot Off") {
                right.second = true;
                f1->endPoint = rtoePos;
                f1->dataEnd = e->getTime();
            }
        }*/
    }

    if (possibleSteps.size()) {
        possibleSteps.sort([&](const Step& s1, const Step& s2)->bool 
        {
            bool full1 = stepTest(s1);
            bool full2 = stepTest(s2);
            if (full1 && full2) {
                double dur1 = s1.get<1>() - s1.get<0>();
                double dur2 = s2.get<1>() - s2.get<0>();
                return dur1 > dur2;
            } else if (full1) {
                return true;
            } else if (full2) {
                return false;
            } else {
                return (s1.get<0>() < s2.get<0>());
            }
        }
        );


        Step step = *possibleSteps.begin();
        if (stepTest(step)) {
            double time1 = step.get<0>();
            double time2 = step.get<1>();
            f1->startPoint = step.get<2>() == IEvent::Left ? getChannelValue(time1, *lhee) : getChannelValue(time1, *rhee);
            f1->dataStart = time1;
            f1->endPoint = step.get<2>() == IEvent::Left ? getChannelValue(time2, *ltoe) : getChannelValue(time2, *rtoe);
            f1->dataEnd = time2;
        } else {
            double time1 = step.get<0>();
            double time2 = step.get<1>();
            if (time1 < 0.0) {
                time1 = f1->hasStartEndData() ? f1->getDataStartTime() : (std::max)(time2 - 1.0, 0.001);
            }
            if (time2 < 0.0) {
                time2 = f1->hasStartEndData() ? f1->getDataEndTime() : (std::min)(time1 + 1.0, static_cast<double>(f1->getLength()));
            }
            f1->startPoint = step.get<2>() == IEvent::Left ? getChannelValue(time1, *lhee) : getChannelValue(time1, *rhee);
            f1->dataStart = time1;
            f1->endPoint = step.get<2>() == IEvent::Left ? getChannelValue(time2, *ltoe) : getChannelValue(time2, *rtoe);
            f1->dataEnd = time2;
        }
    }

    //float startTime1 = f1->getDataStartTime();
    //float endTime1 = f1->getDataEndTime();
    //
    //float ltoeDistTo1 = platforms[0]->getDistanceToCenter(getChannelValue(endTime1, *ltoe));
    //float ltoeDistTo2 = platforms[1]->getDistanceToCenter(getChannelValue(endTime1, *ltoe));
    //float rtoeDistTo1 = platforms[0]->getDistanceToCenter(getChannelValue(endTime1, *rtoe));
    //float rtoeDistTo2 = platforms[1]->getDistanceToCenter(getChannelValue(endTime1, *rtoe));
    //
    //float lmin = std::min(ltoeDistTo1, ltoeDistTo2);
    //float rmin = std::min(rtoeDistTo1, rtoeDistTo2);
    //
    //for (auto it = events.ceb)
    //    f1->startPoint = osg::Vec3(getChannelValue(startTime1, *lhee));
    //f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *ltoe));
    //if (lmin < rmin) {
    //    if (ltoeDistTo1 < ltoeDistTo2) {
    //
    //    } else {			
    //        f1->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
    //        f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *rtoe));
    //        f2->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
    //        f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
    //    }
    //} else {
    //    if (rtoeDistTo1 < rtoeDistTo2) {
    //        f1->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
    //        f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *rtoe));
    //        f2->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
    //        f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
    //
    //    } else {
    //        f1->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
    //        f1->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
    //        f2->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
    //        f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *rtoe));
    //    }
    //}
}


//void C3DParser::computeDataForPlatform( GRFChannelPtr f1, GRFChannelPtr f2, MarkerCollectionPtr markers, GRFCollectionPtr grfs )
//{
//    if (f1->hasStartEndData() && f2->hasStartEndData()) {
//        MarkerChannelPtr ltoe = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("LTOE"));
//        MarkerChannelPtr rtoe = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("RTOE"));
//        MarkerChannelPtr lhee = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("LHEE"));
//        MarkerChannelPtr rhee = boost::const_pointer_cast<MarkerChannel>(markers->tryGetChannelByName("RHEE"));
//        auto platforms = grfs->getPlatforms();
//        float startTime1 = f1->getDataStartTime();
//        float endTime1 = f1->getDataEndTime();
//        float startTime2 = f2->getDataStartTime();
//        float endTime2 = f2->getDataEndTime();
//
//        float ltoeDistTo1 = platforms[0]->getDistanceToCenter(getChannelValue(endTime1, *ltoe));
//        float ltoeDistTo2 = platforms[1]->getDistanceToCenter(getChannelValue(endTime1, *ltoe));
//        float rtoeDistTo1 = platforms[0]->getDistanceToCenter(getChannelValue(endTime1, *rtoe));
//        float rtoeDistTo2 = platforms[1]->getDistanceToCenter(getChannelValue(endTime1, *rtoe));
//
//        float lmin = std::min(ltoeDistTo1, ltoeDistTo2);
//        float rmin = std::min(rtoeDistTo1, rtoeDistTo2);
//
//        if (lmin < rmin) {
//            if (ltoeDistTo1 < ltoeDistTo2) {
//                f1->startPoint = osg::Vec3(getChannelValue(startTime1, *lhee));
//                f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *ltoe));
//                f2->startPoint = osg::Vec3(getChannelValue(startTime2, *rhee));
//                f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *rtoe));
//            } else {			
//                f1->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
//                f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *rtoe));
//                f2->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
//                f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
//            }
//        } else {
//            if (rtoeDistTo1 < rtoeDistTo2) {
//                f1->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
//                f1->endPoint   = osg::Vec3(getChannelValue(endTime1, *rtoe));
//                f2->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
//                f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
//
//            } else {
//                f1->startPoint = osg::Vec3(getChannelValue(startTime2, *lhee));
//                f1->endPoint   = osg::Vec3(getChannelValue(endTime2, *ltoe));
//                f2->startPoint = osg::Vec3(getChannelValue(startTime1, *rhee));
//                f2->endPoint   = osg::Vec3(getChannelValue(endTime2, *rtoe));
//            }
//        }
//
//    }
//}



