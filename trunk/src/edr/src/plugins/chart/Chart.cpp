#include "ChartPCH.h"
#include <osg/LineStipple>
#include <boost/foreach.hpp>
#include <math.h>
#include <iomanip>
#include "Chart.h"
#include "LineChartSerie.h"
#include "ChartSerie.h"
#include <iterator>

#undef  min
#undef  max

using namespace core;

namespace osgText 
{
    float textWidth(const Text & text)
    {
        int c = 0;
        //split text according to line breaks
        auto prev = text.getText().begin();
        auto it = std::find(prev, text.getText().end(), '\n');
        while(it != text.getText().end()){
            c = std::max(c, std::distance(prev, it));
            prev = it;
            prev++;
            it = std::find(prev, text.getText().end(), '\n');
        }

        c = std::max(c, std::distance(prev, text.getText().end()));

        return (float)c * text.getCharacterHeight() / text.getCharacterAspectRatio();
    }

    float textHeight(const Text & text)
    {
        return text.getCharacterHeight() * (1 + std::count(text.getText().begin(), text.getText().end(), '\n'));
    }
} // namespace osgText

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;    
}

Chart::Chart() : 
margin(0.0f),
showGridX(true), showGridY(true), showAxisX(true), showAxisY(true),
showFrame(true), z(0), zRange(0), labelsToAxisesOffset(8), showXUnits(true), showYUnits(true),
x(0), y(0), w(0), h(0), frameWidth(2), axisXInside(true), axisYInside(true),
dashWidth(8), activeSerieIndex(-1), showTitle(true), xUnitsSeparated(true), yUnitsSeparated(true),
normalized(true), dirty(false), titlePosition(TOP_GRID_CENTER),
frameColor(osg::Vec4(1,1,1,1)), axisesColor(osg::Vec4(1,1,1,1)),
cursorColor(osg::Vec4(1,1,1,1)), titleVisible(false), xUnitLabelVisible(false), yUnitLabelVisible(false)
{
    autoRefresh = false;

    geode = new osg::Geode();
    geode->setName("geode");
    addChild(geode);

    textPrototype = createText( osg::Vec3(0, 0, 0), 12, "prototype");
    xAxisUnitsLabel = createText( osg::Vec3(0, 0, 0), 12, "xAxisUnits");
    xAxisUnitsLabel->setText("");
    xAxisUnitsLabel->setAlignment(osgText::TextBase::CENTER_BOTTOM);
    yAxisUnitsLabel = createText( osg::Vec3(0, 0, 0), 12, "yAxisUnits");
    yAxisUnitsLabel->setText("");
    yAxisUnitsLabel->setAlignment(osgText::TextBase::RIGHT_CENTER);
    titleLabel = createText( osg::Vec3(0, 0, 0), 30, "title");
    titleLabel->setText("");
    titleLabel->setAlignment(osgText::TextBase::CENTER_BOTTOM);

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
        /*if ( activeSerieIndex < 0 ) {
            activeSerieIndex = 0;
        }*/
        activeSerieIndex++;
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

        if(activeSerieIndex >= 0){
            if(series[activeSerieIndex] == serie){
                if(series.size() > 1){
                    if(activeSerieIndex - 1 >= 0){
                        activeSerieIndex--;
                    }else{
                        activeSerieIndex = 0;
                    }
                }else{
                    activeSerieIndex = -1;
                }
            }else{
                int idx = std::distance(series.begin(), it);
                if(idx < activeSerieIndex){
                    activeSerieIndex--;
                }
            }
        }

        geode->removeDrawable(serie);
        series.erase(it);
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

float Chart::getTextPrototypeHeight() const
{
    if(textPrototype == nullptr){
        return 0;
    }

    return textPrototype->getCharacterHeight();
}

void Chart::repositionAll()
{
    float x, y, w, h;
    getClientLocation(x,y,w,h);

    float hDelta = 0;
    float xDelta = 0;
    float yDelta = 0;
    float wDelta = 0;

    if(isShowingAxisY() == true){
        hDelta = std::max(hDelta, getTextPrototypeHeight());
        yDelta = std::max(yDelta, getTextPrototypeHeight() / 2.0f);
        xDelta = std::max(xDelta, getYAxisWidth());
    }

    if(isShowingAxisX() == true){
        if(labelsX.empty() == false){
            if(labelsX.front() != nullptr){
                xDelta = std::max(xDelta, osgText::textWidth(*(labelsX.front())) / 2.0f);
            }

            if(labelsX.back() != nullptr){
                wDelta = std::max(wDelta, osgText::textWidth(*(labelsX.back())) / 2.0f);
            }
        }

        yDelta = std::max(yDelta, getXAxisHeight());
    }

    gridPosition.x() = x + xDelta;

    // musimy te¿ uwzglêdniæ prawy opis wartoœci na osi X
    gridPosition.w() = w - (wDelta + xDelta);

    gridPosition.y() = y + yDelta;

    // musimy tez uwzglêdniæ wysokoœc tekstu górnej wartoœci na osi Y

    gridPosition.z() = h - (hDelta + yDelta);

    if(titleVisible == true){
        float totalHeight = osgText::textHeight(*titleLabel) + labelsToAxisesOffset;

        switch(titlePosition)
        {
        case BOTTOM_CHART_CENTER:
        case BOTTOM_GRID_CENTER:
            gridPosition.y() += totalHeight;
            gridPosition.z() -= totalHeight;
            break;

        case TOP_CHART_CENTER:
        case TOP_GRID_CENTER:
            gridPosition.z() -= totalHeight;
            break;
        }
    }
}

//! Przed wywo³aniem tej metody labelki powinny byæ zaktualizowane!!
void Chart::getGridLocation(float& x, float& y, float& w, float& h) const
{
    //getClientLocation(x,y,w,h);

    //float hDelta = 0;
    //float xDelta = 0;
    //float yDelta = 0;
    //float wDelta = 0;

    //if(isShowingAxisY() == true){
    //    hDelta = std::max(hDelta, getTextPrototypeHeight() / 2.0f);

    //    if(isAxisYInside() == false){
    //        //pobierz teoretyczne wymiary i odejmij od ca³ego okna
    //        xDelta = std::max(xDelta, getYAxisWidth());
    //    }
    //}

    //if(isShowingAxisX() == true){
    //    if(labelsX.empty() == false){
    //        if(labelsX.front() != nullptr){
    //            xDelta = std::max(xDelta, osgText::textWidth(*(labelsX.front())) / 2.0f);
    //        }

    //        if(labelsX.back() != nullptr){
    //            wDelta = std::max(wDelta, osgText::textWidth(*(labelsX.back())) / 2.0f);
    //        }
    //    }

    //    if(isAxisXInside() == false){
    //        yDelta = std::max(yDelta, getXAxisHeight());
    //    }
    //}

    //if(isShowingTitle() == true && getTitleText().empty() == false){

    //    float titleHeight = osgText::textHeight(*titleLabel) + labelsToAxisesOffset;

    //    auto pos = titleLabel->getPosition();

    //    switch(titlePosition)
    //    {
    //        case BOTTOM_CHART_CENTER:
    //        case BOTTOM_GRID_CENTER:
    //                                    yDelta += titleHeight;
    //                                    pos.y() = y;

    //                                    break;

    //        case TOP_CHART_CENTER:
    //        case TOP_GRID_CENTER:
    //                                    hDelta += titleHeight;
    //                                    pos.y() = y + h - osgText::textHeight(*titleLabel);
    //                                    break;
    //    }
    //    
    //    titleLabel->setPosition(pos);
    //}

    //x += xDelta;

    //// musimy te¿ uwzglêdniæ prawy opis wartoœci na osi X
    //w -= (wDelta + xDelta);

    //y += yDelta;

    //// musimy tez uwzglêdniæ wysokoœc tekstu górnej wartoœci na osi Y

    //h -= (hDelta + yDelta);
    x = gridPosition.x();
    y = gridPosition.y();
    w = gridPosition.w();
    h = gridPosition.z();
}

osg::Vec4 Chart::getGridLocation() const
{
    /*osg::Vec4 ret(0,0,0,0);
    getGridLocation(ret[0], ret[1], ret[2], ret[3]);
    return ret;*/
    return gridPosition;
}

float Chart::getXAxisHeight() const
{
    float ret = 0;

    if(isAxisXInside() == false){
        ret += dashWidth + labelsToAxisesOffset + getTextPrototypeHeight();
    }

    if(xUnitLabelVisible == true){
        ret += labelsToAxisesOffset + getTextPrototypeHeight();
    }

    return ret;
}

float Chart::getYAxisWidth() const
{
    float ret = 0;

    if(isAxisYInside() == false){
        ret += dashWidth + labelsToAxisesOffset + maxLabelsWidth(labelsY);
    }
    
    if(yUnitLabelVisible == true){
        ret += labelsToAxisesOffset + osgText::textWidth(*yAxisUnitsLabel);
    }

    return ret;
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


bool Chart::isShowingFrame() const
{
    return showFrame;
}

void Chart::setShowFrame(bool showFrame)
{
    this->showFrame = showFrame;
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

bool Chart::isAxisXInside() const
{
    return axisXInside;
}

void Chart::setAxisXInside(bool axisXInside)
{
    this->axisXInside = axisXInside;
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

bool Chart::isAxisYInside() const
{
    return axisYInside;
}

void Chart::setAxisYInside(bool axisYInside)
{
    this->axisYInside = axisYInside;
    setDirty();
}

float Chart::getFrameWidth() const
{
    return frameWidth;
}

void Chart::setFrameWidth(float frameWidth)
{
    this->frameWidth = frameWidth;
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

int Chart::getGridDensity() const
{
    return gridDensity;
}

void Chart::setGridDensity(int gridDensity)
{
    UTILS_ASSERT(gridDensity > 0);

    if(gridDensity < this->gridDensity){
        for(int i = gridDensity+1; i < this->gridDensity + 1; i++){
            geode->removeDrawable(labelsX[i]);
            geode->removeDrawable(labelsY[i]);
        }
    }

    this->gridDensity=gridDensity;
    // +1 poniewa¿ dodawane na ekstremum
    labelsY.resize( gridDensity + 1 );
    labelsX.resize( gridDensity + 1 );
    setDirty();
}

osg::Vec4 Chart::getFrameColor() const
{
    return frameColor;
}

void Chart::setFrameColor(osg::Vec4 frameColor)
{
    this->frameColor = frameColor;
    setDirty();
}

osg::Vec4 Chart::getGridColor() const
{
    return gridColor;
}

void Chart::setGridColor(osg::Vec4 gridColor)
{
    this->gridColor=gridColor;
    setDirty();
}

osg::Vec4 Chart::getAxisesColor() const
{
    return axisesColor;
}

void Chart::setAxisesColor(osg::Vec4 color)
{
    this->axisesColor=color;
    setDirty();
}

osg::Vec4 Chart::getCursorColor() const
{
    return cursorColor;
}

void Chart::setCursorColor(osg::Vec4 color)
{
    cursorColor = color;
    setDirty();
}


bool Chart::isShowingTitle() const
{
    return showTitle;
}

void Chart::setShowTitle(bool showTitle)
{
    this->showTitle = showTitle;
    setDirty();
}


std::string Chart::getTitleText() const
{
    return titleLabel->getText().createUTF8EncodedString();
}

void Chart::setTitleText(const std::string & titleText)
{
    titleLabel->setText(titleText);
    setDirty();
}

int Chart::getTitleTextSize() const
{
    return titleLabel->getCharacterHeight();
}

void Chart::setTitleTextSize(int titleTextSize)
{
    titleLabel->setCharacterSize(titleTextSize);
    setDirty();
}

Chart::TitlePosition Chart::getTitlePosition() const
{
    return titlePosition;
}

void Chart::setTitlePosition(TitlePosition position)
{
    this->titlePosition = position;
    setDirty();
}

bool Chart::isShowingXUnitsSeparately() const
{
    return xUnitsSeparated;
}
void Chart::setShowingXUnitsSeparately(bool unitsSeparate)
{
    this->xUnitsSeparated = unitsSeparate;
    setDirty();
}

bool Chart::isShowingYUnitsSeparately() const
{
    return yUnitsSeparated;
}
void Chart::setShowingYUnitsSeparately(bool unitsSeparate)
{
    this->yUnitsSeparated = unitsSeparate;
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

        for(int i = 0; i < labelsX.size(); i++){
            geode->removeDrawable(labelsX[i]);
            geode->removeDrawable(labelsY[i]);
        }

        labelsX.clear();
        labelsY.clear();

        labelsX.resize(gridDensity + 1);
        labelsY.resize(gridDensity + 1);

        geode->removeDrawable(xAxisUnitsLabel);
        geode->removeDrawable(yAxisUnitsLabel);
        geode->removeDrawable(titleLabel);
        geode->removeDrawable(cursorText);

        std::string tmp;

        tmp = xAxisUnitsLabel->getText().createUTF8EncodedString();
        xAxisUnitsLabel = osg::clone( textPrototype.get(), "label", osg::CopyOp::SHALLOW_COPY );
        xAxisUnitsLabel->setText(tmp);

        tmp = yAxisUnitsLabel->getText().createUTF8EncodedString();
        yAxisUnitsLabel = osg::clone( textPrototype.get(), "label", osg::CopyOp::SHALLOW_COPY );
        yAxisUnitsLabel->setText(tmp);

        tmp = titleLabel->getText().createUTF8EncodedString();
        int fontSize = titleLabel->getCharacterHeight();
        titleLabel = osg::clone( textPrototype.get(), "title", osg::CopyOp::SHALLOW_COPY );
        titleLabel->setText(tmp);
        titleLabel->setCharacterSize(fontSize);

        tmp = cursorText->getText().createUTF8EncodedString();
        auto pos = cursorText->getPosition();
        cursorText = osg::clone( textPrototype.get(), "cursor", osg::CopyOp::SHALLOW_COPY );
        cursorText->setText(tmp);
        geode->addDrawable(cursorText);
        cursorText->setPosition(pos);

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

float Chart::maxLabelsWidth(const std::vector< osgText::TextPtr > & labels)
{
    float ret = 0;
    for(auto it = labels.begin(); it != labels.end(); it++){
        if((*it) != nullptr){
            ret = std::max(ret, osgText::textWidth(*(*it)));
        }
    }

    return ret;
}

void Chart::refreshAllLabelsText()
{
    //odœwie¿ labelki jeœli to konieczne
    ChartSeriePtr activeSerie = this->activeSerieIndex < 0 ? nullptr : series[this->activeSerieIndex];
    if ( activeSerie ){
        
        if(isShowingAxisX() == true){
            auto xrange = activeSerie->getXRange();            

            rebuildLabelsText(labelsX, ((xUnitsSeparated == false) && (showXUnits == true)) ? activeSerie->getXUnit() : "", !showGridX, xrange.first, xrange.second);
        }

        xUnitLabelVisible = (xUnitsSeparated == true) && (showXUnits == true) && (activeSerie->getXUnit().empty() == false);

        if(xUnitLabelVisible == true){
            std::ostringstream unit;
            unit << "[" << activeSerie->getXUnit() << "]";
            xAxisUnitsLabel->setText(unit.str());

            if(geode->getDrawableIndex(xAxisUnitsLabel) == geode->getNumDrawables()){
                geode->addDrawable(xAxisUnitsLabel);
            }
        }else{
            if(geode->getDrawableIndex(xAxisUnitsLabel) != geode->getNumDrawables()){
                geode->removeDrawable(xAxisUnitsLabel);
            }
        }



        if(isShowingAxisY()){
            auto yrange = activeSerie->getYRange();

            rebuildLabelsText(labelsY, ((yUnitsSeparated == false) && (showYUnits == true)) ? activeSerie->getYUnit() : "", !showGridY, yrange.first, yrange.second);
        }

        yUnitLabelVisible = (yUnitsSeparated == true) && (showYUnits == true) && (activeSerie->getYUnit().empty() == false);

        if(yUnitLabelVisible == true){
            std::ostringstream unit;
            unit << "[" << activeSerie->getYUnit() << "]";
            yAxisUnitsLabel->setText(unit.str());
            if(geode->getDrawableIndex(yAxisUnitsLabel) == geode->getNumDrawables()){
                geode->addDrawable(yAxisUnitsLabel);
            }
        }else{
            if(geode->getDrawableIndex(yAxisUnitsLabel) != geode->getNumDrawables()){
                geode->removeDrawable(yAxisUnitsLabel);
            }
        }
    }

    //title
    titleVisible = false;
    if(isShowingTitle() == true && titleLabel->getText().empty() == false){
        titleVisible = true;
        if(geode->getDrawableIndex(titleLabel) == geode->getNumDrawables()){
            geode->addDrawable(titleLabel);
        }
    }else{
        if(geode->getDrawableIndex(titleLabel) != geode->getNumDrawables()){
            geode->removeDrawable(titleLabel);
        }
    }
}

void Chart::refreshAll()
{
    refreshAllLabelsText();

    repositionAll();

    // odstêp miedzy elementami;
    // 2 podane jawnie, bo trzeba zarezerwowaæ wspó³rzêdne dla t³a i osiek
    float zStep = zRange/float(3 + series.size());
    float currentZ = z;

    refreshTitle(currentZ);

    refreshGrid(currentZ);

    currentZ += zStep;


    refreshSeries(currentZ, zStep);

    refreshFrame(currentZ);
    refreshAxis(currentZ);
    refreshLabelsPositions(currentZ);

    currentZ += zStep;

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
    //text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX); 
    text->setAxisAlignment(osgText::Text::SCREEN); 
    text->setAlignment(osgText::Text::CENTER_CENTER);
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
        if ( geom && (geode->getDrawableIndex(geom) < geode->getNumDrawables()) ) {
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
    // czy w ogóle jest coœ do pokazania?
    if ( condition ) {
        if ( text && ( geode->getDrawableIndex(text) < geode->getNumDrawables()) ) {
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

void Chart::refreshSeries(float & currentZ, float zStep)
{
    float x, y, w, h;
    getGridLocation(x,y,w,h);

    if(w < 0 || h < 0)
    {
        return;
    }

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
}

void Chart::refreshTitle(float z)
{
    if(titleVisible == true){
        
        float x, y, w, h;
        getClientLocation(x, y, w, h);

        float titleHeight = osgText::textHeight(*titleLabel);
        
        osg::Vec3 pos(0,0,z);

        switch(titlePosition){
        case BOTTOM_CHART_CENTER:
            pos.x() = x + w / 2.0;
            pos.y() = y;

            break;

        case BOTTOM_GRID_CENTER:
            {
                auto pos = getGridLocation();
                
                pos.x() = pos.x() + pos.w() / 2.0;
                pos.y() = y;
            }

            break;
        case TOP_CHART_CENTER:
            pos.x() = x + w / 2.0;
            pos.y() = y + h - titleHeight;

            break;

        case TOP_GRID_CENTER:
            {
                auto gpos = getGridLocation();

                pos.x() = gpos.x() + gpos.w() / 2.0;
                pos.y() = y + h - titleHeight;
            }

            break;
        }

        titleLabel->setPosition(pos);
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
        getGridLocation(x, y, w, h);

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

void Chart::refreshFrame(float z)
{
    // czy w ogóle jest coœ do pkazania?
    if ( prepareGeometry(frame, !showFrame, "frame" ) ) {
        float x, y, w, h;
        getGridLocation(x, y, w, h);

        // wierzcho³ki
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(frame->getVertexArray());
        vertices->resize(0);
        vertices->push_back(osg::Vec3(x,y,z));
        vertices->push_back(osg::Vec3(x + w,y,z));

        vertices->push_back(osg::Vec3(x + w,y,z));
        vertices->push_back(osg::Vec3(x + w,y + h,z));

        vertices->push_back(osg::Vec3(x + w,y + h,z));
        vertices->push_back(osg::Vec3(x,y + h,z));

        vertices->push_back(osg::Vec3(x,y + h,z));
        vertices->push_back(osg::Vec3(x,y,z));

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(frame->getColorArray());
        colors->resize(1);
        colors->at(0) = frameColor;
        frame->setColorBinding(osg::Geometry::BIND_OVERALL);

        // ustawienie prymitywów
        osg::DrawArrays* primitives = dynamic_cast<osg::DrawArrays*>(frame->getPrimitiveSet(0));
        primitives->set(GL_LINES, 0, vertices->getNumElements());

        // "zabrudzenie" geometrii
        frame->setVertexArray(vertices);
    }
}

void Chart::refreshAxis(float z)
{
    using namespace osg;
    // czy w ogóle jest coœ do pkazania?
    if ( prepareGeometry(axises, !showAxisX && !showAxisY, "axises" ) ) {

        float x, y, w, h;
        getGridLocation(x, y, w, h);

        // wierzcho³ki
        Vec3Array* vertices = dynamic_cast<Vec3Array*>(axises->getVertexArray());
        vertices->resize(0);
        if (showAxisY) {
            // linia
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+h, z));
            
            // koñcówki
            float dashWidth = this->dashWidth;
            if(axisYInside == false){
                dashWidth = -dashWidth;
            }

            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+dashWidth, y, z));
            vertices->push_back(Vec3(x, y+h, z));
            vertices->push_back(Vec3(x+dashWidth, y+h, z));

        }
        if (showAxisX) {
            //linia
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+w, y, z));

            // koñcówki
            float dashWidth = this->dashWidth;
            if(axisXInside == false){
                dashWidth = -dashWidth;
            }

            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+dashWidth, z));
            vertices->push_back(Vec3(x+w, y, z));
            vertices->push_back(Vec3(x+w, y+dashWidth, z));
        }

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(axises->getColorArray());
        colors->resize(1);
        colors->at(0) = axisesColor;
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
    ChartSeriePtr activeSerie = this->activeSerieIndex < 0 ? nullptr : series[this->activeSerieIndex];
    if ( activeSerie ) {

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        auto xrange = activeSerie->getXRange();

        refreshLabels(labelsX, activeSerie->getXUnit(), !showGridX, xrange.first, xrange.second, x , x + w, y + labelsToAxisesOffset, y + labelsToAxisesOffset, z, osgText::Text::CENTER_BOTTOM);

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
        refreshLabels(labelsY, activeSerie->getYUnit(), !showGridY, rangeMin, rangeMin + range, x + labelsToAxisesOffset, x + labelsToAxisesOffset, y, y + h, z, osgText::Text::LEFT_CENTER);
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

        // nadajemy etykiete
        std::ostringstream buffer;
        formatStream(buffer, yrange.first, yrange.second);
        buffer << value;
        cursorText->setText( buffer.str() );

        float width = osgText::textWidth(*cursorText);

        //ustalamy pozycjê - mamy punkt zaczepienia (cursorX, cursorY)
        //defaultowo wartoœci sa pokazywane nad punktem
        //ale mo¿e siê okazaæ ¿e bêd¹ niewidoczne wiêc bêdziemy musieli je modyfikowaæ
        Vec3 pos(cursorX + labelsToAxisesOffset + width / 2.0, cursorY + labelsToAxisesOffset + getTextPrototypeHeight() / 2.0, z);

        if(pos.y() + getTextPrototypeHeight() / 2.0 > y + h){
            pos.y() = cursorY - labelsToAxisesOffset - getTextPrototypeHeight() / 2.0;
        }

        if(pos.x() + width / 2.0 > x + w){
            pos.x() = cursorX - labelsToAxisesOffset - width / 2.0;
        }

        // teraz mo¿na pozycjonowaæ...
        cursorText->setPosition( pos );
        cursorText->setAlignment( osgText::Text::CENTER_CENTER );

        // przygotowanie kursora
        Vec3Array* vertices = dynamic_cast<Vec3Array*>(cursor->getVertexArray());
        vertices->resize(0);

        auto loc = getGridLocation();
        vertices->push_back( Vec3(cursorX, loc[1], z) );
        vertices->push_back( Vec3(cursorX, loc[1] + loc[2], z) );

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(cursor->getColorArray());
        colors->resize(1);
        colors->at(0) = cursorColor;
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

void Chart::rebuildLabelsText(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max)
{
    UTILS_ASSERT(labels.size() > 1);

    std::ostringstream buffer;
    if ( !condition ) {
        formatStream(buffer, min, max);
    }

    float t = 0;
    float dt = 1.0f / (labels.size() - 1);

    for (size_t i = 0; i < labels.size(); ++i, t += dt) {
        osgText::TextPtr& text = labels[i];
        if ( prepareText(text, condition, "label")  ) {

            // konwersja na str
            float val = lerp(min, max, t);
            buffer.str( val < 0 ? "" : " ");
            buffer << val << unit;
            text->setText( buffer.str() );
        }
    }
}

void Chart::refreshLabelsPositions(float currentZ)
{
    float x, y, w, h;
    getGridLocation(x, y, w, h);

    float newY = y - dashWidth - labelsToAxisesOffset;
    float xLabelY = newY - labelsToAxisesOffset - getTextPrototypeHeight();
    osgText::TextBase::AlignmentType xLabelsAligment = osgText::TextBase::CENTER_TOP;
    if(isAxisXInside() == true){
        newY = y + dashWidth + labelsToAxisesOffset;
        xLabelY = y - labelsToAxisesOffset;
        xLabelsAligment = osgText::TextBase::CENTER_BOTTOM;
    }

    refreshLabelsPositions(labelsX, x, x + w, newY, newY, z, xLabelsAligment);

    float newX = x + dashWidth + labelsToAxisesOffset;
    float yLabelX = x - labelsToAxisesOffset;
    osgText::TextBase::AlignmentType yLabelsAligment = osgText::TextBase::LEFT_CENTER;
    if(isAxisYInside() == false){
        newX = x - dashWidth - labelsToAxisesOffset;
        yLabelX = newX - labelsToAxisesOffset - maxLabelsWidth(labelsY);
        yLabelsAligment = osgText::TextBase::RIGHT_CENTER;
    }

    refreshLabelsPositions(labelsY, newX, newX, y, y + h, z, yLabelsAligment);

    if(xUnitLabelVisible == true){
        xAxisUnitsLabel->setAlignment(xLabelsAligment);
        xAxisUnitsLabel->setPosition(osg::Vec3(x + w / 2.0, xLabelY, z));
    }

    if(yUnitLabelVisible == true){
        yAxisUnitsLabel->setAlignment(osgText::TextBase::RIGHT_CENTER);
        yAxisUnitsLabel->setPosition(osg::Vec3(yLabelX, y + h / 2.0, z));
    }
}

void Chart::refreshLabelsPositions(std::vector<osgText::TextPtr> &labels, float x0, float x1, float y0, float y1, float z, osgText::TextBase::AlignmentType alignment)
{
    UTILS_ASSERT(labels.size() > 1);

    float t = 0;
    float dt = 1.0f / (labels.size() - 1);
    
    for (size_t i = 0; i < labels.size(); ++i, t += dt) {
        osgText::TextPtr& text = labels[i];
        if ( text != nullptr ) {
            // ustawienie pozycji
            text->setAlignment(alignment);
            text->setPosition(osg::Vec3(lerp(x0, x1, t), lerp(y0, y1, t) , z));
        }
    }
}

bool Chart::isShowingXUnits() const
{
    return showXUnits;
}

void Chart::setShowingXUnits(bool showUnits)
{
    showXUnits = showUnits;
    setDirty();
}

bool Chart::isShowingYUnits() const
{
    return showYUnits;
}

void Chart::setShowingYUnits(bool showUnits)
{
    showYUnits = showUnits;
    setDirty();
}

