/********************************************************************
	created:	2011/07/14
	created:	14:7:2011   10:52
	filename: 	Motion.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__MOTION_H__
#define HEADER_GUARD_SUBJECT__MOTION_H__

#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/video/Wrappers.h>
#include <core/PluginCommon.h>
#include <core/SmartPtr.h>
#include <core/TypeInfo.h>
#include <boost/range.hpp>

//! Klasa reprezentuje pojedyncza probe pomiarowa w ramach konkretnej sesji.
class Motion
{
public:
	/*Motion();
    Motion(const std::vector<core::TypeInfo>& supportedTypes);*/
    Motion()
    {
        fillDefaultTypesList();
    }

    Motion( const std::vector<core::TypeInfo>& supportedTypes )
    {
        types = supportedTypes;
    }
	virtual ~Motion() {}

public:
    typedef std::vector<core::ObjectWrapperPtr>::iterator iterator;
    typedef std::vector<core::ObjectWrapperPtr>::const_iterator const_iterator;
    typedef boost::iterator_range<iterator> range;
    typedef boost::iterator_range<const_iterator> const_range;

public:
	template <class Ptr>
	bool tryGet(Ptr& object, bool exact = false)
	{
		for(auto it = wrappers.begin(); it != wrappers.end(); it++) {
			if ((*it)->tryGet(object, exact)) {
				return true;
			}
		};

		return false;
	}

	bool hasObjectOfType(const core::TypeInfo& type, bool exact = false)
    {
        if (type == typeid(kinematic::JointAnglesCollection)) {
            return hasJoints();
        } 

	    for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
		    if ((*it)->isSupported(type)) {
			    return true;
		    }
	    }

	    return false;
    }

    core::ObjectWrapperPtr getWrapperOfType(const core::TypeInfo& type, bool exact = false)
    {
        if (type == typeid(kinematic::JointAnglesCollection)) {
            return getJoints();
        }

        for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
            if ((*it)->isSupported(type)) {
                return *it;
            }
        }

        throw std::runtime_error("Object does not exist");
    }

    void addWrapper( const core::ObjectWrapperPtr& wrapper )
    {
        if (!isSupported(wrapper->getTypeInfo())) {
            throw std::runtime_error("type is not supported");
        }
        wrappers.push_back(wrapper);
    }

    template <class Collection>
    void addWrappers(const Collection& wrappers) 
    {
        for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
            addWrapper(*it);
        }
    }

    bool isSupported( const core::TypeInfo& typeToCheck )
    {
        for (auto it = types.begin(); it != types.end(); it++) {
            if (*it == typeToCheck) {
                return true;
            }
        }
        return false;
    }
	//void addWrapper(const core::ObjectWrapperPtr& wrapper);
    //void addWrappers(const std::vector<core::ObjectWrapperPtr>& wrappers);
    range getWrappers() 
    {
        return boost::make_iterator_range(wrappers.begin(), wrappers.end());
    }

    const_range getWrappers() const
    {
        return boost::make_iterator_range(wrappers.cbegin(), wrappers.cend());
    }

    std::vector<core::ObjectWrapperPtr> getWrappers(const core::TypeInfo& type)
    {
        std::vector<core::ObjectWrapperPtr> temp;
        for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
            if ((*it)->getTypeInfo() == type) {
                temp.push_back(*it);
            }
        }
    
       return temp;
    }

    template<class Channel, class Collection>
    std::vector<core::ObjectWrapperPtr> getWrappersFromCollection(const Collection& collection)
    {
        Collection& tempCollection = const_cast<Collection&>(collection);
        std::vector<core::ObjectWrapperPtr> temp;
        int count = tempCollection.getNumChannels();
        int wrappersCount = wrappers.size();
        for (int i = 0; i < count; i++) {
            Channel c = boost::dynamic_pointer_cast<Channel::element_type>(tempCollection.getChannel(i));
            for (int j = 0; j < wrappersCount; j++) {
               Channel wc;
               if (wrappers[j]->tryGet(wc)) {
                  if (wc == c) {
                    temp.push_back(wrappers[j]);
                  }
               }
            }
        }
        return temp;
    }

    //bool isSupported(const core::TypeInfo& type);
    
    std::string getName() const { return name; }
    void setName(std::string val) { name = val; }

private:
	//void createJointCollection(kinematic::SkeletalModelConstPtr model, kinematic::SkeletalDataConstPtr data);
    void fillDefaultTypesList()
    {
        types.clear();
        types.push_back(typeid(GRFCollection));
        types.push_back(typeid(GRFChannel));
        types.push_back(typeid(EMGCollection));
        types.push_back(typeid(EMGChannel));
        types.push_back(typeid(kinematic::JointAnglesCollection));
        types.push_back(typeid(kinematic::SkeletalData));
        types.push_back(typeid(kinematic::SkeletalModel));
        types.push_back(typeid(MarkerCollection));
        types.push_back(typeid(ForceCollection));
        types.push_back(typeid(AngleCollection));
        types.push_back(typeid(PowerCollection));
        types.push_back(typeid(MomentCollection));
        types.push_back(typeid(VectorChannel));
        types.push_back(typeid(C3DEventsCollection));
        types.push_back(typeid(VideoChannel));
    }
	//void fillDefaultTypesList();
    bool hasJoints()
    {
        bool hasData = false;
        bool hasModel = false;
        for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
            if ((*it)->isSupported(typeid(kinematic::JointAnglesCollection))) {
                return true;
            } else if ((*it)->isSupported(typeid(kinematic::SkeletalData))) {
                hasData = true;
            } else if ((*it)->isSupported(typeid(kinematic::SkeletalModel))) {
                hasModel = true;
            }
        }
    
        return hasModel && hasData;
    }

    core::ObjectWrapperPtr getJoints()
    {
        core::ObjectWrapperPtr dataWrapper;
        core::ObjectWrapperPtr modelWrapper;
        for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
            if ((*it)->isSupported(typeid(kinematic::JointAnglesCollection))) {
                return *it;
            } else if ((*it)->isSupported(typeid(kinematic::SkeletalData))) {
                dataWrapper = *it;
            } else if ((*it)->isSupported(typeid(kinematic::SkeletalModel))) {
                modelWrapper = *it;
            }
        }

        if (dataWrapper && modelWrapper) {
            core::IDataManager* manager = core::getDataManager();
            manager->tryParseWrapper(dataWrapper);
            manager->tryParseWrapper(modelWrapper);
            kinematic::SkeletalDataPtr data = dataWrapper->get();
            kinematic::SkeletalModelPtr model = modelWrapper->get();
            kinematic::JointAnglesCollectionPtr joints(new kinematic::JointAnglesCollection());
            joints->setSkeletal(model, data);

            core::ObjectWrapperPtr jointsWrapper = core::ObjectWrapper::create<kinematic::JointAnglesCollection>();
            jointsWrapper->set(joints);
            wrappers.push_back(jointsWrapper);
            manager->addExternalData(jointsWrapper);
            return jointsWrapper;
        }

        throw std::runtime_error("Object does not exist");
    }

private:
	std::vector<core::ObjectWrapperPtr> wrappers;
	std::vector<core::TypeInfo> types;
	std::string name;
    
};


typedef core::shared_ptr<Motion> MotionPtr;
typedef core::shared_ptr<const Motion> MotionConstPtr;

//class Motion
//{
//public:
//	Motion()
//	{
//	}
//	virtual ~Motion() {}
//
//public:
//	GRFCollectionConstPtr getGrf() const { return grf; }
//	void setGrf(GRFCollectionConstPtr val) { grf = val; }
//
//	EMGCollectionConstPtr getEmg() const { return emg; }
//	void setEmg(EMGCollectionConstPtr val) { emg = val; }
//
//	kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }
//	void setJoints(kinematic::JointAnglesCollectionConstPtr val) { joints = val; }
//	void setSkeletalModel(kinematic::SkeletalModelConstPtr val)
//	{
//		skeletalModel = val;
//		if (skeletalModel && skeletalData) {
//			createJointCollection(skeletalModel, skeletalData);
//		}
//
//	}
//	void setSkeletalData(kinematic::SkeletalDataConstPtr val) 
//	{
//		skeletalData = val;
//		if (skeletalModel && skeletalData) {
//			createJointCollection(skeletalModel, skeletalData);
//		}
//	}
//
//	MarkerCollectionConstPtr getMarkers() const { return markers; }
//	void setMarkers(MarkerCollectionConstPtr val) { markers = val; }
//
//	const std::vector<VideoChannelConstPtr>& getVideos() const { return videos; }
//	void addVideo(VideoChannelConstPtr val) 
//	{ 
//		videos.push_back(val); 
//	}
//
//	std::string getName() const { return name; }
//	void setName(std::string val) { name = val; }
//
//	ForceCollectionConstPtr getForces() const { return forces; }
//	void setForces(ForceCollectionConstPtr val) { forces = val; }
//
//	AngleCollectionConstPtr getAngles() const { return angles; }
//	void setAngles(AngleCollectionConstPtr val) { angles = val; }
//
//	PowerCollectionConstPtr getPowers() const { return powers; }
//	void setPowers(PowerCollectionConstPtr val) { powers = val; }
//
//	MomentCollectionConstPtr getMoments() const { return moments; }
//	void setMoments(MomentCollectionConstPtr val) { moments = val; }
//
//    EventsCollectionConstPtr getEvents() const { return events; }
//    void setEvents(EventsCollectionConstPtr val) { events = val; }
//
//private:
//	void createJointCollection(kinematic::SkeletalModelConstPtr model, kinematic::SkeletalDataConstPtr data);
//
//
//private:
//	GRFCollectionConstPtr grf;
//	EMGCollectionConstPtr emg;
//	kinematic::JointAnglesCollectionConstPtr joints;
//	kinematic::SkeletalDataConstPtr skeletalData;
//	kinematic::SkeletalModelConstPtr skeletalModel;
//	MarkerCollectionConstPtr markers;
//	ForceCollectionConstPtr forces;
//	AngleCollectionConstPtr angles;
//	PowerCollectionConstPtr powers;
//	MomentCollectionConstPtr moments;
//    EventsCollectionConstPtr events;
//
//	std::vector<VideoChannelConstPtr> videos;
//	std::string name;
//};
//typedef core::shared_ptr<Motion> MotionPtr;
//typedef core::shared_ptr<const Motion> MotionConstPtr;


#endif
