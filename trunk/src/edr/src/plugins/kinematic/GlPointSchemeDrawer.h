#ifndef HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#define HEADER_GUARD_GLPOINTSCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/skeletalVisualizationScheme.h>	
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy punkty
class GlPointSchemeDrawer : public OsgSchemeDrawer
{
public:
    GlPointSchemeDrawer(DataToDraw toDraw, int sphereComplex, float sphereRadius) :
      dataToDraw(toDraw) ,
      complex(sphereComplex),
      radius(sphereRadius)
      {
      }
public:
    virtual void draw();
    virtual void update();
    virtual void deinit();
    virtual void init(SkeletalVisualizationSchemeConstPtr scheme);
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef SkeletalVisualizationScheme::JointState MarkerState;

private:
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    GeodePtr createMarker(osg::Vec4 color, float scale = 1.0f);
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color);
    GeometryPtr createCustomSphere(int complex, osg::Vec4 color) const;

private:
    osg::ref_ptr<osg::Group> node;
    DataToDraw dataToDraw;
    std::vector<TransformPtr> points;
    std::map<osg::Vec4f, GeometryPtr> spheresByColor;
    int complex;
    float radius;
};

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__