#include "ChartPCH.h"
#include <core/ILog.h>
#include <osg/BlendFunc>
#include "LineChartSerie.h"
#include <boost/foreach.hpp>

#undef min
#undef max


LineChartSerie::LineChartSerie() :
vertices(new osg::Vec3Array), timer(new GeneralTimer())
{
    setDataVariance( osg::Object::DYNAMIC );

    colors = new osg::Vec4Array(1);
    colors->at(0) = osg::Vec4(1,1,1,1);

    setColorArray(colors);
    setColorBinding(osg::Geometry::BIND_OVERALL);
}

void LineChartSerie::setData( const ScalarChannelReaderInterfaceConstPtr& channel )
{
    this->channel = channel;

    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(channel));

    if(normalizedChannel == nullptr){
        normalizedChannel.reset(new ScalarModifier(nonConstChannel, ScalarChannelNormalizer()));
    }else{
        normalizedChannel->setObservedChannel(nonConstChannel);
    }

    if(accessor == nullptr){
        accessor.reset(new ScalarContiniousTimeAccessor(normalizedChannel));
    }

    if(currentValue == nullptr){
        currentValue.reset(new ScalarCurentValueExtractor(accessor, timer));
    }

    stats.reset(new ScalarChannelStats(normalizedChannel));
    tryRefresh();
}

void LineChartSerie::refresh()
{
    UTILS_ASSERT(normalizedChannel, "No data set!");
    UTILS_ASSERT(w > 0 && h >= 0, "Wrong size set!");

    typedef ScalarChannelReaderInterface::point_type point_type;
    typedef ScalarChannelReaderInterface::time_type time_type;

    // dodajemy wierzchołki tylko tak, aby ich gęstość wynosiła maksymalnie verticesPerUnit per piksel
    // określenie maksymalnej liczby wierzchołków
    size_t maxVertices = std::min<size_t>( normalizedChannel->size(), static_cast<size_t>(ceil(w * verticesPerUnit)) );
    
    // odrwacamy wykres (koordynaty OGL)
    time_type lenInv = 1 / normalizedChannel->getLength();

    osg::ref_ptr<osg::DrawArrays> primitiveSet;

    if ( maxVertices == normalizedChannel->size() ) {
        // rezerwujemy odpowiednią ilość miejsca; niekonieczne, ale powinno zmniejszyć liczbę alokacji
        vertices->reserve( maxVertices );
        vertices->resize(0);
        // nie ma co interpolować, po prostu dodajemy
        for(auto i = 0; i != normalizedChannel->size(); i++) {
            vertices->push_back(osg::Vec3(
                normalizedChannel->argument(i) * lenInv * w + x,
                normalizedChannel->value(i) * h + y,
                z
                ));
        }
    } else {
        // rezerwujemy odpowiednią ilość miejsca; niekonieczne, ale powinno zmniejszyć liczbę alokacji
        vertices->reserve( maxVertices*2 );
        vertices->resize(0);

        // o ile będziemy się przesuwać
        const time_type delta = normalizedChannel->getLength() / maxVertices;
        // przedział czasu w ramach którego się przesuwamy
        time_type timeStart = 0;
        time_type timeEnd = delta;
        // interatory
        auto i = 0;
        auto last = normalizedChannel->size();
        // początkowe maksimum i minimum w bieżącym przedziale
        point_type nextMaxValue = 0;
        point_type nextMinValue = 1;
        // wierzchołki
        osg::Vec3 lineStart(0, 0, z);
        osg::Vec3 lineStop(0, 0, z);

        // pętla obliczająca minimum i maksimum w każdym z przedziałów oraz dodająca linie pionowe
        // o długości równej amplitudzie
        while ( i != last ) {
            point_type minValue = nextMinValue;
            point_type maxValue = nextMaxValue;
            // wyznaczenie maksimum i minimum z punktów leżących w całości w danym przedziale
            while ( i != last && normalizedChannel->argument(i) < timeEnd ) {
                maxValue = std::max(maxValue, normalizedChannel->value(i));
                minValue = std::min(minValue, normalizedChannel->value(i));
                ++i;
            }
            // może prawa skrajna wartość jest większa?
            if ( i != last ) {
                point_type interpolated = accessor->getValue( std::min(normalizedChannel->getLength(), timeEnd) );
                maxValue = std::max(maxValue, interpolated);
                minValue = std::min(minValue, interpolated);
                // uwaga: NIE zerujemy max/min, żeby kolejny przedział również zaczynać od zinterpolowanych
                // wartości
                nextMaxValue = nextMinValue = interpolated;
            } else {
                nextMaxValue = 0;
                nextMinValue = 1;
            }
            // zamiana ze skali znormalizowanej na faktyczne wartości
            lineStop.x() = lineStart.x() = timeStart * lenInv * w + x;
            lineStart.y() = minValue * h + y;
            lineStop.y() = maxValue * h + y;
            if ( lineStart.y() == lineStop.y() ) {
                lineStop.y() += 1;
            }
            vertices->push_back(lineStart);
            vertices->push_back(lineStop);
            // następny przedział
            timeStart = timeEnd;
            timeEnd += delta;
        }
    }

    primitiveSet = new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size());

    colors->at(0) = color;
    setColorArray(colors);
    setVertexArray(vertices);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    
   

    if(active == true){
        linewidth->setWidth(2.0f);
    }else{
        linewidth->setWidth(1.0f);
    }

    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    //stateset->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
    //osg::BlendFunc* blend = new osg::BlendFunc();
    //blend->setSourceAlpha(GL_CONSTANT_ALPHA);
    //blend->setDestinationAlpha(GL_ONE_MINUS_SRC_ALPHA);
    //
    //setUseDisplayList(false);
    ////stateset->setAttributeAndModes(blend, osg::StateAttribute::ON);
    //stateset->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON );
    //stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    setStateSet(stateset);

    if ( getNumPrimitiveSets() ) {
        setPrimitiveSet(0, primitiveSet);
    } else {
        addPrimitiveSet(primitiveSet);
    }
}

std::pair<float, float> LineChartSerie::getXRange() const
{
    if ( !normalizedChannel ) {
        throw std::runtime_error("Data not set.");
    }
    // TODO: dodać offsetowanie!
    return std::make_pair<float, float>( 0, normalizedChannel->getLength() );
}

std::pair<float, float> LineChartSerie::getYRange() const
{
    if ( !normalizedChannel ) {
        throw std::runtime_error("Data not set.");
    }
    return std::make_pair<float, float>( stats->minValue(), stats->maxValue() );
}

const std::string& LineChartSerie::getXUnit() const
{
    if ( !channel ) {
        throw std::runtime_error("Data not set.");
    }
    return channel->getTimeBaseUnit();
}

const std::string& LineChartSerie::getYUnit() const
{
    if ( !channel ) {
        throw std::runtime_error("Data not set.");
    }
    return channel->getValueBaseUnit();
}

float LineChartSerie::getValue() const
{
    if ( !channel ) {
        throw std::runtime_error("Data not set.");
    }
    return currentValue->getCurrentValue();
}

float LineChartSerie::getTime() const
{
    if ( !channel ) {
        throw std::runtime_error("Data not set.");
    }
    return timer->getTime();
}

bool LineChartSerie::isActive() const
{
    return active;
}

void LineChartSerie::setActive(bool active)
{
    this->active = active;
    tryRefresh();
}
