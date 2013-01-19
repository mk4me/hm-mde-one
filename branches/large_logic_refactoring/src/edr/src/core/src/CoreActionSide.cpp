#include "CorePCH.h"
#include <core/CoreActionSide.h>

CoreActionSide::CoreActionSide(SideType side) :side_(side)
{

}

CoreActionSide::~CoreActionSide()
{

}

const CoreActionSide::SideType CoreActionSide::side() const
{
	return side_;
}