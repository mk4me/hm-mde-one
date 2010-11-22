#include "C3DModel.h"



//--------------------------------------------------------------------------------------------------
void C3DModel::Clear()
{
    m_MarkerList.clear();
}

//--------------------------------------------------------------------------------------------------
void C3DModel::SetMarkerList( std::vector<IMarker* > markerList )
{
    m_MarkerList = markerList;
}

//--------------------------------------------------------------------------------------------------
std::vector<IMarker* > C3DModel::GetMarkerList()
{
    return m_MarkerList;
}

//--------------------------------------------------------------------------------------------------
void C3DModel::DrawMarkers()
{
    if (m_spMarkerGeode.valid())
        this->removeChild(m_spMarkerGeode.get());

    // create new geode
    m_spMarkerGeode = new osg::Geode();
    m_spMarkerGeode->setName("markers_geode");

    for(int m = 0; m < m_MarkerList.size(); m++)
    {
        m_spMarkerGeode->addDrawable(DrawMarker(m_MarkerList[m]));
    }

    this->addChild(m_spMarkerGeode);
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geometry> C3DModel::DrawMarker(IMarker* marker)
{
    // draw actual bone
    osg::ref_ptr<osg::Geometry>  geometry = new osg::Geometry();

    // vertices
    osg::Vec3Array* vertices = new osg::Vec3Array();
    vertices->push_back(marker->GetActualPossition());

    // indices
    osg::DrawElementsUInt* point = new osg::DrawElementsUInt(osg::PrimitiveSet::POINTS, 0);
    point->push_back(0);

    // set geometry data
    geometry->setVertexArray(vertices);
    geometry->addPrimitiveSet(point);

    // set colors
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

    osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4> *colorIndexArray;
    colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType,4,4>;
    colorIndexArray->push_back(0);

    geometry->setColorArray(colors);
    geometry->setColorIndices(colorIndexArray);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    return geometry;
}

//--------------------------------------------------------------------------------------------------
osg::ref_ptr<osg::Geode> C3DModel::GetMarkerGeode()
{
    if(m_spMarkerGeode)
        return m_spMarkerGeode;

    return m_spMarkerGeode = new osg::Geode();
}