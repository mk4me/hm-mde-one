#include <plugins/chart/ChartPCH.h>
#include <core/LineChart.h>
#include <core/ChartData.h>

#include <core/ChartPointer.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////


LineChart::LineChart() :
x(0), y(0), w(0), h(0), 
autoRefresh(false),
vertices(new osg::Vec3Array),
verticesPerUnit(1)
{
}

void LineChart::setData( core::ScalarChannelPtr data )
{
    this->data = data;
    tryRefresh();
}

void LineChart::setLocation( float x, float y, float w, float h )
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    tryRefresh();
}

void LineChart::setLocation( osg::Vec2 location, osg::Vec2 size )
{
    x = location.x();
    y = location.y();
    w = size.x();
    h = size.y();
    tryRefresh();
}

void LineChart::refresh()
{
    UTILS_ASSERT(data, "No data set!");
    UTILS_ASSERT(w > 0 && h > 0, "Wrong size set!");

    typedef core::ScalarChannel::time_type time_type;

    // dodajemy wierzcho³ki tylko tak, aby ich gêstoœæ wynosi³a maksymalnie verticesPerUnit per piksel
    // rezerwujemy odpowiedni¹ iloœæ miejsca; niekonieczne, ale powinno zmniejszyæ liczbê alokacji
    vertices->reserve( static_cast<osg::Vec3Array::size_type>(ceil(w * verticesPerUnit)) );
    vertices->resize(0);

    // o ile sekund bêdziemy siê przesuwaæ?
    time_type delta = data->getLength() / vertices->capacity();
    
    // TODO: to powinno nieco inaczej dzia³aæ, tj okreœlaæ maksimum i minimum w danym przedziale
    float wMinusX = w - x;
    float hMinusY = h - y;
    for ( time_type t = 0, len = data->getLength(), lenInv = 1 / data->getLength(); t < len; len += delta ) {
        vertices->push_back(osg::Vec3(
            t * lenInv * wMinusX + x,
            data->getNormalizedValue(t) * hMinusY + y, // data->getNormalizedValue(t) * h + y
            -0.1f
        ));
    }
    
    // podmianka starego wykresu
    osg::Geode* newChart = drawChart(vertices);
    replaceChild(lineChart, newChart);
    lineChart = newChart;
}

inline void LineChart::tryRefresh()
{
    if (autoRefresh) {
        refresh();
    }
}

void LineChart::setVerticesPerUnit( float verticesPerUnit )
{
    this->verticesPerUnit = verticesPerUnit;
    tryRefresh();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


deprecated_LineChart::deprecated_LineChart(deprecated_ChartData* data,int x,int y,int width, int height,osg::Vec4 color){
	this->setName("data seires");
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->data=data;
	chartVertices=new osg::Vec3Array();
	setColor(color);
	float currentX=0;
	showLabel=true;
	for(int i=0;i<data->getRNumber();i++)
	{

		if(currentX<data->getNormalizedXValue(i+1)*(width-x))
		{
		chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
		currentX=data->getNormalizedXValue(i)*(width-x)+2;	
		}
	}
	chart=ChartDataSeries::drawChart(chartVertices); 
	

	pointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data,showLabel);
	this->addChild(pointer);
	this->addChild(chart);
	
}
deprecated_LineChart::~deprecated_LineChart(){};
void deprecated_LineChart::addCoord(osg::Vec3 coord){
chartVertices->push_back(osg::Vec3(coord));

}
ChartPointer* deprecated_LineChart::getPointer(){
return pointer;
}
void deprecated_LineChart::repaint(deprecated_ChartData* data,int x,int y,int width,int height){
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	this->data=data;
	chartVertices=new osg::Vec3Array();
	float currentX=0;
	for(int i=0;i<data->getRNumber();i++)
	{
	
		if(currentX<data->getNormalizedXValue(i+1)*(width-x)){
		chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
		currentX=data->getNormalizedXValue(i)*(width-x)+2;
		}
		}
	
	osg::Geode* newChart=ChartDataSeries::drawChart(chartVertices); 
	this->replaceChild(chart,newChart);
	chart=newChart;
	ChartPointer* newPointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data,showLabel);
	this->replaceChild(pointer,newPointer);
	pointer=newPointer;
}

void deprecated_LineChart::setShowLabel(bool showLabel){
	this->showLabel=showLabel;
}