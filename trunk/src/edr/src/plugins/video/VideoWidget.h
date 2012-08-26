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
    //! Obrazki pogrupowane wg. położenia.
    ImagesList images;
    //! Multi view.
    osg::ref_ptr<core::MultiView> multiView;
    
    //! Lista optymatozatorów odpowiadających za wybieranie optymalnego rozmiaru dekodowanego obrazka.
    std::vector< osg::ref_ptr<vidlib::VideoImageStreamSizeOptimizer> > optimizers;

    //! Shader odpowiedzialny za konwersję yuv->rgb.
    osg::ref_ptr<osg::Shader> yuvTextureRectShader;
    //! Shader odpowiedzialny za konwersję yuv->rgb.
    osg::ref_ptr<osg::Shader> yuvTexture2DShader;
    //! Czy ma być używane rozszerzenie texture_rectangle.
    bool useTextureRect;
    //! Format video.
    vidlib::PixelFormat format;
    //! Nazwa samplera w obu shaderach YUV.
    std::string yuvSamplerName;
    //! Nazwa zmiennej trzymającej rozmiar obrazka w obu shaderach YUV. W zasadzie
    //! używane tylko dla texture_rectangle
    std::string yuvImageSizeName;

public:
    //! Konstruktor inicjalizujący UI.
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

    //! Ładuje shadery.
    //! \param yuvTextureRect
    //! \param yuvTexture2D
    void loadShaders(const std::string& yuvTextureRect, const std::string& yuvTexture2D);
    //! Przeładowuje shadery.
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

    //! Tworzy scenę.
    void createScene();
    //! Czyści scenę.
    void clearScene();
};

#endif  // __HEADER_GUARD__VIDEOWIDGET_H__
