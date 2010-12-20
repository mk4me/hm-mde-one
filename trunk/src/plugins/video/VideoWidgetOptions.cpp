#include "VideoPCH.h"
#include "VideoWidgetOptions.h"
#include <plugins/video/VideoService.h>
#include <plugins/video/core/PixelFormat.h>

VideoWidgetOptions::VideoWidgetOptions( VideoService* service ) : service(service)
{
    setupUi(this);
    outputFormatCombo->setItemData(0, qVariantFromValue(vm::PixelFormatYV12) );
    outputFormatCombo->setItemData(1, qVariantFromValue(vm::PixelFormatRGB24) );
    outputFormatCombo->setItemData(2, qVariantFromValue(vm::PixelFormatBGRA) );
}

void VideoWidgetOptions::outputFormatChanged( int value )
{
    service->setOutputFormat( outputFormatCombo->itemData(value).value<vm::PixelFormat>() );
}

void VideoWidgetOptions::textureRectangleChecked( int checked )
{
    service->setUseTextureRectangle(checked != 0);
}