
#include "ChartDecoration.h"

std::string ChartDecoration::float2str(float i)
{
	std::stringstream ss;
	std::string temp;
    ss << i;
    ss >> temp;
    return temp;
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
		//    text->setBackdropType(osgText::Text::OUTLINE);
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
				group->addChild(createLabel(pos-posX,11,float2str(actualScale)));
			if(i!=0)
				this->addChild(createLine(osg::Vec3(pos.x(),pos.y()-5,0),
					  osg::Vec3(pos.x(),pos.y()+5,0),
					  osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
			}
			else{
				group->addChild(createLabel(pos-posY,11,float2str(actualScale)));
				if(i!=0)
				this->addChild(createLine(osg::Vec3(pos.x()-5,pos.y(),0),
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
ChartDecoration::ChartDecoration(osg::Vec3 startPoint,float xEnd,float yEnd) : osg::Group()
{

	this->startPoint=startPoint;
	this->xEnd=xEnd;
	this->yEnd=yEnd;
	  color = osg::Vec4(0,0,0, 1.0);
	  ChartDecoration::createBorder(startPoint,xEnd,yEnd);


}
ChartDecoration::ChartDecoration(osg::Vec3 startPoint,float xEnd,float yEnd,osg::Vec4 color) : osg::Group()
{
	

	this->startPoint=startPoint;
	this->xEnd=xEnd;
	this->yEnd=yEnd;
	this->color=color;

}
float ChartDecoration::getScaleX(){
	return ChartDecoration::scaleX;
}
void ChartDecoration::setScaleY(float scale){
	this->scaleY=scale;
	this->addChild(createAxis(startPoint,startPoint+osg::Vec3(0,yEnd,0),4,scaleY));
}
void ChartDecoration::setScaleX(float scale){
	this->scaleX=scale;
	this->addChild(createAxis(startPoint,startPoint+osg::Vec3(xEnd,0,0),8,scaleX));

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

void ChartDecoration::createBorder(osg::Vec3 startPoint,float xEnd,float yEnd){
	int borderSize = 60;


	this->addChild(createLine(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()-borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	this->addChild(createLine(osg::Vec3(startPoint.x()-borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	this->addChild(createLine(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x()-borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	this->addChild(createLine(osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()-borderSize,0),
		osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0),
		osg::Vec4(0.0f,0.0f,0.0f, 1.0f)));
	createGrid(osg::Vec3(startPoint.x()-borderSize,startPoint.y()-borderSize,0),
			   osg::Vec3(startPoint.x() + xEnd+borderSize,startPoint.y()+yEnd+borderSize,0));

}

void ChartDecoration::createGrid(osg::Vec3 startPoint,osg::Vec3 endPoint){
	int odstep=20;
	int xSize=endPoint.x()-startPoint.x();
	int ySize=endPoint.y()-startPoint.y();

	for(int i=odstep;i<ySize;i=i+odstep){
	this->addChild(createLine(osg::Vec3(startPoint.x(),startPoint.y()+i,-1),
		osg::Vec3(endPoint.x(),startPoint.y()+i,-1),
		osg::Vec4(0.0f,0.0f,1.0f, 0.1f)));
	}

	for(int i=odstep;i<xSize;i=i+odstep){
	this->addChild(createLine(osg::Vec3(startPoint.x()+i,startPoint.y(),-1),
		osg::Vec3(startPoint.x()+i,endPoint.y(),-1),
		osg::Vec4(0.0f,0.0f,1.0f, 0.1f)));
	}

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