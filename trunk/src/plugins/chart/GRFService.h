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

class ChartWidget;


class GRFService : public IChart, public IService
{
    UNIQUE_ID('GRF','SRVC');
private:
 //! Widget.
    ChartWidget* widget;

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
	void setWidget(ChartWidget* widget);

// IService
public:
    //!
    virtual AsyncResult init(IServiceManager* serviceManager, osg::Node* sceneRoot);
    //!
    virtual IWidget* getWidget();
    //!
    virtual AsyncResult compute();
    //!
    virtual AsyncResult lateUpdate(double time, double timeDelta);
    //!
    virtual AsyncResult update(double time, double timeDelta);
    //!
    virtual const std::string& getName() const;

		virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);
// IChart
public:

 

};


#endif  // __HEADER_GUARD__GRFService_H__