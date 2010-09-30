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

class VideoService : public IBaseService
{
  //M_DECLARE_CLASS();
  UNIQUE_ID('VIDE','SRVC');

public:
  virtual AsyncResult OnTick(double delta);
  virtual AsyncResult OnAdded(IServiceManager* serviceManager);

  virtual void SetModel(IModel* model, IDataManager* manager);
  virtual void SetModel(IModel* model);
};


#endif  // __HEADER_GUARD__VIDEOSERVICE_H__