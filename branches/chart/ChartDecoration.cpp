
#include "ChartDecoration.h"

std::string ChartDecoration::formatNumber( float number )
{
 char s[100];
        sprintf(s,"%.2f",number);

  return s;
}


osg::Node* ChartDecoration::createLabel(const osg::Vec3& pos, float size, const std::string& label)
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
		text->setAlignment(osgText::Text::CENTER_CENTER);
		text->setText(label);
		text->setLayout(osgText::Text::LEFT_TO_RIGHT);
		text->setBoundingBoxMargin(0.25f);
		geode->addDrawable( text );

	}

	return geode;    
}

osg::Node* ChartDecoration::createAxis(const osg::Vec3& s, const osg::Vec3& e, int numReps, float scale)
{

	osg::Group* group = new osg::Group;

	osg::Vec3 dv = e-s;
	dv /= float(numReps-1);
	scale /= float(numReps-1);
	float actualScale=0.0f;
	osg::Vec3 pos = s;
	osg::Vec3 posX = osg::Vec3(0,30,0);
	osg::Vec3 posY = osg::Vec3(30,0,0);
	osg::Vec3Array* vertices = new osg::Vec3Array;

	for(int i=0;i<numReps;++i)
	{
	

		if(e.x()>s.x()){
			group->addChild(createLabel(pos-posX,11, (formatNumber(actualScale/data->getFPS()))));
			if(i!=0)
				group->addChild(createLine(osg::Vec3(pos.x(),pos.y()-5,0),
				osg::Vec3(pos.x(),pos.y()+5,0),
				osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
		}
		else{
			group->addChild(createLabel(pos-posY ,11,(formatNumber(actualScale))));
			if(i!=0)
				group->addChild(createLine(osg::Vec3(pos.x()-5,pos.y(),0),
				osg::Vec3(pos.x()+5,pos.y(),0),
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
ChartDecoration::ChartDecoration(osg::Vec3 startPoint,float xEnd,float yEnd,ChartData* data) : osg::Group()
{
	xNumReps=5;
	yNumReps=3;
	gridDensity=10;
	borderSize = 40;
	this->startPoint=osg::Vec3(startPoint.x()+borderSize,startPoint.y()+borderSize,0);
	this->xEnd=xEnd-2*borderSize;
	this->yEnd=yEnd-2*borderSize;
	this->data=data;
	color = osg::Vec4(0,0,0, 1.0);
	gridColor=osg::Vec4(0.0f,0.0f,1.0f, 0.1f);


	init();
}
void ChartDecoration::init(){
	border = createBorder();	
	xAxis=setScaleX(data->getScaleX());
	yAxis=setScaleY(data->getScaleY());
	grid=createGrid(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0));

	this->addChild(xAxis);
	this->addChild(yAxis);
	this->addChild(border);
	this->addChild(grid);
}
float ChartDecoration::getScaleX(){
	return ChartDecoration::scaleX;
}
osg::Group* ChartDecoration::setScaleY(float scale){
	osg::Group* group=new Group();
	this->scaleY=scale;
	group->addChild(createAxis(startPoint,startPoint+osg::Vec3(0,yEnd,0),yNumReps,scaleY));
	return group;
}
osg::Group* ChartDecoration::setScaleX(float scale){
	osg::Group* group=new Group();
	this->scaleX=scale;
	group->addChild( createAxis(startPoint,startPoint+osg::Vec3(xEnd,0,0),xNumReps,scaleX));
	return group;

}
float ChartDecoration::getScaleY(){
	return ChartDecoration::scaleY;
}
osg::Vec3 ChartDecoration::getStartPoint(){
	return ChartDecoration::startPoint;
}
osg::Vec3 ChartDecoration::getEndPoint(){
	return osg::Vec3(startPoint.x()+getXEnd(),startPoint.y()+getYEnd(),0);
}
float ChartDecoration::getXEnd(){
	return ChartDecoration::xEnd;
}
float ChartDecoration::getYEnd(){
	return ChartDecoration::yEnd;
}

osg::Group* ChartDecoration::createBorder(){


	osg::Group* group=new osg::Group();
	group->addChild(createLine(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()-borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	group->addChild(createLine(osg::Vec3(startPoint.x()-borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	group->addChild(createLine(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x()-borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	group->addChild(createLine(osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));

	return group;
}

osg::Group* ChartDecoration::createGrid(osg::Vec3 startPoint,osg::Vec3 endPoint){
	osg::Group* group=new Group();
	int xSize=endPoint.x()-startPoint.x();
	int ySize=endPoint.y()-startPoint.y();


	for(int i=gridDensity;i<ySize;i=i+gridDensity){
		group->addChild(createLine(osg::Vec3(startPoint.x(),startPoint.y()+i,-1),
			osg::Vec3(endPoint.x(),startPoint.y()+i,-1),
			gridColor));
	}

	for(int i=gridDensity;i<xSize;i=i+gridDensity){
		group->addChild(createLine(osg::Vec3(startPoint.x()+i,startPoint.y(),-1),
			osg::Vec3(startPoint.x()+i,endPoint.y(),-1),
			gridColor));
	}
	return group;

}
osg::Node* ChartDecoration::createLine(osg::Vec3 startPoint,osg::Vec3 endPoint,osg::Vec4 lineColor){

	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back(startPoint);
	vertices->push_back(endPoint);

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
int ChartDecoration::getFontSize(){
	return fontSize;
}

void ChartDecoration::setFontSize(int fontSize){
	this->fontSize=fontSize;
}
int ChartDecoration::getBorderSize(){
	return borderSize;
}

void ChartDecoration::setBorderSize(int borderSize){
	this->borderSize=borderSize;
}
int ChartDecoration::getGridDensity(){
	return gridDensity;
}
void ChartDecoration::setGridDensity(int gridDensity){
	this->gridDensity=gridDensity;
}
osg::Vec4 ChartDecoration::getGridColor(){
	return gridColor;
}
void ChartDecoration::setGridColor(osg::Vec4 gridColor){
	this->gridColor=gridColor;
}
osg::Vec4 ChartDecoration::getColor(){
	return color;
}
void ChartDecoration::setColor(osg::Vec4 color){
	this->color=color;
}
void ChartDecoration::setXEnd(float xEnd){
	this->xEnd=xEnd-2*borderSize;
}
void ChartDecoration::setYEnd(float yEnd){
	this->yEnd=yEnd-2*borderSize;
}
void ChartDecoration::repaint(){
	osg::Group* newBorder=createBorder();
	osg::Group* newGrid=createGrid(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0));
	osg::Group* newXAxis=setScaleX(data->getScaleX());
	osg::Group* newYAxis=setScaleY(data->getScaleY());

	this->replaceChild(border,newBorder);
	this->replaceChild(grid,newGrid);
	this->replaceChild(xAxis,newXAxis);
	this->replaceChild(yAxis,newYAxis);

	border=newBorder;
	grid=newGrid;
	xAxis=newXAxis;
	yAxis=newYAxis;
}
int ChartDecoration::getXNumReps(){
return xNumReps;
}
	
void ChartDecoration::setXNumReps(int xNumReps){
	this->xNumReps=xNumReps;
}

int ChartDecoration::getYNumReps(){
return yNumReps;
}

void ChartDecoration::setYNumReps(int yNumReps){
this->yNumReps=yNumReps;
}
void ChartDecoration::setStartPoint(osg::Vec3 startPoint){
	this->startPoint=osg::Vec3(startPoint.x()+borderSize,startPoint.y()+borderSize,0);
}

void ChartDecoration::setLocation(int x,int y,int width,int height){
	this->setStartPoint(osg::Vec3(x,y,0));
	this->setXEnd(width);
	this->setYEnd(height);
	
}