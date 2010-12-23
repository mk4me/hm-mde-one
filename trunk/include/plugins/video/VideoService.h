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
#include <plugins/video/core/PixelFormat.h>
#include <plugins/timeline/Stream.h>

class IModel;
class IDataManager;
class IWidget;
class VideoWidget;
class VideoWidgetOptions;

class VideoService : public IService
{
    UNIQUE_ID('VIDE','SRVC');

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
    typedef std::vector<timeline::StreamPtr> Streams;
    //! Lista strumieni.
    Streams streams;


public:
    //! \return Format u¿ywany przy rysowaniu.
    video::PixelFormat getOutputFormat();
    //! \param format Format u¿ywany przy rysowaniu.
    void setOutputFormat(video::PixelFormat format);
    //! \return Czy u¿ywa rozszerzenia TEXTURE_RECT?
    bool isUsingTextureRectangle();
    //! \param useTextureRectangle Czy u¿ywa rozszerzenia TEXTURE_RECT?
    void setUseTextureRectangle(bool useTextureRectangle);
    

public:
    VideoService();



    virtual IWidget* getWidget()
    { 
        // HACK: ca³y ten system jest shackowany!
        return reinterpret_cast<IWidget*>(widget);
    }

    virtual IWidget* getSettingsWidget()
    {
        return reinterpret_cast<IWidget*>(optionsWidget);
    }

    virtual const std::string& getName() const
    {
        return name;
    }

    virtual AsyncResult update(double time, double timeDelta);

    virtual AsyncResult loadData(IServiceManager* serviceManager, IDataManager* dataManager);

    //!
    virtual osg::Node* debugGetLocalSceneRoot();
};


#endif  // __HEADER_GUARD__VIDEOSERVICE_H__