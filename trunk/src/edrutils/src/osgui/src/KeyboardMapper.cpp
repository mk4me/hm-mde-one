#include "PCH.h"
#include <osgui/KeyboardMapper.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

KeyboardMapper::KeyboardMapper()
{
}

KeyboardMapper::~KeyboardMapper()
{
}

void KeyboardMapper::keyPressed(osgGA::GUIEventAdapter::KeySymbol key)
{
	keysPressed.insert(key);
}

void KeyboardMapper::keyReleased(osgGA::GUIEventAdapter::KeySymbol key)
{
	keysPressed.erase(key);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
