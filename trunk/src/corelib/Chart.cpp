#include <plugins/chart/ChartPCH.h>
#include <core/Chart.h>
#include <core/LineChart.h>
#include <core/ChartData.h>
#include <core/ChartPointer.h>

Chart::Chart(int x,int y,int width,int height){
	labelVisable=true;
	gridColor=osg::Vec4(0.0f,0.0f,1.0f, 0.1f);
	color = osg::Vec4(0,0,0, 1.0);
	borderSize=50;
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
osg::Node* Chart::createLine(int x,int y,int x1,int y1,int z,osg::Vec4 lineColor){

	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(x,y,z));
	vertices->push_back(osg::Vec3(x1,y1,z));

	osg::Group* group = new osg::Group;
	osg::Geometry* geom = new osg::Geometry;
	osg::Vec4Array* colors=new osg::Vec4Array();
	colors->push_back(lineColor);


	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	geom->setVertexArray(vertices);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,vertices->size()));

	osg::Geode* geode = new osg::Geode;
	osg::StateSet* ss = geode->getOrCreateStateSet();


	ss->setMode(GL_BLEND, osg::StateAttribute::ON);

	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable(geom);

	group->addChild(geode);


	return group;
}

osg::Group* Chart::createBorder(){


	osg::Group* group=new osg::Group();
	group->addChild(createLine(x,y,
		width,y,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	group->addChild(createLine(x,height,
		width,height,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	group->addChild(createLine(x,y,
		x,height,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	group->addChild(createLine( width,y,
		 width,height,-1,
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));

	return group;
}

osg::Group* Chart::createGrid(){
	osg::Group* group=new Group();

	for(int i=gridDensity;i<height-y;i=i+gridDensity){
		group->addChild(createLine(x,y+i,
			width,y+i,-1,
			gridColor));
	}

	for(int i=gridDensity;i<width-x;i=i+gridDensity){
		group->addChild(createLine(x+i,y,
			x+i,height,-1,
			gridColor));
	}
	return group;

}

osg::Node* Chart::createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps,float scale)
{
	
	osg::Group* group = new osg::Group;
	
	osg::Vec3 dv = e-s;
	dv /= float(numReps-1);
	scale /= float(numReps-1);
	float actualScale=0.0f;
	osg::Vec3 pos = s;
	osg::Vec3 posX = osg::Vec3(0,30,0);
	osg::Vec3 posY = osg::Vec3(40,0,0);
	osg::Vec3Array* vertices = new osg::Vec3Array;

	for(int i=0;i<numReps;++i)
	{
	

		if(e.x()>s.x()){
			if(labelVisable)
			group->addChild(createLabel(pos-posX,fontSize, (formatNumber(actualScale))));
			if(i!=0)
				group->addChild(createLine(pos.x(),pos.y()-5,
				pos.x(),pos.y()+5,0,
				osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
		}
		else{
			if(labelVisable)
			group->addChild(createLabel(pos-posY ,fontSize,(formatNumber(actualScale))));
			if(i!=0)
				group->addChild(createLine(pos.x()-5,pos.y(),
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

	osg::Geode* geode = new osg::Geode;
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable(geom);

	group->addChild(geode);

	return group;
}
osg::Node* Chart::createLabel(const osg::Vec3& pos, float size, const std::string& label)
{
	osg::Geode* geode = new osg::Geode();
	osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	{
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
		geode->addDrawable( text );

	}

	return geode;    
}

osg::Group* Chart::createYAxis(float scale){
	osg::Group* group=new Group();

	group->addChild(createAxis(osg::Vec3(x+borderSize,y+borderSize,0),osg::Vec3(x+borderSize,height-borderSize,0),yNumReps,scale));
	return group;
}
osg::Group* Chart::createXAxis(float scale){
	osg::Group* group=new Group();

	group->addChild( createAxis(osg::Vec3(x+borderSize,y+borderSize,0),osg::Vec3(width-borderSize,y+borderSize,0),xNumReps,scale));
	return group;

}
void Chart::addChartSeries(ChartData* chartData,osg::Vec4 color){
	float yMaxScale=0;
	if(chartData->getRNumber()>0){
	data.push_back(chartData);
	std::vector<ChartData*>::iterator itData= data.end()-1;
	dataSeries.push_back(new LineChart((*itData),x+borderSize,y+borderSize,width-borderSize,height-borderSize,color));
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

	xAxis=createXAxis((*itData)->getScaleX()/(*itData)->getFPS());
	yAxis=createYAxis((*itData)->getScaleY());
	this->addChild(xAxis);
	this->addChild(yAxis);
	}
	
	mainLabel.push_back(createMainLabel((*itDataSeries)->getColor(),(*itData)->getName()));
	std::vector<Group*>::iterator itLabel= mainLabel.end()-1;
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
	this->x=x;
	this->y=y;
	this->width=width;
	this->height=height;
	repaint();
	
}
osg::Vec4 Chart::getLocation(){
	return osg::Vec4(x,y,width,height);
}

void Chart::repaint(){
	osg::Group* newBorder=createBorder();
	osg::Group* newGrid=createGrid();
	osg::Group* newXAxis=createXAxis((*data.begin())->getScaleX()/(*data.begin())->getFPS());
	osg::Group* newYAxis=createYAxis((*data.begin())->getScaleY());


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
	std::vector<Group*>::iterator itLabel= mainLabel.begin();
	std::vector<osg::Group*> newMainLabel;
	for(; itPos < dataSeries.end(); itPos++,dataPos++,itLabel++){
	  (*itPos)->repaint((*dataPos),x+borderSize,y+borderSize,width-borderSize,height-borderSize);
	  newMainLabel.push_back(createMainLabel((*itPos)->getColor(),(*dataPos)->getName()));
	  std::vector<Group*>::iterator itNewLabel= newMainLabel.end()-1;
	  this->replaceChild((*itLabel),  (*itNewLabel)  ); 
	}
mainLabel=newMainLabel;
}


osg::Group* Chart::createMainLabel(osg::Vec4 color, std::string name){
	
osg::Group* group=new Group();
if(labelVisable){
group->addChild(createLabel(osg::Vec3(x+30+labelOffset,height-10,0),12,name));




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

	osg::Geode* geode = new osg::Geode;
	osg::StateSet* ss = geode->getOrCreateStateSet();


	ss->setMode(GL_BLEND, osg::StateAttribute::ON);

	ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->addDrawable(geom);

	group->addChild(geode);

}
return group;
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