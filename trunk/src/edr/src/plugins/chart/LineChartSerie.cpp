#include "ChartPCH.h"
#include <core/ILog.h>
#include <osg/BlendFunc>
#include "LineChartSerie.h"
#include <boost/foreach.hpp>

#undef min
#undef max


LineChartSerie::LineChartSerie() :
vertices(new osg::Vec3Array)
{
    setDataVariance( osg::Object::DYNAMIC );

    colors = new osg::Vec4Array(1);
    colors->at(0) = osg::Vec4(1,1,1,1);

    setColorArray(colors);
    setColorBinding(osg::Geometry::BIND_OVERALL);
}

void LineChartSerie::setData( const ScalarChannelConstPtr& channel )
{
    this->channel = channel;

    ScalarChannelPtr nonConstChannel(core::const_pointer_cast<ScalarChannel>(channel));

    this->normalizedChannel.reset(utils::AutoModifier::createAutoModChannel(*nonConstChannel, ScalarChannelNormalizer()));
    timer = nonConstChannel->createTimer();
    stats.reset(new ScalarChannelStats(nonConstChannel));
    tryRefresh();
}

void LineChartSerie::refresh()
{
    UTILS_ASSERT(normalizedChannel, "No data set!");
    UTILS_ASSERT(w > 0 && h >= 0, "Wrong size set!");

    typedef ScalarChannel::point_type point_type;
    typedef ScalarChannel::time_type time_type;

    // dodajemy wierzcho�ki tylko tak, aby ich g�sto�� wynosi�a maksymalnie verticesPerUnit per piksel
    // okre�lenie maksymalnej liczby wierzcho�k�w
    size_t maxVertices = std::min<size_t>( normalizedChannel->size(), static_cast<size_t>(ceil(w * verticesPerUnit)) );
    
    // odrwacamy wykres (koordynaty OGL)
    time_type lenInv = 1 / normalizedChannel->getLength();

    osg::ref_ptr<osg::DrawArrays> primitiveSet;

    if ( maxVertices == normalizedChannel->size() ) {
        // rezerwujemy odpowiedni� ilo�� miejsca; niekonieczne, ale powinno zmniejszy� liczb� alokacji
        vertices->reserve( maxVertices );
        vertices->resize(0);
        // nie ma co interpolowa�, po prostu dodajemy
        for(auto it = normalizedChannel->begin(); it != normalizedChannel->end(); it++) {
            vertices->push_back(osg::Vec3(
                it->first * lenInv * w + x,
                it->second * h + y,
                z
                ));
        }
        primitiveSet = new osg::DrawArrays(GL_LINE_STRIP,0,vertices->size());
    } else {
        // rezerwujemy odpowiedni� ilo�� miejsca; niekonieczne, ale powinno zmniejszy� liczb� alokacji
        vertices->reserve( maxVertices*2 );
        vertices->resize(0);

        // o ile b�dziemy si� przesuwa�
        const time_type delta = normalizedChannel->getLength() / maxVertices;
        // przedzia� czasu w ramach kt�rego si� przesuwamy
        time_type timeStart = 0;
        time_type timeEnd = delta;
        // interatory
        ScalarChannel::const_iterator it = normalizedChannel->begin();
        const ScalarChannel::const_iterator last = normalizedChannel->end();
        // pocz�tkowe maksimum i minimum w bie��cym przedziale
        point_type nextMaxValue = 0;
        point_type nextMinValue = 1;
        // wierzcho�ki
        osg::Vec3 lineStart(0, 0, z);
        osg::Vec3 lineStop(0, 0, z);

        // p�tla obliczaj�ca minimum i maksimum w ka�dym z przedzia��w oraz dodaj�ca linie pionowe
        // o d�ugo�ci r�wnej amplitudzie
        while ( it != last ) {
            point_type minValue = nextMinValue;
            point_type maxValue = nextMaxValue;
            // wyznaczenie maksimum i minimum z punkt�w le��cych w ca�o�ci w danym przedziale
            while ( it != last && (*it).first < timeEnd ) {
                maxValue = std::max(maxValue, (*it).second);
                minValue = std::min(minValue, (*it).second);
                ++it;
            }
            // mo�e prawa skrajna warto�� jest wi�ksza?
            if ( it != last ) {
                point_type interpolated = normalizedChannel->getValue( std::min(normalizedChannel->getLength(), timeEnd) );
                maxValue = std::max(maxValue, interpolated);
                minValue = std::min(minValue, interpolated);
                // uwaga: NIE zerujemy max/min, �eby kolejny przedzia� r�wnie� zaczyna� od zinterpolowanych
                // warto�ci
                nextMaxValue = nextMinValue = interpolated;
            } else {
                nextMaxValue = 0;
                nextMinValue = 1;
            }
            // zamiana ze skali znormalizowanej na faktyczne warto�ci
            lineStop.x() = lineStart.x() = timeStart * lenInv * w + x;
            lineStart.y() = minValue * h + y;
            lineStop.y() = maxValue * h + y;
            if ( lineStart.y() == lineStop.y() ) {
                lineStop.y() += 1;
            }
            vertices->push_back(lineStart);
            vertices->push_back(lineStop);
            // nast�pny przedzia�
            timeStart = timeEnd;
            timeEnd += delta;
        }

        primitiveSet = new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size());
    }

    colors->at(0) = color;
    setColorArray(colors);
    setVertexArray(vertices);

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
    // TODO: doda� offsetowanie!
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
    return timer->getCurrentValue();
}

float LineChartSerie::getTime() const
{
    if ( !channel ) {
        throw std::runtime_error("Data not set.");
    }
    return timer->getTime();
}