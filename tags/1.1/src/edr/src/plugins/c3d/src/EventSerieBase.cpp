#include "C3DPCH.h"
#include <plugins/c3d/EventSerieBase.h>

C3DEventsCollection::Context EventSerieBase::getContext() const
{
	return context;
}

void EventSerieBase::setContext(C3DEventsCollection::Context val)
{
	context = val;
}