/********************************************************************
	created:	2013/05/24
	created:	24:5:2013   13:00
	filename: 	FilterBundle.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__FILTERBUNDLE_H__
#define HEADER_GUARD_CORELIB__FILTERBUNDLE_H__

#include <vector>
#include <corelib/IFilterCommand.h>
#include <corelib/IFilterBundle.h>

namespace core {

class CORELIB_EXPORT FilterBundle : public IFilterBundle
{
public:
	virtual ~FilterBundle() {}

public:
    virtual void addFilterCommand(IFilterCommandPtr command);
    virtual IFilterCommandPtr getFilter( int idx );
    virtual int genNumFilters() const;

private:
    std::vector<IFilterCommandPtr> commands;
};
DEFINE_SMART_POINTERS(FilterBundle);
}
#endif
