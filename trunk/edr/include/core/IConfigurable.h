/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:10
	filename: 	IConfigurable.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ICONFIGURABLE_H__
#define HEADER_GUARD_CORE__ICONFIGURABLE_H__

class QWidget;

namespace core {

class IConfigurable
{
public:
    virtual ~IConfigurable() 
    {
    }
    virtual QWidget* getConfigurationWidget() = 0;
};

}
#endif