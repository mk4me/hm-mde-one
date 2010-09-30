#include "VideoPCH.h"
#include <core/IServiceManager.h>
#include <plugins/video/VideoService.h>
#include <core/IModel.h>
#include <core/IDataManager.h>

//M_DECLARED_CLASS(VideoService, )

AsyncResult VideoService::OnTick( double delta )
{
  return AsyncResult_Complete;
}

AsyncResult VideoService::OnAdded( IServiceManager* serviceManager )
{
  return AsyncResult_Complete;
}

void VideoService::SetModel( IModel* model, IDataManager* manager )
{

}

void VideoService::SetModel( IModel* model )
{

}