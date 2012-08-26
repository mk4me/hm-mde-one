#include "ChartPCH.h"
#include "ChartSerie.h"

ChartSerie::ChartSerie() : 
    x(0), y(0), w(0), h(0), z(0),
    autoRefresh(false),
    verticesPerUnit(1)
{

}

void ChartSerie::setVerticesPerUnit( float verticesPerUnit )
{
    this->verticesPerUnit = verticesPerUnit;
    tryRefresh();
}

void ChartSerie::setLocation( float x, float y, float w, float h )
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    tryRefresh();
}

void ChartSerie::setLocation( osg::Vec2 location, osg::Vec2 size )
{
    x = location.x();
    y = location.y();
    w = size.x();
    h = size.y();
    tryRefresh();
}

void ChartSerie::setZ( float z )
{
    this->z = z; 
    tryRefresh();
}

void ChartSerie::setColor( const osg::Vec4& color )
{
    this->color = color;
    tryRefresh();
}
