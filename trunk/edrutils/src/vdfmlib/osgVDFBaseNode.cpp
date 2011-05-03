#include "StdAfx.h"
#include <vdfmlib/osgVDFBaseNode.h>
#include <vdfmlib/osgVDFBaseModel.h>
#include <osgui/Utils2D.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBaseNode::osgVDFBaseNode(dflm::NPtr modelNode, const std::string & nodeName, osgVDFBaseModel * model)
	: osgWidget::Window(nodeName), m_pNode(modelNode), m_pModel(model), m_eStatus(NODE_OK){

        UTILS_ASSERT((modelNode != nullptr, "Nieprawidlowy wezel logiczny"));
}

osgVDFBaseNode::~osgVDFBaseNode(void){

}

dflm::NPtr osgVDFBaseNode::getModelNode() const{
	return m_pNode;
}

osgVDFBaseModel * osgVDFBaseNode::getModel() const{
	return m_pModel;
}

void osgVDFBaseNode::deleteNode()
{
	if(m_pModel == 0){
		throw std::runtime_error("Visual node tries to delete itself without visual model!");
	}

	m_pModel->deleteNodeSelf(this);
}

void osgVDFBaseNode::loggZCoordinates() const{
	std::cout << "Logging node:\t" << this->getName() << std::endl;
	std::cout << "Z coordinates of all elements:" << std::endl;
	std::cout << "node (this window):\t" << osgui::Utils2D::calcAbsZ(this) << std::endl;
}
	
void osgVDFBaseNode::addInPin(osgVDFBasePin * inPin, const std::string & pinName){
	UTILS_ASSERT((inPin != nullptr), "Bledny vizualny pin!");
    
    if(inPin->m_pParentNode != nullptr){
        throw std::runtime_error("Visual input pin already assigned to a node");
    }

	inPin->m_pParentNode = this;
    inPins.insert(inPin);
	graphAddInPin(inPin,pinName);
}

void osgVDFBaseNode::addOutPin(osgVDFBasePin * outPin, const std::string & pinName){
    UTILS_ASSERT((outPin != nullptr), "Bledny vizualny pin!");

    if(outPin->m_pParentNode != nullptr){
        throw std::runtime_error("Visual output pin already assigned to a node");
    }

    outPin->m_pParentNode = this;
    outPins.insert(outPin);
	graphAddOutPin(outPin,pinName);
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