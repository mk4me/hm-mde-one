#include <core/IServiceManager.h>
#include <plugins/video/VideoService.h>

//M_DECLARED_CLASS(VideoService, )

AsyncResult VideoService::OnTick( double delta )
{
  return AsyncResult_Complete;
}

AsyncResult VideoService::OnAdded( IServiceManager* serviceManager )
{
  return AsyncResult_Complete;
}