#include "PointerUpdateCallback.h"

void PointerUpdateCallback::operator ()(osg::Node* node, osg::NodeVisitor* nv)
{ 
	node->asTransform()->asMatrixTransform()->setMatrix(osg::Matrix::translate(data->getNormalizedXValue(actualPoint)*endPoint,0.0f,0.0f));
	if(actualPoint<data->getRNumber()-1){
		actualPoint++;
		text->setText(float2str(data->getValue(actualPoint)-data->getYMin()));
	}

}
PointerUpdateCallback::PointerUpdateCallback(osgText::Text* text,ChartData* data,float endPoint){
	actualPoint=0;
	this->endPoint=endPoint;
	this->text=text;
	this->text->setDataVariance(osg::Object::DYNAMIC);
	this->data=data;
}
std::string PointerUpdateCallback::float2str(float i)
{
	std::stringstream ss;
	std::string temp;
	ss << i;
	ss >> temp;
	return temp;
}