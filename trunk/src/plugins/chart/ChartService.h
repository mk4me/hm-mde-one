/********************************************************************
	created:  2010/10/25
	created:  25:10:2010   18:06
	filename: ChartService.h
	author:	  Michal Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CHARTSERVICE_H__
#define __HEADER_GUARD__CHARTSERVICE_H__

#include <core/IService.h>
#include <plugins/chart/IChart.h>

class ChartWidget;


class ChartService : public IChart, public IService
{
    
private:
 //! Widget.
    ChartWidget* widget;

    //!
    std::string name;
	double targetTime;
    double length;
public:
    ChartService();
    virtual ~ChartService();
	void setTargetTime(double time);
	double getTargetTime();
    double getLength() const;
    void setLength(double length);

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

// IChart
public:

 

};


#endif  // __HEADER_GUARD__CHARTSERVICE_H__