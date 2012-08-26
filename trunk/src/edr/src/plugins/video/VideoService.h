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
#include <vidlib/PixelFormat.h>
#include <osg/ref_ptr>
#include <osg/Image>

class QWidget;
class VideoWidget;
class VideoWidgetOptions;

class VideoService : public core::IService
{
    UNIQUE_ID("{B9564CD1-EA98-4D4C-A41D-9339B6F37673}", "Video Service");

private:
    // TODO
    // tymczasowe
    VideoWidget* widget;
    VideoWidgetOptions* optionsWidget;
    //! Nazwa.
    std::string name;

    //! Lista strumieni.
    typedef std::vector<osg::ref_ptr<osg::Image> > ImagesList;
    //! Lista strumieni.
    ImagesList imagesList;


    //! Lista strumieni.
//    typedef std::vector<timeline::StreamPtr> Streams;
    //! Lista strumieni.
    //Streams streams;


public:
    //! \return Format używany przy rysowaniu.
    vidlib::PixelFormat getOutputFormat();
    //! \param format Format używany przy rysowaniu.
    void setOutputFormat(vidlib::PixelFormat format);
    //! \return Czy używa rozszerzenia TEXTURE_RECT?
    bool isUsingTextureRectangle();
    //! \param useTextureRectangle Czy używa rozszerzenia TEXTURE_RECT?
    void setUseTextureRectangle(bool useTextureRectangle);


public:
    VideoService();

    virtual void finalize() {};

    virtual QWidget* getWidget(std::vector<QObject*>& actions)
    {
        return nullptr;
    }

    virtual QWidget* getSettingsWidget(std::vector<QObject*>& actions)
    {
        return nullptr;
    }

    virtual const std::string& getName() const
    {
        return name;
    }

    //!
    virtual osg::Node* debugGetLocalSceneRoot();
};


#endif  // __HEADER_GUARD__VIDEOSERVICE_H__
