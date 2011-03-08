#ifndef __HEADER_GUARD__EXTKEYBOARDHANDLER_H__
#define __HEADER_GUARD__EXTKEYBOARDHANDLER_H__

#include <osgWidget/ViewerEventHandlers>
#include <set>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class ExtKeyboardHandler :
	public osgWidget::KeyboardHandler
{
public:
	ExtKeyboardHandler(osgWidget::WindowManager* wm);
	virtual ~ExtKeyboardHandler(void);

	virtual bool handle(
		const osgGA::GUIEventAdapter& gea,
		osgGA::GUIActionAdapter& gaa,
		osg::Object* obj,
		osg::NodeVisitor* nv
		);
};

}

#endif
