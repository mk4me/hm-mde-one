/********************************************************************
	created:  2010/10/25
	created:  25:10:2010   18:06
	filename: EMGService.h
	author:	  Michal Szafarski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__EMGSERVICE_H__
#define __HEADER_GUARD__EMGSERVICE_H__

#include <core/IService.h>
#include <plugins/chart/IChart.h>
#include <plugins/timeline/Stream.h>
#include <plugins/timeline/ITimeline.h>

class deprecated__ChartWidget;

class ChartService : public IService
{
    UNIQUE_ID('CHRT', 'SRVC');
private:
    std::vector<timeline::StreamPtr> streams;
    std::string name;

public:
    ChartService();
    virtual AsyncResult loadData(IServiceManager* serviceManager, core::IDataManager* dataManager);
    virtual IWidget* getWidget();
    virtual const std::string& getName() const;
};

class EMGService : public IChart, public IService
{
    UNIQUE_ID('EMG','SRVC');
private:
 //! Widget.
    deprecated__ChartWidget* widget;

    //!
    std::string name;
	double targetTime;
    double length;
public:
    EMGService();
    virtual ~EMGService();
	void setTargetTime(double time);
	double getTargetTime();
    double getLength() const;
    void setLength(double length);
	void setWidget(deprecated__ChartWidget* widget);

// IService
public:
    //!
    virtual AsyncResult init(IServiceManager* serviceManager, core::IDataManager* dataManager, osg::Node* sceneRoot, osgViewer::CompositeViewer* viewer);
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
	
	virtual AsyncResult loadData(IServiceManager* serviceManager, core::IDataManager* dataManager);

    //!
    virtual osg::Node* debugGetLocalSceneRoot();

    virtual void visibilityChanged(IWidget* widget, bool visible);

// IChart
public:

 

};


#endif  // __HEADER_GUARD__EMGService_H__