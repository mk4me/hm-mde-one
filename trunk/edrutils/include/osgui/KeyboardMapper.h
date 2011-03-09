#ifndef HEADER_GUARD__KEYBOARDMAPPER_H__
#define HEADER_GUARD__KEYBOARDMAPPER_H__

#include <osgWidget/ViewerEventHandlers>
#include <set>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class ExtKeyboardHandler;

class KeyboardMapper
{
public:

	typedef std::set<osgGA::GUIEventAdapter::KeySymbol> KEYS_SET;

public:
	KeyboardMapper(void);
	~KeyboardMapper(void);

	bool isKeyPressed(osgGA::GUIEventAdapter::KeySymbol key);
	bool areKeysPressed(const KEYS_SET & keys);

	const KEYS_SET & getPressedKeys() const;

protected:

	friend class ExtKeyboardHandler;

protected:

	void keyPressed(osgGA::GUIEventAdapter::KeySymbol key);
	void keyReleased(osgGA::GUIEventAdapter::KeySymbol key);

protected:
	KEYS_SET keysPressed;
};

}

#endif
