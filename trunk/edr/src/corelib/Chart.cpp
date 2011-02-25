#include <plugins/chart/ChartPCH.h>
#include <core/Chart.h>
#include <core/LineChartSerie.h>
#include <core/ChartData.h>
#include <core/ChartPointer.h>

Chart::Chart(int x,int y,int width,int height) : 
margin(0.0f),
showGridX(false),
showGridY(true)
{
	showLabel=true;
	gridColor=osg::Vec4(0.0f,0.0f,1.0f, 0.1f);
	color = osg::Vec4(0,0,0, 1.0);
	gridDensity=10;
	xNumReps=5;
	yNumReps=3;
	fontSize=11;
	labelOffset=0;
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	init();

	
}
Chart::~Chart(){

}

void Chart::init(){


    background = new osg::Geode();
    background->setName("background");
	
	//grid=refreshGrid();

	
    addChild(background);
	//this->addChild(border);
	
	//this->addChild(grid);
	

}
osg::Geometry* Chart::createLine(int x,int y,int x1,int y1,int z,osg::Vec4 lineColor){

	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x,y,z));
	vertices->push_back(osg::Vec3(x1,y1,z));

	
	osg::Geometry* geom = new osg::Geometry;
	osg::Vec4Array* colors=new osg::Vec4Array();
	colors->push_back(lineColor);


	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	geom->setVertexArray(vertices);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,vertices->size()));

	geom->setName("line");
	return geom;
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

void Chart::refreshAxis()
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

void Chart::refreshGrid()
{
    // czy w ogóle jest coœ do pkazania?
    if ( prepareGeometry(grid, !showGridX && !showGridY, "grid" ) ) {
        // wierzcho³ki
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(grid->getVertexArray());
        vertices->resize(0);
        for ( int i = gridDensity; showGridY && i < height - y; i += gridDensity ) {
            vertices->push_back(osg::Vec3(x,        y+i,    -1.0f));
            vertices->push_back(osg::Vec3(width,    y+i,    -1.0f));
        }
        for(int i=gridDensity; showGridX && i<width-x;i=i+gridDensity) {
            vertices->push_back(osg::Vec3(x+i,      y,      -1.0f));
            vertices->push_back(osg::Vec3(x+i,      height, -1.0f));
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

osg::Geode* Chart::createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps,float scale,std::string unit)
{
	

	osg::Geode* geode = new osg::Geode;
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	osg::Vec3 dv = e-s;
	dv /= float(numReps-1);
	scale /= float(numReps-1);
	float actualScale=0.0f;
	osg::Vec3 pos = s;
	osg::Vec3 posX = osg::Vec3(0,30,0);
	osg::Vec3 posY = osg::Vec3(5,0,0);
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(s);
	vertices->push_back(e);
	for(int i=0;i<numReps;++i)
	{
	

		if(e.x()>s.x()){
			if(showLabel){
				osgText::Text* text=createLabel(pos-posX,fontSize, (formatNumber(actualScale))+unit);
				text->setAlignment(osgText::Text::RIGHT_CENTER);
				geode->addDrawable(text);}
			if(i!=0){
				vertices->push_back(osg::Vec3(pos.x(),pos.y()-5,1));
				vertices->push_back(osg::Vec3(pos.x(),pos.y()+5,1));
			}
				//geode->addDrawable(createLine(pos.x(),pos.y()-5,}
				//pos.x(),pos.y()+5,0,
				//osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
		}
		else{
			if(showLabel){
				osgText::Text* text=createLabel(pos ,fontSize,(formatNumber(actualScale))+unit);
				text->setAlignment(osgText::Text::RIGHT_CENTER);
					geode->addDrawable(text);}
				if(i!=0){
				vertices->push_back(osg::Vec3(pos.x()-5,pos.y(),1));
				vertices->push_back(osg::Vec3(pos.x()+5,pos.y(),1));
			}
		}
		
		pos += dv;
		actualScale += scale;

	}



	osg::Geometry* geom = new osg::Geometry;
	osg::Vec4Array* colors=new osg::Vec4Array();
	colors->push_back(color);

 
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->setVertexArray(vertices);

 geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,vertices->getNumElements())); 

	
	geode->addDrawable(geom);
	geode->setName("axis");


	return geode;
}
osgText::Text* Chart::createLabel(const osg::Vec3& pos, float size, const std::string& label)
{

	osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");

	
		osgText::Text* text = new  osgText::Text;

		text->setColor(color);
		text->setFontResolution(100,100);
		text->setPosition(pos);
		text->setFont(font);
		text->setCharacterSize(size);
		text->setAxisAlignment(osgText::Text::SCREEN); 
		text->setAlignment(osgText::Text::LEFT_CENTER);
		text->setText(label);
		text->setLayout(osgText::Text::LEFT_TO_RIGHT);
		text->setBoundingBoxMargin(0.25f);
		

	

	return text;    
}

void Chart::addChannel( const core::ScalarChannelPtr& channel, osg::Vec4 color )
{
    // tworzymy nowy wykres
    osg::ref_ptr<core::LineChartSerie> serie = new core::LineChartSerie();

    serie->setData(channel);
    serie->setColor(color);
    serie->setName(channel->getName());
    
    // dodajemy do listy i do dzieci
    series.push_back(serie);
    addChild(serie);
}

// 
// void Chart::deprecated_addChartSeries(deprecated_ChartData* chartData,osg::Vec4 color){
// 	xNumReps=2;
// 	yNumReps=2;
// 	borderSize=5;
// 	showLabel=false;
// 	showBorder=false;
// 	float yMaxScale=0;
// 	if(chartData->getRNumber()>0){
// 	deprecated_data.push_back(chartData);
// 	std::vector<deprecated_ChartData*>::iterator itData= deprecated_data.end()-1;
// 	deprecated_dataSeries.push_back(new deprecated_LineChart((*itData),x+borderSize,y+borderSize,width-borderSize,height-borderSize,color));
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
// 	mainLabel.push_back(createMainLabel((*itDataSeries)->getColor(),(*itData)->getName()));
// 	std::vector<osg::Geode*>::iterator itLabel= mainLabel.end()-1;
// 	this->addChild(*itLabel);
// 	labelOffset+=100;
// 	}
// 
// }
// 
// void Chart::deprecated_addChartPreviewSeries(deprecated_ChartData* chartData,osg::Vec4 color){
// 	float yMaxScale=0;
// 	if(chartData->getRNumber()>0){
// 	deprecated_data.push_back(chartData);
// 	std::vector<deprecated_ChartData*>::iterator itData= deprecated_data.end()-1;
// 	deprecated_dataSeries.push_back(new deprecated_LineChart((*itData),x+borderSize*2,y+borderSize,width-borderSize*2,height-borderSize,color));
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
// 	mainLabel.push_back(createMainLabel((*itDataSeries)->getColor(),(*itData)->getName()));
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

std::string Chart::formatNumber( float number )
{
    char s[100];
    sprintf(s,"%.2f",number);

    return s;
}

void Chart::updatePointer(double targetTime){
// 	std::vector<deprecated_LineChart*>::iterator itPos = deprecated_dataSeries.begin();
// 
//     for(; itPos < deprecated_dataSeries.end(); itPos++)
//         (*itPos)->getPointer()->update(targetTime);
}

int Chart::getFontSize(){
	return fontSize;
}

void Chart::setFontSize(int fontSize){
	this->fontSize=fontSize;
	refresh();
}


int Chart::getGridDensity(){
	return gridDensity;
}
void Chart::setGridDensity(int gridDensity){
	this->gridDensity=gridDensity;
	refresh();
}
osg::Vec4 Chart::getGridColor(){
	return gridColor;
}
void Chart::setGridColor(osg::Vec4 gridColor){
	this->gridColor=gridColor;
	refresh();
}
osg::Vec4 Chart::getColor(){
	return color;
}
void Chart::setColor(osg::Vec4 color){
	this->color=color;
	refresh();
}

int Chart::getXNumReps(){
return xNumReps;
}
	
void Chart::setXNumReps(int xNumReps){
	this->xNumReps=xNumReps;
	refresh();
}

int Chart::getYNumReps(){
return yNumReps;
}

void Chart::setYNumReps(int yNumReps){
this->yNumReps=yNumReps;
refresh();
}

void Chart::setLocation(int x,int y,int width,int height){
	if(((x+width)<margin)||(y+height)<margin){
		//throw std::runtime_error("plik c3d nie posiada wymaganych przez wykresy danych");
	}else{
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	refresh();}
	
}
osg::Vec4 Chart::getLocation(){
	return osg::Vec4(x,y,width,height);
}

void Chart::refresh()
{
    //getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON );
    refreshGrid();

    BOOST_FOREACH( core::ChartSeriePtr serie, series ) {
        serie->setLocation( x + margin, y + margin, width-margin*2, height-margin*2 );
        serie->refresh();
    }

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
// 	    newMainLabel.push_back(createMainLabel((*itPos)->getColor(), (*itPos)->getName()));
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
// 	  newMainLabel.push_back(createMainLabel((*itPos)->getColor(),(*dataPos)->getName()));
// 	  std::vector<osg::Geode*>::iterator itNewLabel= newMainLabel.end()-1;
// 	  this->replaceChild((*itLabel),  (*itNewLabel)  ); 
// 	}
// 	mainLabel=newMainLabel;
}


osg::Geode* Chart::createMainLabel(osg::Vec4 color, std::string name){
		osg::Geode* geode = new osg::Geode;
		this->setName(name);
			geode->setName("label");
	if(showLabel){
	osg::StateSet* ss = geode->getOrCreateStateSet();


	ss->setMode(GL_BLEND, osg::StateAttribute::ON);

	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable(createLabel(osg::Vec3(x+30+labelOffset,height-10,0),12,name));
	
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x+10+labelOffset,height-20+5,0));
	vertices->push_back(osg::Vec3(x+20+labelOffset,height-20+5,0));
	vertices->push_back(osg::Vec3(x+20+labelOffset,height-20+15,0));
	vertices->push_back(osg::Vec3(x+10+labelOffset,height-20+15,0));
	vertices->push_back(osg::Vec3(x+10+labelOffset,height-20+5,0));

	
	osg::Geometry* geom = new osg::Geometry;
	osg::Vec4Array* colors=new osg::Vec4Array();
	colors->push_back(color);


	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	geom->setVertexArray(vertices);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,vertices->size()));


	geode->addDrawable(geom);

	

}
return geode;
}

void Chart::setShowLabel(bool showLabel){
	this->showLabel=showLabel;
//     std::vector<deprecated_LineChart*>::iterator itPos = deprecated_dataSeries.begin();
// 	std::vector<deprecated_ChartData*>::iterator dataPos = deprecated_data.begin();
// 	for(; itPos < deprecated_dataSeries.end(); itPos++,dataPos++){
// 		(*itPos)->setShowLabel(showLabel);
// 	}
	refresh();
}

void Chart::setShowBorder(bool showBorder){
	this->showBorder=showBorder;
	refresh();
}
bool Chart::isShowingBorder(){
	return showBorder;
}

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

