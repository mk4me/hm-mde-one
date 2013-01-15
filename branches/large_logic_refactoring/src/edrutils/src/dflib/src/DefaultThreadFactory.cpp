#include "DefaultThreadFactory.h"
#include "DefaultThread.h"

utils::IThread * DefaultThreadFactory::createThread(const Functor & func)
{
	return new DefaultThread(func);
}