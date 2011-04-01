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

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;    
}


Chart::Chart(float x, float y, float width, float height) : 
margin(0.0f),
showGridX(false),
showGridY(false),
showAxisX(true),
showAxisY(true),
z(0),
zRange(0),
x(0), y(0), width(0), height(0),
dashWidth(5),
activeSerie(-1),
labelColor(1,1,1,1)
{
    background = new osg::Geode();
    background->setName("background");
    foreground = new osg::Geode();
    foreground->setName("foreground");
    addChild(background);
    addChild(foreground);

    textPrototype = createText( osg::Vec3(0, 0, 0), 12, "prototype");
    setGridDensity(10);


    refresh();
}

Chart::~Chart()
{
    
}

bool Chart::prepareGeometry(osg::GeometryPtr& geom, bool condition, const char* name)
{
    // czy w ogóle jest coœ do pkazania?
    if ( condition ) {
        if ( geom && background->getDrawableIndex(geom) ) {
            // trzeba usun¹æ!
            background->removeDrawable(geom);
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
            background->addDrawable(geom);
        }
        return true;
    }
}

bool Chart::prepareText( osgText::TextPtr& text, bool condition, const char* name )
{
    // czy w ogóle jest coœ do pkazania?
    if ( condition ) {
        if ( text && background->getDrawableIndex(text) ) {
            // trzeba usun¹æ!
            background->removeDrawable(text);
        }
        text = nullptr;
        return false;
    } else {
        // czy tworzymy od nowa?
        if ( !text ) {
            text = osg::clone( textPrototype.get(), name, osg::CopyOp::DEEP_COPY_ALL );
            background->addDrawable(text);
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

        float x = this->x + margin;
        float y = this->y + margin;
        float height = this->height - 2 * margin;
        float width = this->width - 2 * margin;

        // wierzcho³ki
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(grid->getVertexArray());
        vertices->resize(0);
        //for (int i = gridDensity; showGridY && i < height; i += gridDensity) {
        for ( float dy = 0.0f, step = height / gridDensity; showGridY && dy < height; dy += step  ) {
            vertices->push_back(osg::Vec3(x,        y+dy,    z));
            vertices->push_back(osg::Vec3(x+width,  y+dy,    z));
        }
        //for (int i = gridDensity; showGridX && i < width; i += gridDensity) {
        for ( float dx = 0.0f, step = width / gridDensity; showGridY && dx < width; dx += step ) {
            vertices->push_back(osg::Vec3(x+dx,      y,          z));
            vertices->push_back(osg::Vec3(x+dx,      y+height,   z));
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

        float x = this->x + margin;
        float y = this->y + margin;
        float height = this->height - 2 * margin;
        float width = this->width - 2 * margin;

        // wierzcho³ki
        Vec3Array* vertices = dynamic_cast<Vec3Array*>(axises->getVertexArray());
        vertices->resize(0);
        if (showAxisY) {
            // linia
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+height, z));
            // koñcówki
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+dashWidth, y, z));
            vertices->push_back(Vec3(x, y+height, z));
            vertices->push_back(Vec3(x+dashWidth, y+height, z));
                
        }
        if (showAxisX) {
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x+width, y, z));
            // koñcówki
            vertices->push_back(Vec3(x, y, z));
            vertices->push_back(Vec3(x, y+dashWidth, z));
            vertices->push_back(Vec3(x+width, y, z));
            vertices->push_back(Vec3(x+width, y+dashWidth, z));
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
    float x = this->x + margin;
    float y = this->y + margin;
    float height = this->height - 2 * margin;
    float width = this->width - 2 * margin;

    core::ChartSeriePtr activeSerie = this->activeSerie < 0 ? nullptr : series[this->activeSerie];
    if ( activeSerie ) {
        auto xrange = activeSerie->getXRange();
        refreshLabels(labelsX, activeSerie->getXUnit(), !showGridX, xrange.first, xrange.second, x , x + width, y + offset, y + offset, z, osgText::Text::CENTER_BOTTOM);
        auto yrange = activeSerie->getYRange();
        refreshLabels(labelsY, activeSerie->getYUnit(), !showGridY, yrange.first, yrange.second, x + offset, x + offset, y, y + height, z, osgText::Text::LEFT_CENTER);
    } else {
        BOOST_FOREACH(osgText::TextPtr& text, labelsX) {
            prepareText(text, true, "");
        }
        BOOST_FOREACH(osgText::TextPtr& text, labelsY) {
            prepareText(text, true, "");
        }
    }
}

void Chart::refreshPointer( float z )
{
    float x = this->x + margin;
    float y = this->y + margin;
    float height = this->height - 2 * margin;
    float width = this->width - 2 * margin;
    if ( prepareText(pointer, activeSerie < 0, "cursor") ) {
        pointer->setPosition( osg::Vec3(x+width / 2, y+height/2, z) );
        auto yrange = series[activeSerie]->getYRange();

        int decimalsToShow = 2;
        float diff = yrange.second - yrange.first;
        int logdiff = static_cast<int>(floorf(log10f(diff)));
        decimalsToShow = std::max( decimalsToShow - logdiff, 0 );
        std::ostringstream buffer;
        buffer << std::setiosflags(std::ios::fixed) << std::setprecision(decimalsToShow);
        buffer << series[activeSerie]->getValue();
        pointer->setText( buffer.str() );

    }
}

void Chart::refreshLabels(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max, float x0, float x1, float y0, float y1, float z, osgText::Text::AlignmentType align )
{
    UTILS_ASSERT(labels.size() > 1);

    float t = 0;
    float dt = 1.0f / (labels.size() - 1);

    int decimalsToShow = 2;
    if ( !condition ) {
        float diff = max - min;
        int logdiff = static_cast<int>(floorf(log10f(diff)));
        decimalsToShow = std::max( decimalsToShow - logdiff, 0 );
    }
    std::ostringstream buffer;
    buffer << std::setiosflags(std::ios::fixed) << std::setprecision(decimalsToShow);

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
void Chart::addChannel( const core::ScalarChannelConstPtr& channel, osg::Vec4 color )
{
    if ( activeSerie < 0 ) {
        activeSerie = 0;
    }

    // tworzymy nowy wykres
    osg::ref_ptr<core::LineChartSerie> serie = new core::LineChartSerie();

    serie->setData(channel);
    serie->setColor(color);
    serie->setName(channel->getName());

    // dodajemy do listy i do dzieci
    series.push_back(serie);
    foreground->addDrawable(serie);
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
// 	deprecated_dataSeries.push_back(new deprecated_LineChart((*itData),x+borderSize,y+borderSize,width-borderSize,height-borderSize,axisColor));
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
// 	xAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,(*deprecated_data.begin())->getScaleX()/(*deprecated_data.begin())->getFPS(),"s");
// 	yAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,(*deprecated_data.begin())->getScaleY(),(*deprecated_data.begin())->getUnit());
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
// 	deprecated_dataSeries.push_back(new deprecated_LineChart((*itData),x+borderSize*2,y+borderSize,width-borderSize*2,height-borderSize,axisColor));
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
// 	xAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,(*deprecated_data.begin())->getScaleX()/(*deprecated_data.begin())->getFPS(),"s");
// 	yAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,(*deprecated_data.begin())->getScaleY(),(*deprecated_data.begin())->getUnit());
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


void Chart::refreshLite()
{
    float zStep = zRange/float(2 + series.size());
    refreshPointer(z + zRange - zStep);
}

void Chart::refresh()
{
    // odstêp miedzy elementami;
    // 2 podane jawnie, bo trzeba zarezerwowaæ wspó³rzêdne dla t³a i osiek
    float zStep = zRange/float(2 + series.size());
    float currentZ = z;

    //getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON );
    refreshGrid(currentZ);
    currentZ += zStep;

    BOOST_FOREACH( const core::ChartSeriePtr& serie, series ) {
        serie->setLocation( x + margin, y + margin, std::max(width-margin*2, FLT_MIN), std::max(height-margin*2, FLT_MIN) );
        serie->setZ(currentZ);
        serie->refresh();
        currentZ += zStep;
    }

    refreshAxis(currentZ);
    refreshLabels(currentZ);
    refreshPointer(currentZ);

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
// 	osg::Geode* newXAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,
//         front->getXLength(), front->getXUnit());
// 	osg::Geode* newYAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,
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
//         (*itPos)->setLocation( x+borderSize*2,y+borderSize,width-borderSize*2,height-borderSize );
// 	    (*itPos)->refresh();
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
// 	osg::Geode* newXAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,(*deprecated_data.begin())->getScaleX()/(*deprecated_data.begin())->getFPS(),"s");
// 	osg::Geode* newYAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,(*deprecated_data.begin())->getScaleY(),(*deprecated_data.begin())->getUnit());
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
// 	  (*itPos)->repaint((*dataPos),x+borderSize*2,y+borderSize,width-borderSize*2,height-borderSize);
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
// 	geode->addDrawable(createLabel(osg::Vec3(x+30+labelOffset,height-10,0),12,name));
// 	
// 	osg::Vec3Array* vertices = new osg::Vec3Array;
// 	vertices->push_back(osg::Vec3(x+10+labelOffset,height-20+5,0));
// 	vertices->push_back(osg::Vec3(x+20+labelOffset,height-20+5,0));
// 	vertices->push_back(osg::Vec3(x+20+labelOffset,height-20+15,0));
// 	vertices->push_back(osg::Vec3(x+10+labelOffset,height-20+15,0));
// 	vertices->push_back(osg::Vec3(x+10+labelOffset,height-20+5,0));
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

float Chart::getLength() const
{
    if ( !series.size() ) {
        return 0.0f;
    } else {
        auto longest = std::max_element(series.begin(), series.end(), [](const core::ChartSerie* c1, const core::ChartSerie * c2) {
            return c1->getXLength() < c2->getXLength();
        });
        return (*longest)->getXLength();
    }
}

bool Chart::isShowingGridX() const
{
    return showGridX;
}

bool Chart::isShowingGridY() const
{
    return showGridY;
}

void Chart::setShowGridX( bool showGridX )
{
    this->showGridX = showGridX;
}

void Chart::setShowGridY( bool showGridY )
{
    this->showGridY = showGridY;
}

float Chart::getZ() const
{
    return z;
}

void Chart::setZ( float z )
{
    this->z = z;
}

float Chart::getZRange() const
{
    return zRange;
}

void Chart::setZRange( float zRange )
{
    this->zRange = zRange;
}

bool Chart::isShowingAxisX() const
{
    return showAxisX;
}

void Chart::setShowAxisX( bool showAxisX )
{
    this->showAxisX = showAxisX;
}

bool Chart::isShowingAxisY() const
{
    return showAxisY;
}

void Chart::setShowAxisY( bool showAxisY )
{
    this->showAxisY = showAxisY;
}

float Chart::getDashWidth() const
{
    return dashWidth;
}

void Chart::setDashWidth( float dashWidth )
{
    this->dashWidth = dashWidth;
}

float Chart::getMargin() const
{
    return margin;
}

void Chart::setMargin(float margin)
{
    this->margin=margin;
    refresh();
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
    refresh();
}

osg::Vec4 Chart::getGridColor()
{
    return gridColor;
}

void Chart::setGridColor(osg::Vec4 gridColor)
{
    this->gridColor=gridColor;
    refresh();
}

osg::Vec4 Chart::getAxisColor()
{
    return axisColor;
}

void Chart::setAxisColor(osg::Vec4 color)
{
    this->axisColor=color;
    refresh();
}

void Chart::setLocation(float x, float y, float width, float height)
{
    this->x=x;
    this->y=y;
    this->width=width;
    this->height=height;
    refresh();
}
osg::Vec4 Chart::getLocation()
{
    return osg::Vec4(x, y, width, height);
}

int Chart::getNumChannels() const
{
    return static_cast<int>(series.size());
}

void Chart::removeAllChannels()
{
    foreground->removeDrawables(0, foreground->getNumDrawables());
    series.clear();
    activeSerie = -1;
}

int Chart::getActiveSerieIndex() const
{
    return activeSerie;
}


const core::ChartSerie* Chart::getActiveSerie() const
{
    if ( activeSerie < 0 ) {
        return nullptr;
    } else {
        return series[activeSerie];
    }
}

void Chart::setActiveSerie( int currentSerie )
{
    this->activeSerie = currentSerie;
}

osg::ref_ptr<osgText::Text> Chart::createText( const osg::Vec3& pos, float size, const std::string& label )
{
    osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");
    osg::ref_ptr<osgText::Text> text = new osgText::Text();

    text->setColor( labelColor );
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

osg::Vec4 Chart::getLabelColor() const
{
    return labelColor;
}

void Chart::setLabelColor( osg::Vec4 labelColor )
{
    this->labelColor = labelColor;
}


