/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   14:46
	filename: VideoWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGET_H__
#define __HEADER_GUARD__VIDEOWIDGET_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <osg/ImageStream>
#include <osgViewer/Viewer>
#include <osgWidget/Widget>
#include <core/MultiView.h>
#include <osgui/QOsgWidgets.h>
#include <vidlib/PixelFormat.h>

#include <vidlib/osg/VideoImageStreamSizeOptimizer.h>
#include "ui_video.h"

class VideoWidget : public QWidget, public Ui::VideoWidget
{
    Q_OBJECT
public:
    //! Lista strumieni.
    typedef std::vector<osg::ref_ptr<osg::Image> > ImagesList;

private:
    //! Obrazki pogrupowane wg. po³o¿enia.
    ImagesList images;
    //! Multi view.
    osg::ref_ptr<core::MultiView> multiView;
    
    //! Lista optymatozatorów odpowiadaj¹cych za wybieranie optymalnego rozmiaru dekodowanego obrazka.
    std::vector< osg::ref_ptr<vidlib::VideoImageStreamSizeOptimizer> > optimizers;

    //! Shader odpowiedzialny za konwersjê yuv->rgb.
    osg::ref_ptr<osg::Shader> yuvTextureRectShader;
    //! Shader odpowiedzialny za konwersjê yuv->rgb.
    osg::ref_ptr<osg::Shader> yuvTexture2DShader;
    //! Czy ma byæ u¿ywane rozszerzenie texture_rectangle.
    bool useTextureRect;
    //! Format video.
    vidlib::PixelFormat format;
    //! Nazwa samplera w obu shaderach YUV.
    std::string yuvSamplerName;
    //! Nazwa zmiennej trzymaj¹cej rozmiar obrazka w obu shaderach YUV. W zasadzie
    //! u¿ywane tylko dla texture_rectangle
    std::string yuvImageSizeName;

public:
    //! Konstruktor inicjalizujacu UI.
    VideoWidget();

    virtual ~VideoWidget();

    void init( std::vector<osg::ref_ptr<vidlib::VideoImageStream> > &streams );
    void init( std::vector<std::string> &files );
    /*void init( std::vector<IVideoParserPtr> &parsers );*/

    
    void configureView(int rows, int columns, ImagesList & images);

public:

    void setPixelFormat(vidlib::PixelFormat format);
    void setUseTextureRect(bool useTextureRect);

    //! \return
    inline VideoWidget::ImagesList& getImages()
    { 
        return images;
    }

    //! \return
    osgui::QOsgDefaultWidget* getViewer() const
    { 
        return viewer;
    }

    //! \return
    const std::string& getYuvSamplerName() const
    { 
        return yuvSamplerName;
    }
    //! \param yuvSamplerName
    void setYuvSamplerName(const std::string& yuvSamplerName) 
    { 
        this->yuvSamplerName = yuvSamplerName; 
    }
    //! \return
    const std::string& getYuvImageSizeName() const
    { 
        return yuvImageSizeName;
    }
    //! \param yuvImageSizeName
    void setYuvImageSizeName(const std::string& yuvImageSizeName) 
    { 
        this->yuvImageSizeName = yuvImageSizeName; 
    }

    //! £aduje shadery.
    //! \param yuvTextureRect
    //! \param yuvTexture2D
    void loadShaders(const std::string& yuvTextureRect, const std::string& yuvTexture2D);
    //! Prze³adowuje shadery.
    void loadShaders();

public slots:
    //! Pokazuje menu kontekstowe.
    void onContextMenuRequested(QPoint position);
    //! Pokazuje menu kontekstowe.
    void onFormatYUVToggled(bool toggled);
    //! Pokazuje menu kontekstowe.
    void onFormatRGBToggled(bool toggled);
    //! Pokazuje menu kontekstowe.
    void onFormatBGRAToggled(bool toggled);
    //!
    void onTextureRectangleToggled(bool toggled);


private:

    //! Tworzy widget ze strumieniem.
    //! \param image
    osgWidget::Widget* createStreamWidget(osg::Image* image);
    //! Tworzy widget ze strumieniem.
    //! \param image
    osgWidget::Widget* createStreamWidget(osg::Image* image, vidlib::VideoImageStreamSizeOptimizer* optimizer );
    //! 
    //! \param image
    //! \param sampler
    //! \param imageSize
    osgWidget::Widget* createStreamWidget(osg::Image* image, osg::Uniform* sampler, osg::Uniform* imageSize );

    //! Tworzy scenê.
    void createScene();
    //! Czyœci scenê.
    void clearScene();
};

#endif  // __HEADER_GUARD__VIDEOWIDGET_H__