/********************************************************************
	created:  2010/09/27
	created:  27:9:2010   19:50
	filename: VideoService.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOSERVICE_H__
#define __HEADER_GUARD__VIDEOSERVICE_H__

#include <core/IService.h>

class IModel;
class IDataManager;
class IWidget;
class VideoWidget;

class VideoService : public IService
{
    UNIQUE_ID('VIDE','SRVC');

private:
    // TODO
    // tymczasowe
    QWidget* widget;
    //! Nazwa.
    std::string name;

public:
    VideoService();

    virtual IWidget* getWidget()
    { 
        // HACK: ca³y ten system jest shackowany!
        return reinterpret_cast<IWidget*>(widget);
    }

    virtual const std::string& getName() const
    {
        return name;
    }

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);
};


#endif  // __HEADER_GUARD__VIDEOSERVICE_H__