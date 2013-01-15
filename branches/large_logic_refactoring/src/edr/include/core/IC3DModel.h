#ifndef I_C3DMODEL_H
#define I_C3DMODEL_H

#include <osg/Geode>

#include <vector>

class IMarker;

class IC3DModel
{
public:
    virtual ~IC3DModel() {};

    virtual void Clear() = 0;

    virtual void SetMarkerList(std::vector<IMarker* > markerList) = 0;
    virtual std::vector<IMarker* > GetMarkerList() = 0;

    virtual void DrawMarkers() = 0;
    virtual void RemoveGeode() = 0;
    virtual osg::ref_ptr<osg::Geode> GetMarkerGeode() = 0;

    virtual void SetName(std::string name) = 0;
    virtual std::string GetName() const = 0;
};



#endif //I_C3DMODEL_H
