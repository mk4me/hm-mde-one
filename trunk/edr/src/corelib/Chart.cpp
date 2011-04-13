#include "CorePCH.h"
#include <core/Chart.h>
#include <core/LineChartSerie.h>
#include <core/ChartData.h>
#include <core/ChartPointer.h>
#include <osg/LineStipple>
#include <math.h>
#include <iomanip>

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

void Chart::addChannel( const core::ScalarChannelConstPtr& channel, osg::Vec4 color )
{
    // tworzymy nowy wykres
    osg::ref_ptr<core::LineChartSerie> serie = new core::LineChartSerie();

    serie->setData(channel);
    serie->setColor(color);
    serie->setName(channel->getName());

    addChannel(serie);
}

bool Chart::addChannel( const core::ChartSeriePtr& channel )
{
    if ( geode->containsDrawable(channel) ) {
        UTILS_FAIL("Kana� ju� jest dodany.");
        return false;
    } else {
        if ( activeSerieIndex < 0 ) {
            activeSerieIndex = 0;
        }
        // dodajemy do listy i do dzieci
        series.push_back(channel);
        geode->addDrawable(channel);
        setDirty();
        return true;
    }
}

void Chart::removeAllChannels()
{
    BOOST_FOREACH(core::ChartSeriePtr chart, series) {
        geode->removeDrawable(chart);
    }
    series.clear();
    activeSerieIndex = -1;
    setDirty();
}

int Chart::getNumChannels() const
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

const core::ChartSerie* Chart::getActiveSerie() const
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
    // +1 poniewa� dodawane na ekstremum
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

// osg::Geode* Chart::createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps,float scale,std::string unit)
// {
// 	
// 
// 	osg::Geode* geode = new osg::Geode;
// 	osg::StateSet* stateset = geode->getOrCreateStateSet();
// 	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
// 	osg::Vec3 dv = e-s;
// 	dv /= float(numReps-1);
// 	scale /= float(numReps-1);
// 	float actualScale=0.0f;
// 	osg::Vec3 pos = s;
// 	osg::Vec3 posX = osg::Vec3(0,30,0);
// 	osg::Vec3 posY = osg::Vec3(5,0,0);
// 	osg::Vec3Array* vertices = new osg::Vec3Array;
// 	vertices->push_back(s);
// 	vertices->push_back(e);
// 	for(int i=0;i<numReps;++i)
// 	{
// 	
// 
// 		if(e.x()>s.x()){
// 			if(showLabel){
// 				osgText::Text* text=createLabel(pos-posX,fontSize, (formatNumber(actualScale))+unit);
// 				text->setAlignment(osgText::Text::RIGHT_CENTER);
// 				geode->addDrawable(text);}
// 			if(i!=0){
// 				vertices->push_back(osg::Vec3(pos.x(),pos.y()-5,1));
// 				vertices->push_back(osg::Vec3(pos.x(),pos.y()+5,1));
// 			}
// 				//geode->addDrawable(createLine(pos.x(),pos.y()-5,}
// 				//pos.x(),pos.y()+5,0,
// 				//osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
// 		}
// 		else{
// 			if(showLabel){
// 				osgText::Text* text=createLabel(pos ,fontSize,(formatNumber(actualScale))+unit);
// 				text->setAlignment(osgText::Text::RIGHT_CENTER);
// 					geode->addDrawable(text);}
// 				if(i!=0){
// 				vertices->push_back(osg::Vec3(pos.x()-5,pos.y(),1));
// 				vertices->push_back(osg::Vec3(pos.x()+5,pos.y(),1));
// 			}
// 		}
// 		
// 		pos += dv;
// 		actualScale += scale;
// 
// 	}
// 
// 
// 
// 	osg::Geometry* geom = new osg::Geometry;
// 	osg::Vec4Array* colors=new osg::Vec4Array();
// 	colors->push_back(axisColor);
// 
//  
// 	geom->setColorArray(colors);
// 	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
// 	geom->setVertexArray(vertices);
// 
//  geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,vertices->getNumElements())); 
// 
// 	
// 	geode->addDrawable(geom);
// 	geode->setName("axis");
// 
// 
// 	return geode;
// }
// osgText::Text* Chart::createLabel(const osg::Vec3& pos, float size, const std::string& label)
// {
// 
// 	osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
// 
// 	
// 		osgText::Text* text = new  osgText::Text;
// 
// 		text->setAxisColor(axisColor);
// 		text->setFontResolution(100,100);
// 		text->setPosition(pos);
// 		text->setFont(font);
// 		text->setCharacterSize(size);
// 		text->setAxisAlignment(osgText::Text::SCREEN); 
// 		text->setAlignment(osgText::Text::LEFT_CENTER);
// 		text->setText(label);
// 		text->setLayout(osgText::Text::LEFT_TO_RIGHT);
// 		text->setBoundingBoxMargin(0.25f);
// 		
// 
// 	
// 
// 	return text;    
// }


// 
// void Chart::deprecated_addChartSeries(deprecated_ChartData* chartData,osg::Vec4 axisColor){
// 	xNumReps=2;
// 	yNumReps=2;
// 	borderSize=5;
// 	showLabel=false;
// 	showBorder=false;
// 	float yMaxScale=0;
// 	if(chartData->getRNumber()>0){
// 	deprecated_data.push_back(chartData);
// 	std::vector<deprecated_ChartData*>::iterator itData= deprecated_data.end()-1;
// 	deprecated_dataSeries.push_back(new deprecated_LineChart((*itData),x+borderSize,y+borderSize,w-borderSize,h-borderSize,axisColor));
// 	std::vector<deprecated_LineChart*>::iterator itDataSeries = deprecated_dataSeries.end()-1;
// 	(*itDataSeries)->setShowLabel(showLabel);
// 	this->addChild(*itDataSeries);
// 	
// 	if(deprecated_data.size()>1){
// 		 itData=deprecated_data.begin();
// 		 for(; itData < deprecated_data.end(); itData++){
// 			 if(yMaxScale<(*itData)->getScaleY()){
// 				 yMaxScale=(*itData)->getScaleY();
// 			 }
// 
// 		 }
// 		 itData=deprecated_data.begin();
// 		 for(; itData < deprecated_data.end(); itData++){
// 			 (*itData)->setScaleY(yMaxScale);
// 			 (*itData)->normalize();
// 		 }
// 		repaint(); 
// 	}else{
// 
// 	xAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(w-borderSize*2,y+borderSize,0),xNumReps,(*deprecated_data.begin())->getScaleX()/(*deprecated_data.begin())->getFPS(),"s");
// 	yAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,h-borderSize,0),yNumReps,(*deprecated_data.begin())->getScaleY(),(*deprecated_data.begin())->getUnit());
// 	this->addChild(xAxis);
// 	this->addChild(yAxis);
// 	}
// 	
// 	mainLabel.push_back(createMainLabel((*itDataSeries)->getAxisColor(),(*itData)->getName()));
// 	std::vector<osg::Geode*>::iterator itLabel= mainLabel.end()-1;
// 	this->addChild(*itLabel);
// 	labelOffset+=100;
// 	}
// 
// }
// 
// void Chart::deprecated_addChartPreviewSeries(deprecated_ChartData* chartData,osg::Vec4 axisColor){
// 	float yMaxScale=0;
// 	if(chartData->getRNumber()>0){
// 	deprecated_data.push_back(chartData);
// 	std::vector<deprecated_ChartData*>::iterator itData= deprecated_data.end()-1;
// 	deprecated_dataSeries.push_back(new deprecated_LineChart((*itData),x+borderSize*2,y+borderSize,w-borderSize*2,h-borderSize,axisColor));
// 	std::vector<deprecated_LineChart*>::iterator itDataSeries = deprecated_dataSeries.end()-1;
// 	this->addChild(*itDataSeries);
// 	
// 	if(deprecated_data.size()>1){
// 		 itData=deprecated_data.begin();
// 		 for(; itData < deprecated_data.end(); itData++){
// 			 if(yMaxScale<(*itData)->getScaleY()){
// 				 yMaxScale=(*itData)->getScaleY();
// 			 }
// 
// 		 }
// 		 itData=deprecated_data.begin();
// 		 for(; itData < deprecated_data.end(); itData++){
// 			 (*itData)->setScaleY(yMaxScale);
// 			 (*itData)->normalize();
// 		 }
// 		repaint(); 
// 	}else{
// 
// 	xAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(w-borderSize*2,y+borderSize,0),xNumReps,(*deprecated_data.begin())->getScaleX()/(*deprecated_data.begin())->getFPS(),"s");
// 	yAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,h-borderSize,0),yNumReps,(*deprecated_data.begin())->getScaleY(),(*deprecated_data.begin())->getUnit());
// 	this->addChild(xAxis);
// 	this->addChild(yAxis);
// 	}
// 	
// 	mainLabel.push_back(createMainLabel((*itDataSeries)->getAxisColor(),(*itData)->getName()));
// 	std::vector<osg::Geode*>::iterator itLabel= mainLabel.end()-1;
// 	this->addChild(*itLabel);
// 	labelOffset+=100;
// 	}
// 
// }
// int Chart::deprecated_getFPS(){
// 	if(deprecated_data.size()>0)
// return (*deprecated_data.begin())->getFPS();
// 	else 
// 		return 0;
// }
// int Chart::deprecated_getFrameNumber(){
// 	if(deprecated_data.size()>0)
// return (*deprecated_data.begin())->getRNumber();
// 	else
// 		return 0;
// }


void Chart::refreshCursor()
{
    float zStep = zRange/float(2 + series.size());
    refreshCursor(z + zRange - zStep);
}

void Chart::refreshAll()
{
    float x, y, w, h;
    getClientLocation(x, y, w, h);

    // odst�p miedzy elementami;
    // 2 podane jawnie, bo trzeba zarezerwowa� wsp�rz�dne dla t�a i osiek
    float zStep = zRange/float(2 + series.size());
    float currentZ = z;

    //getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON );
    refreshGrid(currentZ);
    currentZ += zStep;

    if ( normalized ) {
        BOOST_FOREACH( const core::ChartSeriePtr& serie, series ) {
            serie->setLocation( x, y, w, h );
            serie->setZ(currentZ);
            serie->refresh();
            currentZ += zStep;
        }
    } else {

        bool shareZero = true;

        // dzielenie po jednostkach
        std::multimap<std::string, ChartSeriePtr> byUnits;
        // TODO: doda� skalowanie dla OX!
        std::map<std::string, std::pair<float, float> > unitInfo;

        BOOST_FOREACH( const ChartSeriePtr& serie, series ) {
            // pobranie informacji o osi OY
            std::pair<float, float> yRange = serie->getYRange();
            UTILS_ASSERT( yRange.first <= yRange.second );
            const std::string yUnit = serie->getYUnit();
            // uzupe�nienie min/max zakresu danej jednostki
            auto found = unitInfo.find(yUnit);
            if ( found == unitInfo.end() ) {
                unitInfo[yUnit] = yRange;
            } else {
                found->second.first = std::min(yRange.first, found->second.first);
                found->second.second = std::max(yRange.second, found->second.second);
            }
            // uzupe�nienie podzia�u po jednostkach
            byUnits.insert(std::make_pair(serie->getYUnit(), serie));
        }

        if ( shareZero ) {
            // uwsp�lnienie polega na przeskalowaniu zakres�w tak, �eby wzgl�dna odleg�o�� min/max od zera
            // by�a jednakowa
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
            // normalizujemy zakres - w takim stosunku do zakresu musz� by� min i max
            float maxDiff = maxPosRatio + maxNegRatio;
            if ( maxDiff != 0.0f ) {
                float maxDiffInv = 1.0f / maxDiff;
                maxPosRatio = maxPosRatio * maxDiffInv;
                maxNegRatio = maxNegRatio * maxDiffInv;
                // pomocnicze zmienne pozwalaj�ce unikn�� wielokrotnego dzielenia
                float scaleMinToMaxFactor = maxPosRatio / maxNegRatio;
                float scaleMaxToMinFactor = 1.0f / scaleMinToMaxFactor;
                // skalujemy wszystkie zakresy tak, aby stosunek max i min do zakresu by� taki jak wyliczone zbiorcze
                for ( auto it = unitInfo.begin(), last = unitInfo.end(); it != last; ++it ) {
                    std::pair<float, float>& range = it->second;
                    float diff = range.second - range.first;
                    if ( diff != 0.0f ) {
                        float negRatio = fabs(range.first) / diff;
                        float posRatio = fabs(range.second) / diff;
                        if ( maxNegRatio == 0.0f ) {
                            // nie ma co skalowa�
                            UTILS_ASSERT(range.first == 0.0f);
                        } else if ( maxPosRatio == 0.0f ) {
                            // nie ma co skalowa�
                            UTILS_ASSERT(range.second == 0.0f);
                        } else if ( negRatio > maxNegRatio ) {
                            // rozci�gamy g�rny zakres
                            range.second = scaleMinToMaxFactor * fabs(range.first);
                        } else {
                            // rozci�gamy dolny zakres
                            range.first = -scaleMaxToMinFactor * fabs(range.second);
                        }
                    } else {
                        // poniewa� funkcja jest niezmienna wporawdzamy rozszerzamy sztucznie zakres
                        range.first -= maxNegRatio;
                        range.second += maxPosRatio;
                    }
                }
            } else {
                // poniewa� wszystkie funkcje s� sta�e wprowadzamy sztuczny zakres
                for ( auto it = unitInfo.begin(), last = unitInfo.end(); it != last; ++it ) {
                    std::pair<float, float>& range = it->second;
                    // poniewa� funkcja jest niezmienna wporawdzamy rozszerzamy sztucznie zakres
                    range.first -= 1.0f;
                    range.second += 1.0f;
                }
            }
        }

        for ( auto unit = unitInfo.begin(); unit != unitInfo.end(); ++unit ) {
            
            auto first = byUnits.lower_bound(unit->first);
            auto last = byUnits.upper_bound(unit->first);

            // okre�lamy minimum i maksimum z danego zakresu
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
            
            // teraz mo�emy pozycjonowa�
            for ( auto it = first; it != last; ++it ) {
                const core::ChartSeriePtr& serie = it->second;
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

// 	if(showBorder){
// 		if(border){
// 	osg::Geode* newBorder=createBorder();
// 	this->replaceChild(border,newBorder);
// 	border=newBorder;}else{
// 		border=createBorder();
// 		this->addChild(border);}
// 	}else
// 		this->removeChild(this->getChildIndex(border));
// 
// 
//     auto front = series.front();
// 
// 	osg::Geode* newGrid=createGrid();
// 	osg::Geode* newXAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(w-borderSize*2,y+borderSize,0),xNumReps,
//         front->getXLength(), front->getXUnit());
// 	osg::Geode* newYAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,h-borderSize,0),yNumReps,
//         front->getYLength(), front->getYUnit());
// 
// 	this->replaceChild(grid,newGrid);
// 	this->replaceChild(xAxis,newXAxis);
// 	this->replaceChild(yAxis,newYAxis);
// 
// 	
// 	grid=newGrid;
// 	xAxis=newXAxis;
// 	yAxis=newYAxis;
// 	auto itPos = series.begin();
// 	std::vector<osg::Geode*>::iterator itLabel= mainLabel.begin();
// 	std::vector<osg::Geode*> newMainLabel;
// 	for(; itPos < series.end(); itPos++, itLabel++){
//         (*itPos)->setLocation( x+borderSize*2,y+borderSize,w-borderSize*2,h-borderSize );
// 	    (*itPos)->refreshAll();
// 	    newMainLabel.push_back(createMainLabel((*itPos)->getAxisColor(), (*itPos)->getName()));
// 	    std::vector<osg::Geode*>::iterator itNewLabel= newMainLabel.end()-1;
// 	    this->replaceChild((*itLabel),  (*itNewLabel)  ); 
// 	}
// 	mainLabel=newMainLabel;



// 	if(showBorder){
// 		if(border){
// 	osg::Geode* newBorder=createBorder();
// 	this->replaceChild(border,newBorder);
// 	border=newBorder;}else{
// 		border=createBorder();
// 		this->addChild(border);}
// 	}else
// 		this->removeChild(this->getChildIndex(border));
// 	osg::Geode* newGrid=createGrid();
// 	osg::Geode* newXAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(w-borderSize*2,y+borderSize,0),xNumReps,(*deprecated_data.begin())->getScaleX()/(*deprecated_data.begin())->getFPS(),"s");
// 	osg::Geode* newYAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,h-borderSize,0),yNumReps,(*deprecated_data.begin())->getScaleY(),(*deprecated_data.begin())->getUnit());
// 
// 
// 	
// 	this->replaceChild(grid,newGrid);
// 	this->replaceChild(xAxis,newXAxis);
// 	this->replaceChild(yAxis,newYAxis);
// 
// 	
// 	grid=newGrid;
// 	xAxis=newXAxis;
// 	yAxis=newYAxis;
// 	std::vector<deprecated_LineChart*>::iterator itPos = deprecated_dataSeries.begin();
// 	std::vector<deprecated_ChartData*>::iterator dataPos = deprecated_data.begin();
// 	std::vector<osg::Geode*>::iterator itLabel= mainLabel.begin();
// 	std::vector<osg::Geode*> newMainLabel;
// 	for(; itPos < deprecated_dataSeries.end(); itPos++,dataPos++,itLabel++){
// 	  (*itPos)->repaint((*dataPos),x+borderSize*2,y+borderSize,w-borderSize*2,h-borderSize);
// 	  newMainLabel.push_back(createMainLabel((*itPos)->getAxisColor(),(*dataPos)->getName()));
// 	  std::vector<osg::Geode*>::iterator itNewLabel= newMainLabel.end()-1;
// 	  this->replaceChild((*itLabel),  (*itNewLabel)  ); 
// 	}
// 	mainLabel=newMainLabel;
}


// osg::Geode* Chart::createMainLabel(osg::Vec4 axisColor, std::string name){
// 		osg::Geode* geode = new osg::Geode;
// 		this->setName(name);
// 			geode->setName("label");
// 	if(showLabel){
// 	osg::StateSet* ss = geode->getOrCreateStateSet();
// 
// 
// 	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
// 
// 	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
// 	geode->addDrawable(createLabel(osg::Vec3(x+30+labelOffset,h-10,0),12,name));
// 	
// 	osg::Vec3Array* vertices = new osg::Vec3Array;
// 	vertices->push_back(osg::Vec3(x+10+labelOffset,h-20+5,0));
// 	vertices->push_back(osg::Vec3(x+20+labelOffset,h-20+5,0));
// 	vertices->push_back(osg::Vec3(x+20+labelOffset,h-20+15,0));
// 	vertices->push_back(osg::Vec3(x+10+labelOffset,h-20+15,0));
// 	vertices->push_back(osg::Vec3(x+10+labelOffset,h-20+5,0));
// 
// 	
// 	osg::Geometry* geom = new osg::Geometry;
// 	osg::Vec4Array* colors=new osg::Vec4Array();
// 	colors->push_back(axisColor);
// 
// 
// 	geom->setColorArray(colors);
// 	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
// 	geom->setVertexArray(vertices);
// 
// 	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,vertices->size()));
// 
// 
// 	geode->addDrawable(geom);
// 
// 	
// 
// }
// return geode;
// }

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
    //text->setBoundingBoxMargin(0.25f);


    return text;    
}

void Chart::formatStream(std::ostringstream& stream, float min, float max, int decimalsToShow /*= 2*/ )
{
    float diff = fabs(max - min);
    // rz�d r�nicy mi�dzy max i min (ujemny dla diff < 1)
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
    // czy w og�le jest co� do pkazania?
    if ( condition ) {
        if ( geom && geode->getDrawableIndex(geom) ) {
            // trzeba usun��!
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
    // czy w og�le jest co� do pkazania?
    if ( condition ) {
        if ( text && geode->getDrawableIndex(text) ) {
            // trzeba usun��!
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
    // czy w og�le jest co� do pkazania?
    if ( prepareGeometry(grid, !showGridX && !showGridY, "grid" ) ) {

        if ( !grid->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINESTIPPLE) ) {
            grid->getStateSet()->setAttributeAndModes(new osg::LineStipple(2, 0xAAAA));
        }

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        // wierzcho�ki
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(grid->getVertexArray());
        vertices->resize(0);
        //for (int i = gridDensity; showGridY && i < h; i += gridDensity) {
        for ( float dy = 0.0f, step = h / gridDensity; showGridY && dy < h; dy += step  ) {
            vertices->push_back(osg::Vec3(x,        y+dy,    z));
            vertices->push_back(osg::Vec3(x+w,  y+dy,    z));
        }
        //for (int i = gridDensity; showGridX && i < w; i += gridDensity) {
        for ( float dx = 0.0f, step = w / gridDensity; showGridY && dx < w; dx += step ) {
            vertices->push_back(osg::Vec3(x+dx,      y,          z));
            vertices->push_back(osg::Vec3(x+dx,      y+h,   z));
        }

        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(grid->getColorArray());
        colors->resize(1);
        colors->at(0) = gridColor;
        grid->setColorBinding(osg::Geometry::BIND_OVERALL);

        // ustawienie prymityw�w
        osg::DrawArrays* primitives = dynamic_cast<osg::DrawArrays*>(grid->getPrimitiveSet(0));
        primitives->set(GL_LINES, 0, vertices->getNumElements());

        // "zabrudzenie" geometrii
        grid->setVertexArray(vertices);
    }
}

void Chart::refreshAxis(float z)
{
    using namespace osg;
    // czy w og�le jest co� do pkazania?
    if ( prepareGeometry(axises, !showAxisX && !showAxisY, "axises" ) ) {

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        // wierzcho�ki
        Vec3Array* vertices = dynamic_cast<Vec3Array*>(axises->getVertexArray());
        vertices->resize(0);
        if (showAxisY) {
            // linia
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+h, z));
            // ko�c�wki
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+dashWidth, y, z));
            vertices->push_back(Vec3(x, y+h, z));
            vertices->push_back(Vec3(x+dashWidth, y+h, z));

        }
        if (showAxisX) {
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+w, y, z));
            // ko�c�wki
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

        // ustawienie prymityw�w
        osg::DrawArrays* primitives = dynamic_cast<osg::DrawArrays*>(axises->getPrimitiveSet(0));
        primitives->set(GL_LINES, 0, vertices->getNumElements());

        // "zabrudzenie" geometrii
        axises->setVertexArray(vertices);
    }
}

void Chart::refreshLabels( float z )
{
    const float offset = 8;

    core::ChartSeriePtr activeSerie = this->activeSerieIndex < 0 ? nullptr : series[this->activeSerieIndex];
    if ( activeSerie ) {

        float x, y, w, h;
        getClientLocation(x, y, w, h);

        auto xrange = activeSerie->getXRange();

        refreshLabels(labelsX, activeSerie->getXUnit(), !showGridX, xrange.first, xrange.second, x , x + w, y + offset, y + offset, z, osgText::Text::CENTER_BOTTOM);


        osg::Vec2 pos = activeSerie->getPosition();
        osg::Vec2 size = activeSerie->getSize();
        auto yrange = activeSerie->getYRange();

        // znormalizowane wsp�rz�dne
        float ny = (pos.y() - y) / h;
        float nh = size.y() / h;
        // prze�o�enie znormalizowanych wsp�rz�dnych na zakres jednostek
        float range = (yrange.second - yrange.first) / nh;
        float rangeMin = yrange.first - ny * range;
        // od�wie�amy
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
        // okre�lamy miejsce gdzie b�dziemy rysowa� tekst
        core::ChartSeriePtr serie = series[activeSerieIndex];
        float x = serie->getPosition().x();
        float y = serie->getPosition().y();
        float w = serie->getSize().x();
        float h = serie->getSize().y();

        // trzeba jeszcze okre�li� warto�� w bie��cym punkcie czasowym
        auto yrange = serie->getYRange();
        auto xrange = serie->getXRange();
        float value = serie->getValue();
        float time = serie->getTime();

        // znormalizowana warto��, znormalizowany czas
        float nvalue = (value - yrange.first) / (yrange.second - yrange.first);
        float ntime = (time - xrange.first) / (xrange.second - xrange.first);

        float cursorX = lerp(x, x+w, ntime);
        float cursorY = lerp(y, y+h, nvalue);

        // teraz mo�na pozycjonowa�...
        cursorText->setPosition( Vec3(cursorX + xOffset, cursorY + yOffset, z) );
        // no i nada� etykiet�
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
        //vertices->push_back( Vec3(cursorX, cursorY, z) );
        vertices->push_back( Vec3(cursorX, loc.y() + loc.w(), z) );
        //         vertices->push_back(Vec3(cursorX, cursorY + 3, z));
        //         vertices->push_back(Vec3(cursorX+3, cursorY, z));
        //         vertices->push_back(Vec3(cursorX, cursorY - 3, z));
        //         vertices->push_back(Vec3(cursorX-3, cursorY, z));
        //         vertices->push_back(Vec3(cursorX, cursorY + 3, z));


        // kolory
        osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(cursor->getColorArray());
        colors->resize(1);
        colors->at(0) = Vec4(1,1,1,0.33f);
        cursor->setColorBinding(osg::Geometry::BIND_OVERALL);

        // ustawienie prymityw�w
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

