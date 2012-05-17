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
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius);
    GlPointSchemeDrawer(int sphereComplex, float sphereRadius, const osg::Vec4& color);

public:
    virtual void draw();
    virtual void update();
    virtual void deinit();
    virtual void init(VisualizationSchemeConstPtr scheme);
    virtual osg::ref_ptr<osg::Node> getNode() { return node; }


private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef VisualizationScheme::State MarkerState;

private:
    void createMarkersCrowd(const std::vector<MarkerState>& markers);
    GeodePtr createMarker(osg::Vec4 color, float scale = 1.0f);
    TransformPtr addPoint(const osg::Vec3& point, const osg::Vec4& color, const std::string& name = std::string());
    GeometryPtr createCustomSphere(int complex, osg::Vec4 color) const;
    void changeColorIfNecessary(TransformPtr point, const VisualizationScheme::State& state);

private:
    osg::ref_ptr<osg::Group> node;
    DataToDraw dataToDraw;
    std::vector<TransformPtr> points;
    std::map<osg::Vec4f, GeometryPtr> spheresByColor;
    int complex;
    float radius;
    osg::Vec4 customColor;
    bool useCustomColor;
};
typedef core::shared_ptr<GlPointSchemeDrawer> GlPointSchemeDrawerPtr;
typedef core::shared_ptr<const GlPointSchemeDrawer> GlPointSchemeDrawerConstPtr;

#endif //HEADER_GUARD_POINTSCHEMEDRAWER_H__