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

class ChartService : public core::IService
{
    UNIQUE_ID("{CDE9EC79-7AFE-4E1D-ABEC-DF76138C3BAE}", "Chart description");
private:
    std::vector<timeline::StreamPtr> streams;
    std::string name;

public:
    ChartService();

    virtual void finalize() {};
    virtual void loadData(core::IServiceManager* serviceManager, core::IDataManager* dataManager);
    virtual QWidget* getWidget(std::vector<QObject*>& actions);
    virtual const std::string& getName() const;
};

class EMGService : public IChart, public core::IService
{
    UNIQUE_ID("{CDE9EC79-7AFE-4E1D-ABEC-DF76138C3BAE}", "EMG Service");
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

    virtual void finalize() {};
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


#endif  // __HEADER_GUARD__EMGService_H__