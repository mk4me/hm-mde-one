#ifndef MODEL_H
#define MODEL_H

#include "../osgDBPlugin/FMesh.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Shader>
#include <string>

class ObjectService;

//--------------------------------------------------------------------------------------------------
// class representing model loaded from .tbs file model may contain mesh & materials

// TODO:
// tutaj znajduje sie funkcjoanalnosc pozwalajaca
// na przylaczenie efektow, moze sie pewnie jeszcze 
// zdarzyc, ze bedzie trzeba to stad przeniesc/rozbic

typedef osg::ref_ptr<osg::Geode> geoderef;

//--------------------------------------------------------------------------------------------------
class Model
{
public: 
    // Loads model from file 
    Model(const std::string& fileName, bool visible = true);
    // Manually creates the model 
    Model(SVertice* meshBuffer, bool visible = true);
    virtual ~Model();

    // draw mesh
    virtual bool UpdateMesh();
    // valid?
    bool IsValid(); 
    // removes geodes from root
    void RemoveGeodes();
    // draw normals
    void DrawNormals(float size);
    // draw WireFrame
    void DrawWireFrame(unsigned int mesh_id, unsigned int id);
    // add shaders
    bool AddShaders(const std::string& vertex, const std::string& pixel);
    // apply material
    bool ApplyMaterial(unsigned int mesh_id, unsigned int id);
    // return root
    osg::ref_ptr<osg::Group> GetRoot(); 	
    // loads shader
    bool LoadShaderSource(osg::Shader* obj, const std::string& file);

public: 
    // TODO: Fix this 
    ObjectService* _pScene;
    // properly loaded?
    bool _properlyLoaded;
    // material id
    int _materialId;

protected:
    // Root Node 
    osg::ref_ptr<osg::Group> _root;
    // Original mesh
    SVertice* _meshBuffer;
    // Meshes
    std::vector<geoderef> _meshes;

private:
    static std::string _groupName;

};

#endif
