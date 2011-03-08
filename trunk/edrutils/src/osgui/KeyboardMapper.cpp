#include "PCH.h"
#include <osgui/KeyboardMapper.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

KeyboardMapper::KeyboardMapper(void)
{
}


KeyboardMapper::~KeyboardMapper(void)
{
}

bool KeyboardMapper::isKeyPressed(osgGA::GUIEventAdapter::KeySymbol key){
	return keysPressed.find(key) != keysPressed.end();
}

bool KeyboardMapper::areKeysPressed(const KeyboardMapper::KEYS_SET & keys){
	if(keys.empty() == true || keysPressed.empty() == true || keys.size() > keysPressed.size()){
		return false;
	}

	std::vector<osgGA::GUIEventAdapter::KeySymbol> intersection(keys.size());
	std::vector<osgGA::GUIEventAdapter::KeySymbol>::iterator it = std::set_intersection(keysPressed.begin(), keysPressed.end(), keys.begin(), keys.end(), intersection.begin());

	return std::distance(intersection.begin(), it) == keys.size();
}

const KeyboardMapper::KEYS_SET & KeyboardMapper::getPressedKeys() const{
	return keysPressed;
}

void KeyboardMapper::keyPressed(osgGA::GUIEventAdapter::KeySymbol key){
	keysPressed.insert(key);
}

void KeyboardMapper::keyReleased(osgGA::GUIEventAdapter::KeySymbol key){
	keysPressed.erase(key);
}

}
