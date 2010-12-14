#include <plugins/chart/ChartPCH.h>
#include <core/Chart.h>
#include <core/LineChart.h>
#include <core/ChartData.h>
#include <core/ChartPointer.h>

Chart::Chart(int x,int y,int width,int height){
	labelVisable=true;
	gridColor=osg::Vec4(0.0f,0.0f,1.0f, 0.1f);
	color = osg::Vec4(0,0,0, 1.0);
	borderSize=40;
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
	border=createBorder();
	
	grid=createGrid();

	

	this->addChild(border);
	
	this->addChild(grid);
	

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

	
	return geom;
}

osg::Geode* Chart::createBorder(){


	osg::Geode* geode=new osg::Geode();
	geode->addDrawable(createLine(x,y,
		width,y,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	geode->addDrawable(createLine(x,height,
		width,height,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	geode->addDrawable(createLine(x,y,
		x,height,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	geode->addDrawable(createLine( width,y,
		 width,height,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));

	return geode;
}

osg::Geode* Chart::createGrid(){


	
        osg::Geometry* geom = new osg::Geometry;
		osg::Vec3Array* vertices = new osg::Vec3Array;
		for(int i=gridDensity;i<height-y;i=i+gridDensity)
	        {
				
	
				vertices->push_back(osg::Vec3(x,y+i,-1.0f));
				vertices->push_back(osg::Vec3(width,y+i,-1.0f));
	        }
		for(int i=gridDensity;i<width-x;i=i+gridDensity)
	        {
				vertices->push_back(osg::Vec3(x+i,y,-1.0f));
				vertices->push_back(osg::Vec3(x+i,height,-1.0f));
	
	        }
		   geom->setVertexArray(vertices);
	
	     osg::Vec4Array* colors=new osg::Vec4Array();
		 colors->push_back(gridColor);

	
		geom->setColorArray(colors);
		geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	
		    geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0,vertices->getNumElements())); 

		    geom->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	
	        osg::Geode* geode = new osg::Geode;
	        geode->addDrawable(geom);
	               
	     
	return geode;

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
	
	for(int i=0;i<numReps;++i)
	{
	

		if(e.x()>s.x()){
			if(labelVisable){
				osgText::Text* text=createLabel(pos-posX,fontSize, (formatNumber(actualScale))+unit);
				text->setAlignment(osgText::Text::RIGHT_CENTER);
				geode->addDrawable(text);}
			if(i!=0)
				geode->addDrawable(createLine(pos.x(),pos.y()-5,
				pos.x(),pos.y()+5,0,
				osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
		}
		else{
			if(labelVisable){
				osgText::Text* text=createLabel(pos ,fontSize,(formatNumber(actualScale))+unit);
				text->setAlignment(osgText::Text::RIGHT_CENTER);
					geode->addDrawable(text);}
			if(i!=0)
				geode->addDrawable(createLine(pos.x()-5,pos.y(),
				pos.x()+5,pos.y(),0,
				osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
		}
		vertices->push_back(pos);
		pos += dv;
		actualScale += scale;

	}



	osg::Geometry* geom = new osg::Geometry;
	osg::Vec4Array* colors=new osg::Vec4Array();
	colors->push_back(color);

 
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	geom->setVertexArray(vertices);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,vertices->size()));

	
	geode->addDrawable(geom);



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


void Chart::addChartSeries(ChartData* chartData,osg::Vec4 color){
	xNumReps=2;
	yNumReps=2;
	borderSize=5;
	labelVisable=false;
	float yMaxScale=0;
	if(chartData->getRNumber()>0){
	data.push_back(chartData);
	std::vector<ChartData*>::iterator itData= data.end()-1;
	dataSeries.push_back(new LineChart((*itData),x+borderSize,y+borderSize,width-borderSize,height-borderSize,color));
	std::vector<LineChart*>::iterator itDataSeries = dataSeries.end()-1;
	(*itDataSeries)->setLabelVisable(labelVisable);
	this->addChild(*itDataSeries);
	
	if(data.size()>1){
		 itData=data.begin();
		 for(; itData < data.end(); itData++){
			 if(yMaxScale<(*itData)->getScaleY()){
				 yMaxScale=(*itData)->getScaleY();
			 }

		 }
		 itData=data.begin();
		 for(; itData < data.end(); itData++){
			 (*itData)->setScaleY(yMaxScale);
			 (*itData)->normalize();
		 }
		repaint(); 
	}else{

	xAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,(*data.begin())->getScaleX()/(*data.begin())->getFPS(),"s");
	yAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,(*data.begin())->getScaleY(),(*data.begin())->getUnit());
	this->addChild(xAxis);
	this->addChild(yAxis);
	}
	
	mainLabel.push_back(createMainLabel((*itDataSeries)->getColor(),(*itData)->getName()));
	std::vector<osg::Geode*>::iterator itLabel= mainLabel.end()-1;
	this->addChild(*itLabel);
	labelOffset+=100;
	}

}
void Chart::addChartPreviewSeries(ChartData* chartData,osg::Vec4 color){
	float yMaxScale=0;
	if(chartData->getRNumber()>0){
	data.push_back(chartData);
	std::vector<ChartData*>::iterator itData= data.end()-1;
	dataSeries.push_back(new LineChart((*itData),x+borderSize*2,y+borderSize,width-borderSize*2,height-borderSize,color));
	std::vector<LineChart*>::iterator itDataSeries = dataSeries.end()-1;
	this->addChild(*itDataSeries);
	
	if(data.size()>1){
		 itData=data.begin();
		 for(; itData < data.end(); itData++){
			 if(yMaxScale<(*itData)->getScaleY()){
				 yMaxScale=(*itData)->getScaleY();
			 }

		 }
		 itData=data.begin();
		 for(; itData < data.end(); itData++){
			 (*itData)->setScaleY(yMaxScale);
			 (*itData)->normalize();
		 }
		repaint(); 
	}else{

	xAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,(*data.begin())->getScaleX()/(*data.begin())->getFPS(),"s");
	yAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,(*data.begin())->getScaleY(),(*data.begin())->getUnit());
	this->addChild(xAxis);
	this->addChild(yAxis);
	}
	
	mainLabel.push_back(createMainLabel((*itDataSeries)->getColor(),(*itData)->getName()));
	std::vector<osg::Geode*>::iterator itLabel= mainLabel.end()-1;
	this->addChild(*itLabel);
	labelOffset+=100;
	}

}
std::string Chart::formatNumber( float number )
{
 char s[100];
        sprintf(s,"%.2f",number);

  return s;
}
int Chart::getFPS(){
	if(data.size()>0)
return (*data.begin())->getFPS();
	else 
		return 0;
}
int Chart::getFrameNumber(){
	if(data.size()>0)
return (*data.begin())->getRNumber();
	else
		return 0;
}

void Chart::updatePointer(double targetTime){
	std::vector<LineChart*>::iterator itPos = dataSeries.begin();

  for(; itPos < dataSeries.end(); itPos++)
	  (*itPos)->getPointer()->update(targetTime);
}

int Chart::getFontSize(){
	return fontSize;
}

void Chart::setFontSize(int fontSize){
	this->fontSize=fontSize;
	repaint();
}
int Chart::getBorderSize(){
	return borderSize;
}

void Chart::setBorderSize(int borderSize){
	this->borderSize=borderSize;
	repaint();
}
int Chart::getGridDensity(){
	return gridDensity;
}
void Chart::setGridDensity(int gridDensity){
	this->gridDensity=gridDensity;
	repaint();
}
osg::Vec4 Chart::getGridColor(){
	return gridColor;
}
void Chart::setGridColor(osg::Vec4 gridColor){
	this->gridColor=gridColor;
	repaint();
}
osg::Vec4 Chart::getColor(){
	return color;
}
void Chart::setColor(osg::Vec4 color){
	this->color=color;
	repaint();
}

int Chart::getXNumReps(){
return xNumReps;
}
	
void Chart::setXNumReps(int xNumReps){
	this->xNumReps=xNumReps;
	repaint();
}

int Chart::getYNumReps(){
return yNumReps;
}

void Chart::setYNumReps(int yNumReps){
this->yNumReps=yNumReps;
repaint();
}

void Chart::setLocation(int x,int y,int width,int height){
	if(((x+width)<borderSize)||(y+height)<borderSize){
		//throw std::runtime_error("plik c3d nie posiada wymaganych przez wykresy danych");
	}else{
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	repaint();}
	
}
osg::Vec4 Chart::getLocation(){
	return osg::Vec4(x,y,width,height);
}

void Chart::repaint(){
	osg::Geode* newBorder=createBorder();
	osg::Geode* newGrid=createGrid();
	osg::Geode* newXAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(width-borderSize*2,y+borderSize,0),xNumReps,(*data.begin())->getScaleX()/(*data.begin())->getFPS(),"s");
	osg::Geode* newYAxis=createAxis(osg::Vec3(x+borderSize*2,y+borderSize,0),osg::Vec3(x+borderSize*2,height-borderSize,0),yNumReps,(*data.begin())->getScaleY(),(*data.begin())->getUnit());


	this->replaceChild(border,newBorder);
	this->replaceChild(grid,newGrid);
	this->replaceChild(xAxis,newXAxis);
	this->replaceChild(yAxis,newYAxis);

	border=newBorder;
	grid=newGrid;
	xAxis=newXAxis;
	yAxis=newYAxis;
	std::vector<LineChart*>::iterator itPos = dataSeries.begin();
	std::vector<ChartData*>::iterator dataPos = data.begin();
	std::vector<osg::Geode*>::iterator itLabel= mainLabel.begin();
	std::vector<osg::Geode*> newMainLabel;
	for(; itPos < dataSeries.end(); itPos++,dataPos++,itLabel++){
	  (*itPos)->repaint((*dataPos),x+borderSize*2,y+borderSize,width-borderSize*2,height-borderSize);
	  newMainLabel.push_back(createMainLabel((*itPos)->getColor(),(*dataPos)->getName()));
	  std::vector<osg::Geode*>::iterator itNewLabel= newMainLabel.end()-1;
	  this->replaceChild((*itLabel),  (*itNewLabel)  ); 
	}
	mainLabel=newMainLabel;
}


osg::Geode* Chart::createMainLabel(osg::Vec4 color, std::string name){
		osg::Geode* geode = new osg::Geode;

//if(labelVisable){
	
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

	

//}
return geode;
}

void Chart::setLabelVisable(bool labelVisable){
	this->labelVisable=labelVisable;
std::vector<LineChart*>::iterator itPos = dataSeries.begin();
	std::vector<ChartData*>::iterator dataPos = data.begin();
	for(; itPos < dataSeries.end(); itPos++,dataPos++){
		(*itPos)->setLabelVisable(labelVisable);
	}
	repaint();
}