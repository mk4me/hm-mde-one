#ifndef C3DMODEL_H
#define C3DMODEL_H

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>

#include <vector>

#include <core/IC3DModel.h>
#include <core/IMarker.h>


class C3DModel: public IC3DModel, public osg::Group
{
public:
    virtual void Clear();

//     virtual GetAnimation();
//     virtual void SetAnimation();

    virtual void SetMarkerList(std::vector<IMarker* > markerList);
    virtual std::vector<IMarker* > GetMarkerList();

    virtual void DrawMarkers();
    virtual void RemoveGeode();
    virtual osg::ref_ptr<osg::Geode> GetMarkerGeode();

    virtual void SetName(std::string name);
    virtual std::string GetName() const;

private:
    osg::ref_ptr<osg::Geometry> DrawMarkerAsPrimitivePoint(IMarker* marker);

    std::string m_Name;
    std::vector<IMarker* > m_MarkerList;
    osg::ref_ptr<osg::Geode> m_spMarkerGeode;
};


#endif C3DMODEL_H