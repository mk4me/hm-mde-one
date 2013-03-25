/********************************************************************
	created:	2013/03/01
	created:	1:3:2013   8:55
	filename: 	INodeConfiguration.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__INODECONFIGURATION_H__
#define HEADER_GUARD_NEWVDF__INODECONFIGURATION_H__

class QWidget;

namespace vdf {

    class INodeConfiguration
    {
    public:
	    virtual ~INodeConfiguration() {}
        virtual QWidget* getConfigurationWidget() = 0;
        virtual void refreshConfiguration() = 0;
    };
    typedef core::shared_ptr<INodeConfiguration> INodeConfigurationPtr;
    typedef core::shared_ptr<const INodeConfiguration> INodeConfigurationConstPtr;

    class INodeValidation
    {
    public:
        virtual ~INodeValidation() {}
        virtual bool isNodeValid() = 0;
        virtual QString getErrorMessage() = 0;
    };
    typedef core::shared_ptr<INodeValidation> INodeValidationPtr;
    typedef core::shared_ptr<const INodeValidation> INodeValidationConstPtr;

}

#endif
