#include <hanimlib/Displacer.h>

using namespace hAnim;

Displacer::Displacer()
	: Object(Object::Displacer), weight(-1)
{

}

Displacer::Displacer(const Displacer & other)
	: Object(other), coordIndex(other.coordIndex),
	displacements(other.displacements), weight(other.weight)
{

}

Displacer::~Displacer()
{

}