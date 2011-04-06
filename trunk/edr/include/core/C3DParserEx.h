#ifndef HEADER_GUARD_CORE_C3DPARSER_H__
#define HEADER_GUARD_CORE_C3DPARSER_H__

#include <core/IParser.h>
#include <core/c3dParser.h>
#include <core/IDataManager.h>

#include <core/C3DChannels.h>
#include <kinematiclib/KinematicModel.h>
CORE_DEFINE_WRAPPER(C3D_Data, utils::PtrPolicyBoost);

class MarkerSet : public kinematic::IMarkerSet
{
public:
    MarkerSet() {
        markers.reserve(59);
    }

public:
    void addMarker(core::MarkerChannelConstPtr marker) {
        markers.push_back(marker);
    }

public:
    virtual int getMarkersCount() const { return markers.size(); }
    virtual osg::Vec3 getPosition(int markerNo, double normalizedTime) const {
        float time = normalizedTime * markers[markerNo]->getLength();

        // hack, dla normalizedTime == 1 dostaje sie bzdury
        int sps = markers[markerNo]->getSamplesPerSec();
        int currentSample = time * sps;
        int samples = sps * markers[markerNo]->getLength();

        if (currentSample >= samples) {
            time = static_cast<double>(samples - 1) /  sps;
        }

        return markers[markerNo]->getValue(time) * getScale();
    }
    virtual std::string getMarkerName(int markerNo) const {
        return markers[markerNo]->getName();
    }
    virtual double getDuration() const {
        return markers[0]->getLength();
    }

private:
    std::vector<core::MarkerChannelConstPtr> markers;
};
typedef boost::shared_ptr<MarkerSet> MarkerSetPtr;
typedef boost::shared_ptr<const MarkerSet> MarkerSetConstPtr;
CORE_DEFINE_WRAPPER(MarkerSet, utils::PtrPolicyBoost);


class C3DParser : public core::IParser
{
    UNIQUE_ID('C3DP','PARS');

private:
    boost::filesystem::path path;
    std::vector<core::ObjectWrapperPtr> GRFChannels;
    std::vector<core::ObjectWrapperPtr> EMGChannels;
    core::ObjectWrapperPtr MarkerChannels;

public:

    C3DParser();

    virtual ~C3DParser();
    
    virtual void parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path);

    virtual core::IParser* create();

    virtual std::string getSupportedExtensions() const;

    virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects);

    UTILS_DEPRECATED(C3D_Data* getC3dData());
};
#endif
