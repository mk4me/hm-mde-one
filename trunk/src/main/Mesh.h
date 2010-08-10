#ifndef MESH_H
#define MESH_H

#include "../osgDBPlugin/ModelData.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Shader>
#include <osgUtil/GLObjectsVisitor>
#include <string>


// Narazie istnieje struktura w pliku FMesh.h o nazwie SMesh - która pe³ni rolê tej klasy.

class Mesh: public osg::Geometry
{
public:

    //Model Data
    Mesh( SModelData* modelData);

    virtual void update() = 0;

    osg::BoundingBox computeBound() const { return boundingBox; }

    const SModelData* getModelData() const { return modelData; }


    /**
    * We don't support osgUtil::TransformAttributeFunctor
    * since for ridig meshes we share the same vertex
    * buffer that must not be transformed for each model.
    */
    virtual bool supports( const AttributeFunctor& ) { return false; }

    /**
    * For compiling all statesets when Model accepts osgUtil::GLObjectsVisitor.
    * Don't used by software meshes, used by depth and hardware ones.
    */
    virtual void accept( osgUtil::GLObjectsVisitor* ) {}

protected:

    osg::ref_ptr< SModelData >                modelData;
    osg::BoundingBox                      boundingBox;
    bool                                  deformed;
    
};

#endif  // MESH_H
