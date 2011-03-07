#ifndef __HEADER_GUARD__OSG2DUTILS_H__
#define __HEADER_GUARD__OSG2DUTILS_H__

#include <osgWidget/Window>
#include <osgWidget/WindowManager>

class osgUI2DUtils{
public:
	static osgWidget::XYCoord findFreeSpaceInNearby(const osgWidget::XYCoord & pos,
		osgWidget::point_type width, osgWidget::point_type height, const osgWidget::WindowManager * wm);
	
	static bool collidesWithOthers(const osgWidget::Window * ww, const osgWidget::WindowManager * wm,
		const std::set<osgWidget::Window*> & exclude = std::set<osgWidget::Window*>());

	static bool collidesWithOthers(const osg::BoundingBox & bb, const osgWidget::WindowManager * wm,
		const std::set<osgWidget::Window*> & exclude = std::set<osgWidget::Window*>());

	static bool checkWindowsCollision(const osgWidget::Window * w1, const osgWidget::Window * w2);
	
	static osg::BoundingBox generateBox(const osgWidget::Window * ww);
	static osg::BoundingBox generateBox(const osgWidget::Widget * w);
	static osg::BoundingBox generateBox(const osgWidget::XYCoord & pos, osgWidget::point_type width,
		osgWidget::point_type height);

	static bool boxFullyEnclosedInBox(const osg::BoundingBox & bb1, const osg::BoundingBox & bb2);
	static osg::BoundingBox translateBoundingBox(const osg::BoundingBox & bb, const osgWidget::XYCoord & t);
	static osg::BoundingBox rotateBoundingBox(const osg::BoundingBox & bb, const osgWidget::XYCoord & rotCenter, float rotAngle);

	static osgWidget::point_type calcZ(const osgWidget::Window * window);
	static osgWidget::point_type calcZ(const osgWidget::Widget * widget);
};

#endif


