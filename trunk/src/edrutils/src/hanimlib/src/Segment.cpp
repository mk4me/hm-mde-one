#include <hanimlib/Segment.h>

using namespace hAnim;

Segment::Segment()
	: Object(Object::Segment), bboxCenter(0, 0, 0), bboxSize(-1, -1, -1),
centerOfMass(0, 0, 0), mass(-1)
{
	momentsOfInertia.fill(0);
}

Segment::~Segment()
{

}