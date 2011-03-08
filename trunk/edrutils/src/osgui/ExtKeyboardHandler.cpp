#include "PCH.h"
#include <osgui/ExtKeyboardHandler.h>
#include <osgui/KeyboardMapper.h>

#include <algorithm>
#include <vector>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

ExtKeyboardHandler::ExtKeyboardHandler(osgWidget::WindowManager* wm) : osgWidget::KeyboardHandler(wm)
{
}


ExtKeyboardHandler::~ExtKeyboardHandler(void)
{
}

bool ExtKeyboardHandler::handle(
	const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter& gaa,
	osg::Object* obj,
	osg::NodeVisitor* nv
	){

	osgGA::GUIEventAdapter::KeySymbol key = (osgGA::GUIEventAdapter::KeySymbol)gea.getKey();
	osgGA::GUIEventAdapter::EventType ev = gea.getEventType();
	if(key == -1 || (ev != osgGA::GUIEventAdapter::KEYDOWN && ev != osgGA::GUIEventAdapter::KEYUP)){
		return false;
	}
	
	KeyboardMapper * km = dynamic_cast<KeyboardMapper*>(_wm.get());
	if(km != 0){
		if(ev == osgGA::GUIEventAdapter::KEYDOWN){
			km->keyPressed(key);
		}else if(ev == osgGA::GUIEventAdapter::KEYUP){
			km->keyReleased(key);
		}
	}

	return false;

}

}
