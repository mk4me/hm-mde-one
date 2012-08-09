#ifndef HEADER_GUARD_LINESCHEMEDRAWER_H__
#define HEADER_GUARD_LINESCHEMEDRAWER_H__

#include <core/SmartPtr.h>
#include <osg/PositionAttitudeTransform>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywujacy linie szkieletu
class LineSchemeDrawer : public OsgSchemeDrawer
{
public:
    LineSchemeDrawer()
      {
      }

public:
    virtual void draw();
    virtual void update();
    virtual void deinit();
    virtual void init(VisualizationSchemeConstPtr scheme);
    virtual osg::ref_ptr<osg::Node> getNode();

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef VisualizationScheme::Connection Connection;
    typedef VisualizationScheme::State JointState;

private:
    void addLine(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color);

private:
    std::vector<GeodePtr> lines;
    std::vector< osg::ref_ptr<osg::Vec3Array> > buffers;
    osg::ref_ptr<osg::Group> node;
};

class ConeDrawer : public OsgSchemeDrawer
{
    public:
    ConeDrawer(float lineWidth) : lineWidth(lineWidth) {}

public:
    virtual void draw();
    virtual void update();
    virtual void deinit();
    virtual void init(VisualizationSchemeConstPtr scheme);
    virtual osg::ref_ptr<osg::Node> getNode();

private:
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    float lineWidth;

private:
    TransformPtr addTransform(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color);

private:
    std::vector<TransformPtr> cones;
    osg::ref_ptr<osg::Group> node;
};

#endif
