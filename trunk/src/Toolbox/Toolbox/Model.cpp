#include "Model.h"

#include <osg/Geometry>
#include <osg/Texture2D>

#include <osg/Material>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineStipple>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>

#include "ServiceManager.h"
#include "toolboxmain.h"
#include "ObjectService.h"

#include "../OsgExtensions/CustomGroup.h"
#include "../OsgExtensions/SkeletonNode.h"

using namespace osg;

//--------------------------------------------------------------------------------------------------
// Loads model from file 
Model::Model(const std::string& fileName, bool visible): 
  _root(NULL)
, _materialId(-1)
, _meshBuffer(NULL)
, _properlyLoaded(false)
{
    // calls external function loading model <- look into osgDBPlugin
    ref_ptr<osg::Node> ret = osgDB::readNodeFile(fileName);

    if (dynamic_cast<CCustomGroup*>(ret.get()))
    {
        if (ret.valid() && (_root = ret->asGroup())) // this assign is meant to be here
        {
            // TODO: - materialy dzialaja na "slowo honoru"
            // apply proper material
            vector<vector<int> >* setList = ((CCustomGroup*)_root.get())->getMaterialSetList();
            if (setList && setList->size() && (*setList)[0].size())
                _materialId = (*setList)[0][0];

            // save original mesh
            if(((CCustomGroup*)_root.get())->getMesh() != NULL)
            {
                _meshBuffer = (SVertice*) new byte [((CCustomGroup*)_root.get())->getMesh()->mesh_buffer->n];
                memcpy (_meshBuffer, ((CCustomGroup*)_root.get())->getMesh()->mesh_buffer->buffer, 
                    ((CCustomGroup*)_root.get())->getMesh()->mesh_buffer->n);

                _properlyLoaded = true;
            }
        }
    }

    _pScene = ServiceManager::GetInstance()->GetSystemServiceAs<ObjectService>(); 
}

//--------------------------------------------------------------------------------------------------
// Manually creates the model 
Model::Model(SVertice* meshBuffer,  bool visible): 
  _root(NULL)
, _materialId(-1)
, _meshBuffer(meshBuffer)
, _properlyLoaded(false)
{
}

//--------------------------------------------------------------------------------------------------
Model::~Model()
{
    // it's all i suppose...
    if (_root.valid())
    {
        for (int i = _root->getNumParents() - 1; i >= 0; --i)
        {
            _root->getParent(i)->removeChild(_root);
        }
    }

    if (_meshBuffer)
        delete [] _meshBuffer;
    _meshBuffer = NULL;
}

//--------------------------------------------------------------------------------------------------
// draws normal vectors
void Model::DrawNormals(float size)
{
	if (_meshes.size())
	{
        SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh();

		if (mesh)
		{
			ref_ptr<Geode> geode = new Geode();

			int numOfVertices= mesh->mesh_buffer->n / VERTICES_SIZE;
			SVertice*  vData        = _meshBuffer;

			// for every vertice
			for (int v = 0; v < numOfVertices; ++v)
			{
				ref_ptr<Geometry> geometry = new Geometry();

				// vertices
				Vec3Array* vertices = new Vec3Array();
				vertices->push_back(Vec3(	vData[v].position[0], vData[v].position[1], vData[v].position[2]));
				vertices->push_back(Vec3(	vData[v].position[0] + size * vData[v].normal[0], 
											vData[v].position[1] + size * vData[v].normal[1], 
											vData[v].position[2] + size * vData[v].normal[2]));

				// indices
				osg::DrawElementsUInt* line = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
				line->push_back(0);
				line->push_back(1);

				// set geometry data
				geometry->setVertexArray(vertices);
				geometry->addPrimitiveSet(line);

				// set colors
				osg::Vec4Array* colors = new osg::Vec4Array;
				colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
				colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

				osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
				colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
				colorIndexArray->push_back(0);
				colorIndexArray->push_back(1);

				geometry->setColorArray(colors);
				geometry->setColorIndices(colorIndexArray);
				geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
										
				geode->addDrawable(geometry);
			}

			geode->setName("normals");

			_root->addChild(geode);
		}	
	}
};

//--------------------------------------------------------------------------------------------------
// draws WireFrame
void Model::DrawWireFrame(unsigned int mesh_id, unsigned int id)
{
    ref_ptr<Geode>		geode	 = new Geode();
    ref_ptr<Geometry>	geometry = new Geometry();

    // trzeba to wrzuciæ, jako jedna funkcja - do przemyslenia potem.
    // TODO:  trezba zrobiæ medê, która ³aduje model jako trialngle, a potem robi na niej operacje - w zaleznosci co chcemy.
    // ewentualnie rysowac samemu wireframe  nie korzsytaj¹c z osg PolygonOffset i polymode, poniewa¿ one musza mieæ ca³³y model
    // aby móc wygenerowaæ linie siatki.   model jako point odpada.

    for (unsigned int i = 0; i < (((CCustomGroup*)_root.get())->getNumOfMeshes()); ++i)
    {

        SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(i);

        if (mesh && (_pScene->GetViewModelFlag() & ObjectService::MODEL || _pScene->GetViewModelFlag() & ObjectService::WIREFRAME))
        {
            //    ref_ptr<Geode>		geode	 = new Geode();
            //    ref_ptr<Geometry>	geometry = new Geometry();

            geode->setName("mesh");
            geode->addDrawable(geometry);	

            // vertices, texcoords, normals
            int        numOfVertices= mesh->mesh_buffer->n / VERTICES_SIZE;

            //    SVertice*  vData        = _meshBuffer;
            Vec3Array* vertices     = new Vec3Array();
            Vec2Array* texcoords    = new Vec2Array();
            Vec3Array* normals      = new Vec3Array();

            for (int v = 0; v < numOfVertices; ++v)
            {
                // vertex buffer
                vertices->push_back(Vec3(_meshBuffer[v].position[0], _meshBuffer[v].position[1], _meshBuffer[v].position[2]));
                // texcoords
                texcoords->push_back(Vec2(fabs(_meshBuffer[v].texture_vertex[0]), fabs(_meshBuffer[v].texture_vertex[1])));
                // normals
                normals->push_back(Vec3(_meshBuffer[v].normal[0], _meshBuffer[v].normal[1], _meshBuffer[v].normal[2]));
            }

            geometry->setVertexArray(vertices);
            geometry->setTexCoordArray(0, texcoords);
            geometry->setNormalArray(normals);

            // faces
            //TODO: mozna to zrobic jeszcze lepiej. poprawic troszczeke
            DrawElementsUInt* face;
            for (int f = 0; f < mesh->mesh_faces->face_count; ++f)
            {
                face = new DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
                face->push_back(mesh->mesh_faces->faces[f].index[0]);
                face->push_back(mesh->mesh_faces->faces[f].index[1]);
                face->push_back(mesh->mesh_faces->faces[f].index[2]);
                geometry->addPrimitiveSet(face);
            }
        }
    }

    osg::StateSet* stateset = new osg::StateSet;
    osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.0f);
    polyoffset->setUnits(-1.0f);
    osg::PolygonMode* polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
    stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

#if 1
    osg::Material* material = new osg::Material;
    stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
#else
    // version which sets the color of the wireframe.
    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::OFF); // switch glColor usage off
    // turn all lighting off 
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1.0f,0.0f,1.0f));
    stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
#endif

    if(!(_pScene->GetViewModelFlag() & ObjectService::MODEL))
        RemoveGeodes();
    geode->setStateSet(stateset);
    _root->addChild(geode);
}

//--------------------------------------------------------------------------------------------------
// applies material 
bool Model::ApplyMaterial(unsigned int mesh_id, unsigned int id)
{
	// TODO:
	// at the moment - take diffuse texture and load it
	// we should load proper shader... etc

	vector<SMaterial>* materialList = ((CCustomGroup*)_root.get())->getMaterialList();

	if (materialList && materialList->size() > id)
	{
		texmap::iterator i;
		if ((i = (*materialList)[id].texture_list.find(ETextureType::DIFFUSE_MAP)) != (*materialList)[id].texture_list.end())
		{
			Texture2D* tex = new Texture2D();

			wstring path = ((CCustomGroup*)_root.get())->getPath() + L"/" + i->second;
			tex->setDataVariance(osg::Object::DYNAMIC); 
			osg::Image* klnFace = osgDB::readImageFile(string(path.begin(), path.end()));
			if (!klnFace) return false;
			tex->setImage(klnFace);

			StateSet* stateOne = new StateSet();
            

			stateOne->setTextureAttributeAndModes(0, tex, osg::StateAttribute::OVERRIDE|StateAttribute::ON);

			_meshes[mesh_id]->setStateSet(stateOne);
		}
		else
			return false;
	}
	return true;
}


//--------------------------------------------------------------------------------------------------
// removes geodes
void Model::RemoveGeodes()
{
	//_meshes.clear();
    if(!_root)
        return;
    int index = 0;
	int childNum = _root->getNumChildren();
	for (int i = childNum - 1; i >= 0; --i)
	{	
        index++;
		Geode* geode = dynamic_cast<Geode*>(_root->getChild(i));
		if (geode)
			_root->removeChild(geode);   
	}	
}

//--------------------------------------------------------------------------------------------------
// update mesh
bool Model::UpdateMesh()
{
    // Remove all geodes
    //removeGeodes();

    // for every mesh
    for (unsigned int i = 0; i < (((CCustomGroup*)_root.get())->getNumOfMeshes()); ++i)
    {
        SMesh* mesh = ((CCustomGroup*)_root.get())->getMesh(i);

        if (mesh && (_pScene->GetViewModelFlag() & ObjectService::MODEL || _pScene->GetViewModelFlag() & ObjectService::WIREFRAME))
        {
            ref_ptr<Geode>		geode	 = new Geode();
            ref_ptr<Geometry>	geometry = new Geometry();

            geode->setName("mesh");
            geode->addDrawable(geometry);	

            // TODO:
            // moge wziac z goede raz zbudowanego DrawableList - z niego pobraæ geometry a z geometry Vec3Arrary, co anjwyzej tak moge zrobic update.
            // bez koneicznosci tworzenia nowego...
            // Wykorzystac _meshBuffer;

            // vertices, texcoords, normals
            int numOfVertices= mesh->mesh_buffer->n / VERTICES_SIZE;

            //SVertice*  vData        = _meshBuffer;
            Vec3Array* vertices     = new Vec3Array();
            Vec2Array* texcoords    = new Vec2Array();
            Vec3Array* normals      = new Vec3Array();

            for (int v = 0; v < numOfVertices; ++v) 
            {
                // vertex buffer
                vertices->push_back(Vec3(_meshBuffer[v].position[0], _meshBuffer[v].position[1], _meshBuffer[v].position[2]));
                // texcoords
                texcoords->push_back(Vec2(fabs(_meshBuffer[v].texture_vertex[0]), fabs(_meshBuffer[v].texture_vertex[1])));
                // normals
                normals->push_back(Vec3(_meshBuffer[v].normal[0], _meshBuffer[v].normal[1], _meshBuffer[v].normal[2]));
            }

            geometry->setVertexArray(vertices);
            geometry->setTexCoordArray(0, texcoords);
            geometry->setNormalArray(normals);

            // faces
            for (int f = 0; f < mesh->mesh_faces->face_count; ++f)
            {
                DrawElementsUInt* face = new DrawElementsUInt(_pScene->GetPrimitiveModeFlag(), 0);
                face->push_back(mesh->mesh_faces->faces[f].index[0]);
                face->push_back(mesh->mesh_faces->faces[f].index[1]);
                face->push_back(mesh->mesh_faces->faces[f].index[2]);
                geometry->addPrimitiveSet(face);
            }

            // add new mesh..
            if(_meshes.size() == 0)
                _meshes.push_back(geode);
            else
                _meshes[0] = geode;

            _root->addChild(geode);   //TODO: update zamiast remove;

            // apply material
            if (_materialId >= 0)
            {
                if(_pScene->GetViewModelFlag() & ObjectService::MATERIAL)
                    ApplyMaterial(i, _materialId);
                if(_pScene->GetViewModelFlag() & ObjectService::WIREFRAME)
                    DrawWireFrame(i, _materialId);
            }
        }
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool Model::LoadShaderSource(osg::Shader* obj, const std::string& file)
{
	std::string fqFileName = osgDB::findDataFile(file);
	if (!fqFileName.length())
		return false;

	if (obj->loadShaderSourceFromFile( fqFileName.c_str()))
		return true;
	else
		return false;
}

//--------------------------------------------------------------------------------------------------
bool Model::AddShaders(const std::string& vertex_address, const std::string& pixel_address)
{
	for (std::vector<osg::ref_ptr<osg::Geode> >::iterator i = _meshes.begin(); i != _meshes.end(); ++i)
	{
		if ((*i).valid())
		{
			StateSet* state   = (*i)->getOrCreateStateSet();

			Program* program  = new Program;
			Shader*  vertex   = new Shader(osg::Shader::VERTEX);
			Shader*  fragment = new Shader(osg::Shader::FRAGMENT);

			program->addShader(vertex);
			program->addShader(fragment);

			LoadShaderSource(vertex,   vertex_address);
			LoadShaderSource(fragment, pixel_address);

			state->setAttributeAndModes(program, StateAttribute::ON);	
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
bool Model::IsValid()
{
    return _properlyLoaded; 
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Group> Model::GetRoot()
{
    return _root; 
}