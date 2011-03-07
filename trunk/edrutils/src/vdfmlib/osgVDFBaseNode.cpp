#include "StdAfx.h"
#include <vdfmlib/osgVDFBaseNode.h>
#include <vdfmlib/osgVDFBaseModel.h>
#include <vdfmlib/osgUI2DUtils.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBaseNode::osgVDFBaseNode(dflm::NPtr modelNode, const std::string & nodeName, osgVDFBaseModel * model)
	: osgWidget::Window(nodeName), m_pNode(modelNode), m_pModel(model), m_eStatus(NODE_OK){

}

osgVDFBaseNode::~osgVDFBaseNode(void){

}

dflm::NPtr osgVDFBaseNode::getModelNode() const{
	return m_pNode;
}

osgVDFBaseModel * osgVDFBaseNode::getModel() const{
	return m_pModel;
}

bool osgVDFBaseNode::deleteNode(){
	if(m_pModel != 0){
		return m_pModel->deleteNodeSelf(this);
	}

	return false;
}

void osgVDFBaseNode::loggZCoordinates() const{
	std::cout << "Logging node:\t" << this->getName() << std::endl;
	std::cout << "Z coordinates of all elements:" << std::endl;
	std::cout << "node (this window):\t" << osgUI2DUtils::calcZ(this) << std::endl;
}
	
bool osgVDFBaseNode::addInPin(osgVDFBasePin * inPin, const std::string & pinName){
	if(inPin->m_pParentNode == 0 && inPins.insert(inPin).second == true){
		inPin->m_pParentNode = this;
		graphAddInPin(inPin,pinName);
		return true;
	}

	return false;
}

bool osgVDFBaseNode::addOutPin(osgVDFBasePin * outPin, const std::string & pinName){
	if(outPin->m_pParentNode == 0 && outPins.insert(outPin).second == true){
		outPin->m_pParentNode = this;
		graphAddOutPin(outPin,pinName);
		return true;
	}

	return false;
}

const osgVDFBaseNode::VISUAL_PIN_SET & osgVDFBaseNode::getInPins() const {
	return inPins;
}
const osgVDFBaseNode::VISUAL_PIN_SET & osgVDFBaseNode::getOutPins() const {
	return outPins;
}

void osgVDFBaseNode::setNodeVisualStatus(osgVDFBaseNode::NODE_VISUAL_STATUS nodeVisualStatus){
	if(nodeVisualStatus != m_eStatus){
		m_eStatus = nodeVisualStatus;
		graphSetNodeStatus(m_eStatus);
	}
}

osgVDFBaseNode::NODE_VISUAL_STATUS osgVDFBaseNode::getNodeVisualStatus() const{
	return m_eStatus;
}

}