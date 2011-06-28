/********************************************************************
	created:  2010/10/25
	created:  25:10:2010   18:06
	filename: GRFService.h
	author:	  Michal Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__GRFSERVICE_H__
#define __HEADER_GUARD__GRFSERVICE_H__

#include <core/IService.h>
#include <plugins/chart/IChart.h>

class deprecated__ChartWidget;


class GRFService : public IChart, public core::IService
{
    UNIQUE_ID("{92A59449-78EC-40C9-9DE4-57FCAEA93E68}", "GRF Service");
private:
 //! Widget.
    deprecated__ChartWidget* widget;

    //!
    std::string name;
	double targetTime;
    double length;
public:
    GRFService();
    virtual ~GRFService();
	void setTargetTime(double time);
	double getTargetTime();
    double getLength() const;
    void setLength(double length);
	void setWidget(deprecated__ChartWidget* widget);

// IService
public:
    //!
    virtual void init(core::IServiceManager* serviceManager, core::IDataManager* dataManager);
    //!
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    //!
    virtual void update(double time, double timeDelta);
    //!
    virtual const std::string& getName() const;

		virtual void loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager);

    //!
    virtual osg::Node* debugGetLocalSceneRoot();

// IChart
public:

 

};


#endif  // __HEADER_GUARD__GRFService_H__