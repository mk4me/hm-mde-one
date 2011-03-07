#ifndef __HEADER_GUARD__VDFPIN_H__
#define __HEADER_GUARD__VDFPIN_H__

#include <vdfmlib/osgVDFBasePin.h>
#include <osgDB/ReadFile>
#include <vdfmlib/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

template<class ConnectionFactory>
class osgVDFPin : public osgVDFBasePin
{
public:
	META_Widget(osgWidget::Widget, osgVDFPin);

	osgVDFPin (const std::string& name = "", dflm::PinPtr pin = dflm::PinPtr(), osgVDFBaseModel * model = 0);
    osgVDFPin (const osgVDFPin& pin, const osg::CopyOp& co);

	~osgVDFPin(void);

protected:
	virtual void graphSetPinStatus(PIN_VISUAL_STATUS pinVisualStatus);
};

template<class ConnectionFactory>
osgVDFPin<ConnectionFactory>::osgVDFPin(const std::string& name, dflm::PinPtr pin, osgVDFBaseModel * model)
	: osgVDFBasePin(name, pin, model){

	setMinimumSize(13.0f, 12.0f);
	setSize(13.0f, 12.0f);
}

template<class ConnectionFactory>  
osgVDFPin<ConnectionFactory>::osgVDFPin (const osgVDFPin& pin, const osg::CopyOp& co)
	: osgVDFBasePin(pin, co){

}

template<class ConnectionFactory>  
osgVDFPin<ConnectionFactory>::~osgVDFPin(void){

}

template<class ConnectionFactory>
void osgVDFPin<ConnectionFactory>::graphSetPinStatus(osgVDFBasePin::PIN_VISUAL_STATUS pinVisualStatus){
	std::string status;
	std::string pinType;

	switch(this->getModelPin()->getType()){
	case dflm::Pin::PIN_IN:
		pinType = "in";
		break;

	case dflm::Pin::PIN_OUT:
		pinType = "out";
		break;
	}

	switch(pinVisualStatus){
	case osgVDFBasePin::PIN_ACTIVE:
		status = "active";
		break;

	case osgVDFBasePin::PIN_CONNECTED:
		status = "connected";
		break;

	case osgVDFBasePin::PIN_INCOMPATIBLE:
		status = "incompatible";
		break;

	case osgVDFBasePin::PIN_INCOMPLETE:
		status = "incomplete";
		break;

	case osgVDFBasePin::PIN_OK:
		status = "ok";
		break;

	case osgVDFBasePin::PIN_REQUIRED:
		status = "required";
		break;
	}

	this->setStyle("vdf.pin." + pinType + "." + status);

	osgWidget::WindowManager* wm = this->getWindowManager();
	if(wm != 0){
		osgWidget::StyleManager* sm = wm->getStyleManager();
		if(sm != 0){
			sm->applyStyles(this);
		}
	}
}

}

#endif