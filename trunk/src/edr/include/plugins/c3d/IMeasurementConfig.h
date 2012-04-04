/********************************************************************
	created:	2012/04/02
	created:	2:4:2012   11:03
	filename: 	IMeasurementConfig.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__IMEASUREMENTCONFIG_H__
#define HEADER_GUARD_C3DPLUGIN__IMEASUREMENTCONFIG_H__

class IMeasurementConfig
{
public:
	virtual ~IMeasurementConfig() {}

    virtual std::string tr(const std::string& text) const = 0;
    virtual bool hasEntry(const std::string& text) const = 0;
    virtual const std::string& getName() const  = 0;
    virtual int getNumber() const = 0;
};
typedef core::shared_ptr<IMeasurementConfig> IMeasurementConfigPtr;
typedef core::shared_ptr<const IMeasurementConfig> IMeasurementConfigConstPtr;


#endif
