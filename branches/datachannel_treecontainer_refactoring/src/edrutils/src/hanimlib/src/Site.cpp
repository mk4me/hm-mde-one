#include <hanimlib/Site.h>

using namespace hAnim;

Site::Site()
	: Object(Object::Site), center(0, 0, 0), rotation(0, 0, 0, 1),
scale(1, 1, 1), scaleOrientation(0, 0, 0, 1),
translation(0, 0, 0)
{

}

Site::~Site()
{

}