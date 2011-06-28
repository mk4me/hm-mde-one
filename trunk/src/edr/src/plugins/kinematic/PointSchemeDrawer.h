#ifndef HEADER_GUARD_POINTSCHEMEDRAWER_H__
#define HEADER_GUARD_POINTSCHEMEDRAWER_H__
#include <osg/Geode>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy punkty
class PointSchemeDrawer : public OsgSchemeDrawer
{
public:
    PointSchemeDrawer(DataToDraw toDraw) :
      dataToDraw(toDraw) 
      {}
public:
    virtual void draw();
    virtual void update();
    virtual void deinit();
    virtual void init(SkeletalVisualizationSchemeConstPtr scheme);
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef SkeletalVisualizationScheme::JointState MarkerState;

private:
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    GeodePtr createMarker(const osg::Vec4& color, float scale = 1.0f) const;
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color);

private:
    std::vector<TransformPtr> points;
    osg::ref_ptr<osg::Group> node;
    DataToDraw dataToDraw;
};

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__