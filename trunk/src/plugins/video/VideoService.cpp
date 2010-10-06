#include "VideoPCH.h"
#include <core/IServiceManager.h>
#include <plugins/video/VideoService.h>
#include <core/IModel.h>
#include <core/IDataManager.h>
#include "VideoWidget.h"

//M_DECLARED_CLASS(VideoService, )

VideoService::VideoService()
{
    widget = new VideoWidget();
}

AsyncResult VideoService::OnTick( double delta )
{
  return AsyncResult_Complete;
}

AsyncResult VideoService::OnAdded( IServiceManager* serviceManager )
{
  return AsyncResult_Complete;
}

void VideoService::SetModel(IDataManager* dataManager )
{
    std::vector<std::string> files;
    for (int i = 0; i < dataManager->GetFilePathCount(); ++i) {
        files.push_back(dataManager->GetVideoFilePath(i));
    }
    widget->init(files);
    //widget->init()
}

