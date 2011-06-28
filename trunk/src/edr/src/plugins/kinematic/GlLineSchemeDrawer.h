#ifndef HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#define HEADER_GUARD_GLLINESCHEMEDRAWER_H__
#include <map>
#include <osg/Geode>
#include <osg/Vec4f>
#include <plugins/kinematic/skeletalVisualizationScheme.h>	
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy punkty
class GlLineSchemeDrawer : public OsgSchemeDrawer
{
public:
    GlLineSchemeDrawer(DataToDraw toDraw, int cylinderComplexity, float cylinderRadius) :
      dataToDraw(toDraw) ,
      complex(cylinderComplexity),
      radius(cylinderRadius),
      useCustomColor(false)
      {
      }

      GlLineSchemeDrawer(DataToDraw toDraw, int cylinderComplexity, float cylinderRadius, const osg::Vec4& color) :
        dataToDraw(toDraw) ,
        complex(cylinderComplexity),
        radius(cylinderRadius),
        useCustomColor(true),
        customColor(color)
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

private:
    GeometryPtr createCustomCylinder( int complex , float height, float radius, osg::Vec4 color) const;
    TransformPtr addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color);

private:
    std::vector<TransformPtr> cones;
    osg::ref_ptr<osg::Group> node;
    DataToDraw dataToDraw;
    int complex;
    float radius;
    bool useCustomColor;
    osg::Vec4 customColor;
};

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__