/********************************************************************
	created:  2010/09/27
	created:  27:9:2010   19:50
	filename: VideoService.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOSERVICE_H__
#define __HEADER_GUARD__VIDEOSERVICE_H__

#include <core/IBaseService.h>

class IModel;
class IDataManager;
class IWidget;
class VideoWidget;

class VideoService : public IBaseService
{
  //M_DECLARE_CLASS();
  UNIQUE_ID('VIDE','SRVC');

private:
    // TODO
    // tymczasowe
    QWidget* widget;

public:
    VideoService();

    inline QWidget* getWidget() const
    { 
        return widget;
    }

    virtual AsyncResult OnAdded(IServiceManager* serviceManager);

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);
};


#endif  // __HEADER_GUARD__VIDEOSERVICE_H__