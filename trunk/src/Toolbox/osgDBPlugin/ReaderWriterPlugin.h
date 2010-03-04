#pragma once

#include <osg/Notify>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Material>
#include <osg/TexEnv>
#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <osgDB/Registry>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>

#include <osgUtil/TriStripVisitor>

//MIKEC debug only for PrintVisitor
#include <osg/NodeVisitor>

#include <stdlib.h>
#include <string.h>
#include <cstdio>

#include <set>
#include <map>
#include <iostream>
#include <vector>

#include <Windows.h>
#include <assert.h>

#include "FModel.h"
#include "FMesh.h"

using namespace std;
using namespace osg;

// helper - this name is quite long...
#define pPat PositionAttitudeTransform*


//////////////////////////////////////////////////////////////////////////
//
// ??
//
//////////////////////////////////////////////////////////////////////////
class PrintVisitor : public NodeVisitor
{
public:
	PrintVisitor(std::ostream& out)
		: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN), _out(out)
	{
		_indent = 0;
		_step = 4;
	}

	inline void moveIn() { _indent += _step; }
	inline void moveOut() { _indent -= _step; }
	inline void writeIndent() 
	{
		for(int i=0;i<_indent;++i) _out << " ";
	}

	virtual void apply(Node& node)
	{
		moveIn();
		writeIndent(); 
		_out << node.className() <<std::endl;
		traverse(node);
		moveOut();
	}

	virtual void apply(Geode& node)         { apply((Node&)node); }
	virtual void apply(Billboard& node)     { apply((Geode&)node); }
	virtual void apply(LightSource& node)   { apply((Group&)node); }
	virtual void apply(ClipNode& node)      { apply((Group&)node); }

	virtual void apply(Group& node)         { apply((Node&)node); }
	virtual void apply(Transform& node)     { apply((Group&)node); }
	virtual void apply(Projection& node)    { apply((Group&)node); }
	virtual void apply(Switch& node)        { apply((Group&)node); }
	virtual void apply(LOD& node)           { apply((Group&)node); }

protected:
	PrintVisitor& operator = (const PrintVisitor&) { return *this; }

	std::ostream& _out;
	int _indent;
	int _step;
};


//////////////////////////////////////////////////////////////////////////
//
// class representing plugin used to load .model files
//
//////////////////////////////////////////////////////////////////////////
class ReaderWriterPlugin : public osgDB::ReaderWriter
{
public:
	ReaderWriterPlugin();
	virtual const char* className() const { return "3DS Auto Studio Reader"; }
	virtual ReadResult readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const;

private:

	// reads content
	HRESULT GetModelName(IXMLDOMDocument* pXMLDom, std::wstring* name) const;
	HRESULT LoadMesh(std::wstring* address, SFMesh* fmesh) const;

	template <class T>
	HRESULT ReadAnotherCon(FILE* file, T* ret, const T& anticipated) const;

	HRESULT LoadSkeleton(SSkeleton* skeleton, FILE* meshFile) const;
	HRESULT PushSkeletonToOsg(SSkeleton* skeleton, osg::Group* root) const;
	HRESULT CreateGeometry(SMesh* mesh, osg::Group* root) const;
	HRESULT GetMaterials(SMesh* mesh, SFModel* fmodel, osg::Group* root) const;
};


// now register with Registry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(tbs, ReaderWriterPlugin)