#ifndef MODEL_H
#define MODEL_H


#include "../osgDBPlugin/FMesh.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Shader>
#include <string>

class ObjectService;

//////////////////////////////////////////////////////////////////////////
// class representing model loaded from .tbs file
// model may contain mesh & materials

// TODO:
// tutaj znajduje sie funkcjoanalnosc pozwalajaca
// na przylaczenie efektow, moze sie pewnie jeszcze 
// zdarzyc, ze bedzie trzeba to stad przeniesc/rozbic

typedef osg::ref_ptr<osg::Geode> geoderef;


class CModel
{

        ObjectService* _pScene;

		// properly loaded?
		bool	_properlyLoaded;

		// material id
		int		_materialId;
      
		// not allowed - if You are loading anything, you
		// are supposed to know WHAT you are loading
		// ... ofc it may be changed ;-)
		CModel();

		// loads shader
		bool loadShaderSource(osg::Shader* obj, const std::string& file);

	protected:
		// root
		osg::ref_ptr<osg::Group>	_root;
		// original mesh
		SVertice*					_meshBuffer;
		// meshe
		std::vector<geoderef>   	_meshes;

	
	public:    

    	// draw mesh
		virtual bool updateMesh();

		// valid?
		bool isValid()	{ return _properlyLoaded; }

		// removes geodes from root
		void removeGeodes();

		// draw normals
		void drawNormals(float size);

        // draw WireFrame
        void drawWireFrame(unsigned int mesh_id, unsigned int id);

		// add shaders
		bool addShaders(const std::string& vertex, const std::string& pixel);

		// apply material
        bool applyMaterial(unsigned int mesh_id, unsigned int id);

		// return root
		osg::ref_ptr<osg::Group>	getRoot() { return _root; }		

		// c - tor
		CModel(const std::string& fileName, bool visible = true);
        CModel(SVertice* meshBuffer, bool visible = true);

		// d - tor
		virtual ~CModel();

    private:

        static std::string _groupName;
};

#endif
