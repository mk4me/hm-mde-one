#include <hanimlib/Joint.h>

using namespace hAnim;

Joint::Joint()
	: Object(Object::Joint), center(0, 0, 0), limitOrientation(0, 0, 0, 1),
rotation(0, 0, 0, 1), scale(1, 1, 1), scaleOrientation(0, 0, 0, 1),
translation(0, 0, 0)
{
	llimit.fill(osg::Vec3(-osg::PI, -osg::PI, -osg::PI));
	ulimit.fill(osg::Vec3(osg::PI, osg::PI, osg::PI));
}

Joint::~Joint()
{

}