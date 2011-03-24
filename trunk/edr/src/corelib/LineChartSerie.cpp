#include "CorePCH.h"
#include <core/LineChartSerie.h>
#include <core/ChartData.h>
#include <core/ChartPointer.h>
#include <core/Log.h>
#include <osg/BlendFunc>
#undef min
#undef max

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////


LineChartSerie::LineChartSerie() :
vertices(new osg::Vec3Array)
{
    setDataVariance( osg::Object::DYNAMIC );

    colors = new osg::Vec4Array(1);
    colors->at(0) = osg::Vec4(1,1,1,1);

    setColorArray(colors);
    setColorBinding(osg::Geometry::BIND_OVERALL);
}

void LineChartSerie::setData( const core::ScalarChannelConstPtr& data )
{
    this->data = data;
    tryRefresh();
}

void LineChartSerie::refresh()
{
    UTILS_ASSERT(data, "No data set!");
    UTILS_ASSERT(w > 0 && h > 0, "Wrong size set!");

    typedef ScalarChannel::point_type point_type;
    typedef core::ScalarChannel::time_type time_type;

    // dodajemy wierzcho³ki tylko tak, aby ich gêstoœæ wynosi³a maksymalnie verticesPerUnit per piksel
    // okreœlenie maksymalnej liczby wierzcho³ków
    size_t maxVertices = std::min<size_t>( data->getNumPoints(), static_cast<size_t>(ceil(w * verticesPerUnit)) );
    
    // odrwacamy wykres (koordynaty OGL)
    //float w = this->w - x;
    //float h = this->h - y;
    time_type lenInv = 1 / data->getLength();

    osg::ref_ptr<osg::DrawArrays> primitiveSet;

    if ( maxVertices == data->getNumPoints() ) {
        // rezerwujemy odpowiedni¹ iloœæ miejsca; niekonieczne, ale powinno zmniejszyæ liczbê alokacji
        vertices->reserve( maxVertices );
        vertices->resize(0);
        // nie ma co interpolowaæ, po prostu dodajemy
        BOOST_FOREACH( const ScalarChannel::Entry& entry, *data ) {
            vertices->push_back(osg::Vec3(
                entry.time * lenInv * w + x,
                entry.normalizedValue * h + y,
                z
                ));
        }
        primitiveSet = new osg::DrawArrays(GL_LINE_STRIP,0,vertices->size());
    } else {
        // rezerwujemy odpowiedni¹ iloœæ miejsca; niekonieczne, ale powinno zmniejszyæ liczbê alokacji
        vertices->reserve( maxVertices*2 );
        vertices->resize(0);

        // o ile bêdziemy siê przesuwaæ
        const time_type delta = data->getLength() / maxVertices;
        // przedzia³ czasu w ramach którego siê przesuwamy
        time_type timeStart = 0;
        time_type timeEnd = delta;
        // interatory
        ScalarChannel::const_iterator it = data->begin();
        const ScalarChannel::const_iterator last = data->end();
        // pocz¹tkowe maksimum i minimum w bie¿¹cym przedziale
        point_type nextMaxValue = 0;
        point_type nextMinValue = 1;
        // wierzcho³ki
        osg::Vec3 lineStart(0, 0, z);
        osg::Vec3 lineStop(0, 0, z);

        // pêtla obliczaj¹ca minimum i maksimum w ka¿dym z przedzia³ów oraz dodaj¹ca linie pionowe
        // o d³ugoœci równej amplitudzie
        while ( it != last ) {
            point_type minValue = nextMinValue;
            point_type maxValue = nextMaxValue;
            // wyznaczenie maksimum i minimum z punktów le¿¹cych w ca³oœci w danym przedziale
            while ( it != last && it->time < timeEnd ) {
                maxValue = std::max(maxValue, it->normalizedValue);
                minValue = std::min(minValue, it->normalizedValue);
                ++it;
            }
            // mo¿e prawa skrajna wartoœæ jest wiêksza?
            if ( it != last ) {
                point_type interpolated = data->getNormalizedValue( std::min(data->getLength(), timeEnd) );
                maxValue = std::max(maxValue, interpolated);
                minValue = std::min(minValue, interpolated);
                // uwaga: NIE zerujemy max/min, ¿eby kolejny przedzia³ równie¿ zaczynaæ od zinterpolowanych
                // wartoœci
                nextMaxValue = nextMinValue = interpolated;
            } else {
                nextMaxValue = 0;
                nextMinValue = 1;
            }
            // zamiana ze skali znormalizowanej na faktyczne wartoœci
            lineStop.x() = lineStart.x() = timeStart * lenInv * w + x;
            lineStart.y() = minValue * h + y;
            lineStop.y() = maxValue * h + y;
            if ( lineStart.y() == lineStop.y() ) {
                lineStop.y() += 1;
            }
            vertices->push_back(lineStart);
            vertices->push_back(lineStop);
            // nastêpny przedzia³
            timeStart = timeEnd;
            timeEnd += delta;
        }

        primitiveSet = new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size());


//         // rezerwujemy odpowiedni¹ iloœæ miejsca; niekonieczne, ale powinno zmniejszyæ liczbê alokacji
//         vertices->reserve( maxVertices );
//         vertices->resize(0);
// 
//         
//         time_type timeStart = 0;
//         time_type timeEnd = delta;
//         point_type pointSum = 0;
//         size_t pointCount = 0;
// 
//         float lerpLeft = 0;
//         point_type borderLeft = 0;
//         ScalarChannel::const_iterator it = data->begin();
//         ScalarChannel::const_iterator last = data->end();
//         while (it != last) {
//             time_type t = it->time;
//             // liczmy œredni¹ z punktów w przedziale
//             while ( it != last && it->time < timeEnd ) {
//                 ++pointCount;
//                 pointSum += it->normalizedValue;
//                 ++it;
//             }
//             timeEnd += delta;
//             vertices->push_back(osg::Vec3(
//                 t * lenInv * w + x,
//                 (pointSum / pointCount) * h + y,
//                 -0.1f
//                 ));
//             pointCount = 0;
//             pointSum = 0;
//         }

//         for ( auto it = data->begin(), last = data->end(); it != last; ++it ) {
//             if ( it->time < timeEnd ) {
//                 ++pointCount;
//                 pointSum += it->normalizedValue;
//             } else {
//                 // jesteœmy na granicy
//                 auto next = it + 1;
//                 point_type interpolated = pointSum / pointCount;
//                 interpolated += it->normalizedValue + (timeEnd - it->time) * data->getSamplesPerSec() * (next->normalizedValue - it->normalizedValue);
//             }
//         }
// 
//         BOOST_FOREACH( const ScalarChannel::Entry& entry, *data ) {
//             if ( entry.time < timeEnd ) {
//                 ++pointCount;
//                 pointSum += entry.value;
//             } else {
// 
//             }
//         }
    }
    

    

    


    


//     if ( data->getNumPoints() <= vertices->capacity() ) {
//         // o ile sekund bêdziemy siê przesuwaæ?
//         time_type delta = data->getLength() / vertices->capacity();
//         //
//         for ( time_type t = 0, len = data->getLength(), ; t < len; t += delta ) {
// 
//         }
//     } else {
// 
//     }

    colors->at(0) = color;
    setColorArray(colors);
    setVertexArray(vertices);

    //lineChart->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, false);

    if ( getNumPrimitiveSets() ) {
        setPrimitiveSet(0, primitiveSet);
    } else {
        addPrimitiveSet(primitiveSet);
    }
}

std::pair<float, float> LineChartSerie::getXRange() const
{
    if ( !data ) {
        throw std::runtime_error("Data not set.");
    }
    // TODO: dodaæ offsetowanie!
    return std::make_pair<float, float>( 0, data->getLength() );
}

std::pair<float, float> LineChartSerie::getYRange() const
{
    if ( !data ) {
        throw std::runtime_error("Data not set.");
    }
    return std::make_pair<float, float>( data->getMinValue(), data->getMaxValue() );
}

const std::string& LineChartSerie::getXUnit() const
{
    if ( !data ) {
        throw std::runtime_error("Data not set.");
    }
    return data->getXUnit();
}

const std::string& LineChartSerie::getYUnit() const
{
    if ( !data ) {
        throw std::runtime_error("Data not set.");
    }
    return data->getYUnit();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

//
//deprecated_LineChart::deprecated_LineChart(deprecated_ChartData* data,int x,int y,int width, int height,osg::Vec4 color){
//	this->setName("data seires");
//	this->x=x;
//	this->y=y;
//	this->width=width;
//	this->height=height;
//	this->data=data;
//	chartVertices=new osg::Vec3Array();
//	setColor(color);
//	float currentX=0;
//	showLabel=true;
//	for(int i=0;i<data->getRNumber();i++)
//	{
//
//		if(currentX<data->getNormalizedXValue(i+1)*(width-x))
//		{
//		chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
//		currentX=data->getNormalizedXValue(i)*(width-x)+2;	
//		}
//	}
//	chart=ChartDataSeries::drawChart(chartVertices); 
//	
//
//	pointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data,showLabel);
//	this->addChild(pointer);
//	this->addChild(chart);
//	
//}
//deprecated_LineChart::~deprecated_LineChart(){};
//void deprecated_LineChart::addCoord(osg::Vec3 coord){
//chartVertices->push_back(osg::Vec3(coord));
//
//}
//ChartPointer* deprecated_LineChart::getPointer(){
//return pointer;
//}
//void deprecated_LineChart::repaint(deprecated_ChartData* data,int x,int y,int width,int height){
//	this->x=x;
//	this->y=y;
//	this->width=width;
//	this->height=height;
//	this->data=data;
//	chartVertices=new osg::Vec3Array();
//	float currentX=0;
//	for(int i=0;i<data->getRNumber();i++)
//	{
//	
//		if(currentX<data->getNormalizedXValue(i+1)*(width-x)){
//		chartVertices->push_back(osg::Vec3(data->getNormalizedXValue(i)*(width-x)+x,data->getNormalizedYValue(i)*(height-y)+y,-0.1f));
//		currentX=data->getNormalizedXValue(i)*(width-x)+2;
//		}
//		}
//	
//	osg::Geode* newChart=ChartDataSeries::drawChart(chartVertices); 
//	this->replaceChild(chart,newChart);
//	chart=newChart;
//	ChartPointer* newPointer=new ChartPointer(osg::Vec3(x,y,0),osg::Vec3(width,height,0),data,showLabel);
//	this->replaceChild(pointer,newPointer);
//	pointer=newPointer;
//}
//
//void deprecated_LineChart::setShowLabel(bool showLabel){
//	this->showLabel=showLabel;
//}