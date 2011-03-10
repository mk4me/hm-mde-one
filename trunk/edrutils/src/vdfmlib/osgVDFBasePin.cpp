#include "StdAfx.h"
#include <vdfmlib/osgVDFBasePin.h>
#include <vdfmlib/osgVDFBaseModel.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBasePin::osgVDFBasePin (const std::string& name, dflm::PinPtr pin, osgVDFBaseModel * model) :
	osgWidget::Widget("\\pin\\"+name), m_pPin(pin), m_bCenterChanged(false), m_pParentNode(0), m_pModel(model) {

    UTILS_ASSERT((pin != nullptr, "Nieprawidlowy pin logiczny"));
    UTILS_ASSERT((model != nullptr, "Nieprawidlowy model logiczny"));
}

osgVDFBasePin::osgVDFBasePin (const osgVDFBasePin& pin, const osg::CopyOp& co) : Widget(pin),
	m_pPin(pin.m_pPin){
}


osgVDFBasePin::~osgVDFBasePin(void)
{
}

dflm::PinPtr osgVDFBasePin::getModelPin() const{
	return m_pPin;
}

osgVDFBaseNode* osgVDFBasePin::getParentNode() const{
	return m_pParentNode;
}

bool osgVDFBasePin::addConnection(osg::Geode * connection, osgVDFBasePin * other){
	bool ret = false;
	if(m_mConnections.find(connection) == m_mConnections.end()) {
		m_mConnections[connection] = other;
		ret = true;
	}

	return ret;
	
}

bool osgVDFBasePin::removeConnection(osg::Geode * connection){
	return m_mConnections.erase(connection) > 0 ? true : false;
}

const osgVDFBasePin::CONNECTIONS_MAP & osgVDFBasePin::getConnections() const{
	return m_mConnections;
}

osgVDFBasePin::PIN_VISUAL_STATUS osgVDFBasePin::getStaticPinVisualStatus() const{
	PIN_VISUAL_STATUS ret = PIN_OK;

	switch(m_pPin->getStaticStatus()){
	case dflm::Pin::PIN_REQUIRED:
		ret = osgVDFBasePin::PIN_REQUIRED;
		break;

	case dflm::Pin::PIN_INCOMPLETE:
		ret = osgVDFBasePin::PIN_INCOMPLETE;
		break;

	case dflm::Pin::PIN_CONNECTED:
		ret = osgVDFBasePin::PIN_CONNECTED;
		break;
	}

	return ret;
}

osgVDFBasePin::PIN_VISUAL_STATUS osgVDFBasePin::getDynamicPinVisualStatus(osgVDFBasePin * refPin, dflm::MPtr model) const{
	return getDynamicPinVisualStatus(refPin->getModelPin(), model);
}

osgVDFBasePin::PIN_VISUAL_STATUS osgVDFBasePin::getDynamicPinVisualStatus(dflm::PinPtr refPin, dflm::MPtr model) const{
	PIN_VISUAL_STATUS ret = PIN_ACTIVE;

	switch(m_pPin->getDynamicStatus(refPin, model)){
	case dflm::Pin::PIN_COMPATIBLE:
		if(m_pPin->getType() == dflm::Pin::PIN_IN && m_pPin->getConnections().empty() == false){
			ret = osgVDFBasePin::PIN_INCOMPATIBLE;
		}/*else{
			ret = osgVDFBasePin::PIN_COMPATIBLE;
		}*/
		break;

	case dflm::Pin::PIN_COMPATIBLE_CYCLE:
	case dflm::Pin::PIN_COMPATIBLE_FULL:
	case dflm::Pin::PIN_INCOMPATIBLE:
		ret = osgVDFBasePin::PIN_INCOMPATIBLE;
		break;

	}
	
	return ret;
}

void osgVDFBasePin::setPinVisualStatus(PIN_VISUAL_STATUS pinVisualStatus){
	if(m_eVisualStatus != pinVisualStatus){
		m_eVisualStatus = pinVisualStatus;
		graphSetPinStatus(pinVisualStatus);
	}
}

osgVDFBasePin::PIN_VISUAL_STATUS osgVDFBasePin::getPinVisualStatus() const{
	return m_eVisualStatus;
}

const osgWidget::XYCoord & osgVDFBasePin::getCenterPosition(){
	if(m_bCenterChanged == true){
		m_tCenterPosition = getParent()->getAbsoluteOrigin() + getOrigin();
		m_tCenterPosition.x() += getWidth() / 2;
		m_tCenterPosition.y() += getHeight() / 2;
		m_bCenterChanged = false;
	}

	return m_tCenterPosition;
}

osgWidget::XYCoord osgVDFBasePin::getCenterPosition() const{
	osgWidget::XYCoord ret = getParent()->getAbsoluteOrigin() + getOrigin();
	ret.x() += getWidth() / 2;
	ret.x() += getHeight() / 2;
	return ret;
}

void osgVDFBasePin::positioned(){
	m_bCenterChanged = true;
	if(m_pModel != 0){
		m_pModel->updatePinsConnections(this);
	}
}

void osgVDFBasePin::managed(osgWidget::WindowManager* wm){
	graphSetPinStatus(m_eVisualStatus);
}

}