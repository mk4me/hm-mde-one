#include "ChartPCH.h"
#include <osg/LineStipple>
#include <boost/foreach.hpp>
#include <math.h>
#include <iomanip>
#include "Chart.h"
#include "LineChartSerie.h"
#include "ChartSerie.h"

#undef  min
#undef  max

using namespace core;

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;    
}

Chart::Chart() : 
margin(0.0f),
showGridX(false), showGridY(false), showAxisX(true), showAxisY(true),
z(0), zRange(0),
x(0), y(0), w(0), h(0),
dashWidth(5), activeSerieIndex(-1), 
normalized(true), dirty(false)
{
    autoRefresh = false;

    geode = new osg::Geode();
    geode->setName("geode");
    addChild(geode);

    textPrototype = createText( osg::Vec3(0, 0, 0), 12, "prototype");
    setGridDensity(10);


    refreshAll();
    autoRefresh = true;
}

Chart::~Chart()
{
    
}

bool Chart::addSerie( const ChartSeriePtr& serie )
{
    if ( geode->containsDrawable(serie) ) {
        UTILS_FAIL("Kana³ ju¿ jest dodany.");
        return false;
    } else {
        if ( activeSerieIndex < 0 ) {
            activeSerieIndex = 0;
        }
        // dodajemy do listy i do dzieci
        series.push_back(serie);
        geode->addDrawable(serie);
        setDirty();
        return true;
    }
}


bool Chart::removeSerie( const ChartSeriePtr& serie )
{
    auto it = std::find(series.begin(), series.end(), serie);

    bool ret = false;

    if(it != series.end()){
        ret = true;

        if(activeSerieIndex >= 0 && series[activeSerieIndex] == serie){
            if(series.size() > 1){
                if(activeSerieIndex-1 >=0){
                    setActiveSerie(activeSerieIndex-1);
                }else{
                    setActiveSerie(0);
                }
            }else{
                setActiveSerie(-1);
            }
        }

        geode->removeDrawable(serie);
        setDirty();
    }

    return ret;
}

void Chart::removeAllSeries()
{
    BOOST_FOREACH(ChartSeriePtr chart, series) {
        geode->removeDrawable(chart);
    }
    series.clear();
    activeSerieIndex = -1;
    setDirty();
}

int Chart::getNumSeries() const
{
    return static_cast<int>(series.size());
}

int Chart::getActiveSerieIndex() const
{
    return activeSerieIndex;
}

void Chart::setActiveSerie( int currentSerie )
{
    this->activeSerieIndex = currentSerie;
    setDirty();
}

const ChartSerie* Chart::getActiveSerie() const
{
    if ( activeSerieIndex < 0 ) {
        return nullptr;
    } else {
        return series[activeSerieIndex];
    }
}

void Chart::setAutoRefresh( bool autoRefresh )
{
    this->autoRefresh = autoRefresh;
    if ( autoRefresh && dirty ) {
        refreshAll();
    }
}

void Chart::setLocation(float x, float y, float width, float height)
{
    this->x=x;
    this->y=y;
    this->w=width;
    this->h=height;
    setDirty();
}

void Chart::setLocation( osg::Vec4 loc )
{
    setLocation(loc[0], loc[1], loc[2], loc[3]);
}

osg::Vec4 Chart::getLocation() const
{
    return osg::Vec4(x, y, w, h);
}

osg::Vec4 Chart::getClientLocation() const
{
    osg::Vec4 loc;
    getClientLocation(loc[0], loc[1], loc[2], loc[3]);
    return loc;
}

void Chart::getClientLocation( float& x, float& y, float& w, float& h ) const
{
    x = this->x + margin;
    y = this->y + margin; 
    w = std::max(this->w-margin*2, FLT_MIN);
    h = std::max(this->h-margin*2, FLT_MIN);
}

float Chart::getZ() const
{
    return z;
}

void Chart::setZ( float z )
{
    this->z = z;
    setDirty();
}

float Chart::getZRange() const
{
    return zRange;
}

void Chart::setZRange( float zRange )
{
    this->zRange = zRange;
    setDirty();
}

bool Chart::isShowingGridX() const
{
    return showGridX;
}

void Chart::setShowGridX( bool showGridX )
{
    this->showGridX = showGridX;
    setDirty();
}

bool Chart::isShowingGridY() const
{
    return showGridY;
}

void Chart::setShowGridY( bool showGridY )
{
    this->showGridY = showGridY;
    setDirty();
}

bool Chart::isShowingAxisX() const
{
    return showAxisX;
}

void Chart::setShowAxisX( bool showAxisX )
{
    this->showAxisX = showAxisX;
    setDirty();
}

bool Chart::isShowingAxisY() const
{
    return showAxisY;
}

void Chart::setShowAxisY( bool showAxisY )
{
    this->showAxisY = showAxisY;
    setDirty();
}

float Chart::getGridDashWidth() const
{
    return dashWidth;
}

void Chart::setGridDashWidth( float dashWidth )
{
    this->dashWidth = dashWidth;
    setDirty();
}

float Chart::getMargin() const
{
    return margin;
}

void Chart::setMargin(float margin)
{
    this->margin=margin;
    setDirty();
}

int Chart::getGridDensity()
{
    return gridDensity;
}

void Chart::setGridDensity(int gridDensity)
{
    UTILS_ASSERT(gridDensity > 0);
    this->gridDensity=gridDensity;
    // +1 poniewa¿ dodawane na ekstremum
    labelsY.resize( gridDensity + 1 );
    labelsX.resize( gridDensity + 1 );
    setDirty();
}

osg::Vec4 Chart::getGridColor()
{
    return gridColor;
}

void Chart::setGridColor(osg::Vec4 gridColor)
{
    this->gridColor=gridColor;
    setDirty();
}

osg::Vec4 Chart::getAxisColor()
{
    return axisColor;
}

void Chart::setAxisColor(osg::Vec4 color)
{
    this->axisColor=color;
    setDirty();
}

bool Chart::isNormalized() const
{
    return normalized;
}

void Chart::setNormalized( bool normalized )
{
    this->normalized = normalized;
    setDirty();
}

void Chart::setLabelPrototype( osgText::Text* prototype )
{
    if (prototype) {
        textPrototype = prototype;
        labelsX.clear();
        labelsY.clear();
        setDirty();
    } else {
        UTILS_FAIL("Null");
    }
}

void Chart::refreshCursor()
{
    float zStep = zRange/float(2 + series.size());
    refreshCursor(z + zRange - zStep);
}

void Chart::refreshAll()
{
    float x, y, w, h;
    getClientLocation(x, y, w, h);

    // odstêp miedzy elementami;
    // 2 podane jawnie, bo trzeba zarezerwowaæ wspó³rzêdne dla t³a i osiek
    float zStep = zRange/float(2 + series.size());
    float currentZ = z;

    refreshGrid(currentZ);
    currentZ += zStep;

    if ( normalized ) {
        BOOST_FOREACH( const ChartSeriePtr& serie, series ) {
            serie->setLocation( x, y, w, h );
            serie->setZ(currentZ);
            serie->refresh();
            currentZ += zStep;
        }
    } else {

        bool shareZero = true;

        // dzielenie po jednostkach
        std::multimap<std::string, ChartSeriePtr> byUnits;
        // TODO: dodaæ skalowanie dla OX!
        std::map<std::string, std::pair<float, float> > unitInfo;

        BOOST_FOREACH( const ChartSeriePtr& serie, series ) {
            // pobranie informacji o osi OY
            std::pair<float, float> yRange = serie->getYRange();
            UTILS_ASSERT( yRange.first <= yRange.second );
            const std::string & yUnit = serie->getYUnit();
            // uzupe³nienie min/max zakresu danej jednostki
            auto found = unitInfo.find(yUnit);
            if ( found == unitInfo.end() ) {
                unitInfo[yUnit] = yRange;
            } else {
                found->second.first = std::min(yRange.first, found->second.first);
                found->second.second = std::max(yRange.second, found->second.second);
            }
            // uzupe³nienie podzia³u po jednostkach
            byUnits.insert(std::make_pair(serie->getYUnit(), serie));
        }

        if ( shareZero ) {
            // uwspólnienie polega na przeskalowaniu zakresów tak, ¿eby wzglêdna odleg³oœæ min/max od zera
            // by³a jednakowa
            float maxNegRatio = 0.0f;
            float maxPosRatio = 0.0f;
            for ( auto it = unitInfo.begin(), last = unitInfo.end(); it != last; ++it ) {
                // rozszerzamy do zera
                std::pair<float, float>& range = it->second;
                if ( range.second < 0 ) {
                    range.second = 0;
                } else if ( range.first > 0 ) {
                    range.first = 0;
                }
                // obliczamy zakres
                float diff = range.second - range.first;
                if ( diff != 0.0f ) {
                    maxNegRatio = std::max(maxNegRatio, fabs(range.first) / diff );
                    maxPosRatio = std::max(maxPosRatio, fabs(range.second) / diff );
                }
            }
            // normalizujemy zakres - w takim stosunku do zakresu musz¹ byæ min i max
            float maxDiff = maxPosRatio + maxNegRatio;
            if ( maxDiff != 0.0f ) {
                float maxDiffInv = 1.0f / maxDiff;
                maxPosRatio = maxPosRatio * maxDiffInv;
                maxNegRatio = maxNegRatio * maxDiffInv;
                // pomocnicze zmienne pozwalaj¹ce unikn¹æ wielokrotnego dzielenia
                float scaleMinToMaxFactor = maxPosRatio / maxNegRatio;
                float scaleMaxToMinFactor = 1.0f / scaleMinToMaxFactor;
                // skalujemy wszystkie zakresy tak, aby stosunek max i min do zakresu by³ taki jak wyliczone zbiorcze
                for ( auto it = unitInfo.begin(), last = unitInfo.end(); it != last; ++it ) {
                    std::pair<float, float>& range = it->second;
                    float diff = range.second - range.first;
                    if ( diff != 0.0f ) {
                        float negRatio = fabs(range.first) / diff;
                        float posRatio = fabs(range.second) / diff;
                        if ( maxNegRatio == 0.0f ) {
                            // nie ma co skalowaæ
                            UTILS_ASSERT(range.first == 0.0f);
                        } else if ( maxPosRatio == 0.0f ) {
                            // nie ma co skalowaæ
                            UTILS_ASSERT(range.second == 0.0f);
                        } else if ( negRatio > maxNegRatio ) {
                            // rozci¹gamy górny zakres
                            range.second = scaleMinToMaxFactor * fabs(range.first);
                        } else {
                            // rozci¹gamy dolny zakres
                            range.first = -scaleMaxToMinFactor * fabs(range.second);
                        }
                    } else {
                        // poniewa¿ funkcja jest niezmienna wporawdzamy rozszerzamy sztucznie zakres
                        range.first -= maxNegRatio;
                        range.second += maxPosRatio;
                    }
                }
            } else {
                // poniewa¿ wszystkie funkcje s¹ sta³e wprowadzamy sztuczny zakres
                for ( auto it = unitInfo.begin(), last = unitInfo.end(); it != last; ++it ) {
                    std::pair<float, float>& range = it->second;
                    // poniewa¿ funkcja jest niezmienna wporawdzamy rozszerzamy sztucznie zakres
                    range.first -= 1.0f;
                    range.second += 1.0f;
                }
            }
        }

        for ( auto unit = unitInfo.begin(); unit != unitInfo.end(); ++unit ) {
            
            auto first = byUnits.lower_bound(unit->first);
            auto last = byUnits.upper_bound(unit->first);

            // okreœlamy minimum i maksimum z danego zakresu
            float xMin = FLT_MAX;
            float xMax = -FLT_MAX;
            float yMin = unit->second.first;
            float yMax = unit->second.second;
            for ( auto it = first; it != last; ++it ) {
                std::pair<float, float> xRange = it->second->getXRange();
                std::pair<float, float> yRange = it->second->getYRange();
                xMin = std::min(xRange.first, xMin);
                xMax = std::max(xRange.second, xMax);
            }
            
            // teraz mo¿emy pozycjonowaæ
            for ( auto it = first; it != last; ++it ) {
                const ChartSeriePtr& serie = it->second;
                std::pair<float, float> xRange = serie->getXRange();
                std::pair<float, float> yRange = serie->getYRange();

                serie->setLocation( 
                    lerp(x, x+w, (xRange.first - xMin)/(xMax - xMin)),
                    lerp(y, y+h, (yRange.first - yMin)/(yMax - yMin)),
                    w * (xRange.second - xRange.first)/(xMax - xMin),
                    h * (yRange.second - yRange.first)/(yMax - yMin)    
                );

                serie->setZ(currentZ);
                serie->refresh();
                currentZ += zStep;
            }
        } 
    }

    refreshAxis(currentZ);
    refreshLabels(currentZ);
    refreshCursor(currentZ);

    dirty = false;
}

osg::ref_ptr<osgText::Text> Chart::createText( const osg::Vec3& pos, float size, const std::string& label )
{
    osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");
    osg::ref_ptr<osgText::Text> text = new osgText::Text();

    text->setColor(osg::Vec4(1, 1, 1, 1));
    text->setFontResolution(100,100);
    text->setPosition(pos);
    text->setFont(font);
    text->setCharacterSize(size);
    text->setAxisAlignment(osgText::Text::SCREEN); 
    text->setAlignment(osgText::Text::CENTER_TOP);
    text->setText(label);
    text->setLayout(osgText::Text::LEFT_TO_RIGHT);

    return text;    
}

void Chart::formatStream(std::ostringstream& stream, float min, float max, int decimalsToShow /*= 2*/ )
{
    float diff = fabs(max - min);
    // rz¹d ró¿nicy miêdzy max i min (ujemny dla diff < 1)
    int logdiff = static_cast<int>(floorf(log10f(diff)));
    decimalsToShow = std::max( decimalsToShow - logdiff, 0 );
    stream << std::setiosflags(std::ios::fixed) << std::setprecision(decimalsToShow);
}


void Chart::setDirty( bool dirty /*= true*/ )
{
    if ( dirty ) {
        if ( autoRefresh ) {
            refreshAll();
        } else {
            this->dirty = true;
        }
    } else {
        this->dirty = dirty;
    }
}

bool Chart::prepareGeometry( osg::GeometryPtr& geom, bool condition, const char* name )
{
    // czy w ogóle jest coœ do pkazania?
    if ( condition ) {
        if ( geom && geode->getDrawableIndex(geom) ) {
            // trzeba usun¹æ!
            geode->removeDrawable(geom);
        }
        geom = nullptr;
        return false;
    } else {
        // czy tworzymy od nowa?
        if ( !geom ) {
            geom = new osg::Geometry();
            geom->setVertexArray( new osg::Vec3Array );
            geom->setColorArray( new osg::Vec4Array );
            geom->setDataVariance( osg::Object::DYNAMIC );
            geom->addPrimitiveSet( new osg::DrawArrays(GL_LINES, 0, 0) );
            geom->setName(name);
            geode->addDrawable(geom);
        }
        return true;
    }
}
bool Chart::prepareText( osgText::TextPtr& text, bool condition, const char* name )
{
    // czy w ogóle jest coœ do pkazania?
    if ( condition ) {
        if ( text && geode->getDrawableIndex(text) ) {
            // trzeba usun¹æ!
            geode->removeDrawable(text);
        }
        text = nullptr;
        return false;
    } else {
        // czy tworzymy od nowa?
        if ( !text ) {
            text = osg::clone( textPrototype.get(), name, osg::CopyOp::SHALLOW_COPY );
            geode->addDrawable(text);
        }
        return true;
    }
}

void Chart::refreshGrid(float z)
{
    // czy w ogóle jest coœ do pkazania?
    if ( prepareGeometry(grid, !showGridX && !showGridY, "grid" ) ) {

        if ( !grid->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINESTIPPLE) ) {
            grid->getStateSet()->setAttributeAndModes(new osg::LineStipple(2, 0xAAAA));
        }

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        // wierzcho³ki
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(grid->getVertexArray());
        vertices->resize(0);
        for ( float dy = 0.0f, step = h / gridDensity; showGridY && dy < h; dy += step  ) {
            vertices->push_back(osg::Vec3(x,        y+dy,    z));
            vertices->push_back(osg::Vec3(x+w,  y+dy,    z));
        }
        for ( float dx = 0.0f, step = w / gridDensity; showGridY && dx < w; dx += step ) {
            vertices->push_back(osg::Vec3(x+dx,      y,          z));
            vertices->push_back(osg::Vec3(x+dx,      y+h,   z));
        }

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(grid->getColorArray());
        colors->resize(1);
        colors->at(0) = gridColor;
        grid->setColorBinding(osg::Geometry::BIND_OVERALL);

        // ustawienie prymitywów
        osg::DrawArrays* primitives = dynamic_cast<osg::DrawArrays*>(grid->getPrimitiveSet(0));
        primitives->set(GL_LINES, 0, vertices->getNumElements());

        // "zabrudzenie" geometrii
        grid->setVertexArray(vertices);
    }
}

void Chart::refreshAxis(float z)
{
    using namespace osg;
    // czy w ogóle jest coœ do pkazania?
    if ( prepareGeometry(axises, !showAxisX && !showAxisY, "axises" ) ) {

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        // wierzcho³ki
        Vec3Array* vertices = dynamic_cast<Vec3Array*>(axises->getVertexArray());
        vertices->resize(0);
        if (showAxisY) {
            // linia
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+h, z));
            // koñcówki
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+dashWidth, y, z));
            vertices->push_back(Vec3(x, y+h, z));
            vertices->push_back(Vec3(x+dashWidth, y+h, z));

        }
        if (showAxisX) {
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+w, y, z));
            // koñcówki
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+dashWidth, z));
            vertices->push_back(Vec3(x+w, y, z));
            vertices->push_back(Vec3(x+w, y+dashWidth, z));
        }

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(axises->getColorArray());
        colors->resize(1);
        colors->at(0) = Vec4(1,1,1,1);
        axises->setColorBinding(osg::Geometry::BIND_OVERALL);

        // ustawienie prymitywów
        osg::DrawArrays* primitives = dynamic_cast<osg::DrawArrays*>(axises->getPrimitiveSet(0));
        primitives->set(GL_LINES, 0, vertices->getNumElements());

        // "zabrudzenie" geometrii
        axises->setVertexArray(vertices);
    }
}

void Chart::refreshLabels( float z )
{
    const float offset = 8;

    ChartSeriePtr activeSerie = this->activeSerieIndex < 0 ? nullptr : series[this->activeSerieIndex];
    if ( activeSerie ) {

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        auto xrange = activeSerie->getXRange();

        refreshLabels(labelsX, activeSerie->getXUnit(), !showGridX, xrange.first, xrange.second, x , x + w, y + offset, y + offset, z, osgText::Text::CENTER_BOTTOM);

        osg::Vec2 pos = activeSerie->getPosition();
        osg::Vec2 size = activeSerie->getSize();
        auto yrange = activeSerie->getYRange();

        // znormalizowane wspó³rzêdne
        float ny = (pos.y() - y) / h;
        float nh = size.y() / h;
        // prze³o¿enie znormalizowanych wspó³rzêdnych na zakres jednostek
        float range = (yrange.second - yrange.first) / nh;
        float rangeMin = yrange.first - ny * range;
        // odœwie¿amy
        refreshLabels(labelsY, activeSerie->getYUnit(), !showGridY, rangeMin, rangeMin + range, x + offset, x + offset, y, y + h, z, osgText::Text::LEFT_CENTER);
    } else {
        BOOST_FOREACH(osgText::TextPtr& text, labelsX) {
            prepareText(text, true, "");
        }
        BOOST_FOREACH(osgText::TextPtr& text, labelsY) {
            prepareText(text, true, "");
        }
    }
}

void Chart::refreshCursor( float z )
{
    using namespace osg;

    const float xOffset = -5;
    const float yOffset = 0;
    // tekst...
    if ( prepareText(cursorText, activeSerieIndex < 0, "cursorText") && prepareGeometry(cursor, activeSerieIndex < 0, "cursor")) {
        // okreœlamy miejsce gdzie bêdziemy rysowaæ tekst
        ChartSeriePtr serie = series[activeSerieIndex];
        float x = serie->getPosition().x();
        float y = serie->getPosition().y();
        float w = serie->getSize().x();
        float h = serie->getSize().y();

        // trzeba jeszcze okreœliæ wartoœæ w bie¿¹cym punkcie czasowym
        auto yrange = serie->getYRange();
        auto xrange = serie->getXRange();
        float value = serie->getValue();
        float time = serie->getTime();

        // znormalizowana wartoœæ, znormalizowany czas
        float nvalue = (value - yrange.first) / (yrange.second - yrange.first);
        float ntime = (time - xrange.first) / (xrange.second - xrange.first);

        float cursorX = lerp(x, x+w, ntime);
        float cursorY = lerp(y, y+h, nvalue);

        // teraz mo¿na pozycjonowaæ...
        cursorText->setPosition( Vec3(cursorX + xOffset, cursorY + yOffset, z) );
        // no i nadaæ etykietê
        std::ostringstream buffer;
        formatStream(buffer, yrange.first, yrange.second);
        buffer << value;
        cursorText->setText( buffer.str() );
        cursorText->setAlignment( osgText::Text::RIGHT_CENTER );

        // przygotowanie kursora
        Vec3Array* vertices = dynamic_cast<Vec3Array*>(cursor->getVertexArray());
        vertices->resize(0);

        auto loc = getClientLocation();
        vertices->push_back( Vec3(cursorX, loc.y(), z) );
        vertices->push_back( Vec3(cursorX, loc.y() + loc.w(), z) );

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(cursor->getColorArray());
        colors->resize(1);
        colors->at(0) = Vec4(1,1,1,0.33f);
        cursor->setColorBinding(osg::Geometry::BIND_OVERALL);

        // ustawienie prymitywów
        osg::DrawArrays* primitives = dynamic_cast<osg::DrawArrays*>(cursor->getPrimitiveSet(0));
        primitives->set(GL_LINE_STRIP, 0, vertices->getNumElements());

        // "zabrudzenie" geometrii
        cursor->setVertexArray(vertices);

    }
}

void Chart::refreshLabels(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max, float x0, float x1, float y0, float y1, float z, osgText::Text::AlignmentType align )
{
    UTILS_ASSERT(labels.size() > 1);

    float t = 0;
    float dt = 1.0f / (labels.size() - 1);
    std::ostringstream buffer;
    if ( !condition ) {
        formatStream(buffer, min, max);
    }
    for (size_t i = 0; i < labels.size(); ++i, t += dt) {
        osgText::TextPtr& text = labels[i];
        if ( prepareText(text, condition, "label")  ) {

            // konwersja na str
            float val = lerp(min, max, t);
            buffer.str( val < 0 ? "" : " ");
            buffer << val << unit;
            text->setText( buffer.str() );

            // ustawienie pozycji
            text->setPosition( osg::Vec3(lerp(x0, x1, t), lerp(y0, y1, t) , z) );
            text->setAlignment(align);
        }
    }
}

